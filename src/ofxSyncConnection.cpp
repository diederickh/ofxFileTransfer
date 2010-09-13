#include "ofxSyncConnection.h"
#include "ofMain.h"

ofxSyncConnection::ofxSyncConnection(boost::asio::io_service& rService) 
:socket_(rService) 
{
	transfer_man_.startThread();
}

ofxSyncConnection::~ofxSyncConnection() {
	cout << "~~~~ ofxSyncConnection()" << std::endl;
}

void ofxSyncConnection::start() {
	cout << "ofxSyncConnection.start()" << std::endl;
	transfer_addr_= socket_.remote_endpoint().address().to_string();
	
	async_read_until(
			socket_
			,request_
			,"\n\n"
			,boost::bind(
				&ofxSyncConnection::handleRequest
				,shared_from_this()
				,boost::asio::placeholders::error
				,boost::asio::placeholders::bytes_transferred
			)
	);
}

void ofxSyncConnection::handleRequest(
	 const boost::system::error_code& rErr
	,std::size_t nBytesTransferred
)
{
	if(rErr) {
		std::cout << "ERROR: ofxSyncConnection::handleRequest(): " << rErr.message() << std::endl;
		return;
	}
	
	uint32_t transfer_port = 0;
	std::istream request_stream(&request_);
	request_stream.read((char*)&data_size, sizeof(data_size));
	request_stream.read((char*)&transfer_port, sizeof(transfer_port));
	request_stream >> local_path_ >> remote_path_;
	request_stream.read(buffer_.c_array(), 2); // read "\n\n"
	std::cout << "\n-------------\n";
	std::cout << "Bytes transferred: "<< nBytesTransferred << std::endl;
	std::cout << "Local dir: "<< local_path_ << std::endl;
	std::cout << "Remote dir: " << remote_path_ << std::endl;
	std::cout << "Transfer port:" << transfer_port << std::endl;
	std::cout << "Data size: " << data_size << std::endl;
	std::cout << "request_.size(): " << request_.size() << std::endl;
	std::cout << "-------------\n";
	stringstream port_stream;
	port_stream << transfer_port;
	transfer_port_ = port_stream.str();

	// @todo - when the remote doesn't have one file at all it will return 0
	// how do we handle that?
//	return;
//	if(data_size <= 0)
//		return;
	
	// read the first chunk of data.
	size_t read = 0;
	while(request_stream) {
		char buf[1024];
		request_stream.read(&buf[0], 1024);
		remote_list.write(&buf[0], request_stream.gcount());
		read += request_stream.gcount();
		std::cout << "read: "<< read << " bytes" << std::endl;
	}
	bytes_to_read = data_size - read;
	size_t chunk_size = buffer_.size();
	std::cout << "BUFFER SIZE: "<< chunk_size << std::endl;
	if(bytes_to_read < chunk_size)
		chunk_size = bytes_to_read;
		
	std::cout << "ofxSyncConnection: start reading the rest of the buffer: " << bytes_to_read << " with chunk_size:" << chunk_size << std::endl;	
	// .. and read the rest.
	boost::asio::async_read(
		socket_
		,boost::asio::buffer(buffer_.c_array(), chunk_size)
		,boost::bind(
			&ofxSyncConnection::handleSyncData
			,shared_from_this()
			,boost::asio::placeholders::error
			,boost::asio::placeholders::bytes_transferred
		)
	);
}


void ofxSyncConnection::handleSyncData(
			const boost::system::error_code& rErr
			,std::size_t nBytesTransferred
)
{
	if(!rErr) {
		std::cout << "ofxSyncConnection.handleSycnData: got some data:" << nBytesTransferred << "\n";
		remote_list.write(buffer_.c_array(), nBytesTransferred);
		bytes_to_read -= nBytesTransferred;
		if(bytes_to_read > 0) {
			size_t to_read = 4096;
			if(bytes_to_read < to_read) {
				to_read = bytes_to_read;
			}

			boost::asio::async_read(
				socket_
				,boost::asio::buffer(buffer_.c_array(), to_read)
				,boost::bind(
					&ofxSyncConnection::handleSyncData
					,shared_from_this()
					,boost::asio::placeholders::error
					,boost::asio::placeholders::bytes_transferred
				)
			);
	
		}
		else {
			std::cout << "Compare remote and local list." << std::endl;
			cout << "RECEIVED: " << remote_list << std::endl;
			// parse the remote file list.
			bool result = false;
			std::vector<SyncInfo>remote_files;
			result = sync_list_.parseList(remote_list, local_path_, remote_files);
			if(!result) {
				std::cout << "Error: cannot parse remote list" << std::endl;
				return;
			}
				
			// get local list (which is remote of the client)
			std::stringstream local_list;
			std::vector<SyncInfo>local_files;
			sync_list_.getList(remote_path_, local_list);
			result = sync_list_.parseList(local_list, remote_path_, local_files);
			if(!result) {
				std::cout << "Error: cannot parse local list" << std::endl;
				return;
			}
			
			// + start tmp
			std::vector<SyncInfo>::iterator local_it = local_files.begin();
			std::vector<SyncInfo>::iterator remote_it = remote_files.begin();
			while(local_it != local_files.end()) {
				std::cout << "++ local: " << (*local_it).relative_file_name <<std::endl;
				++local_it;
			}
			std::cout << "==================" << std::endl;
			while(remote_it != remote_files.end()) {
				std::cout << "-- remote: " << (*remote_it).relative_file_name <<std::endl;
				++remote_it;
			}
			std::cout << std::endl;
			// - end tmp
			
			// get difference.
			std::vector<SyncInfo>files_to_sync;
			sync_list_.getDifference(local_files, remote_files, files_to_sync);
			if(files_to_sync.size() > 0) {
				// @todo -> sync with remote
				// @todo -> files with white space in name hang
				std::vector<SyncInfo>::iterator it = files_to_sync.begin();
				while(it != files_to_sync.end()) {
					std::string client_dest = (*it).file_name;
					boost::algorithm::replace_first(client_dest, remote_path_, local_path_);
					std::cout << "++ Difference: " << (*it).file_name << std::endl;
					/*
					std::cout	<< std::endl
								<< ">> fileserver: "	<< transfer_addr_	<< std::endl
								<< ">> port: "			<< transfer_port_	<< std::endl
								<< ">> local file: "	<< (*it).file_name  << std::endl
								<< ">> remote file: "	<< (*it).file_name  << std::endl
								<< ">> new_remote: "	<< client_dest		<< std::endl
								<< ">> rel_name: "		<< (*it).relative_file_name << std::endl
								<< "-----------------------" << std::endl
								<< std::endl;
					*/
					
					transfer_man_.transferFile(
						transfer_addr_
						,transfer_port_.c_str()
						,(*it).file_name
						,client_dest
					);
					
					
					++it;
					//break;
				}
			}
				
			std::cout << "READY parsin gbuffer" << std::endl;
			remote_list.str("");
			remote_list.seekg(0,ios::beg);
			remote_list.seekp(0,ios::beg);
			remote_list.clear();
			local_path_ = "reset";
			remote_path_ = "reset";
			std::cout << "Size of request_" << request_.size() << std::endl;
	
			//request_.clear();
			/*
			boost::array<char,4096>::iterator it = buffer_.begin();
			while(it != buffer_.end()) {
				cout << (*it) << ".";
				++it;
			}
			*/
			bytes_to_read = 0;
			data_size = 0;
			//request_
			// restart...
			start();
		}
		
	}
	else {
		std::cout << "Error: " << rErr.message() << std::endl;
	}
}

#include "ofxSyncConnection.h"
ofxSyncConnection::ofxSyncConnection(boost::asio::io_service& rService) 
:socket_(rService) 
{
}

void ofxSyncConnection::start() {
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
	std::istream request_stream(&request_);
	request_stream.read((char*)&data_size, sizeof(data_size));
	request_stream.read(buffer_.c_array(), 2); // read "\n\n"

	if(data_size <= 0)
		return;
	
	// read the first chunk of data.
	size_t read = 0;
	while(request_stream) {
		char buf[1024];
		request_stream.read(&buf[0], 1024);
		remote_list.write(&buf[0], request_stream.gcount());
		read += request_stream.gcount();
	}
	bytes_to_read = data_size - read;
	
	// .. and read the rest.
	boost::asio::async_read(
		socket_
		,boost::asio::buffer(buffer_.c_array(), buffer_.size())
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
			// parse the remote file list.
			uint32_t file_size;
			uint32_t name_size;
 
			while(remote_list) {
				char buf[4096];

				remote_list.read((char*)&file_size, sizeof(file_size));
				remote_list.read((char*)&name_size, sizeof(name_size));
				if(name_size > 4096) {
					std::cout << "Error: length of filename is incorrect: " << name_size << std::endl;
					break;
				}
				remote_list.read(&buf[0], name_size);
				buf[name_size] = '\0';
				std::string name(buf);
				std::cout << "file size: "<< file_size << " name-len: " << name_size << ", name: '" << name << "'" << std::endl;
			}
			std::cout << "READY parsin gbuffer" << std::endl;
			remote_list.str("");
		}
		
	}
	else {
		std::cout << "Error: " << rErr.message() << std::endl;
	}
}

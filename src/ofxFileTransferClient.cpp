#include "ofxFileTransferClient.h"
#include "ofMain.h"

using boost::asio::ip::tcp;

ofxFileTransferClient::ofxFileTransferClient(
					boost::asio::io_service &rIOService
					,const std::string sServer
					,const std::string nPort
					,const std::string sFilePath  
					,const std::string sRemoteFile
):resolver_(rIOService)
,socket_(rIOService)
,file_path_(sFilePath)
,remote_file_(sRemoteFile)
,server_(sServer)
,port_(nPort)
{
}

ofxFileTransferClient::~ofxFileTransferClient() {
	std::cout << "~~~~ ofxFileTransferClient" << std::endl;
}

void ofxFileTransferClient::start() {
	// open file / get size
	source_file_stream_.open(
					ofToDataPath(file_path_).c_str()
					,std::ios_base::binary | std::ios_base::ate
	);
	if(!source_file_stream_) {
		std::cout << ">> failed to open:" << file_path_ << std::endl;
		return;
	}
	
	size_t file_size = source_file_stream_.tellg();
	source_file_stream_.seekg(0);
	
	// send file size and name to server.
	std::ostream request_stream(&request_);

	request_stream	<< remote_file_ << "\n"
					<< file_size << "\n\n";
					
	std::cout	<< ">> request_size:" 	<< request_.size() 
				<< " file_path: " << file_path_
				<< " file_size: "<< file_size
				<< std::endl;
		
	// resolve ofxFileTransferServer
	tcp::resolver::query query(server_, port_);
	resolver_.async_resolve(
				query
				,boost::bind(
						&ofxFileTransferClient::handleResolve
						,shared_from_this()
						,boost::asio::placeholders::error
						,boost::asio::placeholders::iterator
				)
	);
	
}


void ofxFileTransferClient::handleResolve(
				const boost::system::error_code& rErr
				,tcp::resolver::iterator oEndPointIt
)
{
	if(!rErr) {
		tcp::endpoint endpoint = *oEndPointIt;
		socket_.async_connect(
				endpoint
				,boost::bind(
						&ofxFileTransferClient::handleConnect
						,shared_from_this()
						,boost::asio::placeholders::error
						,++oEndPointIt
				)
		);
	}
	else {
		std::cout << ">> error: " << rErr.message() << std::endl;
	}
	
}	

void ofxFileTransferClient::handleConnect(
				const boost::system::error_code& rErr
				,tcp::resolver::iterator oEndPointIt
)
{
	if(!rErr) {
		cout << ">> connected!" << std::endl;
		boost::asio::async_write(
				 socket_
				,request_
				,boost::bind(
						&ofxFileTransferClient::handleFileWrite
						,shared_from_this()
						,boost::asio::placeholders::error
				)
		);
	}
	else if (oEndPointIt != tcp::resolver::iterator()) {
		// connection failed, try next endpoint in list
		socket_.close();
		tcp::endpoint endpoint = *oEndPointIt;
		socket_.async_connect(
			endpoint
			,boost::bind(
				&ofxFileTransferClient::handleConnect
				,shared_from_this()
				,boost::asio::placeholders::error
				,++oEndPointIt
			)
		);
		
	}
	else {
		std::cout << ">> error: " << rErr.message() << std::endl;
	}
}

void ofxFileTransferClient::handleFileWrite(
				const boost::system::error_code& rErr
)
{
	if(!rErr) {
		if(source_file_stream_.eof() == false) {
			source_file_stream_.read(buf_.c_array(), buf_.size());
			if(source_file_stream_.gcount() <= 0) {
				std::cout << ">> read file error when trying to read: " << buf_.size() << " bytes " << std::endl;
				return;
			}
			std::cout << ">> send: " << source_file_stream_.gcount() << " bytes, total: " << source_file_stream_.tellg() << " bytes\n";
			boost::asio::async_write(
					socket_
					,boost::asio::buffer(buf_.c_array(), source_file_stream_.gcount())
					,boost::bind(
						&ofxFileTransferClient::handleFileWrite
						,shared_from_this()
						,boost::asio::placeholders::error
					)
			);
			
			if(rErr) {
				std::cout <<">> send error: " << rErr << std::endl; // not sure bout this one..
			}
		
		}
		else {
			source_file_stream_.close();
			return; // eof()
		}
	}
	else {
		std::cout << ">> error:" << rErr.message() << std::endl;
	}
}
	

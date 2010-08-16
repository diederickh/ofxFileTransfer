#include "ofxSyncClient.h"
#include "ofMain.h"

ofxSyncClient::ofxSyncClient(
	boost::asio::io_service& rIOService
	,const std::string sServer
	,const std::string nPort
):resolver_(rIOService)
,socket_(rIOService)
,server_(sServer)
,port_(nPort)
{
}

void ofxSyncClient::start() {
	tcp::resolver::query query(server_, port_);
	resolver_.async_resolve(
					query
					,boost::bind(	
							&ofxSyncClient::handleResolve
							,shared_from_this()
							,boost::asio::placeholders::error
							,boost::asio::placeholders::iterator
					)
	);
}
		
void ofxSyncClient::handleResolve(
		const boost::system::error_code& rErr
		,tcp::resolver::iterator oEndPoint
)
{
	if(!rErr) {
		tcp::endpoint endpoint = *oEndPoint;
		socket_.async_connect(
						endpoint
						,boost::bind(
							&ofxSyncClient::handleConnect
							,shared_from_this()
							,boost::asio::placeholders::error
							,++oEndPoint
						)
		);
	}
	else {
		std::cout << "Error: " << rErr.message() << std::endl;
	}
}


	
void ofxSyncClient::handleConnect(
		const boost::system::error_code& rErr
		,tcp::resolver::iterator oEndPoint
)
{
	if(!rErr) {
		// send data!
		std::iostream o(&request_);
		dir_list.getList(ofToDataPath("images"), o);
		
		uint32_t size = request_.size();
		std::iostream copy_buf(&size_buf_);
		copy_buf.write((char*)&size, sizeof(size));
		copy_buf.write("\n\n", 2);
	
		cout << "size: "<< request_.size() << std::endl;
		
		boost::asio::async_write(
					socket_
					,size_buf_
					,boost::bind(
						&ofxSyncClient::handleSize
						,shared_from_this()
						,boost::asio::placeholders::error
					)
		);
	}
	else if (oEndPoint != tcp::resolver::iterator()) {
		socket_.close();
		tcp::endpoint endpoint = *oEndPoint;
		socket_.async_connect(
						endpoint
						,boost::bind(
							&ofxSyncClient::handleConnect
							,shared_from_this()
							,boost::asio::placeholders::error
							,++oEndPoint
						)
		);
	}
	else {
		std::cout << "Error: " << rErr.message() << std::endl;
	}
}


void ofxSyncClient::handleSize(
		const boost::system::error_code& rErr
)
{
	if(!rErr) {
		
		boost::asio::async_write(
						socket_
						,request_
						,boost::bind(
							&ofxSyncClient::handleWrite
							,shared_from_this()
							,boost::asio::placeholders::error
							,boost::asio::placeholders::bytes_transferred
						)
		);
	}
	else {
		std::cout << "Error: " << rErr.message() << std::endl;
	}
}

	
	
void ofxSyncClient::handleWrite(
			const boost::system::error_code& rErr
			,size_t nBytesTransferred
)
{
	if(!rErr) {
		std::cout << "READY SENDING: " << nBytesTransferred << " bytes" << std::endl;	
	}
	else {
		std::cout << "Error: " << rErr.message() << std::endl;
	}
}



	
void ofxSyncClient::handleReady(
			const boost::system::error_code& rErr
)
{
	if(!rErr) {
		std::cout << "READY SENDING!!" << std::endl;
	}
	else {
		std::cout << "Error: " << rErr.message() << std::endl;
	}
}
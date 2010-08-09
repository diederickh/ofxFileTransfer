#include "ofxFileTransfer.h"
#include <iostream>

namespace fs = boost::filesystem;

// Create a new ofxFileTransfer instance on the given port. This will accept
// incomming connections and handles file storage and such.
//------------------------------------------------------------------------------
ofxFileTransfer::ofxFileTransfer(const char* nPort)
	:port(nPort)
	,acceptor_(
		io_service
		,tcp::endpoint(tcp::v4()
		, atoi(port)
	)
)
{
}


// Call this once to start accepting incoming connections, this is necessary
// for the server part.
//------------------------------------------------------------------------------
void ofxFileTransfer::accept() {
	startThread(false, true);
}


// Start accepting clients.
//------------------------------------------------------------------------------
void ofxFileTransfer::receive() {
	ofxFileTransferConnection::pointer new_connection = 
			ofxFileTransferConnection::create(acceptor_.io_service());
	connections.push_back(new_connection);	
	acceptor_.async_accept(
					new_connection->socket()
					,boost::bind(
							&ofxFileTransfer::handleAccept
							,this
							,new_connection
							,boost::asio::placeholders::error
					)
	);
}

// Receive a new connection/client.
//------------------------------------------------------------------------------
void ofxFileTransfer::handleAccept(
			ofxFileTransferConnection::pointer pConnection
			,const boost::system::error_code &rError
)
{
	if(!rError) {
		std::cout << "ofxFileTransfer::handleAccept" << std::endl;
		pConnection->start();
		receive();
	}
}

// Transfer a file! (this can be called by a client.)
//------------------------------------------------------------------------------
void ofxFileTransfer::transferFile(
	const char* sHost
	,const char* nPort
	,string sFile
)
{
	ofxFileTransferSend* new_send = new ofxFileTransferSend(
		io_service
		,sHost
		,nPort
		,sFile
		,this
	);
	// must copy pointer or io_service will stop using it
	file_connections.push_back(new_send); 
}


// This is called by the "client" when the transfer is done. We remove/delete
// the instance of ofxFileTransferSend here!
//------------------------------------------------------------------------------
void ofxFileTransfer::onFileTransferReady(ofxFileTransferSend* pFileTransfer) {
	std::cout << "YEEEE" << std::endl;
}


// Utility function: get file size
//------------------------------------------------------------------------------
uint64_t ofxFileTransfer::getFileSize(string sFilePath) {
	fs::path p(sFilePath, fs::native);
	if(!fs::exists(p)) {
		return 0;
	}
	if(!fs::is_regular(p)) {
		return 0;
	}
	return fs::file_size(p);
}


// The ofxFileTransfer server runs in it's own thread (started by "accept()").
//------------------------------------------------------------------------------
void ofxFileTransfer::threadedFunction() {
	receive();
	io_service.run();
}
						

#ifndef OFXFILETRANSFERH
#define OFXFILETRANSFERH

#undef check // necessary to get Boost running on Mac
 
#include <vector> 
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/filesystem/operations.hpp>
	
#include "ofxThread.h"
#include "ofxFileTransferConnection.h"
#include "ofxFileTransferSend.h"

using boost::asio::ip::tcp;
class ofxFileTransferSend;
class ofxFileTransfer : public ofxThread {
public:
	// Create a new FileTransfer instance on port/ip
	ofxFileTransfer(const char* nPort);
	
	// Start listening and receiving files, this starts the thread.
	void accept();
	
	// Start receiving files.
	void receive();
	
	// Handle incoming connections.
	void handleAccept(
		ofxFileTransferConnection::pointer pConnection
		,const boost::system::error_code &rError
	);
	
	// Our thread function.
	virtual void threadedFunction();
	
	// Transfer a file to the remove server.
	void transferFile(
		 const char* sHost
		,const char* nPort 
		,string sFile
	);
	
	// Called when the file transfer is reader, cleans up things.
	void onFileTransferReady(
		ofxFileTransferSend* pFileTransfer
	);
	

	// Utility function: get file size
	static uint64_t getFileSize(string sFilePath);


	
	boost::asio::io_service io_service;
		
private:

	const char* port;
	tcp::acceptor acceptor_;
	vector<ofxFileTransferConnection::pointer> connections;
	vector<ofxFileTransferSend*> file_connections;
};
#endif
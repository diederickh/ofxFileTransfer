#ifndef OFXFILETRANSFERSERVERH
#define OFXFILETRANSFERSERVERH

#undef check
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <iostream>

#include "ofxFileTransferConnection.h"
//#include "ofxFileTransferClient.h"

class ofxFileTransferServer {
public:
		
	ofxFileTransferServer(
				unsigned short nPort
	);
	
	void startThread();
	
	void accept();	
	
	void handleAccept(
				ofxFileTransferConnection::pointer pConnection
				,const boost::system::error_code& rErr
	);

		
private:

	unsigned short port;
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::acceptor acceptor;
};
#endif
#ifndef OFXFILETRANSFERMANAGERH
#define OFXFILETRANSFERMANAGERH

#undef check
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <string>
#include <iostream>
#include <deque>

#include "ofxFileTransferConnection.h"
#include "ofxFileTransferClient.h"

class ofxFileTransferManager {
public:
	ofxFileTransferManager();
	void transferFile(
				 const std::string sServer
				,const std::string nPort
				,const std::string sFile
				,const std::string sRemoteFile
	);
	void startThread();
	
private:
	deque<ofxFileTransferClient::pointer>transfers;
	void run();
	boost::asio::io_service io_service_;
	boost::asio::io_service::work work_;
};

#endif
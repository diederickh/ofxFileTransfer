#ifndef OFXFILETRANSFERSERVERH
#define OFXFILETRANSFERSERVERH

#undef check
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>
#include <iostream>

#include "ofxFileTransferConnection.h"

/**
 * Instantiate this class like:
 * - boost::shared_ptr<ofxFileTransferServer> file_server_(new ofxFileTransferServer(....));
 * see the ofxSyncClient for an example. You need to initialize this way because
 * else we got mutex errors in io_service/boost. 
 *
 */
class ofxFileTransferServer : public boost::enable_shared_from_this<ofxFileTransferServer>{
public:
		
	ofxFileTransferServer(
				unsigned short nPort
	);
	~ofxFileTransferServer();
	void shutdown();
	void startThread();
	
	void accept();	
	
	void handleAccept(
				ofxFileTransferConnection::pointer pConnection
				,const boost::system::error_code& rErr
	);

		
private:
	boost::shared_ptr<boost::thread> thread_ptr;
	unsigned short port;
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::acceptor acceptor;
};
#endif
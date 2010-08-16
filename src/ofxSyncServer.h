#ifndef SYNCSERVERH
#define SYNCSERVERH
#undef check
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "ofxSyncConnection.h"
using boost::asio::ip::tcp;

class ofxSyncServer {
public:
	ofxSyncServer(unsigned short nPort);
	void start();
	void handleAccept(ofxSyncConnection::pointer pCon, const boost::system::error_code &rErr);
private:
	boost::asio::io_service io_service_;
	tcp::acceptor acceptor_;
};
#endif
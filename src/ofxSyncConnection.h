#ifndef OFXSYNCCONNECTIONH
#define OFXSYNCCONNECTIONH
#undef check
#include <iostream>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>

#include <sstream>
#include <vector>
#include "ofxSyncDirList.h"

using boost::asio::ip::tcp;

class ofxSyncConnection : public boost::enable_shared_from_this<ofxSyncConnection> {
public:

	ofxSyncConnection(boost::asio::io_service& rService);
	void start();
	void handleRequest(const boost::system::error_code& rErr, std::size_t nBytesTransferred);
	void handleSyncData(
				const boost::system::error_code& rErr
				,std::size_t nBytesTransferred
	);
	tcp::socket& socket() { return socket_; 	}
	
	typedef boost::shared_ptr<ofxSyncConnection> pointer;

private:
	std::stringstream remote_list;
	tcp::socket socket_;
	boost::asio::streambuf request_;
	boost::array<char, 4096> buffer_;
	uint32_t data_size;
	uint32_t bytes_to_read;
	ofxSyncDirList sync_list_;
};
#endif
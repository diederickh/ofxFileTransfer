#ifndef OFXSYNCCLIENTH
#define OFXSYNCCLIENTH

#undef check 
#include <string>
#include <iostream>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>

#include "ofxSyncDirList.h"
#include "ofxFileTransferServer.h"

using boost::asio::ip::tcp;

class ofxSyncClient : public boost::enable_shared_from_this<ofxSyncClient> {
public:
	typedef boost::shared_ptr<ofxSyncClient> pointer;
	
	ofxSyncClient(
		boost::asio::io_service& rIOService
		,const std::string sSyncServerIP
		,const std::string nSyncServerPort
		,const std::string nTransferServerPort
	);
	
	void start();
		
private:
	void handleResolve(
		const boost::system::error_code& rErr
		,tcp::resolver::iterator oEndPoint
	);
	
	void handleConnect(
		const boost::system::error_code& rErr
		,tcp::resolver::iterator oEndPoint
	);
	
	
	void handleSize(
		const boost::system::error_code& rErr
	);
	
	void handleWrite(
		const boost::system::error_code& rErr
		,size_t nBytesTransferred
	);
	
	void handleReady(
		const boost::system::error_code& rErr
	);
	

	//boost::asio::io_service_ io_service;
	tcp::socket socket_;
	tcp::resolver resolver_;
	boost::array<char, 1024>buf_;
	boost::asio::streambuf request_;
	boost::asio::streambuf size_buf_;
	std::string sync_server_ip_;
	std::string sync_server_port_;
	std::string transfer_server_ip_;
	std::string transfer_server_port_;

	ofxSyncDirList dir_list_;
	ofxFileTransferServer file_server_;
};
#endif
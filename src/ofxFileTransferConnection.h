#ifndef OFXFILETRANSFERCONNECTIONH
#define OFXFILETRANSFERCONNECTIONH


#undef check
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <string>

#include "ofMain.h"
#include "ofxFileTransferManager.h"

class ofxFileTransferConnection : public boost::enable_shared_from_this<ofxFileTransferConnection> {
public:
	typedef boost::shared_ptr<ofxFileTransferConnection> pointer;
	ofxFileTransferConnection(
			boost::asio::io_service& rIOService
		
	);
	~ofxFileTransferConnection();
	void start();
	
	boost::asio::ip::tcp::socket& socket() {	
		return socket_;
	}
private:
	void handleReadRequest(
				const boost::system::error_code& rErr
				,std::size_t nBytesTransferred
	);
	
	void handleFileContent(
				const boost::system::error_code& rErr
				,std::size_t nBytesTransferred
	);
	
	void handleError(
				 const std::string& rFunction
				,const boost::system::error_code& rErr
	);
	
	
	std::string file_path_;
	std::size_t file_size_;
	std::ofstream out_file_stream_;
	boost::asio::ip::tcp::socket socket_;
	boost::asio::streambuf request_buf_;
	boost::array<char, 40960> buffer_;
};

#endif
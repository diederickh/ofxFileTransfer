#ifndef OFXFILETRANSFERCONNECTIONH
#define OFXFILETRANSFERCONNECTIONH

#undef check // necessary to get Boost running on Mac

#include <istream>
#include <ostream>
#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "ofMain.h"
#include "ofxFileTransferConnection.h"

using boost::asio::ip::tcp;

class ofxFileTransferConnection : public boost::enable_shared_from_this<ofxFileTransferConnection> {
public:
	typedef boost::shared_ptr<ofxFileTransferConnection> pointer;
	static pointer create(boost::asio::io_service& rIOService);
		
	tcp::socket& socket();
	void start();

private:	
	ofxFileTransferConnection(
				boost::asio::io_service& rIOService
	);	
	
	void handleFileName(
				const boost::system::error_code& rError
	);
	
	void handleFileSize(
				const boost::system::error_code& rError
	);
	
	void handleFileChunk(
				const boost::system::error_code& rError
				,size_t nBytesTransferred
	);
	
	uint64_t				file_size;
	uint64_t				bytes_left;
	std::string				file_name;
	ofstream				ofile_stream;
	tcp::socket				socket_;
	boost::asio::streambuf	response;
	boost::array<char, 8192> buffer; 
	
};

#endif
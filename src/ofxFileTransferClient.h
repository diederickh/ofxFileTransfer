#ifndef OFXFILETRANSFERCLIENTH
#define OFXFILETRANSFERCLIENTH

#undef check
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>


using boost::asio::ip::tcp;

class ofxFileTransferClient : public boost::enable_shared_from_this<ofxFileTransferClient> {
public:
	typedef boost::shared_ptr<ofxFileTransferClient> pointer;

	ofxFileTransferClient(
					boost::asio::io_service &rIOService
					,const std::string sServer
					,const std::string nPort
					,const std::string sFilePath
					,const std::string sRemoteFile
	);
	~ofxFileTransferClient();
	void start();

private:
	void handleResolve(
					const boost::system::error_code& rErr
					,tcp::resolver::iterator oEndPointIt
	);
	
	void handleConnect(
					const boost::system::error_code& rErr
					,tcp::resolver::iterator oEndPointIt
	);	
	
	void handleFileWrite(
					const boost::system::error_code& rErr
	);
	
	tcp::resolver resolver_;
	tcp::socket socket_;
	boost::array<char,1024>	buf_;
	boost::asio::streambuf request_;
	std::ifstream source_file_stream_;
	std::string file_path_;
	std::string server_;
	std::string port_;
	std::string remote_file_;
};


#endif


/*
pointer create(
					boost::asio::io_service& rIOService
					,const std::string sServer
					,const unsigned short nPort
	);

*/
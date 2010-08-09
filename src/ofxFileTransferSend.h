#ifndef OFXFILETRANSFERSENDH
#define OFXFILETRANSFERSENDH

#undef check // necessary to get Boost running on Mac

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "ofxFileTransfer.h"

using boost::asio::ip::tcp;

class ofxFileTransfer;
class ofxFileTransferIncrementalTransfer;

class ofxFileTransferSend : public boost::enable_shared_from_this<ofxFileTransferSend> {
public:
	ofxFileTransfer* transfer;
	ofxFileTransferSend(
					boost::asio::io_service &rIOService
					,const char* sServer
					,const char* sPort
					,string sFile
					,ofxFileTransfer* pTransfer
	);
	
private:
	// Executed when we resolved the end point.
	void handleResolve(
		const boost::system::error_code &rError
		,tcp::resolver::iterator oEndPointIterator
	);
	
	// Executed when we're connected to the end point.	
	void handleConnect(
		const boost::system::error_code &rError
		,tcp::resolver::iterator rOEndPointIterator
	);
	
	
	// Executed when we need to send file name and size
	void handleSendFileInfo(
		const boost::system::error_code &rError
	);
	
		
	ifstream*				out_stream;
	tcp::resolver			resolver_;
	tcp::socket				socket_;
	const char*				port;
	string					file;
	boost::asio::streambuf	send_data;
	std::ifstream*			file_stream;
	ofxFileTransferIncrementalTransfer*	sender;
};
#endif

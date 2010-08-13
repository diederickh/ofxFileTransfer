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
	~ofxFileTransferSend();
	void onEnd();
	void start();
	void onFileTransferReady(ofxFileTransferIncrementalTransfer* pIncSender);
	void onFileTransferReady();
	
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
	
	void handleCommand(
		const boost::system::error_code &rError
	);
	void reset();
	void sendFileInfo();
		void onTimeout();
		
	void startSendingFileChunks();
	void sendFileChunk();
	void handleFileChunkSend(const boost::system::error_code &rError, size_t nBytesSend);
		
//	ifstream*				out_stream;
	tcp::resolver			resolver_;
	tcp::socket				socket_;
	const char*				port;
	const char*				server;
	string					file;
	boost::asio::streambuf	receive_data;
	boost::asio::streambuf	send_data;
	std::ifstream*			file_stream;
	ofxFileTransferIncrementalTransfer*	sender;

	// implementing the send operations in this class:
	//ofxFileTransferSend*	transfer_sender;
	//tcp::socket&			sock;
	//ifstream*				source;
	
	boost::array<char, 1048576> send_array; 
	boost::asio::streambuf	send_buffer;
	bool send_ready;
	boost::asio::deadline_timer timeout;
	
	//std::vector<char>		buf;
	size_t				chunk_size;
	size_t				bytes_send;
	size_t				bytes_left;
	size_t				file_size;

};
#endif

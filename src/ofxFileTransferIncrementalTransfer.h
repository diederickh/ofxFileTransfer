#ifndef OFXFILETRANSFERINCREMENTALTRANSFERH
#define OFXFILETRANSFERINCREMENTALTRANSFERH

#undef check // necessary to get Boost running on Mac

#include <fstream> 
#include <vector> 
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "ofxFileTransfer.h"
#include "ofxFileTransferConnection.h"

class ofxFileTransferSend;

using boost::asio::ip::tcp;

class ofxFileTransferIncrementalTransfer {
public:
	ofxFileTransferIncrementalTransfer(
				tcp::socket& rSock
				,ofxFileTransferSend* pTransferSender
	);
	void go();
	void sendChunk();
	void onChunkComplete(const boost::system::error_code& rError);
	
	ofxFileTransferSend*	transfer_sender;
	tcp::socket&			sock;
	ifstream*				source;
	std::vector<char>		buf;
	uint64_t				chunk_size;
	uint64_t				bytes_send;
	
private:
	void onEnd();
};
#endif
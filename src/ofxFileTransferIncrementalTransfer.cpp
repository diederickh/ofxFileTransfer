#include "ofxFileTransferIncrementalTransfer.h"
#include "ofxFileTransferSend.h"

using boost::asio::ip::tcp;

// C'tor
//------------------------------------------------------------------------------
ofxFileTransferIncrementalTransfer::ofxFileTransferIncrementalTransfer(
		tcp::socket& rSock
		,ofxFileTransferSend* pTransferSender
	):sock(rSock)
	,transfer_sender(pTransferSender)
	,bytes_send(0)
{
}

// Start transfering data to the server
//------------------------------------------------------------------------------	
void ofxFileTransferIncrementalTransfer::go() {
	sendChunk();
}
	
// Send a chunk of bytes to the server.
//------------------------------------------------------------------------------
void ofxFileTransferIncrementalTransfer::sendChunk() {
	if(*source) {
	
		buf.resize(chunk_size);
		source->read(&buf[0], buf.size());
		std::streamsize N = source->gcount();
		buf.resize(N);
		bytes_send += N;
	//	cout << ">>> send bytes: " << bytes_send << std::endl;
		boost::asio::async_write(
						sock
						,boost::asio::buffer(buf)
						,boost::bind(
								&ofxFileTransferIncrementalTransfer::onChunkComplete
								,this
								,boost::asio::placeholders::error
						)
		);
	}
	else {
		onEnd();
	}
}


// Make sure we're removed from memory by our parent.
//------------------------------------------------------------------------------
void ofxFileTransferIncrementalTransfer::onEnd() {
	transfer_sender->transfer->onFileTransferReady(transfer_sender);
}

// Send chunk of data...
//------------------------------------------------------------------------------
void ofxFileTransferIncrementalTransfer::onChunkComplete(const boost::system::error_code& rError) {
	if(!rError) 
		sendChunk();
	else {
		std::cout << "ERROR in onChunkComplete." << std::endl;
	}
}

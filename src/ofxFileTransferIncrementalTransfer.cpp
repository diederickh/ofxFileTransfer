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

ofxFileTransferIncrementalTransfer::~ofxFileTransferIncrementalTransfer() {
	std::cout << "~~~~ ofxFileTransferIncrementalTransfer()" << std::endl;
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
		
		uint64_t total = 3109650;
		uint64_t left = total - bytes_send;
		//if(left < 8096)
		cout << ">>> send bytes: " << bytes_send << " left: " << left << std::endl;
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
	/*
		boost::asio::async_read_until(	
							socket_
							,response
							,"\r\n"
							,boost::bind(
								&ofxFileTransferIncrementalTransfer::onHandleSizeCheck
								,this
								,boost::asio::placeholders::error
							)	
		);
		*/
		onEnd();
	}
}

void ofxFileTransferIncrementalTransfer::onHandleSizeCheck(const boost::system::error_code& rError) {
	if(!rError) {
		cout << "Okay get size to check" << std::endl;
	}
}


// Make sure we're removed from memory by our parent.
//------------------------------------------------------------------------------
void ofxFileTransferIncrementalTransfer::onEnd() {
	source->close();
	transfer_sender->onFileTransferReady(this);
	//transfer_sender->transfer->onFileTransferReady(transfer_sender);
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

#include "ofxFileTransferIncrementalTransfer.h"
#include "ofxFileTransferSend.h"
#include "ofxFileTransfer.h"
#include <istream>	
#include <boost/date_time/posix_time/posix_time.hpp>
	
// Here, we resolve the server.
//---------------------------------------------------
ofxFileTransferSend::ofxFileTransferSend(
					boost::asio::io_service &rIOService
					,const char* sServer
					,const char* sPort
					,string sFile
					,ofxFileTransfer* pTransfer
)	:port(sPort)
	,server(sServer)
	,socket_(rIOService)
	,timeout(rIOService)
	,resolver_(rIOService)
	,transfer(pTransfer)
	,file(sFile)
	,send_ready(false)
{
}

ofxFileTransferSend::~ofxFileTransferSend() {
	delete file_stream;
	std::cout << "~~~~ ofxFileTransferSend()" << std::endl;
}

void ofxFileTransferSend::start() {
	tcp::resolver::query query(server, port);
	resolver_.async_resolve(
						query
						,boost::bind(
								&ofxFileTransferSend::handleResolve
								,this
								,boost::asio::placeholders::error
								,boost::asio::placeholders::iterator
						)
	);
}

// After resolving the server, we connect to it.	
//---------------------------------------------------
void ofxFileTransferSend::handleResolve(
	 const boost::system::error_code &rError
	,tcp::resolver::iterator oEndPointIterator
)
{

	if (!rError) {
		tcp::endpoint end_point = *oEndPointIterator;
		socket_.async_connect(
						end_point
						,boost::bind(
							&ofxFileTransferSend::handleConnect
							,this
							,boost::asio::placeholders::error
							,++oEndPointIterator
						)
		);
	}
	else {
		std::cout << ">> ERROR: while resolving server: " << std::endl;
	}
}

// After connecting, we send the file size and name.
//---------------------------------------------------
void ofxFileTransferSend::handleConnect(
	const boost::system::error_code &rError
	,tcp::resolver::iterator oEndPointIterator
)
{
	if(!rError) {
		sendFileInfo();
	}
	else {
		// @todo on failure retry!
		std::cout	<< ">> ERROR: connecting to ofxFileTransferServer:"
					<< rError.message()
					<< std::endl;
		std:cout << ">> BUT WE DO SEND THE FILE INFO!!!!" << std::endl;
	}
}
void ofxFileTransferSend::sendFileInfo() {
		boost::asio::async_read_until(
						socket_
						,receive_data
						,"\r\n"
						,boost::bind(
							&ofxFileTransferSend::handleCommand
							,this
							,boost::asio::placeholders::error
						)
		);
		file_size = transfer->getFileSize(file);
		bytes_left = file_size;
		std::ostream send_data_stream(&send_data);
		send_data_stream << file << "\r\n";
		send_data_stream << file_size << "\r\n";
		
		boost::asio::async_write(
						socket_
						,send_data
						,boost::bind(
							&ofxFileTransferSend::handleSendFileInfo
							,this
							,boost::asio::placeholders::error
						)
		);

}

void ofxFileTransferSend::handleCommand(const boost::system::error_code &rError) {
	if(!rError) {
		istream command_stream(&receive_data);
		std::string command;
		command_stream >> command >> std::ws;
		if(command == "retry") {
			reset();
		}
		else if (command == "ok") {
			onFileTransferReady();
		}

	}
	else {
		cout << ">> Error handling command in ofxFiletransferSend: " << rError.message() << std::endl; 
	}
}

void ofxFileTransferSend::reset() {
	file_size = transfer->getFileSize(file);
	bytes_left = file_size;
	bytes_send = 0;
	send_ready = false;
	sendFileInfo();
}

void ofxFileTransferSend::startSendingFileChunks() {
	string file_path = ofToDataPath(file);
	file_stream  = new std::ifstream(file_path.c_str(), std::ios::binary);
	chunk_size = 64;
	
	timeout.expires_from_now(boost::posix_time::seconds(4));
	timeout.async_wait(boost::bind(&ofxFileTransferSend::onTimeout,this));
	
	sendFileChunk();
}

void ofxFileTransferSend::onTimeout() {
	if(!send_ready && bytes_send != 0) {
		reset();
	}
}

void ofxFileTransferSend::sendFileChunk() {
	size_t num_bytes = chunk_size;
	if(bytes_left < chunk_size)
		num_bytes = bytes_left;
	if(num_bytes == 0) {
		send_ready = true;
		return;
	}

	// read some data from the filestream and put it in the buffer.
	if(file_stream->read(&send_array[0], num_bytes)) {
		std::streamsize actually_read = file_stream->gcount();
		//if(actually_read != num_bytes)
		// @todo handle ...
		
	}
	else {
		 cout << "Error while reading from file stream!!!!!!!!!!" << std::endl;
	}

	boost::asio::async_write(
					socket_
					,boost::asio::buffer(send_array,num_bytes)
					,boost::bind(
						&ofxFileTransferSend::handleFileChunkSend
						,this
						,boost::asio::placeholders::error
						,boost::asio::placeholders::bytes_transferred
					)
	);
}
void ofxFileTransferSend::handleFileChunkSend(
	const boost::system::error_code &rError
	,size_t nBytesSend
)
{
	if(!rError) {
		bytes_send += nBytesSend;
		bytes_left -= nBytesSend;
		sendFileChunk();
	}
	else {
		cout << "Error handling file chunk: " << rError.message() << std::endl;
	}
}

void ofxFileTransferSend::onFileTransferReady() {
	transfer->onFileTransferSendReady(this);
}


// After sending the file data, we start an incremental
// transfer and send all the data to the server.
//---------------------------------------------------
void ofxFileTransferSend::handleSendFileInfo(
	const boost::system::error_code &rError
)
{
	if(!rError) {
		cout << ">> handleSendFileInfo, start sending chunks!" << std::endl;
		startSendingFileChunks();
	}
	else {
		std::cout << ">> ERROR: sending file info: " << rError.message() << std::endl;
	}
}	


void ofxFileTransferSend::onFileTransferReady(ofxFileTransferIncrementalTransfer* pIncSender) {}

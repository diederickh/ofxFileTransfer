#include "ofxFileTransferIncrementalTransfer.h"
#include "ofxFileTransferSend.h"
#include "ofxFileTransfer.h"
	
	
// Here, we resolve the server.
//---------------------------------------------------
ofxFileTransferSend::ofxFileTransferSend(
					boost::asio::io_service &rIOService
					,const char* sServer
					,const char* sPort
					,string sFile
					,ofxFileTransfer* pTransfer
)	:port(sPort)
	,socket_(rIOService)
	,resolver_(rIOService)
	,transfer(pTransfer)
	,file(sFile)
{
	tcp::resolver::query query(sServer, sPort);
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
		std::cout << "ERROR: while resolving server: " << std::endl;
	}
}

// After connecting, we send the file size and name.
//---------------------------------------------------
void ofxFileTransferSend::handleConnect(
	const boost::system::error_code &rError
	,tcp::resolver::iterator rOEndPointIterator
)
{
	if(!rError) {
		// @todo get file size
		uint64_t s = ofxFileTransfer::getFileSize(file);
		cout << "SSSSS: " << s <<std::endl;
		cout << "FILE: " << file << std::endl;
		uint64_t size = transfer->getFileSize(file);
		std::ostream send_data_stream(&send_data);
		send_data_stream << file << "\r\n";
		send_data_stream << size << "\r\n";
		
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
	else {
		// @todo on failure retry!
		std::cout	<< "ERROR: connecting to ofxFileTransferServer:"
					<< rError.message()
					<< std::endl;
	}
}

// After sending the file data, we start an incremental
// transfer and send all the data to the server.
//---------------------------------------------------
void ofxFileTransferSend::handleSendFileInfo(
	const boost::system::error_code &rError
)
{
	if(!rError) {
		string file_path = ofToDataPath(file);
		sender = new ofxFileTransferIncrementalTransfer(socket_, this);
		file_stream  = new std::ifstream(file_path.c_str(), std::ios::binary);
		sender->source = file_stream;
		sender->chunk_size = 1 << 20;  
		sender->go();  
	}
	else {
		std::cout << "ERROR: sending file info: " << rError.message() << std::endl;
	}
}	

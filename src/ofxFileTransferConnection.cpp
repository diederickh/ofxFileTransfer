#include "ofxFileTransferConnection.h"
#include "ofxFileTransfer.h"
using boost::asio::ip::tcp;

ofxFileTransferConnection::ofxFileTransferConnection(
	boost::asio::io_service& rIOService
)
	:socket_(rIOService)
	,file_size(0)
{
}


// Create a new smart pointer.
// -----------------------------------------------------------------------------
ofxFileTransferConnection::pointer ofxFileTransferConnection::create(
	boost::asio::io_service& rIOService
)
{
	return pointer(new ofxFileTransferConnection(rIOService));
}

// Get socket reference.
// -----------------------------------------------------------------------------
tcp::socket& ofxFileTransferConnection::socket() {
	return socket_;
}
	
// Start reading data from the socket	
// -----------------------------------------------------------------------------	
void ofxFileTransferConnection::start() {
	boost::asio::async_read_until(	
							socket_
							,response
							,"\r\n"
							,boost::bind(
								&ofxFileTransferConnection::handleFileName
								,shared_from_this()
								,boost::asio::placeholders::error
							)
	);
}
	
// The client sends us the file path to which we need to save the incoming data.
// -----------------------------------------------------------------------------
void ofxFileTransferConnection::handleFileName(
				const boost::system::error_code& rError
) 
{
	if(!rError) {
		istream response_stream(&response);
		response_stream >> file_name;
		boost::asio::async_read_until(	
								socket_
								,response
								,"\r\n"
								,boost::bind(
									&ofxFileTransferConnection::handleFileSize
									,shared_from_this()
									,boost::asio::placeholders::error
								)	
		);
	}
	else {
		cout << "Error: " << rError.message() << std::endl;
	}
}

// After we've received the filename, we get the size and start the download.
// -----------------------------------------------------------------------------
void ofxFileTransferConnection::handleFileSize(
						const boost::system::error_code& rError
) 
{
	if(!rError) {
		// get number of bytes to receive.
		istream response_stream(&response);
		response_stream >> file_size >> std::ws;
		
		// open the file to which we need to write the contents.
		std:string out_path = ofToDataPath(
			file_name +ofToString(ofGetElapsedTimeMillis()) +".jpg"
		);
		ofile_stream.open(
					out_path.c_str()
					,std::ios::out|std::ios::trunc|std::ios::binary
		);
		
		// start retrieving the file data.
		bytes_left = file_size;
		boost::asio::async_read(
			socket_
			,boost::asio::buffer(buffer) 
			,boost::bind(
				&ofxFileTransferConnection::handleFileChunk
				,shared_from_this()
				,boost::asio::placeholders::error
				,boost::asio::placeholders::bytes_transferred
			)
		);

	}
	else {
		cout << "Error: " << rError.message() << std::endl;
	}
}


// We receive a chunk of file data and write it to the file stream.
// -----------------------------------------------------------------------------
void ofxFileTransferConnection::handleFileChunk(
			const boost::system::error_code& rError
			,size_t nBytesTransferred

) {
	if(!rError) {
	
		if(ofile_stream.is_open()) {
			ofile_stream.write(buffer.data(),nBytesTransferred);
			bytes_left -= nBytesTransferred;
		
			if(bytes_left < 2000) {


			}
			if(bytes_left == 0) {
				cout << "ready\n";
				// @todo -> we should notify that we're ready and memory can be freed
				ofile_stream.close();
				return;
			}
			size_t num_bytes = buffer.size();
			if(bytes_left > 0 && bytes_left < buffer.size()) {
				num_bytes = bytes_left;
			}
			//cout << "Bytes left: " << bytes_left << " and get: " << num_bytes << std::endl;
			// async_read(), reads untill the buffer is filled.(until buffer.size())
			boost::asio::async_read(
				socket_
				,boost::asio::buffer(buffer,num_bytes) 
				,boost::bind(
					&ofxFileTransferConnection::handleFileChunk
					,shared_from_this()
					,boost::asio::placeholders::error
					,boost::asio::placeholders::bytes_transferred
				)
			);
		}
	}
	else {
		cout << "Error receiving data..." << rError.message() << std::endl;
	}
}


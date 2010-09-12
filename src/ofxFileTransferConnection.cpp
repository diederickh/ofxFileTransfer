#include "ofxFileTransferConnection.h"
#include <sstream> // only for testing.
namespace fs = boost::filesystem;
//,manager(pManager)
//	,ofxFileTransferManager* pManager
ofxFileTransferConnection::ofxFileTransferConnection(
		boost::asio::io_service& rIOService
)
:socket_(rIOService)
,file_size_(0)
{
}

ofxFileTransferConnection::~ofxFileTransferConnection() {
	std::cout << "~~~~ ofxFileTransferConnection" << std::endl;
}

void ofxFileTransferConnection::start() {
	std::cout << __FUNCTION__ << std::endl;

	async_read_until(
			socket_
			,request_buf_
			,"\n\n"
			,boost::bind(
				&ofxFileTransferConnection::handleReadRequest
				,shared_from_this()
				,boost::asio::placeholders::error
				,boost::asio::placeholders::bytes_transferred
			)
	);
}


void ofxFileTransferConnection::handleReadRequest(
		const boost::system::error_code& rErr
		,std::size_t nBytesTransferred
)
{
	if(rErr) {
		return handleError(__FUNCTION__, rErr);
	}
	std::cout << __FUNCTION__ << "(" << nBytesTransferred << ")" << std::endl;
	std::istream request_stream(&request_buf_);
	//request_stream >> file_path_;
	//request_stream >> file_size_;
	//std::stringstream ss(request_buf_);
	char path_buf[1024];
	request_stream.getline(path_buf,1024);
	request_stream >> file_size_;
	//request_stream.getline(size_buf,1024);
	file_path_ = path_buf;
//	file_size_ = size_buf;
	// create path when necessary
	fs::path dir(file_path_);
	if(!fs::exists(dir.parent_path())) {
		std::cout << " path:" << dir.parent_path() << std::endl;
		fs::create_directories(dir.parent_path());
	}
	
	//file_path_ = ofToDataPath(file_path_);
	//file_pat
	std::cout << __FUNCTION__ << " " << file_size_ << " name:" << file_path_ << std::endl;
	request_stream.read(buffer_.c_array(), 2); // read "\n\n"
	out_file_stream_.open(file_path_.c_str(), std::ios_base::binary);
	if(!out_file_stream_) {
		std::cout << "<< failed opening: " << file_path_ << std::endl;
		return;
	}

	// write extra bytes received
	do {
		request_stream.read(buffer_.c_array(), (std::streamsize)buffer_.size());
		out_file_stream_.write(buffer_.c_array(), request_stream.gcount());

	} while(request_stream.gcount() > 0);

	boost::asio::async_read(
		socket_
		,boost::asio::buffer(buffer_.c_array(), buffer_.size())
		,boost::bind(
			&ofxFileTransferConnection::handleFileContent
			,shared_from_this()
			,boost::asio::placeholders::error
			,boost::asio::placeholders::bytes_transferred
		)
	);
}


void ofxFileTransferConnection::handleFileContent(
		const boost::system::error_code& rErr
		,std::size_t nBytesTransferred
)
{
	if(nBytesTransferred > 0) {
		out_file_stream_.write(buffer_.c_array(), (std::streamsize)nBytesTransferred);
		std::cout << __FUNCTION__ << "<< recv " << out_file_stream_.tellp() << " bytes." << std::endl;
		std::cout << __FUNCTION__ << out_file_stream_.tellp() << " <> " << (std::streamsize)file_size_ << std::endl;
		if(out_file_stream_.tellp() >= (std::streamsize)file_size_) {
			//manager->removeTransfer(shared_from_this());
			return ;
		}
	}
	if(rErr) {
		return handleError(__FUNCTION__, rErr);
	}
	boost::asio::async_read(
		socket_
		,boost::asio::buffer(buffer_.c_array(), buffer_.size())
		,boost::bind(
			&ofxFileTransferConnection::handleFileContent
			,shared_from_this()
			,boost::asio::placeholders::error
			,boost::asio::placeholders::bytes_transferred
		)
	);

}

void ofxFileTransferConnection::handleError(
		const std::string& rFunction
		,const boost::system::error_code& rErr
)
{
	std::cout	<< __FUNCTION__
				<< " in " << rFunction
				<< " due to " << rErr
				<< " " << rErr.message()
				<< std::endl;
}


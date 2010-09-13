#include "ofxSyncClient.h"
#include "ofMain.h"

ofxSyncClient::ofxSyncClient(
	const std::string sSyncServerIP
	,const std::string nSyncServerPort
	,const std::string nTransferServerPort

):resolver_(io_service_)
,socket_(io_service_)
,sync_server_ip_(sSyncServerIP)
,sync_server_port_(nSyncServerPort)
,transfer_server_ip_(sSyncServerIP)
,transfer_server_port_(nTransferServerPort)
,file_server_((unsigned short)atoi(transfer_server_port_.c_str()))
,is_connected(false)
{
	file_server_.startThread();
}

ofxSyncClient::~ofxSyncClient() {
	std::cout << "~~~~ ofxSyncClient()" << std::endl;
}

void ofxSyncClient::connect() {
	std::cout << "ofxSyncClient.start()" << std::endl;
	tcp::resolver::query query(sync_server_ip_, sync_server_port_);
	resolver_.async_resolve(
					query
					,boost::bind(
							&ofxSyncClient::handleResolve
							,shared_from_this()
							,boost::asio::placeholders::error
							,boost::asio::placeholders::iterator
					)
	);
	io_service_.run();
}

void ofxSyncClient::handleResolve(
		const boost::system::error_code& rErr
		,tcp::resolver::iterator oEndPoint
)
{
	if(!rErr) {
		tcp::endpoint endpoint = *oEndPoint;
		socket_.async_connect(
						endpoint
						,boost::bind(
							&ofxSyncClient::handleConnect
							,shared_from_this()
							,boost::asio::placeholders::error
							,++oEndPoint
						)
		);
		//io_service_.run_one();
	}
	else {
		std::cout << "Error: " << rErr.message() << std::endl;
	}
}


void ofxSyncClient::handleConnect(
		const boost::system::error_code& rErr
		,tcp::resolver::iterator oEndPoint
)
{
	if(!rErr) {
		is_connected = true;
		cout << "CONNECTED" << std::endl;
		synchronize();
		//io_service_.run();
	}
	else if (oEndPoint != tcp::resolver::iterator()) {
		socket_.close();
		tcp::endpoint endpoint = *oEndPoint;
		socket_.async_connect(
						endpoint
						,boost::bind(
							&ofxSyncClient::handleConnect
							,shared_from_this()
							,boost::asio::placeholders::error
							,++oEndPoint
						)
		);
	}
	else {
		std::cout << "Error: " << rErr.message() << std::endl;
	}

}
void ofxSyncClient::run() {
	std::cout << "run again..." << std::endl;

	io_service_.run();
}
void ofxSyncClient::synchronize() {
	// send data!
	std::iostream o(&request_);
	//ldir: "client" directory which we want to keep up2date
	//rdit: the "remote" directory on server with which we want to sync
	std::string ldir = "/Users/diederickhuijbers/Documents/programming/c++/of61fatxcode/apps/diederick/0059_sync_client/bin/data/test/";
	std::string rdir = "/Users/diederickhuijbers/Documents/programming/c++/of61fatxcode/apps/diederick/0058_sync_server/bin/data/images/";
	
	//o << "local_dir" << std::endl;
	//o << "remote_dir" << std::endl;
	dir_list_.getList(ldir, o);

	std::cout << "synchronize!" <<std::endl;

	uint32_t size = request_.size();
	uint32_t transfer_port = atoi(transfer_server_port_.c_str());
	std::iostream copy_buf(&size_buf_);
	copy_buf.write((char*)&size, sizeof(size));
	copy_buf.write((char*)&transfer_port, sizeof(transfer_port));
	ldir += "\n";
	rdir += "\n";
	copy_buf.write(ldir.c_str(), ldir.size());
	copy_buf.write(rdir.c_str(), rdir.size());
	copy_buf.write("\n", 1);

	cout << "size: "<< request_.size() << std::endl;

	boost::asio::async_write(
				socket_
				,size_buf_
				,boost::bind(
					&ofxSyncClient::handleSize
					,shared_from_this()
					,boost::asio::placeholders::error
				)
	);
	//io_service_.run_one();
}

bool ofxSyncClient::isConnected() {
	return is_connected;
}

void ofxSyncClient::handleSize(
		const boost::system::error_code& rErr
)
{
	if(!rErr) {
		std::cout << "ofxSyncClient.handleSize()" << std::endl;
		boost::asio::async_write(
						socket_
						,request_
						,boost::bind(
							&ofxSyncClient::handleWrite
							,shared_from_this()
							,boost::asio::placeholders::error
							,boost::asio::placeholders::bytes_transferred
						)
		);
	}
	else {
		std::cout << "Error: " << rErr.message() << std::endl;
	}
}



void ofxSyncClient::handleWrite(
			const boost::system::error_code& rErr
			,size_t nBytesTransferred
)
{
	if(!rErr) {
		std::cout << "ofxSyncClient.handleWrite(): Ready sending"  << nBytesTransferred << std::endl;
		std::cout << "current length: " << request_.size() << std::endl;
		io_service_.reset();
		//std::cout << "READY SENDING: " << nBytesTransferred << " bytes" << std::endl;
	}
	else {
		std::cout << "ofxSyncClient.handleSize(): error: " << rErr.message() << std::endl;
		//std::cout << "Error: " << rErr.message() << std::endl;
	}
}

void ofxSyncClient::handleReady(
			const boost::system::error_code& rErr
)
{
	if(!rErr) {
		std::cout << "READY SENDING!!" << std::endl;
	}
	else {
		std::cout << "Error: " << rErr.message() << std::endl;
	}
}

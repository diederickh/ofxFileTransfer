#include "ofxSyncServer.h"


ofxSyncServer::ofxSyncServer(unsigned short nPort) 
:acceptor_(
		io_service_
		,boost::asio::ip::tcp::endpoint(
				boost::asio::ip::tcp::v4()
				,nPort
		)
		,true
)
{
}

void ofxSyncServer::start() {
	ofxSyncConnection::pointer new_con(new ofxSyncConnection(io_service_));
	acceptor_.async_accept(
		new_con->socket()
		,boost::bind(	
			&ofxSyncServer::handleAccept
			,this
			,new_con
			,boost::asio::placeholders::error
		)
	);
	io_service_.run();
}

void ofxSyncServer::handleAccept(ofxSyncConnection::pointer pCon, const boost::system::error_code &rErr) {
	std::cout << "New connection! whoot" << std::endl;
	if(!rErr) {
		pCon->start();
		ofxSyncConnection::pointer new_con(new ofxSyncConnection(io_service_));
		acceptor_.async_accept(
			new_con->socket()
			,boost::bind(
				&ofxSyncServer::handleAccept
				,this
				,new_con
				,boost::asio::placeholders::error
			)
		);
		
	}
	else {
		std::cout << rErr.message() << std::endl;
	}
}

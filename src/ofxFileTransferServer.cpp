#include "ofxFileTransferServer.h"

ofxFileTransferServer::ofxFileTransferServer(unsigned short nPort)
	:acceptor(
		io_service
		,boost::asio::ip::tcp::endpoint(
			boost::asio::ip::tcp::v4()
			,nPort
		)
		,true
	)
	,port(nPort)
{
}

// test
void ofxFileTransferServer::startThread() {
	thread_ptr = boost::shared_ptr<boost::thread>(
		new boost::thread(
			boost::bind(
				&ofxFileTransferServer::accept
				,shared_from_this()
			)
		)
	);

}
ofxFileTransferServer::~ofxFileTransferServer() {
	shutdown();
}


void ofxFileTransferServer::shutdown() {
	std::cout << "shutdown";
	io_service.stop();
}

void ofxFileTransferServer::accept() {
	ofxFileTransferConnection::pointer new_connection(new ofxFileTransferConnection(io_service));
	acceptor.async_accept(
					new_connection->socket()
					,boost::bind(
						&ofxFileTransferServer::handleAccept
						,shared_from_this()
						,new_connection
						,boost::asio::placeholders::error
					)
	);
	std::cout << __FUNCTION__ << " start accepting on port: " << port << std::endl;
	io_service.run();
	
}


void ofxFileTransferServer::handleAccept(
			ofxFileTransferConnection::pointer pConnection
			,const boost::system::error_code& rErr
)
{
	std::cout << __FUNCTION__ << " " << rErr << ", " << rErr.message() << std::endl;
	if(!rErr) {
		pConnection->start();
		//ofxFileTransferConnection::pointer new_connection(new ofxFileTransferConnection(io_service));
		pConnection.reset(new ofxFileTransferConnection(io_service));
		acceptor.async_accept(
						pConnection->socket()
						,boost::bind(
							&ofxFileTransferServer::handleAccept
							,shared_from_this()
							,pConnection
							,boost::asio::placeholders::error
						)
		);
		

	}
}


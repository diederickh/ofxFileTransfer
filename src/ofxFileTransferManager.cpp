#include "ofxFileTransferManager.h"



ofxFileTransferManager::ofxFileTransferManager()
:work_(io_service_)
{ 
}

void ofxFileTransferManager::transferFile(
			const std::string sServer
			,const std::string nPort
			,const std::string sFile
			,const std::string sRemoteFile
)
{
	ofxFileTransferClient::pointer client(new ofxFileTransferClient(
		io_service_
		,sServer
		,nPort
		,sFile
		,sRemoteFile
	));
	client->start();
}

void ofxFileTransferManager::startThread() {
	boost::thread t(boost::bind(
		&ofxFileTransferManager::run
		,this
	));
}

void ofxFileTransferManager::run() {
	io_service_.run();
}
 
#ifndef OFXSYNCDIRLISTH
#define OFXSYNCDIRLISTH

#include <boost/filesystem.hpp>
#include <string>
#include <iostream>
#include <sstream>

namespace fs = boost::filesystem;

class ofxSyncDirList {
public:
	ofxSyncDirList();
	bool getList(std::string sPath, std::iostream& rOut);

};

#endif
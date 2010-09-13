#ifndef OFXSYNCDIRLISTH
#define OFXSYNCDIRLISTH

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

namespace fs = boost::filesystem;

struct SyncInfo {
	SyncInfo(
		std::string sFileName
		,std::string sRelativeFileName
		,std::size_t nFileSize
		
		)
		:file_name(sFileName)
		,relative_file_name(sRelativeFileName)
		,file_size(nFileSize)
	{
	}
	

	bool operator< (const SyncInfo& a) {
		if(a.relative_file_name == relative_file_name ) {
			return  false;
		}
		return a.relative_file_name < relative_file_name;	
	}
	std::string relative_file_name; 
	std::string file_name;
	std::size_t file_size;
};

struct SyncSort {
	inline bool operator()(const SyncInfo& a, const SyncInfo& b) {
		if(a.relative_file_name == b.relative_file_name ) {
			return  true;
		}
		return a.relative_file_name > b.relative_file_name;
	}
};


class ofxSyncDirList {
public:
	ofxSyncDirList();
	bool getList(std::string sPath, std::iostream& rOut);
	bool parseList(
				 std::iostream& rDataStream
				,std::string sBasePath
				,std::vector<SyncInfo>&rResultList
	);
	bool getDifference(		
				std::vector<SyncInfo>&a
				,std::vector<SyncInfo>&b
				,std::vector<SyncInfo>&rResult
	);
	bool isHidden(fs::path rPath);
	fs::path getPathRelativeTo(fs::path oRelativeTo, fs::path oPath);
};

#endif
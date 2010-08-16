#include "ofxSyncDirList.h"

namespace fs = boost::filesystem;

ofxSyncDirList::ofxSyncDirList() {
}

bool ofxSyncDirList::getList(std::string sPath, std::iostream& rOut) {
	fs::basic_recursive_directory_iterator<fs::path> cur(sPath);
    fs::basic_recursive_directory_iterator<fs::path> end;
	std::stringstream out;
    for (; cur != end; ++cur) {
		if(!fs::is_regular_file(cur->path())) { 
			continue;
		}
		// get file size
		uint32_t file_size = -1;
		file_size = fs::file_size(cur->path());
		
		// create ID we use to create the sha1 hash.
		std::string node_name = cur->path().string();
		uint32_t name_size = node_name.size()+1;
		std::stringstream node_id("");
		node_id.write((char*)&file_size, sizeof(file_size));
		node_id.write((char*)&name_size, sizeof(name_size));
		node_id.write((char*)node_name.c_str(),node_name.size()+1);
		rOut.write(node_id.str().c_str(), node_id.str().size());
    }
}


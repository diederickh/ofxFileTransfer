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

// Parse a stream created by 'getList'.
bool ofxSyncDirList::parseList(std::iostream& rDataStream, std::vector<SyncInfo>&rResultList) {
	uint32_t file_size;
	uint32_t name_size;

	while(rDataStream) {
		char buf[4096];

		rDataStream.read((char*)&file_size, sizeof(file_size));
		rDataStream.read((char*)&name_size, sizeof(name_size));
		if(name_size > 4096) {
			std::cout << "Error: length of filename is incorrect: " << name_size << std::endl;
			break;
		}
		rDataStream.read(&buf[0], name_size);
		buf[name_size] = '\0';
		std::string name(buf);
		
		SyncInfo info(name, file_size);
		rResultList.push_back(info);
		
		std::cout << "file size: "<< file_size << " name-len: " << name_size << ", name: '" << name << "'" << std::endl;
	}
	return true;
}

bool ofxSyncDirList::getDifference(		
				std::vector<SyncInfo>&a
				,std::vector<SyncInfo>&b
				,std::vector<SyncInfo>&rResult
)
{
	std::sort(a.begin(), a.end(), SyncSort());
	std::sort(b.begin(), b.end(), SyncSort());
	std::set_difference(
				a.begin()
				,a.end()
				,b.begin()
				,b.end()
				,std::back_inserter(rResult)
	);
	return true;
}

#include "ofxSyncDirList.h"
#include "ofMain.h"
namespace fs = boost::filesystem;

ofxSyncDirList::ofxSyncDirList() {
}

bool ofxSyncDirList::getList(std::string sPath, std::iostream& rOut) {
	//sPath = ofToDataPath(sPath,true);
	//sPath = "./data/images/";
	//cout << sPath;
	//return true;
	
	// when source directory doesnt exist, return.
	fs::path source_pir(sPath);
	if(!fs::exists(source_pir))
		return false;
	
	fs::basic_recursive_directory_iterator<fs::path> cur(sPath);
    fs::basic_recursive_directory_iterator<fs::path> end;
	std::stringstream out;

    for (; cur != end; ++cur) {
		if(!fs::is_regular_file(cur->path())) {
			continue;
		}
		
		std::string filename = cur->path().filename();
		if(isHidden(cur->path())) {
			continue;
		}
		cout << "Use file in list: "<< filename << std::endl;
	
		
		// get file size
		uint32_t file_size = -1;
		file_size = fs::file_size(cur->path());
		
		std::string node_name = cur->path().string();
		uint32_t name_size = node_name.size()+1;
		std::stringstream node_id("");
		node_id.write((char*)&file_size, sizeof(file_size));
		node_id.write((char*)&name_size, sizeof(name_size));
		node_id.write((char*)node_name.c_str(),node_name.size()+1);
		rOut.write(node_id.str().c_str(), node_id.str().size());
    }
}

bool ofxSyncDirList::isHidden(fs::path rPath) {
	fs::path::iterator it = rPath.begin();
	while (it != rPath.end()) {
		std::string node = (*it);
		if(	node.size() >=1 
			&& node.substr(0,1) == "." 
			&& node.substr(1,1) != ".") 
		{	
			return true;
		}
		++it;
    }
	return false;
}

// from: https://svn.boost.org/trac/boost/ticket/1976
boost::filesystem::path getPathRelativeTo(
	 boost::filesystem::path const path
	,boost::filesystem::path const base) 
{
    if (path.has_root_path()){
        if (path.root_path() != base.root_path()) {
            return path;
        } else {
            return getPathRelativeTo(path.relative_path(), base.relative_path());
        }
    } else {
        if (base.has_root_path()) {
            throw "cannot uncomplete a path relative path from a rooted base";
        } else {
            typedef boost::filesystem::path::const_iterator path_iterator;
            path_iterator path_it = path.begin();
            path_iterator base_it = base.begin();
            while ( path_it != path.end() && base_it != base.end() ) {
                if (*path_it != *base_it) break;
                ++path_it; ++base_it;
            }
            boost::filesystem::path result;
            for (; base_it != base.end(); ++base_it) {
                result /= "..";
            }
            for (; path_it != path.end(); ++path_it) {
                result /= *path_it;
            }
            return result;
        }
	}
}
    

// Parse a stream created by 'getList'.
bool ofxSyncDirList::parseList(std::iostream& rDataStream, std::vector<SyncInfo>&rResultList) {
	uint32_t file_size;
	uint32_t name_size;
//	std::string local_path;
//	std::string remote_path;
//	rDataStream >> local_path >> remote_path;
//	std::cout << "Localpath: " << local_path << ", remote path: " << remote_path << std::endl;
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

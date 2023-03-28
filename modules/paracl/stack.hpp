#pragma once

#include <unordered_map>
#include <string>
#include <deque>
#include <assert.h>

namespace ptree {

//functor MemManager
class MemManager { 
private:
	std::unordered_map<std::string, int> scopeoffset;
	std::unordered_map<std::string, int> nameoffset;
	int stackpointer;
	std::deque<std::string> scopeorder;
	int lastscopeid;
	std::string lastscopename;

public:
	MemManager() : stackpointer(0), lastscopeid(0), lastscopename("0") {
		scopeoffset[""] = 0;
		scopeorder.push_front("");
	}
	~MemManager() = default;
	//create new scope inside the last one and return start offset of new scope
	int openscope() {
		++lastscopeid;
		lastscopename = std::to_string(lastscopeid);
		scopeoffset[lastscopename] = stackpointer;
		scopeorder.push_front(lastscopename);
		return stackpointer; 
	}
	//close the last scope
	void closescope() {
		stackpointer = scopeoffset[scopeorder.front()]; 
		scopeorder.pop_front();
	}
	//create new variable in the last scope and return its offset 
	int operator()(std::string name, int namesize = sizeof(int)) {
		assert(scopeoffset.find(scopeorder.front()) != scopeoffset.end());
		std::string fullname = scopeorder.front() + ":" + name;
		nameoffset[fullname] = stackpointer;
		stackpointer += namesize;
		return nameoffset[fullname];
	}
	//retrun offset of the last variable with such name
	int getnameoffset(std::string name) {
		std::string fullname;
		for (auto it : scopeorder) {
			fullname = it + ":" + name;
			auto nameptr = nameoffset.find(fullname);
			if (nameptr != nameoffset.end())
				return nameptr->second;
		} 
		throw "ptree::MemManager.getnameoffset() error";
	}
	friend std::ostream& operator<< (std::ostream &out, const MemManager &memfunc); 
};

std::ostream& operator<< (std::ostream &out, const MemManager &memfunc) {
    out << "Stackpointer: " << memfunc.stackpointer << std::endl;
    out << "Scope offsets: " << std::endl;
    for (auto &it : memfunc.scopeoffset) 
    	out << it.first << " " << it.second << std::endl;
    for (auto &it : memfunc.nameoffset) 
    	out << it.first << " " << it.second << std::endl;
    return out; 
}

}
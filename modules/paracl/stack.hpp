#pragma once

#include <unordered_map>
#include <string>
#include <assert.h>

namespace ptree {

//functor MemManager
class MemManager { 
private:
	std::unordered_map<std::string, int> scopeoffset;
	std::unordered_map<std::string, int> nameoffset;
	int stackpointer;
public:
	MemManager() : stackpointer(0) {
		scopeoffset[""] = 0;
	}
	~MemManager() = default;
	int openscope(std::string scope) {
		assert(scopeoffset.find(scope) == scopeoffset.end());
		scopeoffset[scope] = stackpointer;
		return stackpointer; 
	}
	void closescope(std::string scope) {
		stackpointer = scopeoffset[scope]; 
	}
	int operator()(std::string name, std::string scope = "", int namesize = sizeof(int)) {
		assert(scopeoffset.find(scope) != scopeoffset.end());
		std::string fullname = scope + ":" + name;
		nameoffset[fullname] = stackpointer;
		stackpointer += namesize;
		return nameoffset[fullname];
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
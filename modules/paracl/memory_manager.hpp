#pragma once

#include "paracl.hpp"

#include <unordered_map>
#include <string>
#include <deque>
#include <vector>
#include <cstring>
#include <assert.h>
#include <iostream>


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
	int maxsize;

public:
	//create MemManager
	MemManager();
	//destroy MemManager
	~MemManager() = default;
	//create new scope inside the last one and return start offset of new scope
	std::pair<int, int> openscope();
	//close the last scope
	void closescope();
	//create new variable in the last scope and return its offset 
	int operator()(std::string name, int namesize = sizeof(int));
	//retrun offset of the last variable with such name or -1 if such variable is not exist
	int getnameoffset(std::string name);
	//return necessary stack size 
	int getmaxstacksize() const;
	friend std::ostream& operator<< (std::ostream &out, const MemManager &memfunc); 
};



//calculate all offsets for variables and return MemManager object with result information
void manage_mem(PTree *unit, MemManager &memfunc);
MemManager manage_tree_mem(PTree *root);

}
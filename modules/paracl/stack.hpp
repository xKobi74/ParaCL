#pragma once

#include <unordered_map>
#include <string>
#include <deque>
#include <vector>
#include <cstring>
#include <assert.h>

#define CAN_BE_IN_STACK typename CONDITION =  					\
	typename std::enable_if<std::is_same<T, int>::value ||		\
    std::is_same<T, double>::value ||							\
    std::is_same<T, float>::value, T>::type

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
	MemManager() : stackpointer(0), lastscopeid(0), lastscopename("0"), maxsize(0) {
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
		maxsize = std::max(maxsize, stackpointer);
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

	int getmaxstacksize() const {
		return maxsize;
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

class Stack {
	char *memory;
public:
	Stack(int maxsize) {
		memory = new char[maxsize];
	}
	~Stack() {
		delete[] memory;
	}
	Stack(const Stack& other) = delete;
    Stack& operator=(const Stack& other) = delete;
    Stack(Stack&& old) {
    	memory = old.memory;
    	old.memory = nullptr;
    }
    Stack& operator=(Stack&& old) {
        std::swap(memory, old.memory);
        return *this;
    }

    template <typename T, CAN_BE_IN_STACK>
    void write(int offset, T &value) {
    	memcpy(memory + offset, &value, sizeof(value));
    }

    template <typename T, CAN_BE_IN_STACK>
    void read(int offset, T &value) const {
    	memcpy(&value, memory + offset, sizeof(value));
    }


};

}
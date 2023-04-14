#pragma once

#include "paracl.hpp"

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
	//create MemManager
	MemManager() : stackpointer(0), lastscopeid(0), lastscopename("0"), maxsize(0) {
		scopeoffset[""] = 0;
		scopeorder.push_front("");
	}
	//destroy MemManager
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
	//retrun offset of the last variable with such name or -1 if such variable is not exist
	int getnameoffset(std::string name) {
		std::string fullname;
		for (auto it : scopeorder) {
			fullname = it + ":" + name;
			auto nameptr = nameoffset.find(fullname);
			if (nameptr != nameoffset.end())
				return nameptr->second;
		} 
		return -1;
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

// Stack memory emulator
class Stack {
	char *memory;
public:
	//create Stack with given size
	Stack(int maxsize) {
		memory = new char[maxsize];
	}
	//destroy Stack
	~Stack() {
		delete[] memory;
	}
	//Stack object can't be copied, it can be only moved 
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

    //write value into Stack in given offset
	//WARNING: this method does not check anything about permission
    template <typename T, CAN_BE_IN_STACK>
    void write(int offset, T &value) {
    	memcpy(memory + offset, &value, sizeof(value));
    }

    //read value from Stack by given offset
    //WARNING: this method does not check anything about permission
    template <typename T, CAN_BE_IN_STACK>
    void read(int offset, T &value) const {
    	memcpy(&value, memory + offset, sizeof(value));
    }
};

//calculate all offsets for variables and return MemManager object with result information
void manage_mem(PTree *unit, MemManager &memfunc);
MemManager manage_tree_mem(PTree *root) {
	MemManager memfunc;
	manage_mem(root, memfunc);
	return memfunc;
}
void manage_mem(PTree *unit, MemManager &memfunc) {
	Block *block = dynamic_cast<Block *>(unit);
	if (block) {
		memfunc.openscope();
		using opit = std::vector<PTree*>::iterator;
		opit it = block->operations.begin();
		opit end = block->operations.end();
		for (; it != end; ++it)
			manage_mem(*it, memfunc);
		memfunc.closescope();
		return;
	}
	IfBlk *ifblock = dynamic_cast<IfBlk *>(unit);
	if (ifblock) {
		manage_mem(ifblock->condition_, memfunc);
		manage_mem(ifblock->getleft(), memfunc);
		manage_mem(ifblock->getright(), memfunc);
		return;
	}
	WhileBlk *whileblock = dynamic_cast<WhileBlk *>(unit);
	if (whileblock) {
		manage_mem(whileblock->condition_, memfunc);
		manage_mem(whileblock->getleft(), memfunc);
		return;
	}
	Expression *expression = dynamic_cast<Expression *>(unit);
	if (expression) {
		manage_mem(expression->getright(), memfunc);
		return;
	}
	Assign *assign = dynamic_cast<Assign *>(unit);
	if (assign) {
		NameInt *nameint = dynamic_cast<NameInt *>(assign->getleft());
		nameint->setoffset(memfunc.getnameoffset(nameint->getvarname()));
		if (nameint->getoffset() < 0)
			nameint->setoffset(memfunc(nameint->getvarname()));
		manage_mem(assign->getright(), memfunc);	
		return;
	}
	NameInt *nameint = dynamic_cast<NameInt *>(unit);
	if (nameint) {
		nameint->setoffset(memfunc.getnameoffset(nameint->getvarname()));
		return;
	}
	if (unit) {
		manage_mem(unit->getleft(), memfunc);
		manage_mem(unit->getright(), memfunc);
		return;
	}
	if (unit) 
		throw "ptree::manage_mem(PTree *,MemManager) error";
}

}
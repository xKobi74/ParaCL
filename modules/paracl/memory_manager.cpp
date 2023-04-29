#include "memory_manager.hpp"

namespace ptree {

MemManager::MemManager() : stackpointer(0), lastscopeid(0), lastscopename("0"), maxsize(0) {
	scopeoffset[""] = 0;
	scopeorder.push_front("");
}
std::pair<int, int> MemManager::openscope() {
	++lastscopeid;
	lastscopename = std::to_string(lastscopeid);
	scopeoffset[lastscopename] = stackpointer;
	scopeorder.push_front(lastscopename);
	return std::pair<int, int>(lastscopeid, stackpointer); 
}
void MemManager::closescope() {
	stackpointer = scopeoffset[scopeorder.front()]; 
	scopeorder.pop_front();
}
int MemManager::operator()(std::string name, int namesize) {
	assert(scopeoffset.find(scopeorder.front()) != scopeoffset.end());
	std::string fullname = scopeorder.front() + ":" + name;
	nameoffset[fullname] = stackpointer;
	stackpointer += namesize;
	maxsize = std::max(maxsize, stackpointer);
	return nameoffset[fullname];
}
int MemManager::getnameoffset(std::string name) {
	std::string fullname;
	for (auto it : scopeorder) {
		fullname = it + ":" + name;
		auto nameptr = nameoffset.find(fullname);
		if (nameptr != nameoffset.end())
			return nameptr->second;
	} 
	return -1;
}
int MemManager::getmaxstacksize() const {
	return maxsize;
}


MemManager manage_tree_mem(PTree *root) {
	MemManager memfunc;
	manage_mem(root, memfunc);
	return memfunc;
}
void manage_mem(PTree *unit, MemManager &memfunc) {
	Block *block = dynamic_cast<Block *>(unit);
	if (block) {
		std::pair<int, int> idandoffset = memfunc.openscope();
		block->id_ = idandoffset.first;
		block->offset_ = idandoffset.second;
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
		NameInt *nameint = assign->lval;
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

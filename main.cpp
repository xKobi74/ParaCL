#include <iostream>

#include "modules/paracl/paracl.hpp"

int main() {
	ptree::PTree root;
	std::cout << root.dump() << std::endl;
	return 0;
}
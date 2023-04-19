#include "stack.hpp"

namespace ptree {

Stack::Stack(int maxsize) {
	memory = new char[maxsize];
}
Stack::~Stack() {
	delete[] memory;
}
Stack::Stack(Stack&& old) {
    memory = old.memory;
   	old.memory = nullptr;
}
Stack& Stack::operator=(Stack&& old) {
    std::swap(memory, old.memory);
    return *this;
}

}
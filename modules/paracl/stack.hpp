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

// Stack memory emulator
class Stack {
	char *memory;
public:
	//create Stack with given size
	Stack(int maxsize);
	//destroy Stack
	~Stack();
	//Stack object can't be copied, it can be only moved 
	Stack(const Stack& other) = delete;
    Stack& operator=(const Stack& other) = delete;
    Stack(Stack&& old);
    Stack& operator=(Stack&& old);

    //write value into Stack in given offset
	//WARNING: this method does not check anything about permission
    template <typename T, CAN_BE_IN_STACK>
    void write(int offset, const T &value) {
        memcpy(memory + offset, &value, sizeof(value));
    }
    //read value from Stack by given offset
    //WARNING: this method does not check anything about permission
    template <typename T, CAN_BE_IN_STACK>
    void read(int offset, T &value) const {
        memcpy(&value, memory + offset, sizeof(value));
    }
};

}
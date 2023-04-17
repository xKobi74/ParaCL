#pragma once

#include <string>
#include <sstream>
#include <memory>

#include "stack.hpp"


namespace ptree {

//return std::string with address of given object
std::string get_addr(const void* addr) {
  std::stringstream address;
  address << addr;
  return address.str();
}

class Stack;

class PTree {
  PTree* parent_;
  PTree *left_, *right_;
public:
  //create PTree with given parent, left and right pointers
  PTree(PTree* parent = nullptr, PTree* left = nullptr, PTree* right = nullptr): parent_(parent), left_(left), right_(right) {};  
  virtual ~PTree() = default;
  //method for tree execution
  virtual std::unique_ptr<PTree> execute(Stack *stack) const {
    return std::unique_ptr<PTree>{};
  }
  //return true if class leaf
  virtual bool isLeaf() const {
    return 0;
  }
  //change parent pointer
  void setparent(PTree *parent) {
    parent_ = parent;
  }
  //get parent pointer
  PTree *getparent() const {
    return parent_;
  }
  //change left pointer
  void setleft(PTree *left) {
    left_ = left;
  }
  //get left pointer
  PTree *getleft() const {
    return left_;
  }
  //change right pointer
  void setright(PTree *right) {
    right_ = right;
  }
  //set right pointer
  PTree *getright() const {
    return right_;
  } 
  //return std::string with info about object in dot format 
  virtual std::string dump() const {
    std::string res = "graph program {";
    if (getleft() != nullptr)
      res += getleft()->dump();
    if (getright() != nullptr)
      res += getright()->dump();
    res += "}";
    return res;
  }
  //return object id
  std::string getid() const {
    return get_addr(this);
  }
  //return unique object name
  std::string getname() const {
    return "node" +  getid();
  }
};
};
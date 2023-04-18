#pragma once

#include <string>
#include <sstream>
#include <memory>

#include "stack.hpp"


namespace ptree {

//return std::string with address of given object
std::string get_addr(const void* addr);

class Stack;

class PTree {
  PTree* parent_;
  PTree *left_, *right_;
public:
  //create PTree with given parent, left and right pointers
  PTree(PTree* parent = nullptr, PTree* left = nullptr, PTree* right = nullptr): parent_(parent), left_(left), right_(right) {};  
  virtual ~PTree() = default;
  //method for tree execution
  virtual std::unique_ptr<PTree> execute(Stack *stack) const;
  //return true if class leaf
  virtual bool isLeaf() const;
  //change parent pointer
  void setparent(PTree *parent);
  //get parent pointer
  PTree *getparent() const ;
  //change left pointer
  void setleft(PTree *left) ;
  //get left pointer
  PTree *getleft() const ;
  //change right pointer
  void setright(PTree *right) ;
  //set right pointer
  PTree *getright() const ;
  //return std::string with info about object in dot format 
  virtual std::string dump() const ;
  //return object id
  std::string getid() const ;
  //return unique object name
  std::string getname() const ;
};
};
#pragma once

#include <string>
#include <sstream>


namespace ptree {

std::string get_addr(const void* addr) {
  std::stringstream address;
  address << addr;
  return address.str();
}

class PTree {
  PTree* parent;
  PTree *left, *right;
public:
  PTree(PTree* parent_ = nullptr, PTree* left_ = nullptr, PTree* right_ = nullptr): parent(parent_), left(left_), right(right_) {};  
  virtual ~PTree() = default;
  virtual PTree* execute() {
    return nullptr;
  }
  virtual bool isLeaf() const {
    return 0;
  }
  virtual std::string dump() const {
    return std::string{""};
  }

  void setparent(PTree *_parent) {
    parent = _parent;
  }
  PTree *getparent() const {
    return parent;
  }
  void setleft(PTree *_left) {
    left = _left;
  }
  PTree *getleft() const {
    return left;
  }
  void setright(PTree *_right) {
    right = _right;
  }
  PTree *getright() const {
    return right;
  }

  std::string getid() const {
    return get_addr(this);
  }
  std::string getname() const {
    return "node" +  getid();
  }
};
};
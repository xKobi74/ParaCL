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
  PTree* parent_;
  PTree *left_, *right_;
public:
  PTree(PTree* parent = nullptr, PTree* left = nullptr, PTree* right = nullptr): parent_(parent), left_(left), right_(right) {};  
  virtual ~PTree() = default;
  virtual PTree* execute() {
    return nullptr;
  }
  virtual bool isLeaf() const {
    return 0;
  }

  void setparent(PTree *parent) {
    parent_ = parent;
  }
  PTree *getparent() const {
    return parent_;
  }
  void setleft(PTree *left) {
    left_ = left;
  }
  PTree *getleft() const {
    return left_;
  }
  void setright(PTree *right) {
    right_ = right;
  }
  PTree *getright() const {
    return right_;
  }

  virtual std::string dump() const {
    std::string res = "graph program {";
    if (getleft() != nullptr)
      res += getleft()->dump();
    if (getright() != nullptr)
      res += getright()->dump();
    res += "}";
    return res;
  }

  std::string getid() const {
    return get_addr(this);
  }
  std::string getname() const {
    return "node" +  getid();
  }
};
};
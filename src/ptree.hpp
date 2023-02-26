#pragma once

#include <string>
#include <sstream>


namespace ptree {
class PTree {
  public:
  PTree* parent;
  PTree *left, *right;
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
  //TODO: test new getid() function
  std::string getid() const {
    std::stringstream address;
    address << static_cast<const void*>(this);
    return address.str();
  }
  std::string getname() const {
    return "node" +  getid();
  }
};
};
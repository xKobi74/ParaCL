#pragma once

#include <string>
#include <cstring>

namespace ptree {
class PTree {
  public:
  PTree* parent;
  PTree *left, *right;
  virtual ~PTree() = default;
  virtual PTree* execute() {}
  virtual bool isLeaf() const {
    return 0;
  }
  virtual std::string dump() const {
    return std::string{""};
  }
  int getid() const {
    int id;
    memcpy(&id, this, sizeof(int));
    return id;
  }
  std::string getname() const {
    int id = getid();
    return "node" +  std::to_string(id);
  }
};
};
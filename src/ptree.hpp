#pragma once

#include <string>

namespace ptree {
class PTree {
  public:
  PTree* parent;
  PTree *left, *right;
  virtual ~PTree();
  virtual PTree* execute();
  virtual bool isLeaf() const;
  virtual std::string dump() const;
};
};
#pragma once

#include <string>

namespace ptree {
class PTree {
  PTree* parent;
  PTree *left, *right;
  virtual ~PTree();
  virtual PTree* execute();
  virtual bool isLeaf() const;
  virtual std::string dump() const;
};
};
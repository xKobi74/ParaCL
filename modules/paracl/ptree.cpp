#include "ptree.hpp"

namespace ptree {
std::string get_addr(const void *addr) {
  std::stringstream address;
  address << addr;
  return address.str();
}
std::unique_ptr<PTree> PTree::execute(Stack *stack) const { return std::unique_ptr<PTree>{}; }
bool PTree::isLeaf() const { return 0; }

void PTree::setparent(PTree *parent) { parent_ = parent; }

PTree *PTree::getparent() const { return parent_; }

void PTree::setleft(PTree *left) { left_ = left; }

PTree *PTree::getleft() const { return left_; }

void PTree::setright(PTree *right) { right_ = right; }

PTree *PTree::getright() const { return right_; }

std::string PTree::dump() const {
  std::string res = "graph program {";
  if (getleft() != nullptr)
    res += getleft()->dump();
  if (getright() != nullptr)
    res += getright()->dump();
  res += "}";
  return res;
}

std::string PTree::getid() const { return get_addr(this); }

std::string PTree::getname() const { return "node" + getid(); }
} // namespace ptree
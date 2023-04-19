#include "leaf.hpp"

namespace ptree {

Leaf::Leaf(PTree* parent): PTree(parent, nullptr, nullptr) {};
bool Leaf::isLeaf() const {
  return true;
}
std::unique_ptr<PTree> Leaf::execute(Stack *stack) const {
  return std::unique_ptr<PTree>{};
}

std::unique_ptr<PTree> intinput() {
  int x;
  std::cin >> x;
#ifdef DBG_CALL
  std::cout << "Input called" << std::endl;
#endif
  return std::unique_ptr<PTree>(new Imidiate<int>(x)); 
}

Reserved::Reserved(PTree* parent, Reserved::Types type) : Leaf(parent), type_(type) {}
Reserved::Types Reserved::gettype() const {
  return type_;
}
std::string Reserved::typetostr() const {
  switch(gettype()) {
    case Types::None:
      return "None";
      break;
    case Types::Input:
      return "Input";
      break;
  }
  return "Smth strange";
}
std::unique_ptr<PTree> Reserved::execute(Stack *stack) const {
#ifdef DBG_CALL
    std::cout << "Resrved called" << std::endl;
#endif
    switch(gettype()) {
      case Types::None:
        return std::unique_ptr<PTree>{};
      case Types::Input:
        return intinput();
    }
    return std::unique_ptr<PTree>{};
}
std::string Reserved::dump() const {
  std::string res;
  std::string parentname = getparent()->getname();
  std::string myname = getname();
  res += myname + " [label=" + '"' + typetostr() + '"' + "];\n";
  return res;
}

NameInfo::NameInfo(PTree* parent, std::string name) : Leaf(parent), name_(name) {};
NameInfo::NameInfo(PTree* parent, int nameid, int offset, std::string name) : Leaf(parent), nameid_(nameid), offset_(offset), name_(name) {};
int NameInfo::getnameid() const {
  return nameid_;
}
int NameInfo::getoffset() const {
  return offset_;
}
void NameInfo::setoffset(int offset) {
  offset_ = offset;
}
std::string NameInfo::getvarname() const {
  return name_;
}

NameInt::NameInt(PTree* parent, int value, std::string name_) : NameInfo(parent, name_), value_(value) {};
NameInt::NameInt(PTree* parent, int value, int nameid, int offset, std::string name_) : NameInfo(parent, nameid, offset, name_), value_(value) {};
int NameInt::getvalue() const { //returns value from the field that value equals to value in stack
  return value_;
}
int NameInt::getvalue(Stack *stack) const { //returns value from the stack
  int value;
  stack->read(getoffset(), value);
  return value;
}
void NameInt::setvalue(int value, Stack *stack) {
  stack->write(getoffset(), value);
  value_ = value;
}
std::string NameInt::dump() const {
  std::string res;
  std::string parentname = getparent()->getname();
  std::string myname = getname();
  res += myname + " [label=" + '"' + getvarname() + '=' + std::to_string(getvalue()) + '\n' + "offset: " + std::to_string(getoffset()) +'"' + "];\n";
  return res;
}
std::unique_ptr<PTree> NameInt::execute(Stack *stack) const {
  return std::unique_ptr<PTree>{new Imidiate<int>(getvalue(stack))};
}

}
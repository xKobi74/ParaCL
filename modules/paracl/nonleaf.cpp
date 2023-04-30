#include "nonleaf.hpp"
#include "leaf.hpp"

#include <stdexcept>
#include <exception>


namespace ptree {
std::string NonLeaf::get_links() const {
  std::string res{""};
  if (getleft() != nullptr)
    res += getname() + " -> " + getleft()->getname() + "\n";
  if (getright() != nullptr)
    res += getname() + " -> " + getright()->getname() + "\n";
  return res;
}

inline std::string NonLeaf::get_chld_dump() const {
  std::string res;
  if (getleft() != nullptr)
    res += getleft()->dump();
  if (getright() != nullptr)
    res += getright()->dump();
  return res;
}

bool NonLeaf::isLeaf() const { return false; }

std::string Expression::dump() const {
  std::string res;
  res += get_chld_dump();

  res += getname() + "[shape = record, label=\"{Expression \\n |" + "{ " +
         get_addr(getleft()) + "\\n (depricated) | " + get_addr(getright()) +
         "\\n(expression)}}\"]\n";

  // HACK: no get_links() here, because nodes should be colorized and with
  // caption
  if (getleft() != nullptr)
    res += getname() + " -> " + getleft()->getname() +
           "[color=\"black\", label=\"continuing\"]\n";
  if (getright() != nullptr)
    res += getname() + " -> " + getright()->getname() +
           "[color=\"black\", label=\"expression\"]\n";

  return res;
}

std::unique_ptr<PTree> Expression::execute(Stack *stack) const {
#ifdef DBG_CALL
  std::cout << "Expression execute" << std::endl;
#endif
  return getright()->execute(stack);
}

template <typename T> T operate(T lhs, T rhs, BinOpType operation) {
  switch (operation) {
  case BinOpType::ADDITION:
    return lhs + rhs;
  case BinOpType::REMAINDER:
    return lhs % rhs;
  case BinOpType::SUBTRACTION:
    return lhs - rhs;
  case BinOpType::MULTIPLICATION:
    return lhs * rhs;
  case BinOpType::DIVISION:
    return lhs / rhs;
  case BinOpType::EQUAL:
    return lhs == rhs;
  case BinOpType::MORE_EQUAL:
    return lhs >= rhs;
  case BinOpType::LESS_EQUAL:
    return lhs <= rhs;
  case BinOpType::NON_EQUAL:
    return lhs != rhs;
  case BinOpType::MORE:
    return lhs > rhs;
  case BinOpType::LESS:
    return lhs < rhs;
  case BinOpType::LOG_AND:
    return lhs && rhs;
  case BinOpType::LOG_OR:
    return lhs || rhs;
  default:
    assert(!"Fault");
    return 0;
  }
}

std::string BinOp::get_op() const {
  switch (operation_) {
  case BinOpType::ADDITION:
    return "+";
  case BinOpType::DIVISION:
    return "/";
  case BinOpType::REMAINDER:
    return "%";
  case BinOpType::MULTIPLICATION:
    return "*";
  case BinOpType::SUBTRACTION:
    return "-";
  case BinOpType::EQUAL:
    return "==";
  case BinOpType::MORE_EQUAL:
    return "\\>=";
  case BinOpType::LESS_EQUAL:
    return "\\<=";
  case BinOpType::NON_EQUAL:
    return "!=";
  case BinOpType::LESS:
    return "\\<";
  case BinOpType::MORE:
    return "\\>";
  case BinOpType::LOG_AND:
    return "&&";
  case BinOpType::LOG_OR:
    return "||";
  default:
    return "?";
  }
}

std::string BinOp::dump() const {
  std::string res;
  res += get_chld_dump();
  res += getname() + "[shape = record, label=\"{Binary operation(" + get_op() +
         ")|" + "{ " + get_addr(getleft()) + " | " + get_addr(getright()) +
         "}}\"]\n";
  res += get_links();
  return res;
}

std::unique_ptr<PTree> BinOp::execute(Stack *stack) const {
#ifdef DBG_CALL
  std::cout << "BinOp execute" << std::endl;
#endif
  auto left_op = getleft()->execute(stack);
  auto right_op = getright()->execute(stack);

  Imidiate<int> *l_exec = dynamic_cast<Imidiate<int> *>(left_op.get());
  Imidiate<int> *r_exec = dynamic_cast<Imidiate<int> *>(right_op.get());

  // TODO: add operations implementation
  assert((l_exec != nullptr) && (r_exec != nullptr));
  int result = operate<int>(l_exec->getvalue(stack), r_exec->getvalue(stack),
                            operation_);
  return std::unique_ptr<PTree>{new Imidiate<int>(result)};
}

std::string UnOp::get_op() const {
  switch (operation_) {
  case UnOpType::POST_ADDITION:
    return "++";
  case UnOpType::POST_SUBTRACTION:
    return "--";
  case UnOpType::MINUS:
    return "-value";
  case UnOpType::NOT:
    return "!value";
  default:
    return "?";
  }
}

std::unique_ptr<PTree> UnOp::execute(Stack *stack) const {
#ifdef DBG_CALL
  std::cout << "UnOp execute" << std::endl;
#endif

  NameInt *var = dynamic_cast<NameInt *>(getleft());
  assert(var != nullptr);
  // here execute method not used because of speed, to not provide more imidiate
  switch (operation_) {
  case UnOpType::POST_ADDITION: {
    int value = var->getvalue(stack);
    var->setvalue(++value, stack);
    return std::unique_ptr<PTree>{new Imidiate<int>(value)};
  }
  case UnOpType::POST_SUBTRACTION: {
    int value = var->getvalue(stack);
    var->setvalue(--value, stack);
    return std::unique_ptr<PTree>{new Imidiate<int>(value)};
  }
  case UnOpType::MINUS: {
    int value = var->getvalue(stack);
    return std::unique_ptr<PTree>{new Imidiate<int>(-value)};
  }
  case UnOpType::NOT: {
    int value = var->getvalue(stack);
    return std::unique_ptr<PTree>{new Imidiate<int>(!value)};
  }
  default: {
    assert(!"Fault");
    return std::unique_ptr<PTree>{};
  }
  }
}

std::string UnOp::dump() const {
  std::string res;
  res += get_chld_dump();
  res += getname() + "[shape = record, label=\"{Unary operation(" + get_op() +
         ")|" + "{ " + get_addr(getleft()) + " | " + get_addr(getright()) +
         "\\n(depricated)}}\"]\n";
  res += get_links();
  return res;
}

void Block::push_expression(PTree *instruction) {
  operations.push_back(instruction);
}

std::string Block::get_blk_info() const {
  std::string res;
#ifdef CPP20
  res += std::format("offset = {:X} \\n block id = {}", offset_, id_);
#else
  res += "offset = " + std::to_string(offset_) +
         "\\n block id = " + std::to_string(id_) + "\\n";
#endif
  res += "expressions count: " + std::to_string(operations.size()) + "\\n";
  return res;
}

void Block::update_blk_info(offset_t offset, block_id id) {
  offset_ = offset;
  id_ = id;
}

std::string Block::dump() const {
  std::string res;
  res += getname() + "[shape = record, label=\"{Block \\n" + get_blk_info() +
         "}\"]\n";
  unsigned int expr_num = 1;
  for (PTree *expr : operations) {
    res += expr->dump();
    res += getname() + " -> " + expr->getname() +
           "[color = \"black\", label=\"expr number: " +
           std::to_string(expr_num++) + "\"]\n";
  }
  return res;
}

std::unique_ptr<PTree> Block::execute(Stack *stack) const {
#ifdef DBG_CALL
  std::cout << "Block execute" << std::endl;
#endif
  for (auto expr : operations)
    expr->execute(stack);
  return std::unique_ptr<PTree>{};
}

std::string Assign::dump() const {
  std::string res;
  res += get_chld_dump();
  res += lval->dump();

  res += getname() + "[shape = record, label=\"{Assignation (a = b) \\n |" +
         "{ " + get_addr(lval) + "\\n (a) | " + get_addr(getright()) +
         "\\n(b)}}\"]\n";

  // HACK: no get_links() here, because nodes should be colorized and with
  // caption
  if (lval != nullptr)
    res += getname() + " -> " + lval->getname() +
           "[color=\"black\", label=\"assignable\"]\n";
  if (getright() != nullptr)
    res += getname() + " -> " + getright()->getname() +
           "[color=\"black\", label=\"to assign\"]\n";

  return res;
}

std::unique_ptr<PTree> Assign::execute(Stack *stack) const {
#ifdef DBG_CALL
  std::cout << "Assign execute" << std::endl;
#endif
  std::unique_ptr<PTree> executed = getright()->execute(stack);
  const Imidiate<int> *to_assign =
      dynamic_cast<Imidiate<int> *>(executed.get());
  lval->setvalue(to_assign->getvalue(), stack);
  return executed;
}

std::string Condition::dump() const {
  std::string res;
  res += get_chld_dump();

  res += getname() + "[shape = record, label=\"{Condition \\n |" + "{ " +
         get_addr(getleft()) + "\\n (condition) | " + get_addr(getright()) +
         "\\n(depricated)}}\"]\n";

  res += get_links();

  return res;
}

std::unique_ptr<PTree> Condition::execute(Stack *stack) const {
#ifdef DBG_CALL
  std::cout << "Condition execute" << std::endl;
#endif
  return getleft()->execute(stack);
}

bool Condition::is_true(Stack *stack) const {
  std::unique_ptr<PTree> executed = execute(stack);
  // I`m so sorry for using dynamic cast here, maybe should use typeid + static_cast
  Imidiate<int> *result = dynamic_cast<Imidiate<int> *>(executed.get());
  assert(result != nullptr);

  return result->getvalue();
}

std::string IfBlk::dump() const {
  std::string res;
  res += get_chld_dump();
  if (condition_ != nullptr)
    res += condition_->dump();

  res += getname() + "[shape = record, label=\"{If clause \\n |" + "{ " +
         get_addr(getleft()) + "\\n (false case) | " + get_addr(getright()) +
         "\\n(true case)}}\"]\n";

  // HACK: no get_links() here, because nodes should be colorized and with
  // caption
  if (getleft() != nullptr)
    res += getname() + " -> " + getleft()->getname() +
           "[color=\"red\", label=\"false\"]\n";
  if (getright() != nullptr)
    res += getname() + " -> " + getright()->getname() +
           "[color=\"green\", label=\"true\"]\n";

  if (condition_ != nullptr)
    res += getname() + " -> " + condition_->getname() + " [style=dotted]\n";
  return res;
}

std::unique_ptr<PTree> IfBlk::execute(Stack *stack) const {
#ifdef DBG_CALL
  std::cout << "If execute" << std::endl;
#endif
  //TODO: remove condidition == nullptr to throw
  if (condition_ == nullptr) {
    throw std::logic_error{"Missing condition in if block"};
    return std::unique_ptr<PTree>{};
  }

  if (condition_->is_true(stack)) {
    if (getright() != nullptr)
      return getright()->execute(stack);
  } else {
    if (getleft() != nullptr)
      return getleft()->execute(stack);
  }
  return std::unique_ptr<PTree>{};
}

std::string WhileBlk::dump() const {
  std::string res;
  res += get_chld_dump();
  if (condition_ != nullptr)
    res += condition_->dump();

  res += getname() + "[shape = record, label=\"{While clause \\n |" + "{ " +
         get_addr(getleft()) + "\\n (cycle) | " + get_addr(getright()) +
         "\\n(depricated)}}\"]\n";

  res += get_links();
  if (condition_ != nullptr)
    res += getname() + " -> " + condition_->getname() +
           " [style=dotted, label=\"condition\"]\n";
  return res;
}

std::unique_ptr<PTree> WhileBlk::execute(Stack *stack) const {
#ifdef DBG_CALL
  std::cout << "While execute" << std::endl;
#endif

  if (condition_ == nullptr){
    throw std::logic_error("No condition in while cycle");
    return std::unique_ptr<PTree>{};
  }

  while (condition_->is_true(stack))
    getleft()->execute(stack);
  return std::unique_ptr<PTree>{};
}

std::string Output::dump() const {
  std::string res;
  res += get_chld_dump();
  res += getname() + "[shape = record, label=\"{print (" +
         get_addr(getright()) + ")}\"]\n";
  res += get_links();
  return res;
}

std::unique_ptr<PTree> Output::execute(Stack *stack) const {
#ifdef DBG_CALL
  std::cout << "Print execute" << std::endl;
#endif
  std::unique_ptr<PTree> executed = getright()->execute(stack);
  // I`m so sorry for using dynamic cast here, maybe should use typeid + static_cast
  Imidiate<int> *value = dynamic_cast<Imidiate<int> *>(executed.get());
  assert(value != nullptr);
  std::cout << value->getvalue() << std::endl;
  return executed;
}
} // namespace ptree
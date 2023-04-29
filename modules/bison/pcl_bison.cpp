#include "pcl_bison.hpp"

extern int yylineno;

ptree::Block* wrap_block(ptree::PTree* statement) {
  if (statement == nullptr) return nullptr;
  ptree::Block* result = dynamic_cast<ptree::Block*> (statement);
  if (!result) {
      result = new ptree::Block{};
      result->push_expression(statement);
  }
  return result;
}

void yyerror(char *s) {
  std::cerr << s << ", line " << yylineno << std::endl;
  exit(1);
}
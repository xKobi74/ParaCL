#pragma once 
//#include <gtest/gtest.h>
//#include <pthread.h>

#include "../modules/paracl/nonleaf.hpp"

TEST(NonLeaf, ConstructTest) {
  ptree::NonLeaf r{nullptr, nullptr, nullptr};
  std::string res = r.dump();
  ASSERT_FALSE(r.isLeaf());
}

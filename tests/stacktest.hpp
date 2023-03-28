#pragma once

#include "../modules/paracl/stack.hpp"

#include <string>
#include <sstream>

TEST(MemManager, MainTest) {
	ptree::MemManager memfunc;
	std::stringstream out;
	out << memfunc;	
}

TEST(MemManager, FunctionalTest) {
	ptree::MemManager memfunc;
	ASSERT_EQ(0, memfunc("a"));
	ASSERT_EQ(4, memfunc("b"));
	ASSERT_EQ(8, memfunc.openscope());
	ASSERT_EQ(8, memfunc("c"));
	memfunc.closescope();
	ASSERT_EQ(8, memfunc("d"));
	ASSERT_EQ(12, memfunc("e"));
	ASSERT_EQ(16, memfunc.openscope());
	memfunc.closescope();
	ASSERT_EQ(16, memfunc.openscope());
	ASSERT_EQ(16, memfunc("b"));
	ASSERT_EQ(20, memfunc.openscope());
	ASSERT_EQ(memfunc.getnameoffset("b"), 16);
	ASSERT_EQ(20, memfunc.openscope());
	memfunc.closescope();
	memfunc.closescope();
	ASSERT_EQ(memfunc.getnameoffset("b"), 16);
	memfunc.closescope();
	ASSERT_EQ(4, memfunc.getnameoffset("b"));
}



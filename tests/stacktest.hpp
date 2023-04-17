#pragma once

#include "../modules/paracl/stack.hpp"
#include "../modules/paracl/memory_manager.hpp"

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
	ASSERT_EQ(8, memfunc.getmaxstacksize());
	ASSERT_EQ(8, memfunc.openscope());
	ASSERT_EQ(8, memfunc("c"));
	memfunc.closescope();
	ASSERT_EQ(12, memfunc.getmaxstacksize());
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
	ASSERT_EQ(20, memfunc.getmaxstacksize());
}

TEST(Stack, FunctionalTest) {
	ptree::Stack st(12);
	int x = 10;
	double y = 100;
	st.write(0, x);
	st.write(4, y);
	int xx;
	double yy;
	st.read(0, xx);
	st.read(4, yy);
	ASSERT_EQ(x, xx);
	ASSERT_EQ(y, yy);
}

TEST(Stack, MoveTest) {
	ptree::Stack st(12);
	int x = 10;
	double y = 100;
	st.write(0, x);
	st.write(4, y);
	ptree::Stack st1 = std::move(st);
	int xx;
	double yy;
	st1.read(0, xx);
	st1.read(4, yy);
	ASSERT_EQ(x, xx);
	ASSERT_EQ(y, yy);
}

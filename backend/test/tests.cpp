#include "CppUTest/TestHarness.h"

#include "../src/programmer_rpn.h"

TEST_GROUP(ProgrammerRPN) {
	programmer_rpn_t rpn;

	void setup() {
		programmer_rpn_init(&rpn);
	}
};

TEST(ProgrammerRPN, InitializeToAllZeroes) {
	CHECK_EQUAL(0, rpn.X_reg);
	CHECK_EQUAL(0, rpn.Y_reg);
	CHECK_EQUAL(0, rpn.Z_reg);
	CHECK_EQUAL(0, rpn.T_reg);
};

TEST(ProgrammerRPN, PushStack) {
	programmer_rpn_push(&rpn, 5);
	CHECK_EQUAL(5, rpn.X_reg);

	programmer_rpn_push(&rpn, 8);
	CHECK_EQUAL(8, rpn.X_reg);
	CHECK_EQUAL(5, rpn.Y_reg);
}

TEST(ProgrammerRPN, PopStack) {
	rpn.X_reg = 1;
	rpn.Y_reg = 2;
	rpn.Z_reg = 3;
	rpn.T_reg = 4;
	CHECK_EQUAL(1, programmer_rpn_pop(&rpn));
	CHECK_EQUAL(2, rpn.X_reg);
	CHECK_EQUAL(3, rpn.Y_reg);
	CHECK_EQUAL(4, rpn.Z_reg);
	CHECK_EQUAL(4, rpn.T_reg);
}
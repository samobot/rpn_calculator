#include "CppUTest/TestHarness.h"

#include "../src/programmer_rpn.h"
#include <CppUTest/UtestMacros.h>

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
	CHECK_EQUAL(PROGRAMMER_RPN_WS_64, rpn.word_size);
	CHECK_EQUAL(PROGRAMMER_RPN_2S_COMP, rpn.sign_mode);
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

TEST(ProgrammerRPN, SetWordSize) {
	rpn.X_reg = 0xBABECAFEBABECAFE;
	programmer_rpn_set_ws(&rpn, PROGRAMMER_RPN_WS_64);
	CHECK_EQUAL(0xBABECAFEBABECAFE, rpn.X_reg);
	programmer_rpn_set_ws(&rpn, PROGRAMMER_RPN_WS_32);
	CHECK_EQUAL(0xBABECAFE, rpn.X_reg);
	programmer_rpn_set_ws(&rpn, PROGRAMMER_RPN_WS_16);
	CHECK_EQUAL(0xCAFE, rpn.X_reg);
	programmer_rpn_set_ws(&rpn, PROGRAMMER_RPN_WS_8);
	CHECK_EQUAL(0xFE, rpn.X_reg);
}

TEST(ProgrammerRPN, Or) {
	programmer_rpn_set_ws(&rpn, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1010101011011010; // intentionally long registers to ensure word size is being continuously enforced
	rpn.Y_reg = 0b0101010101101000;
	rpn.Z_reg = 0xBABECAFE;
	rpn.T_reg = 0xCAFEBABE;
	programmer_rpn_or(&rpn);
	CHECK_EQUAL(0b0000000011111010, rpn.X_reg); // check correct value is outputted
	CHECK_EQUAL(0xFE, rpn.Y_reg); // check stack lift is correct
	CHECK_EQUAL(0xBE, rpn.Z_reg);
	CHECK_EQUAL(0xBE, rpn.T_reg);
}

TEST(ProgrammerRPN, OrImmediate) {
	rpn.X_reg = 0b11011010;
	programmer_rpn_ori(&rpn, 0b11111010);
	CHECK_EQUAL(0b11111010, rpn.X_reg);
}

TEST(ProgrammerRPN, Not) {
	programmer_rpn_set_ws(&rpn, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1010;
	programmer_rpn_not(&rpn);
	CHECK_EQUAL(0b11110101, rpn.X_reg);
}

TEST(ProgrammerRPN, And) {
	programmer_rpn_set_ws(&rpn, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1111111100110011;
	rpn.Y_reg = 0b1111111101010101;
	programmer_rpn_and(&rpn);
	CHECK_EQUAL(0b00010001, rpn.X_reg);
}

TEST(ProgrammerRPN, AndImmediate) {
	programmer_rpn_set_ws(&rpn, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1100110011;
	programmer_rpn_andi(&rpn, 0b1101010101);
	CHECK_EQUAL(0b00010001, rpn.X_reg);
}

TEST(ProgrammerRPN, Xor) {
	programmer_rpn_set_ws(&rpn, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1111111100110011;
	rpn.Y_reg = 0b1111111101010101;
	programmer_rpn_xor(&rpn);
	CHECK_EQUAL(0b01100110, rpn.X_reg);
}

TEST(ProgrammerRPN, XorImmediate) {
	programmer_rpn_set_ws(&rpn, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1100110011;
	programmer_rpn_xori(&rpn, 0b1101010101);
	CHECK_EQUAL(0b01100110, rpn.X_reg);
}

TEST(ProgrammerRPN, SetN) {
	programmer_rpn_set_ws(&rpn, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1100000011; // check correct bit set with extra bits in n
	rpn.Y_reg = 0b0101;
	programmer_rpn_setn(&rpn);
	CHECK_EQUAL(0b1101, rpn.X_reg);
	CHECK_EQUAL(0, rpn.Y_reg);

	rpn.X_reg = 0b00001111; // check no change with n too large
	rpn.Y_reg = 0b0101;
	programmer_rpn_setn(&rpn);
	CHECK_EQUAL(0b0101, rpn.X_reg);
	CHECK_EQUAL(0, rpn.Y_reg);	
}

TEST(ProgrammerRPN, SetNImmediate) {
	programmer_rpn_set_ws(&rpn, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1100001010;
	programmer_rpn_setni(&rpn, 5);
	programmer_rpn_setni(&rpn, 16);
	CHECK_EQUAL(0b00101010, rpn.X_reg);
}

TEST(ProgrammerRPN, ClrN) {
	programmer_rpn_set_ws(&rpn, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1100000011;
	rpn.Y_reg = 0b1111111111;
	programmer_rpn_clrn(&rpn);
	CHECK_EQUAL(0b11110111, rpn.X_reg);
	CHECK_EQUAL(0, rpn.Y_reg);
}

TEST(ProgrammerRPN, ClrNImmediate) {
	programmer_rpn_set_ws(&rpn, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1111111111;
	programmer_rpn_clrni(&rpn, 3);
	CHECK_EQUAL(0b11110111, rpn.X_reg);
}

TEST(ProgrammerRPN, RotateStack) {
	rpn.X_reg = 1;
	rpn.Y_reg = 2;
	rpn.Z_reg = 3;
	rpn.T_reg = 4;
	programmer_rpn_rotate_down(&rpn);
	CHECK_EQUAL(2, rpn.X_reg);
	CHECK_EQUAL(3, rpn.Y_reg);
	CHECK_EQUAL(4, rpn.Z_reg);
	CHECK_EQUAL(1, rpn.T_reg);
	programmer_rpn_rotate_up(&rpn);
	CHECK_EQUAL(1, rpn.X_reg);
	CHECK_EQUAL(2, rpn.Y_reg);
	CHECK_EQUAL(3, rpn.Z_reg);
	CHECK_EQUAL(4, rpn.T_reg);
}
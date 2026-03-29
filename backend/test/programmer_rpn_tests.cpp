#include "CppUTest/TestHarness.h"

#include "../src/programmer_rpn.h"
#include <CppUTest/UtestMacros.h>

TEST_GROUP(ProgrammerRPN) {
	programmer_rpn_t rpn;
	programmer_rpn_t *s = &rpn;

	void setup() {
		programmer_rpn_init(s);
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
	programmer_rpn_push(s, 5);
	CHECK_EQUAL(5, rpn.X_reg);

	programmer_rpn_push(s, 8);
	CHECK_EQUAL(8, rpn.X_reg);
	CHECK_EQUAL(5, rpn.Y_reg);
}

TEST(ProgrammerRPN, PopStack) {
	rpn.X_reg = 1;
	rpn.Y_reg = 2;
	rpn.Z_reg = 3;
	rpn.T_reg = 4;
	CHECK_EQUAL(1, programmer_rpn_pop(s));
	CHECK_EQUAL(2, rpn.X_reg);
	CHECK_EQUAL(3, rpn.Y_reg);
	CHECK_EQUAL(4, rpn.Z_reg);
	CHECK_EQUAL(4, rpn.T_reg);
}

TEST(ProgrammerRPN, SetWordSize) {
	rpn.X_reg = 0xBABECAFEBABECAFE;
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_64);
	CHECK_EQUAL(0xBABECAFEBABECAFE, rpn.X_reg);
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_32);
	CHECK_EQUAL(0xBABECAFE, rpn.X_reg);
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_16);
	CHECK_EQUAL(0xCAFE, rpn.X_reg);
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	CHECK_EQUAL(0xFE, rpn.X_reg);
}

TEST(ProgrammerRPN, Enter) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_16);
	programmer_rpn_enter(s, 0xFFBABE);
	CHECK_EQUAL(0xBABE, rpn.X_reg);

	programmer_rpn_enter(s, -5);
	CHECK_EQUAL((uint16_t)(-5), rpn.X_reg);
}

TEST(ProgrammerRPN, Or) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1010101011011010; // intentionally long registers to ensure word size is being continuously enforced
	rpn.Y_reg = 0b0101010101101000;
	rpn.Z_reg = 0xBABECAFE;
	rpn.T_reg = 0xCAFEBABE;
	programmer_rpn_or(s);
	CHECK_EQUAL(0b0000000011111010, rpn.X_reg); // check correct value is outputted
	CHECK_EQUAL(0xFE, rpn.Y_reg); // check stack lift is correct
	CHECK_EQUAL(0xBE, rpn.Z_reg);
	CHECK_EQUAL(0xBE, rpn.T_reg);
}

TEST(ProgrammerRPN, OrImmediate) {
	rpn.X_reg = 0b11011010;
	programmer_rpn_ori(s, 0b11111010);
	CHECK_EQUAL(0b11111010, rpn.X_reg);
}

TEST(ProgrammerRPN, Not) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1010;
	programmer_rpn_not(s);
	CHECK_EQUAL(0b11110101, rpn.X_reg);
}

TEST(ProgrammerRPN, And) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1111111100110011;
	rpn.Y_reg = 0b1111111101010101;
	programmer_rpn_and(s);
	CHECK_EQUAL(0b00010001, rpn.X_reg);
}

TEST(ProgrammerRPN, AndImmediate) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1100110011;
	programmer_rpn_andi(s, 0b1101010101);
	CHECK_EQUAL(0b00010001, rpn.X_reg);
}

TEST(ProgrammerRPN, Xor) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1111111100110011;
	rpn.Y_reg = 0b1111111101010101;
	programmer_rpn_xor(s);
	CHECK_EQUAL(0b01100110, rpn.X_reg);
}

TEST(ProgrammerRPN, XorImmediate) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1100110011;
	programmer_rpn_xori(s, 0b1101010101);
	CHECK_EQUAL(0b01100110, rpn.X_reg);
}

TEST(ProgrammerRPN, SetN) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1100000011; // check correct bit set with extra bits in n
	rpn.Y_reg = 0b0101;
	programmer_rpn_setn(s);
	CHECK_EQUAL(0b1101, rpn.X_reg);
	CHECK_EQUAL(0, rpn.Y_reg);

	rpn.X_reg = 0b00001111; // check no change with n too large
	rpn.Y_reg = 0b0101;
	programmer_rpn_setn(s);
	CHECK_EQUAL(0b0101, rpn.X_reg);
	CHECK_EQUAL(0, rpn.Y_reg);	
}

TEST(ProgrammerRPN, SetNImmediate) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1100001010;
	programmer_rpn_setni(s, 5);
	programmer_rpn_setni(s, 16);
	CHECK_EQUAL(0b00101010, rpn.X_reg);
}

TEST(ProgrammerRPN, ClrN) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1100000011;
	rpn.Y_reg = 0b1111111111;
	programmer_rpn_clrn(s);
	CHECK_EQUAL(0b11110111, rpn.X_reg);
	CHECK_EQUAL(0, rpn.Y_reg);
}

TEST(ProgrammerRPN, ClrNImmediate) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b1111111111;
	programmer_rpn_clrni(s, 3);
	CHECK_EQUAL(0b11110111, rpn.X_reg);
}

TEST(ProgrammerRPN, RotateStack) {
	rpn.X_reg = 1;
	rpn.Y_reg = 2;
	rpn.Z_reg = 3;
	rpn.T_reg = 4;
	programmer_rpn_rotate_down(s);
	CHECK_EQUAL(2, rpn.X_reg);
	CHECK_EQUAL(3, rpn.Y_reg);
	CHECK_EQUAL(4, rpn.Z_reg);
	CHECK_EQUAL(1, rpn.T_reg);
	programmer_rpn_rotate_up(s);
	CHECK_EQUAL(1, rpn.X_reg);
	CHECK_EQUAL(2, rpn.Y_reg);
	CHECK_EQUAL(3, rpn.Z_reg);
	CHECK_EQUAL(4, rpn.T_reg);
}

TEST(ProgrammerRPN, SwapXY) {
	rpn.X_reg = 5;
	rpn.Y_reg = 10;
	programmer_rpn_swap_XY(s);
	CHECK_EQUAL(10, rpn.X_reg);
	CHECK_EQUAL(5, rpn.Y_reg);
}

TEST(ProgrammerRPN, Clear) {
	rpn.X_reg = 5;
	rpn.Y_reg = 10;
	rpn.Z_reg = 15;
	rpn.T_reg = 20;
	programmer_rpn_clear_X(s);
	CHECK_EQUAL(0, rpn.X_reg);
	rpn.X_reg = 5;
	programmer_rpn_clear_all(s);
	CHECK_EQUAL(0, rpn.X_reg);
	CHECK_EQUAL(0, rpn.Y_reg);
	CHECK_EQUAL(0, rpn.Z_reg);
	CHECK_EQUAL(0, rpn.T_reg);
}

TEST(ProgrammerRPN, OverrideX) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 5;
	programmer_rpn_override_X(s, 0x10FE);
	CHECK_EQUAL(0xFE, rpn.X_reg);
}

TEST(ProgrammerRPN, ChangeSign) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 5;
	programmer_rpn_change_sign(s);
	CHECK_EQUAL((uint8_t)(-5), rpn.X_reg);
}

TEST(ProgrammerRPN, LeftShift) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b110000101;
	programmer_rpn_lshift(s);
	CHECK_EQUAL(10, rpn.X_reg);
}

TEST(ProgrammerRPN, RightShift) {
	rpn.X_reg = 0b00001011;
	programmer_rpn_rshift(s);
	CHECK_EQUAL(5, rpn.X_reg);
}

TEST(ProgrammerRPN, LeftShiftN) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.Y_reg = 5;
	rpn.X_reg = 0b100000010;
	programmer_rpn_lshiftn(s);
	CHECK_EQUAL(20, rpn.X_reg);
}

TEST(ProgrammerRPN, LeftShiftNImmediate) {
	rpn.X_reg = 5;
	programmer_rpn_lshiftni(s, 2);
	CHECK_EQUAL(20, rpn.X_reg);
}

TEST(ProgrammerRPN, RightShiftN) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.Y_reg = 0b00010101;
	rpn.X_reg = 0b100000010;
	programmer_rpn_rshiftn(s);
	CHECK_EQUAL(5, rpn.X_reg);
}

TEST(ProgrammerRPN, RightShiftNImmediate) {
	rpn.X_reg = 0b00010101;
	programmer_rpn_rshiftni(s, 2);
	CHECK_EQUAL(5, rpn.X_reg);
}

TEST(ProgrammerRPN, LeftRoll) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b10111010;
	programmer_rpn_lroll(s);
	CHECK_EQUAL(0b01110101, rpn.X_reg);
	programmer_rpn_lroll(s);
	CHECK_EQUAL(0b11101010, rpn.X_reg);
}

TEST(ProgrammerRPN, RightRoll) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b11101010;
	programmer_rpn_rroll(s);
	CHECK_EQUAL(0b01110101, rpn.X_reg);
	programmer_rpn_rroll(s);
	CHECK_EQUAL(0b10111010, rpn.X_reg);
}

TEST(ProgrammerRPN, LeftRollN) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 3 + 8;
	rpn.Y_reg = 0b01011011;
	programmer_rpn_lrolln(s);
	CHECK_EQUAL(0b11011010, rpn.X_reg);
}

TEST(ProgrammerRPN, LeftRollNImmediate) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b01011011;
	programmer_rpn_lrollni(s, 3+8);
	CHECK_EQUAL(0b11011010, rpn.X_reg);
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_64);
	rpn.X_reg = 0xCAFE0000AAAABABE;
	programmer_rpn_lrollni(s, 32 + 64 + 4);
	CHECK_EQUAL(0xAAABABECAFE0000A, rpn.X_reg);
}

TEST(ProgrammerRPN, RightRollN) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 3 + 8;
	rpn.Y_reg = 0b01011011;
	programmer_rpn_rrolln(s);
	CHECK_EQUAL(0b01101011, rpn.X_reg);
}

TEST(ProgrammerRPN, RightRollNImmediate) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0b01011011;
	programmer_rpn_rrollni(s, 3+8);
	CHECK_EQUAL(0b01101011, rpn.X_reg);
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_64);
	rpn.X_reg = 0xCAFE0000AAAABABE;
	programmer_rpn_rrollni(s, 32 + 64 + 4);
	CHECK_EQUAL(0x0AAAABABECAFE000, rpn.X_reg);
}

TEST(ProgrammerRPN, Add) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0xFF0000 + 10;
	rpn.Y_reg = -5; // unsigned or 1's doesn't matter for addition or subtraction since they are the same for both
	programmer_rpn_add(s);
	CHECK_EQUAL(5, rpn.X_reg);
}

TEST(ProgrammerRPN, AddImmediate) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 0xFF0000 + 10;
	programmer_rpn_addi(s, -5);
	CHECK_EQUAL(5, rpn.X_reg);
}

TEST(ProgrammerRPN, Sub) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.Y_reg = 0xFF0000 + 10;
	rpn.X_reg = -5; // unsigned or 1's doesn't matter for addition or subtraction since they are the same for both
	programmer_rpn_sub(s);
	CHECK_EQUAL(15, rpn.X_reg);
}

TEST(ProgrammerRPN, SubImmediate) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = -5;
	programmer_rpn_subi(s, 0xFF0000 + 10);
	CHECK_EQUAL((uint8_t)(-15), rpn.X_reg);
}

TEST(ProgrammerRPN, Mul) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = (uint8_t)-3;
	rpn.Y_reg = 15;
	programmer_rpn_mul(s);
	CHECK_EQUAL((uint8_t)-45, rpn.X_reg);
}

TEST(ProgrammerRPN, MulImmediate) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = (uint8_t)-3;
	programmer_rpn_muli(s, 15);
	CHECK_EQUAL((uint8_t)-45, rpn.X_reg);
}

TEST(ProgrammerRPN, Div) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = 3;
	rpn.Y_reg = (uint8_t)-45;
	programmer_rpn_div(s);
	CHECK_EQUAL((uint8_t)-15, rpn.X_reg);

	programmer_rpn_set_signmode(s, PROGRAMMER_RPN_UNSIGNED);
	rpn.X_reg = 5;
	rpn.Y_reg = 250;
	programmer_rpn_div(s);
	CHECK_EQUAL(50, rpn.X_reg);
}

TEST(ProgrammerRPN, DivImmediate) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	rpn.X_reg = (uint8_t)-45;
	programmer_rpn_divi(s, 15);
	CHECK_EQUAL((uint8_t)-3, rpn.X_reg);

	programmer_rpn_set_signmode(s, PROGRAMMER_RPN_UNSIGNED);
	rpn.X_reg = 250;
	programmer_rpn_divi(s, 5);
	CHECK_EQUAL(50, rpn.X_reg);
}

TEST(ProgrammerRPN, DoubleWordMul) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	programmer_rpn_set_signmode(s, PROGRAMMER_RPN_UNSIGNED);
	rpn.X_reg = 250;
	rpn.Y_reg = 67;
	programmer_rpn_dwmul(s);
	CHECK_EQUAL(0b01000001, rpn.Y_reg);
	CHECK_EQUAL(0b01101110, rpn.X_reg);

	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	programmer_rpn_set_signmode(s, PROGRAMMER_RPN_2S_COMP);
	rpn.X_reg = (uint8_t)-70;
	rpn.Y_reg = 67;
	programmer_rpn_dwmul(s);
	CHECK_EQUAL(0b11101101, rpn.Y_reg);
	CHECK_EQUAL(0b10101110, rpn.X_reg);
}

TEST(ProgrammerRPN, DoubleWordMulImmediate) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	programmer_rpn_set_signmode(s, PROGRAMMER_RPN_UNSIGNED);
	rpn.X_reg = 250;
	programmer_rpn_dwmuli(s, 67);
	CHECK_EQUAL(0b01000001, rpn.Y_reg);
	CHECK_EQUAL(0b01101110, rpn.X_reg);

	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_8);
	programmer_rpn_set_signmode(s, PROGRAMMER_RPN_2S_COMP);
	rpn.X_reg = (uint8_t)-70;
	programmer_rpn_dwmuli(s, 67);
	CHECK_EQUAL(0b11101101, rpn.Y_reg);
	CHECK_EQUAL(0b10101110, rpn.X_reg);
}

TEST(ProgrammerRPN, DoubleWordMul64) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_64);
	programmer_rpn_set_signmode(s, PROGRAMMER_RPN_UNSIGNED);
	//rpn.X_reg = 0b0110101111011011110110100000010110100111100100001000000010101110;
	//rpn.Y_reg = 0b0010001011011100111011111000101101000101001001000011111000001010;
	rpn.X_reg = 0x6BDBDA05A79080AE;
	rpn.Y_reg = 0x22DCEF8B45243E0A;
	programmer_rpn_dwmul(s);
	CHECK_EQUAL(0x0EB048D10B296099, rpn.Y_reg);
	CHECK_EQUAL(0x3979B64889472ACC, rpn.X_reg);


	rpn.X_reg = -789347989798098323; // 0x0AF453D088159593 positive same number
	rpn.Y_reg = 5739218739873934922; // 0x4FA5CD0946736E4A
	__int128 result = (__int128)-789347989798098323 * 5739218739873934922;
	programmer_rpn_set_signmode(s, PROGRAMMER_RPN_2S_COMP);
	programmer_rpn_dwmul(s);
	CHECK_EQUAL((uint64_t) result, rpn.X_reg);
	CHECK_EQUAL((uint64_t) (result >> 64), rpn.Y_reg);
}

TEST(ProgrammerRPN, DoubleWordMul64Immediate) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_64);
	programmer_rpn_set_signmode(s, PROGRAMMER_RPN_UNSIGNED);
	//rpn.X_reg = 0b0110101111011011110110100000010110100111100100001000000010101110;
	//rpn.Y_reg = 0b0010001011011100111011111000101101000101001001000011111000001010;
	rpn.X_reg = 0x6BDBDA05A79080AE;
	programmer_rpn_dwmuli(s, 0x22DCEF8B45243E0A);
	CHECK_EQUAL(0x0EB048D10B296099, rpn.Y_reg);
	CHECK_EQUAL(0x3979B64889472ACC, rpn.X_reg);


	rpn.X_reg = -789347989798098323; // 0x0AF453D088159593 positive same number
	rpn.Y_reg = 0xBABECAFE;
	__int128 result = (__int128)-789347989798098323 * 5739218739873934922;
	programmer_rpn_set_signmode(s, PROGRAMMER_RPN_2S_COMP);
	programmer_rpn_dwmuli(s, 5739218739873934922);
	CHECK_EQUAL((uint64_t) result, rpn.X_reg);
	CHECK_EQUAL((uint64_t) (result >> 64), rpn.Y_reg);
	CHECK_EQUAL(0xBABECAFE, rpn.Z_reg);
}

TEST(ProgrammerRPN, Mod) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_16);
	programmer_rpn_set_signmode(s, PROGRAMMER_RPN_UNSIGNED);
	rpn.X_reg = 9723;
	rpn.Y_reg = 49547;
	programmer_rpn_mod(s);
	CHECK_EQUAL(932, rpn.X_reg);

	programmer_rpn_set_signmode(s, PROGRAMMER_RPN_2S_COMP);
	rpn.X_reg = 9723;
	rpn.Y_reg = 49547;
	programmer_rpn_mod(s);
	CHECK_EQUAL(3457, rpn.X_reg);
}

TEST(ProgrammerRPN, ModImmediate) {
	programmer_rpn_set_ws(s, PROGRAMMER_RPN_WS_16);
	programmer_rpn_set_signmode(s, PROGRAMMER_RPN_UNSIGNED);
	rpn.X_reg = 49547;
	programmer_rpn_modi(s, 9723);
	CHECK_EQUAL(932, rpn.X_reg);

	programmer_rpn_set_signmode(s, PROGRAMMER_RPN_2S_COMP);
	rpn.X_reg = 49547;
	programmer_rpn_modi(s, 9723);
	CHECK_EQUAL(3457, rpn.X_reg);
}
#include "CppUTest/TestHarness.h"

#include "../src/and_or_decomp.h"
#include <CppUTest/UtestMacros.h>

TEST_GROUP(AndOrDecomp) {
	and_or_decomp_t result;
    and_or_decomp_t *ptr = &result;

	void setup() {
		result.and_p = 0;
        result.or_p = 0;
	}
};

TEST(AndOrDecomp, ZeroAsExpected) {
    and_or_decomp(ptr, 0, 0);
    CHECK_EQUAL(~0L, result.and_p);
    CHECK_EQUAL(0, result.or_p);
}

TEST(AndOrDecomp, UnmaskedBitsIgnored) {
    and_or_decomp(ptr, 0xFF, 0);
    CHECK_EQUAL(~0L, result.and_p);
    CHECK_EQUAL(0, result.or_p);
}

TEST(AndOrDecomp, ProperDecomp) {
    and_or_decomp(ptr, 0b00011000, 0b00111100);
    CHECK_EQUAL(~0L ^ 0b00100100, result.and_p);
    CHECK_EQUAL(0b00011000, result.or_p);
}

TEST(AndOrDecomp, ProperDecompWithExtraBits) {
    and_or_decomp(ptr, 0b110011001, 0b00111100);
    CHECK_EQUAL(~0L ^ 0b00100100, result.and_p);
    CHECK_EQUAL(0b00011000, result.or_p);
}

TEST(AndOrDecomp, DecompGivesCorrectResults) {
    and_or_decomp(ptr, 0b00011000, 0b00111100);

    uint64_t test_pattern = 0b10100110;
    test_pattern &= result.and_p;
    test_pattern |= result.or_p;
    CHECK_EQUAL(0b10011010, test_pattern);
}
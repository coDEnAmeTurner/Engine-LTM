#include <iostream>

float SafeFloatDivide(float A, float B, float ResultIfBIsZero);
float SafeFloatDivide_pred(float A, float B, float ResultIfBIsZero);
#ifdef TEST_PREDICATION

	void test_parallel_concurrency() {
		std::cout << SafeFloatDivide_pred(1.0f, 0.0f, 5.0f) << std::endl;
	}
#endif 
float SafeFloatDivide(float A, float B, float ResultIfBIsZero) {
	if (B == 0.0f) {
		return ResultIfBIsZero;
	}
	return A / B;
}
float SafeFloatDivide_pred(float A, float B, float ResultIfBIsZero) {
	const std::uint32_t condition = (std::uint32_t) B != 0.0f;

	const std::uint32_t mask = 0U - condition;

	const float q = A / B;	

	union Convert {
		float fVal;
		std::uint32_t uint32Val;
	};
	Convert convert, convertIf0, convertResult;
	convert.fVal = q;
	convertIf0.fVal = ResultIfBIsZero;
	
	const std::uint32_t result = (mask & convert.uint32Val) | (~mask & convertIf0.uint32Val);
	convertResult.uint32Val = result;

	return convertResult.fVal;
}

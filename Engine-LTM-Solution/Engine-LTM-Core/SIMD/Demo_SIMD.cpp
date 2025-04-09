#ifdef SIMD_DEMO
#include <xmmintrin.h>
#include <stdio.h>
#include <cassert>
#include <intrin.h>

#pragma region Some useful SSE Intrinsics
void Demo_useful_intrinsics()
{
	alignas(16) float A[4];
	alignas(16) float B[4] = { 2.0f, 4.0f, 6.0f, 8.0f };

	__m128 a = _mm_set_ps(4.0f, 3.0f, 2.0f, 1.0f);
	__m128 b = _mm_load_ps(&B[0]);

	__m128 r = _mm_add_ps(a, b);

	_mm_store_ps(&A[0], a);

	alignas(16) float R[4];
	_mm_store_ps(&R[0], r);

	printf("a = %.1f %.1f %.1f %.1f\n", A[0], A[1], A[2], A[3]);
	printf("b = %.1f %.1f %.1f %.1f\n", B[0], B[1], B[2], B[3]);
	printf("r = %.1f %.1f %.1f %.1f\n", R[0], R[1], R[2], R[3]);
}
#pragma endregion
#pragma region Vectorize Loop
void AddArrays_ref(int count, float* results, const float* dataA, const float* dataB) {
	for (int i = 0; i < count; i++) {
		results[i] = dataA[i] + dataB[i];
	}
}

void AddArrays_sse(int count, float* results, const float* dataA, const float* dataB) {
	assert(count % 4 == 0);
	
	for (int i = 0; i < count; i += 4) {
		__m128 a = _mm_load_ps(&dataA[i]);
		__m128 b = _mm_load_ps(&dataB[i]);
		__m128 r = _mm_add_ps(a, b);
		_mm_store_ps(&results[i], r);
	}
}
void DotArrays_ref(int count, float* result, const float* dataA, const float* dataB) {
	for (int i = 0; i < count; i++) {
		const int j = i * 4;

		result[i] = dataA[j + 0] * dataB[j + 0] +
			dataA[j + 1] * dataB[j + 1] +
			dataA[j + 2] * dataB[j + 2] +
			dataA[j + 3] * dataB[j + 3];
	}
}
void DotArrays_sse_horizontal(int count, float r[], const float a[], const float b[]) {
	for (int i = 0; i < count; i++)
	{
		const int j = i * 4;

		__m128 va = _mm_load_ps(&a[j]);
		__m128 vb = _mm_load_ps(&b[j]);

		__m128 v0 = _mm_mul_ps(va, vb);

		__m128 v1 = _mm_hadd_ps(v0, v0);
		__m128 v2 = _mm_hadd_ps(v1, v1);

		_mm_store_ss(&r[i], v2);
	}
}
void DotArrays_sse(int count, float* result, const float* dataA, const float* dataB) {
	//assert(count % 4 == 0);

	__m128 v1_in_a = _mm_load_ps(&dataA[(0) * 3]);
	__m128 v2_in_a = _mm_load_ps(&dataA[(1) * 3]);
	__m128 v3_in_a = _mm_load_ps(&dataA[(2) * 3]);
	__m128 v4_in_a = _mm_load_ps(&dataA[(3) * 3]);

	__m128 v1_in_b = _mm_load_ps(&dataB[(0) * 3]);
	__m128 v2_in_b = _mm_load_ps(&dataB[(1) * 3]);
	__m128 v3_in_b = _mm_load_ps(&dataB[(2) * 3]);
	__m128 v4_in_b = _mm_load_ps(&dataB[(3) * 3]);

	__m128 r;
	r = _mm_mul_ps(v1_in_a, v1_in_b);
	r = _mm_add_ps(r, _mm_mul_ps(v2_in_a, v2_in_b));
	r = _mm_add_ps(r, _mm_mul_ps(v3_in_a, v3_in_b));
	r = _mm_add_ps(r, _mm_mul_ps(v4_in_a, v4_in_b));

	_mm_store_ps(&result[0], r);
}
void DotArrays_sse_transpose(int count, float* result, const float* dataA, const float* dataB) {
	//assert(count % 4 == 0);

	__m128 v1_in_a = _mm_load_ps(&dataA[(0) * 3]);
	__m128 v2_in_a = _mm_load_ps(&dataA[(1) * 3]);
	__m128 v3_in_a = _mm_load_ps(&dataA[(2) * 3]);
	__m128 v4_in_a = _mm_load_ps(&dataA[(3) * 3]);

	__m128 v1_in_b = _mm_load_ps(&dataB[(0) * 3]);
	__m128 v2_in_b = _mm_load_ps(&dataB[(1) * 3]);
	__m128 v3_in_b = _mm_load_ps(&dataB[(2) * 3]);
	__m128 v4_in_b = _mm_load_ps(&dataB[(3) * 3]);

	_MM_TRANSPOSE4_PS(v1_in_a, v2_in_a, v3_in_a, v4_in_a);
	_MM_TRANSPOSE4_PS(v1_in_b, v2_in_b, v3_in_b, v4_in_b);

	__m128 r;
	r = _mm_mul_ps(v1_in_a, v1_in_b);
	r = _mm_add_ps(r, _mm_mul_ps(v2_in_a, v2_in_b));
	r = _mm_add_ps(r, _mm_mul_ps(v3_in_a, v3_in_b));
	r = _mm_add_ps(r, _mm_mul_ps(v4_in_a, v4_in_b));

	_mm_store_ps(&result[0], r);
}
union Mat44 {
	float c[4][4];	

	__m128 row[4];	// 4 rows
};
__m128 MulVecMat_sse(float* result,  __m128 v,  Mat44 m) {
	_MM_TRANSPOSE4_PS(m.row[0], m.row[1], m.row[2], m.row[3]);

	__m128 r = _mm_mul_ps(_mm_shuffle_ps(v, v, 0x00), m.row[0]);
	r = _mm_add_ps(r, _mm_mul_ps(_mm_shuffle_ps(v, v, 0x55), m.row[1]));
	r = _mm_add_ps(r, _mm_mul_ps(_mm_shuffle_ps(v, v, 0xaa), m.row[2]));
	r = _mm_add_ps(r, _mm_mul_ps(_mm_shuffle_ps(v, v, 0xff), m.row[3]));

	_mm_store_ps(&result[0], r);

	return r;
}	
void MulMatMat_sse(Mat44& R, Mat44 A, Mat44 B) {
	//now each row is the original column
	_MM_TRANSPOSE4_PS(A.row[0], A.row[1], A.row[2], A.row[3]);
	alignas (16) float result[4];
	R.row[0] = MulVecMat_sse(result, A.row[0], B);
	R.row[1] = MulVecMat_sse(result, A.row[1], B);
	R.row[2] = MulVecMat_sse(result, A.row[2], B);
	R.row[3] = MulVecMat_sse(result, A.row[3], B);
	_MM_TRANSPOSE4_PS(R.row[0], R.row[1], R.row[2], R.row[3]);

}
#pragma endregion
#pragma region Vector Predication For SQRT an array of floats
void sqrt_array_sse(float* _restrict_r, const float* __restrict__a, int count) {
	assert(count % 4 == 0);
	__m128 vz = _mm_set1_ps(0.0f);

	for (int i = 0; i < count; i += 4) {
		__m128 a = _mm_load_ps(&__restrict__a[i]);
		__m128 r = _mm_sqrt_ps(a);
		__m128 mask = _mm_cmpge_ps(a, vz);
		__m128 r_masked = _mm_and_ps(mask, r);
		__m128 vs_not_masked = _mm_andnot_ps(mask, vz);
		__m128 r_final = _mm_or_ps(r_masked, vs_not_masked);

		_mm_store_ps(&_restrict_r[i], r_final);

	}
}
#pragma endregion


void test_parallel_concurrency() {
	//Demo_useful_intrinsics();

	/*Mat44 mat;
	mat.c[0][0] = 1.0f; mat.c[0][1] = 2.0f; mat.c[0][2] = 3.0f; mat.c[0][3] = 4.0f;
	mat.c[1][0] = 5; mat.c[1][1] = 6; mat.c[1][2] = 7; mat.c[1][3] = 8;
	mat.c[2][0] = 9; mat.c[2][1] = 10; mat.c[2][2] = 11; mat.c[2][3] = 12;
	mat.c[3][0] = 13; mat.c[3][1] = 14; mat.c[3][2] = 15; mat.c[3][3] = 16;

	alignas(16) float result[4];
	alignas(16) float v[4] = { 1.0f, 2.0f, 3.0f, 4.0f };

	MulVecMat_sse(result, _mm_load_ps(&v[0]), mat);

	Mat44 R;
	MulMatMat_sse(R, mat, mat);*/

	float A[8] = { 1.0f, 0.0f, -3.0f, 4.0f, 1.0f, 0.0f, -3.0f, 4.0f };
	float r[8] = { 0.0f };	
	sqrt_array_sse(r, A, 8);

}
#endif
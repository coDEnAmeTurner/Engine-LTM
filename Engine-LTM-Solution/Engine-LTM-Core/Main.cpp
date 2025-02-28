#include <cstdio>
#include <iostream>

void f() {
#ifdef _DEBUG
	printf("Calling f in debug mode\n");
#else
	printf("Calling f in nondebug mode\n");
#endif
}

void log_debug() {
	printf("Log debug!!!\n");
}

void log_nondebug() {
	printf("Log nondebug!!!\n");
}

#ifdef USER_DEBUG
#define _LogInGeneral() log_debug()
#else
#define _LogInGeneral() log_nondebug()
#endif

#if ASSERTIONS_ENABLED
// define some inline assembly that causes a break
// into the debugger -- this will be different on each
// target CPU
//#define debugBreak() __asm { int 3 }
// check the expression and fail if it is false
#define ASSERT(expr) \
if (expr) {\
	printf("The expression is true\n");\
} \
else \
{ \
	printf("The expression is false\n");\
	__debugbreak(); \
}
#else
#define ASSERT(expr) // evaluates to nothing
#endif

struct NeedsToBe128Bytes
{
	uint32_t m_a;
	uint32_t m_b;
	uint32_t m_c;
	uint32_t m_d;
	uint32_t m_e;
	uint32_t m_f;
	uint32_t m_g;
	uint32_t m_h;
	uint32_t m_i;
	uint32_t m_k;
	uint32_t m_l;
	uint32_t m_m;
	uint32_t m_n;
	uint32_t m_o;
	uint32_t m_p;
	uint32_t m_q;
	uint32_t m_r;
	uint32_t m_s;
	uint32_t m_t;
	uint32_t m_u;
	uint32_t m_v;
	uint32_t m_w;
	uint32_t m_x;
	uint32_t m_y;
	uint32_t m_z;
	uint32_t m_1;
	uint32_t m_2;
	uint32_t m_3;
	uint32_t m_5;
	uint32_t m_6;
	uint32_t m_7;
	uint32_t m_8;
	// etc.
};

#define _ASSERT_GLUE(a, b) a ## b
#define ASSERT_GLUE(a, b) _ASSERT_GLUE(a, b)
#define STATIC_ASSERT(expr) \
	enum \
	{ \
		ASSERT_GLUE(g_assert_fail_, __LINE__) \
		= 1 / (int)(!!(expr)) \
	}

int main() {
	f();
	_LogInGeneral();
	ASSERT(5 != 5);
	STATIC_ASSERT(sizeof(NeedsToBe128Bytes) == 128, "Wrong size");
	system("pause");
}
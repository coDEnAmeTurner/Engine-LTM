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

int main() {
	f();
	_LogInGeneral();
	system("pause");
}
#include <iostream>
#include "MyLibrary_DLL.h"
#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        std::cout << "DLL loaded (process attach)" << std::endl;
        break;
    case DLL_PROCESS_DETACH:
        std::cout << "DLL unloaded (process detach)" << std::endl;
        break;
    case DLL_THREAD_ATTACH:
        std::cout << "Thread attach" << std::endl;
        break;
    case DLL_THREAD_DETACH:
        std::cout << "Thread detach" << std::endl;
        break;
    }
    return TRUE;
}

void PrintMessage_DLL() {
	std::cout << "Hello from DLL" << std::endl;
}
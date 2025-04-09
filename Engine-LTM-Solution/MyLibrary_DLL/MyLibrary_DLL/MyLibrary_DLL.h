#pragma once

#ifdef MYLIBRARY_EXPORTS
#define MYLIBRARY_API _declspec(dllexport)
#else
#define MYLIBRARY_API _declspec(dllimport)
#endif

extern "C" MYLIBRARY_API void PrintMessage_DLL();
#pragma once
#include <Windows.h>

#ifndef NDEBUG    
    // CWE-134 - PRevent usage of dbg_printf with the passing of the format to this func call
    void dbg_printf(const wchar_t* fmt, ...);
    #define DbgPrintErr(args, ...) do { \
    DWORD errorCode = GetLastError(); \
    char err[256] = { 0 }; \
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, \
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, NULL); \
    dbg_printf(L"Status: %S  in File: \"%S\"\n  Line: %d Function Name: %S\n  [%d] Message:  ", err, __FILE__, __LINE__, __func__,  GetCurrentThreadId()); \
    dbg_printf(args, __VA_ARGS__); \
    dbg_printf(L"\n"); \
    } while (0)

    // Used for memory leak detection 
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>

#else
    #define DbgPrintErr(args, ...) (void)0
#endif

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);

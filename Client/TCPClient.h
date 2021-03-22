#pragma once

#include <windows.h>


#ifndef NDEBUG
void dbg_printf(const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfwprintf(stderr, fmt, args);
    va_end(args);
}
#define DbgPrintErr(args, ...) do { \
        DWORD errorCode = GetLastError(); \
        char err[256] = { 0 }; \
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, \
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, NULL); \
        dbg_printf(L"Status: %S in File: \"%S\"\n Line: %d Function Name: %S\n Message: ", err, __FILE__, __LINE__, __func__);\
        dbg_printf(args, __VA_ARGS__); \
        } while (0)

// Used for memory leak detection 
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define SETDBGFLAG() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)

#else
// TODO: Create log file and log to a file location 
#define SETDBGFLAG() 

#define DbgPrintErr(args, ...) (void)0
#endif

void printError(BOOL socketError, IN OPTIONAL PWCHAR fmtMsg, IN OPTIONAL DWORD errorCode);
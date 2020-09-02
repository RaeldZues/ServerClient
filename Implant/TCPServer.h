#pragma once

#include <WinSock2.h>
#include "ErrorHandler.h"

// This isnt 32 bit compliant, doesnt convert properly
typedef BOOL (WINAPI* ProcessData)(PSERVERPARAM);

typedef struct _SERVERPARAM {
    SOCKET ClientSock;         // Client socket for return data 
    // Future usages for locking data while processing 
    PHANDLE ClientDataMutex;   // Mutex - Currently unused 
    WSABUF* ClientData;        // WSABUF for data storage 
    ProcessData ProcessBuffer; // Function ptr to process the data recieved      
    // TODO: Note this can be determined in line as well which might need to change 
    BOOL WaitForLargeBuffer;   // Should the server recieve and reallocate a large buffer 
    BOOL FinishedRecv;         // Used to determine if hte client has finished with the connection
    BOOL TerminateConnection;  // Used to determine if the server shuts down the connectin
    BOOL SendResponse;         // Should the server send a response for the processed data?
} SERVERPARAM, *PSERVERPARAM;

DWORD WINAPI HandleConnection(LPVOID serverParam);
SOCKET ServerSocketInit(char* ip, char* port);
PSERVERPARAM ServerDataInit(BOOL SendResponse, BOOL WaitForLargeBuffer, ProcessData func, SOCKET client_sock);
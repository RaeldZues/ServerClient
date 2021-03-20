#pragma once

#include <WinSock2.h>
#include "ErrorHandler.h"

// ALERT: This isnt 32 bit compliant, doesnt convert properly
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

/// <summary>
/// Recvs data from client specified by the socket into the buffer 
/// </summary>
/// <param name="serverParam">- Parameter as a PSERVERPARAM</param>
/// <returns></returns>
DWORD WINAPI HandleConnection(LPVOID serverParam);

/// <summary>
/// Initialize a server socket on a port and ip provided 
/// <para>Note: Currently supports hard coded setup and not dynamic</para>
/// </summary>
/// <param name="ip">- <para>Currently only accepting IPv4 addresses</para></param>
/// <param name="port">- Port to Bind to</param>
/// <returns>INVALID_SOCKET on error</returns>
SOCKET ServerSocketInit(char* ip, char* port);

/// <summary>
/// Initializes a server data parameter structure for use 
/// </summary>
/// <param name="SendResponse"> - Do you require a response sent to the connecting client</param>
/// <param name="WaitForLargeBuffer">: 
///  <param> False - individual sends under 1024 size, True - Similar to recvall function
///  </param>
/// </param>
/// <param name="func">ProcessData Function pointer</param>
/// <param name="client_sock"></param>
/// <returns>NULL on error</returns>
PSERVERPARAM ServerDataInit(BOOL SendResponse, BOOL WaitForLargeBuffer, ProcessData func, SOCKET client_sock);
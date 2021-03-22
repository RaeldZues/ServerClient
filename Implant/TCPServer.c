#pragma comment(lib, "ws2_32.lib")
 // Do this or add Ws2_32.lib to additional dependencies by:
 // Properties -> Configuration Properties -> Linker -> Input ->
 // Additional Dependencies
#include "TCPServer.h"
#include <ws2tcpip.h>
#include <stdio.h>

#define BUF_SIZE 1024
/// <summary>
/// Initialize wsa 
/// </summary>
static void InitWSA()
{
    WSADATA wsaData = { 0 };
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        DbgPrintErr(L"Failed to init WSA\n");
        ExitProcess(EXIT_FAILURE);
    }
}

SOCKET ServerSocketInit(char* ip, char *port)
{
    struct addrinfo *result = NULL;
    struct addrinfo hints;    
    SOCKET ListenSocket = INVALID_SOCKET;
    int rc = 0;
    DbgPrintErr(L"Starting initServer\n");
    // Load Winsock
    InitWSA();
    // Make sure the hints struct is zeroed out
    SecureZeroMemory((PVOID) & hints, sizeof(struct addrinfo));
    // Initialize the hints to obtain the 
    // wildcard bind address for IPv4
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    rc = getaddrinfo(ip, port, &hints, &result);
    if (rc != 0) 
    {
        DbgPrintErr(L"getaddrinfo failed with error: %d\n", rc);
        return INVALID_SOCKET;
    }
    ListenSocket = WSASocketW(result->ai_family, result->ai_socktype, result->ai_protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (ListenSocket == INVALID_SOCKET)
    {
        DbgPrintErr(L"socket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        return INVALID_SOCKET;
    }    
    int optval = 1;
    rc = setsockopt(ListenSocket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char *)&optval,sizeof(optval));
    if (rc != 0)
    {
        DbgPrintErr(L"Failed to set socket options\n");
        return INVALID_SOCKET;
    }
    
    rc = bind(ListenSocket, result->ai_addr, (int) result->ai_addrlen);
    if (rc == SOCKET_ERROR) {
        DbgPrintErr(L"bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        return INVALID_SOCKET;
    }

    rc = listen(ListenSocket, SOMAXCONN);
    if (rc == SOCKET_ERROR)
    {
        DbgPrintErr(L"listen failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        return INVALID_SOCKET;
    }
    DbgPrintErr(L"Returning Socket\n");
    freeaddrinfo(result);
    return ListenSocket;
}

PSERVERPARAM ServerDataInit(BOOL SendResponse, BOOL WaitForLargeBuffer, ProcessData func, SOCKET client_sock)
{
    if (INVALID_SOCKET == client_sock) return NULL;
    PSERVERPARAM ServerData = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(SERVERPARAM));
    if (!ServerData) return NULL;
    ServerData->ClientSock = client_sock;
    // TODO: Use critical section vs mutex 
    HANDLE hMutex = CreateMutexA(NULL, TRUE, NULL);
    if (!hMutex)
    {
        HeapFree(GetProcessHeap(), 0, ServerData);
        return NULL;
    }
    ServerData->ClientDataMutex = hMutex;
    // Intentionally not allocated for buffer in the recv func 
    WSABUF *recvData = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WSABUF));
    if (!recvData)
    {
        CloseHandle(ServerData->ClientDataMutex);
        HeapFree(GetProcessHeap(), 0, ServerData);
        return NULL;
    }        
    ServerData->ClientData = recvData;
    ServerData->ProcessBuffer = func;
    ServerData->WaitForLargeBuffer = WaitForLargeBuffer;
    ServerData->FinishedRecv = FALSE;
    ServerData->TerminateConnection = FALSE;
    ServerData->SendResponse = SendResponse;
    return ServerData;
}

/// <summary>
/// Destructor for the full serverparam structure including all nested structures and buffers
/// </summary>
/// <param name="ServerData"></param>
/// <returns>TRUE if completly successfull, FALSE if not</returns>
BOOL ServerDataDestroy(PSERVERPARAM ServerData)
{
    BOOL retVal = TRUE;
    if (ServerData)
    {
        if (INVALID_SOCKET != ServerData->ClientSock)
            if (0 != shutdown(ServerData->ClientSock, SD_BOTH))
                retVal = FALSE;
            if (0 != closesocket(ServerData->ClientSock))
                retVal = FALSE;
        if (ServerData->ClientDataMutex && 0 == CloseHandle(ServerData->ClientDataMutex))
            retVal = FALSE;
        if (NULL != ServerData->ClientData && ServerData->ClientData->buf)
            if (0 == HeapFree(GetProcessHeap(), 0, ServerData->ClientData->buf))
                retVal = FALSE;
        if (NULL != ServerData->ClientData)
            if (0 == HeapFree(GetProcessHeap(), 0, ServerData->ClientData))
                retVal = FALSE;
        if (0 == HeapFree(GetProcessHeap(), 0, ServerData))
            retVal = FALSE;
   
    }
    DbgPrintErr(L"Cleanup of the server data was %d", retVal);
    return retVal;
}

DWORD WINAPI HandleConnection(LPVOID serverParam)
{    
    // Input validation 
    // TODO: Add better input validation after casting
    PSERVERPARAM ServerData = (PSERVERPARAM)serverParam;
    if (ServerData->ClientSock == SOCKET_ERROR)
    {
        DbgPrintErr(L"Invalid socket\n");
        return EXIT_FAILURE;
    }
    // Local variable established
    DWORD tid = GetCurrentThreadId();

    DWORD retVal = EXIT_SUCCESS;
    DWORD BytesRecved = 0;
    DWORD CurrentBytes = 0;
    DWORD flags = 0;

    WSAOVERLAPPED RecvOverlapped;
    DbgPrintErr(L"Started new thread\n", tid);
    // Clean buffer for first use 
    LONG CurrSize = 2048;
    char *buffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, CurrSize);
    if (!buffer)
    {
        DbgPrintErr(L"Failed to heap allocate buffer\n");
        return EXIT_FAILURE;
    }
        
    // Make sure the RecvOverlapped struct is zeroed out
    SecureZeroMemory((PVOID)&RecvOverlapped, sizeof(WSAOVERLAPPED));

    // Create an event handle and setup an overlapped structure.
    RecvOverlapped.hEvent = WSACreateEvent();
    if (RecvOverlapped.hEvent == NULL) {
        wprintf(L"WSACreateEvent failed: %d\n", WSAGetLastError());
        closesocket(ServerData->ClientSock);
        return EXIT_FAILURE;
    }

    // Loop until the server says to shut down
    while (FALSE == ServerData->TerminateConnection && retVal == EXIT_SUCCESS)
    {
        // Get data - Loop until the client is done 
        // Set by callee as a controlling method based on data 
        while (FALSE == ServerData->FinishedRecv)
        {            
            // Local buffer used for recv into 
            char buff[BUF_SIZE] = { 0 };            
            WSABUF localwsa = { 0 };
            localwsa.buf = buff;
            localwsa.len = BUF_SIZE;

            // Recv Data - Currently using a blocking socket 
            int error = 0;
            int rc = WSARecv(ServerData->ClientSock, &localwsa, 1, &BytesRecved, &flags, &RecvOverlapped, NULL);
            // Break if theres an error recieving, terminate connection hard 
            if (rc == SOCKET_ERROR && (WSA_IO_PENDING != (error = WSAGetLastError())))
            {
                DbgPrintErr(L"\tUnable to receive data on thread\n");
                retVal = SOCKET_ERROR;
                ServerData->TerminateConnection = TRUE;
                break;
            }

            // TODO: what happens if theres more data than i can hold?
            // And how to check for that and handle it?

            // Not sure I need to use this or not as a whole. 
            {
                // Overlapped wait event 
                rc = WSAWaitForMultipleEvents(1, &RecvOverlapped.hEvent, TRUE, INFINITE, TRUE);
                if (rc == WSA_WAIT_FAILED) {
                    wprintf(L"WSAWaitForMultipleEvents failed with error: %d\n", WSAGetLastError());
                    break;
                }
                // Get result from the overlapped event 
                rc = WSAGetOverlappedResult(ServerData->ClientSock, &RecvOverlapped, &BytesRecved, FALSE, &flags);
                if (rc == FALSE) {
                    wprintf(L"WSARecv operation failed with error: %d\n", WSAGetLastError());
                    break;
                }
                WSAResetEvent(RecvOverlapped.hEvent);
            }

            LONG total = BytesRecved + CurrentBytes;
            // Break if no data has been sent 
            if (BytesRecved == 0)
            {
                DbgPrintErr(L"No bytes received\n", tid);
                ServerData->TerminateConnection = TRUE;
                break;
            }

            // Handling overflows 
            // If what i recieved total is less than my overall buffer, shove into buffer 
            if (total > CurrSize)
            {
                DbgPrintErr(L"Reallocating Buffer size\n");
                char* tmp = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, buffer, (SIZE_T)CurrSize * 2);
                if (NULL == tmp)
                {
                    DbgPrintErr(L"ZOMG I rans out o mems!\n");
                    // TODO handle clean exit from error for thread 
                    // 1.6 GB dies 
                    ServerData->TerminateConnection = TRUE;
                    retVal = SOCKET_ERROR;
                    break;
                }
                CurrSize *= 2;
                DbgPrintErr(L"ZOMG I got ze memories %d currsize\n", CurrSize);
                buffer = tmp;
            }

            // Copy to local collection buffer 
            CopyMemory(buffer + CurrentBytes, localwsa.buf, localwsa.len);
            CurrentBytes += BytesRecved;

            // Process data on a per recv basis
            // Note for usage: 
            //     If you fail to set the WaitForLargeBuffer to true in APP, this will inf loop until connection drop
            if (NULL != ServerData->ProcessBuffer && FALSE == ServerData->WaitForLargeBuffer)
            {
                // Set buffer 
                ServerData->ClientData->buf = buffer;
                ServerData->ClientData->len = CurrentBytes;
                DbgPrintErr(L"Processing SINGLE recv data with the function.\n  Data   : %S\n", ServerData->ClientData->buf);
                if (TRUE != ServerData->ProcessBuffer(ServerData))
                    retVal = EXIT_FAILURE;
                if (FALSE == ServerData->WaitForLargeBuffer)
                    CurrentBytes = 0;
            }

            // Send Response on a per recv basis 
            if (TRUE == ServerData->SendResponse && FALSE == ServerData->WaitForLargeBuffer)
            {
                DWORD BytesSent = 0;
                flags = 0;
                error = WSASend(ServerData->ClientSock, ServerData->ClientData, 1, &BytesSent, flags, NULL, NULL);
                if (SOCKET_ERROR == error)
                {
                    DbgPrintErr(L"WSA Send\n");
                    retVal = SOCKET_ERROR;
                    break;
                }
                DbgPrintErr(L"Server responded individual send with %S total bytes: %d\n", ServerData->ClientData->buf, ServerData->ClientData->len);
            }            
        }
        // Process data all in one 
        // Used for larger data being sent. 
        if (NULL != ServerData->ProcessBuffer && TRUE == ServerData->WaitForLargeBuffer)
        {
            // Set buffer 
            ServerData->ClientData->buf = buffer;
            ServerData->ClientData->len = CurrentBytes;
            DbgPrintErr(L"Processing FULL data with the function. Data: %S\n", ServerData->ClientData->buf);
            if (TRUE != ServerData->ProcessBuffer(ServerData))
                retVal = EXIT_FAILURE;
        }
        // Send Response fop all data 
        if (TRUE == ServerData->SendResponse && TRUE == ServerData->WaitForLargeBuffer)
        {
            DWORD BytesSent = 0;
            flags = 0;
            int error = WSASend(ServerData->ClientSock, ServerData->ClientData, 1, &BytesSent, flags, NULL, NULL);
            if (SOCKET_ERROR == error)
            {
                DbgPrintErr(L"WSA Send: %s\n");
                retVal = SOCKET_ERROR;
                break;
            }
            DbgPrintErr(L"Server responded to all data with %S total bytes: %d\n", ServerData->ClientData->buf, ServerData->ClientData->len);
        }
        ServerData->TerminateConnection = TRUE;
    }
    WSACloseEvent(RecvOverlapped.hEvent);
    // TODO: Move this out of the handle connection due to Single Resp Princ.
    ServerDataDestroy(ServerData);
    DbgPrintErr(L"Shutting down client socket\n");
    
    return retVal;
}
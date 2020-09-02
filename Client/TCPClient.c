#pragma comment(lib, "ws2_32.lib")
 // Do this or add Ws2_32.lib to additional dependencies by:
 // Properties -> Configuration Properties -> Linker -> Input ->
 // Additional Dependencies

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
// Late include due to ordering of winsock requierement 
#include "TCPClient.h"

 // Used for memory leak id 
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define BUF_SIZE 1024
#define SERVER_PORT 8080
#define MAX_THREADS 1000
#define SERVER_PORTSTR "8080"
#define SERVER_IP "127.0.0.1"


/// <summary>
/// Prints the error string based on getlasterror
/// </summary>
/// <param name="socketError"> Boolean for if its a socket issue</param>
/// <param name="fmtMsg">Format String option NOTE: Must end with %s</param>
/// <param name="errorCode">Return code from a function call to pass as the error code</param>
void printError(BOOL socketError, IN OPTIONAL PWCHAR fmtMsg, IN OPTIONAL DWORD errorCode)
{
    if (fmtMsg == NULL)
        fmtMsg = L"%s\n";

    wchar_t err[256] = { 0 };
    if (errorCode == 0 || !socketError)
        errorCode = GetLastError();
    else if (socketError)
        errorCode = WSAGetLastError();
    
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, NULL);
    wprintf(fmtMsg, err);
}

/// <summary>
/// Initialize wsa
/// </summary>
void InitWSA()
{
    // TODO: Call WSAStartup()
    WSADATA wsaData = { 0 };
    if (0 != WSAStartup(MAKEWORD(2, 0), &wsaData))
    {
        printError(TRUE, L"Unable to init WSA: %s\n", 0);
        exit(1);
    }
}

/**
 * Gives the user a socket to send messages to the server at the specified
 * IP address and port
 * @returns SOCKET socket to connect to server
 */
SOCKET connectToServer(const char* ip, const char* port)
{
    SOCKET sock = INVALID_SOCKET;
    //WSAOVERLAPPED SendOverlapped;
    int iResult = 0;
    BOOL bOptVal = FALSE;
    int bOptLen = sizeof(BOOL);
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;

    SecureZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    
    iResult = getaddrinfo(ip, port, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        return 1;
    }
    // Head of list saving 
    ptr = result;
    // Iterate through list of results 
    do 
    {
        // Get socket 
        sock = WSASocketW(result->ai_family, result->ai_socktype, result->ai_protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
        if (INVALID_SOCKET  == sock)
        {
            printf("Bad socket\n");
            continue;
        }
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&bOptVal, bOptLen);
        // Attempt to connect 
        iResult = WSAConnect(sock, result->ai_addr, result->ai_addrlen, NULL, NULL, NULL, NULL);
        if (iResult == SOCKET_ERROR)
        {
            DbgPrintErr(L"Unable to wsaconnect %s\n", iResult);
            closesocket(sock);
            sock = INVALID_SOCKET;
            continue;
        }
        printf("CONNECTED FINALLY\n");
        break;
    }while (result = result->ai_next);

    if (sock == INVALID_SOCKET)
        printf("UNABLE TO CREATE VALID SOCKET AND CONNECT\n");
    // Validation of socket on caller function
    freeaddrinfo(ptr);
    return sock;
}

DWORD WINAPI SendData(SOCKET sock, LPVOID param, size_t size, BOOL stayalive)
{
    //Connect to server 
    DWORD tid = GetCurrentThreadId();
    WSABUF dataBuf = { 0 };
    DWORD bytes = 0;
    DWORD flags = 0;
    dataBuf.buf = (char *)param;
    // TODO: Conversion Error
    dataBuf.len = size;
    
    // Send data with no overlapping function 
    // TODO: Work on overlapped IO
    int err = WSASend(sock, &dataBuf, 1, &bytes, flags, NULL, NULL);
    if (SOCKET_ERROR == err)
    {
        printError(TRUE, L"WSA Send: %s\n", 0);
        return SOCKET_ERROR;
    }
    printf("\t[%d] Supposedly sent: %s\n", tid, dataBuf.buf);
    if (!stayalive)
        closesocket(sock);
    return ERROR_SUCCESS;
}

DWORD WINAPI RecvData(SOCKET sock, WSABUF *data, BOOL stayalive)
{
    if (sock == INVALID_SOCKET || !data)
    {
        printError(FALSE, L"Invalid parameter passed to ThreadedRecvData: %s\n", 0);
        return ERROR_INVALID_PARAMETER;
    }
    DWORD RecvBytes = 0;
    DWORD Flags = 0;
    DWORD err = 0;
    data->buf = (char *)calloc(1, sizeof(char) * BUF_SIZE);
    if (!data->buf)
    {
        printError(FALSE, L"Invalid allocation of memory for data buffer: %s\n", 0);
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    data->len = BUF_SIZE;
    int rc = WSARecv(sock, data, 1, &RecvBytes, &Flags, NULL, NULL);
    if ((rc == SOCKET_ERROR) && (WSA_IO_PENDING != (err = WSAGetLastError())))
        printError(TRUE, L"WSA Recv call: %s\n", err);
    if (!stayalive)
        closesocket(sock);
    return ERROR_SUCCESS;
}

int __cdecl main(void)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    DWORD bytesRecv = 0;
    DWORD tid = 0;
    SOCKET sock = INVALID_SOCKET;
    DWORD ret = 0;    
    HANDLE threads[MAX_THREADS] = { 0 };
    WSABUF recvd = { 0 };
    // TODO: Use SOCKET, WSABUF, InitWSA(), connectToServer(), WSASend(),
    // WSARecv(), closesocket(), and WSACleanup()
    InitWSA();
    
    int i = 0;
    sock = connectToServer("127.0.0.1", "8080");
    char* data = "Hello world\n";
    SendData(sock, data, strlen(data) , TRUE);
    RecvData(sock, &recvd, TRUE);
    printf("%s\n", recvd.buf);
    closesocket(sock);
    Sleep(500);
    WSACleanup();
    return ERROR_SUCCESS;
}

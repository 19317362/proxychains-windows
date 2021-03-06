﻿#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <locale.h>
#include <strsafe.h>
#include <iostream>

DWORD WINAPI ThreadFunc(LPVOID lpVoid)
{
    HeapLock(GetProcessHeap());
    printf("in thread");
    HeapUnlock(GetProcessHeap());
    return 0;
}

#pragma comment(lib, "Ws2_32.lib")
int main()
{
    setlocale(LC_ALL, "");

    WSADATA wsaData;

    wprintf(L"%p\n", OutputDebugStringA);

    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);     // Initialize Winsock 2.2
    if (iResult != 0) {
        fwprintf(stderr, L"WSAStartup() failed: %d\n", iResult);
        return 1;
    }

    if (0)
    {
        //char ipstrbuf[100] = "192.168.1.1:65535";
        char ipstrbuf[100] = "[2005::1]:1";
        char ipstrbuf2[100] = { 0 };
        DWORD ipstrbuflen = _countof(ipstrbuf2);
        struct sockaddr_storage s;
        ADDRINFOW RequeryAddrInfoHints;
        ADDRINFOW* pRequeryAddrInfo = NULL;

        ZeroMemory(&RequeryAddrInfoHints, sizeof(RequeryAddrInfoHints));
        RequeryAddrInfoHints.ai_family = AF_UNSPEC;
        RequeryAddrInfoHints.ai_protocol = IPPROTO_TCP;
        RequeryAddrInfoHints.ai_socktype = SOCK_STREAM;
        RequeryAddrInfoHints.ai_flags = AI_NUMERICHOST;

        int addrlen = sizeof(s);

        wprintf(L"%d\n", WSAStringToAddressA(ipstrbuf, AF_INET6, NULL, (LPSOCKADDR)&s, &addrlen));
        wprintf(L"%d\n", WSAAddressToStringA((LPSOCKADDR)&s, addrlen, NULL, ipstrbuf2, &ipstrbuflen));
        wprintf(L"%S\n", ipstrbuf2);
    }

    DWORD dwLen;
    WCHAR g_HostPrintBuf[400];
    dwLen = _countof(g_HostPrintBuf);
    g_HostPrintBuf[0] = L'\0';
    int len = sizeof(SOCKADDR);

    struct addrinfo hints;
    ::ZeroMemory(&hints, sizeof(hints));
    
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct hostent* pH;
    WCHAR szIp[100];

    pH = gethostbyname("www.baidu.com");
    wprintf(L"gethostbyname(): addrtype=%hx name=%S(%hu)\n", pH->h_addrtype, pH->h_name, pH->h_length);
    wprintf(L" aliases:\n");
    for (char** pA = pH->h_aliases; *pA; pA++) {
        wprintf(L"   %S\n", *pA);
    }
    wprintf(L" addrs:\n");
    for (char** pA = pH->h_addr_list; *pA; pA++) {
        //WSAAddressToStringW((LPSOCKADDR)*pA, sizeof(SOCKADDR), NULL, szIp, &cchIp);
        InetNtopW(AF_INET, *pA, szIp, _countof(szIp));
        wprintf(L"   %ls\n", szIp);
    }

    pH = gethostbyname("openwrt.reserved");
    wprintf(L"gethostbyname(openwrt.reserved): addrtype=%hx name=%S(%hu)\n", pH->h_addrtype, pH->h_name, pH->h_length);
    wprintf(L" aliases:\n");
    for (char** pA = pH->h_aliases; *pA; pA++) {
        wprintf(L"   %S\n", *pA);
    }
    wprintf(L" addrs:\n");
    for (char** pA = pH->h_addr_list; *pA; pA++) {
        //WSAAddressToStringW((LPSOCKADDR)*pA, sizeof(SOCKADDR), NULL, szIp, &cchIp);
        InetNtopW(AF_INET, *pA, szIp, _countof(szIp));
        wprintf(L"   %ls\n", szIp);
    }

    ADDRINFOW* addrsResult;
    iResult = GetAddrInfoW(L"ip.sb", L"80", NULL, &addrsResult);
    if (iResult != 0) {
        fprintf(stderr, "getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    WCHAR ipstrbuf[100];
    DWORD ipstrbuflen = _countof(ipstrbuf);
    int iaddr = 0;
    struct sockaddr_storage_xp xxxx;
    int ilen = sizeof(xxxx);
    for (ADDRINFOW* a = addrsResult; a; a = a->ai_next, iaddr++) {
        WSAAddressToStringW(a->ai_addr, (DWORD)a->ai_addrlen, NULL, ipstrbuf, &ipstrbuflen);
        
        wprintf(L"addrs[%d]\naddr: %ls\naddrlen: %u\ncanonname: %ls\nfamily: %d\nflags: %d\nprotocol: %d\nsocktype: %d\n\n", iaddr, ipstrbuf, (unsigned)a->ai_addrlen, a->ai_canonname, a->ai_family, a->ai_flags, a->ai_protocol, a->ai_socktype);
    }

    if (0) {
        int cbIn;
        struct sockaddr in;
        cbIn = sizeof(in);
        dwLen = _countof(g_HostPrintBuf);
        wprintf(L"%d\n", WSAStringToAddressW((WCHAR*)L"127", AF_INET, NULL, &in, &cbIn));
        wprintf(L"%d\n", WSAAddressToStringW(&in, sizeof(in), NULL, g_HostPrintBuf, &dwLen));
        wprintf(L"%ls\n", g_HostPrintBuf);
    }

    SOCKET ConnectSocket = INVALID_SOCKET;
    ADDRINFOW& firstAddr = addrsResult[0];
    ConnectSocket = socket(firstAddr.ai_family, firstAddr.ai_socktype, firstAddr.ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        fprintf(stderr, "Error at socket(): %ld\n", WSAGetLastError());
        FreeAddrInfoW(addrsResult);
        WSACleanup();
        return 1;
    }

    iResult = connect(ConnectSocket, firstAddr.ai_addr, (int)firstAddr.ai_addrlen);
    printf("%d %u %u\n", iResult, WSAGetLastError(), GetLastError());
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }


    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(ConnectSocket, &fds);

    printf("Waiting...\n");
    iResult = select(-1, NULL, &fds, NULL, NULL);
    printf("Waiting done. %d, %u\n", iResult, WSAGetLastError());

    FreeAddrInfoW(addrsResult);

    if (ConnectSocket == INVALID_SOCKET) {
        fprintf(stderr, "Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    char httpReqBuf[] = "GET / HTTP/1.1\r\nHost: ip.sb\r\nUser-Agent: curl/7.66.0\r\nAccept: */*\r\n\r\n";
    const int RECEIVE_CSTR_LEN = 1024;
    char httpRespBuf[RECEIVE_CSTR_LEN + 1];

    iResult = send(ConnectSocket, httpReqBuf, sizeof(httpReqBuf) - sizeof(char), 0);
    if (iResult == SOCKET_ERROR) {
        fprintf(stderr, "send() failed: %ld\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    fprintf(stderr, "Bytes sent: %ld\n", iResult);

    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        fprintf(stderr, "shutdown() failed: %ld\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    do {
        iResult = recv(ConnectSocket, httpRespBuf, RECEIVE_CSTR_LEN, 0);
        if (iResult > 0) {
            fprintf(stderr, "Bytes received: %ld\n", iResult);
            httpRespBuf[iResult] = '\0';
            printf(httpRespBuf);
            fflush(stdout);
        }
        else if (iResult == 0)
            fprintf(stderr, "Connection closed\n");
        else
            fprintf(stderr, "recv() failed: %ld\n", WSAGetLastError());
    } while (iResult > 0);

    iResult = shutdown(ConnectSocket, SD_RECEIVE);
    if (iResult == SOCKET_ERROR) {
        fprintf(stderr, "shutdown() failed: %ld\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ConnectSocket);
    WSACleanup();

    Sleep(INFINITE);
    return 0;

}

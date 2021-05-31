#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h> 

#define BUF_SIZE 100
#define NAME_SIZE 20
#define MAX_CLNT 256
#pragma comment(lib, "ws2_32.lib")

unsigned WINAPI HandleClnt(void* arg);
void SendMsg(char* msg, int len);
void ErrorHandling(char* msg);

int clntCnt = 0;
SOCKET clntSocks[MAX_CLNT];
HANDLE hMutex;

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAdr, clntAdr;
    int clntAdrSz, clntcnt,i,j,n, numcnt=0, oldnum = 0;
    char buf[BUF_SIZE];
    char** name;
    int strlen;
    HANDLE  hThread;
    char gauge[] = "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO";
    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hMutex = CreateMutex(NULL, FALSE, NULL);
    hServSock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(atoi(argv[1]));

    if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");
    if (listen(hServSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen() error");

    printf("======================================= \n");
    printf("배스킨라빈스 31게임을 시작하겠습니다 !! \n");
    printf("몇 명이서 플레이 하겠습니까?\n");
    printf(">> ");
    scanf("%d", &clntcnt);
    name = malloc(sizeof(char*) * clntcnt);
    for (i = 0; i < clntcnt; i++) {
        name[i] = malloc(NAME_SIZE);
    }
    for (i = 0; i < clntcnt; i++) {
        clntAdrSz = sizeof(clntAdr);
        hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSz);
        
        WaitForSingleObject(hMutex, INFINITE);
        clntSocks[clntCnt++] = hClntSock;
        ReleaseMutex(hMutex);

        //hThread =(HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void*)&hClntSock, 0, NULL);
        strlen = recv(hClntSock, buf, NAME_SIZE, 0);
        sprintf(name[i], "%s", buf);
        send(hClntSock, &i, sizeof(int), 0);
        printf("Connected client IP: %s %s \n", inet_ntoa(clntAdr.sin_addr), name[i]);
    }
    int lose;
    int minus1 = -1;
    int minus2 = -2;
    for (i = 0;; i++) {
        if (i == clntcnt) {
            i = 0;
        }
        strlen = recv(clntSocks[i], &n, sizeof(int), 0);
        numcnt += n;
        
        if (strlen == SOCKET_ERROR) {
            printf("<ERROR> 소켓 오류 발생.\n");
            break;
        }
        else if (strlen == 0) {
            printf("<MSG> 상대방이 소켓을 종료함.\n");
            break;
        }
        
        for (j = oldnum; j < numcnt; j++) {
            gauge[j] = 'X';
        }

        printf("%s%s ", gauge, name[i]);
        for (j = oldnum; j < numcnt; j++) {
            printf("%d ", j + 1);
        }
        printf("\n");
        if (numcnt == 30) {
            if ((i + 1) == clntcnt) {
                lose = 0;
            }
            else {
                lose = i + 1;
            }
            for (j = 0; j < clntcnt; j++) {
                if (j == lose) {
                    send(clntSocks[j], &minus1, sizeof(int), 0);
                }
                else {
                    send(clntSocks[j], &minus2, sizeof(int), 0);
                }

            }
            printf("%s의 패배입니다.", name[lose]);
            exit(1);
        }
        oldnum = numcnt;
        if ((i + 1) == clntcnt) {
            send(clntSocks[0], &oldnum, sizeof(int), 0);
        }
        else {
            send(clntSocks[i + 1], &oldnum, sizeof(int), 0);
        }
        
    }
    
    closesocket(hServSock);
    WSACleanup();
    return 0;
}
void ErrorHandling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}


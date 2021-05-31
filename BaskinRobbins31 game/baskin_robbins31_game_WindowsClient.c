#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#define BUF_SIZE 100
#define NAME_SIZE 20
#pragma comment(lib, "ws2_32.lib")

void* send_msg(SOCKET arg);
void* recv_msg(SOCKET arg);
void error_handling(char* msg);

int main(int argc, char* argv[])
{
    WSADATA wsaInfo;
    int errorNum, errorCode;
    char name[NAME_SIZE] = "[DEFAULT]";
    int n;

    if (argc != 4) {
        printf("Usage : %s <IP> <port> <name>\n", argv[0]);
        exit(1);
    }

    sprintf(name, "[%s]", argv[3]);         //name변수에 argv[3]이 저장됨

    // 1. 소켓 라이브러리 초기화...
    errorNum = WSAStartup(MAKEWORD(2, 2), &wsaInfo);
    if (errorNum != 0) {
        printf("<ERROR> 소켓 초기화 오류\n");
        exit(0);
    }
    SOCKET clientSocket;
    // 2. 소켓 생성...socket()
    clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    // 3. 서버주소 설정
    SOCKADDR_IN  serverAddr; // 주소: IP주소, port 번호
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET; // IPv4
    serverAddr.sin_port = htons(atoi(argv[2])); // network byte ordering
    serverAddr.sin_addr.s_addr = inet_addr((argv[1]));

    // 4. 서버 연결 설정(요청)
    connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));


    // 게임
    send(clientSocket, name, strlen(name) + 1, 0);
    printf("============================================== \n");
    printf("현봉지의 배스킨라빈스 31게임을 시작하겠습니다. \n");
    recv(clientSocket, &n, sizeof(int), 0);

    while (1) {
        if (n > 0) {
            recv_msg(clientSocket);
            send_msg(clientSocket);
        }
        else {
            send_msg(clientSocket);
            recv_msg(clientSocket);
        }
    }

    closesocket(clientSocket);
    return 0;
}

void* send_msg(SOCKET arg) {
    int n;
    char msg[BUF_SIZE];
    SOCKET clientSocket = arg;

    while (1) {
        printf("숫자를 입력하세요 :");
        fgets(msg, BUF_SIZE, stdin);

        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
        {
            closesocket(clientSocket);
            exit(0);
        }

        n = atoi(msg);
        if ((n > 3) || (n < 1)) {
            printf("잘못 입력하셨습니다. 1~3 숫자를 입력해주세요(종료:q)\n");
            continue;
        }
        break;
    }
    send(clientSocket, &n, sizeof(int), 0);

    return NULL;
}

void* recv_msg(SOCKET arg) {
    int number;
    SOCKET clientSocket = arg;
    int returnValue;
    returnValue = recv(clientSocket, &number, sizeof(int), 0);
    if (returnValue <= 0) {
        return (void*)-1;
    }
    if (number == -1) {
        printf("패배!!!!!!\n");
        exit(1);
    }
    else if(number == -2){
        printf("승리!!!!!!\n");
        exit(1);
    }
    printf("마지막 숫자: %d\n", number);
    return NULL;
}

void error_handling(char* msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
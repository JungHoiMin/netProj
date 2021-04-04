#include <stdio.h>
#include <WinSock2.h>
#include <process.h>

#define MAX_BUF_SIZE	100
#define IP_ADDRESS		"119.195.200.159"
#define PORT_NUMBER		9000

void errMsg(char* msg);
unsigned _stdcall sendMsg(void);
void receiveMsg(void);

int error;
WSADATA winsockData;
SOCKET serverSocket;
SOCKADDR_IN serverAddr;	// IPv4
SOCKET clientSocket;
SOCKADDR_IN clientAddr;
int addrLen;
char myMsg[MAX_BUF_SIZE];
char msg[MAX_BUF_SIZE];

int main(void) {
	receiveMsg();
	return 0;
}

void errMsg(char* msg) {
	printf("> %s 실패\n", msg);
	exit(1);
}

void receiveMsg(void) {
	printf("> 서버 프로그램 실행.\n");

	error = WSAStartup(MAKEWORD(2, 2), &winsockData);
	if (error != 0)
		errMsg("소켓 초기화 및 라이브러리 연결");

	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&serverAddr, 0, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT_NUMBER);
	serverAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

	bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	
	listen(serverSocket, 1);
	addrLen = sizeof(clientAddr);
	printf("> accept 함수를 호출하여 클라이언트 연결 요청을 대기합니다... \n");
	clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &addrLen);

	printf("> 클라이언트(IP:%s, Port:%d)가 연결되었습니다.\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
	_beginthreadex(NULL, 0, sendMsg, 0, 0, NULL);
	while (1) {
		while (1) {
			if (recv(clientSocket, msg, sizeof(msg), 0) != -1)
				break;
		}
		if (strcmp(msg, "q") != 0)
			printf("상대>> %s\n", msg);
		else
			break;
		
	}
	printf("> 통신을 종료합니다.\n\n");
	closesocket(clientSocket);
	closesocket(serverSocket);
	error = WSACleanup();
	if (error == SOCKET_ERROR)
		errMsg("소켓 종료");

	return;
}

unsigned _stdcall sendMsg(void) {
	while (1) {
		printf("User>> ");
		scanf("%s", &myMsg);
		send(clientSocket, myMsg, sizeof(myMsg), 0);
		if (strcmp(myMsg, "q") == 0)
			break;
	}
}
#include <stdio.h>
#include <WinSock2.h>
#include <process.h>

#define MAX_BUF_SIZE	100
#define IP_ADDRESS		"127.0.0.1"
#define PORT_NUMBER		9000

void errMsg(char* msg);

void connect_Internet(void);
int main(void) {
	connect_Internet();
	return 0;
}

void errMsg(char* msg) {
	printf("> %s 실패\n", msg);
	exit(1);
}

void connect_Internet(void) {
	int error;
	WSADATA winsockData;
	SOCKET clientSocket;
	SOCKADDR_IN serverAddr;
	int returnValue;
	char myMsg[MAX_BUF_SIZE];
	char msg[MAX_BUF_SIZE];

	printf("> 클라이언트 프로그램 실행.\n");

	error = WSAStartup(MAKEWORD(2, 2), &winsockData);
	if (error != 0)
		errMsg("소켓 초기화 및 라이브러리 연결");

	clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&serverAddr, 0, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT_NUMBER);
	serverAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

	connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	printf("> 서버와 연결이 완료되었습니다.\n");

	while (1) {
		recv(clientSocket, msg, sizeof(msg), 0);
		if (strcmp(msg, "q") != 0)
			printf("상대>> %s\n", msg);
		else
			break;

		printf("User>> ");
		scanf("%s", &myMsg);
		send(clientSocket, myMsg, sizeof(myMsg), 0);
		if (strcmp(myMsg, "q") == 0)
			break;
	}

	printf("> 통신을 종료합니다.\n\n");
	closesocket(clientSocket);
	error = WSACleanup();
	if (error == SOCKET_ERROR)
		errMsg("소켓 종료");

	return;
}
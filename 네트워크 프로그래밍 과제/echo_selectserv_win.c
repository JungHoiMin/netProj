/*
* 최초 작성자: 정회민
* 최초 작성일: 2021.05.25
* 최초 변경일:
* 목적: 서버와 클라이언트를 이용한 갤러그 게임 제작(게임 서버 네트워크 프로그래밍)
* 개정 이력:	2021.05.25
				2021.05.26
				2021.05.27
				2021.05.28
				2021.05.29
				2021.05.31
				2021.06.01
				2021.06.02
* 저작권: 정회민
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
#define MIN_MOVE 3
#define MAX_MOVE 49
void ErrorHandling(char* message);
const int player_y = 26;

int main(int argc, char* argv[])
{
	int player_x = 26;
	int dead_client = -1;
	int clientInfo;

	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr, clientaddr;
	TIMEVAL timeout;
	fd_set reads, cpyReads;

	int adrSz;
	int strLen, fdNum, i, j;
	int recv_buf[BUF_SIZE];
	int send_buf[BUF_SIZE];
	int addrlen;

	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");
	if (listen(hServSock, 5) == SOCKET_ERROR)
		ErrorHandling("listen() error");

	FD_ZERO(&reads);
	FD_SET(hServSock, &reads);

	while (1)
	{
		cpyReads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		if ((fdNum = select(0, &cpyReads, 0, 0, &timeout)) == SOCKET_ERROR)
			break;

		if (fdNum == 0)
			continue;

		for (i = 0; i < reads.fd_count; i++)
		{
			if (FD_ISSET(reads.fd_array[i], &cpyReads))
			{
				if (reads.fd_array[i] == hServSock)     // connection request!
				{
					adrSz = sizeof(clntAdr);
					hClntSock =
						accept(hServSock, (SOCKADDR*)&clntAdr, &adrSz);
					FD_SET(hClntSock, &reads);
					printf("connected client: Port:%d, IP:%s \n",
						clntAdr.sin_port, inet_ntoa(clntAdr.sin_addr)
					);
				}
				else    // read message!
				{
					strLen = recv(reads.fd_array[i], recv_buf, 12, 0);
					if (strLen <= 0)    // close request!
					{
						closesocket(cpyReads.fd_array[i]);
						FD_CLR(reads.fd_array[i], &reads);
						printf("closed client: %d, StrLen:%d \n",
							cpyReads.fd_array[i], strLen);
					}
					else
					{
						send_buf[0] = reads.fd_count-2;
						send_buf[1] = 0;
						send_buf[2] = 0;
						addrlen = sizeof(clientaddr);
						getpeername(reads.fd_array[i], (SOCKADDR*)&clientaddr, &addrlen);
						clientInfo = recv_buf[0];

						if (clientInfo == 1) {	// 1: player
							player_x += recv_buf[1];
							dead_client = recv_buf[2];
							
							if (player_x < MIN_MOVE) {
								player_x = MIN_MOVE;
							}
							else if (player_x > MAX_MOVE) {
								player_x = MAX_MOVE;
							}

							if (dead_client != -1) {
								closesocket(cpyReads.fd_array[dead_client+1]);
								FD_CLR(reads.fd_array[dead_client+1], &reads);
								printf("closed client: %d, StrLen:%d \n",
									cpyReads.fd_array[i], strLen);
							}

							send_buf[2] = recv_buf[2];
						}

						else if (clientInfo == 2) {
							if (recv_buf[1] < MIN_MOVE) {
								recv_buf[1] = MIN_MOVE;
							}
							else if (recv_buf[1] > MAX_MOVE) {
								recv_buf[1] = MAX_MOVE;
							}
							send_buf[(i - 1) * 3 ] = i - 1;
							send_buf[(i - 1) * 3 + 1] = recv_buf[1];
							send_buf[(i - 1) * 3 + 2] = recv_buf[2];
						}

						send_buf[1] = player_x;
						printf("%d, %d, %d, %d, %d, %d, %d, %d, %d\n", send_buf[0], send_buf[1], send_buf[2], send_buf[3], send_buf[4], send_buf[5], send_buf[6], send_buf[7], send_buf[8]);
						/*for (j = 0; j < send_buf[0]; j++) {
							if (((player_y == send_buf[(j + 1) * 3 + 1] + 1) || (player_y == send_buf[(j + 1) * 3 + 1])) &&
								((send_buf[1] - 4 >= send_buf[(j + 1) * 3 + 2] - 1) && (send_buf[1] + 4 <= send_buf[(j + 1) * 3 + 2] + 1))) {
								printf("종료");
								exit(1);
							}
						}*/
						for (j = 0; j < reads.fd_count; j++) {
							if ((send_buf[0] == 0) || (send_buf[0] == send_buf[(i - 1) * 3]))
								send(reads.fd_array[j], &send_buf, (send_buf[0] + 1) * sizeof(int) * 3, 0);    // echo!
						}

					}
				}
			}
		}
	}
	closesocket(hServSock);
	WSACleanup();
	return 0;
}
void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
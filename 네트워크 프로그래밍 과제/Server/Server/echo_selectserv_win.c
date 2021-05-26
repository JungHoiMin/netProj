#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
#define MIN_MOVE 1
#define MAX_MOVE 75
void ErrorHandling(char *message);

int main(int argc, char *argv[])
{
	int player_x = 39;
	int player_dx = 0;
	int player_fire = 0;
	int clientInfo;

	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr, clientaddr;
	TIMEVAL timeout;
	fd_set reads, cpyReads;

	int adrSz;
	int strLen, fdNum, i,j;
	int recv_buf[BUF_SIZE];
	int send_buf[BUF_SIZE];
	int addrlen;

	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!"); 

	hServSock=socket(PF_INET, SOCK_STREAM, 0);
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family=AF_INET;
	servAdr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAdr.sin_port=htons(atoi(argv[1]));
	
	if(bind(hServSock, (SOCKADDR*) &servAdr, sizeof(servAdr))==SOCKET_ERROR)
		ErrorHandling("bind() error");
	if(listen(hServSock, 5)==SOCKET_ERROR)
		ErrorHandling("listen() error");

	FD_ZERO(&reads);
	FD_SET(hServSock, &reads);

	while(1)
	{
		cpyReads=reads;
		timeout.tv_sec=5;
		timeout.tv_usec=5000;

		if( (fdNum = select(0, &cpyReads, 0, 0, &timeout)) == SOCKET_ERROR )
			break;
		
		if( fdNum == 0 )
			continue;

		for(i=0; i<reads.fd_count; i++)
		{
			if(FD_ISSET(reads.fd_array[i], &cpyReads))
			{
				if(reads.fd_array[i]==hServSock)     // connection request!
				{
					adrSz=sizeof(clntAdr);
					hClntSock=
						accept(hServSock, (SOCKADDR*)&clntAdr, &adrSz);
					FD_SET(hClntSock, &reads);
					printf("connected client: Port:%d, IP:%s \n", 
						clntAdr.sin_port, inet_ntoa(clntAdr.sin_addr)	
					);
				}
				else    // read message!
				{
					strLen=recv(reads.fd_array[i], recv_buf, 12, 0);
					if(strLen<=0)    // close request!
					{
						FD_CLR(reads.fd_array[i], &reads);
						closesocket(cpyReads.fd_array[i]);
						printf("closed client: %d, StrLen:%d \n", 
							cpyReads.fd_array[i], strLen );
					}
					else
					{						
						// 클라이언트 정보 얻기			
						send_buf[0] = reads.fd_count;
						send_buf[1] = 0;
						send_buf[2] = 0;
						addrlen = sizeof(clientaddr);
						getpeername(reads.fd_array[i], (SOCKADDR *)&clientaddr, &addrlen);
						clientInfo = recv_buf[0];

						if (clientInfo == 1) {	// 1: player
							player_dx = recv_buf[1];
							player_fire = recv_buf[2];

							switch (player_dx) {
							case 1: player_dx = 0; break;
							case 2: player_dx = -1; break;
							case 3: player_dx = 1; break;
							}

							player_x += player_dx;
							if (player_x < MIN_MOVE) {
								player_x = MIN_MOVE;
							}
							else if (player_x > MAX_MOVE) {
								player_x = MAX_MOVE;
							}

							send_buf[1] = player_x;
							send_buf[2] = player_fire;
						}
						printf("%d, %d\n", send_buf[0], send_buf[1]);
						for (j = 0; j < reads.fd_count; j++) {
							if (FD_ISSET(reads.fd_array[j], &cpyReads)) {
								send(reads.fd_array[j], &send_buf, reads.fd_count * 8 + 4, 0);    // echo!
							}
								
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

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
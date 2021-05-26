/*
* 최초 작성자: 정회민
* 최초 작성일: 2021.05.25
* 최초 변경일:
* 목적: 서버와 클라이언트를 이용한 갤러그 게임 제작(네트워크 프로그래밍)
* 개정 이력:
* 저작권: 정회민
*/
#include <winsock2.h>
#include <stdio.h>
#include "Control.h"
#include "GameWindow.h"
#define	BUF_SIZE	512

void ErrorDisplay( char *str )
{
	printf("<ERROR> %s!!!\n", str );
	exit(-1);
}
 
// 사용자 정의 데이터 수신 함수
int recvn( SOCKET s, char *buf, int len, int flags )
{
	int 		received;
	char 	*ptr = buf;
	int 		left = len;

	while( left > 0 )
	{
		received = recv(s, ptr, left, flags);
		if(received == SOCKET_ERROR) 
			return SOCKET_ERROR;
		else if(received == 0) 
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}


int main( int argc, char* argv[] )
{
	int	retval;		//recv(), send() 수행하면 return 값: 전송한 데이타 수 
	WindowSetting();
	int x = 39;
	int flag_fire = 0;
	int dx;
	// argv[0] : TCPClient
	// argv[1] : IP 주소
	// argv[2] : Port 번호
	if( argc != 3 )	
	{
		printf("<input format>TCPClient <IP> <Port>\n");
		exit(0);
	}

	// 윈속 DLL: open()
	WSADATA	wsa;
	retval = WSAStartup(  MAKEWORD(2, 2), &wsa );
	if( retval != 0 )	return -1;

	// 소켓 하나 만들기...
	SOCKET	ClientSocket;
	ClientSocket = socket( AF_INET, SOCK_STREAM, 0 );
	if( ClientSocket == INVALID_SOCKET )
	{
		ErrorDisplay("socket() error(INVALID_SOCKET)");
	}

	// connect() 함수를 이용해서 서버에 접속 시도...
	SOCKADDR_IN	ServerAddr;
	ZeroMemory( &ServerAddr, sizeof(ServerAddr)  );
	ServerAddr.sin_family			= AF_INET;
	ServerAddr.sin_port			= htons( atoi(argv[2]) );		// 서버 포트 번호...
	ServerAddr.sin_addr.s_addr		= inet_addr( argv[1] );		// 서버 IP 주소...
	
	retval = connect( ClientSocket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr) );
	if( retval == SOCKET_ERROR )
	{
		ErrorDisplay("connect() error(SOCKET_ERROR)");
	}

	// 데이타를 송, 수신하기...
	int Buf[BUF_SIZE+1];	// 전송 시 Buf: (1|x좌표|총 발사 여부), 수신 시 Buf: (공격자 수|플레이어 x좌표|공격자1 x좌표|공격자1 y좌표|...) 공격자 x,y 좌표 둘다 0일경우 충돌임을 표시
	//int		iLen;
	
	
	while( 1 )
	{
		// 데이타를 입력하고, 그 데이타를 서버에 전송하기...
		// 버퍼 청소...
		ZeroMemory( Buf, sizeof(Buf) );
		Buf[0] = 1;	// 1:player임을 의미
		// 사용자로부터 방향키 입력...
		while (1) {
			PrintPlayer(x);
			flag_fire = 0;
			dx = PlayerActivity();
			if (dx == -11) {
				flag_fire = 1;
				dx = 0;
				break;
			}
			else { 
				Buf[1] = dx;
				Buf[2] = flag_fire;
				//strcpy(&Buf[1], dx);
				//strcpy(&Buf[5], flag_fire);
			}

			// 이동 전송
			retval = send(ClientSocket, &Buf, 12, 0);
			if (retval == SOCKET_ERROR){
				printf("<ERROR> send()(SOCKET_ERROR)!!!\n");
				break;
			}

			
			printf("여기 %s", Buf);
			// 좌표 수신
			retval = recvn(ClientSocket, &Buf, retval, 0);
			if (retval == SOCKET_ERROR){
				printf("<ERROR> recvn()(SOCKET_ERROR)!!!\n");
				break;
			}
			else if (retval == 0) {
				x = 0;
			}
			else {
				x = Buf[1];

				if (x < MIN_MOVE) {
					x = MIN_MOVE;
				}
				else if (x > MAX_MOVE) {
					x = MAX_MOVE;
				}
			}

		}
		printf("\n[보낼 데이타] ");
		//if( fgets( Buf, BUF_SIZE+1, stdin ) == NULL )	break;
		
		
		
		printf("[TCP 클라이언트] %d 바이트를 보냈습니다.\n", retval);

		// 서버로부터 데이타를 수신하기...
		

		// 받은 데이타를 출력하기...
		Buf[retval]= '\0';
		printf("[TCP 클라이언트] %d 바이트를 받았습니다.\n", retval);
		printf("[받은 데이타] %s \n", Buf);
	}

	// 소켓 close...
	closesocket( ClientSocket ); 
	
	// 윈속 DLL: close()
	WSACleanup();

	return 0;
}
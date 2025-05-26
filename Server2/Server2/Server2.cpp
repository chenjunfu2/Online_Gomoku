#include "Server2.h"

char msg[100] = { 0 };
int szIPAnum;
SOCKET sockConn1, sockConn2;

void Recv_Thread(LPVOID);

int main(void)
{
	
	SOCKET sockServ;
	SOCKADDR_IN addrClient;
	int nLength = sizeof(SOCKADDR);

	printf("������������˿ں�:");
	scanf("%d", &szIPAnum);
	
	if (!Server(sockServ, szIPAnum))
	{
		system("pause");
		return 0;
	}
	
	sockConn1 = accept(sockServ, (SOCKADDR*)&addrClient, &nLength);
	printf("IP:%s�˿�:%d  ��������������\n", inet_ntoa(addrClient.sin_addr), addrClient.sin_port);
	sockConn2 = accept(sockServ, (SOCKADDR*)&addrClient, &nLength);
	printf("IP:%s�˿�:%d  ��������������\n", inet_ntoa(addrClient.sin_addr), addrClient.sin_port);


	_beginthread(Recv_Thread, 0, NULL);
	while (true)
	{
		recv(sockConn1, msg, 100, 0);
		send(sockConn2, msg, strlen(msg) + 1, 0);
	}
	

	system("pause");
	return 0;
}

void Recv_Thread(LPVOID)
{
	while (true)
	{
		recv(sockConn2, msg, 100, 0);
		send(sockConn1, msg, strlen(msg) + 1, 0);
	}
}
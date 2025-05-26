#pragma once

#include <process.h>
#include <stdio.h>
#include <WinSock2.h>
#pragma comment(lib ,"ws2_32.lib")

//������
//�׽��֣� �׽��֣�socket�� : IP��ַ��Ϣ�� �˿���Ϣ
//winsock.h
//winsock2.h	2.0�汾 2.2


bool Server(SOCKET& sockServ, int& szIPAnum)
{
	//1.���������2.2�汾
	WSADATA wd;
	if (WSAStartup(MAKEWORD(2, 2), &wd))
	{
		printf("�����׽��ֿ�ʧ��.\n");
		return false;
	}

	//2.�ж�ʵ������İ汾�Ƿ���һ��
	if (LOBYTE(wd.wVersion) != 2 || HIBYTE(wd.wVersion) != 2)
	{
		printf("���ص��׽��ֿ�汾��һ��.\n");
		return false;
	}

	//3.�����׽���
	//IPv4, IPv6
	//TCP/IPЭ�飺��TCP, UDP�� TCPЭ��
	//TCP : ���������Э�飩���ص㣺�������Ƚ������ӣ��ȶ��Էǳ��ߣ�һ�㲻�ᶪ�����ɿ��Ը�		SOCK_STREAM
	//Э�飺���൱�ڻ���Լ���ã��������ص�

	sockServ = socket(AF_INET, SOCK_STREAM, 0);


	//4.���׽���
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//���С�� h:host, to: ת�� ��n: net ,  s:short
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(szIPAnum);						//0-��1024�� 1024����
	if (SOCKET_ERROR == bind(sockServ, (sockaddr*)&addrSrv, sizeof(sockaddr)))
	{
		printf("���׽���ʧ��.\n");
		return false;
	}

	//5.���׽�������Ϊ����ģʽ ������Ե�ˣ�
	if (SOCKET_ERROR == listen(sockServ, 5)) //5p
	{
		printf("����ʧ��,�������磡\n");
		return false;
	}

	printf("���ڼ���...\n");

	//ÿһ�������Ϸ������Ŀͻ��˵�ַ��Ϣ
	//SOCKADDR_IN addrClient;
	//nLength = sizeof(SOCKADDR);


	//ר�ŵȴ��ͻ�����������... 
	//sockConn1 = accept(sockServ, (SOCKADDR*)&addrClient, &nLength);
	//printf("IP:%s�˿�:%d  ��������������\n", inet_ntoa(addrClient.sin_addr), addrClient.sin_port);
	//sockConn2 = accept(sockServ, (SOCKADDR*)&addrClient, &nLength);
	//printf("IP:%s�˿�:%d  ��������������\n", inet_ntoa(addrClient.sin_addr), addrClient.sin_port);

	//��������
	//char msg[100]="���Ǵ���.";
	//send(sockConn, msg, strlen(msg) + 1, 0);

	//��������
	//char msg[100];
	//recv(sockConn, msg, 101, 1);
	//printf("%s\n", msg);

	return true;
}
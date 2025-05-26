#pragma once

#include <process.h>
#include <stdio.h>
#include <WinSock2.h>
#include "Struct_Falgs.h"
#pragma comment(lib ,"ws2_32.lib")

#define MSG_DONTWAIT 0

#define DEBUG

#ifndef DEBUG

#define IPAD	"43.249.193.233"
#define IPNU	61112

#else

#define IPAD	"127.0.0.1"
#define IPNU	6969

#endif


extern TCHAR szAppName[];
static char szIPAddress[20] = IPAD;
static int szIPAnum = IPNU;


bool Client(SOCKET& sockClient, char* szIPAddress, int& szIPAnum)
{
	WSADATA wd;

	//1.���������2.2�汾
	if (WSAStartup(MAKEWORD(2, 2), &wd))
	{
		//MessageBox(nullptr, TEXT("�����׽��ֿ�ʧ��!"), szAppName, MB_ICONERROR);
		//MessageBeep(0);
		return false;
	}

	//2.�ж�ʵ������İ汾�Ƿ���һ��
	if (LOBYTE(wd.wVersion) != 2 || HIBYTE(wd.wVersion) != 2)
	{
		//MessageBox(nullptr, TEXT("���ص��׽��ֿ�汾��һ��!"), szAppName, MB_ICONERROR);
		//MessageBeep(0);
		return false;
	}

	//3.�����׽���
	sockClient = socket(AF_INET, SOCK_STREAM, 0);//IPPROTO_TCP
	if (sockClient == SOCKET_ERROR)
	{
		WSACleanup();
		//MessageBox(nullptr, TEXT("����Socketʧ��!"), szAppName, MB_ICONERROR);
		//MessageBeep(0);
		return false;
	}

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(szIPAddress);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(szIPAnum);

	//if (bind(sockClient, (struct sockaddr*)&szIPAddress, sizeof(szIPAddress)); == -1)
	//{
	//	WSACleanup();
	//	closesocket(sockClient);
	//	MessageBox(nullptr, TEXT("��ʧ��!"), szAppName, MB_ICONERROR);
	//	MessageBeep(0);
	//	return false;
	//}

	//4.���ӷ�����
	if (SOCKET_ERROR == connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)))
	{
		//MessageBox(nullptr, TEXT("���ӷ�����ʧ��!"), szAppName, MB_ICONERROR);
		//MessageBeep(0);
		return false;
	}

	//MessageBox(nullptr, TEXT("���ӳɹ�!"), szAppName, MB_ICONINFORMATION);
	return true;
}




int StartLine(const SOCKET& socketClient)
{
	//char msgflag[sizeof(int)];//׼������
	int len = STARTLINE;//д����
	//memcpy(msgflag, &len, sizeof(int));//�����ֶο������ַ���

	return send(socketClient, (char*)&len, sizeof(int), MSG_WAITALL);//����������ͱ��
}

int EndLine(const SOCKET& socketClient)
{
	//char msgflag[sizeof(int)];//׼������
	int len = ENDLINE;//д����
	//memcpy(msgflag, &len, sizeof(int));//�����ֶο������ַ���

	return send(socketClient, (char*)&len, sizeof(int), MSG_DONTWAIT);//����������ͱ��
}
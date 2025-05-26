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

	//1.加载网络库2.2版本
	if (WSAStartup(MAKEWORD(2, 2), &wd))
	{
		//MessageBox(nullptr, TEXT("加载套接字库失败!"), szAppName, MB_ICONERROR);
		//MessageBeep(0);
		return false;
	}

	//2.判断实际请求的版本是否是一致
	if (LOBYTE(wd.wVersion) != 2 || HIBYTE(wd.wVersion) != 2)
	{
		//MessageBox(nullptr, TEXT("加载的套接字库版本不一致!"), szAppName, MB_ICONERROR);
		//MessageBeep(0);
		return false;
	}

	//3.创建套接字
	sockClient = socket(AF_INET, SOCK_STREAM, 0);//IPPROTO_TCP
	if (sockClient == SOCKET_ERROR)
	{
		WSACleanup();
		//MessageBox(nullptr, TEXT("创建Socket失败!"), szAppName, MB_ICONERROR);
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
	//	MessageBox(nullptr, TEXT("绑定失败!"), szAppName, MB_ICONERROR);
	//	MessageBeep(0);
	//	return false;
	//}

	//4.连接服务器
	if (SOCKET_ERROR == connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)))
	{
		//MessageBox(nullptr, TEXT("连接服务器失败!"), szAppName, MB_ICONERROR);
		//MessageBeep(0);
		return false;
	}

	//MessageBox(nullptr, TEXT("连接成功!"), szAppName, MB_ICONINFORMATION);
	return true;
}




int StartLine(const SOCKET& socketClient)
{
	//char msgflag[sizeof(int)];//准备数组
	int len = STARTLINE;//写入标记
	//memcpy(msgflag, &len, sizeof(int));//将该字段拷贝入字符串

	return send(socketClient, (char*)&len, sizeof(int), MSG_WAITALL);//向服务器发送标记
}

int EndLine(const SOCKET& socketClient)
{
	//char msgflag[sizeof(int)];//准备数组
	int len = ENDLINE;//写入标记
	//memcpy(msgflag, &len, sizeof(int));//将该字段拷贝入字符串

	return send(socketClient, (char*)&len, sizeof(int), MSG_DONTWAIT);//向服务器发送标记
}
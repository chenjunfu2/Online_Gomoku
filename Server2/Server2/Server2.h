#pragma once

#include <process.h>
#include <stdio.h>
#include <WinSock2.h>
#pragma comment(lib ,"ws2_32.lib")

//网络编程
//套接字， 套接字（socket） : IP地址信息， 端口信息
//winsock.h
//winsock2.h	2.0版本 2.2


bool Server(SOCKET& sockServ, int& szIPAnum)
{
	//1.加载网络库2.2版本
	WSADATA wd;
	if (WSAStartup(MAKEWORD(2, 2), &wd))
	{
		printf("加载套接字库失败.\n");
		return false;
	}

	//2.判断实际请求的版本是否是一致
	if (LOBYTE(wd.wVersion) != 2 || HIBYTE(wd.wVersion) != 2)
	{
		printf("加载的套接字库版本不一致.\n");
		return false;
	}

	//3.创建套接字
	//IPv4, IPv6
	//TCP/IP协议：（TCP, UDP） TCP协议
	//TCP : （传输控制协议），特点：必须事先建立连接，稳定性非常高，一般不会丢包，可靠性高		SOCK_STREAM
	//协议：就相当于互相约定好，互相遵守的

	sockServ = socket(AF_INET, SOCK_STREAM, 0);


	//4.绑定套接字
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//大端小端 h:host, to: 转到 ，n: net ,  s:short
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(szIPAnum);						//0-》1024， 1024以上
	if (SOCKET_ERROR == bind(sockServ, (sockaddr*)&addrSrv, sizeof(sockaddr)))
	{
		printf("绑定套接字失败.\n");
		return false;
	}

	//5.将套接字设置为监听模式 （等有缘人）
	if (SOCKET_ERROR == listen(sockServ, 5)) //5p
	{
		printf("监听失败,请检查网络！\n");
		return false;
	}

	printf("正在监听...\n");

	//每一个连接上服务器的客户端地址信息
	//SOCKADDR_IN addrClient;
	//nLength = sizeof(SOCKADDR);


	//专门等待客户的连接请求... 
	//sockConn1 = accept(sockServ, (SOCKADDR*)&addrClient, &nLength);
	//printf("IP:%s端口:%d  已连接至服务器\n", inet_ntoa(addrClient.sin_addr), addrClient.sin_port);
	//sockConn2 = accept(sockServ, (SOCKADDR*)&addrClient, &nLength);
	//printf("IP:%s端口:%d  已连接至服务器\n", inet_ntoa(addrClient.sin_addr), addrClient.sin_port);

	//发送数据
	//char msg[100]="你是大佬.";
	//send(sockConn, msg, strlen(msg) + 1, 0);

	//接收数据
	//char msg[100];
	//recv(sockConn, msg, 101, 1);
	//printf("%s\n", msg);

	return true;
}
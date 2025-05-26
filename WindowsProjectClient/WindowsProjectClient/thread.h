#pragma once

#include "linked_list.h"

DWORD recvID = 0, sendID = 0;

SOCKET socketClient;//套接字结构体
HANDLE LINE = NULL;//连接成功事件

#pragma chMSG("稍后处理此处未使用的变量")
//bool usech = true;//请求更改标志
bool uget = false;//断线重连后从服务器重新接收棋盘数据标志

HANDLE Send = NULL;//发送成功or失败事件
HANDLE Recv = NULL;//接收成功or失败事件

DATA Sdata;//发送失败的数据项
DATA Rdata;//接收失败的数据项

HANDLE OK_SEND = NULL;//允许发送消息事件
HANDLE OK_RECV = NULL;//允许接收消息事件

NODE Snode;//发送队列
NODE Rnode;//接收队列

CRITICAL_SECTION Sn_se;//发送队列关键段
CRITICAL_SECTION Rn_se;//接收队列关键段

bool endcodeS = false;//结束SEND线程
bool endcodeR = false;//结束RECV线程

HANDLE SuspThreSe = NULL;//使Send线程执行完毕当前操作后挂起
HANDLE SuspThreRe = NULL;//使Recv线程执行完毕当前操作后挂起

bool ServerEnd = false;//服务器主动断开连接
//bool gamestart = false;//游戏开始标记

HANDLE rt[3] = { 0 };//Recv线程等待的事件内核对象
HANDLE st[3] = { 0 };//Send线程等待的事件内核对象

#define endthreadS endcodeS = true
#define endthreadR endcodeR = true

#define begthreadS endcodeR = false
#define begthreadR endcodeR = false

#define OKSR(os) (endcodeS = endcodeR = os)

//bool r;//Recv队列执行结果
//#define Add_Recv(ada) (EnterCriticalSection(&Rn_se), r = Add_HpQueue(Rnode, ada), LeaveCriticalSection(&Rn_se), r)//接收到的消息被加入队列
//#define Del_Recv(dda) (EnterCriticalSection(&Rn_se), r = Del_CpQueue(Rnode, dda), LeaveCriticalSection(&Rn_se), r)//主调函数从队列中得到要被处理的消息并删除

//bool s;//Send队列执行结果
//#define Add_Send(ada) (EnterCriticalSection(&Sn_se), s = Add_HpQueue(Snode, ada), LeaveCriticalSection(&Sn_se), s)//主调函数使用其向队列内添加即将要发送的消息
//#define Del_Send(dda) (EnterCriticalSection(&Sn_se), s = Del_CpQueue(Snode, dda), LeaveCriticalSection(&Sn_se), s)//发送出的消息被从队列中删除



bool Add_Recv(const DATA& data)//接收到的消息被加入队列
{
	EnterCriticalSection(&Rn_se);
	bool r = Add_HpQueue(Rnode, data);
	LeaveCriticalSection(&Rn_se);
	return r;
}

bool Add_Send(const DATA& data)//主调函数使用其向队列内添加即将要发送的消息
{
	EnterCriticalSection(&Sn_se);
	bool s = Add_HpQueue(Snode, data);
	LeaveCriticalSection(&Sn_se);
	return s;
}

bool Del_Recv(DATA& data)//主调函数从队列中得到要被处理的消息并删除
{
	EnterCriticalSection(&Rn_se);
	bool r = Del_CpQueue(Rnode, data);
	LeaveCriticalSection(&Rn_se);
	return r;
}

bool Del_Send(DATA& data)//发送出的消息被从队列中删除
{
	EnterCriticalSection(&Sn_se);
	bool s = Del_CpQueue(Snode, data);
	LeaveCriticalSection(&Sn_se);
	return s;
}



//MSG_WAITALL 阻塞式消息接收flag

unsigned Recv_Thread(LPVOID)
{
	DATA data;
	int buflen;
	char msgflag[sizeof(int)] = { 0 };

waitnext:	
	WaitForSingleObject(SuspThreRe, INFINITE);
	{
		if (endcodeS)
			return true;

		WaitForMultipleObjects(3, rt, TRUE, INFINITE);
		
		//if (!LINE || !OK_SEND || !Recv)
			//continue;

	recan:
		if ((buflen = recv(socketClient, msgflag, sizeof(int), MSG_WAITALL)) <= 0)
		{
			data = { ERROR_N,-1,NULL };
			Rdata = data;
			ResetEvent(Recv);
			goto waitnext;
		}
		else if (buflen != sizeof(int))
			goto recan;
		else
		{
			data = { ERROR_N,-1,NULL };
			Rdata = data;
			ResetEvent(Recv);
			goto waitnext;
		}

		memcpy(&data.flags, msgflag, sizeof(int));

		if (data.flags > 0)
		{
			switch (data.flags)//根据收到的标志判断要使用的结构体大小
			{
			case _POINTXY:
				data.lens = sizeof(POINTXY);
				data.datap = (char*)malloc(sizeof(POINTXY));
				break;
			case _USCO:
				data.lens = sizeof(USCO);
				data.datap = (char*)malloc(sizeof(USCO));
				break;
			case _USRE:
				data.lens = sizeof(USRE);
				data.datap = (char*)malloc(sizeof(USRE));
				break;
			case _USRO:
				data.lens = sizeof(USRO);
				data.datap = (char*)malloc(sizeof(USRO));
				break;
			case _CHESS:
				data.lens = sizeof(CHESS);
				data.datap = (char*)malloc(sizeof(CHESS));
				break;
			default:
				//ResetEvent(Recv);
				goto waitnext;
				break;
			}
		}
		else
		{
			if (data.flags == ENDLINE)
			{
				ServerEnd = true;
				{
					data.lens = 0;
					data.datap = NULL;
					Add_Recv(data);
					SetEvent(Recv);
				}
				goto waitnext;
			}
			else
			{
				data.lens = 0;
				data.datap = NULL;
				Add_Recv(data);
				SetEvent(Recv);
				goto waitnext;
			}
			
		}

	recan2:
		if ((buflen = recv(socketClient, data.datap, data.lens, MSG_WAITALL)) <= 0)
		{
			data.datap = NULL;
			Rdata = data;
			ResetEvent(Recv);
			goto waitnext;
		}
		else if (buflen != data.lens)
			goto recan2;
		else
		{
			data.datap = NULL;
			Rdata = data;
			ResetEvent(Recv);
			goto waitnext;
		}

		Add_Recv(data);
		SetEvent(Recv);
		//memset(msg, 0, 100 * sizeof(char));

		goto waitnext;
	}
	return true;
}


//MSG_WAITALL 阻塞式消息发送flag

unsigned Send_Thread(LPVOID)
{
	DATA data;
	int buflen;
waitnext:
	WaitForSingleObject(SuspThreSe, INFINITE);
	{
		if (endcodeS)
			return true;

		WaitForMultipleObjects(3, st, TRUE, INFINITE);

		//if (!LINE || !OK_RECV || !Send)
			//continue;

		WaitForSingleObject(HaveNode, INFINITE);

		if (!Del_Send(data))
			goto waitnext;

		//char msgflag[sizeof(int)];
		//memcpy(msgflag, &data.flags, sizeof(int));

	recan:
		if ((buflen = send(socketClient, (char*)&data.flags, sizeof(int), MSG_WAITALL)) <= 0)
		{
			Sdata = data;
			ResetEvent(Send);
			goto waitnext;
		}
		else if (buflen != sizeof(int))
			goto recan;
		else
		{
			Sdata = data;
			ResetEvent(Send);
			goto waitnext;
		}
		
		if (data.lens == 0)
		{
			SetEvent(Send);
			goto waitnext;
		}

	recan2:
		if ((buflen = send(socketClient, data.datap, data.lens, MSG_WAITALL)) <= 0)
		{
			Sdata = data;
			ResetEvent(Send);
			goto waitnext;
		}
		else if (buflen != data.lens)
			goto recan2;
		else
		{
			Sdata = data;
			ResetEvent(Send);
			goto waitnext;
		}

		free(data.datap);
		SetEvent(Send);

		goto waitnext;
	}
	return true;
}
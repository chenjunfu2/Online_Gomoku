#pragma once

#include "linked_list.h"

DWORD recvID = 0, sendID = 0;

SOCKET socketClient;//�׽��ֽṹ��
HANDLE LINE = NULL;//���ӳɹ��¼�

#pragma chMSG("�Ժ���˴�δʹ�õı���")
//bool usech = true;//������ı�־
bool uget = false;//����������ӷ��������½����������ݱ�־

HANDLE Send = NULL;//���ͳɹ�orʧ���¼�
HANDLE Recv = NULL;//���ճɹ�orʧ���¼�

DATA Sdata;//����ʧ�ܵ�������
DATA Rdata;//����ʧ�ܵ�������

HANDLE OK_SEND = NULL;//��������Ϣ�¼�
HANDLE OK_RECV = NULL;//���������Ϣ�¼�

NODE Snode;//���Ͷ���
NODE Rnode;//���ն���

CRITICAL_SECTION Sn_se;//���Ͷ��йؼ���
CRITICAL_SECTION Rn_se;//���ն��йؼ���

bool endcodeS = false;//����SEND�߳�
bool endcodeR = false;//����RECV�߳�

HANDLE SuspThreSe = NULL;//ʹSend�߳�ִ����ϵ�ǰ���������
HANDLE SuspThreRe = NULL;//ʹRecv�߳�ִ����ϵ�ǰ���������

bool ServerEnd = false;//�����������Ͽ�����
//bool gamestart = false;//��Ϸ��ʼ���

HANDLE rt[3] = { 0 };//Recv�̵߳ȴ����¼��ں˶���
HANDLE st[3] = { 0 };//Send�̵߳ȴ����¼��ں˶���

#define endthreadS endcodeS = true
#define endthreadR endcodeR = true

#define begthreadS endcodeR = false
#define begthreadR endcodeR = false

#define OKSR(os) (endcodeS = endcodeR = os)

//bool r;//Recv����ִ�н��
//#define Add_Recv(ada) (EnterCriticalSection(&Rn_se), r = Add_HpQueue(Rnode, ada), LeaveCriticalSection(&Rn_se), r)//���յ�����Ϣ���������
//#define Del_Recv(dda) (EnterCriticalSection(&Rn_se), r = Del_CpQueue(Rnode, dda), LeaveCriticalSection(&Rn_se), r)//���������Ӷ����еõ�Ҫ���������Ϣ��ɾ��

//bool s;//Send����ִ�н��
//#define Add_Send(ada) (EnterCriticalSection(&Sn_se), s = Add_HpQueue(Snode, ada), LeaveCriticalSection(&Sn_se), s)//��������ʹ�������������Ӽ���Ҫ���͵���Ϣ
//#define Del_Send(dda) (EnterCriticalSection(&Sn_se), s = Del_CpQueue(Snode, dda), LeaveCriticalSection(&Sn_se), s)//���ͳ�����Ϣ���Ӷ�����ɾ��



bool Add_Recv(const DATA& data)//���յ�����Ϣ���������
{
	EnterCriticalSection(&Rn_se);
	bool r = Add_HpQueue(Rnode, data);
	LeaveCriticalSection(&Rn_se);
	return r;
}

bool Add_Send(const DATA& data)//��������ʹ�������������Ӽ���Ҫ���͵���Ϣ
{
	EnterCriticalSection(&Sn_se);
	bool s = Add_HpQueue(Snode, data);
	LeaveCriticalSection(&Sn_se);
	return s;
}

bool Del_Recv(DATA& data)//���������Ӷ����еõ�Ҫ���������Ϣ��ɾ��
{
	EnterCriticalSection(&Rn_se);
	bool r = Del_CpQueue(Rnode, data);
	LeaveCriticalSection(&Rn_se);
	return r;
}

bool Del_Send(DATA& data)//���ͳ�����Ϣ���Ӷ�����ɾ��
{
	EnterCriticalSection(&Sn_se);
	bool s = Del_CpQueue(Snode, data);
	LeaveCriticalSection(&Sn_se);
	return s;
}



//MSG_WAITALL ����ʽ��Ϣ����flag

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
			switch (data.flags)//�����յ��ı�־�ж�Ҫʹ�õĽṹ���С
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


//MSG_WAITALL ����ʽ��Ϣ����flag

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
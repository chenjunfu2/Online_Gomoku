#pragma once

#include <malloc.h>
#include <Windows.h>
#include "Struct_Falgs.h"
using namespace std;

HANDLE HaveNode = NULL;

//���ݽṹ��
typedef struct data {
	int flags = ERROR_N;//�ýṹ��ı��
	int lens = -1;//�ṹ�峤��
	char* datap = NULL;//ָ��ṹ���ָ��
}DATA;

//�ڵ�ṹ��
typedef struct Queue {
	Queue* uQueue = nullptr;	//��һ�ڵ�
	DATA	data;	//��������
	Queue* nQueue = nullptr;	//��һ�ڵ�
}QUEUE;

//����ָ��ṹ��
typedef struct Node {
	QUEUE* hpQueue = nullptr;//ͷ�ڵ�ָ��
	QUEUE* cpQueue = nullptr;//β�ڵ�ָ��
	int numQueue = 0;//�������
}NODE;


//���һ��
inline bool Add_HpQueue(NODE& node, const DATA& data)
{
	QUEUE* npQueue = nullptr;
	if ((npQueue = (QUEUE*)malloc(sizeof(QUEUE))) == NULL)
		return false;

	npQueue->uQueue = nullptr;
	npQueue->nQueue = node.hpQueue;
	npQueue->data = data;

	if (node.cpQueue == nullptr)
	{
		node.cpQueue = npQueue;
		SetEvent(HaveNode);
	}

	node.hpQueue = npQueue;
	node.numQueue++;

	return true;
}

//ɾ��һ��
inline bool Del_CpQueue(NODE& node, DATA& data)
{
	if (node.cpQueue == nullptr)
		return false;
		
	data = node.cpQueue->data;
	
	QUEUE* delQueue = node.cpQueue;
	node.cpQueue = node.cpQueue->uQueue;
	free(delQueue);

	delQueue = nullptr;
	if (node.cpQueue == nullptr)
	{
		node.hpQueue = nullptr;
		ResetEvent(HaveNode);
	}

	node.numQueue--;

	return true;
}

//ɾ������
inline bool Del_AllQueue(NODE& node)
{
	if (node.hpQueue == nullptr)
		return false;

	QUEUE* delQueue = node.hpQueue;

	while (delQueue != nullptr)
	{
		node.hpQueue = node.hpQueue->nQueue;
		free(delQueue);
		delQueue = node.hpQueue;
	}

	node.cpQueue = node.hpQueue = nullptr;
	node.numQueue = 0;
	ResetEvent(HaveNode);

	return true;
}
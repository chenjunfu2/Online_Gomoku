#pragma once

#include <malloc.h>
#include <Windows.h>
#include "Struct_Falgs.h"
using namespace std;

HANDLE HaveNode = NULL;

//数据结构体
typedef struct data {
	int flags = ERROR_N;//该结构体的标记
	int lens = -1;//结构体长度
	char* datap = NULL;//指向结构体的指针
}DATA;

//节点结构体
typedef struct Queue {
	Queue* uQueue = nullptr;	//上一节点
	DATA	data;	//数据区域
	Queue* nQueue = nullptr;	//下一节点
}QUEUE;

//链表指针结构体
typedef struct Node {
	QUEUE* hpQueue = nullptr;//头节点指针
	QUEUE* cpQueue = nullptr;//尾节点指针
	int numQueue = 0;//链表个数
}NODE;


//添加一项
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

//删除一项
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

//删除所有
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
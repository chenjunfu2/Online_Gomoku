#pragma once

#define USDRAW		-15//����
#define USLOSE		-14//����

#define YENDLINE	-13//�Է��Ͽ�����
#define YDEDLINE	-12//�Է��������

#define YOU_LOSE	-11//��
#define YOU_WIN		-10//Ӯ

#define GMEND		-9//��Ϸ����
#define GMSTART		-8//��Ϸ��ʼ

#define UUNSTART	-7//ȡ��׼��
#define USESTART	-6//׼��

#define USECHANGE	-5//�������

#define CHESSALL	-4//�������ݷ������
#define GETCHESS	-3//�ӷ����������������

#define STARTLINE	-2//��ʼ����
#define ENDLINE		-1//�Ͽ�����

#define ERROR_N		0//���󡢿ձ��

//�ṹ����
#define _POINTXY	1//��������
#define _USCO		2//ѡ��ṹ
#define _USRE		3//��������
#define _USRO		4//������
#define _CHESS		5//���̱��


//�ṹ��

typedef struct pointxy {
	short x;//�������꣨�������꣩
	short y;//�������꣨�������꣩
	bool WB;//���������־����true��false��
}POINTXY;

typedef struct user_choose {
	bool first;//���ֱ�־
	bool WB;//�ڰ����־����true��false��
}USCO;

typedef struct user_recll {
	bool recll;//�Ƿ��������
	POINTXY lastpoint;//��������������ɫ
}USRE;

typedef struct user_recok {
	bool recok;//�Ƿ�ͬ�����
	POINTXY previouspoint;//��һ�������������ɫ��������ֵ�����ڹ��̵ľֲ���̬����
}USRO;

typedef struct chess {
	bool have_chess;//��λ���Ƿ������־
	bool WB;//���������־����true��false��
}CHESS;
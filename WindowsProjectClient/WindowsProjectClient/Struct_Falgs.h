#pragma once

#define USDRAW		-15//和棋
#define USLOSE		-14//认输

#define YENDLINE	-13//对方断开连接
#define YDEDLINE	-12//对方意外断线

#define YOU_LOSE	-11//输
#define YOU_WIN		-10//赢

#define GMEND		-9//游戏结束
#define GMSTART		-8//游戏开始

#define UUNSTART	-7//取消准备
#define USESTART	-6//准备

#define USECHANGE	-5//请求更换

#define CHESSALL	-4//棋盘数据发送完毕
#define GETCHESS	-3//从服务器获得棋盘数据

#define STARTLINE	-2//开始连接
#define ENDLINE		-1//断开连接

#define ERROR_N		0//错误、空标记

//结构体标记
#define _POINTXY	1//棋盘数据
#define _USCO		2//选择结构
#define _USRE		3//悔棋请求
#define _USRO		4//请求结果
#define _CHESS		5//棋盘标记


//结构体

typedef struct pointxy {
	short x;//下棋坐标（数组坐标）
	short y;//下棋坐标（数组坐标）
	bool WB;//黑棋或白棋标志（黑true白false）
}POINTXY;

typedef struct user_choose {
	bool first;//先手标志
	bool WB;//黑白棋标志（黑true白false）
}USCO;

typedef struct user_recll {
	bool recll;//是否请求悔棋
	POINTXY lastpoint;//最后落子坐标和颜色
}USRE;

typedef struct user_recok {
	bool recok;//是否同意悔棋
	POINTXY previouspoint;//上一个落子坐标和颜色，将被赋值给窗口过程的局部静态变量
}USRO;

typedef struct chess {
	bool have_chess;//此位置是否有棋标志
	bool WB;//黑棋或白棋标志（黑true白false）
}CHESS;
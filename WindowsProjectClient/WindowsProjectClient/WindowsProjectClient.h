#pragma once

#include "resource.h"
#include <windows.h>
#include <mmsystem.h>

#define xy			20//分成19格
#define menul		43//菜单宽度
#define lowClient	456//最小客户区

#define es      (smClient / 120)//网格格点的大小
#define as		(smClient / 43)//棋子相对于棋盘的大小(半径)
#define mc5     (mc * 5)
#define mc15    (mc * 15)
#define mox		LOWORD(lParam)
#define moy		HIWORD(lParam)

#define Small(x,y) ((x)<(y))?(x):(y)



void Map(const HDC& hdc, int cxClient, int cyClient, int smClient)			//19*19棋盘
{
	if (cxClient <= 0 || cyClient <= 0)
		return;

	int mx = (cxClient - smClient) / 2;
	int my = (cyClient - smClient) / 2;
	int mc = smClient / xy;

	for (int i = my + mc; i <= my - mc + smClient; i += mc)
	{
		MoveToEx(hdc, mx + mc - 1, i, NULL);
		LineTo(hdc, mx - mc + smClient, i);
	}

	for (int i = mx + mc; i <= mx - mc + smClient; i += mc)
	{
		MoveToEx(hdc, i, my + mc - 1, NULL);
		LineTo(hdc, i, my - mc + smClient);
	}


	for (int i = (mx + mc5); i <= (mx + mc15); i += mc5)
		for (int j = my + mc5; j <= my + mc15; j += mc5)
			Ellipse(hdc, i - es, j - es, i + es, j + es);

	

	return;
}

void ColorMap(HDC hdc, HBRUSH BkBrush, HBRUSH SkBrush, int cxClient, int cyClient, int smClient)
{
	if (cxClient <= 0 || cyClient <= 0)
		return;

	int mx = (cxClient - smClient) / 2;
	int my = (cyClient - smClient) / 2;

	RECT Bk = { 0,0,cxClient,cyClient };
	RECT Sk = { mx,my,mx + smClient,my + smClient };
	FillRect(hdc, &Bk, BkBrush);
	FillRect(hdc, &Sk, SkBrush);

	return;
}

POINT MouseSx(POINT pm, int cxClient, int cyClient, int smClient)
{
	

	if (cxClient <= 0 || cyClient <= 0)
		return { 0,0 };

	int mx = (cxClient - smClient) / 2;
	int my = (cyClient - smClient) / 2;
	int mc = smClient / xy;
	int mv = (int)((double)mc / 1.5);

	if (pm.x < mx + mv || pm.x > mx - mv + smClient || pm.y < my + mv || pm.y > my - mv + smClient)
		return { -1,-1 };

	int cmx, cmy;
	pm.x -= mx, pm.y -= my;

	if (pm.x % mc == 0)
		cmx = pm.x;
	else
	{
		if ((pm.x % mc) < (mc / 2 + 1))
			cmx = pm.x - pm.x % mc;
		else
			cmx = pm.x - pm.x % mc + mc;
	}

	if (pm.y % mc == 0)
		cmy = pm.y;
	else
	{
		if ((pm.y % mc) < (mc / 2 + 1))
			cmy = pm.y - pm.y % mc;
		else
			cmy = pm.y - pm.y % mc + mc;
	}
	

	//return { mx + cmx - as, my + cmy - as, mx + cmx + as, my + cmy + as };
	return { mx + cmx,my + cmy };
}

bool operator!= (POINT a, POINT b)
{
	return (a.x != b.x) || (a.y != b.y);
}

bool operator== (POINT a, POINT b)
{
	return (a.x == b.x) && (a.y == b.y);
}


POINT XYtoPO(POINT point, int cxClient, int cyClient, int smClient)
{
	if (cxClient <= 0 || cyClient <= 0)
		return{ -1,-1 };
	if (point.x < 0 || point.y < 0)
		return { -1,-1 };

	int mx = (cxClient - smClient) / 2;
	int my = (cyClient - smClient) / 2;
	int mc = smClient / xy;

	return { (point.x - (mx + mc)) / mc,(point.y - (my + mc)) / mc };
}


POINT POtoXY(POINT point, int cxClient, int cyClient, int smClient)
{
	if (cxClient <= 0 || cyClient <= 0)
		return{ -1,-1 };
	if (point.x < 0 || point.y < 0)
		return { -1,-1 };

	int mx = (cxClient - smClient) / 2;
	int my = (cyClient - smClient) / 2;
	int mc = smClient / xy;

	return { (point.x * mc) + (mx + mc),(point.y * mc) + (my + mc) };
}

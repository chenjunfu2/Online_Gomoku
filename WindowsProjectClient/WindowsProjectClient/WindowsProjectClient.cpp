// WindowsProjectClient.cpp : 定义应用程序的入口点。

#include "framework.h"
#include "Client.h"
#include "thread.h"
#include "Struct_Falgs.h"
#include "linked_list.h"
#include "WindowsProjectClient.h"
#pragma comment(lib, "winmm.lib")
#ifdef DEBUG
#include <debugapi.h>
#endif

#define DWHITE false	//白
#define DBLACK true		//黑
#define TIMER_THR	1	//时钟


// 此代码模块中包含的函数的前向声明:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

HBRUSH BkBrush;
HBRUSH SkBrush;
HCURSOR HC_ARROW;
HCURSOR HC_WAIT;
HCURSOR HC_NO;
TCHAR szAppName[] = TEXT("五子棋");

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{

	//SkBrush = CreateSolidBrush(RGB(255, 255, 255));
	//背景画刷
	{
		HBITMAP hbitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
		SkBrush = CreatePatternBrush(hbitmap);
		DeleteObject(hbitmap);
	}

	//棋盘背景画刷
	{
		HBITMAP hbitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
		BkBrush = CreatePatternBrush(hbitmap);
		DeleteObject(hbitmap);
	}
	
	//鼠标光标
	{
		HC_ARROW = LoadCursor(nullptr, IDC_ARROW);
		HC_WAIT = LoadCursor(nullptr, IDC_WAIT);
		HC_NO = LoadCursor(nullptr, IDC_NO);
	}
	

	WNDCLASSEXW wcex;
	{
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECTCLIENT));
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);//CreateSolidBrush(RGB(230, 160, 90));
		wcex.lpszMenuName = MAKEINTRESOURCEW(IDR_MENU1);
		wcex.lpszClassName = szAppName;
		wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
	}

	if (!RegisterClassExW(&wcex))
	{
		MessageBox(nullptr, TEXT("窗口注册失败!"), szAppName, MB_ICONERROR | MB_TASKMODAL);
		return 0;
	}

	HWND hwnd = CreateWindow(szAppName, szAppName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);//656, 656 + menul

	ShowWindow(hwnd, nCmdShow);
	InvalidateRect(hwnd, NULL, true);
	UpdateWindow(hwnd);
	
	
	// 主消息循环:
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	static HBRUSH Ellipsebrush, hbrushLAT, hbrushWH, hbrushBH, hbrushWL, hbrushBL;//画刷
	static POINT point = { 0,0 };//坐标结构体
	static HPEN hpenLine, hpenNp;//画笔
	
	static int cxClient, cyClient, smClient;//客户区坐标
	static bool MLD = false, MRD = false;//左键按下，右键按下

	static HBRUSH pieceM = NULL, pieceD = NULL, pieceN = NULL;//黑棋或白棋画刷
	static bool useWB = DBLACK;//黑棋或白棋标志（黑true白false）

	static CHESS chess[xy - 1][xy - 1] = { 0,0 };//棋盘数据数组
	static POINTXY lastchess = { -1,-1,DBLACK };//最后一棋的位置和颜色

	static bool first = false;//先手标记（true先手false后手）
	static bool noch = true;//禁下标志
	static bool Ustart = false;//准备状态（true准备false未准备）
	static bool Ubegin = false;//开始标记
	static bool Choose = false;//选择标记（防止选择过后重新选择）

	static POINT pointDD;//检测按下鼠标的那一刻与抬起的时候位置是否一致
	static unsigned short FLS = 0, FLR = 0;//连续发送错误计数  连续接收错误计数

	static HMENU hmenu = GetMenu(hWnd);//得到菜单句柄
	static DATA Tdata;//处理收到的数据

	static HGLOBAL wavedata = NULL;//音频资源加载句柄
	static LPCWSTR wavep = NULL;//音频资源内存句柄


	switch (message)
	{
		case WM_CREATE:
		{
			hpenLine = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
			hpenNp = CreatePen(PS_NULL, 0, RGB(0, 0, 0));

			Ellipsebrush = CreateSolidBrush(RGB(0, 0, 0));
			hbrushLAT = CreateSolidBrush(RGB(200, 0, 0));
			
			//SkBrush = CreateSolidBrush(RGB(230, 160, 90));

			hbrushBH = CreateSolidBrush(RGB(14, 14, 14));
			hbrushWH = CreateSolidBrush(RGB(241, 241, 241));
			hbrushBL = CreateSolidBrush(RGB(84, 84, 84));
			hbrushWL = CreateSolidBrush(RGB(171, 171, 171));

			wavedata = LoadResource(GetModuleHandle(NULL), FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_WAVE1), TEXT("WAVE")));//TEXT("#IDR_WAVE1")	GetClassLong(hWnd, GCL_HMODULE)		RT_RCDATA
			wavep = (LPCWSTR)LockResource(wavedata);

			//初始化事件内核对象
			//1:NULL,2:true表示手动重置事件false表示自动重置事件,3:事件初始化触发状态,4:NULL
			LINE = CreateEvent(NULL, TRUE, FALSE, NULL);
			HaveNode = CreateEvent(NULL, TRUE, FALSE, NULL);

			Send = CreateEvent(NULL, TRUE, TRUE, NULL);
			Recv = CreateEvent(NULL, TRUE, TRUE, NULL);

			OK_SEND = CreateEvent(NULL, TRUE, FALSE, NULL);
			OK_RECV = CreateEvent(NULL, TRUE, FALSE, NULL);

			SuspThreSe = CreateEvent(NULL, TRUE, FALSE, NULL);
			SuspThreRe = CreateEvent(NULL, TRUE, FALSE, NULL);

			rt[0] = LINE;
			rt[1] = Recv;
			rt[2] = OK_SEND;

			st[0] = LINE;
			st[1] = Send;
			st[2] = OK_RECV;

			//初始化关键段数据结构成员
			InitializeCriticalSection(&Sn_se);
			InitializeCriticalSection(&Rn_se);

			//设置初始菜单
			EnableMenuItem(hmenu, 1, MF_BYPOSITION | MF_GRAYED);//1
			EnableMenuItem(hmenu, 2, MF_BYPOSITION | MF_GRAYED);//2
			EnableMenuItem(hmenu, IDM_UC, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hmenu, IDM_US_START, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hmenu, IDM_US_FSTART, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hmenu, IDM_US_CS, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hmenu, IDM_US_DRAW, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hmenu, IDM_US_LOSE, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hmenu, IDM_END_LINE, MF_BYCOMMAND | MF_GRAYED);
			DrawMenuBar(hWnd);
		}
	break;
		case WM_TIMER://此处理接收到的消息
		{
			if (WaitForSingleObject(LINE, 0) == WAIT_TIMEOUT)
				break;

			if (ServerEnd)
				PostMessage(hWnd, WM_COMMAND, IDM_END_LINE, 1);
			if ((FLS >= 15 || FLR >= 15) && !ServerEnd)
			{
				ServerEnd = true;
				FLS = FLR = 0;
				if (MessageBox(nullptr, TEXT("链接意外断开，是否重试？"), szAppName, MB_ICONERROR | MB_RETRYCANCEL | MB_SYSTEMMODAL) != IDTRYAGAIN)//IDRETRY	IDTRYAGAIN	此处无法阻塞消息，求解--->使用postmessage进行异步调用即可
					PostMessage(hWnd, WM_COMMAND, IDM_END_LINE, 2);
			}


			{
				if (WaitForSingleObject(Send, 0) == WAIT_TIMEOUT)
				{
					Add_Send(Sdata);
					SetEvent(Send);
					++FLS;
				}
				else
					FLS = 0;

				if (WaitForSingleObject(Recv, 0) == WAIT_TIMEOUT)
				{
					free(Rdata.datap);
					SetEvent(Recv);
					++FLR;
				}
				else
					FLR = 0;
			}
			
			if (!Del_Recv(Tdata))
				break;


			if (Tdata.lens > 0)
			{
				switch (Tdata.flags)
				{
					case _POINTXY:
					{
						if (!Ubegin || first)
							break;
						if (chess[((POINTXY*)Tdata.datap)->y][((POINTXY*)Tdata.datap)->x].have_chess)
							break;

						//PlaySound((LPCWSTR)IDR_WAVE1, GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);
						//LoadResource() FindResource()
						PlaySound(wavep, NULL, SND_ASYNC | SND_MEMORY);

						first = !first;
						chess[((POINTXY*)Tdata.datap)->y][((POINTXY*)Tdata.datap)->x].have_chess = true;
						chess[((POINTXY*)Tdata.datap)->y][((POINTXY*)Tdata.datap)->x].WB = ((POINTXY*)Tdata.datap)->WB;
						lastchess = { (short)((POINTXY*)Tdata.datap)->x,(short)((POINTXY*)Tdata.datap)->y,((POINTXY*)Tdata.datap)->WB };

						Sleep(4);

						InvalidateRect(hWnd, NULL, false);
					}
					break;
					case _USCO:
					{
						if (Choose)
							break;

						Choose = true;


						
					}
					break;
					case _USRE:
					{
						if (!Ubegin)
							break;

					}
					break;
					case _USRO:
					{
						if (!Ubegin)
							break;

					}
					break;
					case _CHESS:
					{
						if (!Ubegin)
							break;

					}
					break;
				default:
					break;
				}
				free(Tdata.datap);
			}
			else
			{
				switch (Tdata.flags)
				{
					case USECHANGE:
					{
						if (Choose == false || MessageBox(nullptr, TEXT("对方请求更换（先后手/黑白棋）你同意吗？"), szAppName, MB_ICONINFORMATION | MB_OKCANCEL | MB_TASKMODAL) != IDOK)
							break;

						memset(chess, 0, sizeof(chess));
						//usech = true;
						Choose = false;
						first = false;
						noch = false;
						pieceM = NULL;
						pieceD = NULL;
						pieceN = NULL;

						EnableMenuItem(GetSubMenu(hmenu, 1), 1, MF_BYPOSITION | MF_ENABLED);//1,1
						EnableMenuItem(GetSubMenu(hmenu, 1), 0, MF_BYPOSITION | MF_ENABLED);//1,0

						EnableMenuItem(hmenu, IDM_F_WHITE, MF_BYCOMMAND | MF_ENABLED);
						EnableMenuItem(hmenu, IDM_F_BLACK, MF_BYCOMMAND | MF_ENABLED);
						EnableMenuItem(hmenu, IDM_S_WHITE, MF_BYCOMMAND | MF_ENABLED);
						EnableMenuItem(hmenu, IDM_S_BLACK, MF_BYCOMMAND | MF_ENABLED);

						EnableMenuItem(hmenu, IDM_US_START, MF_BYCOMMAND | MF_GRAYED);
						EnableMenuItem(hmenu, IDM_UC, MF_BYCOMMAND | MF_GRAYED);
						DrawMenuBar(hWnd);
					}
					break;
					case USESTART:
					{

					}
					break;
					case UUNSTART:
					{

					}
					break;
					case GMSTART:
					{

					}
					break;
					case GMEND:
					{

					}
					break;
					case YOU_WIN:
					{

					}
					break;
					case YOU_LOSE:
					{

					}
					break;
					case YDEDLINE:
					{

					}
					break;
					case YENDLINE:
					{

					}
					break;
					case USLOSE:
					{

					}
					break;
					case USDRAW:
					{

					}
					break;
					case ERROR_N:
					{

					}
					break;
				default:
					break;
				}
			}
		}
	break;
	case WM_SIZE:
		{
			cxClient = LOWORD(lParam); //宽
			cyClient = HIWORD(lParam); //高

			smClient = Small(cxClient, cyClient);//小边
			if (smClient % xy != 0)
				smClient = (smClient - smClient % xy);
		}
	break;
	case WM_PAINT:
		{
			//LPRECT lpRect;
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);


			//使用内存+GDI绘图
			{
				HDC hdcmem = CreateCompatibleDC(hdc);
				HBITMAP hBitmap = CreateCompatibleBitmap(hdc, cxClient, cyClient);
				SelectObject(hdcmem, hBitmap);

				//设置透明文字背景
				{
					int sbm = SetBkMode(hdcmem, TRANSPARENT);

					//背景颜色
					{
						ColorMap(hdcmem, BkBrush, SkBrush, cxClient, cyClient, smClient);
					}

					//棋盘盘面
					{
						HGDIOBJ hpen = SelectObject(hdcmem, hpenLine);
						HGDIOBJ hbrush = SelectObject(hdcmem, Ellipsebrush);

						Map(hdcmem, cxClient, cyClient, smClient);

						SelectObject(hdcmem, hbrush);
						SelectObject(hdcmem, hpen);//GetStockObject(BLACK_PEN)
					}

					//鼠标位置棋子
					if (!MRD && !MLD && pieceM != NULL)
					{
						POINT pointM;
						HGDIOBJ hpen = SelectObject(hdcmem, hpenNp);
						HGDIOBJ hbrush = SelectObject(hdcmem, pieceM);

						pointM = XYtoPO(point, cxClient, cyClient, smClient);
						if (pointM != POINT{ -1,-1 })
						{
							if (chess[pointM.y][pointM.x].have_chess)//如果此位置已有棋子则不画
							{
								if (GetCursor() != HC_NO)
								{
									SetCapture(hWnd);
									SetCursor(HC_NO);
								}
							}
							else
							{
								if (GetCursor() != HC_ARROW)
								{
									ReleaseCapture();
									SetCursor(HC_ARROW);
								}
								Ellipse(hdcmem, point.x - as, point.y - as, point.x + as, point.y + as);
							}
						}

						SelectObject(hdcmem, hbrush);
						SelectObject(hdcmem, hpen);
					}

					//已下的棋子
					if (pieceD != NULL && pieceN != NULL)
					{
						POINT pointD, pointL;
						HGDIOBJ hpen = SelectObject(hdcmem, hpenNp);

						//己方棋子
						{
							HGDIOBJ hbrush = SelectObject(hdcmem, pieceD);

							for (int i = 0; i < xy - 1; ++i)
							{
								for (int j = 0; j < xy - 1; ++j)
								{
									if (chess[j][i].have_chess && chess[j][i].WB == useWB)
									{
										pointD = POtoXY({ i,j }, cxClient, cyClient, smClient);
										if (pointD != POINT{ -1,-1 })
											Ellipse(hdcmem, pointD.x - as, pointD.y - as, pointD.x + as, pointD.y + as);
									}
								}
							}

							SelectObject(hdcmem, hbrush);
						}
						
						//对方棋子
						{
							HGDIOBJ hbrush = SelectObject(hdcmem, pieceN);

							for (int i = 0; i < xy - 1; ++i)
							{
								for (int j = 0; j < xy - 1; ++j)
								{
									if (chess[j][i].have_chess && chess[j][i].WB != useWB)
									{
										pointD = POtoXY({ i,j }, cxClient, cyClient, smClient);
										if (pointD != POINT{ -1,-1 })
											Ellipse(hdcmem, pointD.x - as, pointD.y - as, pointD.x + as, pointD.y + as);
									}
								}
							}

							SelectObject(hdcmem, hbrush);
						}
						
						//最新下的棋子标记
						{
							HGDIOBJ hbrush = SelectObject(hdcmem, hbrushLAT);

							if (lastchess.x >= 0 && lastchess.y >= 0)
							{
								pointL = POtoXY({ lastchess.x,lastchess.y }, cxClient, cyClient, smClient);
								if (pointL != POINT{ -1,-1 })
									Ellipse(hdcmem, pointL.x - as / 4, pointL.y - as / 4, pointL.x + as / 4, pointL.y + as / 4);
							}

							SelectObject(hdcmem, hbrush);
						}


						SelectObject(hdcmem, hpen);
					}

					SetBkMode(hdcmem, sbm);
				}

				BitBlt(hdc, 0, 0, cxClient, cyClient, hdcmem, 0, 0, SRCCOPY);
				DeleteDC(hdcmem);
				DeleteObject(hBitmap);
			}

			
			EndPaint(hWnd, &ps);
		}
	break;
	case WM_GETMINMAXINFO:
		{
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = lowClient;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = lowClient + menul;
		}
	break;
	case WM_LBUTTONDOWN:
		{
			SetCapture(hWnd);
			MLD = true;
			pointDD = MouseSx({ mox,moy }, cxClient, cyClient, smClient);
		}
	break;
	case WM_LBUTTONUP:
		{
			ReleaseCapture();
			MLD = false;

			if (!first || WaitForSingleObject(LINE, 0) == WAIT_TIMEOUT || noch)//if (useWB == lastchess.WB)
				break;//break;

			POINT pointU = MouseSx({ mox,moy }, cxClient, cyClient, smClient);
			if (pointU == POINT{ -1,-1 } || pointU != pointDD)
				break;

			pointU = XYtoPO(pointU, cxClient, cyClient, smClient);
			if (pointU == POINT{ -1,-1 })
				break;
			
			if (chess[pointU.y][pointU.x].have_chess)
				break;

			//PlaySound((LPCWSTR)IDR_WAVE1, GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);
			PlaySound(wavep, NULL, SND_ASYNC | SND_MEMORY);

			first = !first;
			chess[pointU.y][pointU.x].have_chess = true;
			chess[pointU.y][pointU.x].WB = useWB;
			lastchess = { (short)pointU.x,(short)pointU.y,useWB };

			POINTXY* pointxyp = (POINTXY*)malloc(sizeof(POINTXY));
			*pointxyp = lastchess;
			DATA dataD = { _POINTXY,sizeof(POINTXY),(char*)pointxyp };
			Add_Send(dataD);

			InvalidateRect(hWnd, NULL, false);
		}
	break;
	case WM_RBUTTONDOWN:
		{
			SetCapture(hWnd);
			MRD = true;
		}
	break;
	case WM_RBUTTONUP:
		{
			ReleaseCapture();
			MRD = false;

			if (GetMenuState(hmenu, IDM_US_CS, MF_BYCOMMAND) != MF_GRAYED && GetMenuState(hmenu, 2, MF_BYPOSITION) != MF_GRAYED && mox >= 0 && mox <= cxClient && moy >= 0 && moy <= cyClient)
				PostMessage(hWnd, WM_COMMAND, IDM_US_CS, NULL);
		}
	break;
	case WM_MOUSEMOVE:
		{
			if (MRD || MLD|| pieceM == NULL)
				break;

			point = MouseSx({ mox,moy }, cxClient, cyClient, smClient);
			
			InvalidateRect(hWnd, NULL, false);

			break;
		}
	break;
	case WM_COMMAND:
		{
			// 分析菜单选择:
			switch (LOWORD(wParam))
			{
			case IDM_START_LINE://开始链接
				{
					ServerEnd = false;
					OKSR(false);
					SetCursor(HC_WAIT);

					if (Client(socketClient, szIPAddress, szIPAnum))
					{
						StartLine(socketClient);

						begthreadR;
						begthreadS;
						_beginthreadex(NULL, 0, (_beginthreadex_proc_type)Recv_Thread, NULL, NULL, (unsigned*)&recvID);
						_beginthreadex(NULL, 0, (_beginthreadex_proc_type)Send_Thread, NULL, NULL, (unsigned*)&recvID);

						EnableMenuItem(hmenu, 1, MF_BYPOSITION | MF_ENABLED);//1
						EnableMenuItem(hmenu, 2, MF_BYPOSITION | MF_ENABLED);//2
						EnableMenuItem(hmenu, IDM_END_LINE, MF_BYCOMMAND | MF_ENABLED);
						EnableMenuItem(hmenu, IDM_START_LINE, MF_BYCOMMAND | MF_GRAYED);
						DrawMenuBar(hWnd);
						SetTimer(hWnd, TIMER_THR, 500, NULL);//一秒触发一次，回调函数NULL
						SetCursor(HC_ARROW);
						OKSR(true);
						SetEvent(LINE);
						MessageBox(nullptr, TEXT("连接服务器成功!"), szAppName, MB_ICONINFORMATION | MB_TASKMODAL);
					}
					else
					{
						SetCursor(HC_ARROW);
						OKSR(false);
						MessageBox(nullptr, TEXT("连接服务器失败!"), szAppName, MB_ICONERROR | MB_TASKMODAL);
					}

					InvalidateRect(hWnd, NULL, false);
				}
			break;
			case IDM_END_LINE://断开连接
				{
					if (lParam == 1 || lParam == 2)
					{
						closesocket(socketClient);
						OKSR(false);
						SetCursor(HC_WAIT);
						goto end;
					}
						
					if (MessageBox(nullptr, TEXT("你确定要断开连接吗？"), szAppName, MB_ICONINFORMATION | MB_OKCANCEL | MB_TASKMODAL) != IDOK)
						break;

					OKSR(false);
					SetCursor(HC_WAIT);

					if (EndLine(socketClient) > 0 && closesocket(socketClient) == 0)
					{
					end:
						endthreadR;
						endthreadS;

						{
							memset(chess, 0, sizeof(chess));
							lastchess = { -1,-1,DBLACK };
							useWB = DBLACK;
							first = false;
							noch = true;
							Ustart = false;
							Choose = false;
							Ubegin = false;
							pieceM = NULL;
							pieceD = NULL;
							pieceN = NULL;
						}

						EnableMenuItem(GetSubMenu(hmenu, 1), 1, MF_BYPOSITION | MF_ENABLED);//1,1
						EnableMenuItem(GetSubMenu(hmenu, 1), 0, MF_BYPOSITION | MF_ENABLED);//1,0

						EnableMenuItem(hmenu, IDM_F_WHITE, MF_BYCOMMAND | MF_ENABLED);
						EnableMenuItem(hmenu, IDM_F_BLACK, MF_BYCOMMAND | MF_ENABLED);
						EnableMenuItem(hmenu, IDM_S_WHITE, MF_BYCOMMAND | MF_ENABLED);
						EnableMenuItem(hmenu, IDM_S_BLACK, MF_BYCOMMAND | MF_ENABLED);
						
						EnableMenuItem(hmenu, 1, MF_BYPOSITION | MF_GRAYED);//1
						EnableMenuItem(hmenu, 2, MF_BYPOSITION | MF_GRAYED);//2	

						EnableMenuItem(hmenu, IDM_US_START, MF_BYCOMMAND | MF_GRAYED);
						EnableMenuItem(hmenu, IDM_US_FSTART, MF_BYCOMMAND | MF_ENABLED);

						EnableMenuItem(hmenu, IDM_UC, MF_BYCOMMAND | MF_GRAYED);

						EnableMenuItem(hmenu, IDM_START_LINE, MF_BYCOMMAND | MF_ENABLED);
						EnableMenuItem(hmenu, IDM_END_LINE, MF_BYCOMMAND | MF_GRAYED);

						EnableMenuItem(hmenu, IDM_US_CS, MF_BYCOMMAND | MF_GRAYED);
						EnableMenuItem(hmenu, IDM_US_DRAW, MF_BYCOMMAND | MF_GRAYED);
						EnableMenuItem(hmenu, IDM_US_LOSE, MF_BYCOMMAND | MF_GRAYED);

						DrawMenuBar(hWnd);
						KillTimer(hWnd, TIMER_THR);//杀掉时钟
						SetCursor(HC_ARROW);
						OKSR(false);
						ResetEvent(LINE);

						if (ServerEnd)
							MessageBox(nullptr, TEXT("服务器已断开连接!"), szAppName, MB_ICONINFORMATION | MB_TASKMODAL);
						else
							MessageBox(nullptr, TEXT("已断开服务器连接!"), szAppName, MB_ICONINFORMATION | MB_TASKMODAL);

						ServerEnd = false;
					}
					else
					{
						SetCursor(HC_ARROW);
						OKSR(false);
						if (!ServerEnd)
							MessageBox(hWnd, TEXT("断开服务器连接失败!"), szAppName, MB_ICONERROR | MB_TASKMODAL);
					}

					InvalidateRect(hWnd, NULL, false);
				}
			break;
			case IDM_EXIT://退出
				DestroyWindow(hWnd);
			break;
			case IDM_F_WHITE://先手白(该项可被灰色)
				{
					if (WaitForSingleObject(LINE, 0) == WAIT_TIMEOUT)
						break;

					Choose = true;
					pieceM = hbrushWL;
					pieceD = hbrushWH;
					pieceN = hbrushBH;
					first = true;
					noch = false;
					//usech = false;
					useWB = DWHITE;
					lastchess.WB = DBLACK;
					
					EnableMenuItem(hmenu, IDM_F_WHITE, MF_BYCOMMAND | MF_DISABLED);
					EnableMenuItem(hmenu, IDM_F_BLACK, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hmenu, IDM_UC, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hmenu, IDM_US_START, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(GetSubMenu(hmenu, 1), 1, MF_BYPOSITION | MF_GRAYED);//1,1
					DrawMenuBar(hWnd);

					USCO* uscop = (USCO*)malloc(sizeof(USCO));
					*uscop = { true,DWHITE };
					DATA dataU = { _USCO,sizeof(USCO),(char*)uscop };
					Add_Send(dataU);
				}
			break;
			case IDM_F_BLACK://先手黑(该项可被灰色)
				{
					if (WaitForSingleObject(LINE, 0) == WAIT_TIMEOUT)
						break;

					Choose = true;
					pieceM = hbrushBL;
					pieceD = hbrushBH;
					pieceN = hbrushWH;
					first = true;
					noch = false;
					//usech = false;
					useWB = DBLACK;
					lastchess.WB = DWHITE;

					EnableMenuItem(hmenu, IDM_F_BLACK, MF_BYCOMMAND | MF_DISABLED);
					EnableMenuItem(hmenu, IDM_F_WHITE, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hmenu, IDM_UC, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hmenu, IDM_US_START, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(GetSubMenu(hmenu, 1), 1, MF_BYPOSITION | MF_GRAYED);//1,1
					DrawMenuBar(hWnd);

					USCO* uscop = (USCO*)malloc(sizeof(USCO));
					*uscop = { true,DBLACK };
					DATA dataU = { _USCO,sizeof(USCO),(char*)uscop };
					Add_Send(dataU);
				}
			break;
			case IDM_S_WHITE://后手白(该项可被灰色)				
				{
					if (WaitForSingleObject(LINE, 0) == WAIT_TIMEOUT)
						break;

					Choose = true;
					pieceM = hbrushWL;
					pieceD = hbrushWH;
					pieceN = hbrushBH;
					first = false;
					noch = false;
					//usech = false;
					useWB = DWHITE;
					lastchess.WB = DBLACK;

					EnableMenuItem(hmenu, IDM_S_WHITE, MF_BYCOMMAND | MF_DISABLED);
					EnableMenuItem(hmenu, IDM_S_BLACK, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hmenu, IDM_UC, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hmenu, IDM_US_START, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(GetSubMenu(hmenu, 1), 0, MF_BYPOSITION | MF_GRAYED);//1,0
					DrawMenuBar(hWnd);

					USCO* uscop = (USCO*)malloc(sizeof(USCO));
					*uscop = { false,DWHITE };
					DATA dataU = { _USCO,sizeof(USCO),(char*)uscop };
					Add_Send(dataU);
				}
			break;
			case IDM_S_BLACK://后手黑(该项可被灰色)				
				{
					if (WaitForSingleObject(LINE, 0) == WAIT_TIMEOUT)
						break;

					Choose = true;
					pieceM = hbrushBL;
					pieceD = hbrushBH;
					pieceN = hbrushWH;
					first = false;
					noch = false;
					//usech = false;
					useWB = DBLACK;
					lastchess.WB = DWHITE;

					EnableMenuItem(hmenu, IDM_S_BLACK, MF_BYCOMMAND | MF_DISABLED);
					EnableMenuItem(hmenu, IDM_S_WHITE, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hmenu, IDM_UC, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hmenu, IDM_US_START, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(GetSubMenu(hmenu, 1), 0, MF_BYPOSITION | MF_GRAYED);//1,0
					DrawMenuBar(hWnd);

					USCO* uscop = (USCO*)malloc(sizeof(USCO));
					*uscop = { false,DBLACK };
					DATA dataU = { _USCO,sizeof(USCO),(char*)uscop };
					Add_Send(dataU);
				}
			break;
			case IDM_UC://请求更换(该项可被灰色)请求更换时解锁前后手、黑白选项，选择后再次变灰//菜单选择后未选择的部分MF_GRAYED已选择的部分MF_DISABLED其余MF_ENABLED
				{
					if (WaitForSingleObject(LINE, 0) == WAIT_TIMEOUT)
						break;
					if (MessageBox(nullptr, TEXT("你确定要更换吗？"), szAppName, MB_ICONINFORMATION | MB_OKCANCEL | MB_TASKMODAL) != IDOK)
						break;
					
					memset(chess, 0, sizeof(chess));
					Choose = false;
					first = false;
					noch = true;
					pieceM = NULL;
					pieceD = NULL;
					pieceN = NULL;

					EnableMenuItem(GetSubMenu(hmenu, 1), 1, MF_BYPOSITION | MF_ENABLED);//1,1
					EnableMenuItem(GetSubMenu(hmenu, 1), 0, MF_BYPOSITION | MF_ENABLED);//1,0

					EnableMenuItem(hmenu, IDM_F_WHITE, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hmenu, IDM_F_BLACK, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hmenu, IDM_S_WHITE, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hmenu, IDM_S_BLACK, MF_BYCOMMAND | MF_ENABLED);

					EnableMenuItem(hmenu, IDM_US_START, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hmenu, IDM_UC, MF_BYCOMMAND | MF_GRAYED);
					DrawMenuBar(hWnd);

					DATA dataU = { USECHANGE,0,NULL };
					Add_Send(dataU);
				}
			break;
			case IDM_US_START://准备(该项可被灰色)
				{
					if (WaitForSingleObject(LINE, 0) == WAIT_TIMEOUT)
						break;

					Ustart = true;
					EnableMenuItem(hmenu, IDM_US_START, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hmenu, IDM_US_FSTART, MF_BYCOMMAND | MF_ENABLED);
					DrawMenuBar(hWnd);

					DATA dataU = { USESTART,0,NULL };
					Add_Send(dataU);
				}
			break;
			case IDM_US_FSTART://取消准备(该项可被灰色)
				{
					if (WaitForSingleObject(LINE, 0) == WAIT_TIMEOUT)
						break;

					Ustart = false;
					EnableMenuItem(hmenu, IDM_US_FSTART, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hmenu, IDM_US_START, MF_BYCOMMAND | MF_ENABLED);
					DrawMenuBar(hWnd);

					DATA dataU = { UUNSTART,0,NULL };
					Add_Send(dataU);
				}
			break;
			case IDM_US_CS://悔棋(该项可被灰色)如果没有准备则灰色 如果返回的最后一步颜色与该用户不符则撤回两部，如果结构为-1，-1则代表无棋子
				{
					if (WaitForSingleObject(LINE, 0) == WAIT_TIMEOUT)
						break;
					if (MessageBox(nullptr, TEXT("你确定要悔棋吗？"), szAppName, MB_ICONINFORMATION | MB_OKCANCEL | MB_TASKMODAL) != IDOK)
						break;

					noch = true;
					if (lastchess.x < 0 || lastchess.y < 0)
					{
						MessageBox(nullptr, TEXT("棋盘没有对局数据，无法悔棋！"), szAppName, MB_ICONINFORMATION | MB_OK | MB_TASKMODAL);
						break;
					}
					

					USRE* usrep = (USRE*)malloc(sizeof(USRE));
					*usrep = { true,lastchess };
					DATA dataD = { _USRE,sizeof(POINTXY),(char*)usrep };
					Add_Send(dataD);
				}
			break;
			case IDM_US_LOSE://游戏开始后可以被选择，选择后弹出提示
				{
					if (WaitForSingleObject(LINE, 0) == WAIT_TIMEOUT)
						break;
					if (MessageBox(nullptr, TEXT("你确定要认输吗？"), szAppName, MB_ICONINFORMATION | MB_OKCANCEL | MB_TASKMODAL) != IDOK)
						break;

					noch = false;
					Ubegin = false;

					EnableMenuItem(hmenu, IDM_US_LOSE, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hmenu, IDM_US_DRAW, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hmenu, IDM_US_CS, MF_BYCOMMAND | MF_GRAYED);

					EnableMenuItem(GetSubMenu(hmenu, 1), 1, MF_BYPOSITION | MF_ENABLED);//1,1
					EnableMenuItem(GetSubMenu(hmenu, 1), 0, MF_BYPOSITION | MF_ENABLED);//1,0

					EnableMenuItem(hmenu, IDM_F_WHITE, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hmenu, IDM_F_BLACK, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hmenu, IDM_S_WHITE, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hmenu, IDM_S_BLACK, MF_BYCOMMAND | MF_ENABLED);

					EnableMenuItem(hmenu, IDM_US_FSTART, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hmenu, IDM_UC, MF_BYCOMMAND | MF_GRAYED);
					DrawMenuBar(hWnd);

					DATA dataU = { USLOSE,0,NULL };
					Add_Send(dataU);
				}
			break;
			case IDM_US_DRAW://游戏开始后可以被选择，选择后弹出提示
				{
					if (WaitForSingleObject(LINE, 0) == WAIT_TIMEOUT)
						break;
					if (MessageBox(nullptr, TEXT("你确定要和棋吗？"), szAppName, MB_ICONINFORMATION | MB_OKCANCEL | MB_TASKMODAL) != IDOK)
						break;

					noch = false;
					Ubegin = false;

					EnableMenuItem(hmenu, IDM_US_DRAW, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hmenu, IDM_US_LOSE, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hmenu, IDM_US_CS, MF_BYCOMMAND | MF_GRAYED);

					EnableMenuItem(GetSubMenu(hmenu, 1), 1, MF_BYPOSITION | MF_ENABLED);//1,1
					EnableMenuItem(GetSubMenu(hmenu, 1), 0, MF_BYPOSITION | MF_ENABLED);//1,0

					EnableMenuItem(hmenu, IDM_F_WHITE, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hmenu, IDM_F_BLACK, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hmenu, IDM_S_WHITE, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hmenu, IDM_S_BLACK, MF_BYCOMMAND | MF_ENABLED);

					EnableMenuItem(hmenu, IDM_US_FSTART, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hmenu, IDM_UC, MF_BYCOMMAND | MF_GRAYED);
					DrawMenuBar(hWnd);

					DATA dataU = { USDRAW,0,NULL };
					Add_Send(dataU);
				}
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_DESTROY:
		{
			{
				if (WaitForSingleObject(LINE, 0) == WAIT_OBJECT_0)
				{
					EndLine(socketClient);
					closesocket(socketClient);
					endthreadR;
					endthreadS;
					ResetEvent(LINE);
				}
			}

			{
				DeleteObject(hpenLine);
				DeleteObject(hpenNp);

				DeleteObject(Ellipsebrush);

				DeleteObject(BkBrush);
				DeleteObject(SkBrush);

				DeleteObject(hbrushWH);
				DeleteObject(hbrushBH);
				DeleteObject(hbrushWL);
				DeleteObject(hbrushBL);

				UnlockResource(wavedata);
				wavep = NULL;
				DeleteObject(wavedata);
			}

			{
				CloseHandle(LINE);
				CloseHandle(Send);
				CloseHandle(Recv);
				CloseHandle(OK_SEND);
				CloseHandle(OK_RECV);
				CloseHandle(SuspThreSe);
				CloseHandle(SuspThreRe);

				DeleteCriticalSection(&Sn_se);
				DeleteCriticalSection(&Rn_se);
			}

			{
				Del_AllQueue(Snode);
				Del_AllQueue(Rnode);
			}

			//GetTimeFormat
			KillTimer(hWnd, TIMER_THR);

			PostQuitMessage(0);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


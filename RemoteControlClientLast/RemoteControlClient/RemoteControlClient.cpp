// RemoteControlClient.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "RemoteControlClient.h"
#include "Connect.h"
#include "ScreenSnap.h"
#include <WinUser.h>
#include "RemoteCMD.h"
#include "RemoteFile.h"
#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPTSTR    lpCmdLine,
  _In_ int       nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  // TODO:  在此放置代码。
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);


  MSG msg;
  HACCEL hAccelTable;

  // 初始化全局字符串
  LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadString(hInstance, IDC_REMOTECONTROLCLIENT, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  // 执行应用程序初始化: 
  if (!InitInstance(hInstance, nCmdShow))
  {
    return FALSE;
  }


  hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_REMOTECONTROLCLIENT));

  // 主消息循环: 
  while (GetMessage(&msg, NULL, 0, 0))
  {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return (int)msg.wParam;
}



//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_REMOTECONTROLCLIENT));
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCE(IDC_REMOTECONTROLCLIENT);
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  HWND hWnd;

  hInst = hInstance; // 将实例句柄存储在全局变量中

  hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

  if (!hWnd)
  {
    return FALSE;
  }

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  return TRUE;
}




//处理所有的鼠标事件，移动，单机，双击都要发送消息，把最新的操作都更新过来显示
int g_nSocket = INVALID_SOCKET;
BOOL bStartup = FALSE;
HWND g_hWnd = NULL;
HWND g_hDlgWnd = NULL;
HWND g_hFileDlgWnd = NULL;


//CMD模态对话框处理函数
BOOL CALLBACK CmdModDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;
  CRemoteCMD cRemote;//cmd控制
  g_hDlgWnd = hwndDlg;
  switch (uMsg)
  {
    //  //创建显示对话框之前触发的消息
    //case  WM_INITDIALOG:
    //  //允许父窗口接受鼠标键盘操作
    //  EnableWindow(g_hWnd, TRUE);
    //  break;

  case WM_CLOSE:
    //模态对话框销毁使用EndDialog
    //关闭对话框后关闭远程cmd程序
    CMD_HEAD_INFO head;
    head.m_nCmd = CMD_CLOSE_CMD;
    head.m_Len = 0;
    int bytes;
    bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
    if (bytes < 0)
    {
      CConnect::ReportError(_T("send head in theCRemoteCMD::SendExeCmd"));
    }
    EndDialog(hwndDlg, 0);
    break;
  case WM_COMMAND:
    wmId = LOWORD(wParam);
    wmEvent = HIWORD(wParam);
    switch (wmEvent)
    {
    case BN_CLICKED:
    {
                     switch (wmId)
                     {
                     case BTN_EXECMD:
                     {
                                      //获取文本框的窗口句柄
                                      HWND hEdt = GetDlgItem(hwndDlg, IDE_EXECMD);
                                      char szBuff[MAXBYTE] = { MAXBYTE, 0 };
                                      //获取文本
                                      GetWindowText(hEdt, szBuff, MAXBYTE);
                                      if (strlen(szBuff) <= 0)
                                      {
                                        CConnect::ShowMsg("请输入要执行的cmd命令");
                                        return -1;
                                      }
                                      cRemote.SendExeCmd(szBuff);

                     }
                     }

    }
    }
  default:
    break;
  }
  return FALSE;
}

//文件模态对话框处理函数
BOOL CALLBACK FileModDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;
  CRemoteFile cFile;//文件控制
  g_hFileDlgWnd = hwndDlg;
  switch (uMsg)
  {
    //  //创建显示对话框之前触发的消息
    //case  WM_INITDIALOG:
    //  //允许父窗口接受鼠标键盘操作
    //  EnableWindow(g_hWnd, TRUE);
    //  break;

  case WM_CLOSE:
    //模态对话框销毁使用EndDialog
    EndDialog(hwndDlg, 0);
    break;
  case WM_COMMAND:
    wmId = LOWORD(wParam);
    wmEvent = HIWORD(wParam);
    switch (wmEvent)
    {
    case BN_CLICKED:
    {
                     switch (wmId)
                     {
                     case BTN_SHOWDIRVER:
                     {
                                          cFile.SendSelectDirver();
                                          break;
                     }
                     case BTN_SHOWFILE:
                     {
                                        //获取文本框的窗口句柄
                                        HWND hEdt = GetDlgItem(hwndDlg, IDE_SELECTFILE);
                                        char szBuff[MAXBYTE] = { MAXBYTE, 0 };
                                        //获取文本
                                        GetWindowText(hEdt, szBuff, MAXBYTE);
                                        if (strlen(szBuff) <= 0)
                                        {
                                          CConnect::ShowMsg("请输入要显示的文件绝对路径");
                                          return -1;
                                        }
                                        cFile.SendSelectFile(szBuff);
                                        break;
                     }
                     case BTN_UPFILE:
                     {
                                      //获取文本框的窗口句柄
                                      HWND hEdt = GetDlgItem(hwndDlg, IDE_UPFILE);
                                      char szBuff[MAXBYTE] = { MAXBYTE, 0 };
                                      //获取文本
                                      GetWindowText(hEdt, szBuff, MAXBYTE);
                                      if (strlen(szBuff) <= 0)
                                      {
                                        CConnect::ShowMsg("请输入要上传的文件绝对路径");
                                        return -1;
                                      }
                                      cFile.SendUpFile(szBuff);
                                      break;
                     }
                     case BTN_DOWNFILE:
                     {
                                        //获取文本框的窗口句柄
                                        HWND hEdt = GetDlgItem(hwndDlg, IDE_DOWNFILE);
                                        char szBuff[MAXBYTE] = { MAXBYTE, 0 };
                                        //获取文本
                                        GetWindowText(hEdt, szBuff, MAXBYTE);
                                        if (strlen(szBuff) <= 0)
                                        {
                                          CConnect::ShowMsg("请输入要下载的文件绝对路径");
                                          return -1;
                                        }
                                        cFile.SendDownFile(szBuff);
                                        break;
                     }
                     }

    }
    }
  default:
    break;
  }
  return FALSE;
}

//专门显示CMD模态对话框的线程
DWORD WINAPI ShowDialogThread(LPVOID lpParameter)
{
  //创建模态对话框
  DialogBox(hInst,
    MAKEINTRESOURCE(IDD_CMDDIALOG),
    g_hWnd,
    CmdModDialogProc);
  return 0;
}
//专门显示文件操作模态对话框的线程
DWORD WINAPI ShowFileDialogThread(LPVOID lpParameter)
{
  //创建模态对话框
  DialogBox(hInst,
    MAKEINTRESOURCE(IDD_FILEDIALOG),
    g_hWnd,
    FileModDialogProc);
  return 0;
}

//处理所有服务器回复的消息
DWORD WINAPI workThread(LPVOID lpParameter)
{

  typedef int(*PFNUNCOMPRESS)(void*, unsigned long*, void*, unsigned long);
  //获取解压函数
  HMODULE hDll = ::LoadLibrary(_T("zlib1.dll"));
  if (hDll == NULL)
  {
    CConnect::ReportError(_T("LoadLibrary in the workThread"));
    return -1;
  }
  PFNUNCOMPRESS pfnUncompress = (PFNUNCOMPRESS)GetProcAddress(hDll, "uncompress");
  if (pfnUncompress == NULL)
  {
    CConnect::ReportError(_T("GetProcAddress in the workThread"));
    return -1;
  }
  CScreenSnap cScreen;
  CRemoteCMD cRemote;
  CRemoteFile cFile;
  CMD_HEAD_INFO head;
  //接收消息
  while (true)
  {
    int nRet = recv(g_nSocket, (char*)&head, sizeof(head), 0);
    if (nRet < 0)
    {
      CConnect::ReportError(_T("recv head in the workThread"));
      return -1;
    }
    switch (head.m_nCmd)
    {
    case CMD_SCREEN:
    {
                     HWND hWnd = g_hWnd;
                     nRet = cScreen.OnCmdScreen(hWnd, head, pfnUncompress);
                     if (nRet < 0)
                     {
                       CConnect::ReportError(_T("recv head in the workThread"));
                       return -1;
                     }
                     break;
    }
    case CMD_CREATE_CMD:
    {
                         CConnect::ShowMsg("远程cmd创建成功");
                         CreateThread(NULL, 0, ShowDialogThread, NULL, 0, NULL);
                         break;
    }
    case CMD_EXECUTE_CMD:
    {
                          //显示结果 显示到那个对话框的那个控件参数 HWND hWnd, CMD_HEAD_INFO head
                          cRemote.ShowCmd(g_hDlgWnd, head, IDE_SHOWCMD);
                          break;
    }
    case CMD_SELECET_DIRVER:
    {
                             cFile.ShowFile(g_hFileDlgWnd, head, IDE_SHOWDIRVER);
                             break;

    }
    case CMD_SELECET_FILE:
    {
                           cFile.ShowFile(g_hFileDlgWnd, head, IDE_SHOWFILE);
                           break;
    }
    case CMD_DOWNLOAD_FILE:
    {
                            cFile.HandDownFile(head);
                            break;
    }
    case CMD_ERROR_CODE:
    {
                         CConnect::HandError(head);
                         break;
    }
    }
  }

  return 0;
}


//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;
  PAINTSTRUCT ps;
  HDC hdc;

  g_hWnd = hWnd;
  CConnect cConnect;
  CScreenSnap cScreen;//截图控制
  CRemoteCMD cRemoteCmd;//cmd控制
  CRemoteFile cRemoteFile;//文件控制
  switch (message)
  {
  case WM_COMMAND:
    wmId = LOWORD(wParam);
    wmEvent = HIWORD(wParam);
    // 分析菜单选择: 
    switch (wmId)
    {
    case IDM_ABOUT:
      DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
      break;
    case IDM_EXIT:
      DestroyWindow(hWnd);
      break;
    case IDM_CONNECT:
      cConnect.OnConnect(hWnd);
      CreateThread(NULL, 0, workThread, NULL, 0, NULL);
      break;
    case IDM_CLOSE:
      cConnect.OnDisConnect(hWnd);
      break;
    case IDM_SCREEN:
      cScreen.SendScreen();
      break;
    case IDM_CMD:
      cRemoteCmd.SendCreatCmd();
      break;
    case IDM_FILE:
      CreateThread(NULL, 0, ShowFileDialogThread, NULL, 0, NULL);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;
  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);
    // TODO:  在此添加任意绘图代码...
    EndPaint(hWnd, &ps);
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  case WM_MOUSEMOVE:
    if (g_nSocket > 0 && bStartup == TRUE)
    {
      cScreen.OnMyMouseMove(hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam); (hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam);
      break;
    }
    else
    {
      break;
    }
  case WM_LBUTTONDOWN:
    if (g_nSocket > 0 && bStartup == TRUE)
    {
      cScreen.OnMouseLButtonDown(hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam); (hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam);
      break;
    }
    else
    {
      break;
    }
  case WM_LBUTTONUP:
    if (g_nSocket > 0 && bStartup == TRUE)
    {
      cScreen.OnMouseLButtonUp(hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam); (hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam);
      break;
    }
    else
    {
      break;
    }
  case WM_LBUTTONDBLCLK:
    if (g_nSocket > 0 && bStartup == TRUE)
    {
      cScreen.OnMouseLButtonDoubleClick(hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam); (hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam);
      break;
    }
    else
    {
      break;
    }
  case WM_RBUTTONDOWN:
    if (g_nSocket > 0 && bStartup == TRUE)
    {
      cScreen.OnMouseRightButtonDown(hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam); (hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam);
      break;
    }
    else
    {
      break;
    }
  case WM_RBUTTONUP:
    if (g_nSocket > 0 && bStartup == TRUE)
    {
      cScreen.OnMouseRightButtonUp(hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam); (hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam);
      break;
    }
    else
    {
      break;
    }
  case WM_RBUTTONDBLCLK:
    if (g_nSocket > 0 && bStartup == TRUE)
    {
      cScreen.OnMouseRightButtonDoubleClick(hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam); (hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam);
      break;
    }
    else
    {
      break;
    }
  case WM_KEYDOWN:
    if (g_nSocket > 0 && bStartup == TRUE)
    {
      cScreen.OnKeyDown(wParam);
      break;
    }
    else
    {
      break;
    }
  case WM_KEYUP:
    if (g_nSocket > 0 && bStartup == TRUE)
    {
      cScreen.OnKeyUp(wParam);
      break;
    }
    else
    {
      break;
    }
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  UNREFERENCED_PARAMETER(lParam);
  switch (message)
  {
  case WM_INITDIALOG:
    return (INT_PTR)TRUE;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
    {
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }
    break;
  }
  return (INT_PTR)FALSE;
}

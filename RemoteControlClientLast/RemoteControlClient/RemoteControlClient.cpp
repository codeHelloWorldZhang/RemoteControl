// RemoteControlClient.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "RemoteControlClient.h"
#include "Connect.h"
#include "ScreenSnap.h"
#include <WinUser.h>
#include "RemoteCMD.h"
#include "RemoteFile.h"
#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

// �˴���ģ���а����ĺ�����ǰ������: 
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

  // TODO:  �ڴ˷��ô��롣
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);


  MSG msg;
  HACCEL hAccelTable;

  // ��ʼ��ȫ���ַ���
  LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadString(hInstance, IDC_REMOTECONTROLCLIENT, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  // ִ��Ӧ�ó����ʼ��: 
  if (!InitInstance(hInstance, nCmdShow))
  {
    return FALSE;
  }


  hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_REMOTECONTROLCLIENT));

  // ����Ϣѭ��: 
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
//  ����:  MyRegisterClass()
//
//  Ŀ��:  ע�ᴰ���ࡣ
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
//   ����:  InitInstance(HINSTANCE, int)
//
//   Ŀ��:  ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  HWND hWnd;

  hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

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




//�������е�����¼����ƶ���������˫����Ҫ������Ϣ�������µĲ��������¹�����ʾ
int g_nSocket = INVALID_SOCKET;
BOOL bStartup = FALSE;
HWND g_hWnd = NULL;
HWND g_hDlgWnd = NULL;
HWND g_hFileDlgWnd = NULL;


//CMDģ̬�Ի�������
BOOL CALLBACK CmdModDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;
  CRemoteCMD cRemote;//cmd����
  g_hDlgWnd = hwndDlg;
  switch (uMsg)
  {
    //  //������ʾ�Ի���֮ǰ��������Ϣ
    //case  WM_INITDIALOG:
    //  //�������ڽ��������̲���
    //  EnableWindow(g_hWnd, TRUE);
    //  break;

  case WM_CLOSE:
    //ģ̬�Ի�������ʹ��EndDialog
    //�رնԻ����ر�Զ��cmd����
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
                                      //��ȡ�ı���Ĵ��ھ��
                                      HWND hEdt = GetDlgItem(hwndDlg, IDE_EXECMD);
                                      char szBuff[MAXBYTE] = { MAXBYTE, 0 };
                                      //��ȡ�ı�
                                      GetWindowText(hEdt, szBuff, MAXBYTE);
                                      if (strlen(szBuff) <= 0)
                                      {
                                        CConnect::ShowMsg("������Ҫִ�е�cmd����");
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

//�ļ�ģ̬�Ի�������
BOOL CALLBACK FileModDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;
  CRemoteFile cFile;//�ļ�����
  g_hFileDlgWnd = hwndDlg;
  switch (uMsg)
  {
    //  //������ʾ�Ի���֮ǰ��������Ϣ
    //case  WM_INITDIALOG:
    //  //�������ڽ��������̲���
    //  EnableWindow(g_hWnd, TRUE);
    //  break;

  case WM_CLOSE:
    //ģ̬�Ի�������ʹ��EndDialog
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
                                        //��ȡ�ı���Ĵ��ھ��
                                        HWND hEdt = GetDlgItem(hwndDlg, IDE_SELECTFILE);
                                        char szBuff[MAXBYTE] = { MAXBYTE, 0 };
                                        //��ȡ�ı�
                                        GetWindowText(hEdt, szBuff, MAXBYTE);
                                        if (strlen(szBuff) <= 0)
                                        {
                                          CConnect::ShowMsg("������Ҫ��ʾ���ļ�����·��");
                                          return -1;
                                        }
                                        cFile.SendSelectFile(szBuff);
                                        break;
                     }
                     case BTN_UPFILE:
                     {
                                      //��ȡ�ı���Ĵ��ھ��
                                      HWND hEdt = GetDlgItem(hwndDlg, IDE_UPFILE);
                                      char szBuff[MAXBYTE] = { MAXBYTE, 0 };
                                      //��ȡ�ı�
                                      GetWindowText(hEdt, szBuff, MAXBYTE);
                                      if (strlen(szBuff) <= 0)
                                      {
                                        CConnect::ShowMsg("������Ҫ�ϴ����ļ�����·��");
                                        return -1;
                                      }
                                      cFile.SendUpFile(szBuff);
                                      break;
                     }
                     case BTN_DOWNFILE:
                     {
                                        //��ȡ�ı���Ĵ��ھ��
                                        HWND hEdt = GetDlgItem(hwndDlg, IDE_DOWNFILE);
                                        char szBuff[MAXBYTE] = { MAXBYTE, 0 };
                                        //��ȡ�ı�
                                        GetWindowText(hEdt, szBuff, MAXBYTE);
                                        if (strlen(szBuff) <= 0)
                                        {
                                          CConnect::ShowMsg("������Ҫ���ص��ļ�����·��");
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

//ר����ʾCMDģ̬�Ի�����߳�
DWORD WINAPI ShowDialogThread(LPVOID lpParameter)
{
  //����ģ̬�Ի���
  DialogBox(hInst,
    MAKEINTRESOURCE(IDD_CMDDIALOG),
    g_hWnd,
    CmdModDialogProc);
  return 0;
}
//ר����ʾ�ļ�����ģ̬�Ի�����߳�
DWORD WINAPI ShowFileDialogThread(LPVOID lpParameter)
{
  //����ģ̬�Ի���
  DialogBox(hInst,
    MAKEINTRESOURCE(IDD_FILEDIALOG),
    g_hWnd,
    FileModDialogProc);
  return 0;
}

//�������з������ظ�����Ϣ
DWORD WINAPI workThread(LPVOID lpParameter)
{

  typedef int(*PFNUNCOMPRESS)(void*, unsigned long*, void*, unsigned long);
  //��ȡ��ѹ����
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
  //������Ϣ
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
                         CConnect::ShowMsg("Զ��cmd�����ɹ�");
                         CreateThread(NULL, 0, ShowDialogThread, NULL, 0, NULL);
                         break;
    }
    case CMD_EXECUTE_CMD:
    {
                          //��ʾ��� ��ʾ���Ǹ��Ի�����Ǹ��ؼ����� HWND hWnd, CMD_HEAD_INFO head
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


//  ����:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;
  PAINTSTRUCT ps;
  HDC hdc;

  g_hWnd = hWnd;
  CConnect cConnect;
  CScreenSnap cScreen;//��ͼ����
  CRemoteCMD cRemoteCmd;//cmd����
  CRemoteFile cRemoteFile;//�ļ�����
  switch (message)
  {
  case WM_COMMAND:
    wmId = LOWORD(wParam);
    wmEvent = HIWORD(wParam);
    // �����˵�ѡ��: 
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
    // TODO:  �ڴ���������ͼ����...
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

// �����ڡ������Ϣ�������
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

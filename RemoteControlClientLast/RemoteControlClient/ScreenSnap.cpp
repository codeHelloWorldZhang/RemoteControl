#include "stdafx.h"
#include "ScreenSnap.h"
extern int g_nSocket;
extern BOOL bStartup;

CScreenSnap::CScreenSnap()
{
}


CScreenSnap::~CScreenSnap()
{
}

//显示截图
void CScreenSnap::ShowScreen(HWND hWnd, int width, int height, char* buffer)
{
  HDC hdcWind = ::GetDC(hWnd);
  //创建内存dc
  HDC hdcCompatible = ::CreateCompatibleDC(hdcWind);
  if (hdcCompatible == NULL)
  {
    CConnect::ReportError(_T("CreateCompatibleDC in the CScreenSnap::ShowScreen"));
    return;
  }
  //创建位图内存
  HBITMAP hbmScreen = ::CreateCompatibleBitmap(hdcWind, width, height);
  if (hbmScreen == NULL)
  {
    CConnect::ReportError(_T("CreateCompatibleBitmap in the CScreenSnap::ShowScreen"));
    return;
  }

  //选择位图
  HGDIOBJ hdcOld = ::SelectObject(hdcCompatible, hbmScreen);
  if (hdcOld == NULL)
  {
    CConnect::ReportError(_T("SelectObject in the CScreenSnap::ShowScreen"));
    return;
  }
  //填充位图
  LONG lRet = SetBitmapBits(hbmScreen, width * height * 4, buffer);
  if (lRet == 0)
  {
    CConnect::ReportError(_T("SetBitmapBits in the CScreenSnap::ShowScreen"));
    return;
  }
  //拷贝位图到窗口
  bool bRet = ::BitBlt(hdcWind, 0, 0, width, height, hdcCompatible, 0, 0, SRCCOPY);
  if (!bRet)
  {
    CConnect::ReportError(_T("BitBlt in the CScreenSnap::ShowScreen"));
    return;
  }

  DeleteDC(hdcWind);
  DeleteDC(hdcCompatible);
  DeleteObject(hbmScreen);

}

//处理回复的CMD_SCREEN消息
int CScreenSnap::OnCmdScreen(HWND hWnd, CMD_HEAD_INFO head, PFNUNCOMPRESS pfnUncompress)
{
  CMD_SCREEN_INFO info;
  int nRet = recv(g_nSocket, (char*)&info, sizeof(info), 0);
  if (nRet < 0)
  {
    CConnect::ReportError(_T("recv CMD_SCREEN_INFO in the CScreenSnap::OnCmdScreen"));
    return -1;
  }

  char *lpCompressBuff = new char[head.m_Len];
  int nEnSize = head.m_Len;
  int RecvBytes = 0;
  int bytes = 0;
  while (RecvBytes < nEnSize)
  {
    bytes = recv(g_nSocket, lpCompressBuff + RecvBytes, nEnSize - RecvBytes, 0);
    if (bytes < 0)
    {
      CConnect::ReportError(_T("recv lpCompressBuff in the CScreenSnap::OnCmdScreen"));
      return -1;
    }
    RecvBytes += bytes;
  }
  //解压缩
  char *pDecomBuffer = new char[info.m_nTotalSize];
  nRet = pfnUncompress(pDecomBuffer, (unsigned long*)&info.m_nTotalSize, lpCompressBuff, nEnSize);
  if (nRet != 0)
  {
    CConnect::ReportError(_T("pfnUncompress error in the CScreenSnap::OnCmdScreen"));
    return nRet;
  }
  //显示截图
  ShowScreen(hWnd, info.m_nWidth, info.m_nHeight, pDecomBuffer);
  delete[] lpCompressBuff;
  lpCompressBuff = NULL;
  delete[] pDecomBuffer;
  pDecomBuffer = NULL;
}


// 处理鼠标指针移动消息  
int WINAPI CScreenSnap::OnMyMouseMove(HWND hWnd, int x, int y, int nMark)
{
  //发送鼠标移动消息，服务器截图后返回
  CMD_MOUSE_INFO stMousePoint;
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_WM_MOUSEMOVE;
  head.m_Len = sizeof(CMD_MOUSE_INFO);
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CScreenSnap::OnMyMouseMove send head"));
    return -1;
  }
  stMousePoint.m_nX = x;
  stMousePoint.m_nY = y;
  bytes = send(g_nSocket, (char*)&stMousePoint, sizeof(stMousePoint), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CScreenSnap::OnMyMouseMove send stMousePoint"));
    return -1;
  }
}

// 处理鼠标左键按下消息  
void WINAPI CScreenSnap::OnMouseLButtonDown(HWND hWnd, int x, int y, int nMark)
{
  CMD_MOUSE_INFO stMousePoint;
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_WM_LBUTTONDOWN;
  head.m_Len = sizeof(stMousePoint);
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CScreenSnap::OnMouseLButtonDown send head"));
    return;
  }
  stMousePoint.m_nX = x;
  stMousePoint.m_nY = y;
  bytes = send(g_nSocket, (char*)&stMousePoint, sizeof(stMousePoint), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CScreenSnap::OnMouseLButtonDown send stMousePoint"));
    return;
  }
}

// 处理鼠标左键抬起消息  
void WINAPI CScreenSnap::OnMouseLButtonUp(HWND hWnd, int x, int y, int nMark)
{
  CMD_MOUSE_INFO stMousePoint;
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_WM_LBUTTONUP;
  head.m_Len = sizeof(stMousePoint);
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CScreenSnap::OnMouseLButtonUp send head"));
    return;
  }
  stMousePoint.m_nX = x;
  stMousePoint.m_nY = y;
  bytes = send(g_nSocket, (char*)&stMousePoint, sizeof(stMousePoint), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CScreenSnap::OnMouseLButtonUp send stMousePoint"));
    return;
  }
}

// 处理鼠标左键双击消息  
void WINAPI CScreenSnap::OnMouseLButtonDoubleClick(HWND hWnd, int x, int y, int nMark)
{
  CMD_MOUSE_INFO stMousePoint;
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_WM_LBUTTONDBLCLK;
  head.m_Len = sizeof(stMousePoint);
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CScreenSnap::OnMouseLButtonDoubleClick send head"));
    return;
  }
  stMousePoint.m_nX = x;
  stMousePoint.m_nY = y;
  bytes = send(g_nSocket, (char*)&stMousePoint, sizeof(stMousePoint), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CScreenSnap::OnMouseLButtonDoubleClick send stMousePoint"));
    return;
  }
}

void WINAPI CScreenSnap::OnMouseRightButtonDown(HWND hWnd, int x, int y, int nMark)
{
  CMD_MOUSE_INFO stMousePoint;
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_WM_RIGHTBUTTONDOWN;
  head.m_Len = sizeof(stMousePoint);
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CScreenSnap::OnMouseRightButtonDown send head"));
    return;
  }
  stMousePoint.m_nX = x;
  stMousePoint.m_nY = y;
  bytes = send(g_nSocket, (char*)&stMousePoint, sizeof(stMousePoint), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CScreenSnap::OnMouseRightButtonDown send stMousePoint"));
    return;
  }
}

void WINAPI CScreenSnap::OnMouseRightButtonUp(HWND hWnd, int x, int y, int nMark)
{
  CMD_MOUSE_INFO stMousePoint;
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_WM_RIGHTBUTTONUP;
  head.m_Len = sizeof(stMousePoint);
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CScreenSnap::OnMouseRightButtonUp send head"));
    return;
  }
  stMousePoint.m_nX = x;
  stMousePoint.m_nY = y;
  bytes = send(g_nSocket, (char*)&stMousePoint, sizeof(stMousePoint), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CScreenSnap::OnMouseRightButtonUp send stMousePoint"));
    return;
  }
}

void WINAPI CScreenSnap::OnMouseRightButtonDoubleClick(HWND hWnd, int x, int y, int nMark)
{
  CMD_MOUSE_INFO stMousePoint;
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_WM_RIGHTBUTTONDBLCLK;
  head.m_Len = sizeof(stMousePoint);
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CScreenSnap::OnMouseRightButtonDoubleClick send head"));
    return;
  }
  stMousePoint.m_nX = x;
  stMousePoint.m_nY = y;
  bytes = send(g_nSocket, (char*)&stMousePoint, sizeof(stMousePoint), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CScreenSnap::OnMouseRightButtonDoubleClick send stMousePoint"));
    return;
  }
}


//处理键盘消息
void CScreenSnap::OnKeyDown(WPARAM wparam)
{
  CMD_KEYBOARY_INFO stVirKey;
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_WM_KEYDOWN;
  head.m_Len = sizeof(stVirKey);
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CScreenSnap::OnKeyDown send head"));
    return;
  }
  stVirKey.m_nVirKey = wparam;
  bytes = send(g_nSocket, (char*)&stVirKey, sizeof(stVirKey), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CScreenSnap::OnKeyDown send stVirKey"));
    return;
  }
}

void CScreenSnap::OnKeyUp(WPARAM wparam)
{
  CMD_KEYBOARY_INFO stVirKey;
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_WM_KEYUP;
  head.m_Len = sizeof(stVirKey);
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CScreenSnap::OnKeyUp send head"));
    return;
  }
  stVirKey.m_nVirKey = wparam;
  bytes = send(g_nSocket, (char*)&stVirKey, sizeof(stVirKey), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CScreenSnap::OnKeyUp send stVirKey"));
    return;
  }
}

void CScreenSnap::SendScreen()
{
  bStartup = TRUE;

  if (g_nSocket < 0)
  {
    CConnect::ShowMsg("请连接服务器");
    return;
  }

  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_SCREEN;
  head.m_Len = 0;
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("send head in the workThread"));
    return;
  }
}

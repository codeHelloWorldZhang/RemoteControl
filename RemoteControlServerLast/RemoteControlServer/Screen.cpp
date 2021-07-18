#include "Screen.h"


CScreen::CScreen()
{
}


CScreen::~CScreen()
{
}

//处理客户端截图消息
int CScreen::ScreenSnap(int nScoket)
{
  int nResult = 0;
  //获取屏幕分辨率
  int nScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
  int nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

  //获取桌面dc
  HDC hdcScreen = ::CreateDC("DISPLAY", NULL, NULL, NULL);
  if (hdcScreen == NULL)
  {
    CTCPServer::ReportError(_T("CreateDC"));
    return -1;
  }

  //创建内存dc
  HDC hdcCompatible = ::CreateCompatibleDC(hdcScreen);
  if (hdcCompatible == NULL)
  {
    CTCPServer::ReportError(_T("CreateDC"));
    return -1;
  }
  //创建位图内存
  HBITMAP hbmScreen = ::CreateCompatibleBitmap(hdcScreen, nScreenWidth, nScreenHeight);
  if (hbmScreen == NULL)
  {
    CTCPServer::ReportError(_T("CreateCompatibleBitmap"));
    return -1;
  }

  //选择位图
  HGDIOBJ hdcOld = ::SelectObject(hdcCompatible, hbmScreen);
  if (hdcOld == NULL)
  {
    CTCPServer::ReportError(_T("SelectObject"));
    return -1;
  }
  //拷贝位图
  bool bRet = ::BitBlt(hdcCompatible, 0, 0, nScreenWidth, nScreenHeight, hdcScreen, 0, 0, SRCCOPY);
  if (!bRet)
  {
    CTCPServer::ReportError(_T("BitBlt"));
    return -1;
  }
  //一个像素是4个字节保存颜色,RGB三个字节可以代表任意一种颜色，另外一个字节是扩展
  //申请一块内存保存位图的像素
  int nTotalSize = nScreenWidth*nScreenHeight*sizeof(long);
  LPVOID lpBits = ::GlobalAlloc(GMEM_FIXED, nTotalSize);

  //将位图的值拷贝到指定的缓冲区中
  int nRet = ::GetBitmapBits(hbmScreen, nTotalSize, lpBits);
  if (nRet == 0)
  {
    CTCPServer::ReportError(_T("GetBitmapBits"));
    return -1;
  }
  //调用dll中的压缩函数压缩数据
  typedef int(*PFNCOMPRESS)(void*, unsigned long*, void*, unsigned long);
  HMODULE hDll = ::LoadLibrary(_T("zlib1.dll"));
  if (hDll == NULL)
  {
    CTCPServer::ReportError(_T("LoadLibrary"));
    return -1;
  }
  PFNCOMPRESS pfncompress = (PFNCOMPRESS)GetProcAddress(hDll, "compress");
  if (pfncompress == NULL)
  {
    CTCPServer::ReportError(_T("GetProcAddress pfncompress"));
    return -1;
  }

  LPVOID lpCompressBuff = new char[nTotalSize / 4];
  memset(lpCompressBuff, 0, nTotalSize / 4);
  unsigned long ulEnSize;
  //压缩
  int nErrorCode = pfncompress(lpCompressBuff, &ulEnSize, lpBits, nTotalSize);
  if (nErrorCode != 0)
  {
    printf("pfncompress error\n");
  }


  //发送头
  CMD_HEAD_INFO stHead;
  stHead.m_nCmd = CMD_SCREEN;
  stHead.m_Len = ulEnSize;//压缩后的大小
  int nbytes = send(nScoket, (char*)&stHead, sizeof(stHead), 0);
  if (nbytes == SOCKET_ERROR)
  {
    CTCPServer::ReportError(_T("send CMD_HEAD_INFO"));
    return -1;
  }
  //发送源图大小信息
  CMD_SCREEN_INFO stInfo;
  stInfo.m_nHeight = nScreenHeight;
  stInfo.m_nWidth = nScreenWidth;
  stInfo.m_nTotalSize = nTotalSize;
  nbytes = send(nScoket, (char*)&stInfo, sizeof(stInfo), 0);
  if (nbytes == SOCKET_ERROR)
  {
    CTCPServer::ReportError(_T("send CMD_SCREEN_INFO"));
    return -1;
  }
  //发送数据，要发送完全
  int nSendBytes = 0;
  while (nSendBytes < ulEnSize)
  {
    nbytes = send(nScoket, (char*)lpCompressBuff + nSendBytes, ulEnSize - nSendBytes, 0);
    if (nbytes == SOCKET_ERROR)
    {
      CTCPServer::ReportError(_T("send lpBits"));
      nResult = -1;
      goto SAFE_EXIT;
    }
    nSendBytes += nbytes;
  }


SAFE_EXIT:
  GlobalFree((HGLOBAL)lpBits);
  delete[] lpCompressBuff;
  lpCompressBuff = NULL;
  DeleteDC(hdcCompatible);
  DeleteDC(hdcScreen);
  DeleteObject(hbmScreen);

  return nResult;
}

//处理客户端鼠标消息
void CScreen::OnMouseMove(int nScoket)
{
  CMD_MOUSE_INFO stMousePoint;

  int result = recv(nScoket, (char*)&stMousePoint, sizeof(stMousePoint), 0);
  if (result < 0)
  {
    CTCPServer::ReportError(_T("OnMouseMove recv"));
    return;
  }
  SetCursorPos(stMousePoint.m_nX, stMousePoint.m_nY);
  mouse_event(MOUSEEVENTF_MOVE, 0, 0, MOUSEEVENTF_WHEEL, NULL);
}

void CScreen::OnMouseLButtonDown(int nScoket)
{
  CMD_MOUSE_INFO stMousePoint;

  int result = recv(nScoket, (char*)&stMousePoint, sizeof(stMousePoint), 0);
  if (result < 0)
  {
    CTCPServer::ReportError(_T("OnMouseMove recv"));
    return;
  }
  SetCursorPos(stMousePoint.m_nX, stMousePoint.m_nY);
  mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, MOUSEEVENTF_WHEEL, NULL);
}

void CScreen::OnMouseLButtonUp(int nScoket)
{
  CMD_MOUSE_INFO stMousePoint;

  int result = recv(nScoket, (char*)&stMousePoint, sizeof(stMousePoint), 0);
  if (result < 0)
  {
    CTCPServer::ReportError(_T("OnMouseMove recv"));
    return;
  }
  SetCursorPos(stMousePoint.m_nX, stMousePoint.m_nY);
  mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, MOUSEEVENTF_WHEEL, NULL);
}

void CScreen::OnMouseLButtonDoubleClick(int nScoket)
{
  CMD_MOUSE_INFO stMousePoint;

  int result = recv(nScoket, (char*)&stMousePoint, sizeof(stMousePoint), 0);
  if (result < 0)
  {
    CTCPServer::ReportError(_T("OnMouseMove recv"));
    return;
  }
  SetCursorPos(stMousePoint.m_nX, stMousePoint.m_nY);
  //模拟鼠标的两次双击
  mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, MOUSEEVENTF_WHEEL, NULL);
  mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, MOUSEEVENTF_WHEEL, NULL);

}

void CScreen::OnMouseRightButtonDown(int nScoket)
{
  CMD_MOUSE_INFO stMousePoint;

  int result = recv(nScoket, (char*)&stMousePoint, sizeof(stMousePoint), 0);
  if (result < 0)
  {
    CTCPServer::ReportError(_T("OnMouseMove recv"));
    return;
  }
  SetCursorPos(stMousePoint.m_nX, stMousePoint.m_nY);
  mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, MOUSEEVENTF_WHEEL, NULL);

}

void CScreen::OnMouseRightButtonUp(int nScoket)
{
  CMD_MOUSE_INFO stMousePoint;

  int result = recv(nScoket, (char*)&stMousePoint, sizeof(stMousePoint), 0);
  if (result < 0)
  {
    CTCPServer::ReportError(_T("OnMouseMove recv"));
    return;
  }
  SetCursorPos(stMousePoint.m_nX, stMousePoint.m_nY);
  mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, MOUSEEVENTF_WHEEL, NULL);

}

void CScreen::OnMouseRightButtonDoubleClick(int nScoket)
{
  CMD_MOUSE_INFO stMousePoint;

  int result = recv(nScoket, (char*)&stMousePoint, sizeof(stMousePoint), 0);
  if (result < 0)
  {
    CTCPServer::ReportError(_T("OnMouseMove recv"));
    return;
  }
  SetCursorPos(stMousePoint.m_nX, stMousePoint.m_nY);
  //模拟鼠标的两次双击
  mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, MOUSEEVENTF_WHEEL, NULL);
  mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, MOUSEEVENTF_WHEEL, NULL);

}

//处理客户端发来的键盘消息
void CScreen::OnKeyDown(int nScoket)
{
  CMD_KEYBOARY_INFO stKeyInfo;
  int result = recv(nScoket, (char*)&stKeyInfo, sizeof(stKeyInfo), 0);
  if (result < 0)
  {
    CTCPServer::ReportError(_T("OnKeyDown recv"));
    return;
  }
  keybd_event(stKeyInfo.m_nVirKey, 0, 0, 0);
}

void CScreen::OnKeyUp(int nScoket)
{
  CMD_KEYBOARY_INFO stKeyInfo;
  int result = recv(nScoket, (char*)&stKeyInfo, sizeof(stKeyInfo), 0);
  if (result < 0)
  {
    CTCPServer::ReportError(_T("OnKeyDown recv"));
    return;
  }
  keybd_event(stKeyInfo.m_nVirKey, 0, KEYEVENTF_KEYUP, 0);
}

#include "Screen.h"


CScreen::CScreen()
{
}


CScreen::~CScreen()
{
}

//����ͻ��˽�ͼ��Ϣ
int CScreen::ScreenSnap(int nScoket)
{
  int nResult = 0;
  //��ȡ��Ļ�ֱ���
  int nScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
  int nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

  //��ȡ����dc
  HDC hdcScreen = ::CreateDC("DISPLAY", NULL, NULL, NULL);
  if (hdcScreen == NULL)
  {
    CTCPServer::ReportError(_T("CreateDC"));
    return -1;
  }

  //�����ڴ�dc
  HDC hdcCompatible = ::CreateCompatibleDC(hdcScreen);
  if (hdcCompatible == NULL)
  {
    CTCPServer::ReportError(_T("CreateDC"));
    return -1;
  }
  //����λͼ�ڴ�
  HBITMAP hbmScreen = ::CreateCompatibleBitmap(hdcScreen, nScreenWidth, nScreenHeight);
  if (hbmScreen == NULL)
  {
    CTCPServer::ReportError(_T("CreateCompatibleBitmap"));
    return -1;
  }

  //ѡ��λͼ
  HGDIOBJ hdcOld = ::SelectObject(hdcCompatible, hbmScreen);
  if (hdcOld == NULL)
  {
    CTCPServer::ReportError(_T("SelectObject"));
    return -1;
  }
  //����λͼ
  bool bRet = ::BitBlt(hdcCompatible, 0, 0, nScreenWidth, nScreenHeight, hdcScreen, 0, 0, SRCCOPY);
  if (!bRet)
  {
    CTCPServer::ReportError(_T("BitBlt"));
    return -1;
  }
  //һ��������4���ֽڱ�����ɫ,RGB�����ֽڿ��Դ�������һ����ɫ������һ���ֽ�����չ
  //����һ���ڴ汣��λͼ������
  int nTotalSize = nScreenWidth*nScreenHeight*sizeof(long);
  LPVOID lpBits = ::GlobalAlloc(GMEM_FIXED, nTotalSize);

  //��λͼ��ֵ������ָ���Ļ�������
  int nRet = ::GetBitmapBits(hbmScreen, nTotalSize, lpBits);
  if (nRet == 0)
  {
    CTCPServer::ReportError(_T("GetBitmapBits"));
    return -1;
  }
  //����dll�е�ѹ������ѹ������
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
  //ѹ��
  int nErrorCode = pfncompress(lpCompressBuff, &ulEnSize, lpBits, nTotalSize);
  if (nErrorCode != 0)
  {
    printf("pfncompress error\n");
  }


  //����ͷ
  CMD_HEAD_INFO stHead;
  stHead.m_nCmd = CMD_SCREEN;
  stHead.m_Len = ulEnSize;//ѹ����Ĵ�С
  int nbytes = send(nScoket, (char*)&stHead, sizeof(stHead), 0);
  if (nbytes == SOCKET_ERROR)
  {
    CTCPServer::ReportError(_T("send CMD_HEAD_INFO"));
    return -1;
  }
  //����Դͼ��С��Ϣ
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
  //�������ݣ�Ҫ������ȫ
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

//����ͻ��������Ϣ
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
  //ģ����������˫��
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
  //ģ����������˫��
  mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, MOUSEEVENTF_WHEEL, NULL);
  mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, MOUSEEVENTF_WHEEL, NULL);

}

//����ͻ��˷����ļ�����Ϣ
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

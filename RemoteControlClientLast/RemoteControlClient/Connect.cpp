#include "stdafx.h"
#include "Connect.h"
extern int g_nSocket;
#define SERVER_PORT 18888
//#define SERVER_IP "192.168.1.8"
#define SERVER_IP "127.0.0.1"
CConnect::CConnect()
{
}


CConnect::~CConnect()
{
}

//连接
LRESULT CConnect::OnConnect(HWND hWnd)
{
  if (g_nSocket > 0)
  {
    ShowMsg("服务器已连接");
    return 0;
  }

  g_nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (g_nSocket <= 0)
  {
    ReportError("socket");
    return 0;
  }

  //连接
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(SERVER_PORT);
  if (inet_pton(AF_INET, SERVER_IP, &addr.sin_addr) < 0)
  {
    ReportError("inet_pton");
    return 0;
  }
  int nRet = connect(g_nSocket, (sockaddr*)&addr, sizeof(addr));
  if (nRet < 0)
  {
    ReportError("connect");
    return 0;
  }


  ShowMsg("connect ok");
  return 0;
}

//关闭连接
LRESULT CConnect::OnDisConnect(HWND hWnd)
{
  if (g_nSocket > 0)
  {
    closesocket(g_nSocket);
    g_nSocket = INVALID_SOCKET;
  }
  else
  {
    ShowMsg("服务器已断开");
    return 0;
  }


  //showMsg("OnDisConnect");
  return 0;
}


void CConnect::ReportError(const char* pre)
{
  //#if win_32
  //#if linux
  LPVOID lpMsgBuf;
  FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR)&lpMsgBuf,
    0,
    NULL
    );

  MessageBox(NULL, (char*)lpMsgBuf, NULL, MB_OK);

  LocalFree(lpMsgBuf);

}

void CConnect::ShowMsg(const char* msg)
{
  MessageBox(NULL, msg, "提示", MB_OK);
}

void CConnect::HandError(CMD_HEAD_INFO stHead)
{
  CMD_RETURN_CODE stErroeCode;
  int nRet = recv(g_nSocket, (char*)&stErroeCode, stHead.m_Len, 0);
  if (nRet < 0)
  {
    ReportError(_T("CConnect::HandError"));
    return;
  }
  switch (stErroeCode.m_Code)
  {
  case -1:
    ShowMsg("文件不存在 文件打开错误");
  default:
    break;
  }
}

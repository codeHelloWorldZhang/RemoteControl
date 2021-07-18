#include "stdafx.h"
#include "RemoteCMD.h"
extern int g_nSocket;

CRemoteCMD::CRemoteCMD()
{
}


CRemoteCMD::~CRemoteCMD()
{
}

void CRemoteCMD::SendCreatCmd()
{
  if (g_nSocket < 0)
  {
    CConnect::ShowMsg("请连接服务器");
    return;
  }
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_CREATE_CMD;
  head.m_Len = 0;
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CRemoteCMD::SendCreatCmd"));
    return;
  }
}

void CRemoteCMD::SendExeCmd(char* szComand)
{
  if (g_nSocket < 0)
  {
    CConnect::ShowMsg("请连接服务器");
    return;
  }
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_EXECUTE_CMD;
  head.m_Len = strlen(szComand) + 1;
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("send head in theCRemoteCMD::SendExeCmd"));
    return;
  }
  bytes = send(g_nSocket, szComand, head.m_Len, 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("send szComand in the CRemoteCMD::SendExeCmd"));
    return;
  }
  
}

void CRemoteCMD::ShowCmd(HWND hWnd, CMD_HEAD_INFO head, int ID)
{
  char* pTempCommad = new char[head.m_Len];
  memset(pTempCommad, 0, head.m_Len);
  int nRet = recv(g_nSocket, pTempCommad, head.m_Len, 0);
  if (nRet < 0)
  {
    CConnect::ReportError(_T("recv head in the CRemoteCMD::ShowCmd"));
    return;
  }
  BOOL bRet = SetDlgItemText(hWnd, ID, pTempCommad);
  if (bRet == FALSE)
  {
    CConnect::ReportError(_T("SetDlgItemText in the CRemoteCMD::ShowCmd"));
    return;
  }
  delete[] pTempCommad;
}

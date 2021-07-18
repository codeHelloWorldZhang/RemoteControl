#include "Screen.h"
#include "CMDHander.h"
#include "FileHander.h"
BOOL g_bFlag = FALSE;

DWORD WINAPI ScreenSnapThread(LPVOID lpParameter)
{
  CScreen cScreen;
  int nClientSocket = (int)lpParameter;
  while (true)
  {
    cScreen.ScreenSnap(nClientSocket);
    Sleep(500);
  }
}

DWORD WINAPI WorkThread(LPVOID lpParameter)
{

  CScreen cScreen;
  CCMDHander cCMDHander;
  CFileHander cFileHander;
  int nClientSocket = (int)lpParameter;
  sockaddr_in stClientAdddr;
  int nlen = sizeof(stClientAdddr);

  _tprintf(_T("workThread create\n"));

  //客户端连接 通过socket获取客户端地址信息
  if (getpeername(nClientSocket, (sockaddr*)&stClientAdddr, &nlen) < 0) {
    CTCPServer::ReportError(_T("getpeername"));
    return 0;
  }

  char arrchIp[16];
  int  nPort;
  inet_ntop(AF_INET, (void *)&stClientAdddr.sin_addr, arrchIp, sizeof(arrchIp));
  nPort = ntohs(stClientAdddr.sin_port);
  _tprintf(_T("%s:%d connect\n"), arrchIp, nPort);


  while (true)
  {
    CMD_HEAD_INFO stHead;

    int result = recv(nClientSocket, (char*)&stHead, sizeof(stHead), 0);
    if (result < 0) 
    {
      CTCPServer::ReportError(_T("recv"));
      break;
    }

    switch (stHead.m_nCmd)
    {
    case CMD_SCREEN:
      if (g_bFlag == FALSE)
      {
        CreateThread(NULL, 0, ScreenSnapThread, (LPVOID)nClientSocket, 0, NULL);
      }
      g_bFlag = TRUE;
      break;
    case CMD_WM_MOUSEMOVE:
      cScreen.OnMouseMove(nClientSocket);
      break;
    case CMD_WM_LBUTTONDOWN:
      cScreen.OnMouseLButtonDown(nClientSocket);
      break;
    case CMD_WM_LBUTTONUP:
      cScreen.OnMouseLButtonUp(nClientSocket);
      break;
    case CMD_WM_LBUTTONDBLCLK:
      cScreen.OnMouseLButtonDoubleClick(nClientSocket);
      break;
    case CMD_WM_RIGHTBUTTONDOWN:
      cScreen.OnMouseRightButtonDown(nClientSocket);
      break;
    case CMD_WM_RIGHTBUTTONUP:
      cScreen.OnMouseRightButtonUp(nClientSocket);
      break;
    case CMD_WM_RIGHTBUTTONDBLCLK:
      cScreen.OnMouseRightButtonDoubleClick(nClientSocket);
      break;
    case CMD_WM_KEYDOWN:
      cScreen.OnKeyDown(nClientSocket);
      break;
    case CMD_WM_KEYUP:
      cScreen.OnKeyUp(nClientSocket);
      break;
    case CMD_CREATE_CMD:
      cCMDHander.CreatCMD(nClientSocket);
      break;
    case CMD_EXECUTE_CMD:
      cCMDHander.ExecuteCMD(nClientSocket,stHead.m_Len);
      break;
    case CMD_CLOSE_CMD:
      cCMDHander.CloseCMD();
      break;
    case CMD_SELECET_DIRVER:
      cFileHander.FindDriver(nClientSocket);
      break;
    case CMD_SELECET_FILE:
      cFileHander.FindDesignationFile(nClientSocket,stHead.m_Len);
      break;
    case CMD_UPLOAD_FILE:
      cFileHander.UploadFile(nClientSocket, stHead.m_Len);
      break;
    case CMD_DOWNLOAD_FILE:
      cFileHander.DownloadFile(nClientSocket, stHead.m_Len);
      break;
    }
  }


  closesocket(nClientSocket);

  printf("workThread exit\n");

  return 0;
}


//全局变量
int main()
{
  CTCPServer stServer;
  stServer.BindSocket();
  stServer.ListenSocket();
  while (true)
  {
    int nClientSocket = stServer.AcceptSocket();
    if (nClientSocket < 0)
    {
      CTCPServer::ReportError(_T("accept"));
      break;
    }
    else
    {
      CreateThread(NULL, 0, WorkThread, (LPVOID)nClientSocket, 0, NULL);
    }
  }
  return 0;
}
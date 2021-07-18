#include "CMDHander.h"


CCMDHander::CCMDHander()
{
}


CCMDHander::~CCMDHander()
{
}

void CCMDHander::CreatCMD(int nScoket)
{
  SECURITY_ATTRIBUTES sa;
  sa.nLength = sizeof(sa);
  sa.bInheritHandle = TRUE;
  sa.lpSecurityDescriptor = NULL;

  BOOL bRet = CreatePipe(
    &m_hReadFather,  //读出端句柄
    &m_hWriteSon, //写入端句柄
    &sa,
    0); //使用默认的缓冲区大小
  if (bRet == FALSE)
  {
    CTCPServer::ReportError("CreatePipe int the CreatCMD ");
  }

  bRet = CreatePipe(
    &m_hReadSon,  //读出端句柄
    &m_hWriteFather, //写入端句柄
    &sa,
    0); //使用默认的缓冲区大小
  if (bRet == FALSE)
  {
    CTCPServer::ReportError("CreatePipe int the CreatCMD ");
  }
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  si.dwFlags = STARTF_USESTDHANDLES;
  si.hStdInput = m_hReadSon;
  si.hStdOutput = m_hWriteSon;
  ZeroMemory(&pi, sizeof(pi));

  TCHAR szName[] = { _T("cmd") };

  // Start the child process. 
  if (!CreateProcess(NULL, // No module name (use command line). 
    szName, // Command line. 
    NULL,             // Process handle not inheritable. 
    NULL,             // Thread handle not inheritable. 
    TRUE,            // Set handle inheritance to FALSE. 
    CREATE_NO_WINDOW,                // No creation flags. 
    NULL,             // Use parent's environment block. 
    NULL,             // Use parent's starting directory. 
    &si,              // Pointer to STARTUPINFO structure.
    &pi)             // Pointer to PROCESS_INFORMATION structure.
    )
  {
    CTCPServer::ReportError("CreateProcess fail");
  }
  m_hSonProcess = pi.hProcess;

  //发送头
  CMD_HEAD_INFO stHead;
  stHead.m_nCmd = CMD_CREATE_CMD;
  stHead.m_Len = 0;
  int nbytes = send(nScoket, (char*)&stHead, sizeof(stHead), 0);
  if (nbytes == SOCKET_ERROR)
  {
    CTCPServer::ReportError(_T("send CMD_HEAD_INFO fail in the CCMDHander::CreatCMD "));
    return;
  }
}

void CCMDHander::ExecuteCMD(int nScoket, int nLen)
{
  char* pTempCommad = new char[nLen];
  int nRet = recv(nScoket, pTempCommad, nLen, 0);
  if (nRet < 0)
  {
    CTCPServer::ReportError(_T("recv head in the workThread"));
    return;
  }
  string strCommand = pTempCommad;
  delete[] pTempCommad;
  strCommand += _T("\n");//cmd命令需要接受回车
  DWORD dwBytesToWrite = 0;
  BOOL bRet = WriteFile(
    m_hWriteFather,
    strCommand.c_str(),
    strCommand.length(),
    &dwBytesToWrite,
    NULL);
  if (!bRet)
  {
    CTCPServer::ReportError("WriteFile fail in the CCMDHander::ExecuteCMD");
    return;
  }

  TCHAR szBuff[1024] = { 0 };
  DWORD dwBytesToRead = 0;
  DWORD dwBytesAvail = 0;  //管道中可用字节数
  PeekNamedPipe(m_hReadFather, NULL, NULL, NULL, &dwBytesAvail, NULL);//判断管道中是否有数据

  //如果管道中还有数据,则读取返回给客户端
  if (dwBytesAvail > 0)
  {
    BOOL bRet = ReadFile(
      m_hReadFather,
      szBuff,
      sizeof(szBuff),
      &dwBytesToRead,
      NULL);
    if (!bRet)
    {
      CTCPServer::ReportError("ReadFile fail in the ExecuteCMD");
      return;
    }
    //发送头
    CMD_HEAD_INFO stHead;
    stHead.m_nCmd = CMD_EXECUTE_CMD;
    stHead.m_Len = dwBytesToRead + 1;
    int nbytes = send(nScoket, (char*)&stHead, sizeof(stHead), 0);
    if (nbytes == SOCKET_ERROR)
    {
      CTCPServer::ReportError(_T("send CMD_HEAD_INFO fail in the ExecuteCMD"));
      return;
    }
    //发送返回的指令结果
    nbytes = send(nScoket, (char*)szBuff, stHead.m_Len, 0);
    if (nbytes == SOCKET_ERROR)
    {
      CTCPServer::ReportError(_T("send CMD_SCREEN_INFO"));
      return;
    }
  }

}

void CCMDHander::CloseCMD()
{
  TerminateProcess(m_hSonProcess, 0);
}

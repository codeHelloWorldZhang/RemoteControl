#include "stdafx.h"
#include "RemoteFile.h"
#include <string>
using namespace std;
extern int g_nSocket;

CRemoteFile::CRemoteFile()
{
}


CRemoteFile::~CRemoteFile()
{
}

void CRemoteFile::SendSelectDirver()
{
  if (g_nSocket < 0)
  {
    CConnect::ShowMsg("请连接服务器");
    return;
  }
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_SELECET_DIRVER;
  head.m_Len = 0;
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("CRemoteFile::SendSelectDirver"));
    return;
  }
}

void CRemoteFile::SendSelectFile(char* szComand)
{
  if (g_nSocket < 0)
  {
    CConnect::ShowMsg("请连接服务器");
    return;
  }
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_SELECET_FILE;
  head.m_Len = strlen(szComand) + 1;
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("send head in the CRemoteFile::SendSelectFile"));
    return;
  }
  bytes = send(g_nSocket, szComand, head.m_Len, 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("send szComand in the CRemoteFile::SendSelectFile"));
    return;
  }
}

void CRemoteFile::ShowFile(HWND hWnd, CMD_HEAD_INFO head, int ID)
{
  char* pTempCommad = new char[head.m_Len];
  memset(pTempCommad, 0, head.m_Len);
  int nRet = recv(g_nSocket, pTempCommad, head.m_Len, 0);
  if (nRet < 0)
  {
    CConnect::ReportError(_T("recv head in the CRemoteFile::ShowFile"));
    return;
  }
  BOOL bRet = SetDlgItemText(hWnd, ID, pTempCommad);
  if (bRet == FALSE)
  {
    CConnect::ReportError(_T("SetDlgItemText in the CRemoteFile::ShowFile"));
    return;
  }
  delete[] pTempCommad;
}

void CRemoteFile::SendUpFile(char* pFilePath)
{
  if (g_nSocket < 0)
  {
    CConnect::ShowMsg("请连接服务器");
    return;
  }
  //打开要上传的文件
  HANDLE hOldFile = CreateFile(pFilePath,//文件名
    GENERIC_READ | GENERIC_WRITE, //可读可写
    NULL,
    NULL,
    OPEN_EXISTING, //打开已经存在的文件
    FILE_ATTRIBUTE_NORMAL,
    NULL);
  if (hOldFile == INVALID_HANDLE_VALUE)
  {
    CConnect::ReportError(_T("CreateFile fail in the CRemoteFile::SendUpFile"));
    return;
  }

  //获取文件信息
  WIN32_FIND_DATA fdFileData;         /* 存储文件数据 */
  HANDLE hFindFile;   /* 文件句柄 */
  if ((hFindFile = FindFirstFile(pFilePath, &fdFileData)) == INVALID_HANDLE_VALUE)
  {
    CConnect::ReportError(_T("FindFirstFile fail in the CRemoteFile::SendUpFile"));
    return;
  }
  //把下载的的文件进行传输
  CMD_HEAD_INFO stHead;
  stHead.m_nCmd = CMD_UPLOAD_FILE;
  stHead.m_Len = sizeof(CMD_FILE_INFO);
  int bytes;
  bytes = send(g_nSocket, (char*)&stHead, sizeof(stHead), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("send stHead in the CRemoteFile::SendUpFile"));
    return;
  }

  CMD_FILE_INFO stFileInfo;
  strcpy_s(stFileInfo.szFilePath, strlen(fdFileData.cFileName) + 1, fdFileData.cFileName);
  stFileInfo.m_nFileLen = GetFileSize(hOldFile, NULL);//获取文件的大小
  bytes = send(g_nSocket, (char*)&stFileInfo, sizeof(stFileInfo), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("send stFileInfo in the CRemoteFile::SendUpFile"));
    return;
  }

  char szBuf[2048] = { 0 };
  DWORD  dwReadLen = 0;
  while (TRUE)
  {
    memset(szBuf, 0, 2048);
    if (ReadFile(hOldFile, szBuf, 2048, &dwReadLen, NULL) && dwReadLen > 0)
    {
      int nRet = send(g_nSocket, szBuf, dwReadLen, 0);
      if (nRet < 0)
      {
        CConnect::ReportError(_T("send szBuf in the CRemoteFile::SendUpFile"));
        return;
      }
    }
    else
    {
      break;
    }
   
  }

  FindClose(hFindFile);
  CloseHandle(hOldFile);
}

void CRemoteFile::SendDownFile(char* pFilePath)
{
  if (g_nSocket < 0)
  {
    CConnect::ShowMsg("请连接服务器");
    return;
  }
  //把下载的的文件进行传输
  CMD_HEAD_INFO stHead;
  stHead.m_nCmd = CMD_DOWNLOAD_FILE;
  stHead.m_Len = sizeof(CMD_FILE_INFO);
  int bytes;
  bytes = send(g_nSocket, (char*)&stHead, sizeof(stHead), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("send stHead in the CRemoteFile::SendDownFile"));
    return;
  }

  CMD_FILE_INFO stFileInfo;
  strcpy_s(stFileInfo.szFilePath, strlen(pFilePath) + 1, pFilePath);
  stFileInfo.m_nFileLen = 0;
  bytes = send(g_nSocket, (char*)&stFileInfo, sizeof(stFileInfo), 0);
  if (bytes < 0)
  {
    CConnect::ReportError(_T("send stFileInfo in the CRemoteFile::SendDownFile"));
    return;
  }
}

void CRemoteFile::HandDownFile(CMD_HEAD_INFO head)
{
  CMD_FILE_INFO stFileInfo;
  int nRet = recv(g_nSocket, (char*)&stFileInfo, head.m_Len, 0);
  if (nRet < 0)
  {
    CConnect::ReportError(_T("recv lpszPath in the CRemoteFile::HandDownFile"));
    return;
  }

  //创建新文件,默认放在c盘
  string strFilePath = "C:\\";
  strFilePath += stFileInfo.szFilePath;
  HANDLE hNewFile = CreateFile(strFilePath.c_str(),//文件名
    GENERIC_READ | GENERIC_WRITE, //可读可写
    NULL,
    NULL,
    CREATE_NEW, //创建新的拷贝文件
    FILE_ATTRIBUTE_NORMAL,
    NULL);
  if (hNewFile == INVALID_HANDLE_VALUE)
  {
    CConnect::ReportError(_T("CreateFile in the CRemoteFile::HandDownFile"));
    return;
  }
  //把上传的文件进行创建
  char szBuf[2048] = { 0 };
  DWORD dwWriteLen = 0;
  int nReadLen = 0;
  while (dwWriteLen < stFileInfo.m_nFileLen)
  {
    memset(szBuf, 0, 2048);
    nReadLen = recv(g_nSocket, szBuf, 2048, 0);
    if (nReadLen < 0)
    {
      CConnect::ReportError(_T("recv szBuf in the CRemoteFile::HandDownFile"));
      return;
    }
    DWORD dwRealWriteLen = 0;
    WriteFile(hNewFile, szBuf, nReadLen, &dwRealWriteLen, NULL);
    if (nReadLen != dwRealWriteLen)
    {
      CConnect::ReportError(_T("WriteFile in the CRemoteFile::HandDownFile"));
      return;
    }
    dwWriteLen += dwRealWriteLen;
  }
  CloseHandle(hNewFile);
}

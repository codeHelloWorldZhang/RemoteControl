#include "FileHander.h"
#include "TCPServer.h"
#include "protocol.h"
#include <atlstr.h>
#include <string>
using namespace std;
CFileHander::CFileHander()
{
}


CFileHander::~CFileHander()
{
}

void CFileHander::FindDriver(int nScoket)
{
  char rootPath[10] = { 0 }, driveType[21] = { 0 };
  UINT nType;
  char szReply[1024] = { 0 };
  char* pTemp = szReply;
  for (char a = 'A'; a <= 'Z'; a++)
  {
    memset(rootPath, 0, 10);
    memset(driveType, 0, 21);
    sprintf_s(rootPath, "%c:\\", a);
    nType = GetDriveType(rootPath);
    if (nType != DRIVE_NO_ROOT_DIR)
    {
      switch (nType)
      {
      case DRIVE_FIXED:
        strcpy_s(driveType, 21, "硬盘");
        break;
      case DRIVE_REMOVABLE:
        strcpy_s(driveType, 21, "移动硬盘");
        break;
      case DRIVE_CDROM:
        strcpy_s(driveType, 21, "光盘");
        break;
      case DRIVE_RAMDISK:
        strcpy_s(driveType, 21, "RAM盘");
        break;
      case DRIVE_REMOTE:
        strcpy_s(driveType, 21, "Remote(Network) drive 网络磁盘");
        break;
      case DRIVE_UNKNOWN:
      default:
        strcpy_s(driveType, 21, "未知盘");
        break;
      }
      sprintf_s(pTemp, MAX_PATH, "%s  %s\r\n", rootPath, driveType);
      int nLen = strlen(rootPath) + strlen(driveType) + 2 + 2;
      pTemp += nLen;
    }
  }

  //发送头
  CMD_HEAD_INFO stHead;
  stHead.m_nCmd = CMD_SELECET_DIRVER;
  stHead.m_Len = strlen(szReply) + 1;
  int nbytes = send(nScoket, (char*)&stHead, sizeof(stHead), 0);
  if (nbytes == SOCKET_ERROR)
  {
    CTCPServer::ReportError(_T("send CMD_HEAD_INFO fail in the CFileHander::FindDriver "));
    return;
  }
  nbytes = send(nScoket, szReply, stHead.m_Len, 0);
  if (nbytes == SOCKET_ERROR)
  {
    CTCPServer::ReportError(_T("send szReply fail in the CFileHander::FindDriver "));
    return;
  }

}

void CFileHander::FindDesignationFile(int nScoket, int nLen)
{
  char* lpszPath = new char[nLen];
  memset(lpszPath, 0, nLen);
  int nRet = recv(nScoket, lpszPath, nLen, 0);
  if (nRet < 0)
  {
    CTCPServer::ReportError(_T("recv lpszPath in the CFileHander::FindDesignationFile"));
    return;
  }
  char szFilePath[MAX_PATH] = { 0 };      /* 遍历路径(正则表达式*.*) */
  char szAllPath[MAX_PATH] = { 0 };        /* 存储具体文件的绝对路径 */
  WIN32_FIND_DATA fdFileData;         /* 存储文件数据 */
  HANDLE hFindFile;   /* 文件句柄 */
  char szReply[1024] = { 0 };
  string strReply;
  sprintf_s(szFilePath, _T("%s%s"), lpszPath, _T("\\*.*"));
  if ((hFindFile = FindFirstFile(szFilePath, &fdFileData)) == INVALID_HANDLE_VALUE)
  {
    printf("Find file failed, Error code:%d\n", GetLastError());
    return;
  }
  do
  {
    memset(szAllPath, 0, MAX_PATH);
    memset(szReply, 0, 1024);
    sprintf_s(szAllPath, _T("%s\\%s"), lpszPath, fdFileData.cFileName);
    if (!strcmp(fdFileData.cFileName, _T("."))) /* 找到的为当前目录. */
    {
      continue;
    }
    if (!strcmp(fdFileData.cFileName, _T("..")))/* 找到的为父目录.. */
    {
      continue;
    }
    sprintf_s(szReply, MAX_PATH, _T("%s\\%s"), lpszPath, fdFileData.cFileName);
    strReply += szReply;
    //隐藏文件不显示
    /*
    if (fdFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
    {
      strReply += "<hidden>";
    }
    */
    if (fdFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      strReply += "    <dir>";
    }
    strReply += "\r\n";
  } while (FindNextFile(hFindFile, &fdFileData));
  FindClose(hFindFile);
  delete[] lpszPath;
  //发送头
  CMD_HEAD_INFO stHead;
  stHead.m_nCmd = CMD_SELECET_FILE;
  stHead.m_Len = strReply.length() + 1;
  int nbytes = send(nScoket, (char*)&stHead, sizeof(stHead), 0);
  if (nbytes == SOCKET_ERROR)
  {
    CTCPServer::ReportError(_T("send CMD_HEAD_INFO fail in the CFileHander::FindDriver "));
    return;
  }
  nbytes = send(nScoket, strReply.c_str(), stHead.m_Len, 0);
  if (nbytes == SOCKET_ERROR)
  {
    CTCPServer::ReportError(_T("send szReply fail in the CFileHander::FindDriver "));
    return;
  }

}

void CFileHander::UploadFile(int nScoket, int nLen)
{
  CMD_FILE_INFO stFileInfo;
  int nRet = recv(nScoket, (char*)&stFileInfo, nLen, 0);
  if (nRet < 0)
  {
    CTCPServer::ReportError(_T("recv lpszPath in the CFileHander::UploadFile"));
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
    CTCPServer::ReportError(_T("CreateFile in the CFileHander::UploadFile"));
    return;
  }
  //把上传的文件进行创建
  char szBuf[2048] = { 0 };
  DWORD dwWriteLen = 0;
  int nReadLen = 0;
  while (dwWriteLen < stFileInfo.m_nFileLen)
  {
    memset(szBuf, 0, 2048);
    nReadLen = recv(nScoket, szBuf, 2048, 0);
    if (nReadLen < 0)
    {
      CTCPServer::ReportError(_T("recv szBuf in the CFileHander::UploadFile"));
      return;
    }
    DWORD dwRealWriteLen = 0;
    WriteFile(hNewFile, szBuf, nReadLen, &dwRealWriteLen, NULL);
    if (nReadLen != dwRealWriteLen)
    {
      CTCPServer::ReportError(_T("WriteFile in the CFileHander::UploadFile"));
      return;
    }
    dwWriteLen += dwRealWriteLen;
  }
  CloseHandle(hNewFile);
}

void CFileHander::DownloadFile(int nScoket, int nLen)
{
  CMD_FILE_INFO stFileInfo;
  int nRet = recv(nScoket, (char*)&stFileInfo, nLen, 0);
  if (nRet < 0)
  {
    CTCPServer::ReportError(_T("recv lpszPath in the CFileHander::UploadFile"));
    return;
  }

  //打开要下载的文件
  HANDLE hOldFile = CreateFile(stFileInfo.szFilePath,//文件名
    GENERIC_READ | GENERIC_WRITE, //可读可写
    NULL,
    NULL,
    OPEN_EXISTING, //打开已经存在的文件
    FILE_ATTRIBUTE_NORMAL,
    NULL);
  if (hOldFile == INVALID_HANDLE_VALUE)
  {
    CTCPServer::ReportError(_T("CreateFile in the CFileHander::DownloadFile"));
    //发送错误码
    //发送头
    CMD_HEAD_INFO stHead;
    stHead.m_nCmd = CMD_ERROR_CODE;
    stHead.m_Len = sizeof(CMD_RETURN_CODE);
    int nbytes = send(nScoket, (char*)&stHead, sizeof(stHead), 0);
    if (nbytes == SOCKET_ERROR)
    {
      CTCPServer::ReportError(_T("send stHead fail in the CFileHander::DownloadFile "));
      return;
    }
    CMD_RETURN_CODE stCode;
    stCode.m_Code = -1;
    nbytes = send(nScoket, (char*)&stCode, sizeof(stCode), 0);
    if (nbytes == SOCKET_ERROR)
    {
      CTCPServer::ReportError(_T("send stCode fail in the CFileHander::DownloadFile "));
      return;
    }
    return;
  }
 
  //获取文件信息
  WIN32_FIND_DATA fdFileData;         /* 存储文件数据 */
  HANDLE hFindFile;   /* 文件句柄 */
  if ((hFindFile = FindFirstFile(stFileInfo.szFilePath, &fdFileData)) == INVALID_HANDLE_VALUE)
  {
    CTCPServer::ReportError(_T("FindFirstFile fail in the CRemoteFile::SendUpFile"));
    return;
  }

  //把下载的的文件进行传输
  CMD_HEAD_INFO stHead;
  stHead.m_nCmd = CMD_DOWNLOAD_FILE;
  stHead.m_Len = sizeof(CMD_FILE_INFO);
  int bytes;
  bytes = send(nScoket, (char*)&stHead, sizeof(stHead), 0);
  if (bytes < 0)
  {
    CTCPServer::ReportError(_T("send stHead in the CFileHander::UploadFile"));
    return;
  }

  CMD_FILE_INFO stSendFileInfo;
  strcpy_s(stSendFileInfo.szFilePath, strlen(fdFileData.cFileName) + 1, fdFileData.cFileName);
  stSendFileInfo.m_nFileLen = GetFileSize(hOldFile, NULL);//获取文件的大小
  bytes = send(nScoket, (char*)&stSendFileInfo, sizeof(stSendFileInfo), 0);
  if (bytes < 0)
  {
    CTCPServer::ReportError(_T("send stFileInfo in the CFileHander::UploadFile"));
    return;
  }

  char szBuf[2048] = { 0 };
  DWORD  dwReadLen = 0;
  while (TRUE)
  {
    memset(szBuf, 0, 2048);
    if (ReadFile(hOldFile, szBuf, 2048, &dwReadLen, NULL) && dwReadLen > 0)
    {
      int nRet = send(nScoket, szBuf, dwReadLen, 0);
      if (nRet < 0)
      {
        CTCPServer::ReportError(_T("send szBuf in the CFileHander::UploadFile"));
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

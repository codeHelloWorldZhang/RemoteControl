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
        strcpy_s(driveType, 21, "Ӳ��");
        break;
      case DRIVE_REMOVABLE:
        strcpy_s(driveType, 21, "�ƶ�Ӳ��");
        break;
      case DRIVE_CDROM:
        strcpy_s(driveType, 21, "����");
        break;
      case DRIVE_RAMDISK:
        strcpy_s(driveType, 21, "RAM��");
        break;
      case DRIVE_REMOTE:
        strcpy_s(driveType, 21, "Remote(Network) drive �������");
        break;
      case DRIVE_UNKNOWN:
      default:
        strcpy_s(driveType, 21, "δ֪��");
        break;
      }
      sprintf_s(pTemp, MAX_PATH, "%s  %s\r\n", rootPath, driveType);
      int nLen = strlen(rootPath) + strlen(driveType) + 2 + 2;
      pTemp += nLen;
    }
  }

  //����ͷ
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
  char szFilePath[MAX_PATH] = { 0 };      /* ����·��(������ʽ*.*) */
  char szAllPath[MAX_PATH] = { 0 };        /* �洢�����ļ��ľ���·�� */
  WIN32_FIND_DATA fdFileData;         /* �洢�ļ����� */
  HANDLE hFindFile;   /* �ļ���� */
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
    if (!strcmp(fdFileData.cFileName, _T("."))) /* �ҵ���Ϊ��ǰĿ¼. */
    {
      continue;
    }
    if (!strcmp(fdFileData.cFileName, _T("..")))/* �ҵ���Ϊ��Ŀ¼.. */
    {
      continue;
    }
    sprintf_s(szReply, MAX_PATH, _T("%s\\%s"), lpszPath, fdFileData.cFileName);
    strReply += szReply;
    //�����ļ�����ʾ
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
  //����ͷ
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

  //�������ļ�,Ĭ�Ϸ���c��
  string strFilePath = "C:\\";
  strFilePath += stFileInfo.szFilePath;
  HANDLE hNewFile = CreateFile(strFilePath.c_str(),//�ļ���
    GENERIC_READ | GENERIC_WRITE, //�ɶ���д
    NULL,
    NULL,
    CREATE_NEW, //�����µĿ����ļ�
    FILE_ATTRIBUTE_NORMAL,
    NULL);
  if (hNewFile == INVALID_HANDLE_VALUE)
  {
    CTCPServer::ReportError(_T("CreateFile in the CFileHander::UploadFile"));
    return;
  }
  //���ϴ����ļ����д���
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

  //��Ҫ���ص��ļ�
  HANDLE hOldFile = CreateFile(stFileInfo.szFilePath,//�ļ���
    GENERIC_READ | GENERIC_WRITE, //�ɶ���д
    NULL,
    NULL,
    OPEN_EXISTING, //���Ѿ����ڵ��ļ�
    FILE_ATTRIBUTE_NORMAL,
    NULL);
  if (hOldFile == INVALID_HANDLE_VALUE)
  {
    CTCPServer::ReportError(_T("CreateFile in the CFileHander::DownloadFile"));
    //���ʹ�����
    //����ͷ
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
 
  //��ȡ�ļ���Ϣ
  WIN32_FIND_DATA fdFileData;         /* �洢�ļ����� */
  HANDLE hFindFile;   /* �ļ���� */
  if ((hFindFile = FindFirstFile(stFileInfo.szFilePath, &fdFileData)) == INVALID_HANDLE_VALUE)
  {
    CTCPServer::ReportError(_T("FindFirstFile fail in the CRemoteFile::SendUpFile"));
    return;
  }

  //�����صĵ��ļ����д���
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
  stSendFileInfo.m_nFileLen = GetFileSize(hOldFile, NULL);//��ȡ�ļ��Ĵ�С
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

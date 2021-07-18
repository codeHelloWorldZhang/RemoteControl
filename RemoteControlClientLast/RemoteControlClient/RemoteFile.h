#pragma once
#include "protocol.h"
#include "Connect.h"
class CRemoteFile
{
public:
  CRemoteFile();
  ~CRemoteFile();
  void SendSelectDirver();//发送查找盘符数据包
  void SendSelectFile(char* szComand);//发送查找指定目录的数据包
  void ShowFile(HWND hWnd, CMD_HEAD_INFO head, int ID);//收到回复显示盘符或者文件
  void SendUpFile(char* pFilePath);//发送上传文件数据包
  void SendDownFile(char* pFilePath);//发送下载文件数据包
  void HandDownFile(CMD_HEAD_INFO head);
};


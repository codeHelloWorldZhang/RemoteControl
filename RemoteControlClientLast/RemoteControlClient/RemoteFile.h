#pragma once
#include "protocol.h"
#include "Connect.h"
class CRemoteFile
{
public:
  CRemoteFile();
  ~CRemoteFile();
  void SendSelectDirver();//���Ͳ����̷����ݰ�
  void SendSelectFile(char* szComand);//���Ͳ���ָ��Ŀ¼�����ݰ�
  void ShowFile(HWND hWnd, CMD_HEAD_INFO head, int ID);//�յ��ظ���ʾ�̷������ļ�
  void SendUpFile(char* pFilePath);//�����ϴ��ļ����ݰ�
  void SendDownFile(char* pFilePath);//���������ļ����ݰ�
  void HandDownFile(CMD_HEAD_INFO head);
};


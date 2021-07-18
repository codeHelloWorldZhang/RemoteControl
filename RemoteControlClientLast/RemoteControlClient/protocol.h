#pragma once
#include <windows.h>
enum COMMAND
{
  CMD_SCREEN,//��ͼ
  CMD_WM_MOUSEMOVE,//����ƶ�
  CMD_WM_LBUTTONDOWN,//����������
  CMD_WM_LBUTTONUP,//����������
  CMD_WM_LBUTTONDBLCLK,//������˫��
  CMD_WM_RIGHTBUTTONDOWN,//����Ҽ�����
  CMD_WM_RIGHTBUTTONUP,//����Ҽ�����
  CMD_WM_RIGHTBUTTONDBLCLK,//����Ҽ�˫��

  CMD_WM_KEYDOWN,//���̰���
  CMD_WM_KEYUP,//���̵���

  CMD_CREATE_CMD,//����cmd
  CMD_EXECUTE_CMD,//ִ��cmdָ��
  CMD_CLOSE_CMD,//�ر�cmd

  CMD_SELECET_DIRVER,//�鿴�̷�
  CMD_SELECET_FILE,//�鿴ָ��Ŀ¼
  CMD_UPLOAD_FILE,//�ϴ��ļ�
  CMD_DOWNLOAD_FILE,//�����ļ�

  CMD_ERROR_CODE,//������

};

struct CMD_HEAD_INFO
{
  int m_nCmd;
  int m_Len;
};

struct CMD_SCREEN_INFO
{
  int m_nWidth;
  int m_nHeight;
  int m_nTotalSize;
};
struct CMD_MOUSE_INFO
{
  int m_nX;
  int m_nY;
};

struct CMD_KEYBOARY_INFO
{
  int m_nVirKey;
};


struct CMD_FILE_INFO
{
  CMD_FILE_INFO()
  {
    m_nFileLen = 0;
    memset(szFilePath, 0, MAX_PATH);
  }
  int m_nFileLen;
  char szFilePath[MAX_PATH];
};

/*
-1 �ļ������� �ļ��򿪴���
*/
struct CMD_RETURN_CODE
{
  short m_Code;
};




#pragma once
#include <windows.h>
enum COMMAND
{
  CMD_SCREEN,//截图
  CMD_WM_MOUSEMOVE,//鼠标移动
  CMD_WM_LBUTTONDOWN,//鼠标左键按下
  CMD_WM_LBUTTONUP,//鼠标左键弹起
  CMD_WM_LBUTTONDBLCLK,//鼠标左键双击
  CMD_WM_RIGHTBUTTONDOWN,//鼠标右键按下
  CMD_WM_RIGHTBUTTONUP,//鼠标右键弹起
  CMD_WM_RIGHTBUTTONDBLCLK,//鼠标右键双击

  CMD_WM_KEYDOWN,//键盘按下
  CMD_WM_KEYUP,//键盘弹起

  CMD_CREATE_CMD,//创建cmd
  CMD_EXECUTE_CMD,//执行cmd指令
  CMD_CLOSE_CMD,//关闭cmd

  CMD_SELECET_DIRVER,//查看盘符
  CMD_SELECET_FILE,//查看指定目录
  CMD_UPLOAD_FILE,//上传文件
  CMD_DOWNLOAD_FILE,//下载文件

  CMD_ERROR_CODE,//错误码

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
-1 文件不存在 文件打开错误
*/
struct CMD_RETURN_CODE
{
  short m_Code;
};




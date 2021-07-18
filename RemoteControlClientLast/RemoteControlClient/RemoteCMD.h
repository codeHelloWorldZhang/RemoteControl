#pragma once
#include "Connect.h"
#include <string>
using namespace std;
class CRemoteCMD
{
public:
  CRemoteCMD();
  ~CRemoteCMD();
  void SendCreatCmd();//发送创建cmd程序指令
  void SendExeCmd(char* szComand);//发送执行cmd程序指令
  void ShowCmd(HWND hWnd, CMD_HEAD_INFO head,int ID);//收到回复显示cmd指令
};


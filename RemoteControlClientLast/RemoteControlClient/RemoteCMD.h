#pragma once
#include "Connect.h"
#include <string>
using namespace std;
class CRemoteCMD
{
public:
  CRemoteCMD();
  ~CRemoteCMD();
  void SendCreatCmd();//���ʹ���cmd����ָ��
  void SendExeCmd(char* szComand);//����ִ��cmd����ָ��
  void ShowCmd(HWND hWnd, CMD_HEAD_INFO head,int ID);//�յ��ظ���ʾcmdָ��
};


#pragma once
#include "TCPServer.h"
#include "protocol.h"
#include <string>
using namespace std;
class CCMDHander
{
public:
  CCMDHander();
  ~CCMDHander();
  void CreatCMD(int nScoket);//����cmd����
  void ExecuteCMD(int nScoket, int nLen);//ִ��cmdָ��
  void CloseCMD();
private:
  HANDLE m_hReadFather;
  HANDLE m_hWriteFather;
  HANDLE m_hReadSon;
  HANDLE m_hWriteSon;
  HANDLE m_hSonProcess;
};


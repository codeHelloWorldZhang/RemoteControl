#pragma once
#include "protocol.h"
#include <WinSock2.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
class CConnect
{
public:
  CConnect();
  ~CConnect();
  static void ShowMsg(const char* msg); 
  static void ReportError(const char* pre);
  static void HandError(CMD_HEAD_INFO stHead);
  LRESULT OnConnect(HWND hWnd);//连接
  LRESULT OnDisConnect(HWND hWnd);//关闭连接
};


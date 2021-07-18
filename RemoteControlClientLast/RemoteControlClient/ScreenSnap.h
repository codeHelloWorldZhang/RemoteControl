#pragma once
#include "Connect.h"
typedef int(*PFNUNCOMPRESS)(void*, unsigned long*, void*, unsigned long);
class CScreenSnap
{
public:
  CScreenSnap();
  ~CScreenSnap();
  //发送截图消息
  void SendScreen();

  //显示图片
  void ShowScreen(HWND hWnd, int width, int height, char* buffer);
  //收到截图回复处理
  int OnCmdScreen(HWND hWnd, CMD_HEAD_INFO head, PFNUNCOMPRESS pfnUncompress);
 
  //发送鼠标移动消息
  int WINAPI OnMyMouseMove(HWND hWnd, int x, int y, int nMark);
  
  //发送鼠标左键按下消息
  void WINAPI OnMouseLButtonDown(HWND hWnd, int x, int y, int nMark);
  
  //发送鼠标左键弹起消息
  void WINAPI OnMouseLButtonUp(HWND hWnd, int x, int y, int nMark);
  
  //发送鼠标左键双击消息
  void WINAPI OnMouseLButtonDoubleClick(HWND hWnd, int x, int y, int nMark);
  
  //发送鼠标右键按下消息
  void WINAPI OnMouseRightButtonDown(HWND hWnd, int x, int y, int nMark);
 
  //发送鼠标右键弹起消息
  void WINAPI OnMouseRightButtonUp(HWND hWnd, int x, int y, int nMark);
 
  //发送鼠标右键弹起消息
  void WINAPI OnMouseRightButtonDoubleClick(HWND hWnd, int x, int y, int nMark);
  
  //发送键盘按下消息
  void OnKeyDown(WPARAM wparam);
  
  //发送键盘弹起消息
  void OnKeyUp(WPARAM wparam);

};


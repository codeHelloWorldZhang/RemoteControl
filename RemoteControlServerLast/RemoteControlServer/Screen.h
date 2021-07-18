#pragma once
#include "TCPServer.h"
#include "protocol.h"
class CScreen
{
public:
  CScreen();
  ~CScreen();
  int ScreenSnap(int nScoket);//截图
  void OnMouseMove(int nScoket);//鼠标移动
  void OnMouseLButtonDown(int nScoket);//鼠标左键按下
  void OnMouseLButtonUp(int nScoket);//鼠标左键弹起
  void OnMouseLButtonDoubleClick(int nScoket);//鼠标左键双击
  void OnMouseRightButtonDown(int nScoket);//鼠标左键按下
  void OnMouseRightButtonUp(int nScoket);//鼠标左键弹起
  void OnMouseRightButtonDoubleClick(int nScoket);//鼠标左键双击

  void OnKeyDown(int nScoket);//键盘按下
  void OnKeyUp(int nScoket);//键盘弹起
};


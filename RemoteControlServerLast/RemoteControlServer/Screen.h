#pragma once
#include "TCPServer.h"
#include "protocol.h"
class CScreen
{
public:
  CScreen();
  ~CScreen();
  int ScreenSnap(int nScoket);//��ͼ
  void OnMouseMove(int nScoket);//����ƶ�
  void OnMouseLButtonDown(int nScoket);//����������
  void OnMouseLButtonUp(int nScoket);//����������
  void OnMouseLButtonDoubleClick(int nScoket);//������˫��
  void OnMouseRightButtonDown(int nScoket);//����������
  void OnMouseRightButtonUp(int nScoket);//����������
  void OnMouseRightButtonDoubleClick(int nScoket);//������˫��

  void OnKeyDown(int nScoket);//���̰���
  void OnKeyUp(int nScoket);//���̵���
};


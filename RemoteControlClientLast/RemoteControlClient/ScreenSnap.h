#pragma once
#include "Connect.h"
typedef int(*PFNUNCOMPRESS)(void*, unsigned long*, void*, unsigned long);
class CScreenSnap
{
public:
  CScreenSnap();
  ~CScreenSnap();
  //���ͽ�ͼ��Ϣ
  void SendScreen();

  //��ʾͼƬ
  void ShowScreen(HWND hWnd, int width, int height, char* buffer);
  //�յ���ͼ�ظ�����
  int OnCmdScreen(HWND hWnd, CMD_HEAD_INFO head, PFNUNCOMPRESS pfnUncompress);
 
  //��������ƶ���Ϣ
  int WINAPI OnMyMouseMove(HWND hWnd, int x, int y, int nMark);
  
  //����������������Ϣ
  void WINAPI OnMouseLButtonDown(HWND hWnd, int x, int y, int nMark);
  
  //����������������Ϣ
  void WINAPI OnMouseLButtonUp(HWND hWnd, int x, int y, int nMark);
  
  //����������˫����Ϣ
  void WINAPI OnMouseLButtonDoubleClick(HWND hWnd, int x, int y, int nMark);
  
  //��������Ҽ�������Ϣ
  void WINAPI OnMouseRightButtonDown(HWND hWnd, int x, int y, int nMark);
 
  //��������Ҽ�������Ϣ
  void WINAPI OnMouseRightButtonUp(HWND hWnd, int x, int y, int nMark);
 
  //��������Ҽ�������Ϣ
  void WINAPI OnMouseRightButtonDoubleClick(HWND hWnd, int x, int y, int nMark);
  
  //���ͼ��̰�����Ϣ
  void OnKeyDown(WPARAM wparam);
  
  //���ͼ��̵�����Ϣ
  void OnKeyUp(WPARAM wparam);

};


#pragma once
class CFileHander
{
public:
  CFileHander();
  ~CFileHander();
  void FindDriver(int nScoket);//�����̷�
  void FindDesignationFile(int nScoket, int nLen);//����ָ��Ŀ¼�µ������ļ�
  void UploadFile(int nScoket, int nLen);//�ϴ��ļ�
  void DownloadFile(int nScoket, int nLen);//�����ļ�
};


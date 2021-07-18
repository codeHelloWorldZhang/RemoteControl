#pragma once
class CFileHander
{
public:
  CFileHander();
  ~CFileHander();
  void FindDriver(int nScoket);//查找盘符
  void FindDesignationFile(int nScoket, int nLen);//查找指定目录下的所有文件
  void UploadFile(int nScoket, int nLen);//上传文件
  void DownloadFile(int nScoket, int nLen);//下载文件
};


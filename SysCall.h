#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "FileManager.h"
#include <string>

#define NOERROR 0
using namespace std;

class SysCall {
    FileManager *m_fileManager;


public:
    SysCall();

    ~SysCall();

    void Initialize();

    void sysLs();

    void sysCd(string dirName);

    void sysMkDir(string dirName);

    void sysCreate(string fileName);

    void sysDelete(string fileName);

    void sysOpen(string fileName);

    void sysClose(string fd);

    void sysSeek(string fd, string offset, string origin);

    void sysWrite(string fd, string inFile, string size);

    void sysRead(string fd, string outFile, string size);

    void sysRename(string ori, string cur);  //重命名文件、文件夹
    void sysTree(string path);               //打印树状目录

private:
    bool checkError();

    bool checkPathName(string path);

    void __sysCd__(string dirName);

    void __sysTree__(string path, int depth);//内部打印树状目录
};
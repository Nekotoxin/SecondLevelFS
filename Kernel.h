//
// Created by nekotoxin on 23-3-31.
//

#ifndef SECONDLEVELFS_KERNEL_H
#define SECONDLEVELFS_KERNEL_H

#include "DiskDriver.h"
#include "BufferManager.h"
#include "FileSystem.h"
#include "FileManager.h"
#include "SysCall.h"
#include "UserManager.h"


class Kernel {
public:
// 定义一些全局常量


// 工具函数
    Kernel();

    ~Kernel();

    static Kernel &Instance();

    void Initialize(); //文件系统初始化
    void Quit();       //退出文件系统
// Kernel的子组件
    DiskDriver &GetDiskDriver();

    BufferManager &GetBufferManager();

    FileSystem &GetFileSystem();

    FileManager &GetFileManager();

    UserManager& GetUserManager();
//    User& GetSuperUser();
    User& GetUser(); //作为单体实例，单用户时放在这里，多用户时放在线程局部数据中
//    UserManager& GetUserManager();

// Kernel提供的文件系统API
//    FD Sys_Open(std::string& fpath,int mode=File::FWRITE);
//    int Sys_Close(FD fd);
//    int Sys_Creat(std::string& fpath,int mode=File::FWRITE);
//    int Sys_Delete(std::string& fpath);
//    int Sys_Read(FD fd, size_t size, size_t nmemb, void* ptr);
//    int Sys_Write(FD fd, size_t size, size_t nmemb, void* ptr);
//    /*whence : 0 设为offset；1 加offset；2 文件结束位置加offset*/
//    int Sys_Seek(FD fd, long int offset, int whence);
//    int Sys_CreatDir(std::string &fpath);

private:
// Kernel子组件的初始化函数

private:
    static Kernel instance; // 单体实例

// 指向子组件的指针
    DiskDriver *m_DiskDriver;
    BufferManager *m_BufferManager;
    FileSystem *m_FileSystem;
    FileManager *m_FileManager;
    UserManager *m_UserManager;
//    User* m_User;
//    UserManager* m_UserManager;
};

#endif //SECONDLEVELFS_KERNEL_H

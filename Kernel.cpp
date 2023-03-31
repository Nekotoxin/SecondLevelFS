//
// Created by nekotoxin on 23-3-31.
//

#include "Kernel.h"

Kernel Kernel::instance;
DiskDriver g_DiskDriver;
BufferManager g_BufferManager;
FileSystem g_FileSystem;
FileManager g_FileManager;
UserManager g_UserManager;
extern SysCall g_UserCall;
extern INodeTable g_INodeTable;


Kernel::Kernel() {

}

Kernel::~Kernel() {

}

Kernel &Kernel::Instance() {
    return instance;
}

DiskDriver &Kernel::GetDiskDriver() {
    return *(this->m_DiskDriver);
}

BufferManager &Kernel::GetBufferManager() {
    return *(this->m_BufferManager);
}

FileSystem &Kernel::GetFileSystem() {
    return *(this->m_FileSystem);
}

FileManager &Kernel::GetFileManager() {
    return *(this->m_FileManager);
}


void Kernel::Initialize() {
    this->m_DiskDriver = &g_DiskDriver;
    this->m_BufferManager = &g_BufferManager;
    g_BufferManager.Initialize();
    this->m_FileSystem = &g_FileSystem;
    g_FileSystem.Initialize();
    this->m_UserManager = &g_UserManager;
    this->m_FileManager = &g_FileManager;
    g_FileManager.Initialize();
    g_UserCall.Initialize();

    User* u=Kernel::Instance().GetUserManager().GetUser();
    u->u_cdir = g_INodeTable.IGet(FileSystem::ROOT_INODE_NO);
    u->u_curdir = "/";
    printf("[INFO] root 登录成功.\n", pthread_self());
    printf("[info] 文件系统初始化完毕.\n");
}

UserManager &Kernel::GetUserManager() {
    return *(this->m_UserManager);
}




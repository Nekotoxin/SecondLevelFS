//
// Created by nekotoxin on 23-3-31.
//

#include "Kernel.h"

Kernel Kernel::instance;
DiskDriver g_DiskDriver;
BufferManager g_BufferManager;
FileSystem g_FileSystem;
FileManager g_FileManager;
extern UserCall g_UserCall;




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
    this->m_FileManager = &g_FileManager;
    g_FileManager.Initialize();
    g_UserCall.Initialize();
    printf("[info] 文件系统初始化完毕.\n");
}





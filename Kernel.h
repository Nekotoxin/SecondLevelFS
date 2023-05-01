/* 
 * Created by nekotoxin on 23-3-31.
 * */

#ifndef SECONDLEVELFS_KERNEL_H
#define SECONDLEVELFS_KERNEL_H

#include "DiskDriver.h"
#include "BufferManager.h"
#include "FileSystem.h"
#include "FileManager.h"
#include "UserManager.h"


class Kernel {
public:
    Kernel() = default;

    ~Kernel() = default;

    static Kernel &Instance();

    void Initialize(); /* 文件系统初始化 */
    void Quit();       /* 退出文件系统 */
    /*  Kernel的子组件 */
    DiskDriver &GetDiskDriver();

    BufferManager &GetBufferManager();

    FileSystem &GetFileSystem();

    FileManager &GetFileManager();

    UserManager &GetUserManager();

    User &GetUser(); /* 作为单体实例，单用户时放在这里，多用户时放在线程局部数据中 */

private:
    static Kernel instance; /*  单体实例 */

/*  指向子组件的指针 */
    DiskDriver *m_DiskDriver;
    BufferManager *m_BufferManager;
    FileSystem *m_FileSystem;
    FileManager *m_FileManager;
    UserManager *m_UserManager;
/* User* m_User;
 * UserManager* m_UserManager; */
public:

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

    void sysRename(string ori, string cur);  /* 重命名文件、文件夹 */
    void sysTree(string path);               /* 打印树状目录 */

private:
    bool checkError();

    bool checkPathName(string path);

    void __sysCd__(string dirName);

    void __sysTree__(string path, int depth);/* 内部打印树状目录 */
};

#endif /* SECONDLEVELFS_KERNEL_H */

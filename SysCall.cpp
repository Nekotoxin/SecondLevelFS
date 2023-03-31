#include "FileManager.h"
#include "SysCall.h"
#include "Utility.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "Kernel.h"
#include "User.h"

using namespace std;

SysCall g_UserCall;
//    extern是一个关键字，它告诉编译器存在着一个变量或者一个函数，如果在当前编译语句的前面
//中没有找到相应的变量或者函数，也会在当前文件的后面或者其它文件中定义

SysCall::SysCall() {

}

void SysCall::Initialize() {
//    User& u 
//    u->u_error = NOERROR;  //存放错误码
    m_fileManager = &Kernel::Instance().GetFileManager();
//
//    u->u_dirp = "/";                                   //系统调用参数(一般用于Pathname)的指针
//    u->u_curdir = "/";                             //当前工作目录完整路径
//    u->u_cdir = m_fileManager->rootDirINode;//指向当前目录的Inode指针
//    u->u_pdir = NULL;                     //指向父目录的Inode指针
//    memset(u->u_arg, 0, sizeof(u->u_arg));                  //指向核心栈现场保护区中EAX寄存器
}


SysCall::~SysCall() {}

//此函数改变Usercall对象的dirp(系统调用参数 一般用于Pathname)数据成员
//只检查文件名是否过长
bool SysCall::checkPathName(string path) {
    if (path.empty()) {
        cout << "参数路径为空" << endl;
        return false;
    }
    User* u=Kernel::Instance().GetUserManager().GetUser();
    if (path[0] == '/' || path.substr(0, 2) != "..")
        u->u_dirp = path;            //系统调用参数(一般用于Pathname)的指针
    else {
        if (u->u_curdir.back() != '/')
            u->u_curdir += '/';
        string pre = u->u_curdir;//当前工作目录完整路径 cd命令才会改变curDirPath的值
        unsigned int p = 0;
        //可以多重相对路径 .. ../../
        for (; pre.length() > 3 && p < path.length() && path.substr(p, 2) == "..";) {
            pre.pop_back();
            pre.erase(pre.find_last_of('/') + 1);
            p += 2;
            p += p < path.length() && path[p] == '/';
        }
        u->u_dirp = pre + path.substr(p);
    }

    if (u->u_dirp.length() > 1 && u->u_dirp.back() == '/')
        u->u_dirp.pop_back();

    for (unsigned int p = 0, q = 0; p < u->u_dirp.length(); p = q + 1) {
        q = u->u_dirp.find('/', p);
        q = Utility::min(q, (unsigned int) u->u_dirp.length());
        if (q - p > DirectoryEntry::DIRSIZ) {
            cout << "文件名或文件夹名过长" << endl;
            return false;
        }
    }
    return true;
}

void SysCall::sysMkDir(string dirName) {
    if (!checkPathName(dirName))
        return;
    User* u=Kernel::Instance().GetUserManager().GetUser();
    u->u_arg[1] = INode::IFDIR;//存放当前系统调用参数 文件类型：目录文件
    m_fileManager->Creat();
    checkError();
}

void SysCall::sysLs() {
    User* u=Kernel::Instance().GetUserManager().GetUser();
    cout<<u->ls.empty()<<endl;
    if(!u->ls.empty())
        u->ls.clear();
    m_fileManager->Ls();
    if (checkError())
        return;
    cout << u->ls << endl;
}

void SysCall::sysRename(string ori, string cur) {
    User* u=Kernel::Instance().GetUserManager().GetUser();
    string curDir = u->u_curdir;
    if (!checkPathName(ori))
        return;
    if (!checkPathName(cur))
        return;
    if (ori.find('/') != string::npos) {
        string nextDir = ori.substr(0, ori.find_last_of('/'));
        sysCd(nextDir);
        ori = ori.substr(ori.find_last_of('/') + 1);
        if (cur.find('/') != string::npos)
            cur = cur.substr(cur.find_last_of('/') + 1);
    }
    m_fileManager->Rename(ori, cur);
    sysCd(curDir);
    if (checkError())
        return;
}

void SysCall::__sysTree__(string path, int depth) {
    User* u=Kernel::Instance().GetUserManager().GetUser();
    vector<string> dirs;
    string nextDir;
    u->ls.clear();
    m_fileManager->Ls();
    for (int i = 0, p = 0; i < u->ls.length();) {
        p = u->ls.find('\n', i);
        dirs.emplace_back(u->ls.substr(i, p - i));
        i = p + 1;
    }
    for (int i = 0; i < dirs.size(); i++) {
        nextDir = (path == "/" ? "" : path) + '/' + dirs[i];
        for (int i = 0; i < depth + 1; i++)
            cout << "|   ";
        cout << "|---" << dirs[i] << endl;
        __sysCd__(nextDir);
        if (u->u_error != NOERROR) {//访问到文件
            u->u_error = NOERROR;
            continue;
        }
        __sysTree__(nextDir, depth + 1);
    }
    __sysCd__(path);
    return;
}

void SysCall::sysTree(string path) {
    User* u=Kernel::Instance().GetUserManager().GetUser();
    if (u->u_curdir.length() > 1 && u->u_curdir.back() == '/')
        u->u_curdir.pop_back();
    string curDir = u->u_curdir;

    if (path == "")
        path = curDir;

    if (!checkPathName(path))
        return;

    path = u->u_dirp;
    __sysCd__(path);
    if (u->u_error != NOERROR) {//访问到文件
        cout << "目录路径不存在！" << endl;
        u->u_error = NOERROR;
        __sysCd__(curDir);
        return;
    }
    cout << "|---" << (path == "/" ? "/" : path.substr(path.find_last_of('/') + 1)) << endl;
    __sysTree__(path, 0);
    __sysCd__(curDir);
}

void SysCall::__sysCd__(string dirName) {
    if (!checkPathName(dirName))
        return;
    m_fileManager->ChDir();
}

void SysCall::sysCd(string dirName) {
    if (!checkPathName(dirName))
        return;
    m_fileManager->ChDir();
    checkError();
}

void SysCall::sysCreate(string fileName) {
    User* u=Kernel::Instance().GetUserManager().GetUser();
    if (!checkPathName(fileName))
        return;
    u->u_arg[1] = (INode::IREAD | INode::IWRITE);//存放当前系统调用参数
    m_fileManager->Creat();
    checkError();
}

void SysCall::sysDelete(string fileName) {
    if (!checkPathName(fileName))
        return;
    m_fileManager->UnLink();
    checkError();
}

void SysCall::sysOpen(string fileName) {
    User* u=Kernel::Instance().GetUserManager().GetUser();
    if (!checkPathName(fileName))
        return;
    u->u_arg[1] = (File::FREAD | File::FWRITE);//存放当前系统调用参数
    m_fileManager->Open();
    if (checkError())
        return;
    cout << "打开文件成功，返回的文件句柄fd为 " << u->u_ar0[User::EAX] << endl;
}

//传入sfd句柄
void SysCall::sysClose(string fd) {
    User* u=Kernel::Instance().GetUserManager().GetUser();
    u->u_arg[0] = stoi(fd);//存放当前系统调用参数
    m_fileManager->Close();
    checkError();
}

void SysCall::sysSeek(string fd, string offset, string origin) {
    User* u=Kernel::Instance().GetUserManager().GetUser();
    u->u_arg[0] = stoi(fd);
    u->u_arg[1] = stoi(offset);
    u->u_arg[2] = stoi(origin);
    m_fileManager->Seek();
    checkError();
}

void SysCall::sysWrite(string sfd, string inFile, string size) {
    User* u=Kernel::Instance().GetUserManager().GetUser();
    int fd = stoi(sfd), usize = 0;
    if (size.empty() || (usize = stoi(size)) < 0) {
        cout << "参数必须大于等于零 ! \n";
        return;
    }
    char *buffer = new char[usize];
    fstream fin(inFile, ios::in | ios::binary);
    if (!fin.is_open()) {
        cout << "打开文件" << inFile << "失败" << endl;
        return;
    }
    fin.read(buffer, usize);
    fin.close();
    u->u_arg[0] = fd;
    u->u_arg[1] = (int) buffer;
    u->u_arg[2] = usize;
    m_fileManager->Write();

    if (checkError())
        return;
    cout << "成功写入" << u->u_ar0[User::EAX] << "字节" << endl;
    delete[] buffer;
}

void SysCall::sysRead(string sfd, string outFile, string size) {
    User* u=Kernel::Instance().GetUserManager().GetUser();
    int fd = stoi(sfd);
    int usize = stoi(size);
    char *buffer = new char[usize];
    u->u_arg[0] = fd;
    u->u_arg[1] = (int) buffer;
    u->u_arg[2] = usize;
    m_fileManager->Read();
    if (checkError())
        return;

    cout << "成功读出" << u->u_ar0[User::EAX] << "字节" << endl;
    if (outFile == "std") {
        for (uint32 i = 0; i < u->u_ar0[User::EAX]; ++i)
            cout << (char) buffer[i];
        cout << endl;
        delete[] buffer;
        return;
    } else {
        fstream fout(outFile, ios::out | ios::binary);
        if (!fout) {
            cout << "打开文件" << outFile << "失败" << endl;
            return;
        }
        fout.write(buffer, u->u_ar0[User::EAX]);
        fout.close();
        delete[] buffer;
        return;
    }
}

bool SysCall::checkError() {
    User* u=Kernel::Instance().GetUserManager().GetUser();
    if (u->u_error != NOERROR) {
        switch (u->u_error) {
            case ENOENT:
                cout << "找不到文件或者文件夹" << endl;
                break;
            case EBADF:
                cout << "找不到文件句柄" << endl;
                break;
            case EACCES:
                cout << "权限不足" << endl;
                break;
            case ENOTDIR:
                cout << "文件夹不存在" << endl;
                break;
            case ENFILE:
                cout << "文件表溢出" << endl;
                break;
            case EMFILE:
                cout << "打开文件过多" << endl;
                break;
            case EFBIG:
                cout << "文件过大" << endl;
                break;
            case ENOSPC:
                cout << "磁盘空间不足" << endl;
                break;
            default:
                break;
        }

        u->u_error = NOERROR;
        return true;
    }
    return false;
}



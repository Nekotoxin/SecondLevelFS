#ifndef SECONDLEVELFS_USER_H
#define SECONDLEVELFS_USER_H

#include "FileManager.h"

#define NOERROR 0

typedef int ErrorCode;

class User {
public:
    static const int EAX = 0;/* u.u_ar0[EAX]；访问现场保护区中EAX寄存器的偏移量 */

///* enum ErrorCode
// * {
// * 	NOERROR	= 0,	/* No error */
// * 	EPERM	= 1,	/* Operation not permitted */
// * 	ENOENT	= 2,	/* No such file or directory */
// * 	ESRCH	= 3,	/* No such process */
// * 	EINTR	= 4,	/* Interrupted system call */
// * 	EIO		= 5,	/* I/O error */
// * 	ENXIO	= 6,	/* No such device or address */
// * 	E2BIG	= 7,	/* Arg list too long */
// * 	ENOEXEC	= 8,	/* Exec format error */
// * 	EBADF	= 9,	/* Bad file number */
// * 	EAGAIN	= 11,	/* Try again */
// * 	ENOMEM	= 12,	/* Out of memory */
// * 	EACCES	= 13,	/* Permission denied */
// * 	EFAULT  = 14,	/* Bad address */
// * 	ENOTBLK	= 15,	/* Block device required */
// * 	EBUSY 	= 16,	/* Device or resource busy */
// * 	EEXIST	= 17,	/* File exists */
// * 	EXDEV	= 18,	/* Cross-device link */
// * 	ENODEV	= 19,	/* No such device */
// * 	ENOTDIR	= 20,	/* Not a directory */
// * 	EISDIR	= 21,	/* Is a directory */
// * 	EINVAL	= 22,	/* Invalid argument */
// * 	ENFILE	= 23,	/* File table overflow */
// * 	EMFILE	= 24,	/* Too many open files */
// * 	ENOTTY	= 25,	/* Not a typewriter(terminal) */
// * 	ETXTBSY	= 26,	/* Text file busy */
// * 	EFBIG	= 27,	/* File too large */
// * 	ENOSPC	= 28,	/* No space left on device */
// * 	ESPIPE	= 29,	/* Illegal seek */
// * 	EROFS	= 30,	/* Read-only file system */
// * 	EMLINK	= 31,	/* Too many links */
// * 	EPIPE	= 32,	/* Broken pipe */
// * 	ENOSYS	= 100,
// * 	//EFAULT	= 106
// * }; */

public:
    INode *u_cdir;        /* 指向当前目录的Inode指针 */
    INode *u_pdir;         /* 指向父目录的Inode指针 */
    DirectoryEntry u_dent;              /* 当前目录的目录项 */
    char u_dbuf[DirectoryEntry::DIRSIZ];/* 当前路径分量 */
    string u_curdir;                /* 当前工作目录完整路径 */
    string u_dirp;                      /* 系统调用参数(一般用于Pathname)的指针 */

    long long u_arg[5];                       /* 存放当前系统调用参数 */
    /* 系统调用相关成员 */
    uint32 u_ar0[5];                    /*     指向核心栈现场保护区中EAX寄存器 */
    /* 存放的栈单元，本字段存放该栈单元的地址。
     * 在V6中r0存放系统调用的返回值给用户程序，
     * x86平台上使用EAX存放返回值，替代u.u_ar0[R0] */
    ErrorCode u_error;                  /* 存放错误码 */
    OpenFiles u_ofiles;                 /* 进程打开文件描述符表对象 */
    IOParameter u_IOParam;              /* 记录当前读、写文件的偏移量，用户目标区域和剩余字节数参数 */
    string ls;

    short u_uid;        /* 有效用户ID */
};

#endif /* SECONDLEVELFS_USER_H */

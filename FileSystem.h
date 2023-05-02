#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "INode.h"
#include "BufferManager.h"
#include "DiskDriver.h"

/*
 * 文件系统存储资源管理块(Super Block)的定义。
 */
class SuperBlock {
/* Functions */
public:
    /* Constructors */
    SuperBlock();
    /* Destructors */
    ~SuperBlock();

public:
    int s_isize;                /* 外存INode区占用的盘块数 1022 */
    int s_fsize;                /* 文件系统的数据盘块总数  16384 - 1024 = 15360 */

    int s_nfree;                /* 直接管理的空闲盘块数量 */
    int s_free[100];  /* 直接管理的空闲盘块索引表 */

    int s_ninode;                   /* 直接管理的空闲外存INode数量 */
    int s_inode[100];     /* 直接管理的空闲外存INode索引表 */

    int s_fmod;                     /* 内存中super block副本被修改标志，意味着需要更新外存对应的Super Block */
    int s_time;                     /* 最近一次更新时间 */
    int padding[18];                /* 填充使SuperBlock块大小等于1024字节，占据2个扇区 */ // macos
    //ubuntu :38

    pthread_mutex_t s_flock;        /* 封锁空闲盘块索引表标志 */
    pthread_mutex_t s_ilock;        /* 封锁空闲Inode表标志 */
};

/* DiskINode节点的索引结构 32字节 */
class DirectoryEntry {
public:
    static const int DIRSIZ = 28;/* 目录项中路径部分的最大字符串长度 */
public:
    int m_ino;                   /* 目录项中INode编号部分，即对应文件在块设备上的外存索引节点号 */
    char name[DIRSIZ];           /* 目录项中路径名部分 */
};

/*
 * 文件系统类(FileSystem)管理文件存储设备中
 * 的各类存储资源，磁盘块、外存INode的分配、
 * 释放。
 */
class FileSystem {
public:
    /* static consts */
    static const int BLOCK_SIZE = 512;           /* Block块大小，单位字节 */
    static const int SUPER_BLOCK_SECTOR_NUMBER = 0;/* 定义SuperBlock位于磁盘上的扇区号，占据两个扇区 */
    static const int ROOTINO = 0;          /* 文件系统根目录外存INode编号 */

    static const int INODE_NUMBER_PER_SECTOR = 8;       /* 外存INode对象长度为64字节，每个磁盘块可以存放512/64 = 8个外存Inode */
    static const int INODE_ZONE_START_SECTOR = 2;       /* 外存INode区位于磁盘上的起始扇区号 */
    static const int INODE_ZONE_SIZE = 1022;            /* 磁盘上外存INode区占据的扇区数 */

    static const int DATA_START_SECTOR = INODE_ZONE_START_SECTOR + INODE_ZONE_SIZE;     /* 数据区的起始扇区号 */
    static const int DATA_END_SECTOR = 16384 - 1;                                       /* 数据区的最后扇区号 8MB / 512B = 16384 */
    static const int DATA_SECTOR_NUMBER = 16384 - DATA_START_SECTOR;                    /* 数据区占据的扇区数量 */

    FileSystem() = default;

    ~FileSystem();

    void Format();    /* 格式化整个文件系统 */
    void Initialize();

    void SpbLoad();             /* 系统初始化时读入SuperBlock */
    void SpbFlush();            /* 将SuperBlock对象的内存副本更新到存储设备的SuperBlock中去 */

    /* 磁盘Inode节点的分配与回收算法设计与实现 */
    INode *IAlloc();        /* 在存储设备上分配一个空闲外存INode，一般用于创建新的文件 */
    void IFree(int number); /* 释放编号为number的外存INode，一般用于删除文件 */

    Buf *Alloc();               /* 在存储设备上分配空闲磁盘块 */
    void Free(int blkno);       /* 释放存储设备上编号为blkno的磁盘块 */

    SuperBlock *GetFS();

private:
    DiskDriver *m_diskDriver;
    SuperBlock *m_superBlock;
    BufferManager *m_bufferManager;
};

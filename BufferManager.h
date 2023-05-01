#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "DiskDriver.h"
#include "Buf.h"
#include <unordered_map>

using namespace std;

class BufferManager {
public:
    static const int NBUF = 100;            /* 缓存控制块、缓冲区的数量 */
    static const int BUFFER_SIZE = 512;     /* 缓冲区大小。 以字节为单位 */

private:
    Buf *m_bufList;                 /* 自由缓存队列控制块 */
    Buf m_Buf[NBUF];                /* 缓存控制块数组 */
    unsigned char buffer[NBUF][BUFFER_SIZE];/* 缓冲区数组 */
    unordered_map<int, Buf *> m_blknoBufMap;
    DiskDriver *m_diskDriver;

public:
    BufferManager();

    ~BufferManager();

    void Initialize();

    Buf *GetBlk(int blkno);         /* 申请一块缓存，用于读写设备上的块blkno */
    void Brelse(Buf *bp);           /* 释放缓存控制块buf */
    Buf *Bread(int blkno);          /* 读一个磁盘块，blkno为目标磁盘块逻辑块号 */
    void Bwrite(Buf *bp);           /* 写一个磁盘块 */
    void Bdwrite(Buf *bp);          /* 延迟写磁盘块 */
    void Bclear(Buf *bp);           /* 清空缓冲区内容 */
    void Bflush();                         /* 将队列中延迟写的缓存全部输出到磁盘 */
    void FormatBuffer();                   /* 格式化所有Buffer */

private:
    void InitList();                 /* 缓存控制块队列的初始化 */
    void DetachNode(Buf *pb);
};
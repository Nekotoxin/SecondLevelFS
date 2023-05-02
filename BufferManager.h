#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "DiskDriver.h"
#include "Buf.h"
#include <unordered_map>

using namespace std;

class BufferManager {
public:
    static const int NBUF = 100;            /* 缓存控制块数量 */
    static const int BUFFER_SIZE = 512;     /* 缓冲区大小 */

public:
    BufferManager();

    ~BufferManager();

    void Initialize();

    Buf *GetBlk(int blkno);         /* 申请一块缓存（读写blkno扇区） */
    void Brelse(Buf *bp);           /* 释放buf */
    Buf *Bread(int blkno);          /* 读blkno扇区 */
    void Bwrite(Buf *bp);           /* 写磁盘 */
    void Bdwrite(Buf *bp);          /* 延迟写磁盘 */
    void Bclear(Buf *bp);           /* 清空缓冲区 */
    void Bflush();                         /* 延迟写缓存全部flush到磁盘 */
    void InitializeBuffer();                   /* 初始化缓冲区 */
    void InitializeQueue();                 /* 初始化缓存队列 */
    void DetachNode(Buf *bp);               /* 分离控制块pb */

private:
    Buf *m_bufList;                 /* 自由缓存队列控制块 */
    Buf m_Buf[NBUF];                /* 缓存控制块数组 */
    unsigned char buffer[NBUF][BUFFER_SIZE];    /* 缓冲区数组 */
    unordered_map<int, Buf *> m_blknoBufMap;    /* 逻辑块号到缓存控制块的映射 */
    DiskDriver *m_diskDriver;
};
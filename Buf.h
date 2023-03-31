#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <iostream>
#include <pthread.h>

using namespace std;

/* 缓存块
*     缓存对应的物理盘号，缓存的标志（是否已经和硬盘同步，或者是延迟写），
* 缓存编号以及缓存的前后指针等信息。
*/
//分配和释放的操作也非常简单，分配是从队列头取第一个缓存块，释放时将该缓存块标志位置换后放在队列尾部。
class Buf {
public:
    //flags中的标志位
    enum BufFlag	/* b_flags中标志位 */
    {
        B_WRITE = 0x1,		/* 写操作。将缓存中的信息写到硬盘上去 */
        B_READ	= 0x2,		/* 读操作。从盘读取信息到缓存中 */
        B_DONE	= 0x4,		/* I/O操作结束 */
        B_ERROR	= 0x8,		/* I/O因出错而终止 */
        B_BUSY	= 0x10,		/* 相应缓存正在使用中 */
        B_WANTED = 0x20,	/* 有进程正在等待使用该buf管理的资源，清B_BUSY标志时，要唤醒这种进程 */
        B_ASYNC	= 0x40,		/* 异步I/O，不需要等待其结束 */
        B_DELWRI = 0x80		/* 延迟写，在相应缓存要移做他用时，再将其内容写到相应块设备上 */
    };
    unsigned int flags;   //缓存控制块标志位

    Buf *forw;
    Buf *back;

    int wcount;              //需传送的字节数
    unsigned char *addr;  //指向该缓存控制块所管理的缓冲区的首地址
    int blkno;              //磁盘逻辑块号
    int no;
//    pthread_mutex_t buf_lock;

    Buf() {
        flags = 0;
        forw = NULL;
        back = NULL;
        wcount = 0;
        addr = NULL;
        blkno = -1;
        no = 0;
    }

    void Reset() {
        flags = 0;
        forw = NULL;
        back = NULL;
        wcount = 0;
        addr = NULL;
        blkno = -1;
        no = 0;
    }
};
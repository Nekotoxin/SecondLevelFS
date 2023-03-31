#include "BufferManager.h"
#include "Utility.h"
#include "Kernel.h"

//CacheBlock只用到了两个标志，B_DONE和B_DELWRI，分别表示已经完成IO和延迟写的标志。
//空闲Buffer无任何标志
BufferManager::BufferManager() {

}

void BufferManager::Initialize() {
    m_bufList = new Buf;
    InitList();
    m_diskDriver = &Kernel::Instance().GetDiskDriver();
}

BufferManager::~BufferManager() {
    Bflush();
    delete m_bufList;
}

void BufferManager::FormatBuffer() {
    for (int i = 0; i < NBUF; ++i)
        m_Buf[i].Reset();
    InitList();
}

void BufferManager::InitList() {
    for (int i = 0; i < NBUF; ++i) {
        if (i)
            m_Buf[i].forw = m_Buf + i - 1;
        else {
            m_Buf[i].forw = m_bufList;
            m_bufList->back = m_Buf + i;
        }

        if (i + 1 < NBUF)
            m_Buf[i].back = m_Buf + i + 1;
        else {
            m_Buf[i].back = m_bufList;
            m_bufList->forw = m_Buf + i;
        }
        m_Buf[i].addr = buffer[i];
        m_Buf[i].no = i;
        pthread_mutex_init(&m_Buf[i].buf_lock, NULL);
    }
}

//采用LRU Cache 算法，每次从头部取出，使用后放到尾部
void BufferManager::DetachNode(Buf *pb) {
    if (pb->back == NULL)
        return;
    pb->forw->back = pb->back;
    pb->back->forw = pb->forw;
    pb->back = NULL;
    pb->forw = NULL;
}


//申请一块缓存，从缓存队列中取出，用于读写设备上的块blkno
Buf *BufferManager::GetBlk(int blkno) {
    Buf *pb;
    if (m_blknoBufMap.find(blkno) != m_blknoBufMap.end()) {
        pb = m_blknoBufMap[blkno];
        pthread_mutex_lock(&pb->buf_lock); // P操作, 加锁
        DetachNode(pb);
        return pb;
    }
    pb = m_bufList->back;
    if (pb == m_bufList) {
        cout << "无缓存块可供使用" << endl;
        return NULL;
    }
    pthread_mutex_lock(&pb->buf_lock); // P操作, 加锁
    DetachNode(pb);
    m_blknoBufMap.erase(pb->blkno);
    if (pb->flags & Buf::B_DELWRI)
        m_diskDriver->write(pb->addr, BUFFER_SIZE, pb->blkno * BUFFER_SIZE);
    pb->flags &= ~(Buf::B_DELWRI | Buf::B_DONE);
    pb->blkno = blkno;
    m_blknoBufMap[blkno] = pb;
    return pb;
}

//释放缓存控制块buf
void BufferManager::Brelse(Buf *pb) {
    if (pb->back != NULL)
        return;
    pb->forw = m_bufList->forw;
    pb->back = m_bufList;
    m_bufList->forw->back = pb;
    m_bufList->forw = pb;
    pthread_mutex_unlock(&pb->buf_lock); // V操作, 解锁
}

//读一个磁盘块，blkno为目标磁盘块逻辑块号
Buf *BufferManager::Bread(int blkno) {
    Buf *pb = GetBlk(blkno);
    if (pb->flags & (Buf::B_DONE | Buf::B_DELWRI))
        return pb;
    m_diskDriver->read(pb->addr, BUFFER_SIZE, pb->blkno * BUFFER_SIZE);
    pb->flags |= Buf::B_DONE;
    return pb;
}

//写一个磁盘块
void BufferManager::Bwrite(Buf *pb) {
    pb->flags &= ~(Buf::B_DELWRI);
    m_diskDriver->write(pb->addr, BUFFER_SIZE, pb->blkno * BUFFER_SIZE);
    pb->flags |= (Buf::B_DONE);
    this->Brelse(pb);
}

//延迟写磁盘块
void BufferManager::Bdwrite(Buf *bp) {
    bp->flags |= (Buf::B_DELWRI | Buf::B_DONE);
    this->Brelse(bp);
    return;
}

//清空缓冲区内容
void BufferManager::Bclear(Buf *bp) {
    memset(bp->addr, 0, BufferManager::BUFFER_SIZE);
    return;
}

//将队列中延迟写的缓存全部输出到磁盘
void BufferManager::Bflush() {
    Buf *pb = NULL;
    for (int i = 0; i < NBUF; ++i) {
        pb = m_Buf + i;
        if ((pb->flags & Buf::B_DELWRI)) {
            pb->flags &= ~(Buf::B_DELWRI);
            m_diskDriver->write(pb->addr, BUFFER_SIZE, pb->blkno * BUFFER_SIZE);
            pb->flags |= (Buf::B_DONE);
        }
    }
}

#include "BufferManager.h"
#include "Common.h"

extern DiskDriver myDiskDriver;

//CacheBlock只用到了两个标志，B_DONE和B_DELWRI，分别表示已经完成IO和延迟写的标志。
//空闲Buffer无任何标志
BufferManager::BufferManager()
{
	bufferList = new Buf;
	InitList();
	diskDriver = &myDiskDriver;
}

BufferManager::~BufferManager()
{
	Bflush();
	delete bufferList;
}

void BufferManager::FormatBuffer()
{
	for (int i = 0; i < NBUF; ++i)
		nBuffer[i].Reset();
	InitList();
}

void BufferManager::InitList()
{
	for (int i = 0; i < NBUF; ++i) {
		if (i) 
			nBuffer[i].forw = nBuffer + i - 1;
		else {
			nBuffer[i].forw = bufferList;
			bufferList->back = nBuffer + i;
		}

		if (i + 1 < NBUF)
			nBuffer[i].back = nBuffer + i + 1;
		else {
			nBuffer[i].back = bufferList;
			bufferList->forw = nBuffer + i;
		}
		nBuffer[i].addr = buffer[i];
		nBuffer[i].no = i;
	}
}

//采用LRU Cache 算法，每次从头部取出，使用后放到尾部
void BufferManager::DetachNode(Buf* pb)
{
	if (pb->back == NULL)
		return;
	pb->forw->back = pb->back;
	pb->back->forw = pb->forw;
	pb->back = NULL;
	pb->forw = NULL;
}

void BufferManager::InsertTail(Buf* pb)
{
	if (pb->back != NULL)
		return;
	pb->forw = bufferList->forw;
	pb->back = bufferList;
	bufferList->forw->back = pb;
	bufferList->forw = pb;
}

//申请一块缓存，从缓存队列中取出，用于读写设备上的块blkno
Buf* BufferManager::GetBlk(int blkno)
{
	Buf* pb;
	if (map.find(blkno) != map.end()) {
		pb = map[blkno];
		DetachNode(pb);
		return pb;
	}
	pb = bufferList->back;
	if (pb == bufferList) {
		cout << "无缓存块可供使用" << endl;
		return NULL;
	}
	DetachNode(pb);
	map.erase(pb->blkno);
	if (pb->flags & Buf::CB_DELWRI)
		diskDriver->write(pb->addr, BUFFER_SIZE, pb->blkno * BUFFER_SIZE);
	pb->flags &= ~(Buf::CB_DELWRI | Buf::CB_DONE);
	pb->blkno = blkno;
	map[blkno] = pb;
	return pb;
}

//释放缓存控制块buf
void BufferManager::Brelse(Buf* pb)
{
	InsertTail(pb);
}

//读一个磁盘块，blkno为目标磁盘块逻辑块号
Buf* BufferManager::Bread(int blkno)
{
	Buf* pb = GetBlk(blkno);
	if (pb->flags & (Buf::CB_DONE | Buf::CB_DELWRI))
		return pb;
	diskDriver->read(pb->addr, BUFFER_SIZE, pb->blkno * BUFFER_SIZE);
	pb->flags |= Buf::CB_DONE;
	return pb;
}

//写一个磁盘块
void BufferManager::Bwrite(Buf* pb)
{
	pb->flags &= ~(Buf::CB_DELWRI);
	diskDriver->write(pb->addr, BUFFER_SIZE, pb->blkno * BUFFER_SIZE);
	pb->flags |= (Buf::CB_DONE);
	this->Brelse(pb);
}

//延迟写磁盘块
void BufferManager::Bdwrite(Buf* bp)
{
	bp->flags |= (Buf::CB_DELWRI | Buf::CB_DONE);
	this->Brelse(bp);
	return;
}

//清空缓冲区内容
void BufferManager::Bclear(Buf* bp)
{
	memset(bp->addr, 0, BufferManager::BUFFER_SIZE);
	return;
}

//将队列中延迟写的缓存全部输出到磁盘
void BufferManager::Bflush()
{
	Buf* pb = NULL;
	for (int i = 0; i < NBUF; ++i) {
		pb = nBuffer + i;
		if ((pb->flags & Buf::CB_DELWRI)) {
			pb->flags &= ~(Buf::CB_DELWRI);
			diskDriver->write(pb->addr, BUFFER_SIZE, pb->blkno * BUFFER_SIZE);
			pb->flags |= (Buf::CB_DONE);
		}
	}
}

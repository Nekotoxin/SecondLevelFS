#include "BufferManager.h"
#include "Common.h"

extern DiskDriver myDiskDriver;

//CacheBlockֻ�õ���������־��B_DONE��B_DELWRI���ֱ��ʾ�Ѿ����IO���ӳ�д�ı�־��
//����Buffer���κα�־
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

//����LRU Cache �㷨��ÿ�δ�ͷ��ȡ����ʹ�ú�ŵ�β��
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

//����һ�黺�棬�ӻ��������ȡ�������ڶ�д�豸�ϵĿ�blkno
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
		cout << "�޻����ɹ�ʹ��" << endl;
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

//�ͷŻ�����ƿ�buf
void BufferManager::Brelse(Buf* pb)
{
	InsertTail(pb);
}

//��һ�����̿飬blknoΪĿ����̿��߼����
Buf* BufferManager::Bread(int blkno)
{
	Buf* pb = GetBlk(blkno);
	if (pb->flags & (Buf::CB_DONE | Buf::CB_DELWRI))
		return pb;
	diskDriver->read(pb->addr, BUFFER_SIZE, pb->blkno * BUFFER_SIZE);
	pb->flags |= Buf::CB_DONE;
	return pb;
}

//дһ�����̿�
void BufferManager::Bwrite(Buf* pb)
{
	pb->flags &= ~(Buf::CB_DELWRI);
	diskDriver->write(pb->addr, BUFFER_SIZE, pb->blkno * BUFFER_SIZE);
	pb->flags |= (Buf::CB_DONE);
	this->Brelse(pb);
}

//�ӳ�д���̿�
void BufferManager::Bdwrite(Buf* bp)
{
	bp->flags |= (Buf::CB_DELWRI | Buf::CB_DONE);
	this->Brelse(bp);
	return;
}

//��ջ���������
void BufferManager::Bclear(Buf* bp)
{
	memset(bp->addr, 0, BufferManager::BUFFER_SIZE);
	return;
}

//���������ӳ�д�Ļ���ȫ�����������
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

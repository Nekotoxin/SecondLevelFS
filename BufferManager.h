#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "DiskDriver.h"
#include "Buf.h"
#include <unordered_map>
using namespace std;

class BufferManager
{
public:
	static const int NBUF = 100;            //������ƿ顢������������
	static const int BUFFER_SIZE = 512;     //��������С�� ���ֽ�Ϊ��λ

private:
	Buf* bufferList;                 //���ɻ�����п��ƿ�
	Buf nBuffer[NBUF];               //������ƿ�����
	unsigned char buffer[NBUF][BUFFER_SIZE];//����������
	unordered_map<int, Buf*> map;
	DiskDriver* diskDriver;

public:
	BufferManager();
	~BufferManager();
	Buf* GetBlk(int blkno);         //����һ�黺�棬���ڶ�д�豸�ϵĿ�blkno
	void Brelse(Buf* bp);           //�ͷŻ�����ƿ�buf
	Buf* Bread(int blkno);          //��һ�����̿飬blknoΪĿ����̿��߼����
	void Bwrite(Buf* bp);           //дһ�����̿�
	void Bdwrite(Buf* bp);          //�ӳ�д���̿�
	void Bclear(Buf* bp);           //��ջ���������
	void Bflush();                         //���������ӳ�д�Ļ���ȫ�����������
	void FormatBuffer();                   //��ʽ������Buffer

private:
	void InitList();                 //������ƿ���еĳ�ʼ��
	void DetachNode(Buf* pb);
	void InsertTail(Buf* pb);
};
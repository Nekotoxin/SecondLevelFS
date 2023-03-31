#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <iostream>
#include "Utility.h"

using namespace std;
const char diskFileName[] = "myDisk.img";  //磁盘镜像文件名

class DiskDriver {
public:
	FILE* diskp;                     //磁盘文件指针

public:
	DiskDriver();
	~DiskDriver();
	void Reset();
	//写磁盘函数
	void write(const uint8* in_buffer, uint32 in_size, int offset = -1, uint32 origin = SEEK_SET);
	//读磁盘函数
	void read(uint8* out_buffer, uint32 out_size, int offset = -1, uint32 origin = SEEK_SET);

	//检查镜像文件是否存在
	bool Exists();
	//打开镜像文件
	void Construct();
};
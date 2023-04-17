#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <iostream>
#include "Utility.h"

using namespace std;
const char diskFileName[] = "myDisk.img";  //磁盘镜像文件名

#define NORMAL_DISK_MODE 0
#define MMAP_DISK_MODE 1

#define DISK_SIZE (8*1024*1024)

class DiskDriver {
public:
    int diskMode;                    // mmap方式或普通方式
    FILE *diskp;                     //磁盘文件指针
    uint8 *diskmapp;                 //磁盘映射指针

public:
    // 构造函数
    // 如果文件不存在，不创建新的文件
    DiskDriver();

    ~DiskDriver();

    // 写磁盘函数
    // in_buffer: 写入的数据
    // in_size: 写入的数据大小
    // offset: 写入的偏移量
    // origin: 写入的起始位置
    void write(const uint8 *in_buffer, uint32 in_size, int offset = -1, uint32 origin = SEEK_SET);

    // 读磁盘函数
    // out_buffer: 读出的数据
    // out_size: 读出的数据大小
    // offset: 读出的偏移量
    // origin: 读出的起始位置
    void read(uint8 *out_buffer, uint32 out_size, int offset = -1, uint32 origin = SEEK_SET);

    // 判断磁盘文件是否存在
    // 返回值：存在返回true，否则返回false
    bool Exists();

    // 打开镜像文件
    // 如果文件不存在，则创建一个新的镜像文件
    void OpenIMGFile();

    // 使用mmap方式打开磁盘文件
    // 内存文件系统，提高读写效率
    void UseMMAP();

};
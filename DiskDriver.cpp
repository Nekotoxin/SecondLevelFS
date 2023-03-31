#include "DiskDriver.h"

DiskDriver::DiskDriver()
{
    diskp = fopen(diskFileName, "rb+");
}
DiskDriver::~DiskDriver()
{
    if (diskp != NULL) {
        fclose(diskp);
    }
}
void DiskDriver::Reset()
{
    if (diskp != NULL)
        fclose(diskp);
    diskp = fopen(diskFileName, "rb+");
}
//写磁盘函数
void DiskDriver::write(const uint8* in_buffer, uint32 in_size, int offset, uint32 origin) {
    if (offset >= 0)
        fseek(diskp, offset, origin);
    fwrite(in_buffer, in_size, 1, diskp);
    return;
}
//读磁盘函数
void DiskDriver::read(uint8* out_buffer, uint32 out_size, int offset, uint32 origin) {
    if (offset >= 0)
        fseek(diskp, offset, origin);
    fread(out_buffer, out_size, 1, diskp);
    return;
}

//检查镜像文件是否存在
bool DiskDriver::Exists()
{
    return diskp != NULL;
}
//打开镜像文件
void DiskDriver::Construct()
{
    diskp = fopen(diskFileName, "wb+");
    if (diskp == NULL)
        printf("Disk Construct Error!\n");
}
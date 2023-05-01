#include "DiskDriver.h"
#include<sys/mman.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include <errno.h>
#include <sys/stat.h>

DiskDriver::DiskDriver() {
    diskp = fopen(diskFileName, "rb+");
    diskmapp = NULL;
    diskMode = NORMAL_DISK_MODE;
}

DiskDriver::~DiskDriver() {
    if (diskp != NULL) {
        if (diskMode == MMAP_DISK_MODE) {
            munmap(diskmapp, DISK_SIZE);
        }
        fclose(diskp);
    }
}

void DiskDriver::write(const uint8 *in_buffer, uint32 in_size, int offset, uint32 origin) {
    if (diskMode == MMAP_DISK_MODE) {
        uint8 *p = diskmapp;
        if (offset >= 0)
            p += offset;
        memcpy(p, in_buffer, in_size);
        return;
    } else {
        if (offset >= 0)
            fseek(diskp, offset, origin);
        fwrite(in_buffer, in_size, 1, diskp);
    }
}

void DiskDriver::read(uint8 *out_buffer, uint32 out_size, int offset, uint32 origin) {
    if (diskMode == MMAP_DISK_MODE) {
        uint8 *p = diskmapp;
        if (offset >= 0)
            p += offset;
        memcpy(out_buffer, p, out_size);
        return;
    } else {
        if (offset >= 0)
            fseek(diskp, offset, origin);
        fread(out_buffer, out_size, 1, diskp);
    }
}

bool DiskDriver::Exists() {
    return diskp != NULL;
}

void DiskDriver::OpenIMGFile() {
    diskp = fopen(diskFileName, "wb+"); /*  没有就创建 */
    if (diskp == NULL) {
        printf("Disk OpenIMGFile Error!\n");
        exit(1);
    }
}

void DiskDriver::UseMMAP() {
    int fd = fileno(diskp);
    ftruncate(fd, DISK_SIZE); /*  8MB */
    struct stat sb;
    fstat(fd, &sb);
    diskmapp = (uint8 *) mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (diskmapp == MAP_FAILED) {
        printf("mmap error: %s\n", strerror(errno));
        exit(1);
    }
    diskMode = MMAP_DISK_MODE;
    diskp = NULL;
}

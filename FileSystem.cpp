#include "Utility.h"
#include "FileSystem.h"
#include "OpenFileManager.h"
#include "BufferManager.h"
#include "Kernel.h"
#include <ctime>

extern INodeTable g_INodeTable;
SuperBlock g_SuperBlock;


void FileSystem::Initialize() {
    m_diskDriver = &Kernel::Instance().GetDiskDriver();
    m_superBlock = &g_SuperBlock;
    m_bufferManager = &Kernel::Instance().GetBufferManager();

    if (!m_diskDriver->Exists())
        Format();
    else
        SpbLoad();
    /*  定义是否使用mmap */
    this->m_diskDriver->UseMMAP();
}


FileSystem::~FileSystem() {
    SpbFlush();
    m_diskDriver = NULL;
    m_superBlock = NULL;
}


/* 格式化整个文件系统 */
void FileSystem::Format() {
    m_diskDriver->OpenIMGFile(); /*  打开磁盘文件 */
    /*  初始化SuperBlock */
    m_superBlock->s_isize = FileSystem::INODE_ZONE_SIZE;
    m_superBlock->s_fsize = FileSystem::DATA_END_SECTOR + 1;
    m_superBlock->s_nfree = 0;
    m_superBlock->s_free[0] = -1; /*  -1表示空闲盘块索引表已经用完 */
    m_superBlock->s_ninode = 0;
    m_superBlock->s_fmod = 0;
    time((time_t *) &m_superBlock->s_time); /*  获取当前时间 */
    m_diskDriver->write((uint8 *) (m_superBlock), sizeof(SuperBlock), 0); /*  将SuperBlock写入磁盘 */

    /*  初始化INode区 */
    DiskINode rootInode;
    rootInode.d_mode |= INode::IALLOC | INode::IFDIR; /*  设置根目录的属性 */
    m_diskDriver->write((uint8 *) &rootInode, sizeof(rootInode)); /*  将根目录的INode写入磁盘 */
    DiskINode ZeroInode;
    for (int i = 1; i < FileSystem::INODE_ZONE_SIZE * FileSystem::INODE_NUMBER_PER_SECTOR; ++i) {
        if (m_superBlock->s_ninode < 100) {
            m_superBlock->s_inode[m_superBlock->s_ninode++] = i; /*  将空的INode编号写入SuperBlock的索引表 */
        }
        m_diskDriver->write((uint8 *) &ZeroInode, sizeof(ZeroInode));
    }
    /*  初始化数据区 */
    char dataBlock[BLOCK_SIZE];
    for (int i = 0; i < FileSystem::DATA_SECTOR_NUMBER; ++i) {
        if (m_superBlock->s_nfree >= 100) {
            /*  这是队长块，记录了下一组空闲盘块的数量和编号 */
            ((int *) dataBlock)[0] = m_superBlock->s_nfree; /*  记录下一组空闲盘块的数量 */
            memcpy(dataBlock + sizeof(int), &m_superBlock->s_free, sizeof(m_superBlock->s_free));
            m_diskDriver->write((uint8 *) &dataBlock, BLOCK_SIZE); /*  将队长块写入磁盘 */
            m_superBlock->s_nfree = 0; /*  开始下一组的记数 */
        } else {
            /*  这是普通块，全部初始化为0 */
            memset(dataBlock, 0, BLOCK_SIZE);
            m_diskDriver->write((uint8 *) &dataBlock, BLOCK_SIZE);
        }
        m_superBlock->s_free[m_superBlock->s_nfree++] =
                i + FileSystem::DATA_START_SECTOR; /*  将空闲盘块的编号写入SuperBlock的索引表 */
    }

    time((time_t *) &m_superBlock->s_time);
    m_diskDriver->write((uint8 *) (m_superBlock), sizeof(SuperBlock), 0); /*  更新SuperBlock */
}

/* 系统初始化时读入SuperBlock */
void FileSystem::SpbLoad() {
    fseek(m_diskDriver->diskp, 0, 0);
    m_diskDriver->read((uint8 *) (m_superBlock), sizeof(SuperBlock), 0);
    pthread_mutex_init(&m_superBlock->s_ilock, NULL);
    pthread_mutex_init(&m_superBlock->s_flock, NULL);

}

/* 将SuperBlock对象的内存副本更新到存储设备的SuperBlock中去 */
void FileSystem::SpbFlush() {
    Buf *pCache;
    pthread_mutex_lock(&m_superBlock->s_ilock);
    pthread_mutex_lock(&m_superBlock->s_flock);
    m_superBlock->s_fmod = 0;
    m_superBlock->s_time = (int) time(NULL);
    for (int j = 0; j < 2; j++) {
        int *p = (int *) m_superBlock + j * 128;
        pCache = this->m_bufferManager->GetBlk(FileSystem::SUPER_BLOCK_SECTOR_NUMBER + j);
        memcpy(pCache->addr, p, BLOCK_SIZE);
        this->m_bufferManager->Bwrite(pCache);
    }
    pthread_mutex_unlock(&m_superBlock->s_ilock);
    pthread_mutex_unlock(&m_superBlock->s_flock);
    g_INodeTable.UpdateINodeTable();
    this->m_bufferManager->Bflush();
}

/* 在存储设备上分配空闲磁盘块 */
Buf *FileSystem::Alloc() {
    User *u = Kernel::Instance().GetUserManager().GetUser();
    int blkno;
    Buf *pCache;
    pthread_mutex_lock(&m_superBlock->s_flock);
    /* 从索引表“栈顶”获取空闲磁盘块编号 */
    blkno = m_superBlock->s_free[--m_superBlock->s_nfree];

    /* 若获取磁盘块编号为零，则表示已分配尽所有的空闲磁盘块 */
    if (blkno <= 0) {
        m_superBlock->s_nfree = 0;
        u->u_error = ENOSPC;
        return NULL;
    }

/* 栈已空，新分配到空闲磁盘块中记录了下一组空闲磁盘块的编号
 * 将下一组空闲磁盘块的编号读入SuperBlock的空闲磁盘块索引表s_free[100]中 */
    if (m_superBlock->s_nfree <= 0) {
        pCache = this->m_bufferManager->Bread(blkno);
        int *p = (int *) pCache->addr;
        m_superBlock->s_nfree = *p++;
        memcpy(m_superBlock->s_free, p, sizeof(m_superBlock->s_free));
        this->m_bufferManager->Brelse(pCache);
    }
    pthread_mutex_unlock(&m_superBlock->s_flock);
    pCache = this->m_bufferManager->GetBlk(blkno);
    if (pCache)
        this->m_bufferManager->Bclear(pCache);
    m_superBlock->s_fmod = 1;
    return pCache;
}

/* 在存储设备dev上分配一个空闲外存INode，一般用于创建新的文件 */
INode *FileSystem::IAlloc() {
    User *u = Kernel::Instance().GetUserManager().GetUser();
    Buf *pCache;
    INode *pINode;
    int ino;
    /* SuperBlock直接管理的空闲Inode索引表已空，必须到磁盘上搜索空闲Inode */
    if (m_superBlock->s_ninode <= 0) {
        ino = -1;
        pthread_mutex_lock(&m_superBlock->s_ilock);
        for (int i = 0; i < m_superBlock->s_isize; ++i) {
            pCache = this->m_bufferManager->Bread(FileSystem::INODE_ZONE_START_SECTOR + i);
            int *p = (int *) pCache->addr;
            for (int j = 0; j < FileSystem::INODE_NUMBER_PER_SECTOR; ++j) {
                ++ino;
                int mode = *(p + j * 64 / sizeof(int)); /*  64: sizeof(Inode) */
                if (mode)
                    continue;
/* 如果外存inode的i_mode == 0，此时并不能确定该inode是空闲的，
 * 因为有可能是内存inode没有写到磁盘上, 所以要继续搜索内存inode中是否有相应的项 */
                if (g_INodeTable.IsLoaded(ino) == -1) {
                    m_superBlock->s_inode[m_superBlock->s_ninode++] = ino;
                    if (m_superBlock->s_ninode >= 100)
                        break;
                }
            }

            this->m_bufferManager->Brelse(pCache);
            if (m_superBlock->s_ninode >= 100)
                break;
        }
        pthread_mutex_unlock(&m_superBlock->s_ilock);
        if (m_superBlock->s_ninode <= 0) {
            u->u_error = ENOSPC;
            return NULL;
        }
    }
    ino = m_superBlock->s_inode[--m_superBlock->s_ninode];
    pINode = g_INodeTable.IGet(ino);
    if (NULL == pINode) {
        cout << "无空闲内存存储INode" << endl;
        return NULL;
    }

    pINode->Clean();
    m_superBlock->s_fmod = 1;
    return pINode;
}

/* 释放编号为number的外存INode，一般用于删除文件 */
void FileSystem::IFree(int number) {
    if (m_superBlock->s_ninode >= 100)
        return;
    m_superBlock->s_inode[m_superBlock->s_ninode++] = number;
    m_superBlock->s_fmod = 1;
}

/* 释放存储设备dev上编号为blkno的磁盘块 */
void FileSystem::Free(int blkno) {
    Buf *pCache;
    pthread_mutex_lock(&m_superBlock->s_flock);
    if (m_superBlock->s_nfree >= 100) {
        pCache = this->m_bufferManager->GetBlk(blkno);
        int *p = (int *) pCache->addr;
        *p++ = m_superBlock->s_nfree;
        memcpy(p, m_superBlock->s_free, sizeof(int) * 100);
        m_superBlock->s_nfree = 0;
        this->m_bufferManager->Bwrite(pCache);
    }

    m_superBlock->s_free[m_superBlock->s_nfree++] = blkno;
    pthread_mutex_unlock(&m_superBlock->s_flock);
    m_superBlock->s_fmod = 1;
}

SuperBlock *FileSystem::GetFS() {
    return &g_SuperBlock;
}

SuperBlock::SuperBlock() {
    pthread_mutex_init(&s_ilock, NULL);
    pthread_mutex_init(&s_flock, NULL);
}

SuperBlock::~SuperBlock() {
    pthread_mutex_destroy(&s_ilock);
    pthread_mutex_destroy(&s_flock);
}

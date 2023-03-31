#include "Utility.h"
#include "FileSystem.h"
#include "OpenFileManager.h"
#include "BufferManager.h"
#include "Kernel.h"
#include <ctime>
#include "SysCall.h"

extern INodeTable g_INodeTable;
extern SysCall g_UserCall;
SuperBlock g_SuperBlock;


FileSystem::FileSystem() {

}

void FileSystem::Initialize() {
    m_diskDriver = &Kernel::Instance().GetDiskDriver();
    m_superBlock = &g_SuperBlock;
    m_bufferManager = &Kernel::Instance().GetBufferManager();

    if (!m_diskDriver->Exists())
        Format();
    else
        LoadSuperBlock();
}


FileSystem::~FileSystem() {
    Update();
    m_diskDriver = NULL;
    m_superBlock = NULL;
}


//格式化整个文件系统
void FileSystem::Format() {
    // initialize superblock
    m_superBlock->s_isize = FileSystem::INODE_SECTOR_NUMBER;
    m_superBlock->s_fsize = FileSystem::DISK_SECTOR_NUMBER;
    m_superBlock->s_nfree = 0;
    m_superBlock->s_free[0] = -1;
    m_superBlock->s_ninode = 0;
    m_superBlock->s_fmod = 0;
    time((time_t *) &m_superBlock->s_time);

    m_diskDriver->Construct();
    m_diskDriver->write((uint8 *) (m_superBlock), sizeof(SuperBlock), 0);

    DiskINode emptyDINode, rootDINode;
    rootDINode.d_mode |= INode::IALLOC | INode::IFDIR;
    m_diskDriver->write((uint8 *) &rootDINode, sizeof(rootDINode));
    for (int i = 1; i < FileSystem::INODE_NUMBER_ALL; ++i) {
        if (m_superBlock->s_ninode < SuperBlock::MAX_NUMBER_INODE)
            m_superBlock->s_inode[m_superBlock->s_ninode++] = i;
        m_diskDriver->write((uint8 *) &emptyDINode, sizeof(emptyDINode));
    }
    char freeBlock[BLOCK_SIZE], freeBlock1[BLOCK_SIZE];
    memset(freeBlock, 0, BLOCK_SIZE);
    memset(freeBlock1, 0, BLOCK_SIZE);

    for (int i = 0; i < FileSystem::DATA_SECTOR_NUMBER; ++i) {
        if (m_superBlock->s_nfree >= SuperBlock::MAX_NUMBER_FREE) {
            memcpy(freeBlock1, &m_superBlock->s_nfree, sizeof(int) + sizeof(m_superBlock->s_free));
            m_diskDriver->write((uint8 *) &freeBlock1, BLOCK_SIZE);
            m_superBlock->s_nfree = 0;
        } else
            m_diskDriver->write((uint8 *) freeBlock, BLOCK_SIZE);
        m_superBlock->s_free[m_superBlock->s_nfree++] = i + FileSystem::DATA_START_SECTOR;
    }

    time((time_t *) &m_superBlock->s_time);
    m_diskDriver->write((uint8 *) (m_superBlock), sizeof(SuperBlock), 0);
}

//系统初始化时读入SuperBlock
void FileSystem::LoadSuperBlock() {
    fseek(m_diskDriver->diskp, 0, 0);
    m_diskDriver->read((uint8 *) (m_superBlock), sizeof(SuperBlock), 0);
}

//将SuperBlock对象的内存副本更新到存储设备的SuperBlock中去
void FileSystem::Update() {
    Buf *pCache;
    m_superBlock->s_fmod = 0;
    m_superBlock->s_time = (int) time(NULL);
    for (int j = 0; j < 2; j++) {
        int *p = (int *) m_superBlock + j * 128;
        pCache = this->m_bufferManager->GetBlk(FileSystem::SUPERBLOCK_START_SECTOR + j);
        memcpy(pCache->addr, p, BLOCK_SIZE);
        this->m_bufferManager->Bwrite(pCache);
    }
    g_INodeTable.UpdateINodeTable();
    this->m_bufferManager->Bflush();
}

//在存储设备上分配空闲磁盘块
Buf *FileSystem::Alloc() {
    User* u=Kernel::Instance().GetUserManager().GetUser();
    int blkno;
    Buf *pCache;
    //从索引表“栈顶”获取空闲磁盘块编号
    blkno = m_superBlock->s_free[--m_superBlock->s_nfree];

    //若获取磁盘块编号为零，则表示已分配尽所有的空闲磁盘块
    if (blkno <= 0) {
        m_superBlock->s_nfree = 0;
        u->u_error = ENOSPC;
        return NULL;
    }

    //栈已空，新分配到空闲磁盘块中记录了下一组空闲磁盘块的编号
    //将下一组空闲磁盘块的编号读入SuperBlock的空闲磁盘块索引表s_free[100]中
    if (m_superBlock->s_nfree <= 0) {
        pCache = this->m_bufferManager->Bread(blkno);
        int *p = (int *) pCache->addr;
        m_superBlock->s_nfree = *p++;
        memcpy(m_superBlock->s_free, p, sizeof(m_superBlock->s_free));
        this->m_bufferManager->Brelse(pCache);
    }
    pCache = this->m_bufferManager->GetBlk(blkno);
    if (pCache)
        this->m_bufferManager->Bclear(pCache);
    m_superBlock->s_fmod = 1;
    return pCache;
}

//在存储设备dev上分配一个空闲外存INode，一般用于创建新的文件
INode *FileSystem::IAlloc() {
    User* u=Kernel::Instance().GetUserManager().GetUser();
    Buf *pCache;
    INode *pINode;
    int ino;
    //SuperBlock直接管理的空闲Inode索引表已空，必须到磁盘上搜索空闲Inode
    if (m_superBlock->s_ninode <= 0) {
        ino = -1;
        for (int i = 0; i < m_superBlock->s_isize; ++i) {
            pCache = this->m_bufferManager->Bread(FileSystem::INODE_START_SECTOR + i);
            int *p = (int *) pCache->addr;
            for (int j = 0; j < FileSystem::INODE_NUMBER_PER_SECTOR; ++j) {
                ++ino;
                int mode = *(p + j * FileSystem::INODE_SIZE / sizeof(int));
                if (mode)
                    continue;
                //如果外存inode的i_mode == 0，此时并不能确定该inode是空闲的，
                //因为有可能是内存inode没有写到磁盘上, 所以要继续搜索内存inode中是否有相应的项
                if (g_INodeTable.IsLoaded(ino) == -1) {
                    m_superBlock->s_inode[m_superBlock->s_ninode++] = ino;
                    if (m_superBlock->s_ninode >= SuperBlock::MAX_NUMBER_INODE)
                        break;
                }
            }

            this->m_bufferManager->Brelse(pCache);
            if (m_superBlock->s_ninode >= SuperBlock::MAX_NUMBER_INODE)
                break;
        }
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

//释放编号为number的外存INode，一般用于删除文件
void FileSystem::IFree(int number) {
    if (m_superBlock->s_ninode >= SuperBlock::MAX_NUMBER_INODE)
        return;
    m_superBlock->s_inode[m_superBlock->s_ninode++] = number;
    m_superBlock->s_fmod = 1;
}

//释放存储设备dev上编号为blkno的磁盘块
void FileSystem::Free(int blkno) {
    Buf *pCache;
    if (m_superBlock->s_nfree >= SuperBlock::MAX_NUMBER_FREE) {
        pCache = this->m_bufferManager->GetBlk(blkno);
        int *p = (int *) pCache->addr;
        *p++ = m_superBlock->s_nfree;
        memcpy(p, m_superBlock->s_free, sizeof(int) * SuperBlock::MAX_NUMBER_FREE);
        m_superBlock->s_nfree = 0;
        this->m_bufferManager->Bwrite(pCache);
    }

    m_superBlock->s_free[m_superBlock->s_nfree++] = blkno;
    m_superBlock->s_fmod = 1;
}

SuperBlock *FileSystem::GetFS() {
    return &g_SuperBlock;
}
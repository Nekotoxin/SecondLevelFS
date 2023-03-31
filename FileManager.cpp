#include "Utility.h"
#include "FileManager.h"
#include "BufferManager.h"
#include "UserCall.h"

extern BufferManager g_BufferManager;
extern OpenFileTable g_OpenFileTable;
extern FileSystem g_FileSystem;
extern INodeTable g_INodeTable;
extern UserCall g_UserCall;

FileManager::FileManager()
{
    m_fileSystem = &g_FileSystem;
    m_openFileTable = &g_OpenFileTable;
    m_inodeTable = &g_INodeTable;
	rootDirINode = m_inodeTable->IGet(FileSystem::ROOT_INODE_NO);//根据外存INode编号获取对应INode。如果该INode已经在内存中，返回该内存INode；
		                                                         //如果不在内存中，则将其读入内存后上锁并返回该内存INode
	                                                             //文件系统根目录外存INode编号
	rootDirINode->i_count += 0xff;//引用计数                            
}

FileManager::~FileManager() { }

//功能：打开文件
//效果：建立打开文件结构，内存i节点开锁 、i_count 为正数（i_count ++）
void FileManager::Open()
{
	INode* pINode = this->NameI(FileManager::OPEN);
	if (pINode == NULL)
		return;
	this->Open1(pINode, 0);
}

//Creat()系统调用处理过程
void FileManager::Creat()
{
	INode* pINode;

	int newACCMode = g_UserCall.arg[1];//存放当前系统调用参数 文件类型：目录文件
	//搜索目录的模式为1，表示创建；若父目录不可写，出错返回
	pINode = this->NameI(FileManager::CREATE);
	//没有找到相应的INode，或NameI出错
	if (NULL == pINode) {
		if (g_UserCall.userErrorCode)
			return;

		pINode = this->MakNode(newACCMode);
		if (NULL == pINode)
			return;
		//如果创建的名字不存在，使用参数trf = 2来调用open1()
		this->Open1(pINode, 2);
		return;
	}
	//如果NameI()搜索到已经存在要创建的文件，则清空该文件（用算法ITrunc()）
	this->Open1(pINode, 1);
	pINode->i_mode |= newACCMode;
}

//目录搜索，将路径转化为相应的INode返回上锁后的INode
//返回NULL表示目录搜索失败，否则是根指针，指向文件的内存打开i节点 ，上锁的内存i节点
INode* FileManager::NameI(enum DirectorySearchMode mode)
{
	INode* pINode = g_UserCall.nowDirINodePointer;
	Buf* pCache;
	int freeEntryOffset; //以创建文件模式搜索目录时，记录空闲目录项的偏移量
	unsigned int index = 0, nindex = 0;

	//如果该路径是'/'开头的，从根目录开始搜索，否则从进程当前工作目录开始搜索
	if ('/' == g_UserCall.dirp[0]) {
		nindex = ++index + 1;
		pINode = this->rootDirINode;
	}
	//外层循环每次处理pathname中一段路径分量
	while (1) {
		//如果出错则释放当前搜索到的目录文件Inode，并退出
		if (g_UserCall.userErrorCode != UserCall::U_NOERROR)
			break;
		//整个路径搜索完毕，返回相应Inode指针。目录搜索成功返回
		if (nindex >= g_UserCall.dirp.length())
			return pINode;
		//如果要进行搜索的不是目录文件，释放相关Inode资源则退出
		if ((pINode->i_mode & INode::IFMT) != INode::IFDIR) {//文件类型：目录文件
			g_UserCall.userErrorCode = UserCall::U_ENOTDIR;//文件夹不存在
			break;
		}

		//将Pathname中当前准备进行匹配的路径分量拷贝到u.u_dbuf[]中，便于和目录项进行比较。
		nindex = g_UserCall.dirp.find_first_of('/', index);
		memset(g_UserCall.dbuf, 0, sizeof(g_UserCall.dbuf));
		memcpy(g_UserCall.dbuf, g_UserCall.dirp.data() + index, (nindex == (unsigned int)string::npos ? g_UserCall.dirp.length() : nindex) - index);
		index = nindex + 1;
		//内层循环部分对于u.u_dbuf[]中的路径名分量，逐个搜寻匹配的目录项
		g_UserCall.IOParam.offset = 0;
		//设置为目录项个数 ，含空白的目录项
		g_UserCall.IOParam.count = pINode->i_size / sizeof(DirectoryEntry);
		freeEntryOffset = 0;
		pCache = NULL;
		while (1) {
			/* 对目录项已经搜索完毕 */
			if (0 == g_UserCall.IOParam.count) {
				if (NULL != pCache)
					g_BufferManager.Brelse(pCache);
				//如果是创建新文件
				if (FileManager::CREATE == mode && nindex >= g_UserCall.dirp.length()) {
					//将父目录Inode指针保存起来，以后写目录项WriteDir()函数会用到
					g_UserCall.paDirINodePointer = pINode;
					if (freeEntryOffset) //此变量存放了空闲目录项位于目录文件中的偏移量
						g_UserCall.IOParam.offset = freeEntryOffset - sizeof(DirectoryEntry); //将空闲目录项偏移量存入u区中，写目录项WriteDir()会用到
					else //问题：为何if分支没有置IUPD标志？  这是因为文件的长度没有变呀
						pINode->i_flag |= INode::IUPD;
					//找到可以写入的空闲目录项位置，NameI()函数返回
					return NULL;
				}
				//目录项搜索完毕而没有找到匹配项，释放相关Inode资源，并退出
				g_UserCall.userErrorCode = UserCall::U_ENOENT;
				goto out;
			}
			//已读完目录文件的当前盘块，需要读入下一目录项数据盘块
			if (0 == g_UserCall.IOParam.offset % INode::BLOCK_SIZE) {
				if (pCache)
					g_BufferManager.Brelse(pCache);
				//计算要读的物理盘块号
				int phyBlkno = pINode->Bmap(g_UserCall.IOParam.offset / INode::BLOCK_SIZE);
				pCache = g_BufferManager.Bread(phyBlkno);
			}
			//没有读完当前目录项盘块，则读取下一目录项至u.u_dent
			memcpy(&g_UserCall.dent, pCache->addr + (g_UserCall.IOParam.offset % INode::BLOCK_SIZE), sizeof(g_UserCall.dent));
            g_UserCall.IOParam.offset += sizeof(DirectoryEntry);
			g_UserCall.IOParam.count--;
			//如果是空闲目录项，记录该项位于目录文件中偏移量
			if (0 == g_UserCall.dent.m_ino) {
				if (0 == freeEntryOffset)
					freeEntryOffset = g_UserCall.IOParam.offset;
				//跳过空闲目录项，继续比较下一目录项
				continue;
			}

			if (!memcmp(g_UserCall.dbuf, &g_UserCall.dent.name, sizeof(DirectoryEntry) - 4))
				break;
		}

		//从内层目录项匹配循环跳至此处，说明pathname中当前路径分量匹配成功了，还需匹配pathname中下一路径分量，直至遇到'\0'结束
		if (pCache)
			g_BufferManager.Brelse(pCache);

		//如果是删除操作，则返回父目录Inode，而要删除文件的Inode号在u.u_dent.m_ino中
		if (FileManager::DELETE == mode && nindex >= g_UserCall.dirp.length())
			return pINode;

		//匹配目录项成功，则释放当前目录Inode，根据匹配成功的目录项m_ino字段获取相应下一级目录或文件的Inode
		this->m_inodeTable->IPut(pINode);
		pINode = this->m_inodeTable->IGet(g_UserCall.dent.m_ino);
		//回到外层While(true)循环，继续匹配Pathname中下一路径分量

		if (NULL == pINode) //获取失败
			return NULL;
	}

out:
	this->m_inodeTable->IPut(pINode);
	return NULL;
}

//trf == 0由open调用
//trf == 1由creat调用，creat文件的时候搜索到同文件名的文件
//trf == 2由creat调用，creat文件的时候未搜索到同文件名的文件，这是文件创建时更一般的情况
//mode参数：打开文件模式，表示文件操作是 读、写还是读写
void FileManager::Open1(INode* pINode, int trf)
{
	//在creat文件的时候搜索到同文件名的文件，释放该文件所占据的所有盘块
	if (1 == trf)
		pINode->ITrunc();//释放Inode对应文件占用的磁盘块
	//分配打开文件控制块File结构
	File* pFile = this->m_openFileTable->FAlloc();//在系统打开文件表中分配一个空闲的File结构
	if (NULL == pFile) {
		this->m_inodeTable->IPut(pINode);
		return;
	}
	pFile->inode = pINode;

	//为打开或者创建文件的各种资源都已成功分配，函数返回
	if (g_UserCall.userErrorCode == 0)
		return;
	else { //如果出错则释放资源
		//释放打开文件描述符
		int fd = g_UserCall.ar0[UserCall::EAX];
		if (fd != -1) {
			g_UserCall.ofiles.SetF(fd, NULL);
			//递减File结构和Inode的引用计数 ,File结构没有锁 f_count为0就是释放File结构了
			pFile->count--;
		}
		this->m_inodeTable->IPut(pINode);
	}
}

//被Creat()系统调用使用，用于为创建新文件分配内核资源
//为新创建的文件写新的i节点和父目录中新的目录项(相应参数在User结构中)
//返回的pINode是上了锁的内存i节点，其中的i_count是 1
INode* FileManager::MakNode(int mode)
{
	INode* pINode;
	//分配一个空闲DiskInode，里面内容已全部清空
	pINode = this->m_fileSystem->IAlloc();
	if (NULL == pINode)
		return NULL;

	pINode->i_flag = (INode::IACC | INode::IUPD);
	pINode->i_mode = mode | INode::IALLOC;
	pINode->i_nlink = 1;
	//将目录项写入u.u_u_dent，随后写入目录文件
	this->WriteDir(pINode);
	return pINode;
}

//向父目录的目录文件写入一个目录项
//把属于自己的目录项写进父目录，修改父目录文件的i节点 、将其写回磁盘。
void FileManager::WriteDir(INode* pINode)
{
	//设置目录项中INode编号部分
	g_UserCall.dent.m_ino = pINode->i_number;
	//设置目录项中pathname分量部分
	memcpy(g_UserCall.dent.name, g_UserCall.dbuf, DirectoryEntry::DIRSIZ);

    g_UserCall.IOParam.count = DirectoryEntry::DIRSIZ + 4;
    g_UserCall.IOParam.base = (unsigned char*)&g_UserCall.dent;
	//将目录项写入父目录文件
	g_UserCall.paDirINodePointer->WriteI();
	this->m_inodeTable->IPut(g_UserCall.paDirINodePointer);
}


void FileManager::Close()
{
	int fd = g_UserCall.arg[0];
	//获取打开文件控制块File结构
	File* pFile = g_UserCall.ofiles.GetF(fd);
	if (NULL == pFile)
		return;
	//释放打开文件描述符fd，递减File结构引用计数
	g_UserCall.ofiles.SetF(fd, NULL);
	this->m_openFileTable->CloseF(pFile);
}

void FileManager::UnLink()
{
	//注意删除文件夹有磁盘泄露
	INode* pINode;
	INode* pDeleteINode;
	pDeleteINode = this->NameI(FileManager::DELETE);
	if (NULL == pDeleteINode)
		return;

	pINode = this->m_inodeTable->IGet(g_UserCall.dent.m_ino);
	if (NULL == pINode)
		return;
	//写入清零后的目录项
	g_UserCall.IOParam.offset -= (DirectoryEntry::DIRSIZ + 4);
    g_UserCall.IOParam.base = (unsigned char*)&g_UserCall.dent;
    g_UserCall.IOParam.count = DirectoryEntry::DIRSIZ + 4;

    g_UserCall.dent.m_ino = 0;
	pDeleteINode->WriteI();
	//修改inode项
	pINode->i_nlink--;
	pINode->i_flag |= INode::IUPD;

	this->m_inodeTable->IPut(pDeleteINode);
	this->m_inodeTable->IPut(pINode);
}

void FileManager::Seek()
{
	File* pFile;
	int fd = g_UserCall.arg[0];

	pFile = g_UserCall.ofiles.GetF(fd);
	if (NULL == pFile)
		return; //若FILE不存在，GetF有设出错码

	int offset = g_UserCall.arg[1];

	switch (g_UserCall.arg[2]) {
	case 0:
		//读写位置设置为offset
		pFile->offset = offset;
		break;
	case 1:
		//读写位置加offset(可正可负)
		pFile->offset += offset;
		break;
	case 2:
		//读写位置调整为文件长度加offset
		pFile->offset = pFile->inode->i_size + offset;
		break;
	default:
		break;
	}
	cout << "文件指针成功移动到 " << pFile->offset << endl;
}

void FileManager::Read()
{
	//直接调用Rdwr()函数即可
	this->Rdwr(File::FREAD);
}

void FileManager::Write()
{
	//直接调用Rdwr()函数即可
	this->Rdwr(File::FWRITE);
}

void FileManager::Rdwr(enum File::FileFlags mode)
{
	File* pFile;
	//根据Read()/Write()的系统调用参数fd获取打开文件控制块结构
	pFile = g_UserCall.ofiles.GetF(g_UserCall.arg[0]);
	if (NULL == pFile) //不存在该打开文件，GetF已经设置过出错码，所以这里不需要再设置了
		return;

    g_UserCall.IOParam.base = (unsigned char*)g_UserCall.arg[1]; //目标缓冲区首址
	g_UserCall.IOParam.count = g_UserCall.arg[2];                //要求读/写的字节数

	g_UserCall.IOParam.offset = pFile->offset; //设置文件起始读位置
	if (File::FREAD == mode)
		pFile->inode->ReadI();
	else
		pFile->inode->WriteI();
	//根据读写字数，移动文件读写偏移指针
	pFile->offset += (g_UserCall.arg[2] - g_UserCall.IOParam.count);
	//返回实际读写的字节数，修改存放系统调用返回值的核心栈单元
	g_UserCall.ar0[UserCall::EAX] = g_UserCall.arg[2] - g_UserCall.IOParam.count;
}

void FileManager::Ls()
{
	INode* pINode = g_UserCall.nowDirINodePointer;
	Buf* pCache = NULL;
    g_UserCall.IOParam.offset = 0;
    g_UserCall.IOParam.count = pINode->i_size / sizeof(DirectoryEntry);
	while (g_UserCall.IOParam.count) {
		if (0 == g_UserCall.IOParam.offset % INode::BLOCK_SIZE) {
			if (pCache)
				g_BufferManager.Brelse(pCache);
			int phyBlkno = pINode->Bmap(g_UserCall.IOParam.offset / INode::BLOCK_SIZE);
			pCache = g_BufferManager.Bread(phyBlkno);
		}
		memcpy(&g_UserCall.dent, pCache->addr + (g_UserCall.IOParam.offset % INode::BLOCK_SIZE), sizeof(g_UserCall.dent));
        g_UserCall.IOParam.offset += sizeof(DirectoryEntry);
		g_UserCall.IOParam.count--;

		if (0 == g_UserCall.dent.m_ino)
			continue;

        g_UserCall.ls += g_UserCall.dent.name;
        g_UserCall.ls += "\n";
	}

	if (pCache)
		g_BufferManager.Brelse(pCache);
}

void FileManager::Rename(string ori, string cur)
{
	INode* pINode = g_UserCall.nowDirINodePointer;
	Buf* pCache = NULL;
    g_UserCall.IOParam.offset = 0;
    g_UserCall.IOParam.count = pINode->i_size / sizeof(DirectoryEntry);
	while (g_UserCall.IOParam.count) {
		if (0 == g_UserCall.IOParam.offset % INode::BLOCK_SIZE) {
			if (pCache)
				g_BufferManager.Brelse(pCache);
			int phyBlkno = pINode->Bmap(g_UserCall.IOParam.offset / INode::BLOCK_SIZE);
			pCache = g_BufferManager.Bread(phyBlkno);
		}

		DirectoryEntry tmp;
		memcpy(&tmp, pCache->addr + (g_UserCall.IOParam.offset % INode::BLOCK_SIZE), sizeof(g_UserCall.dent));

		if (strcmp(tmp.name, ori.c_str()) == 0) {
			strcpy(tmp.name, cur.c_str());
			memcpy(pCache->addr + (g_UserCall.IOParam.offset % INode::BLOCK_SIZE), &tmp, sizeof(g_UserCall.dent));
		}
        g_UserCall.IOParam.offset += sizeof(DirectoryEntry);
		g_UserCall.IOParam.count--;
	}

	if (pCache)
		g_BufferManager.Brelse(pCache);
}

//改变当前工作目录
void FileManager::ChDir()
{
	INode* pINode;
	pINode = this->NameI(FileManager::OPEN);
	if (NULL == pINode)
		return;
	//搜索到的文件不是目录文件
	if ((pINode->i_mode & INode::IFMT) != INode::IFDIR) {
        g_UserCall.userErrorCode = UserCall::U_ENOTDIR;
		this->m_inodeTable->IPut(pINode);
		return;
	}

    g_UserCall.nowDirINodePointer = pINode;
	//路径不是从根目录'/'开始，则在现有u.u_curdir后面加上当前路径分量
	if (g_UserCall.dirp[0] != '/')
        g_UserCall.curDirPath += g_UserCall.dirp;
	else //如果是从根目录'/'开始，则取代原有工作目录
		g_UserCall.curDirPath = g_UserCall.dirp;
	if (g_UserCall.curDirPath.back() != '/')
		g_UserCall.curDirPath.push_back('/');
}
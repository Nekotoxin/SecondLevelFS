/* 
 * Created by nekotoxin on 23-3-31.
 * */

#include "UserManager.h"

extern INodeTable g_INodeTable;

/*  用户管理器构造：初始化用户数组, root用户初始化 */
UserManager::UserManager() {
    for (int i = 0; i < USER_N; ++i) {
        (this->pusers)[i] = NULL;
    }
    this->user_addr.clear();    /*  清空线程id与用户数组索引的映射 */
    pthread_t pid = pthread_self();
    user_addr[pid] = 0; /*  root用户 */
    pusers[0] = (User *) malloc(sizeof(User));
    pusers[0]->u_uid = 0;
}

/*  获得当前线程的 User */
User *UserManager::GetUser() {
    pthread_t pthread_id = pthread_self();
    if (user_addr.find(pthread_id) == user_addr.end()) {
        printf("ERR: 线程 %p 的 User 结构无法得到，系统错误.\n", pthread_id);
        exit(1);
    }
    return pusers[user_addr[pthread_id]];
}

UserManager::~UserManager() {
    for (auto &puser: this->pusers) {
        if (puser != NULL)
            free(puser);
    }
}

bool UserManager::Login(string uname) {
    /*  取得线程 id */
    pthread_t pthread_id = pthread_self();
    /*  检查该线程是否已登录 */
    if (user_addr.find(pthread_id) != user_addr.end()) {
        printf("ERR: 线程 %p 重复登录\n", pthread_id);
        return false;
    }
    /*  寻找空闲的pusers指针 */
    int i;
    for (i = 0; i < USER_N; ++i) {
        if (pusers[i] == NULL) {
            break;
        }
    }
    if (i == USER_N) {
        printf("ERR: UserManager无空闲资源可用，用户并发数量达到上限\n");
        return false;
    }
    /*  i 为空闲索引 */
    pusers[i] = (User *) malloc(sizeof(User));
    if (pusers[i] == NULL) {
        printf("ERR: UserManager申请堆空间失败\n");
        return false;
    }
    /*  建立pid与addr的关联 */
    user_addr[pthread_id] = i;
    pusers[i]->u_uid = 0;
    printf("[INFO] 线程 %p 登录成功.\n", pthread_id);
/* 设置 User 结构的初始值
 * 1. 关联根目录 */
    pusers[i]->u_cdir = g_INodeTable.IGet(FileSystem::ROOTINO);
    pusers[i]->u_cdir->NFrele();
    pusers[i]->u_curdir = "/";
    return true;

/* pusers[i]->u_cdir->NFrele();
 * strcpy(pusers[i]->u_curdir, "/");
 * 2. 尝试创建家目录
 * SecondFileKernel::Instance().Sys_CreatDir(uname);
 * 3. 转到家目录
 * pusers[i]->u_error = NOERROR;
 * char dirname[512] = {0};
 * strcpy(dirname, uname.c_str());
 * pusers[i]->u_dirp = dirname;
 * pusers[i]->u_arg[0] = (unsigned long long)(dirname);
 * FileManager &fimanag = SecondFileKernel::Instance().GetFileManager();
 * fimanag.ChDir();
 * return true; */
}

bool UserManager::Logout() {
    /* 将系统更新至磁盘
     * SecondFileKernel::Instance().Quit();
     * 取得线程 id */
    pthread_t pthread_id = pthread_self();
    /*  检查该线程是否已登录 */
    if (user_addr.find(pthread_id) == user_addr.end()) {
        printf("ERR: 线程 %p 未登录，无需登出\n", pthread_id);
        return false;
    }
    int i = user_addr[pthread_id];
    free(pusers[i]);
    user_addr.erase(pthread_id);
    printf("[INFO] 线程 %p 登出成功.\n", pthread_id);
    return true;
}

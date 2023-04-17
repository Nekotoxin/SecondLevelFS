//
// Created by nekotoxin on 23-3-31.
//

#ifndef SECONDLEVELFS_USERMANAGER_H
#define SECONDLEVELFS_USERMANAGER_H

#include <map>
#include "User.h"

class UserManager {
public:
    static const int USER_N = 100; // 最多支持100个用户同时在线
    UserManager();

    ~UserManager();

    bool Login(string uname);

    bool Logout();

    // 获得当前线程User
    User *GetUser();

public:
    // 线程pid -> User数组索引
    std::map<pthread_t, int> user_addr;
    // User数组
    User *pusers[USER_N];
};


#endif //SECONDLEVELFS_USERMANAGER_H

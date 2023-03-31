#define _CRT_SECURE_NO_WARNINGS

#include "DiskDriver.h"
#include "BufferManager.h"
#include "OpenFileManager.h"
#include "FileManager.h"
#include "SysCall.h"
#include "Kernel.h"
#include <iostream>
#include <sstream>
#include <unordered_map>

using namespace std;
extern SysCall g_UserCall;

bool AutoTest() {
    User* u=Kernel::Instance().GetUserManager().GetUser();
    cout << "注意：自动测试不包含格式化操作" << endl;
    cout << "由于测试程序中的文件句柄写定，所以如果之前打开过文件可能会执行出错，建议第一步就自动测试" << endl;
    cout << "shell " << u->u_curdir << " > " << "mkdir /bin" << endl;
    g_UserCall.sysMkDir("/bin");
    cout << "shell " << u->u_curdir << " > " << "mkdir /etc" << endl;
    g_UserCall.sysMkDir("/etc");
    cout << "shell " << u->u_curdir << " > " << "mkdir /home" << endl;
    g_UserCall.sysMkDir("/home");
    cout << "shell " << u->u_curdir << " > " << "mkdir /dev" << endl;
    g_UserCall.sysMkDir("/dev");
    cout << "shell " << u->u_curdir << " > " << "ls" << endl;
    g_UserCall.sysLs();

    cout << "shell " << u->u_curdir << " > " << "mkdir /home/texts" << endl;
    g_UserCall.sysMkDir("/home/texts");
    cout << "shell " << u->u_curdir << " > " << "mkdir /home/reports" << endl;
    g_UserCall.sysMkDir("/home/reports");
    cout << "shell " << u->u_curdir << " > " << "mkdir /home/photos" << endl;
    g_UserCall.sysMkDir("/home/photos");
    cout << "shell " << u->u_curdir << " > " << "ftree /" << endl;
    g_UserCall.sysTree("/");

    cout << "shell " << u->u_curdir << " > " << "cd /home/texts" << endl;
    g_UserCall.sysCd("/home/texts");
    cout << "shell " << u->u_curdir << " > " << "fcreate Readme.txt" << endl;
    g_UserCall.sysCreate("Readme.txt");
    cout << "shell " << u->u_curdir << " > " << "fopen Readme.txt" << endl;
    g_UserCall.sysOpen("Readme.txt");
    cout << "shell " << u->u_curdir << " > " << "fwrite 8 Readme.txt 2609" << endl;
    g_UserCall.sysWrite("8", "Readme.txt", "2609");
    cout << "shell " << u->u_curdir << " > " << "fseek 8 0 begin" << endl;
    g_UserCall.sysSeek("8", "0", "0");
    cout << "shell " << u->u_curdir << " > " << "fread 8 ReadmeOut.txt 2609" << endl;
    g_UserCall.sysRead("8", "ReadmeOut.txt", "2609");
    cout << "shell " << u->u_curdir << " > " << "fclose 8" << endl;
    g_UserCall.sysClose("8");

    cout << "shell " << u->u_curdir << " > " << "cd /home/reports" << endl;
    g_UserCall.sysCd("/home/reports");
    cout << "shell " << u->u_curdir << " > " << "fcreate Report.pdf" << endl;
    g_UserCall.sysCreate("Report.pdf");
    cout << "shell " << u->u_curdir << " > " << "fopen Report.pdf" << endl;
    g_UserCall.sysOpen("Report.pdf");
    cout << "shell " << u->u_curdir << " > " << "fwrite 9 Report.pdf 1604288" << endl;
    g_UserCall.sysWrite("9", "Report.pdf", "1604288");
    cout << "shell " << u->u_curdir << " > " << "fseek 9 0 begin" << endl;
    g_UserCall.sysSeek("9", "0", "0");
    cout << "shell " << u->u_curdir << " > " << "fread 9 ReportOut.pdf 1604288" << endl;
    g_UserCall.sysRead("9", "ReportOut.pdf", "1604288");
    cout << "shell " << u->u_curdir << " > " << "fclose 9" << endl;
    g_UserCall.sysClose("9");

    cout << "shell " << u->u_curdir << " > " << "cd /home/photos" << endl;
    g_UserCall.sysCd("/home/photos");
    cout << "shell " << u->u_curdir << " > " << "fcreate DennisRitchie.jpg" << endl;
    g_UserCall.sysCreate("DennisRitchie.jpg");
    cout << "shell " << u->u_curdir << " > " << "fopen DennisRitchie.jpg" << endl;
    g_UserCall.sysOpen("DennisRitchie.jpg");
    cout << "shell " << u->u_curdir << " > " << "fwrite 10 DennisRitchie.jpg 7402" << endl;
    g_UserCall.sysWrite("10", "DennisRitchie.jpg", "7402");
    cout << "shell " << u->u_curdir << " > " << "fread 10 DennisRitchieOut.jpg 7402" << endl;
    g_UserCall.sysSeek("10", "0", "0");
    cout << "shell " << u->u_curdir << " > " << "fclose 10" << endl;
    g_UserCall.sysRead("10", "DennisRitchieOut.jpg", "7402");

    cout << "shell " << u->u_curdir << " > " << "mkdir /test" << endl;
    g_UserCall.sysMkDir("/test");
    cout << "shell " << u->u_curdir << " > " << "cd /test" << endl;
    g_UserCall.sysCd("/test");
    cout << "shell " << u->u_curdir << " > " << "fcreate Jerry" << endl;
    g_UserCall.sysCreate("Jerry");
    cout << "shell " << u->u_curdir << " > " << "fopen Jerry" << endl;
    g_UserCall.sysOpen("Jerry");
    cout << "shell " << u->u_curdir << " > " << "fwrite 13 input.txt 800" << endl;
    g_UserCall.sysWrite("13", "input.txt", "800");
    cout << "shell " << u->u_curdir << " > " << "fseek 13 500 begin" << endl;
    g_UserCall.sysSeek("13", "500", "0");
    cout << "shell " << u->u_curdir << " > " << "fread 13 std 500" << endl;
    g_UserCall.sysRead("13", "std", "500");
    cout << "shell " << u->u_curdir << " > " << "fseek 13 500 begin" << endl;
    g_UserCall.sysSeek("13", "500", "0");
    cout << "shell " << u->u_curdir << " > " << "fread 13 abc.txt 500" << endl;
    g_UserCall.sysRead("13", "abc.txt", "500");
    cout << "shell " << u->u_curdir << " > " << "fwrite 13 abc.txt 300" << endl;
    g_UserCall.sysWrite("13", "abc.txt", "300");
    cout << "shell " << u->u_curdir << " > " << "fclose 13" << endl;
    g_UserCall.sysClose("13");
    cout << "shell " << u->u_curdir << " > " << "ftree /" << endl;
    g_UserCall.sysTree("/");
    cout << "shell " << u->u_curdir << " > " << "cd /test" << endl;
    g_UserCall.sysCd("/test");
    cout << "shell " << u->u_curdir << " > " << "frename Jerry Larry" << endl;
    g_UserCall.sysRename("Jerry", "Larry");
    cout << "shell " << u->u_curdir << " > " << "ls" << endl;
    g_UserCall.sysLs();
    cout << "shell " << u->u_curdir << " > " << "ftree /" << endl;
    g_UserCall.sysTree("/");

    cout << "自动测试结束" << endl << endl;
    return true;
}

int main() {
//	SysCall& u = g_UserCall;
    Kernel::Instance().Initialize();
//    return 0;

    User* u=Kernel::Instance().GetUserManager().GetUser();

    cout << "***************************************************************************************" << endl
         << "*                                                                                     *" << endl
         << "*                                   类Unix文件系统                                    *" << endl
         << "*                                                                                     *" << endl
         << "* [操作说明]:                                                                         *" << endl
         << "* [命令]:help <op_name>\t[功能]:命令提示                                               *" << endl
         << "* [命令]:test\t[功能]:自动测试                                                       *" << endl
         << "* [命令]:fformat\t[功能]:格式化文件系统                                         *" << endl
         << "* [命令]:ls\t[功能]:查看当前目录内容                                               *" << endl
         << "* [命令]:mkdir <dirname>\t[功能]:生成文件夹                                     *" << endl
         << "* [命令]:cd <dirname>\t[功能]:进入目录                                               *" << endl
         << "* [命令]:fcreate <filename>\t[功能]:创建文件名为filename的文件                     *" << endl
         << "* [命令]:fopen <filename>\t[功能]:打开文件名为filename的文件                     *" << endl
         << "* [命令]:fwrite <fd> <infile> <size>\t[功能]:从infile输入，写入fd文件size字节       *" << endl
         << "* [命令]:fread <fd> <outfile> <size>\t[功能]:从fd文件读取size字节，输出到outfile    *" << endl
         << "* [命令]:fread <fd> std <size>\t[功能]:从fd文件读取size字节，输出到屏幕               *" << endl
         << "* [命令]:fseek <fd> <step> begin\t[功能]:以begin模式把fd文件指针偏移step        *" << endl
         << "* [命令]:fseek <fd> <step> cur\t[功能]:以cur模式把fd文件指针偏移step                  *" << endl
         << "* [命令]:fseek <fd> <step> end\t[功能]:以end模式把fd文件指针偏移step                  *" << endl
         << "* [命令]:fclose <fd>\t[功能]:关闭文件句柄为fd的文件                                 *" << endl
         << "* [命令]:fdelete <filename>\t[功能]:删除文件文件名为filename的文件或者文件夹       *" << endl
         << "* [命令]:frename <filename> <filename1>\t[功能]:将文件fliename重命名为filename1        *" << endl
         << "* [命令]:ftree <dirname>\t[功能]:列出dirname的文件目录树                        *" << endl
         << "* [命令]:exit \t[功能]:退出系统，并将缓存内容存至磁盘                                 *" << endl
         << "***************************************************************************************" << endl;

    string line, opt, val[3];
    while (true) {
        cout << "shell " << u->u_curdir << " > ";
        getline(cin, line);
        if (line.size() == 0)
            continue;

        stringstream in(line);
        in >> opt;
        val[0] = val[1] = val[2] = "";

        //格式化文件系统
        if (opt == "fformat") {
            //Us.sysCd("/");
//			g_OpenFileTable.Reset();
//			g_INodeTable.Reset();
//			g_BufferManager.FormatBuffer();
//            g_FileSystem.Format();
            //g_UserCall.u_ofiles.Reset();
            cout << "格式化完毕，文件系统已退出，请重新启动！" << endl;
            return 0;
        }
            //查看当前目录内容
        else if (opt == "ls")
            g_UserCall.sysLs();
            //生成文件夹
        else if (opt == "mkdir") {
            in >> val[0];
            if (val[0][0] != '/')
                val[0] = u->u_curdir + val[0];
            g_UserCall.sysMkDir(val[0]);
        }
            //进入目录
        else if (opt == "cd") {
            in >> val[0];
            g_UserCall.sysCd(val[0]);
        }
            //创建文件名为filename的文件
        else if (opt == "fcreate") {
            in >> val[0];
            if (val[0][0] != '/')
                val[0] = u->u_curdir + val[0];
            g_UserCall.sysCreate(val[0]);
        }
            //打开文件名为filename的文件
        else if (opt == "fopen") {
            in >> val[0];
            if (u->u_ar0[User::EAX] == 0) {
                g_UserCall.sysMkDir("demo");
                g_UserCall.sysDelete("demo");
            }
            if (val[0][0] != '/')
                val[0] = u->u_curdir + val[0];

            g_UserCall.sysOpen(val[0]);
        }
            //退出系统，并将缓存内容存至磁盘
        else if (opt == "exit")
            return 0;
            //关闭文件句柄为fd的文件
        else if (opt == "fclose") {
            in >> val[0];
            g_UserCall.sysClose(val[0]);
        } else if (opt == "fseek") {
            in >> val[0] >> val[1] >> val[2];
            //以begin模式把fd文件指针偏移step
            if (val[2] == "begin")
                g_UserCall.sysSeek(val[0], val[1], string("0"));
                //以cur模式把fd文件指针偏移step
            else if (val[2] == "cur")
                g_UserCall.sysSeek(val[0], val[1], string("1"));
                //以end模式把fd文件指针偏移step
            else if (val[2] == "end")
                g_UserCall.sysSeek(val[0], val[1], string("2"));
        }
            //从fd文件读取size字节，输出到outfile
            //从fd文件读取size字节，输出到屏幕
        else if (opt == "fread") {
            in >> val[0] >> val[1] >> val[2];
            g_UserCall.sysRead(val[0], val[1], val[2]);
        }
            //从infile输入，写入fd文件size字节
        else if (opt == "fwrite") {
            in >> val[0] >> val[1] >> val[2];
            g_UserCall.sysWrite(val[0], val[1], val[2]);
        }
            //删除文件文件名为filename的文件或者文件夹
        else if (opt == "fdelete") {
            in >> val[0];
            if (val[0][0] != '/')
                val[0] = u->u_curdir + val[0];
            g_UserCall.sysDelete(val[0]);
        } else if (opt == "test")
            AutoTest();
            //重命名文件或文件夹
        else if (opt == "frename") {
            in >> val[0] >> val[1];
            g_UserCall.sysRename(val[0], val[1]);
        } else if (opt == "ftree") {
            in >> val[0];
            g_UserCall.sysTree(val[0]);
        } else if (opt == "help") {
            in >> val[0];
            if (val[0] == "" || val[0] == "?") {
                cout << "[命令]:test\t[功能]:自动测试" << endl
                     << "[命令]:fformat\t[功能]:格式化文件系统" << endl
                     << "[命令]:ls\t[功能]:查看当前目录内容" << endl
                     << "[命令]:mkdir <dirname>\t[功能]:生成文件夹" << endl
                     << "[命令]:cd <dirname>\t[功能]:进入目录" << endl
                     << "[命令]:fcreate <filename>\t[功能]:创建文件名为filename的文件" << endl
                     << "[命令]:fopen <filename>\t[功能]:打开文件名为filename的文件" << endl
                     << "[命令]:fwrite <fd> <infile> <size>\t[功能]:从infile输入，写入fd文件size字节" << endl
                     << "[命令]:fread <fd> <outfile> <size>\t[功能]:从fd文件读取size字节，输出到outfile" << endl
                     << "[命令]:fread <fd> std <size>\t[功能]:从fd文件读取size字节，输出到屏幕" << endl
                     << "[命令]:fseek <fd> <step> begin\t[功能]:以begin模式把fd文件指针偏移step" << endl
                     << "[命令]:fseek <fd> <step> cur\t[功能]:以cur模式把fd文件指针偏移step" << endl
                     << "[命令]:fseek <fd> <step> end\t[功能]:以end模式把fd文件指针偏移step" << endl
                     << "[命令]:fclose <fd>\t[功能]:关闭文件句柄为fd的文件" << endl
                     << "[命令]:fdelete <filename>\t[功能]:删除文件文件名为filename的文件或者文件夹" << endl
                     << "[命令]:frename <filename> <filename1>\t[功能]:将文件fliename重命名为filename1" << endl
                     << "[命令]:ftree <dirname>\t[功能]:列出dirname的文件目录树" << endl
                     << "[命令]:exit\t[功能]:退出系统，并将缓存内容存至磁盘" << endl;
            } else if (val[0] == "test")
                cout << "[命令]:test\t[功能]:自动测试" << endl;
            else if (val[0] == "fformat")
                cout << "[命令]:fformat\t[功能]:格式化文件系统" << endl;
            else if (val[0] == "ls")
                cout << "[命令]:ls\t[功能]:查看当前目录内容" << endl;
            else if (val[0] == "mkdir")
                cout << "[命令]:mkdir <dirname>\t[功能]:生成文件夹" << endl;
            else if (val[0] == "cd")
                cout << "[命令]:cd <dirname>\t[功能]:进入目录" << endl;
            else if (val[0] == "fcreate")
                cout << "[命令]:fcreate <filename>\t[功能]:创建文件名为filename的文件" << endl;
            else if (val[0] == "fopen")
                cout << "[命令]:fopen <filename>\t[功能]:打开文件名为filename的文件" << endl;
            else if (val[0] == "fwrite")
                cout << "[命令]:fwrite <fd> <infile> <size>\t[功能]:从infile输入，写入fd文件size字节" << endl;
            else if (val[0] == "fread")
                cout << "[命令]:fread <fd> <outfile> <size>\t[功能]:从fd文件读取size字节，输出到outfile" << endl
                     << "[命令]:fread <fd> std <size>\t[功能]:从fd文件读取size字节，输出到屏幕" << endl;
            else if (val[0] == "fseek")
                cout << "[命令]:fseek <fd> <step> begin\t[功能]:以begin模式把fd文件指针偏移step" << endl
                     << "[命令]:fseek <fd> <step> cur\t[功能]:以cur模式把fd文件指针偏移step" << endl
                     << "[命令]:fseek <fd> <step> end\t[功能]:以end模式把fd文件指针偏移step" << endl;
            else if (val[0] == "fclose")
                cout << "[命令]:fclose <fd>\t[功能]:关闭文件句柄为fd的文件" << endl;
            else if (val[0] == "fdelete")
                cout << "[命令]:fdelete <filename>\t[功能]:删除文件文件名为filename的文件或者文件夹" << endl;
            else if (val[0] == "frename")
                cout << "[命令]:frename <filename> <filename1>\t[功能]:将文件fliename重命名为filename1" << endl;
            else if (val[0] == "ftree")
                cout << "[命令]:ftree <dirname>\t[功能]:列出dirname的文件目录树" << endl;
            else if (val[0] == "exit")
                cout << "[命令]:exit\t[功能]:退出系统，并将缓存内容存至磁盘" << endl;
        }
    }
    return 0;
}
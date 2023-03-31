#define _CRT_SECURE_NO_WARNINGS

#include "DiskDriver.h"
#include "BufferManager.h"
#include "OpenFileManager.h"
#include "FileManager.h"
#include "Kernel.h"
#include <iostream>
#include <sstream>
#include <unordered_map>

using namespace std;

bool AutoTest() {
    User *u = Kernel::Instance().GetUserManager().GetUser();
    cout << "注意：自动测试不包含格式化操作" << endl;
    cout << "由于测试程序中的文件句柄写定，所以如果之前打开过文件可能会执行出错，建议第一步就自动测试" << endl;
    cout << "shell " << u->u_curdir << " > " << "mkdir /bin" << endl;
    Kernel::Instance().sysMkDir("/bin");
    cout << "shell " << u->u_curdir << " > " << "mkdir /etc" << endl;
    Kernel::Instance().sysMkDir("/etc");
    cout << "shell " << u->u_curdir << " > " << "mkdir /home" << endl;
    Kernel::Instance().sysMkDir("/home");
    cout << "shell " << u->u_curdir << " > " << "mkdir /dev" << endl;
    Kernel::Instance().sysMkDir("/dev");
    cout << "shell " << u->u_curdir << " > " << "ls" << endl;
    Kernel::Instance().sysLs();

    cout << "shell " << u->u_curdir << " > " << "mkdir /home/texts" << endl;
    Kernel::Instance().sysMkDir("/home/texts");
    cout << "shell " << u->u_curdir << " > " << "mkdir /home/reports" << endl;
    Kernel::Instance().sysMkDir("/home/reports");
    cout << "shell " << u->u_curdir << " > " << "mkdir /home/photos" << endl;
    Kernel::Instance().sysMkDir("/home/photos");
    cout << "shell " << u->u_curdir << " > " << "ftree /" << endl;
    Kernel::Instance().sysTree("/");

    cout << "shell " << u->u_curdir << " > " << "cd /home/texts" << endl;
    Kernel::Instance().sysCd("/home/texts");
    cout << "shell " << u->u_curdir << " > " << "fcreate Readme.txt" << endl;
    Kernel::Instance().sysCreate("Readme.txt");
    cout << "shell " << u->u_curdir << " > " << "fopen Readme.txt" << endl;
    Kernel::Instance().sysOpen("Readme.txt");
    cout << "shell " << u->u_curdir << " > " << "fwrite 8 Readme.txt 2609" << endl;
    Kernel::Instance().sysWrite("8", "Readme.txt", "2609");
    cout << "shell " << u->u_curdir << " > " << "fseek 8 0 begin" << endl;
    Kernel::Instance().sysSeek("8", "0", "0");
    cout << "shell " << u->u_curdir << " > " << "fread 8 ReadmeOut.txt 2609" << endl;
    Kernel::Instance().sysRead("8", "ReadmeOut.txt", "2609");
    cout << "shell " << u->u_curdir << " > " << "fclose 8" << endl;
    Kernel::Instance().sysClose("8");

    cout << "shell " << u->u_curdir << " > " << "cd /home/reports" << endl;
    Kernel::Instance().sysCd("/home/reports");
    cout << "shell " << u->u_curdir << " > " << "fcreate Report.pdf" << endl;
    Kernel::Instance().sysCreate("Report.pdf");
    cout << "shell " << u->u_curdir << " > " << "fopen Report.pdf" << endl;
    Kernel::Instance().sysOpen("Report.pdf");
    cout << "shell " << u->u_curdir << " > " << "fwrite 9 Report.pdf 1604288" << endl;
    Kernel::Instance().sysWrite("9", "Report.pdf", "1604288");
    cout << "shell " << u->u_curdir << " > " << "fseek 9 0 begin" << endl;
    Kernel::Instance().sysSeek("9", "0", "0");
    cout << "shell " << u->u_curdir << " > " << "fread 9 ReportOut.pdf 1604288" << endl;
    Kernel::Instance().sysRead("9", "ReportOut.pdf", "1604288");
    cout << "shell " << u->u_curdir << " > " << "fclose 9" << endl;
    Kernel::Instance().sysClose("9");

    cout << "shell " << u->u_curdir << " > " << "cd /home/photos" << endl;
    Kernel::Instance().sysCd("/home/photos");
    cout << "shell " << u->u_curdir << " > " << "fcreate DennisRitchie.jpg" << endl;
    Kernel::Instance().sysCreate("DennisRitchie.jpg");
    cout << "shell " << u->u_curdir << " > " << "fopen DennisRitchie.jpg" << endl;
    Kernel::Instance().sysOpen("DennisRitchie.jpg");
    cout << "shell " << u->u_curdir << " > " << "fwrite 10 DennisRitchie.jpg 7402" << endl;
    Kernel::Instance().sysWrite("10", "DennisRitchie.jpg", "7402");
    cout << "shell " << u->u_curdir << " > " << "fread 10 DennisRitchieOut.jpg 7402" << endl;
    Kernel::Instance().sysSeek("10", "0", "0");
    cout << "shell " << u->u_curdir << " > " << "fclose 10" << endl;
    Kernel::Instance().sysRead("10", "DennisRitchieOut.jpg", "7402");

    cout << "shell " << u->u_curdir << " > " << "mkdir /test" << endl;
    Kernel::Instance().sysMkDir("/test");
    cout << "shell " << u->u_curdir << " > " << "cd /test" << endl;
    Kernel::Instance().sysCd("/test");
    cout << "shell " << u->u_curdir << " > " << "fcreate Jerry" << endl;
    Kernel::Instance().sysCreate("Jerry");
    cout << "shell " << u->u_curdir << " > " << "fopen Jerry" << endl;
    Kernel::Instance().sysOpen("Jerry");
    cout << "shell " << u->u_curdir << " > " << "fwrite 13 input.txt 800" << endl;
    Kernel::Instance().sysWrite("13", "input.txt", "800");
    cout << "shell " << u->u_curdir << " > " << "fseek 13 500 begin" << endl;
    Kernel::Instance().sysSeek("13", "500", "0");
    cout << "shell " << u->u_curdir << " > " << "fread 13 std 500" << endl;
    Kernel::Instance().sysRead("13", "std", "500");
    cout << "shell " << u->u_curdir << " > " << "fseek 13 500 begin" << endl;
    Kernel::Instance().sysSeek("13", "500", "0");
    cout << "shell " << u->u_curdir << " > " << "fread 13 abc.txt 500" << endl;
    Kernel::Instance().sysRead("13", "abc.txt", "500");
    cout << "shell " << u->u_curdir << " > " << "fwrite 13 abc.txt 300" << endl;
    Kernel::Instance().sysWrite("13", "abc.txt", "300");
    cout << "shell " << u->u_curdir << " > " << "fclose 13" << endl;
    Kernel::Instance().sysClose("13");
    cout << "shell " << u->u_curdir << " > " << "ftree /" << endl;
    Kernel::Instance().sysTree("/");
    cout << "shell " << u->u_curdir << " > " << "cd /test" << endl;
    Kernel::Instance().sysCd("/test");
    cout << "shell " << u->u_curdir << " > " << "frename Jerry Larry" << endl;
    Kernel::Instance().sysRename("Jerry", "Larry");
    cout << "shell " << u->u_curdir << " > " << "ls" << endl;
    Kernel::Instance().sysLs();
    cout << "shell " << u->u_curdir << " > " << "ftree /" << endl;
    Kernel::Instance().sysTree("/");

    cout << "自动测试结束" << endl << endl;
    return true;
}

int main() {
    Kernel::Instance().Initialize();
    User *u = Kernel::Instance().GetUserManager().GetUser();

    cout << "***************************************************************************************" << endl
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
            //Kernel::Instance().u_ofiles.Reset();
            cout << "格式化完毕，文件系统已退出，请重新启动！" << endl;
            return 0;
        }
            //查看当前目录内容
        else if (opt == "ls")
            Kernel::Instance().sysLs();
            //生成文件夹
        else if (opt == "mkdir") {
            in >> val[0];
            if (val[0][0] != '/')
                val[0] = u->u_curdir + val[0];
            Kernel::Instance().sysMkDir(val[0]);
        }
            //进入目录
        else if (opt == "cd") {
            in >> val[0];
            Kernel::Instance().sysCd(val[0]);
        }
            //创建文件名为filename的文件
        else if (opt == "fcreate") {
            in >> val[0];
            if (val[0][0] != '/')
                val[0] = u->u_curdir + val[0];
            Kernel::Instance().sysCreate(val[0]);
        }
            //打开文件名为filename的文件
        else if (opt == "fopen") {
            in >> val[0];
            if (u->u_ar0[User::EAX] == 0) {
                Kernel::Instance().sysMkDir("demo");
                Kernel::Instance().sysDelete("demo");
            }
            if (val[0][0] != '/')
                val[0] = u->u_curdir + val[0];

            Kernel::Instance().sysOpen(val[0]);
        }
            //退出系统，并将缓存内容存至磁盘
        else if (opt == "exit")
            return 0;
            //关闭文件句柄为fd的文件
        else if (opt == "fclose") {
            in >> val[0];
            Kernel::Instance().sysClose(val[0]);
        } else if (opt == "fseek") {
            in >> val[0] >> val[1] >> val[2];
            //以begin模式把fd文件指针偏移step
            if (val[2] == "begin")
                Kernel::Instance().sysSeek(val[0], val[1], string("0"));
                //以cur模式把fd文件指针偏移step
            else if (val[2] == "cur")
                Kernel::Instance().sysSeek(val[0], val[1], string("1"));
                //以end模式把fd文件指针偏移step
            else if (val[2] == "end")
                Kernel::Instance().sysSeek(val[0], val[1], string("2"));
        }
            //从fd文件读取size字节，输出到outfile
            //从fd文件读取size字节，输出到屏幕
        else if (opt == "fread") {
            in >> val[0] >> val[1] >> val[2];
            Kernel::Instance().sysRead(val[0], val[1], val[2]);
        }
            //从infile输入，写入fd文件size字节
        else if (opt == "fwrite") {
            in >> val[0] >> val[1] >> val[2];
            Kernel::Instance().sysWrite(val[0], val[1], val[2]);
        }
            //删除文件文件名为filename的文件或者文件夹
        else if (opt == "fdelete") {
            in >> val[0];
            if (val[0][0] != '/')
                val[0] = u->u_curdir + val[0];
            Kernel::Instance().sysDelete(val[0]);
        } else if (opt == "test")
            AutoTest();
            //重命名文件或文件夹
        else if (opt == "frename") {
            in >> val[0] >> val[1];
            Kernel::Instance().sysRename(val[0], val[1]);
        } else if (opt == "ftree") {
            in >> val[0];
            Kernel::Instance().sysTree(val[0]);
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
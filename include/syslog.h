/******************************************************************************
* ��¼�������е���־��Ϣ
*******************************************************************************/

#ifndef __SYSLOG_H__
#define __SYSLOG_H__


#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/io.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/prctl.h>


#include "CPosixMutex.h"


using namespace std;

#define KEEP_TIME_H     60*60       // �ļ�����ʱ�� СʱΪ��λ
#define MAX_SIZE_M      1024*1024   // �ļ���С MBΪ��λ
#define CHECK_TIME_M    60          // ����ļ�״̬���ʱ�� ����Ϊ��λ

#define LOG(fmt, ...) log_info(__FUNCTION__, fmt, ##__VA_ARGS__)


typedef map<string, int> map_si;

class SysLog
{
public:
    SysLog();
    SysLog(string log_file_name, int keep_time, int max_size);
    ~SysLog();

    int get_filename_from_dir(map_si &m_log_file, string input_path);
    string join_path(string &dest_path, string src_path, string add_path);
    void get_new_name(string &dest_name, string src_name);
    int rename_file(string src_name, string dest_name);
    void check_log_state();
    void run();

    int make_dir(string dir_path);
    int open_log_file(string log_file_name);

    static void *log_thread(void* arg);

public:
    ofstream fwrite;
    CPosixMutex mutex;
private:
    string      log_path_name;
    string      log_name;
    string      log_path;
    int         log_size;
    int         log_time;
};


// ȫ�ֺ��� ���ں궨��LOG
void log_info(const char *func, string fmt, ...);


#endif //__SYSLOG_H__


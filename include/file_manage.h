/******************************************************************************
* 记录程序运行的日志信息
*******************************************************************************/

#ifndef __FILE_MANAGE_H__
#define __FILE_MANAGE_H__


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


#include "tinyxml.h"

using namespace std;

#define TIME_M		60			// 文件保留时间 分钟为单位
#define TIME_H		3600		// 文件保留时间 小时为单位
#define TIME_D		86400		// 文件保留时间 天为单位

typedef struct _ManageDir
{	
	string src_path;
	string dest_path;
	int keep_time;
}
ManageDir;

typedef vector<ManageDir> vector_md;

class FileManage
{
public:
	FileManage();
	int run();
	int load_config_info(string file_name);
	int make_dir(string dir_path);
	string join_path(
						string &dest_path, 
						string src_path, 
						string add_path);
	int rename_file(string src_name, string dest_name);
	int remove_dir(string dir_path);
	int get_dir_from_time(string &dir, time_t tt);
	int get_dir_from_name(string &dir, string file_name);
	int back_file_to_dir(string src_path, string dest_path);
	int check_dir_time(string path, int keep_time);
	
	int manage_file();
	int manage_dir();


private:
	vector_md v_manage;
	int  wait_time;
	string front_word;
	string back_word;
};


void *manage_file_thread(void* arg);
void *manage_dir_thread(void* arg);




#endif //__FILE_MANAGE_H__


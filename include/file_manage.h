/******************************************************************************
* ��¼�������е���־��Ϣ
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

#define TIME_M		60			// �ļ�����ʱ�� ����Ϊ��λ
#define TIME_H		3600		// �ļ�����ʱ�� СʱΪ��λ
#define TIME_D		86400		// �ļ�����ʱ�� ��Ϊ��λ


typedef map<string, int> map_si;

class FileManage
{
public:
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
	int back_and_check(string path, int i_time);
	int manage_dir();


private:
	map_si m_manage;
};


void *manage_thread(void* arg);



#endif //__FILE_MANAGE_H__

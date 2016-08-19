/******************************************************************************

*******************************************************************************/

//////////////////////////////////////////////////////////////////////////
#ifndef __DATA_FILTER_H__
#define __DATA_FILTER_H__
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <sys/types.h>   
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <sys/prctl.h>



#include <string>
#include <sstream>
#include <fstream> 
#include <iostream>
#include <vector>
#include <map>
#include <deque>

using namespace std;
#include <boost/algorithm/string.hpp>

#include "tinyxml.h"
#include "brd_deque.h"

#define MAX_PATH 1024
#define THREAD_TMP_DIR "./tmp/thread"
#define INPUT_TMP_DIR "path"

typedef vector<string> vector_str;
typedef vector<int> vector_int;

typedef struct key
{
	string			key_name;		// 文件名关键字
	int				key_site;		// 文件名关键字位置,0为任意位置,1为头匹配,2为尾匹配
	bool operator <(const key& other) const
	{
		if(key_name < other.key_name)
		{
			return true;
		}
		else if(0 == key_name.compare(other.key_name))
		{
			return key_site < other.key_site;
		}
		return false;
	}
}Key;
typedef vector<Key> vector_key;


// 输入目录属性
typedef struct input
{
	string			input_path;		// 传入文件目录全路径
	string			exp_name;		// 文件后缀名,值为""表示任意后缀名
	string			bak_path;		// 
	bool operator <(const input& other) const
	{
		if(input_path < other.input_path)
		{
			return true;
		}
		else if(0 == input_path.compare(other.input_path))
		{
			return exp_name < other.exp_name;
		}
		else if(0 == exp_name.compare(other.exp_name))
		{
			return bak_path < other.bak_path;
		}
		return false;
	}
}Input;
typedef vector<Input> vector_input;

// 输出目录属性
typedef struct output
{
	string			output_path;	// 输出文件目录全路径
	int				del_size0;		// 1为删除，0为不删除
	bool operator <(const output& other) const
	{
		if(output_path < other.output_path)
		{
			return true;
		}
		else if(0 == output_path.compare(other.output_path))
		{
			return del_size0 < other.del_size0;
		}
		return false;
	}
}Output;
typedef vector<Output> vector_output;



// 配置属性
typedef struct
{
	vector_key		v_key;
	vector_input	v_input;
	vector_output	v_output;
	int				copy;			//copy: 是否copy文件, 0为不copy根据条件过滤, 1为直接完整copy不经过任何筛选
	string			backup_path;	// 源文件备份路径
	int				head; 			// 0为正常过滤首行,1、2为需要首行,-1、-2为不需要首行
	int				tail; 			// 0为正常过滤尾行,1、2为需要尾行,-1、-2为不需要尾行
	string			split;			// 文件列的分隔符
	int 			direction;		// 正反方向抽取标识 0为column和filter都正向，1为column反向，2为filter反向，3为column和filter都反向
	string			column;			// 值为需要抽取的列(0,1,2,3, "" is all) 注：当direction为方向时column是不需要抽取的列
	string			filter;			// 数据过滤表达式 值为""表示不需要过滤，col[num](expression)：num是列号，expression为过滤表达式
									// 注：当direction为方向时column是不需要抽取的列
}Config;

typedef struct 
{
	string tmp_path;
	string bak_path;
}Tmp_bak;

typedef CDeque<string> CDeque_str;

typedef vector<Config> vector_cf;

typedef map<Output, Config> map_oc;

typedef map<Input, map_oc> map_im;

typedef map<Key, map_im> map_km;

typedef map<string, Tmp_bak> map_st;


class FilterData
{
public:
	FilterData();
	~FilterData();

	/*******************************************
    * @brief : 初始化 config 结构体
    *******************************************/
	void init_config(Config *data);

	/*******************************************
    * @brief : 获取一条过滤条件信息
    *******************************************/
    void get_base_info(TiXmlElement *pElement, Config &m_config);

	/*******************************************
    * @brief : 创建文件夹
    *******************************************/
	int make_dir(string dir_path);
	
	/*******************************************
    * @brief : 加载配置文件信息
    *******************************************/
	int load_config_info(string str_file_name);

	/*******************************************
    * @brief : 检查配置文件完整性
    *******************************************/
	int check_config();

	int get_filter_map();

	int get_inpath_map();

	/*******************************************
    * @brief : 检测num是否在vector中
    *******************************************/
	bool is_in_vector(int num, vector_int vec);

	/*******************************************
    * @brief : 根据规则获取输出数据
    *******************************************/
	int get_out_vector(
		vector_str &v_out_str,
		vector_str v_str, 
		vector_int v_column,
		int direction,
		bool col_spill_flag);

	/*******************************************
    * @brief : 根据规则从指定目录获取文件名字
    *******************************************/
	int get_filename_from_dir(
		string src_path, 
		string input_path,
		bool is_check);

	/*******************************************
    * @brief : 根据规则从指定目录获取文件名字
    *******************************************/
	string join_path(
		string &dest_path, 
		string src_path, 
		string add_path);

	/*******************************************
    * @brief : 重命名文件
    *******************************************/
	int rename_file(string src_name, string dest_name);

	/*******************************************
    * @brief : 备份一个目录下的多个文件
    *******************************************/
	int copy_file(
		string src_path,
		string dest_path);

	/*******************************************
    * @brief : 处理一个目录下的多个文件
    *******************************************/
	int process_files(
		string file_path_name,
		string file_name,
		map_oc &m_oc);
	
	/*******************************************
    * @brief : 解析配置文件及数据筛选
    *******************************************/
	void analysis_config(size_t thread_num);

	/*******************************************
    * @brief : 得到需要输出的列
    *******************************************/
	int get_column(vector_int &v_column, string column);
	
	/*******************************************
    * @brief : or条件筛选
    *******************************************/
	bool field_filter_or(vector_str v_str, string filter, bool &fil_spill_flag);
	
	/*******************************************
    * @brief : and条件筛选
    *******************************************/
	bool field_filter_and(vector_str v_str, string filter, bool &fil_spill_flag);
	
	/*******************************************
    * @brief : 单个条件分解
    *******************************************/
	bool field_filter_data(vector_str v_str, string filter, bool &fil_spill_flag);

	/*******************************************
    * @brief : 具体条件比较
    *******************************************/
	bool condition_analysis(string item, string filter);

	int create_thread_dir();

	void scanning_file();

	void check_thread_temp();
	
	int make_all_dir();

	void str_add_int(string &dest, int num);

	int run(string config_path);

private:
	int max_thread_num;
	// 全局配置过滤条件
	Config global_info;
	
	// 具体配置过滤条件列表
	vector<Config> v_filter;

	map_km m_filter;
	
	map_st m_inpath_to_tmp;
	
	CDeque_str file_deque;
	
};

/*******************************************
    * @brief    : 传入线程函数的参数
*******************************************/
typedef struct
{
	FilterData *p_fd;
	size_t thread_num;
}Thread_arg;

/*******************************************
    * @brief    : 每条规则对应的处理线程
*******************************************/
void *filter_thread(void *arg);



#endif //__DATA_FILTER_H__


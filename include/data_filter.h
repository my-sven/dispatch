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
	string			key_name;		// �ļ����ؼ���
	int				key_site;		// �ļ����ؼ���λ��,0Ϊ����λ��,1Ϊͷƥ��,2Ϊβƥ��
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


// ����Ŀ¼����
typedef struct input
{
	string			input_path;		// �����ļ�Ŀ¼ȫ·��
	string			exp_name;		// �ļ���׺��,ֵΪ""��ʾ�����׺��
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

// ���Ŀ¼����
typedef struct output
{
	string			output_path;	// ����ļ�Ŀ¼ȫ·��
	int				del_size0;		// 1Ϊɾ����0Ϊ��ɾ��
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



// ��������
typedef struct
{
	vector_key		v_key;
	vector_input	v_input;
	vector_output	v_output;
	int				copy;			//copy: �Ƿ�copy�ļ�, 0Ϊ��copy������������, 1Ϊֱ������copy�������κ�ɸѡ
	string			backup_path;	// Դ�ļ�����·��
	int				head; 			// 0Ϊ������������,1��2Ϊ��Ҫ����,-1��-2Ϊ����Ҫ����
	int				tail; 			// 0Ϊ��������β��,1��2Ϊ��Ҫβ��,-1��-2Ϊ����Ҫβ��
	string			split;			// �ļ��еķָ���
	int 			direction;		// ���������ȡ��ʶ 0Ϊcolumn��filter������1Ϊcolumn����2Ϊfilter����3Ϊcolumn��filter������
	string			column;			// ֵΪ��Ҫ��ȡ����(0,1,2,3, "" is all) ע����directionΪ����ʱcolumn�ǲ���Ҫ��ȡ����
	string			filter;			// ���ݹ��˱��ʽ ֵΪ""��ʾ����Ҫ���ˣ�col[num](expression)��num���кţ�expressionΪ���˱��ʽ
									// ע����directionΪ����ʱcolumn�ǲ���Ҫ��ȡ����
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
    * @brief : ��ʼ�� config �ṹ��
    *******************************************/
	void init_config(Config *data);

	/*******************************************
    * @brief : ��ȡһ������������Ϣ
    *******************************************/
    void get_base_info(TiXmlElement *pElement, Config &m_config);

	/*******************************************
    * @brief : �����ļ���
    *******************************************/
	int make_dir(string dir_path);
	
	/*******************************************
    * @brief : ���������ļ���Ϣ
    *******************************************/
	int load_config_info(string str_file_name);

	/*******************************************
    * @brief : ��������ļ�������
    *******************************************/
	int check_config();

	int get_filter_map();

	int get_inpath_map();

	/*******************************************
    * @brief : ���num�Ƿ���vector��
    *******************************************/
	bool is_in_vector(int num, vector_int vec);

	/*******************************************
    * @brief : ���ݹ����ȡ�������
    *******************************************/
	int get_out_vector(
		vector_str &v_out_str,
		vector_str v_str, 
		vector_int v_column,
		int direction,
		bool col_spill_flag);

	/*******************************************
    * @brief : ���ݹ����ָ��Ŀ¼��ȡ�ļ�����
    *******************************************/
	int get_filename_from_dir(
		string src_path, 
		string input_path,
		bool is_check);

	/*******************************************
    * @brief : ���ݹ����ָ��Ŀ¼��ȡ�ļ�����
    *******************************************/
	string join_path(
		string &dest_path, 
		string src_path, 
		string add_path);

	/*******************************************
    * @brief : �������ļ�
    *******************************************/
	int rename_file(string src_name, string dest_name);

	/*******************************************
    * @brief : ����һ��Ŀ¼�µĶ���ļ�
    *******************************************/
	int copy_file(
		string src_path,
		string dest_path);

	/*******************************************
    * @brief : ����һ��Ŀ¼�µĶ���ļ�
    *******************************************/
	int process_files(
		string file_path_name,
		string file_name,
		map_oc &m_oc);
	
	/*******************************************
    * @brief : ���������ļ�������ɸѡ
    *******************************************/
	void analysis_config(size_t thread_num);

	/*******************************************
    * @brief : �õ���Ҫ�������
    *******************************************/
	int get_column(vector_int &v_column, string column);
	
	/*******************************************
    * @brief : or����ɸѡ
    *******************************************/
	bool field_filter_or(vector_str v_str, string filter, bool &fil_spill_flag);
	
	/*******************************************
    * @brief : and����ɸѡ
    *******************************************/
	bool field_filter_and(vector_str v_str, string filter, bool &fil_spill_flag);
	
	/*******************************************
    * @brief : ���������ֽ�
    *******************************************/
	bool field_filter_data(vector_str v_str, string filter, bool &fil_spill_flag);

	/*******************************************
    * @brief : ���������Ƚ�
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
	// ȫ�����ù�������
	Config global_info;
	
	// �������ù��������б�
	vector<Config> v_filter;

	map_km m_filter;
	
	map_st m_inpath_to_tmp;
	
	CDeque_str file_deque;
	
};

/*******************************************
    * @brief    : �����̺߳����Ĳ���
*******************************************/
typedef struct
{
	FilterData *p_fd;
	size_t thread_num;
}Thread_arg;

/*******************************************
    * @brief    : ÿ�������Ӧ�Ĵ����߳�
*******************************************/
void *filter_thread(void *arg);



#endif //__DATA_FILTER_H__


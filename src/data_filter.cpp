//////////////////////////////////////////////////////////////////////////

#include "data_filter.h"
#include "syslog.h"



//////////////////////////////////////////////////////////////////////////

void FilterData::init_config(Config *data)
{
	data->copy = -1;
	data->split = "";
	data->need_head_tail = -1;
	data->direction = -1;
	data->column = "";
	data->filter = "";
}

FilterData::FilterData()
{
	init_config(&global_info);
}

FilterData::~FilterData()
{
	v_filter.clear();
}

void FilterData::get_base_info(TiXmlElement *pElement, Config &p_config)
{
	while(pElement)
	{
		if(!strcmp(pElement->Value(), "key"))
    	{
			Key k;
			if(pElement->Attribute("key_name"))
    		{
    			k.key_name = pElement->Attribute("key_name");
    		}
			else
			{
				k.key_name = "";
			}

			if(pElement->Attribute("key_site"))
    		{
    			k.key_site = atoi(pElement->Attribute("key_site"));
    		}
			else
			{
				k.key_site = -1;
			}

			p_config.v_key.push_back(k);
			
    	}
		else if(!strcmp(pElement->Value(), "input_path"))
    	{
    		Input in;
    		if(pElement->Attribute("path"))
    		{
    			in.input_path = pElement->Attribute("path");
    		}
			else
			{
				in.input_path = "";
			}

			if(pElement->Attribute("exp_name"))
    		{
    			in.exp_name = pElement->Attribute("exp_name");
    		}
			else
			{
				in.exp_name = "";
			}

			if(pElement->Attribute("bak_path"))
    		{
    			in.bak_path = pElement->Attribute("bak_path");
    		}
			else
			{
				in.bak_path = "";
			}
			p_config.v_input.push_back(in);
			
    	}
		else if(!strcmp(pElement->Value(), "output_path"))
    	{
    		Output out;
    		if(pElement->Attribute("path"))
    		{
    			out.output_path = pElement->Attribute("path");
    		}
			else
			{
				out.output_path = "";
			}
			
			if(pElement->Attribute("del_size0"))
    		{
    			out.del_size0 = atoi(pElement->Attribute("del_size0"));
    		}
			else
			{
				out.del_size0 = -1;
			}

			p_config.v_output.push_back(out);
			
    	}
		else if(!strcmp(pElement->Value(), "mode"))
		{
			if(pElement->Attribute("copy"))
    		{
    			p_config.copy = atoi(pElement->Attribute("copy"));
    		}
			else
			{
				p_config.copy = global_info.copy;
			}
			
			if(pElement->Attribute("need_head_tail"))
    		{
				p_config.need_head_tail = atoi(pElement->Attribute("need_head_tail"));
    		}
			else
			{
				p_config.need_head_tail = global_info.need_head_tail;
			}
			
			if(pElement->Attribute("split"))
    		{
    			p_config.split = pElement->Attribute("split");
    		}
			else
			{
				p_config.split = global_info.split;
			}
			
			if(pElement->Attribute("direction"))
    		{
    			p_config.direction = atoi(pElement->Attribute("direction"));
    		}
			else
			{
				p_config.direction = global_info.direction;
			}
		}
		else if(!strcmp(pElement->Value(), "field"))
		{
			if(pElement->Attribute("column"))
    		{
    			p_config.column = pElement->Attribute("column");
    		}
			else
			{
				p_config.column = global_info.column;
			}
			
			if(pElement->Attribute("filter"))
    		{
    			p_config.filter = pElement->Attribute("filter");
    		}
			else
			{
				p_config.filter = global_info.filter;
			}
		}
	
		pElement = pElement->NextSiblingElement();
	}
}


int FilterData::load_config_info(string str_file_name)
{
	TiXmlDocument doc(str_file_name.c_str());
	if(!doc.LoadFile())
	{
		LOG("Error-> Config file: %s load failed", str_file_name.c_str());
        return -1;
	}

	TiXmlElement *pRoot = doc.RootElement();
	TiXmlElement *pElement = pRoot->FirstChildElement();
    TiXmlElement *pSubElement;

	while(pElement)
    {
		//加载基础配置
    	if(!strcmp(pElement->Value(), "base")) 
        {
        	if(pElement->Attribute("max_thread_num"))
    		{
    			max_thread_num = atoi(pElement->Attribute("max_thread_num"));
    		}
        }
		
    	//加载全局配置
    	if(!strcmp(pElement->Value(), "global")) 
        {
        	pSubElement = pElement->FirstChildElement();
        	get_base_info(pSubElement, global_info);
        }

		//加载个体配置
        if(!strcmp(pElement->Value(), "file_list")) 
        {
        	pSubElement = pElement->FirstChildElement();
			while(pSubElement)
			{
				if(!strcmp(pSubElement->Value(), "one_filter"))
				{
					Config m_config;
					init_config(&m_config);
					get_base_info(pSubElement->FirstChildElement(), m_config);
					v_filter.push_back(m_config);
				}
				pSubElement = pSubElement->NextSiblingElement();
			}
        }
        pElement = pElement->NextSiblingElement();
    }

	if(check_config() != 0)
	{
		return -1;
	}
	get_filter_map();
	get_inpath_map();
	
	return 0;
}

int FilterData::check_config()
{
	if(v_filter.empty())
	{
		LOG("Error-> Config is empty: please check config file");
		return -1;
	}
	
	for(size_t site=0; site<v_filter.size(); site++)
	{
		if(v_filter[site].copy == -1)
		{
			v_filter[site].copy = global_info.copy;
		}

		if(v_filter[site].need_head_tail == -1)
		{
			v_filter[site].need_head_tail = global_info.need_head_tail;
		}

		if(!strcmp(v_filter[site].split.c_str(), ""))
		{
			v_filter[site].split = global_info.split;
		}

		if(v_filter[site].direction== -1)
		{
			v_filter[site].direction = global_info.direction;
		}

		if(!strcmp(v_filter[site].column.c_str(), ""))
		{
			v_filter[site].column = global_info.column;
		}

		if(!strcmp(v_filter[site].filter.c_str(), ""))
		{
			v_filter[site].filter = global_info.filter;
		}

		boost::trim_if(v_filter[site].column, boost::is_any_of(", "));
		boost::trim_if(v_filter[site].filter, boost::is_any_of(" |*"));

		if(v_filter[site].copy<0 || v_filter[site].copy>1)
		{
			LOG("Error-> Invalid copy: %d range(0,1)", v_filter[site].copy);
			return -1;
		}
		
		if(v_filter[site].need_head_tail<0 || v_filter[site].need_head_tail>3)
		{
			LOG("Error-> Invalid need_head_tail: %d range(0,3)", v_filter[site].need_head_tail);
			return -1;
		}

		if(v_filter[site].direction<0 || v_filter[site].direction>3)
		{
			LOG("Error-> Invalid direction: %d range(0,3)", v_filter[site].direction);
			return -1;
		}

		for(size_t i=0; i<v_filter[site].v_output.size(); i++)
		{
			if(!strcmp(v_filter[site].v_output[i].output_path.c_str(), ""))
			{
				v_filter[site].v_output[i].output_path = global_info.v_output[0].output_path;
			}

			if(-1 == v_filter[site].v_output[i].del_size0)
			{
				v_filter[site].v_output[i].del_size0 = global_info.v_output[0].del_size0;
			}
			
			boost::trim_right_if(v_filter[site].v_output[i].output_path, boost::is_any_of("/ "));
			
			if(v_filter[site].v_output[i].del_size0<0 || v_filter[site].v_output[i].del_size0>1)
			{
				LOG("Error-> Invalid direction: %d range(0,1)", v_filter[site].v_output[i].del_size0);
				return -1;
			}
		}


		for(size_t j=0; j<v_filter[site].v_input.size(); j++)
		{
			if(!strcmp(v_filter[site].v_input[j].input_path.c_str(), ""))
			{
				v_filter[site].v_input[j].input_path = global_info.v_input[0].input_path;
			}

			if(!strcmp(v_filter[site].v_input[j].exp_name.c_str(), ""))
			{
				v_filter[site].v_input[j].exp_name = global_info.v_input[0].exp_name;
			}

			if(!strcmp(v_filter[site].v_input[j].bak_path.c_str(), ""))
			{
				v_filter[site].v_input[j].bak_path = global_info.v_input[0].bak_path;
			}

			boost::trim_right_if(v_filter[site].v_input[j].input_path, boost::is_any_of("/ "));
			boost::trim_if(v_filter[site].v_input[j].exp_name, boost::is_any_of(" "));
			boost::trim_right_if(v_filter[site].v_input[j].bak_path, boost::is_any_of("/ "));
		}

		for(size_t j=0; j<v_filter[site].v_key.size(); j++)
		{
			if(!strcmp(v_filter[site].v_key[j].key_name.c_str(), ""))
			{
				v_filter[site].v_key[j].key_name = global_info.v_key[0].key_name;
			}

			if(-1 == v_filter[site].v_key[j].key_site)
			{
				v_filter[site].v_key[j].key_site = global_info.v_key[0].key_site;
			}

			boost::trim_if(v_filter[site].v_key[j].key_name, boost::is_any_of(" "));
			
			if(v_filter[site].v_key[j].key_site<0 || v_filter[site].v_key[j].key_site>2)
			{
				LOG("Error-> Invalid key_site: %d range(0,1)", v_filter[site].v_key[j].key_site);
				return -1;
			}

		}
	}

	return 0;
}

int FilterData::get_filter_map()
{
	for(size_t fil=0; fil<v_filter.size(); fil++)
	{
		for(size_t key=0; key<v_filter[fil].v_key.size(); key++)
		{
			map_km::iterator it;
			
			// 检查是否存在相同的key 和 key_site
			it = m_filter.find(v_filter[fil].v_key[key]);
		
			if(it != m_filter.end())
			{
				for(size_t in=0; in<v_filter[fil].v_input.size(); in++)
				{
					map_im::iterator it_im;
				
					// 检查是否已有相同的input和exp_name目录
					it_im = it->second.find(v_filter[fil].v_input[in]);
				
					if(it_im!=it->second.end())
					{
						for(size_t out=0; out<v_filter[fil].v_output.size(); out++)
						{
							it_im->second.insert(
								pair<Output, Config>(v_filter[fil].v_output[out], v_filter[fil]));
						}
					}
					else
					{	
						map_oc m_oc;
						for(size_t out=0; out<v_filter[fil].v_output.size(); out++)
						{
							m_oc.insert(pair<Output, Config>(v_filter[fil].v_output[out], v_filter[fil]));
						}
						it->second.insert(pair<Input, map_oc>(v_filter[fil].v_input[in], m_oc));
					}
				}
			}
			else
			{
				map_im m_im;
				map_oc m_oc;
				for(size_t in=0; in<v_filter[fil].v_input.size(); in++)
				{
					for(size_t out=0; out<v_filter[fil].v_output.size(); out++)
					{
						m_oc.insert(pair<Output, Config>(v_filter[fil].v_output[out], v_filter[fil]));
					}
					m_im.insert(pair<Input, map_oc>(v_filter[fil].v_input[in], m_oc));
				}
				m_filter.insert(pair<Key, map_im>(v_filter[fil].v_key[key], m_im));
			}

		}
	}

	return 0;
}

int FilterData::get_inpath_map()
{
	int count = 0;
	string input_tmp_dir = INPUT_TMP_DIR;
	
	for(size_t site=0; site<v_filter.size(); site++)
	{
		for(size_t in=0; in<v_filter[site].v_input.size(); in++)
		{
			// 创建 m_inpath_to_tmp map
			map_st::iterator it_tmp;
			it_tmp = m_inpath_to_tmp.find(v_filter[site].v_input[in].input_path);
			if(it_tmp == m_inpath_to_tmp.end())
			{
			    input_tmp_dir = INPUT_TMP_DIR;
				str_add_int(input_tmp_dir, count);
				Tmp_bak tmp;
				tmp.tmp_path = input_tmp_dir;
				tmp.bak_path = v_filter[site].v_input[in].bak_path;
				m_inpath_to_tmp.insert(make_pair(
					v_filter[site].v_input[in].input_path,tmp));
				count++;
			}
		}
	}
}

int FilterData::get_column(vector_int &v_column, string column)
{
	vector_str v_str;
	if(!strcmp(column.c_str(), ""))
	{
		return 0;
	}
	
	boost::split(v_str, column, boost::is_any_of(","), boost::token_compress_on);
	for(size_t i=0; i<v_str.size(); i++)
	{
		v_column.push_back(atoi(v_str[i].c_str()));
	}
	return 0;
}


bool FilterData::condition_analysis(string item, string filter)
{
	bool ret = false;
	size_t pos;
	string str_data;

	double item_d = 0.0;
	double filter_d = 0.0;
	
	boost::trim(item);
	boost::trim(filter);
	str_data = filter.substr(1);
	item_d = atof(item.c_str());
	filter_d = atof(str_data.c_str());

	switch(filter[0])
	{
		case '=':
			if(item_d == filter_d)
			{
				return true;
			}
			break;
		case '!':
			if(item_d != filter_d || strcmp(item.c_str(), str_data.c_str()))
			{
				return true;
			}
			break;
		case '<':
			if(item_d < filter_d)
			{
				return true;
			}
			break;
		case '>':
			if(item_d > filter_d)
			{
				return true;
			}
			break;
		case '$':
			pos = str_data.rfind("$");
			if(pos != string::npos)
			{	
				string str_data_sub;
				size_t pos_sub;
				str_data_sub = str_data.substr(0,pos);
				pos_sub = item.find(str_data_sub);
				if(pos_sub != string::npos)
				{
					return true;
				}
			}
			else if(boost::ends_with(item, str_data))
			{
				return true;
			}
			break;
		default:
			pos = filter.rfind("$");
			if(pos != string::npos)
			{	
				str_data = filter.substr(0,pos);
				if(boost::starts_with(item, str_data))
				{
					return true;
				}
			}
			else if(!strcmp(item.c_str(), filter.c_str()))
			{
				return true;
			}

	}

	return false;
	
}


bool FilterData::field_filter_data(vector_str v_str, string filter, bool &fil_spill_flag)
{
	vector_str v_filter_data;
	bool ret = false;
	int column = 0;
	
	boost::split(v_filter_data, filter, boost::is_any_of("[]()"), boost::token_compress_on);
	
	if(v_filter_data.size() < 3)
	{
		if(fil_spill_flag)
		{
			LOG("Error-> Filter criteria is incomplete: %s", filter.c_str());
			fil_spill_flag = false;
		}
		return ret;
	}
	
	if(strcmp(v_filter_data[0].c_str(), "col"))
	{
		if(fil_spill_flag)
		{
			LOG("Error-> Filter format is error: %s", filter.c_str());
			fil_spill_flag = false;
		}
		return ret;
	}
	
	column = atoi(v_filter_data[1].c_str());
	if(column<0 || column>=v_str.size())
	{
		if(fil_spill_flag)
		{
			LOG("Error-> Filter cloumn out of range: %s", filter.c_str());
			fil_spill_flag = false;
		}
		return ret;
	}
	
	ret = condition_analysis(v_str[column], v_filter_data[2]);
	
	return ret;
}

bool FilterData::field_filter_and(vector_str v_str, string filter, bool &fil_spill_flag)
{
	vector_str v_filter_and;
	bool ret = false;
	
	boost::split(v_filter_and, filter, boost::is_any_of("**"), boost::token_compress_on);
	
	for(size_t i=0; i<v_filter_and.size(); i++)
	{
		if(0 == i)
		{
			ret = field_filter_data(v_str, v_filter_and[i], fil_spill_flag);
		}
		else
		{
			ret = ret && field_filter_data(v_str, v_filter_and[i], fil_spill_flag);
		}
	}
	return ret;
}

bool FilterData::field_filter_or(vector_str v_str, string filter, bool &fil_spill_flag)
{
	vector_str v_filter_or;
	bool ret = false;

	if(!strcmp(filter.c_str(), ""))
	{
		return true;
	}
	
	boost::split(v_filter_or, filter, boost::is_any_of("||"), boost::token_compress_on);
	
	for(size_t i=0; i<v_filter_or.size(); i++)
	{
		ret = ret || field_filter_and(v_str, v_filter_or[i], fil_spill_flag);
	}
	
	return ret;
}

bool FilterData::is_in_vector(int num, vector_int vec)
{
	for(size_t i=0; i<vec.size(); i++)
	{
		if(num == vec[i])
		{
			return true;
		}
	}
	return false;
}

int FilterData::get_out_vector(
	vector_str &v_out_str,
	vector_str v_str, 
	vector_int v_column,
	int direction,
	bool col_spill_flag)
{
	int ret = 0;
		
	if(v_column.empty())
	{
		v_out_str = v_str;
	}
	else
	{
		if(0 == direction || 2 == direction)
		{	
			for(size_t i=0; i<v_column.size();i++)    
		    {    
		    	if(v_column[i]>=0 && v_column[i]<v_str.size())
				{
					v_out_str.push_back(v_str[v_column[i]]);
				}
				else if(col_spill_flag)
				{
					LOG("Warning-> Select cloumn out of range: %d", v_column[i]);
					ret = -1;
				}
		    }
		}
		else
		{
			for(size_t i=0; i<v_str.size();i++)    
		    {   
				if(!(is_in_vector(i, v_column)))
				{
					v_out_str.push_back(v_str[i]);
				}
		    }
		}
	}
	return ret;
}

int FilterData::get_filename_from_dir(
	string src_path, 
	string input_path,
	bool is_check)
{
	DIR *dirptr = NULL;  
	struct dirent *entry;
	string file_name;
	if((dirptr = opendir(input_path.c_str())) == NULL)  
	{  
		LOG("Error-> Open dir error: %s %s", input_path.c_str(), strerror(errno));
		return -1;
	}
	
	while (entry = readdir(dirptr))  
	{  
		file_name = entry->d_name;
		boost::trim_if(file_name, boost::is_any_of(" \n\r"));
		
		if(!strcmp(file_name.c_str(), ".") || !strcmp(file_name.c_str(), ".."))
		{
			continue;
		}
		if(boost::ends_with(file_name, ".tmp"))
		{
			continue;
		}
		
		struct stat buf;
		string full_name;
		join_path(full_name, input_path, file_name);
		if(lstat(full_name.c_str(), &buf) < 0) 
		{
			LOG("Error-> Get stat error: %s %s", full_name.c_str(), strerror(errno));
		}
		
		if(S_ISDIR(buf.st_mode)) 
		{
			get_filename_from_dir(src_path, full_name, is_check);
		}
		else if(S_ISREG(buf.st_mode)) 
		{
			if(is_check)
			{
				string deque_name;
				join_path(deque_name, src_path, file_name);
				file_deque.push_back(deque_name);
			}
			else
			{
				string tmp_path;
				string tmp_name;
				join_path(tmp_path, THREAD_TMP_DIR, m_inpath_to_tmp.find(src_path)->second.tmp_path);
				make_dir(tmp_path);
				join_path(tmp_name, tmp_path, file_name);
				rename_file(full_name, tmp_name);

				string deque_name;
				join_path(deque_name, src_path, file_name);
				file_deque.push_back(deque_name);
			}
		}
		
	}
	closedir(dirptr);
	  
	return 0;  
}

int FilterData::make_dir(string dir_path)
{
	size_t pos;
	string par_dir_path;
	if(0 != access(dir_path.c_str(), 0))
	{
		boost::trim_right_if(dir_path, boost::is_any_of("/ "));
		pos = dir_path.find_last_of("/");
		par_dir_path = dir_path.substr(0, pos);
		if(0 != make_dir(par_dir_path))
		{
			return -1;
		}
		if(0 != mkdir(dir_path.c_str(), 0777))
		{
			LOG("Error-> Mkdir error: %s %s", dir_path.c_str(), strerror(errno));
			return -1;
		}
	}
	return 0;
}

string FilterData::join_path(
	string &dest_path, 
	string src_path, 
	string add_path)
{
	dest_path = src_path;
	dest_path += "/";
	dest_path += add_path;
	return dest_path;
}

int FilterData::rename_file(string src_name, string dest_name)
{	
	size_t pos;
	string dir_path;

	if(0 != rename(src_name.c_str(), dest_name.c_str()))
	{
		if(0 != access(src_name.c_str(), 0))
		{
			LOG("Error-> dir or file is not exist: %s", src_name.c_str(), strerror(errno));
			return -1;
		}
		
		dir_path = dest_name.substr(0, dest_name.find_last_of("/"));
		make_dir(dir_path);
		
		if(0 != rename(src_name.c_str(), dest_name.c_str()))
		{
			LOG("Error-> move %s to %s error: %s", src_name.c_str(),dest_name.c_str(), strerror(errno));
			return -1;
		}
	}
	return 0;
}


int FilterData::copy_file(string src_name, string dest_name)
{
	ifstream fread;
	ofstream fout;
	string dir_path;

	fread.open(src_name.c_str(), ios_base::in|ios_base::binary);
	if(!fread)
	{
		LOG("Error-> Open file failed: %s", src_name.c_str());
		return -1;
	}

	fout.open(dest_name.c_str(), ios_base::out|ios_base::binary|ios_base::trunc);
	if(!fout)
	{
		dir_path = dest_name.substr(0, dest_name.find_last_of("/"));
		make_dir(dir_path);
		fout.open(dest_name.c_str(), ios_base::out|ios_base::binary|ios_base::trunc);
		if(!fout)
		{
			LOG("Error-> Open file failed: %s", dest_name.c_str());
			return -1;
		}
	}

	fout<<fread.rdbuf();

	fread.close();
	fout.close();
	
}


int FilterData::process_files(
	string input_name,
	string file_name,
	map_oc &m_oc)
{
	int ret = -1;
	map_oc::iterator it_oc;
	
	for(it_oc=m_oc.begin(); it_oc != m_oc.end(); it_oc++)
	{
		string out_path = it_oc->first.output_path;
		int del_size0 = it_oc->first.del_size0;
		Config *p_config = &(it_oc->second);

		bool fil_spill_flag = true;
		bool col_spill_flag = true;
		ifstream fread;
		ofstream fout;
		
		string str_read_line;
		string output_name;
		string temp_name;
		string backup_name;

		make_dir(out_path); // 判断输出路径是否存在，并创建

		join_path(output_name, out_path, file_name);
		join_path(backup_name, p_config->backup_path, file_name);
		temp_name = output_name;
		temp_name += ".tmp";

		if(p_config->copy) // copy为 1 时完全复制文件
		{
			copy_file(input_name, temp_name);
			rename_file(temp_name, output_name);
			continue;
		}

		vector_int v_column;
		get_column(v_column, p_config->column);

		fread.open(input_name.c_str());
		if(!fread)
		{
			LOG("Error-> Open file failed: %s", input_name.c_str());
			return -1;
		}

		fout.open(temp_name.c_str(), ios::trunc|ios::out);
		if(!fout)
		{
			LOG("Error-> Open file failed: %s", temp_name.c_str());
			return -1;
		}

		// need_head_tail为1,3时 保留第一行数据
		if(1==p_config->need_head_tail || 3==p_config->need_head_tail)
		{
			getline(fread, str_read_line);
			fout<< str_read_line <<endl;
		}
		// 循环处理文件里的每一行数据
		while(!fread.eof())
		{
			getline(fread, str_read_line);
			if(fread.eof())
			{
				if(2==p_config->need_head_tail || 3==p_config->need_head_tail)
				{
					fout<< str_read_line <<endl;
					continue;
				}
			}

			if(0 == str_read_line.size()) // 跳过空数据行
			{
				continue;
			}
			
			bool filter_flag;
			vector_str v_str;
			vector_str v_out_str;
			
			boost::split(v_str, str_read_line, boost::is_any_of( p_config->split.c_str()), boost::token_compress_on);

			// GET过滤条件真假
			filter_flag = field_filter_or(v_str, p_config->filter, fil_spill_flag);
			
			// direction为2,3时为反向抽取 filter_flag取反
			if(2 == p_config->direction || 3 == p_config->direction) 
			{
				filter_flag = !filter_flag;
			}
			
			if(filter_flag) // 判断过滤条件真假 真就写入文件
			{
				// 抽取需要输出的列
				ret = get_out_vector(v_out_str, v_str, v_column, p_config->direction, col_spill_flag);
				if(0 != ret)
				{
					col_spill_flag = false;
					LOG("Warning-> In tish file: %s", input_name.c_str());
				}
				fout<< boost::join(v_out_str, p_config->split) <<endl;
			}
		}
		
		if(fread.is_open())
		{
			fread.close();
		}
		if(fout.is_open())
		{
			fout.close();
		}

		struct stat fs;
		if(0 != stat(temp_name.c_str(),&fs)) // 获取文件状态
		{
			LOG("Error-> Get file stat error: %s %s", temp_name.c_str(), strerror(errno));
		}

		if(0 == fs.st_size && 1 == del_size0) // 判断文件大小，并判断文件为零时是否删除
		{
			if(0 != remove(temp_name.c_str())) // 为零且del_size0为真 删除该文件
			{
				LOG("Error-> rename error: %s %s", temp_name.c_str(), strerror(errno));
			}
		}
		else
		{
			rename_file(temp_name, output_name); // 将temp文件更名为输出文件
		}

	}

	return 0;
}

void FilterData::str_add_int(string &dest, int num)
{
	stringstream ss;
	string str;
	ss<<num;
	ss>>str;
	dest += str;
}

void FilterData::analysis_config(size_t thread_num)
{
	while(1)
	{
		while(0 != file_deque.size())
		{
			string path_and_name = file_deque.front();
			size_t pos = path_and_name.find_last_of("/");
			string path_name = path_and_name.substr(0, pos);
			string file_name = path_and_name.substr(pos+1);

			LOG("Log%d->  begin: %s", thread_num, path_and_name.c_str());

			string bak_path;
			string bak_name;
			bak_path = m_inpath_to_tmp.find(path_name)->second.bak_path;
			join_path(bak_name, bak_path, file_name);

			string tmp_path;
			string tmp_name;
			join_path(tmp_path, THREAD_TMP_DIR, m_inpath_to_tmp.find(path_name)->second.tmp_path);
			join_path(tmp_name, tmp_path, file_name);

			map_km::iterator it_km;
			for(it_km=m_filter.begin(); it_km != m_filter.end(); it_km++)
			{
				bool key_flag = false;
				string key_name;
				int key_site;
				key_name = it_km->first.key_name;
				key_site = it_km->first.key_site;

				boost::trim_if(file_name, boost::is_any_of(" \n\r"));
				string name = file_name.substr(0, file_name.find_last_of("."));
				
				if(0==key_site && boost::contains(file_name, key_name))
				{
					key_flag = true;
				}
				else if(1==key_site && boost::starts_with(file_name, key_name))
				{
					key_flag = true;
				}
				else if(2==key_site && boost::ends_with(name, key_name))
				{
					key_flag = true;
				}
				
				if(key_flag)
				{
					map_im::iterator it_im;

					for(it_im = it_km->second.begin(); it_im != it_km->second.end(); it_im++)
					{
						bool exp_flag = false;
						// 判断输入路径是否符合关键字的input_path
						if(!strcmp(it_im->first.input_path.c_str(), path_name.c_str()))
						{
							vector_str v_exp;
							boost::split(v_exp, it_im->first.exp_name, 
											boost::is_any_of(","), 
											boost::token_compress_on);
							for(size_t i=0; i<v_exp.size(); i++)
							{	
								// 判断文件后缀名是否符合关键字的后缀名
								if(boost::ends_with(file_name, v_exp[i]))
								{
									exp_flag = true;
									break;
								}
							}
						}
						if(exp_flag)
						{
							process_files(tmp_name, file_name, it_im->second);
						}
					}
					
				}
			}

			// 判断bak_path是否为空,不为空备份到bak_path
			if(strcmp(bak_path.c_str(), ""))
			{
				rename_file(tmp_name, bak_name);
			}

			LOG("Log%d-> finish: %s", thread_num, path_and_name.c_str());
		}

		sleep(1);
	}
}

void FilterData::scanning_file()
{
	map_st::iterator it_st;
	while(1)
	{
		for(it_st = m_inpath_to_tmp.begin(); it_st != m_inpath_to_tmp.end(); it_st++)
		{
			
			string tmp_path;
			string input_path = it_st->first;
			vector_str v_file_name;
			vector_str::iterator it_dir;
			LOG("Log-> start scanning file :%s",input_path.c_str());

			if(0 != get_filename_from_dir(input_path, input_path, false))
			{
				m_inpath_to_tmp.erase(it_st);
				it_st--;
				continue;
			}
			LOG("Log-> end scanning file :%s",input_path.c_str());
		}
		sleep(60);
	}
}

void FilterData::check_thread_temp()
{
	map_st::iterator it_st;
	for(it_st = m_inpath_to_tmp.begin(); it_st != m_inpath_to_tmp.end(); it_st++)
	{
		string input_path = it_st->first;
		string tmp_path;
		join_path(tmp_path, THREAD_TMP_DIR, it_st->second.tmp_path);
		get_filename_from_dir(input_path, tmp_path, true);
	}
}

int FilterData::create_thread_dir()
{
	string thread_tmp_dir = THREAD_TMP_DIR;
	string input_tmp;

	if(0 != make_dir(thread_tmp_dir))
	{
		LOG("Error-> Mkdir tmp_thread error: %s", thread_tmp_dir.c_str());
		return -1;
	}

	map_st::iterator it;
	for(it=m_inpath_to_tmp.begin(); it != m_inpath_to_tmp.end(); it++)
	{
		join_path(input_tmp, thread_tmp_dir, it->second.tmp_path);
		if(0 != make_dir(input_tmp))
		{
			LOG("Error-> Mkdir tmp_thread error: %s", input_tmp.c_str());
			return -1;
		}
	}

	return 0;
}

int FilterData::make_all_dir()
{
	vector<Config>::iterator it;
	for(it=v_filter.begin(); it<v_filter.end(); it++)
	{
		for(size_t i=0; i<it->v_output.size(); i++)
		{
			if(0 != make_dir(it->v_output[i].output_path.c_str()))
			{
				LOG("Error-> Mkdir error: %s", it->v_output[i].output_path.c_str());
				return -1;
			}
		}

		for(size_t i=0; i<it->v_input.size(); i++)
		{
			if(0 != make_dir(it->v_input[i].bak_path.c_str()))
			{
				LOG("Error-> Mkdir error: %s", it->v_input[i].bak_path.c_str());
				return -1;
			}
		}

	}
	return 0;
}

int FilterData::run()
{
	int ret = -1;

	if(0 != make_all_dir()) // 创建所有输出目录，避免子线程创建冲突
	{
		return -1;
	}
	
	if(0 != create_thread_dir()) //创建线程缓存目录
	{
		return -1;
	}
	
	Thread_arg thread_arg;
	vector<pthread_t> v_thread_id;
	for(int d_num=0; d_num < max_thread_num; d_num++) // 循环创建线程
	{
		thread_arg.p_fd = this;
		thread_arg.thread_num = d_num;
		
		pthread_t th_id;
		ret = pthread_create(&th_id, NULL, filter_thread, (void*)&thread_arg);
		if(0 != ret)
		{
			LOG("Error-> create thread error: %s", strerror(errno));
			continue;
		}
		v_thread_id.push_back(th_id);
		sleep(1);
	}
	
	// 检查线程缓存目录是否还有文件未处理，有则添加到线程处理队列中
	check_thread_temp();

	// 扫描输入目录
	scanning_file();

}

void *filter_thread(void *arg)
{
	pthread_detach(pthread_self());
	
	Thread_arg *p_th_arg;
	p_th_arg = (Thread_arg *)arg;

	FilterData *p_fd = p_th_arg->p_fd;
	size_t thread_num = p_th_arg->thread_num;

	p_fd->analysis_config(thread_num);
	
}





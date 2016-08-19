
#include "file_manage.h"
#include "syslog.h"


int FileManage::load_config_info(string file_name)
{
	TiXmlDocument doc(file_name.c_str());
	if(!doc.LoadFile())
	{
		LOG("Error-> Config file: %s load failed", file_name.c_str());
        return -1;
	}

	TiXmlElement *pRoot = doc.RootElement();
	TiXmlElement *pElement = pRoot->FirstChildElement();
    TiXmlElement *pSubElement;

	string path="";
	string temp="";
	int time=0;

	while(pElement)
    {
    	path="";
		time=0;
		
    	if(!strcmp(pElement->Value(), "manage")) 
        {
        	if(pElement->Attribute("path"))
    		{
    			path = pElement->Attribute("path");
    		}

			if(pElement->Attribute("time"))
    		{
    			temp = pElement->Attribute("time");
    		}

			string t_str;
			t_str = temp.substr(0, temp.size()-1);
			switch(temp[temp.size()-1])
			{
			case 'm':
			case 'M':
				time = TIME_M*(atoi(t_str.c_str()));
				break;
			case 'h':
			case 'H':
				time = TIME_H*(atoi(t_str.c_str()));
				break;
			case 'd':
			case 'D':
				time = TIME_D*(atoi(t_str.c_str()));
				break;
			default:
				LOG("Error-> config unit of time error: %s", temp.c_str());
				return -1;
			}
			
			boost::trim_right_if(path, boost::is_any_of("/ "));
			if(0 != path.size() && 0 != time)
			{
				m_manage.insert(make_pair(path, time));
			}
			else
			{
				LOG("Warning-> Invalid config path:%s time:%d", path.c_str(),temp.c_str() );
			}
        }

        pElement = pElement->NextSiblingElement();
    }

	if(m_manage.empty())
	{
		LOG("Warning-> config is null !");
		return -1;
	}

	return 0;
}


int FileManage::make_dir(string dir_path)
{
	size_t pos;
	string par_dir_path;
	if(0 == dir_path.size())
	{
		return 0;
	}
	if(0 != access(dir_path.c_str(), 0))
	{
		boost::trim_right_if(dir_path, boost::is_any_of("/ "));
		pos = dir_path.find_last_of("/");
		par_dir_path = dir_path.substr(0, pos);
		if(0 != pos)
		{
			if(0 != make_dir(par_dir_path))
			{
				return -1;
			}
		}
		if(0 != mkdir(dir_path.c_str(), 0777))
		{
			LOG("Error-> Mkdir error: %s %s", dir_path.c_str(), strerror(errno));
			return -1;
		}
	}
	return 0;
}

string FileManage::join_path(
	string &dest_path, 
	string src_path, 
	string add_path)
{
	dest_path = src_path;
	dest_path += "/";
	dest_path += add_path;
	return dest_path;
}

int FileManage::rename_file(string src_name, string dest_name)
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

int FileManage::remove_dir(string dir_path)
{
	DIR *dirptr = NULL;  
	struct dirent *entry;
	string file_name;
	if((dirptr = opendir(dir_path.c_str())) == NULL)	
	{  
		LOG("Error-> Open dir error: %s %s", dir_path.c_str(), strerror(errno));
		return -1;
	}
	while (entry = readdir(dirptr))  
	{
		file_name = entry->d_name;
		if(!strcmp(file_name.c_str(), ".") || !strcmp(file_name.c_str(), ".."))
		{
			continue;
		}
		string full_name;
		join_path(full_name, dir_path, file_name);
		
		struct stat s_stat;
		if(lstat(full_name.c_str(), &s_stat) < 0) 
		{
			LOG("Error-> Get stat error: %s %s", full_name.c_str(), strerror(errno));
			continue;
		}

		if(S_ISREG(s_stat.st_mode)) 
		{
			if(0 != remove(full_name.c_str())) 
			{
				LOG("Error-> rename error: %s %s", full_name.c_str(), strerror(errno));
			}
		}
		else if(S_ISDIR(s_stat.st_mode)) 
		{
			remove_dir(full_name);
		}
	}

	if(0 != remove(dir_path.c_str())) 
	{
		LOG("Error-> rename error: %s %s", dir_path.c_str(), strerror(errno));
	}

	return 0;
}

int FileManage::get_dir_from_time(string &dir, time_t tt)
{
	struct tm *t;
	t=localtime(&tt);
	char buf[16]={0};
	
	sprintf(buf, "%4d%02d%02d%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour);
	dir = buf;
	return 0;
}

int FileManage::get_dir_from_name(string &dir, string file_name)
{
	string tmp;
	size_t pos;
	pos = file_name.find_last_of("_");
	tmp = file_name.substr(0, pos);
	pos = tmp.find_last_of("_");
	tmp = tmp.substr(pos+1);
	if(tmp.size() < 10)
	{
		LOG("Error-> file name time format error: %s",file_name.c_str());
		return -1;
	}
	dir = tmp.substr(0, 10);
	return 0;
}


int FileManage::back_and_check(string path, int i_time)
{
	DIR *dirptr = NULL;  
	struct dirent *entry;
	string file_name;
	if((dirptr = opendir(path.c_str())) == NULL)	
	{  
		LOG("Warning-> Open dir fault: %s %s", path.c_str(), strerror(errno));
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
		
		struct stat s_stat;
		string full_name;
		join_path(full_name, path, file_name);
		
		if(lstat(full_name.c_str(), &s_stat) < 0) 
		{
			LOG("Error-> Get stat error: %s %s", full_name.c_str(), strerror(errno));
			continue;
		}

		if(S_ISREG(s_stat.st_mode)) 
		{
			string new_dir="";
			string new_path;
			string new_name;
		//	get_dir_from_time(new_dir, s_stat.st_mtime);
			if(0 != get_dir_from_name(new_dir, file_name))
			{
				continue;
			}
			join_path(new_path, path, new_dir);
			join_path(new_name, new_path, file_name);
			make_dir(new_path);
			rename_file(full_name, new_name);
		}
		else if(S_ISDIR(s_stat.st_mode)) 
		{
			time_t tt;
			time(&tt);
			if(tt > s_stat.st_mtime+i_time)
			{
				remove_dir(full_name);
			/*	char cmd_rm[256]={0};
				sprintf(cmd_rm, "rm -rf %s", full_name.c_str());
				system(cmd_rm);
				LOG("Log-> remove dir: %s", full_name.c_str()); 
			*/
			}
		}
		
	}
	closedir(dirptr);
	  
	return 0;  
}



int FileManage::manage_dir()
{
	map_si::iterator it;
	while(1)
	{
		for(it = m_manage.begin(); it != m_manage.end(); it++)
		{
			back_and_check(it->first, it->second);
		}

		sleep(5);
	}
}

int FileManage::run()
{
	int ret;

	if(0 != load_config_info("manage_config.xml"))
	{
		return -1;
	}
	
	pthread_t th_id;
	ret = pthread_create(&th_id, NULL, manage_thread, (void*)this);
	if(0 != ret)
	{
		LOG("Error-> create thread error: %s", strerror(ret));
		return -1;
	}

	return 0;
}


void *manage_thread(void* arg)
{
	pthread_detach(pthread_self());

	prctl(PR_SET_NAME, "manage_thread");
	FileManage *p_fm = (FileManage *)arg;
	p_fm->manage_dir();
}




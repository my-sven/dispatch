
#include "syslog.h"

extern SysLog g_log;

SysLog::SysLog()
{
	log_path_name = "./default_sys.log";
	log_path = log_path_name.substr(0, log_path_name.find_last_of("/"));
	log_name = log_path_name.substr(log_path_name.find_last_of("/")+1);
	log_time = KEEP_TIME_H;
	log_size = MAX_SIZE_M;
	open_log_file(log_path_name);
}

SysLog::SysLog(string log_file_name, int keep_time_h, int max_size_m)
{
	log_path_name = log_file_name;
	log_path = log_path_name.substr(0, log_path_name.find_last_of("/"));
	log_name = log_path_name.substr(log_path_name.find_last_of("/")+1);
	log_time = keep_time_h*(KEEP_TIME_H);
	log_size = max_size_m*(MAX_SIZE_M);
	open_log_file(log_path_name);
}

SysLog::~SysLog()
{
	fwrite.close();
}

int SysLog::make_dir(string dir_path)
{
	size_t pos;
	string par_dir_path;
	if(0 != access(dir_path.c_str(), 0))
	{
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

int SysLog::open_log_file(string log_file_name)
{
	int ret;
	struct stat fs;
	if(access(log_file_name.c_str(), 0))
	{
		string par_dir_path;
		par_dir_path = log_file_name.substr(0, log_file_name.find_last_of("/"));
		make_dir(par_dir_path);
	}
	
	fwrite.open(log_file_name.c_str(), ios::app|ios::out);
	if(!fwrite)
	{
		cerr<<"Error-> Open file failed: "<< log_file_name.c_str() <<endl;
		return -1;
	}
	
}


int SysLog::get_filename_from_dir(
	map_si &m_log_file, 
	string input_path)
{
	DIR *dirptr = NULL;  
	struct dirent *entry;
	string file_name;
	if((dirptr = opendir(input_path.c_str())) == NULL)  
	{  
		cerr<<"Error-> Open dir error: "<<input_path<<" "<<strerror(errno)<<endl;
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
		
		m_log_file.insert(pair<string, int>(file_name,1));
	}
	closedir(dirptr);
	  
	return 0;  
}


string SysLog::join_path(
	string &dest_path, 
	string src_path, 
	string add_path)
{
	dest_path = src_path;
	dest_path += "/";
	dest_path += add_path;
	return dest_path;
}


void SysLog::get_new_name(string &dest_name, string src_name, int num)
{
 	string tmp_name = src_name.substr(0, src_name.find_last_of('.'));
	stringstream ss;
	string str;
	ss<<num;
	ss>>str;
	if(num<10 && num>0)
	{
		tmp_name += "00";
	}
	else if(num<100 && num>=10)
	{
		tmp_name += "0";
	}
	tmp_name += str;
	dest_name = tmp_name + ".log";
}

int SysLog::rename_file(string src_name, string dest_name)
{	
	size_t pos;
	string par_dir_path;

	if(0 != rename(src_name.c_str(), dest_name.c_str()))
	{
		if(0 != access(src_name.c_str(), 0))
		{
			LOG("Error-> file is not exist: %s", src_name.c_str(), strerror(errno));
			return -1;
		}
		pos = dest_name.find_last_of("/");
		par_dir_path = dest_name.substr(0, pos);
		make_dir(par_dir_path);
		if(0 != rename(src_name.c_str(), dest_name.c_str()))
		{
			LOG("Error-> move %s error: %s", src_name.c_str(), strerror(errno));
			return -1;
		}
	}
	return 0;
}


void SysLog::check_log_state()
{	
	int ret;
	struct stat f_stat;
	while(1)
	{
		ret = stat(log_path_name.c_str(),&f_stat);
		if(ret == 0)
		{
			if(f_stat.st_size > log_size)
			{
				map_si m_log_file;
				get_filename_from_dir(m_log_file, log_path);
				int size = m_log_file.size();
				map_si::reverse_iterator it_lf;
				for(it_lf = m_log_file.rbegin(); it_lf != m_log_file.rend(); it_lf++)
				{
					if (0 == log_name.compare(it_lf->first))
					{
						string new_name;
						get_new_name(new_name, log_name, 1);
						string tmp_path_name;
						join_path(tmp_path_name, log_path,new_name);
						mutex.Lock();
						fwrite.close();
						rename_file(log_path_name, tmp_path_name);
						open_log_file(log_path_name);
						mutex.Unlock();
					}
					else
					{
						string new_name;
						get_new_name(new_name, log_name, size);
						string old_path_name;
						join_path(old_path_name, log_path,it_lf->first);
						string new_path_name;
						join_path(new_path_name, log_path,new_name);
						
						struct stat tmp_stat;
						ret = stat(old_path_name.c_str(),&tmp_stat);
						if(0 != ret)
						{
							size--;
							continue;
						}
						time_t timep;
						time(&timep);
						
						if(ret == 0 && (timep - tmp_stat.st_mtime) > log_time)
						{
							remove(old_path_name.c_str());
							size--;
						}
						else
						{
							rename_file(old_path_name, new_path_name);
							size--;
						}
					}
				}
			}
		}
		else
		{
			LOG("Error-> get %s stat error: %s", log_path_name.c_str(), strerror(errno));
		}
		sleep(CHECK_TIME_M);
	}
}

void SysLog::run()
{
	int ret;
	pthread_t th_id;
	ret = pthread_create(&th_id, NULL, log_thread, (void*)this);
	if(0 != ret)
	{
		LOG("Error-> create thread error: %s", strerror(errno));
	}
}

void *log_thread(void* arg)
{
	pthread_detach(pthread_self());
	
	SysLog *p_log = (SysLog *)arg;
	p_log->check_log_state();
}

void log_info(const char * func, string fmt, ...)
{
	int len = 0;
	char buffer[512]={0};

	time_t t = time(0); 
	char now_time[64]; 
	strftime(now_time, sizeof(now_time), "%Y/%m/%d %X",localtime(&t)); 
	
	len = sprintf(buffer,"[%s][%s] ", now_time, func);
	
	va_list args;
	va_start(args, fmt);
	vsprintf(buffer+len, fmt.c_str(), args);
	va_end(args);

//	cerr<<buffer<<endl;
	g_log.mutex.Lock();
	g_log.fwrite<<buffer<<endl;
	g_log.mutex.Unlock();
}



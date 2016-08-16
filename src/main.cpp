
#include "data_filter.h"
#include "syslog.h"

// log�ļ���./log/sys.log���ļ�����ʱ��168Сʱ��������־�ļ���С5M
SysLog g_log("./log/sys.log", 168, 10);

int main(int argc, char *argv[])
{
	g_log.run(); // ����log�Զ����
	
	string str_start("--------------------------start--------------------------");
	LOG(str_start.c_str());
	
	int ret;
	FilterData fd;
	string config_path;
	if(argc > 1)
	{
		config_path = argv[1];
	}
	else
	{
		config_path = "filter_config.xml" ;
	}
	ret = fd.load_config_info(config_path);
	if(ret)
	{
		LOG("Error-> Load config file is failed: %s",config_path.c_str());
		cout<<"Error-> Load config file is failed: "<< config_path <<endl;
		return -1;
	}
	
	ret = fd.run();
	if(ret)
	{
		LOG("Error-> Run error: %s",config_path.c_str());
		cout<<"Error-> Run error: "<< config_path <<endl;
		return -1;
	}

	string str_finish("--------------------------finish--------------------------");
	LOG(str_finish.c_str());
	
	return ret;
}


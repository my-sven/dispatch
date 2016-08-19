
#include "data_filter.h"
#include "file_manage.h"
#include "syslog.h"

// log�ļ���./log/sys.log���ļ�����ʱ��168Сʱ��������־�ļ���С5M
SysLog g_log("./log/sys.log", 168, 10);

int main(int argc, char *argv[])
{
	int ret;
	g_log.run(); // ����log�Զ����
	
	string str_start("--------------------------start--------------------------");
	LOG(str_start.c_str());

	FileManage fm;
	ret = fm.run();
	if(ret)
	{
		LOG("Error-> start file manage failed " );
		cout<<"Error-> start file manage failed "<<endl;
		return -1;
	}
	
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

	
	ret = fd.run(config_path);
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


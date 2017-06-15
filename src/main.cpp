
#include "data_filter.h"
#include "file_manage.h"
#include "syslog.h"

Logger getLogger()
{
    if (CBaseLogger::p_instance == NULL)
    {
        CBaseLogger::p_instance = new CBaseLogger();
    }
    return CBaseLogger::p_instance->m_mapLogger["root"];
}

int main(int argc, char *argv[])
{
    int ret;
    
    LOG4CPLUS_DEBUG(getLogger(),"--------------------------start--------------------------");

    FileManage fm;
    ret = fm.run();
    if(ret)
    {
        LOG4CPLUS_DEBUG(getLogger(), "Error-> start file manage failed " );
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
        LOG4CPLUS_DEBUG(getLogger(),"Error-> Run error: %s"<<config_path);
        return -1;
    }

    LOG4CPLUS_DEBUG(getLogger(),"--------------------------end--------------------------");
    
    return ret;
}


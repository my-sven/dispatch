/******************************************************************************
* 记录程序运行的日志信息
*******************************************************************************/

#ifndef __SYSLOG_H__
#define __SYSLOG_H__

#include <map>
#include <string>
#include<log4cplus/logger.h>
#include<log4cplus/fileappender.h>
#include <log4cplus/layout.h>

using namespace log4cplus;
using namespace std;

#define FILE_SIZE (5*1024*1024)
#define FILE_NUM 5
#define LOG_DIR "./log/"

typedef map<string, Logger> LOGGERMAP;
typedef LOGGERMAP::iterator LOGGERITOR;


class CBaseLogger
{
public:
    CBaseLogger();
    CBaseLogger(string logName, int logLevel=0, int fileSize=FILE_SIZE, int fileNum=FILE_NUM);
    virtual ~CBaseLogger();
    
public:
    void addLogger(string logName, int logLevel=0, int fileSize=FILE_SIZE, int fileNum=FILE_NUM);

public:
    LOGGERMAP m_mapLogger;
    static CBaseLogger *p_instance;
};


#endif //__SYSLOG_H__


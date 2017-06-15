
#include "syslog.h"

CBaseLogger * CBaseLogger::p_instance=NULL;

CBaseLogger::CBaseLogger()
{
    Logger root =  Logger::getRoot();

    string  pattern = "%D{%Y/%m/%d %X.%q} [%p] %m [%l]%n";
    auto_ptr<Layout> p_layout(new  PatternLayout(pattern));
    
    string fileName = LOG_DIR + root.getName() + ".log";
    SharedAppenderPtr  p_append(new  RollingFileAppender(fileName.c_str(), FILE_SIZE, FILE_NUM));
    p_append->setLayout(p_layout);
    
    root.addAppender(p_append);
    m_mapLogger.insert(make_pair(root.getName().c_str(), root));
}

CBaseLogger::~CBaseLogger()
{
    
}

CBaseLogger::CBaseLogger(string logName, int logLevel, int fileSize, int fileNum)
{
    Logger root =  Logger::getRoot();

    string  pattern = "%D{%Y/%m/%d %X.%q} [%p] %m [%l]%n";
    auto_ptr<Layout> p_layout(new  PatternLayout(pattern));
    
    string fileName = LOG_DIR + logName + ".log";
    SharedAppenderPtr  p_append(new  RollingFileAppender(fileName.c_str(), fileSize, fileNum));
    p_append->setLayout(p_layout);
    
    root.addAppender(p_append);
    root.setLogLevel(logLevel);
    m_mapLogger.insert(make_pair(root.getName().c_str(), root));
}

void CBaseLogger::addLogger(string logName, int logLevel, int fileSize, int fileNum)
{
    Logger logger =  Logger::getInstance(logName.c_str());
    
    string  pattern = "%D{%Y/%m/%d %X.%q} [%p] %m [%l]%n";
    auto_ptr<Layout> p_layout(new  PatternLayout(pattern));
    
    string fileName = LOG_DIR + logName + ".log";
    SharedAppenderPtr  p_append(new  RollingFileAppender(fileName.c_str(), fileSize, fileNum));
    p_append->setLayout(p_layout);
    
    logger.addAppender(p_append);
    logger.setLogLevel(logLevel);
    m_mapLogger.insert(make_pair(logger.getName().c_str(), logger));
}



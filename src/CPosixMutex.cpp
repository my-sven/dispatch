#include <assert.h>
#include "CPosixMutex.h"

//##ModelId=3F94A19B01E4
CPosixMutex::CPosixMutex()
{
    Init();
}

//##ModelId=3F94A1AE000A
CPosixMutex::~CPosixMutex()
{
    Done();
}

//The implement for unix
#ifndef WIN32
bool CPosixMutex::Lock()
{
    int result=pthread_mutex_lock(&m_Mutex);assert(result==0);
    return result==0;
}

bool CPosixMutex::Unlock()
{
    int result=pthread_mutex_unlock(&m_Mutex); assert(result==0);
    return result==0;
}

bool CPosixMutex::Init()
{
    int result=pthread_mutex_init(&m_Mutex,0);  assert(result==0);
    return result==0;
}

bool CPosixMutex::Done()
{
    int result=pthread_mutex_destroy(&m_Mutex); assert(result==0);
    return result==0;
}
#else
//The implement for windows
bool CPosixMutex::Lock()
{
    return ::WaitForSingleObject(m_Mutex, INFINITE) == WAIT_OBJECT_0;
}

bool CPosixMutex::Unlock()
{
    return ::ReleaseMutex(m_Mutex);
}

bool CPosixMutex::Init()
{
    m_Mutex = ::CreateMutex(NULL, false, NULL);
    return (m_Mutex != NULL);
}

bool CPosixMutex::Done()
{
    if(m_Mutex != NULL)
    {
        ::CloseHandle(m_Mutex);
        m_Mutex = NULL;
    }
    return true;
}
#endif

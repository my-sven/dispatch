#ifndef CPOSIXMUTEX_H_HEADER_INCLUDED_C054D248
#define CPOSIXMUTEX_H_HEADER_INCLUDED_C054D248
#ifndef WIN32
#include <pthread.h>
#else
#include <windows.h>
#endif

//##ModelId=3F8F42CA0089
//##Documentation
//## ��װposix��׼�Ļ���������
class CPosixMutex
{
  public:
    //##ModelId=3F94A151004D
    //##Documentation
    //## ����ʹ��Ȩ
    //## ����:
    //##        pthread_mutex_lock(&m_mutex);
    bool Lock();

    //##ModelId=3F94A1670199
    //##Documentation
    //## �黹ʹ��Ȩ
    //## ����:
    //##        pthread_mutex_unlock(&m_mutex);
    bool Unlock();

    //##ModelId=3F94A19B01E4
    //##Documentation
    //## ���캯��
    //##        init();
    CPosixMutex();

    //##ModelId=3F94A1AE000A
    //##Documentation
    //## ��������
    //## ����:
    //##      Done();
    ~CPosixMutex();
    //##ModelId=3F94A17F0233
    //##Documentation
    //## ��ʼ��mutex,����ϵͳ��Դ
    //## ����:
    //##      pthread_mutex_init(&m_mutex,0);
    bool Init();
    //##ModelId=3F94A1BA0148
    //##Documentation
    //## �ͷ�ϵͳ��Դ
    //## ����:
    //##           pthread_mutex_destroy(&m_mutex);
    bool Done();

  protected:
#ifndef WIN32
    //##ModelId=3FA859D5036E
    pthread_mutex_t m_Mutex;
#else
    HANDLE m_Mutex;
#endif

};

//added by luojq
//##ModelId=40F1FAC602E9
template<typename locktype>
class auto_lock_t
{
public:
    //##ModelId=40F1FAC70394
    auto_lock_t(locktype &lock) : m_lock(lock)
    { m_lock.Lock(); }

    //##ModelId=40F1FAC703B2
    ~auto_lock_t()
    { m_lock.Unlock(); }
private:
    //##ModelId=40F1FAC7038B
    locktype & m_lock;
};

//##ModelId=40F1FAC602CB
typedef auto_lock_t<CPosixMutex> auto_lock;

#endif /* CPOSIXMUTEX_H_HEADER_INCLUDED_C054D248 */

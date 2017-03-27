/*********************************************************
* 文 件 名: brd_double_deque.h
* 功    能: 定义切换双队列
* 作    者: 何立平
* 备    注: 主要功能有 队列入栈, 队列出栈, 批量获取队列数据
*           双队列切换. 双队列在内部对临界资源作了管理, 应用
*           层数据不用考虑锁问题.
* 创建时间: 2011/1/25  
* 修改历史
*     时间       作者             原因
* 
**********************************************************/
#ifndef __BRD_DOUBLE_LIST_HH__
#define __BRD_DOUBLE_LIST_HH__
#include "CPosixMutex.h"
#include <stdint.h>
#include <list>
#include <deque>
using namespace std;

// 队列类型
enum enDEQUETYPE
{
    DEQUEZERO=0,  //Zero队列
    DEQUEONE=1,   //One队列
};
typedef enDEQUETYPE DEQUETYPE;

//class CMsg;
template<typename T>
class CDeque
{
public:
    CDeque()
       : m_dqCntZero(0)
    {
        lstsize = 0;                    // add by dlq
    }
    
    /*********************************************************
    * 功    能: 队列入栈,插入队列头
    * 输入参数: 
    *           pT  T类型指针
    * 输出参数: 
    *           无
    * 返 回 值: 
    *           无
    * 作    者: 何立平
    * 备    注: 
    * 创建时间: 2011/1/25  
    * 修改历史
    *     时间       作者             原因
    * 
    **********************************************************/
    inline void push_front(T pT)
    {
        m_lckZero.Lock();
        m_dqZero.push_front(pT);
        m_dqCntZero++;
        m_lckZero.Unlock();
    };
    
    /*********************************************************
    * 功    能: 队列入栈,插入队列尾
    * 输入参数: 
    *           pT  T类型指针
    *          bbdelay_push: 指示是否延时投递，设置延时投递可以降低抢锁的几率
    *            只能局限于一个线程存放，其他线程取的情况
    * 输出参数: 
    *           无
    * 返 回 值: 
    *           无
    * 作    者: 何立平
    * 备    注: 
    * 创建时间: 2011/1/25  
    * 修改历史
    *     时间       作者             原因
    * 
    **********************************************************/
    inline void push_back(T pT)
    {
        m_lckZero.Lock();
        m_dqZero.push_back(pT);
        m_dqCntZero++;
        m_lckZero.Unlock();
    };
    
    /*********************************************************
    * 功    能: 队列入栈,插入队列尾
    * 输入参数: 
    *           pT  T类型指针
    *          bbdelay_push: 指示是否延时投递，设置延时投递可以降低抢锁的几率
    *            只能局限于一个线程存放，其他线程取的情况
    * 输出参数: 
    *           无
    * 返 回 值: 
    *           无
    * 作    者: 何立平
    * 备    注: 
    * 创建时间: 2011/1/25  
    * 修改历史
    *     时间       作者             原因
    * 
    **********************************************************/
    inline void push_back(T pT, bool bdelay_push)
    {
        if(bdelay_push)
        {
            lmsgs.push_back(pT);
            if(++lstsize < 5000)
            {
                return;
            }
        }
        
        m_lckZero.Lock();
        if(bdelay_push)
        {
            typename deque<T>::iterator it;
            for(it = lmsgs.begin(); it!=lmsgs.end(); it++)
            {
                pT = *it;
                m_dqZero.push_back(pT);
                m_dqCntZero++;
            }
            lstsize = 0;
            lmsgs.clear();
        }
        else
        {
            m_dqZero.push_back(pT);
            m_dqCntZero++;
        }
        m_lckZero.Unlock();
    };
    
    /*********************************************************
    * 功    能: 队列出栈,从队列头取出数据
    * 输入参数: 
    *           无
    * 输出参数: 
    *           无
    * 返 回 值: 
    *           T*  指针
    * 作    者: 何立平
    * 备    注: 内部已经实现队列空间释放, 不用再释放
    * 创建时间: 2011/1/25  
    * 修改历史
    *     时间       作者             原因
    * 
    **********************************************************/
    inline T front(deque<T> *plist )
    {
        register int size;
        T pT = 0;
        m_lckZero.Lock();
        size = m_dqZero.size();
        if(size)
        {
            m_dqCntZero--;
            pT = m_dqZero.front();
            m_dqZero.pop_front();
            if(plist)
            {
                size--;
                if(size > 5000)
                {
                    size = 5000;
                }
                
                while(--size >0)
                {
                    m_dqCntZero--;
                    plist->push_back(pT);
                    pT = m_dqZero.front();
                    m_dqZero.pop_front();
                }
                plist->push_back(pT);
            }
        }
        else
        {//队列为空  切换队列
            m_dqCntZero = 0;//One 队列大小清
        }
        //m_uiCntOne = m_dqOne.size();
        m_lckZero.Unlock();
        
        return pT;
    };

    /*********************************************************
    * 功    能: 队列出栈,从队列头取出数据
    * 输入参数: 
    *           无
    * 输出参数: 
    *           无
    * 返 回 值: 
    *           T*  指针
    * 作    者: 何立平
    * 备    注: 内部已经实现队列空间释放, 不用再释放
    * 创建时间: 2011/1/25  
    * 修改历史
    *     时间       作者             原因
    * 
    **********************************************************/
    inline T front()
    {
        T pT;
        m_lckZero.Lock();
        if(m_dqZero.size())
        {
            m_dqCntZero--;
            pT = m_dqZero.front();
            m_dqZero.pop_front();
        }
        else 
        {
            m_dqCntZero=0;
        }
        m_lckZero.Unlock();
        
        return pT;
    };

    /*********************************************************
    * 功    能: 队列大小
    * 输入参数: 
    *           无
    * 输出参数: 
    *           无
    * 返 回 值: 
    *           uint64_t  队列中T*的个数
    * 作    者: 何立平
    * 备    注: 应用不用考虑锁问题
    * 创建时间: 2011/1/25  
    * 修改历史
    *     时间       作者             原因
    * 
    **********************************************************/
    inline uint64_t size()
    {
        return m_dqCntZero;
    }
    
    /*********************************************************
    * 功    能: 批量获取队列数据
    * 输入参数: 
    *           deque<T*>   T*类型缓存队列
    *           uint32_t    获取队列个数
    * 输出参数: 
    *           deque<T*>   T*类型缓存队列
    * 返 回 值: 
    *           uint32_t    成功从队列中获取T*的个数
    * 作    者: 何立平
    * 备    注: 如果队列中的个数不足uiCnt的大小,则切换获取队列
    * 创建时间: 2011/1/25  
    * 修改历史
    *     时间       作者             原因
    * 
    **********************************************************/
    inline uint32_t GetAllBuff(deque<T>& ls, const uint32_t uiCnt = 100)
    {
        if (m_dqCntZero == 0)
        {
            return 0;
        }
        
        m_lckZero.Lock();
        m_dqCntZero = m_dqZero.size();
        uint32_t uiNum = m_dqCntZero<uiCnt?m_dqCntZero:uiCnt;
        for(int i=0; i<uiNum; i++)
        {
            m_dqCntZero--;
            ls.push_back(m_dqZero.front());
            m_dqZero.pop_front();
        }
        m_lckZero.Unlock();

        return ls.size();
    }
    
private:
    deque<T>        m_dqZero;         //Zero 队列
    CPosixMutex     m_lckZero;        //Zero 队列互斥锁
    uint64_t        m_dqCntZero;      //Zero 队列大小
    int             lstsize;          //尺寸
    deque<T>        lmsgs;            //add by dlq
};


#endif //#ifndef __BRD_DOUBLE_LIST_HH__

/*********************************************************
* �� �� ��: brd_double_deque.h
* ��    ��: �����л�˫����
* ��    ��: ����ƽ
* ��    ע: ��Ҫ������ ������ջ, ���г�ջ, ������ȡ��������
*           ˫�����л�. ˫�������ڲ����ٽ���Դ���˹���, Ӧ��
*           �����ݲ��ÿ���������.
* ����ʱ��: 2011/1/25  
* �޸���ʷ
*     ʱ��       ����             ԭ��
* 
**********************************************************/
#ifndef __BRD_DOUBLE_LIST_HH__
#define __BRD_DOUBLE_LIST_HH__
#include "CPosixMutex.h"
#include <stdint.h>
#include <list>
#include <deque>
using namespace std;

// ��������
enum enDEQUETYPE
{
    DEQUEZERO=0,  //Zero����
    DEQUEONE=1,   //One����
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
    * ��    ��: ������ջ,�������ͷ
    * �������: 
    *           pT  T����ָ��
    * �������: 
    *           ��
    * �� �� ֵ: 
    *           ��
    * ��    ��: ����ƽ
    * ��    ע: 
    * ����ʱ��: 2011/1/25  
    * �޸���ʷ
    *     ʱ��       ����             ԭ��
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
    * ��    ��: ������ջ,�������β
    * �������: 
    *           pT  T����ָ��
    *          bbdelay_push: ָʾ�Ƿ���ʱͶ�ݣ�������ʱͶ�ݿ��Խ��������ļ���
    *            ֻ�ܾ�����һ���̴߳�ţ������߳�ȡ�����
    * �������: 
    *           ��
    * �� �� ֵ: 
    *           ��
    * ��    ��: ����ƽ
    * ��    ע: 
    * ����ʱ��: 2011/1/25  
    * �޸���ʷ
    *     ʱ��       ����             ԭ��
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
    * ��    ��: ������ջ,�������β
    * �������: 
    *           pT  T����ָ��
    *          bbdelay_push: ָʾ�Ƿ���ʱͶ�ݣ�������ʱͶ�ݿ��Խ��������ļ���
    *            ֻ�ܾ�����һ���̴߳�ţ������߳�ȡ�����
    * �������: 
    *           ��
    * �� �� ֵ: 
    *           ��
    * ��    ��: ����ƽ
    * ��    ע: 
    * ����ʱ��: 2011/1/25  
    * �޸���ʷ
    *     ʱ��       ����             ԭ��
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
    * ��    ��: ���г�ջ,�Ӷ���ͷȡ������
    * �������: 
    *           ��
    * �������: 
    *           ��
    * �� �� ֵ: 
    *           T*  ָ��
    * ��    ��: ����ƽ
    * ��    ע: �ڲ��Ѿ�ʵ�ֶ��пռ��ͷ�, �������ͷ�
    * ����ʱ��: 2011/1/25  
    * �޸���ʷ
    *     ʱ��       ����             ԭ��
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
        {//����Ϊ��  �л�����
            m_dqCntZero = 0;//One ���д�С��
        }
        //m_uiCntOne = m_dqOne.size();
        m_lckZero.Unlock();
        
        return pT;
    };

    /*********************************************************
    * ��    ��: ���г�ջ,�Ӷ���ͷȡ������
    * �������: 
    *           ��
    * �������: 
    *           ��
    * �� �� ֵ: 
    *           T*  ָ��
    * ��    ��: ����ƽ
    * ��    ע: �ڲ��Ѿ�ʵ�ֶ��пռ��ͷ�, �������ͷ�
    * ����ʱ��: 2011/1/25  
    * �޸���ʷ
    *     ʱ��       ����             ԭ��
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
    * ��    ��: ���д�С
    * �������: 
    *           ��
    * �������: 
    *           ��
    * �� �� ֵ: 
    *           uint64_t  ������T*�ĸ���
    * ��    ��: ����ƽ
    * ��    ע: Ӧ�ò��ÿ���������
    * ����ʱ��: 2011/1/25  
    * �޸���ʷ
    *     ʱ��       ����             ԭ��
    * 
    **********************************************************/
    inline uint64_t size()
    {
        return m_dqCntZero;
    }
    
    /*********************************************************
    * ��    ��: ������ȡ��������
    * �������: 
    *           deque<T*>   T*���ͻ������
    *           uint32_t    ��ȡ���и���
    * �������: 
    *           deque<T*>   T*���ͻ������
    * �� �� ֵ: 
    *           uint32_t    �ɹ��Ӷ����л�ȡT*�ĸ���
    * ��    ��: ����ƽ
    * ��    ע: ��������еĸ�������uiCnt�Ĵ�С,���л���ȡ����
    * ����ʱ��: 2011/1/25  
    * �޸���ʷ
    *     ʱ��       ����             ԭ��
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
    deque<T>        m_dqZero;         //Zero ����
    CPosixMutex     m_lckZero;        //Zero ���л�����
    uint64_t        m_dqCntZero;      //Zero ���д�С
    int             lstsize;          //�ߴ�
    deque<T>        lmsgs;            //add by dlq
};


#endif //#ifndef __BRD_DOUBLE_LIST_HH__

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ThreadPool.h
/// @brief �̳߳�
/// 
/// @author ������
/// @version 1.0
/// @date   2017.7.7
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Typedef.h"
#include "SyncQueue.h"

#include <list>
#include <thread>
#include <functional>
#include <memory>
#include <atomic>
#include <vector>


#define DEFAULT_TASK_COUNT_MAX  1024             ///< ���������

class CTask;

//����Ự�ӿڶ���
class CTaskSink
{
public:	
	virtual bool OnEnd(CTask* pTask) = 0;     // ׼��ִ������
};



class CTask 
{
public:
	CTask(unsigned int nType, CTaskSink* pTaskSink = nullptr)
		: m_nType(nType)
		, m_pTaskSink(pTaskSink)
	{}
	virtual	~CTask() {}

public:
	void SetType(unsigned int nType) { m_nType = nType; }
	unsigned int GetType()           { return m_nType; }
	void SetTaskSink(CTaskSink* pTaskSink) { m_pTaskSink = pTaskSink; }

public:
	virtual bool Execute() = 0;
	virtual void End() 
	{
		if (nullptr != m_pTaskSink)
		{
			m_pTaskSink->OnEnd(this);
		}
	}
	
	virtual std::string GetDesc() { return ""; }
	
private:
	unsigned int      m_nType;       ///< ��������
	CTaskSink*   m_pTaskSink;   ///< ����ص��ӿ�
};

using THREAD_POOL_FUN = std::function<void()>;

class CSimpleCTask : public CTask
{
public:
	CSimpleCTask(const THREAD_POOL_FUN& refThreadFun, CTaskSink* pTaskSink = nullptr)
		: CTask(0, pTaskSink)
		, m_pThreadFun(refThreadFun)
	{
	}
	
public:
	virtual bool Execute()
	{
		m_pThreadFun();
		return true;
	}
	
public:
	THREAD_POOL_FUN m_pThreadFun;
};
	
using TASK = std::shared_ptr<CTask>;

class CCustomThreadPool
{
public:	
	CCustomThreadPool(unsigned int nMaxTaskCount = DEFAULT_TASK_COUNT_MAX)
		: m_objTaskQueue(nMaxTaskCount)
	{
	}	
	
	~CCustomThreadPool()
	{
	}
	
public:
//	void AddTask(TASK&& refNewTask)
//	{
//		m_objTaskQueue.Put(std::forward<TASK>(refNewTask));
//	}
	
	bool AddTask(TASK refNewTask)
	{
		m_objTaskQueue.Put(refNewTask);
		//LOG_INFO("Append a new thread task '%0X'.", refNewTask.get());

		return true;
	}
	
//--------------------------------------------------------------------------------------------------------------------
//�������ƣ�Start
/// @brief �����̳߳�
///
/// ��Ҫ���Ǵ��������̣߳������̶߳�����ӵ��߳��б���
/// @param _in nWorkThreads->�����̵߳�����
/// @author ������
//--------------------------------------------------------------------------------------------------------------------
	void Start(unsigned int nWorkThreads = std::thread::hardware_concurrency() * 2)
	{
		m_bRuning = true;
		
		try
		{
			for (unsigned int i = 0; i < nWorkThreads; i++)
			{
				std::thread pNewThread(&CCustomThreadPool::ThreadFun, this);

#ifdef WIN32
				SetThreadPriority(pNewThread.native_handle(), THREAD_PRIORITY_TIME_CRITICAL);
#endif
				m_objWordThreads.push_back(std::move(pNewThread));
			}
		}
		catch (std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
		}
	}
	
//--------------------------------------------------------------------------------------------------------------------
//�������ƣ�Stop
/// @brief ֹͣ�̳߳�
///
/// ���������в��ر����еĹ����߳�
/// @author ������
//--------------------------------------------------------------------------------------------------------------------
	void Stop()
	{
		std::call_once(m_objFlag, [this]{StopAllThreads(); });
	}
	
private:
	void ThreadFun()
	{
		while (m_bRuning)
		{
			TASK refTask;
			
			m_objTaskQueue.Take(refTask);	

			if (nullptr != refTask)
			{
				//LOG_INFO("The thread '%0X' is runing.", refTask.get());
				refTask->Execute();
				//LOG_INFO("The thread task was completed '%0X'.", refTask.get());
				refTask->End();
			}
		}

		//LOG_INFO("The thread will be exited.");
	}
	
	
	void StopAllThreads()
	{
		m_bRuning = false;
		m_objTaskQueue.Close();
		 
		for (auto& refThread : m_objWordThreads)
		{
			if (refThread.joinable())
			{
				refThread.join();
			}
		}
		
		m_objWordThreads.clear();		
	}
	
private:
	std::vector<std::thread> m_objWordThreads;   ///< �����߳��б�
	CSyncQueue<TASK>       m_objTaskQueue;                           ///< �������
	std::atomic_bool             m_bRuning {true};                       ///< �������б�־
	std::once_flag                m_objFlag;		                               ///< 
};
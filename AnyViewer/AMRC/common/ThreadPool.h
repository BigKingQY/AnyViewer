//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ThreadPool.h
/// @brief 线程池
/// 
/// @author 黄丽云
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


#define DEFAULT_TASK_COUNT_MAX  1024             ///< 最大任务数

class CTask;

//抽象会话接口定义
class CTaskSink
{
public:	
	virtual bool OnEnd(CTask* pTask) = 0;     // 准备执行任务
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
	unsigned int      m_nType;       ///< 任务类型
	CTaskSink*   m_pTaskSink;   ///< 任务回掉接口
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
//函数名称：Start
/// @brief 启动线程池
///
/// 主要就是创建工作线程，并把线程对象添加到线程列表中
/// @param _in nWorkThreads->工作线程的数量
/// @author 黄丽云
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
//函数名称：Stop
/// @brief 停止线程池
///
/// 清空任务队列并关闭所有的工作线程
/// @author 黄丽云
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
	std::vector<std::thread> m_objWordThreads;   ///< 工作线程列表
	CSyncQueue<TASK>       m_objTaskQueue;                           ///< 任务队列
	std::atomic_bool             m_bRuning {true};                       ///< 正在运行标志
	std::once_flag                m_objFlag;		                               ///< 
};
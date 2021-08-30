//////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file pTimer.h
/// @brief 时钟管理类
/// 
/// @author
/// @version 1.0
/// @date 2017
/// @note 
//////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "MiscellaneousFunctions.h"

#include <vector>
#include <mutex>
#include <memory>

using ONTIMERPROC = std::function<void(void)>;


/// @brief 时钟对象定义
class CTimerManager;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 
/// </summary>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CTimer : public std::enable_shared_from_this<CTimer>
{
public:
	enum TIME_TYPE { TT_ONCE = 0, TT_CIRCLE = 1 };

	CTimer(CTimerManager* pTimerManager = nullptr);
	~CTimer();
	
public:

	// 初始化时钟
	void Initialize(ONTIMERPROC fnTimerProc, const unsigned int nInterval, const TIME_TYPE type);

	// 启动一个定时器
	void   Start();
	
	// 终止一个定时器
	void   Stop();

	// 执行
	void OnTimer(unsigned long long nNow);

private:
	DECLARE_MEMBER_AND_METHOD_V11(CTimerManager*,m_pTimerManager, TimerManager, nullptr);
	
	//调用函数，包括仿函数
	DECLARE_MEMBER_AND_METHOD_V11(ONTIMERPROC,m_fnTimerProc, TimerProc,nullptr);

	// 时钟类型
	DECLARE_MEMBER_AND_METHOD_V11(TIME_TYPE,m_nType, Type, TT_ONCE);
	
	//间隔
	DECLARE_MEMBER_AND_METHOD_V11(unsigned int,m_nInterval, Interval,0);
	
	//过期
	DECLARE_MEMBER_AND_METHOD_V11(unsigned long long,m_nExpires, Expires,0);
	
	DECLARE_MEMBER_AND_METHOD_V11(int,m_nHeapIndex, HeapIndex,-1);
};



using CTimerPtr = std::shared_ptr<CTimer>;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 
/// </summary>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CTimerHeapEntry
{
public:
	CTimerHeapEntry(unsigned long long nTime, CTimerPtr pTimer)
		: m_nTime(nTime)
		, m_pTimer(pTimer)
	{
	}

	~CTimerHeapEntry(){}

	CTimerHeapEntry(const CTimerHeapEntry &refSource)
		: m_nTime(refSource.m_nTime)
		, m_pTimer(refSource.m_pTimer)
	{
	}

	CTimerHeapEntry& operator=(const CTimerHeapEntry &refSource)
	{
		if (this == &refSource)
		{
			return *this;
		}

		m_nTime = (refSource.m_nTime);
		m_pTimer = (refSource.m_pTimer);

		return *this;
	}
private:
	DECLARE_MEMBER_AND_METHOD_V11(unsigned long long,m_nTime, Time,0);
	DECLARE_MEMBER_AND_METHOD_V11(CTimerPtr,m_pTimer, Timer,nullptr);
};




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 
/// </summary>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CTimerManager
{
public:
	//获取当前毫秒数
	static unsigned long long GetCurrentMillisecs();

	//探测执行
	void Run();

	// 添加一个定时器
	bool CreateTimer(CTimerPtr pTimer);

	// 移除一个定时器
	bool DelTimer(CTimerPtr pTimer);

	bool ModifyTimer(CTimerPtr pTimer, const unsigned int nInterval);

	// 查找时钟
	bool LookupTimer(CTimerPtr pTimer);
private:
	//定时上浮
	void UpHeap(int nIndex);

	//定时下沉
	void DownHeap(int nIndex);

	//交换两个pTimer索引
	void Swapheap(int nIndex1, int nIndex2);

private:
	std::vector<CTimerHeapEntry>  m_arrHeap;
	std::recursive_mutex     m_objMutex;
};

using CTimerManagerPtr = std::shared_ptr<CTimerManager>;

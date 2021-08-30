//////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file pTimer.c
/// @brief 线程安全的时钟管理类
/// 
/// @author
/// @version 1.0
/// @date 2017
/// @note 该定时器是基于时间轮 (Timing-Wheel) 方式实现的定时器
/// 参考资料 http://www.ibm.com/developerworks/cn/linux/l-cn-timers/
///             http://blog.csdn.net/walkingman321/article/details/6101536
/// http://www.cnblogs.com/gtarcoder/p/4924097.html
/// https://www.cnblogs.com/highway-9/p/5737421.html
//////////////////////////////////////////////////////////////////////////////////////////////////////
#include "timer.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <chrono>
#include <ctime>
#include <assert.h> 
#include <algorithm>

using namespace std::chrono;




CTimer::CTimer(CTimerManager* pTimerManager)
	: m_pTimerManager(pTimerManager)
{
}

CTimer::~CTimer()
{
	// 不能调用Stop();	
}

//--------------------------------------------------------------------------------
/// <summary>
/// 初始化时钟
/// </summary>
/// <param name="fnTimerProc">时钟响应函数</param>
/// <param name="nInterval">时间间隔(单位：毫秒数)</param>
/// <param name="nType">时钟类型</param>
/// <created>Andy,2019/7/8</created>
/// <changed>Andy,2019/7/8</changed>
//--------------------------------------------------------------------------------
void CTimer::Initialize(
	ONTIMERPROC fnTimerProc
	, const unsigned int nInterval
	, const TIME_TYPE nType)
{
	assert(fnTimerProc != nullptr && nInterval > 0);

	m_fnTimerProc = fnTimerProc;
	m_nInterval = nInterval;
	m_nExpires = nInterval + CTimerManager::GetCurrentMillisecs();
	m_nType = nType;
}


//--------------------------------------------------------------------------------
/// <summary>
/// 启动一个定时器
/// </summary>
/// <created>Andy,2019/7/8</created>
/// <changed>Andy,2019/7/8</changed>
//--------------------------------------------------------------------------------
void  CTimer::Start()
{
	if (nullptr != m_pTimerManager)
	{
		m_pTimerManager->CreateTimer(shared_from_this());
	}
}

//--------------------------------------------------------------------------------
/// <summary>
/// 终止一个定时器
/// </summary>
/// <created>Andy,2019/7/8</created>
/// <changed>Andy,2019/7/8</changed>
//--------------------------------------------------------------------------------
void CTimer::Stop()
{
	if (nullptr != m_pTimerManager)
	{
		if (m_nHeapIndex != -1)
		{
			m_pTimerManager->DelTimer(shared_from_this());
			m_nHeapIndex = -1;
		}
	}
}

//--------------------------------------------------------------------------------
/// <summary>
/// 
/// </summary>
/// <param name="nNow"></param>
/// <created>Andy,2019/7/8</created>
/// <changed>Andy,2019/7/8</changed>
//--------------------------------------------------------------------------------
void CTimer::OnTimer(unsigned long long /*nNow*/)
{
	m_fnTimerProc();	
}


/*----------------------------CTimerManager---------------------------*/

//--------------------------------------------------------------------------------
/// <summary>
/// 添加一个定时器
/// </summary>
/// <param name="pTimer">指向时钟对象的指针</param>
/// <created>Andy,2019/7/8</created>
/// <changed>Andy,2019/7/8</changed>
//--------------------------------------------------------------------------------
bool CTimerManager::CreateTimer(CTimerPtr pTimer)
{
	bool nResult = false;
	assert(nullptr != pTimer);

	if (nullptr != pTimer)
	{
		if (!LookupTimer(pTimer))
		{
			std::lock_guard<std::recursive_mutex> objLock(m_objMutex);

			//插到数组最后一个位置上，上浮
			pTimer->SetHeapIndex(m_arrHeap.size());
			pTimer->SetTimerManager(this);

			m_arrHeap.push_back(CTimerHeapEntry(pTimer->GetExpires(), pTimer));
			UpHeap(m_arrHeap.size() - 1);

			nResult = true;
		}
	}

	return nResult;
}

//--------------------------------------------------------------------------------
/// <summary>
/// 
/// </summary>
/// <param name="pTimer"></param>
/// <created>Andy,2019/7/8</created>
/// <changed>Andy,2019/7/8</changed>
//--------------------------------------------------------------------------------
bool CTimerManager::DelTimer(CTimerPtr pTimer)
{
	bool nResult = false;
	assert(nullptr != pTimer);

	if (nullptr != pTimer)
	{
		//头元素用数组未元素替换，然后下沉
		std::lock_guard<std::recursive_mutex> objLock(m_objMutex);
		const int nIndex = pTimer->GetHeapIndex();

		if (!m_arrHeap.empty() && nIndex < int(m_arrHeap.size()))
		{
			if (nIndex == int(m_arrHeap.size() - 1))  // 如果是最后的时钟
			{
				m_arrHeap.pop_back();
			}
			else  //more than one
			{
				Swapheap(nIndex, m_arrHeap.size() - 1);
				m_arrHeap.pop_back();

				int nParentIndex = (nIndex - 1) / 2;

				if (nIndex > 0
					&& nParentIndex >= 0
					&& m_arrHeap[nIndex].GetTime() < m_arrHeap[nParentIndex].GetTime())
				{
					UpHeap(nIndex);
				}
				else
				{
					DownHeap(nIndex);
				}
			}
		}
		pTimer->SetTimerManager(nullptr);
		nResult = true;
	}

	return nResult;
}

//--------------------------------------------------------------------------------
/// <summary>
/// 修改时钟
/// </summary>
/// <param name="pTimer">指向始终对象的指针</param>
/// <param name="nInterval">超时时间(单位：毫秒)</param>
/// <created>Andy,2019/7/8</created>
/// <changed>Andy,2019/7/8</changed>
//--------------------------------------------------------------------------------
bool CTimerManager::ModifyTimer(CTimerPtr pTimer, const unsigned int nInterval)
{
	bool nResult = false;
	assert(nullptr != pTimer);

	if (nullptr != pTimer)
	{
		DelTimer(pTimer);

		pTimer->SetInterval(nInterval);
		pTimer->SetExpires(nInterval + CTimerManager::GetCurrentMillisecs());

		nResult = CreateTimer(pTimer);
	}

	return nResult;
}

//--------------------------------------------------------------------------------
/// <summary>
/// 查找时钟
/// </summary>
/// <param name="pTimer"></param>
/// <returns></returns>
/// <created>Andy,2019/9/12</created>
/// <changed>Andy,2019/9/12</changed>
//--------------------------------------------------------------------------------
bool CTimerManager::LookupTimer(CTimerPtr pTimer)
{
	bool nResult = false;
	std::lock_guard<std::recursive_mutex> objLock(m_objMutex);

	for(CTimerHeapEntry objCurTimer: m_arrHeap)
	{
		if (objCurTimer.GetTimer() == pTimer)
		{
			nResult = true;
			LOG_INFO("Exit timer!");
			break;
		}
	}

	return nResult;
}

//--------------------------------------------------------------------------------
/// <summary>
/// 
/// </summary>
/// <created>Andy,2019/7/8</created>
/// <changed>Andy,2019/7/8</changed>
//--------------------------------------------------------------------------------
void CTimerManager::Run()
{
	std::lock_guard<std::recursive_mutex> objLock(m_objMutex);
	const unsigned long long nNow = GetCurrentMillisecs();

	while (!m_arrHeap.empty() && m_arrHeap[0].GetTime() <= nNow)
	{
		CTimerPtr pTimer = m_arrHeap[0].GetTimer();

		DelTimer(pTimer);
		pTimer->OnTimer(nNow);

		if (pTimer->GetType() == CTimer::TT_CIRCLE)
		{
			pTimer->SetExpires(nNow + pTimer->GetInterval());
			CreateTimer(pTimer);
		}
	}
}

//--------------------------------------------------------------------------------
/// <summary>
/// 
/// </summary>
/// <param name="nIndex"></param>
/// <created>Andy,2019/7/8</created>
/// <changed>Andy,2019/7/8</changed>
//--------------------------------------------------------------------------------
void CTimerManager::UpHeap(int nIndex)
{
	//下至上，和父节点比较。如果小于父节点上浮
	int nParentIndex = (nIndex - 1) / 2;

	while (nIndex > 0
		&& nParentIndex >= 0
		&& m_arrHeap[nIndex].GetTime() < m_arrHeap[nParentIndex].GetTime())
	{
		Swapheap(nIndex, nParentIndex);
		nIndex = nParentIndex;
		nParentIndex = (nIndex - 1) / 2;
	}
}

//--------------------------------------------------------------------------------
/// <summary>
/// 
/// </summary>
/// <param name="nIndex"></param>
/// <created>Andy,2019/7/8</created>
/// <changed>Andy,2019/7/8</changed>
//--------------------------------------------------------------------------------
void CTimerManager::DownHeap(int nIndex)
{
	//从上到下，算出左右子节点，和最小的交换
	int nLeftChild = nIndex * 2 + 1;

	while (nLeftChild < int(m_arrHeap.size()))
	{
		const int nMiddleChild = (nLeftChild + 1 == int(m_arrHeap.size())
			|| m_arrHeap[nLeftChild].GetTime() < m_arrHeap[nLeftChild + 1].GetTime()) ? nLeftChild : nLeftChild + 1;

		if (m_arrHeap[nIndex].GetTime() < m_arrHeap[nMiddleChild].GetTime())
		{
			break;
		}

		Swapheap(nIndex, nMiddleChild);
		nIndex = nMiddleChild;
		nLeftChild = nIndex * 2 + 1;
	}
}

//--------------------------------------------------------------------------------
/// <summary>
/// 
/// </summary>
/// <param name="nIndex1"></param>
/// <param name="nIndex2"></param>
/// <created>Andy,2019/7/8</created>
/// <changed>Andy,2019/7/8</changed>
//--------------------------------------------------------------------------------
void CTimerManager::Swapheap(int nIndex1, int nIndex2)
{
	assert(nIndex1 < int(m_arrHeap.size()));
	assert(nIndex2 < int(m_arrHeap.size()));

	CTimerHeapEntry objHeapEntry = m_arrHeap[nIndex1];

	m_arrHeap[nIndex1] = m_arrHeap[nIndex2];
	m_arrHeap[nIndex2] = objHeapEntry;
	m_arrHeap[nIndex1].GetTimer()->SetHeapIndex(nIndex1);
	m_arrHeap[nIndex2].GetTimer()->SetHeapIndex(nIndex2);
}

unsigned long long CTimerManager::GetCurrentMillisecs()
{
	const system_clock::time_point time_point_now = system_clock::now(); // 获取当前时间点
	const system_clock::duration duration_since_epoch	= time_point_now.time_since_epoch(); // 从1970-01-01 00:00:00到当前时间点的时长
	const time_t nResult = duration_cast<milliseconds>(duration_since_epoch).count(); // 将时长转换为微秒数

	return  nResult; // 当前时间的毫秒数
}

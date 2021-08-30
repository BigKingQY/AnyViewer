//////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file pTimer.c
/// @brief �̰߳�ȫ��ʱ�ӹ�����
/// 
/// @author
/// @version 1.0
/// @date 2017
/// @note �ö�ʱ���ǻ���ʱ���� (Timing-Wheel) ��ʽʵ�ֵĶ�ʱ��
/// �ο����� http://www.ibm.com/developerworks/cn/linux/l-cn-timers/
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
	// ���ܵ���Stop();	
}

//--------------------------------------------------------------------------------
/// <summary>
/// ��ʼ��ʱ��
/// </summary>
/// <param name="fnTimerProc">ʱ����Ӧ����</param>
/// <param name="nInterval">ʱ����(��λ��������)</param>
/// <param name="nType">ʱ������</param>
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
/// ����һ����ʱ��
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
/// ��ֹһ����ʱ��
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
/// ���һ����ʱ��
/// </summary>
/// <param name="pTimer">ָ��ʱ�Ӷ����ָ��</param>
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

			//�嵽�������һ��λ���ϣ��ϸ�
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
		//ͷԪ��������δԪ���滻��Ȼ���³�
		std::lock_guard<std::recursive_mutex> objLock(m_objMutex);
		const int nIndex = pTimer->GetHeapIndex();

		if (!m_arrHeap.empty() && nIndex < int(m_arrHeap.size()))
		{
			if (nIndex == int(m_arrHeap.size() - 1))  // ���������ʱ��
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
/// �޸�ʱ��
/// </summary>
/// <param name="pTimer">ָ��ʼ�ն����ָ��</param>
/// <param name="nInterval">��ʱʱ��(��λ������)</param>
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
/// ����ʱ��
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
	//�����ϣ��͸��ڵ�Ƚϡ����С�ڸ��ڵ��ϸ�
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
	//���ϵ��£���������ӽڵ㣬����С�Ľ���
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
	const system_clock::time_point time_point_now = system_clock::now(); // ��ȡ��ǰʱ���
	const system_clock::duration duration_since_epoch	= time_point_now.time_since_epoch(); // ��1970-01-01 00:00:00����ǰʱ����ʱ��
	const time_t nResult = duration_cast<milliseconds>(duration_since_epoch).count(); // ��ʱ��ת��Ϊ΢����

	return  nResult; // ��ǰʱ��ĺ�����
}

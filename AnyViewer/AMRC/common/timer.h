//////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file pTimer.h
/// @brief ʱ�ӹ�����
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


/// @brief ʱ�Ӷ�����
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

	// ��ʼ��ʱ��
	void Initialize(ONTIMERPROC fnTimerProc, const unsigned int nInterval, const TIME_TYPE type);

	// ����һ����ʱ��
	void   Start();
	
	// ��ֹһ����ʱ��
	void   Stop();

	// ִ��
	void OnTimer(unsigned long long nNow);

private:
	DECLARE_MEMBER_AND_METHOD_V11(CTimerManager*,m_pTimerManager, TimerManager, nullptr);
	
	//���ú����������º���
	DECLARE_MEMBER_AND_METHOD_V11(ONTIMERPROC,m_fnTimerProc, TimerProc,nullptr);

	// ʱ������
	DECLARE_MEMBER_AND_METHOD_V11(TIME_TYPE,m_nType, Type, TT_ONCE);
	
	//���
	DECLARE_MEMBER_AND_METHOD_V11(unsigned int,m_nInterval, Interval,0);
	
	//����
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
	//��ȡ��ǰ������
	static unsigned long long GetCurrentMillisecs();

	//̽��ִ��
	void Run();

	// ���һ����ʱ��
	bool CreateTimer(CTimerPtr pTimer);

	// �Ƴ�һ����ʱ��
	bool DelTimer(CTimerPtr pTimer);

	bool ModifyTimer(CTimerPtr pTimer, const unsigned int nInterval);

	// ����ʱ��
	bool LookupTimer(CTimerPtr pTimer);
private:
	//��ʱ�ϸ�
	void UpHeap(int nIndex);

	//��ʱ�³�
	void DownHeap(int nIndex);

	//��������pTimer����
	void Swapheap(int nIndex1, int nIndex2);

private:
	std::vector<CTimerHeapEntry>  m_arrHeap;
	std::recursive_mutex     m_objMutex;
};

using CTimerManagerPtr = std::shared_ptr<CTimerManager>;

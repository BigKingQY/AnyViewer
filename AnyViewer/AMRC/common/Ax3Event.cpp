#include "stdafx.h"
#include "Ax3Event.h"

CAx3Event::CAx3Event()
{
}

CAx3Event::~CAx3Event()
{
	m_objCondition.notify_one();
}

void CAx3Event::Notify()
{
	m_objCondition.notify_one();
}

// ********************************************************************************
/// <summary>
/// 等待事件或者超时
/// </summary>
/// <param name="nMilliseconds"></param>
/// <returns>true:事件触发，false:超时</returns>
/// <created>Andy,2020/11/6</created>
/// <changed>Andy,2020/11/6</changed>
// ********************************************************************************
bool CAx3Event::WaitForEvent(DWORD nMilliseconds)
{
	std::unique_lock<std::mutex> locker(m_objMutex);

	return (m_objCondition.wait_for(locker, std::chrono::milliseconds(nMilliseconds)) != std::cv_status::timeout);
}

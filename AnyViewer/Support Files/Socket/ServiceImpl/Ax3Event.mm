//
//  Ax3Event.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/9.
//

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
/// µ»¥˝ ¬º˛ªÚ’ﬂ≥¨ ±
/// </summary>
/// <param name="nMilliseconds"></param>
/// <returns>true: ¬º˛¥•∑¢£¨false:≥¨ ±</returns>
/// <created>Andy,2020/11/6</created>
/// <changed>Andy,2020/11/6</changed>
// ********************************************************************************
bool CAx3Event::WaitForEvent(DWORD nMilliseconds)
{
    std::unique_lock<std::mutex> locker(m_objMutex);

    return (m_objCondition.wait_for(locker, std::chrono::milliseconds(nMilliseconds)) != std::cv_status::timeout);
}

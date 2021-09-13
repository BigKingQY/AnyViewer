//
//  CNetTimer.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/13.
//

#include "CNetTimer.h"

void CNetTimer::restart()
{
    m_objNetTime = std::chrono::system_clock::now();
}

long CNetTimer::elapsed() const
{
    std::chrono::system_clock::time_point nowTime = std::chrono::system_clock::now();
    std::chrono::duration<int> duration = std::chrono::duration_cast<std::chrono::seconds>(nowTime - m_objNetTime);
    
    return duration.count();
}

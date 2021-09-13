//
//  CNetTimer.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/13.
//  由于boost::timer在iOS平台上不准确，用c++的timer来替换

#ifndef CNetTimer_h
#define CNetTimer_h

#include <chrono>

class CNetTimer
{
    
public:
    
    ///重置定时器
    void restart();
    
    ///获取与上次定时的时间差
    long elapsed() const;
    
    
private:
    
    std::chrono::system_clock::time_point       m_objNetTime;
};

#endif /* CNetTimer_h */

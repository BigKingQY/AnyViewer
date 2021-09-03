//
//  Timer.hpp
//  AnyViewer
//
//  Created by Aomei on 2021/8/31.
//

#pragma once
//#include "MiscellaneousFunctions.h"

#include <vector>
#include <mutex>
#include <memory>
#include "TypeDefine.h"

using ONTIMERPROC = std::function<void(void)>;


/// @brief  ±÷”∂‘œÛ∂®“Â
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

    // ≥ı ºªØ ±÷”
    void Initialize(ONTIMERPROC fnTimerProc, const unsigned int nInterval, const TIME_TYPE type);

    // ∆Ù∂Ø“ª∏ˆ∂® ±∆˜
    void   Start();
    
    // ÷’÷π“ª∏ˆ∂® ±∆˜
    void   Stop();

    // ÷¥––
    void OnTimer(unsigned long long nNow);

private:
    DECLARE_MEMBER_AND_METHOD_V11(CTimerManager*,m_pTimerManager, TimerManager, nullptr);
    
    //µ˜”√∫Ø ˝£¨∞¸¿®∑¬∫Ø ˝
    DECLARE_MEMBER_AND_METHOD_V11(ONTIMERPROC,m_fnTimerProc, TimerProc,nullptr);

    //  ±÷”¿‡–Õ
    DECLARE_MEMBER_AND_METHOD_V11(TIME_TYPE,m_nType, Type, TT_ONCE);
    
    //º‰∏Ù
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int,m_nInterval, Interval,0);
    
    //π˝∆⁄
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
    //ªÒ»°µ±«∞∫¡√Î ˝
    static unsigned long long GetCurrentMillisecs();

    //ÃΩ≤‚÷¥––
    void Run();

    // ÃÌº”“ª∏ˆ∂® ±∆˜
    bool CreateTimer(CTimerPtr pTimer);

    // “∆≥˝“ª∏ˆ∂® ±∆˜
    bool DelTimer(CTimerPtr pTimer);

    bool ModifyTimer(CTimerPtr pTimer, const unsigned int nInterval);

    // ≤È’“ ±÷”
    bool LookupTimer(CTimerPtr pTimer);
private:
    //∂® ±…œ∏°
    void UpHeap(int nIndex);

    //∂® ±œ¬≥¡
    void DownHeap(int nIndex);

    //Ωªªª¡Ω∏ˆpTimerÀ˜“˝
    void Swapheap(int nIndex1, int nIndex2);

private:
    std::vector<CTimerHeapEntry>  m_arrHeap;
    std::recursive_mutex     m_objMutex;
};

using CTimerManagerPtr = std::shared_ptr<CTimerManager>;

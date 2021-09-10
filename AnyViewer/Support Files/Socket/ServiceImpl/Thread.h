//
//  Thread.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/10.
//

#ifndef Thread_h
#define Thread_h


#pragma once
#include "Typedef.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

class CThread
{
public:
    CThread();
    virtual ~CThread();

    enum THREAD_STATE
    {
        TS_STOPED,     ///<Õ£÷π◊¥Ã¨£¨∞¸¿®¥”Œ¥∆Ù∂Øπ˝∫Õ∆Ù∂Ø∫Û±ªÕ£÷π
        TS_RUNNING,    ///<‘À––◊¥Ã¨
        TS_PAUSED      ///<‘›Õ£◊¥Ã¨
    };

public:
    THREAD_STATE GetStatus() const;

    unsigned int GetThreadId() const;

    bool IsTerminating();

    bool isActive() const { return  (m_nState != TS_STOPED); }

    // …Ë÷√œﬂ≥Ã”≈œ»º∂
    void SetPriority(const int nPriority);
public:

    virtual bool Start();
    virtual void Stop();
    virtual void Pause();
    virtual void Resume();

    /**
 * Terminates thread execution.
 * @remark thread-safe.
 */
    virtual void Terminate();

    // This function calling before call a derived Execute() function to
// perform any additional action.
    virtual void InitByDerived();
public:
    static void sleep(DWORD millis);

    /**
     * Yield execution to the next ready thread.
     */
    static void yield();

protected:
    virtual void Execute() = 0;

    /**
 * Slot of Terminate() signal.
 * Method called from Terminate() method.
 * Can be overrided by subclasses to gracefully shutdown thread.
 */
    virtual void OnTerminate();

    void SetStatus(const THREAD_STATE nState) {    m_nState = nState;}

protected:
    void Run();

    void SetTerminated(const bool bTerminated) { m_bTerminated = bTerminated; }
protected:
    std::thread              m_objThread;
    std::mutex               m_objMutex;
    std::condition_variable  m_objCondition;
    std::atomic<THREAD_STATE> m_nState{ TS_STOPED };
    volatile bool            m_bTerminated = true;

    // øÿ÷∆À¯»∑±£m_objThread∂‘œÛ «”––ßµƒ
    std::mutex               m_objCtrlMutex;
};

using CThreadPtr = std::shared_ptr<CThread>;
using CThreadArray = std::vector<CThreadPtr>;


#endif /* Thread_h */

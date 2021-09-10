//
//  Thread.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/10.
//

#include "Thread.h"
#include "Log.h"
#include <sstream>

using namespace std;

// ********************************************************************************
/// <summary>
/// œﬂ≥Ã∂‘œÛππ‘Ï∫Ø ˝
/// </summary>
/// <param name="bAutoStart">true:◊‘∂Ø∆Ù∂Ø,false:≤ª◊‘∂Ø∆Ù∂Ø</param>
/// <created>Andy,2020/11/6</created>
/// <changed>Andy,2020/11/6</changed>
// ********************************************************************************
CThread::CThread()
{
}

CThread::~CThread()
{
    Stop();
}

CThread::THREAD_STATE CThread::GetStatus() const
{
    return m_nState;
}

unsigned int CThread::GetThreadId() const
{
    std::ostringstream ossID;

    ossID << m_objThread.get_id();

    const std::string strID = ossID.str();
    const unsigned nResult = std::stoul(strID);

    return nResult;
}

// ********************************************************************************
/// <summary>
/// ∆Ù∂Øœﬂ≥Ã∂‘œÛ
/// </summary>
/// <returns>true:≥…π¶,false: ß∞‹</returns>
/// <created>Andy,2020/11/6</created>
/// <changed>Andy,2020/11/6</changed>
// ********************************************************************************
bool CThread::Start()
{
    std::ostringstream  ossLogMsg;
    bool bResult = false;

    if (TS_STOPED == m_nState)
    {
        std::unique_lock<std::mutex> objAutoLock(m_objCtrlMutex);

        //…Ë÷√◊¥Ã¨±ÿ–Î‘⁄∆Ù∂Øœﬂ≥Ã÷Æ«∞£¨“ÚŒ™œﬂ≥Ã∫Ø ˝÷––Ë“™≈–∂œ◊¥Ã¨m_nState
        m_bTerminated = false;
        m_nState = TS_RUNNING;

        std::thread objThread([this]() {Run(); });

        m_objThread = std::move(objThread);
        bResult = true;
        ossLogMsg << "Start the thread <" << m_objThread.get_id()<<"> successfully!";
    }
    else
    {
        ossLogMsg << "The thread has started :" << m_objThread.get_id();
    }

    LOG_DEBUG("%s", ossLogMsg.str().c_str());
    
    return bResult;
}

void CThread::Stop()
{
    if (TS_STOPED != m_nState)
    {
        Terminate();
        m_nState = TS_STOPED;
        m_objCondition.notify_all();  // Notify one waiting thread, if there is one.

        {
            std::unique_lock<std::mutex> objAutoLock(m_objCtrlMutex);
            
            if (m_objThread.joinable())
            {
                m_objThread.join(); // wait for thread finished
            }
        }
    }
    else
    {
        LOG_WARN("%s", "The thread don't start!");
    }
}

void CThread::Pause()
{
    if (TS_RUNNING == m_nState)
    {
        m_nState = TS_PAUSED;
    }
}

void CThread::Resume()
{
    if (TS_PAUSED == m_nState)
    {
        m_objCondition.notify_all();
        m_nState = TS_RUNNING;
    }
}

void CThread::Terminate()
{
    m_bTerminated = true;

    OnTerminate();
}

void CThread::yield()
{
//    SwitchToThread();
}


void CThread::InitByDerived()
{
}

void CThread::sleep(DWORD millis)
{
//    Sleep(millis);
    ::sleep(millis);
}

void CThread::OnTerminate()
{
}

bool CThread::IsTerminating()
{
    return m_bTerminated;
}

// ********************************************************************************
/// <summary>
///
/// </summary>
/// <param name="nPriority"></param>
/// <created>Andy,2021/3/12</created>
/// <changed>Andy,2021/3/12</changed>
// ********************************************************************************
void CThread::SetPriority(const int nPriority)
{
    const HANDLE pThreadHandle = (HANDLE)m_objThread.native_handle();

    if (nullptr != pThreadHandle)
    {
//        SetThreadPriority(pThreadHandle, nPriority);
    }
    else
    {
        LOG_DEBUG("Invalid thread handle!");
    }
}


void CThread::Run()
{
    std::unique_lock<std::mutex> objAutoLock(m_objCtrlMutex);  ///< ¥À––¥˙¬Î≤ªƒ‹»°œ˚
    std::ostringstream  ossLogMsg;

    ossLogMsg << "enter thread:" << this_thread::get_id();
    LOG_DEBUG("%s",ossLogMsg.str().c_str());
    
    InitByDerived();

    while (TS_STOPED != GetStatus())
    {
        try
        {
            Execute();
        }
        catch (std::exception& e)
        {
            LOG_ERROR("exception:%s", e.what());
        }
        catch (...)
        {
            LOG_ERROR("exception");
        }

        if (TS_PAUSED == GetStatus())
        {
            std::unique_lock<std::mutex> locker(m_objMutex);

            while (TS_PAUSED == GetStatus())
            {
                m_objCondition.wait(locker); // Unlock m_objMutex and wait to be notified
            }
        }
    }

    m_objThread.detach();
    m_nState = TS_STOPED;

    ossLogMsg.str("");
    ossLogMsg << "exit thread:" << this_thread::get_id();
    LOG_DEBUG("%s", ossLogMsg.str().c_str());
}

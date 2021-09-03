//
//  SDPMessageBus.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//


#ifndef SDPMessageBus_h
#define SDPMessageBus_h

#pragma once

#include "SDPFunctionTraits.h"
#include <string>
#include <map>
#include <any>
#include <boost/noncopyable.hpp>
//#include <boost/thread.hpp>
#include <boost/any.hpp>

class CFunctionInfo
{
public:
    template<typename Function>
    CFunctionInfo(unsigned int nID,Function && fnRoutine)
        : m_nID(nID)
        , m_fnRoutine(fnRoutine)
    {
    }
public:
    unsigned int GetID() { return m_nID; }
    boost::any GetRoutine() { return m_fnRoutine; }
    
public:
    unsigned int             m_nID;
    boost::any                 m_fnRoutine;
};

class CMessageBus : boost::noncopyable
{
public:
    // Message
    template<typename Function>
    unsigned int  Register(const unsigned int nTopic,Function && fnRoutine)
    {
        std::lock_guard<std::mutex> objLock(m_objMutex);
        auto func = to_function(std::forward<Function>(fnRoutine));
        
        return Add(nTopic, std::move(func));
    }

    //unregister Message
    //template<typename R, typename... Args,typename Function>
    bool Unregister(const unsigned int nTopic, const unsigned int nRoutineID)
    {
        bool bResult = false;
        std::lock_guard<std::mutex> objLock(m_objMutex);
        auto range = m_mapObserver.equal_range(nTopic);

        for (Iterater it = range.first; it != range.second; ++it)
        {
            if (boost::any_cast<CFunctionInfo>(it->second).GetID() == nRoutineID)
            {
                m_mapObserver.erase(it);
                bResult = true;
                break;
            }
        }

        return bResult;
    }

    //send message
    template<typename R>
    void SendReq(const unsigned int nTopic)
    {
        std::lock_guard<std::mutex> objLock(m_objMutex);
        using function_type = std::function<R()>;
        auto range = m_mapObserver.equal_range(nTopic);  //range  std::pair<iterator, iterator>
        //std::pair<std::multimap<std::string, any>::iterator, std::multimap<std::string, any>::iterator> range = m_mapObserver.equal_range(strMsgType);
        
        for (Iterater it = range.first; it != range.second; ++it)
        {
            auto fnRoutine = boost::any_cast<function_type>(boost::any_cast<CFunctionInfo>(it->second).GetRoutine());
            fnRoutine();
        }
    }

    template<typename R, typename... Args>
    void SendReq(const unsigned int nTopic,Args&&... args)
    {
        std::lock_guard<std::mutex> objLock(m_objMutex);
        using function_type = std::function<R(Args...)>;
        auto range = m_mapObserver.equal_range(nTopic);

        for (Iterater it = range.first; it != range.second; ++it)
        {
            auto fnRoutine = boost::any_cast<function_type>(boost::any_cast<CFunctionInfo>(it->second).GetRoutine());
            fnRoutine(args...);
        }
    }

    void RemoveTopic(const unsigned int nTopic)
    {
        std::lock_guard<std::mutex> objLock(m_objMutex);
        auto range = m_mapObserver.equal_range(nTopic);

        m_mapObserver.erase(range.first, range.second);
    }

private:
    template<typename Function>
    unsigned int Add(const unsigned int nTopic, Function && fnRoutine)
    {
        m_nNextRoutineID++;
        m_mapObserver.emplace(nTopic, CFunctionInfo(m_nNextRoutineID, fnRoutine));

        return m_nNextRoutineID;
    }

private:
    unsigned int             m_nNextRoutineID = 0;
    std::mutex      m_objMutex;           ///< ≤ªƒ‹ π”√µ›πÈÀ¯£¨∑Ò‘Úø…ƒ‹ª·“˝∆≥Ã–Ú±¿¿£
    std::multimap<unsigned int, boost::any> m_mapObserver;
    typedef std::multimap<unsigned int, boost::any>::iterator Iterater;
};

#endif

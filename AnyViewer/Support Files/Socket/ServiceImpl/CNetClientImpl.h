//
//  CNetClientImpl.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//

#ifndef CNetClientImpl_h
#define CNetClientImpl_h

//=============================================
/// @brief 通信端点事件接口
///
/// 事务对象与通信端点之间的事件接口，通信端点将调用该接口方法来通知事务对象，所有
/// 的事务对象也必须实现该接口
//=============================================
#pragma once
#include "CNetEndpoint.h"
//#include "PacketBase.h"
//#include "ServiceHeader.h"
//#include "CNetIPDataTransfer.h"
#include "CCustomThreadPool.h"
#include <errno.h>
#import <fcntl.h>


//=============================================
/// @brief 服务端事务对象模板基类
///
/// 该类实现了一个高效的网络通信服务器，用户可以自定义通信端点和数据包
//=============================================

template <typename EndpointT>
class CNetServiceImpl : public CNetEndpointEventSink
{
public:
    using PTR_ENDPOINT = std::shared_ptr<EndpointT>;

    CNetServiceImpl(void)
    {
    }

    CNetServiceImpl(unsigned int nIP, unsigned int nPort, unsigned int nWorkThreadNum = 8)
        : m_nIP(nIP)
        , m_nListenPort(nPort)
        , m_nWorkThreadNum(nWorkThreadNum)
    {
    }

    virtual ~CNetServiceImpl(void)
    {
        assert(SS_RUNING != m_nStatus);
        if (SS_RUNING != m_nStatus)
        {
            DestroyAllConversation();

            if (INVALID_SOCKET != m_nListeningFD)
            {
                close(m_nListeningFD);
                m_nListeningFD = INVALID_SOCKET;
            }

            if (INVALID_SOCKET != m_nEpollFD)
            {
                close(m_nEpollFD);
                m_nEpollFD = INVALID_SOCKET;
            }
        }

        if (nullptr != m_pTimerWhell)
        {
            delete m_pTimerWhell;
            m_pTimerWhell = nullptr;
        }
    }
public:
    void SetPort(unsigned int nPort)
    {
        m_nListenPort = nPort;
    }

    // ********************************************************************************
    /// <summary>
    /// 根据socket句柄查找对应的通信端点
    /// </summary>
    /// <param name="nFD"></param>
    /// <returns></returns>
    /// <created>Andy,2020/10/10</created>
    /// <changed>Andy,2020/10/10</changed>
    // ********************************************************************************
    PTR_ENDPOINT LookupEndpoint(const unsigned int nFD)
    {
        PTR_ENDPOINT objResult = nullptr;
        auto itr = m_mapAliveEndpoints.find(nFD);
        
        if (m_mapAliveEndpoints.end() != itr)
        {
            objResult = (*itr).second;
        }

        return objResult;
    }

public:

    


    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetServiceImpl::Start
    /// @brief  启动捕捉客户端连接请求的线程，开始接收客户端的连接请求
    ///
    /// @param _in true->成功,false->失败
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    bool Start()
    {
        assert(INVALID_SOCKET != m_nListeningFD);
        if (INVALID_SOCKET == m_nListeningFD)
        {
            return false;
        }

        assert(SS_INITIALIZED == m_nStatus || SS_STOP == m_nStatus);
        if (SS_INITIALIZED != m_nStatus && SS_STOP != m_nStatus)
        {
            return false;
        }

        m_nStatus = SS_RUNING;

        // 启动工作线程池
        m_objThreadPool.Start(m_nWorkThreadNum);

        // 启动Poll线程
//        std::thread objThread(std::bind(&CNetServiceImpl::EpollThread, this));
//        m_objPollThread = std::move(objThread);

        return true;
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetServiceImpl::Stop
    /// @brief  停止网络服务，主要包括三部分工作，参见下面的详细描述
    ///
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    void Stop()
    {
        assert(SS_RUNING == m_nStatus);
        if (SS_RUNING != m_nStatus)
        {
            return;
        }

        // 1、下面的代码是关闭侦听线程，
        m_nStatus = SS_STOP;

        // 停止poll线程
        m_objPollThread.join();


        // 停止工作线程池
        m_objThreadPool.Stop();

        // 2、下面的代码是让所有的会话停止接收数据
        for (auto refValue : m_mapAliveEndpoints)
        {
            refValue.second->SetContinueReceivingData(false);
        }

        // 5、把所有的会话记录移除到废弃列表
        //modify by lei at 2017/10/30 因为之前关闭连接，会造成和timer互锁
        DestroyAllConversation();
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetServiceImpl::SendBroadcastNotify
    /// @brief  向连接到该服务器的所有客户端发送通知
    ///
    /// @returnTRUE->成功,false->失败
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    virtual bool SendBroadcastNotify(CNetPacket* pPacket)
    {
        bool bResult = false;

        assert(pPacket);
        if (pPacket)
        {
            std::lock_guard<std::recursive_mutex> objAutoLock(m_objEndpointMutex);

            for (auto& refValue : m_mapAliveEndpoints)
            {
                if (nullptr != refValue.second)
                {
                    refValue.second->Send(pPacket);
                }
            }
        }

        return bResult;
    }

    ///@brief修改端点超时时间
    void ModTimer(PTR_NET_ENDPOINT_INTERFACE ptrNetEndpoint)
    {
        PTR_ENDPOINT ptrEndpoint = std::static_pointer_cast<EndpointT>(ptrNetEndpoint);
        CTimerPtr pTimer = ptrEndpoint->GetTimer();

        if (nullptr != pTimer)
        {
            m_pTimerWhell->ModifyTimer(pTimer, m_nKeepAliveTimeout * 1000);
        }
    }

public:
    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetServiceImpl::OnError
    /// @brief  处理服务器引发的错误
    ///
    /// @param _in SEVERITY_LEVEL nSeverity->错误的严重级别,参看定义SEVERITY_LEVEL
    /// @param _in SERVICE_EVENT nEvent->事件类型
    /// @param _in char lpErrMsg->提示消息
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    virtual void OnError(SEVERITY_LEVEL /*nSeverity*/, SERVICE_EVENT  /*nEvent*/, const char* /*lpErrMsg*/)
    {
        /*    const static char* szSeverityTips[] = { ("Information"),("Notice"),("Warning"),("Error") };


                    ATLTRACE( ("%s %s\r\n"), szSeverityTips[nSeverity], lpErrMsg);*/
    }

    virtual bool OnCloseEventHandle(PTR_NET_ENDPOINT_INTERFACE pConversationClosed)
    {
        assert(pConversationClosed);
//        if (nullptr != pConversationClosed)
//        {
//            CNetIPDataTransferPtr pNetDataTransfer = pConversationClosed->GetTransfer();
//
//            if (pNetDataTransfer)
//            {
//                sockaddr_in   objSocketAddr;
//                objSocketAddr.sin_addr.s_addr = htonl(pNetDataTransfer->GetIP());
//                LOG_DEBUG("The connection (%s: %d) will be closed"
//                    , inet_ntoa(objSocketAddr.sin_addr)
//                    , pNetDataTransfer->GetPort());
//            }
//
//            DiscardEndpoint(std::dynamic_pointer_cast<EndpointT>(pConversationClosed));
//        }

        return true;
    }
    virtual void OnNewConversationEventHandle(PTR_NET_ENDPOINT_INTERFACE /*pEndpoint*/) {}

    virtual bool OnReceivedPacketEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CNetPacket* /*pPacket*/) override
    {
        ModTimer(pEndpoint);
        return true;
    }
protected:
    virtual void ConnectTimeout(int nFd, unsigned int nIp, unsigned int nPort)
    {
        auto it = m_mapAliveEndpoints.find(nFd);

        if (m_mapAliveEndpoints.end() != it)
        {
            PTR_ENDPOINT ptrEndpoint = it->second;

            if (ptrEndpoint)
            {
                CNetIPDataTransferPtr pNetDataTransfer = ptrEndpoint->GetTransfer();

                if (nIp == pNetDataTransfer->GetIP() && nPort == pNetDataTransfer->GetPort())
                {
                    OnCloseEventHandle(ptrEndpoint);
//                    LOG_DEBUG("client fd[%d] timeout, it,s will be closed!", nFd);
                }
            }
        }
    }

private:



    //----------------------------------------------------------------------------------------------------
    //  函数名称: SetNoblockSocket
    /// @brief   把一个会话端点对象设置为非阻塞的模式
    ///
    /// @param  fd ->socket标识
    /// @return true:成功，false:失败
    //----------------------------------------------------------------------------------------------------
    bool SetNoblockSocket(int fd)
    {
        bool  nResult = false;
        const int OPTION = fcntl(fd, F_GETFL, 0);

        if (OPTION >= 0)
        {
            if (fcntl(fd, F_SETFL, OPTION | O_NONBLOCK) >= 0)
            {
                nResult = true;
            }
        }
        assert(true == nResult);

        return nResult;
    }



private:
    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetServiceImpl::CreateEndpoint
    /// @brief  创建一个会话对象，并将它插入会话列表
    ///
    /// @return PTR_ENDPOINT->成功返回一个指向新创建会话对象的指针
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    PTR_ENDPOINT CreateEndpoint()
    {
        return std::make_shared<EndpointT>(this);
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetServiceImpl::DiscardEndpoint
    /// @brief 把一个会话对象从列表中删除，并把它放入废除列表，等待将来释放
    ///
    /// @param _in PTR_ENDPOINTpEndpoint->被释放的会话对象
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    void DiscardEndpoint(PTR_ENDPOINT pEndpoint)
    {
        if (pEndpoint != nullptr)
        {
            //移除epoll事件
            EpollDeleteFD(pEndpoint->GetFD());

            //删除超时定时器(注意，必须放在m_objEndpointMutex锁之前，因为会和timer锁互锁)
            CTimerPtr  pTimer = pEndpoint->GetTimer();
            if (nullptr != pTimer)
            {
                m_pTimerWhell->DelTimer(pTimer);
            }


            std::lock_guard<std::recursive_mutex> objAutoLock(m_objEndpointMutex);
            //该socket fd已经被关闭了
            if (INVALID_SOCKET == pEndpoint->GetFD())
            {
                return;
            }

            auto itr = m_mapAliveEndpoints.find(pEndpoint->GetFD());

            assert(m_mapAliveEndpoints.end() != itr);
            if (m_mapAliveEndpoints.end() != itr)
            {
                int nFd = pEndpoint->GetFD();
                m_mapAliveEndpoints.erase(itr);
                LOG_DEBUG("close the fd[%d] then has %d endpoints is alive", nFd, m_mapAliveEndpoints.size());
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetServiceImpl::DestroyAllConversation
    /// @brief 释放所有的会话对象并清除列表
    ///
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    void DestroyAllConversation()
    {
        std::lock_guard<std::recursive_mutex> objAutoLock(m_objEndpointMutex);

        m_mapAliveEndpoints.clear();
    }

private:   //下面的代码是禁止该类的拷贝构造函数和赋值操作
    CNetServiceImpl(const CNetServiceImpl& refNetServiceImpl) {}
    CNetServiceImpl& operator=(const CNetServiceImpl& refNetServiceImpl) { return *this; }

protected:
    std::recursive_mutex                              m_objEndpointMutex;                        ///< 会话链表临界区
    std::map<int, PTR_ENDPOINT >  m_mapAliveEndpoints;      ///< 当前活跃的会话列表
protected:
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int,
    m_nIP,
        IP,
        0);                      ///< 服务器IP地址

    DECLARE_MEMBER_AND_METHOD_V11(unsigned int,
    m_nListenPort,
        ListenPort,
        0);    ///< 侦听端口

    DECLARE_MEMBER_AND_GET_METHOD_V11(int,
    m_nListeningFD,
        ListeningFD,
        -1);    ///< 服务器用于侦听的socket文件表述符号

    DECLARE_MEMBER_AND_METHOD_V11(int,
    m_nPollTimeout,
        PollTimeout,
        DEFAULT_POLL_TIMEOUT);                      ///< POLL超时时间

    DECLARE_MEMBER_AND_METHOD_V11(unsigned int,
    m_nMaxUsers,
        MaxUsers,
        MAX_USER);///< 支持最大的并发连接数


    DECLARE_MEMBER_AND_METHOD_V11(NET_TRANSACTION_STATUS,
    m_nStatus,
        Status,
        SS_INVALID);     ///< 服务器状态

    DECLARE_MEMBER_AND_METHOD_V11(CTimerManager*, m_pTimerWhell, TimerWhell, nullptr); ///<时间轮

    DECLARE_MEMBER_AND_METHOD_V11(U32,
    m_nKeepAliveTimeout,
        KeepAliveTimeout,
        DEFAULT_KEEP_ALIVE_TIMEOUT);                      ///< POLL超时时间

    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nWorkThreadNum, WorkThreadNum, 8); ///<工作线程数


    int                         m_nEpollFD{ -1 };             ///<
    CCustomThreadPool             m_objThreadPool;            ///< 工作线程池
    std::thread               m_objPollThread;             ///< 扫描socket事件线程
};





//=============================================
/// @brief 客户事务对象模板基类
///
/// 该类实现了一个基本的通信客户端，
//=============================================
template <typename EndpointT>
class CNetClientImpl : public CNetEndpointEventSink
{
public:
    CNetClientImpl(unsigned int nIP = 0, unsigned int nPort = 0)
        : m_nIP(nIP)
        , m_nPort(nPort)
    {
        
    }

    ~CNetClientImpl()
    {
        
    }

    CNetClientImpl(const CNetClientImpl& rPacketIoManager) = delete;
    CNetClientImpl& operator=(const CNetClientImpl& rPacketIoManager) = delete;

public:
    std::shared_ptr<EndpointT> GetEndpoint() { return m_objEndpoint; }          //会话对象

public:
    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetClientImpl::Initial
    /// @brief  初始化客户端 (主要活动就是初始化套接字库和创建一个事件对象)
    ///
    /// @return true->成功,false->失败
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    virtual bool Initial()
    {
        bool nResult = InitSocket();

        if (nResult)
        {
            m_objEndpoint = std::make_shared<EndpointT>(this);

            if (nullptr != m_objEndpoint)
            {
                m_objEndpoint->SetClient(true);
                nResult = m_objEndpoint->Initial();

                if (nResult)
                {
                    m_nStatus = SS_INITIALIZED;
                }
            }
            else
            {
                nResult = false;
            }
        }

        return nResult;
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetClientImpl::Start
    /// @brief 启动客户端，初始化会话对象，然后连接到服务器，最后启动接收线程
    ///
    /// @return   true: 成功, false: 失败
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    virtual bool Start()
    {
        assert(SS_INITIALIZED == m_nStatus || SS_STOP == m_nStatus);
        if (SS_INITIALIZED != m_nStatus && SS_STOP != m_nStatus)
        {
            return false;
        }


        bool bResult = m_objEndpoint->Connect(m_nIP, m_nPort, m_nConnectionTimeout);

        //创建线程的代码放在此处主要是等待通信链路创建
        // 线程启动一定要在socket连接成功以后启动
        std::thread objReceivingThead(std::bind(&CNetClientImpl::ReceivingThead, this));

        m_objReceivingThead = std::move(objReceivingThead);
        m_nStatus = SS_RUNING;

        return bResult;
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetClientImpl::Stop
    /// @brief 停止客户端，主要活动包括: 停止数据接受线程和关闭套接字
    ///
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    virtual void Stop()
    {
        assert(SS_RUNING == m_nStatus);

        if (SS_RUNING == m_nStatus)
        {
            m_objEndpoint->SetContinueReceivingData(false);
            m_objReceivingThead.join();
            m_objEndpoint->Close();
            m_nStatus = SS_STOP;
        }
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetClientImpl::RudelyStop
    /// @brief 粗鲁停止客户端，主要活动包括: 停止数据接受线程和关闭套接字
    ///
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    virtual void RudelyStop()
    {
        assert(SS_RUNING == m_nStatus);

        if (SS_RUNING == m_nStatus)
        {
            m_objEndpoint->SetContinueReceivingData(false);
            m_objReceivingThead.join();
            m_objEndpoint->Close(true);

            m_nStatus = SS_STOP;
        }
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetClientImpl::Send
    /// @brief 发送一个报文到服务器
    ///
    /// @param _in CNetPacket* pPacket->一个指向报文对象的指针
    /// @return true->成功,false->失败
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    virtual bool Send(CNetPacket* pPacket)
    {
        return m_objEndpoint->Send(pPacket);
    }
    
    virtual bool OnEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint, SEVERITY_LEVEL nSeverity, SERVICE_EVENT nEvent) override { return true; };

public:
    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetClientImpl::OnError
    /// @brief  处理服务器引发的错误
    ///
    /// @param _in SEVERITY_LEVEL nSeverity->错误的严重级别,参看定义SEVERITY_LEVEL
    /// @param _in SERVICE_EVENT nEvent->事件类型
    /// @param _in LPCSTR lpErrMsg->提示消息
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    virtual void OnError(SEVERITY_LEVEL nSeverity, SERVICE_EVENT  /*nEvent*/, const char* lpErrMsg)
    {
        //const static char* szSeverityTips[] = { ("Information"), ("Notice"), ("Warning"), ("Error") };
    //
        //CString strLog;
        //strLog.Format(("%s %s\r\n"), szSeverityTips[nSeverity], lpErrMsg);
        //ATLTRACE((LPCSTR)strLog);
    }

    bool WaitACK(unsigned int nTimeOut = 5 * 1000)
    {
        std::unique_lock<std::mutex> objAutoLock(m_objMutex);
        return (m_objAckRecevied.wait_until(objAutoLock, std::chrono::milliseconds(nTimeOut), []() {return true; }) == std::cv_status::no_timeout);
    }

protected:
    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetClientImpl::InitSocket
    /// @brief  初始化socket库
    ///
    /// @param _in If no error occurs, this function returns zero. If an error occurs, itr returns one of the error codes listed in the following table.
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    bool InitSocket()
    {
        return true;
    }

    // ********************************************************************************
/// <summary>
/// 接收数据包线程
/// </summary>
/// <created>Andy,2020/12/22</created>
/// <changed>Andy,2020/12/22</changed>
// ********************************************************************************
    void ReceivingThead()
    {
        m_objEndpoint->SetContinueReceivingData(true);
        m_objEndpoint->ReceiveData();
    }

protected:
    std::shared_ptr<EndpointT>     m_objEndpoint{ nullptr };         ///< 会话对象
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nIP, IP, 0)                       ///< 目标主机IP地址
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nPort, Port, 0)                   ///< 目标主机端口
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nConnectionTimeout, ConnectionTimeout, 30)            ///< 连接超时，单位s
    DECLARE_MEMBER_AND_METHOD_V11(NET_TRANSACTION_STATUS, m_nStatus, Status, SS_INVALID)               ///< 状态

    std::recursive_mutex           m_objMutex;
    std::condition_variable        m_objAckRecevied;
    std::thread                    m_objReceivingThead;
};

#endif

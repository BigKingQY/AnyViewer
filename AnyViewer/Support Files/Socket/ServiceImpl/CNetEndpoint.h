//
//  CNetEndpoint.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//

#ifndef CNetEndpoint_h
#define CNetEndpoint_h

#pragma once
#include "CNetIPDataTransfer.h"
#include "Timer.h"
#include "CDataPacket.h"
#include "EnumHeader.h"
#include <thread>


//typedef std::list<CNetPacket*>                              PACKETS_LIST;   ///< 数据包链表
//typedef std::list<CNetPacket*>::iterator                   PACKETS_LIST_ITR;

//typedef std::list<CSentPacketRecord*>                    PACKETS_RECORD_LIST; ///< 数据包记录链表
//typedef std::list<CSentPacketRecord*>::iterator         PACKETS_RECORD_LIST_ITR;



class CNetIPDataTransfer;
using CNetIPDataTransferPtr = std::shared_ptr<CNetIPDataTransfer>;

//=============================================
/// @brief 会话端点接口抽象定义
///
/// 这是一个接口类，定义了通信端点的常见方法
//=============================================
class CNetEndpointInterface;
using PTR_NET_ENDPOINT_INTERFACE = std::shared_ptr<CNetEndpointInterface>;
class CNetEndpointInterface : public std::enable_shared_from_this<CNetEndpointInterface>
{
public:
    /// @brief 关闭当前的会话
    ///
    /// @param bRude 关闭会话端点的模式,true表示立即关闭，否则表示温柔的关闭
    virtual void Close(bool bRude = false) = 0;

    /// @brief 发送数据包
    ///
    /// @param pPacket 一个指向数据包对象的指针
    /// @return  true: 成功，false: 失败
    virtual bool Send(CNetPacket* pPacket) = 0;

    /// @brief 从网络中接收数据包
    virtual void ReceiveData() = 0;

    /// @brief 得到与当前通信端点关联的数据传输对象
    ///
    /// @return  返回一个指向数据包传输对象的指针
    virtual CNetIPDataTransferPtr  GetTransfer() = 0;
};





//
using PTR_NET_PACKET = std::shared_ptr<CNetPacket>;
class CNetEndpointEventSink
{
public:
    CNetEndpointEventSink() {}
    virtual ~CNetEndpointEventSink() {}

public:
    /// @brief 在处理接收到的数据包之前调用该函数
    virtual bool OnProReceivedPacketEventHandle(PTR_NET_ENDPOINT_INTERFACE, CNetPacket*) { return true; }

    /// @brief 收到数据事件
    virtual bool OnProReceivedDataHandle(PTR_NET_ENDPOINT_INTERFACE, const CDataStreamBuffer& refInputBuffer) { return false; }

    /// @brief 接收数据包事件
    virtual bool OnReceivedPacketEventHandle(PTR_NET_ENDPOINT_INTERFACE, CNetPacket*) { return true; }
    virtual bool OnReceivedPacketEventHandle(PTR_NET_ENDPOINT_INTERFACE, PTR_NET_PACKET) { return true; }

    /// @brief 发送数据包之前调用该函数,false->停止发送,true->继续发送
    virtual bool OnPreSendPacketEventHandle(PTR_NET_ENDPOINT_INTERFACE, CNetPacket*) { return true; }

    /// @brief 重新发送数据包之前调用该函数,false->停止发送,true->继续发送
    virtual bool OnPreResendPacketEventHandle(PTR_NET_ENDPOINT_INTERFACE, CNetPacket*) { return true; }
public:

    /// @brief 会话端点重连事件
    virtual bool OnReconnectEventHandle(PTR_NET_ENDPOINT_INTERFACE) { return true; }

    /// @brief 通用的事件
    virtual bool OnEventHandle(PTR_NET_ENDPOINT_INTERFACE, SEVERITY_LEVEL /*nSeverity*/, SERVICE_EVENT /*nEvent*/) { return true; }
    
    /// @brief 会话对象关闭事件
    virtual bool OnCloseEventHandle(PTR_NET_ENDPOINT_INTERFACE) { return true; }
};

//*******************************************************************
/// <summary>
/// 等待应答任务
/// </summary>
//*******************************************************************
template <typename PacketT>
class CWaitingResponseTask
{
public:
    CWaitingResponseTask(const int nPacketID,const int nTimeout = 5)
        : m_objPacket(nullptr)
        , m_nPacketID(nPacketID)
        , m_nDiscard(false)
        , m_nTimeout(nTimeout)
    {}

    virtual ~CWaitingResponseTask()
    {
    }

public:
    void SetPacket(PacketT * pPacket)
    {
        assert(nullptr != pPacket);

        if (nullptr != pPacket)
        {
            m_objPacket = std::make_shared<PacketT>();

            *(m_objPacket.get()) = *pPacket;
        }
    }

    bool Wait()
    {
        std::unique_lock<std::mutex> objLocker(m_objMutex);
        const bool bResult = (m_cvNotify.wait_for(objLocker, std::chrono::seconds(m_nTimeout)) != std::cv_status::timeout);

        return bResult;
    }

    void Notify() { m_cvNotify.notify_all(); }

    std::shared_ptr<PacketT> GetPacket() { return  m_objPacket; }

    void SetPacketID(int nPacketID) { m_nPacketID = nPacketID; }
    int GetPacketID() { return m_nPacketID; }

    void SetDiscard(bool nDiscard) { m_nDiscard = nDiscard; }
    bool GetDiscard() { return m_nDiscard; }
    int GetTimeout() { return m_nTimeout; }

public:
    std::shared_ptr<PacketT>   m_objPacket;           ///< 收到的应答数据包
    std::mutex                 m_objMutex;
    std::condition_variable    m_cvNotify;
    int                        m_nPacketID;           ///< 需要应答数据包id
    bool                       m_nDiscard;            ///< 被废弃
    int                        m_nTimeout;            ///< 超时间阀值(单位：描述)
};


//=============================================
/// @brief 普通的通信端点类
///
/// 下面的类是一个简单通用会话模板类,该类有三个模板化的参数，分别是:
/// 1、整个通信所有使用的数据包类，用户需要根据实际情况自定义类，但是必须从CNetPacket派生
/// 2、定义该通信端点是属于客户端还是服务端，默认是客户端
/// 3、定义该通信所使用数据传输类，默认是"CNetTCPDataTransfer", 用户可以根据自己的需要
/// 自己定义，但必须从类"CNetIPDataTransfer"派生
//=============================================
class CNetTCPDataTransfer;
template <typename PacketT,  class TransferT = CNetTCPDataTransfer, const U32 INTPUT_BUFFER_SIZE = INIT_INPUT_BUFFE_SIZE>
class CNetEndpoint : public CNetEndpointInterface
{
public:
    //------------------------------------------------------------------------------------
    // CNetEndpoint
    /// @brief 构造函数
    ///
    /// @param- _in pNetEndpointEventSink -一个指向事件回调接口的指针
    /// @author 黄丽云
    //------------------------------------------------------------------------------------
    CNetEndpoint(CNetEndpointEventSink* pNetEndpointEventSink = nullptr)
        : m_pNetEndpointEventSink(pNetEndpointEventSink)
    {

    }

    //------------------------------------------------------------------------------------
    // ~CNetEndpoint
    /// @brief 析构函数
    ///
    /// @author 黄丽云
    //------------------------------------------------------------------------------------
    virtual ~CNetEndpoint()
    {
        m_pTransfer = nullptr;
    }

public:
    virtual CNetIPDataTransferPtr  GetTransfer()  override { return m_pTransfer; }

    //------------------------------------------------------------------------------------
    // SetInputBufferSize
    /// @brief 设置接收缓冲区的尺寸
    ///
    /// @param nSize->输入缓冲区的新尺寸
    /// @author 黄丽云
    //------------------------------------------------------------------------------------
    void SetInputBufferSize(int nSize) { m_objInputBuffer.Allocate(nSize); }

    int GetFD()
    {
        int nFD = -1;

        if (nullptr != m_pTransfer)
        {
            nFD = m_pTransfer->GetFD();
        }

        return nFD;
    }


    void AddRef() { m_nRefCount++; }
    void Release() { m_nRefCount--; }
    int GetRefCount() { return m_nRefCount; }

public:

    //------------------------------------------------------------------------------------
    // ChanageStatus
    /// @brief 设置当前的通信状态，并发送通知
    ///
    /// @param _in CONVERSATION_STATE nStatus ->新的状态
    /// @param _in boolbNotify ->是否发送状态改变通知, 缺省需要发送
    /// @author 黄丽云
    //------------------------------------------------------------------------------------
    void ChanageStatus(CONVERSATION_STATE nStatus, bool bNotify = true)
    {
        m_nStatus = nStatus;

        if (bNotify)
        {
            switch (m_nStatus)
            {
            case RS_RECONNECTING:
                m_pNetEndpointEventSink->OnEventHandle(shared_from_this(),
                    SL_WARNING,
                    SE_FAILT_TO_CONNECT);
                break;

            case RS_ESTABLISH:
                m_pNetEndpointEventSink->OnEventHandle(shared_from_this(),
                    SL_INFORMATION,
                    SE_CONNECTED);
                break;

            case RS_SENDING_HEART_PACKET:
                m_pNetEndpointEventSink->OnEventHandle(shared_from_this(),
                    SL_NOTICE,
                    SE_SWITCH_SENDING_HEART);
                break;

            case RS_CLOSE:
                m_pNetEndpointEventSink->OnEventHandle(shared_from_this(),
                    SL_INFORMATION,
                    SE_CLOSE_CONNECTION);
                break;

            case RS_INIT:
                break;

            default:
                assert(false);
            }
        }
    }
    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetEndpoint::Initial
    /// @brief 初始化会话对象，如果没有数据传输对象就分配一个，并把socket附加到数据传输对象
    ///
    /// @param _in SOCKET pSocket->一个指向套接字句柄的指针
    /// @return true->成功,false->失败
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------


    virtual bool Initial(int nFD = -1)
    {
        assert(nullptr == m_pTransfer);
        m_pTransfer = std::make_shared<TransferT>();

        if (nullptr != m_pTransfer)
        {
            m_objInputBuffer.Allocate(INTPUT_BUFFER_SIZE);

            if (-1 != nFD)
            {
                m_pTransfer->Attach(nFD);
            }
        }
        else
        {
            return false;
        }

        m_pTimer = std::make_shared<CTimer>();

        return (nullptr != m_pTimer);
    }


    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetClientImpl::Connect
    /// @brief 连接到指定的服务器
    ///
    /// @param _in unsigned int nIP->服务器的IP地址
    /// @param _in unsigned int nPort->服务器的端口
    /// @param _in int nTimeout->连接超时时间(缺省是30秒)
    /// @return  true: 成功, false: 失败
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    virtual bool Connect(unsigned int nIP, unsigned int nPort, int nTimeout/* = 30*/)
    {
        bool bResult = false;

        assert(m_pTransfer);

        if (m_pTransfer)
        {
            if (RS_ESTABLISH != GetStatus())
            {
                m_pTransfer->SetIP(nIP);
                m_pTransfer->SetPort(nPort);
                bResult = m_pTransfer->Connect(nTimeout);

                if (bResult)
                {
                    ChanageStatus(RS_ESTABLISH);
                }
                else
                {
                    ChanageStatus(RS_RECONNECTING);
                }
            }
        }

        return bResult;
    }

    // ********************************************************************************
    /// <summary>
    /// 发送原始数据
    /// </summary>
    /// <param name="pData">PacketT* pPacket->一个指向报文对象的指针</param>
    /// <param name="nLen"></param>
    /// <returns>true->成功,false->失败</returns>
    /// <created>Andy,2021/3/5</created>
    /// <changed>Andy,2021/3/5</changed>
    // ********************************************************************************
    virtual bool Send(const BYTE* pData,const unsigned int nLen)
    {
        bool bResult = false;

        assert(nullptr != m_pTransfer);

        if (RS_CLOSE != m_nStatus && nullptr != m_pTransfer)
        {
            const int nBytesSent = m_pTransfer->Send(pData, nLen);
        
            bResult = (nBytesSent == nLen);

            if (GetClient() && SOCKET_ERROR == nBytesSent)
            {
                Connect(m_pTransfer->GetIP(), m_pTransfer->GetPort(), 30);
                m_pNetEndpointEventSink->OnReconnectEventHandle(shared_from_this());
            }
        }

        return bResult;
    }

    //--------------------------------------------------------------------------------------------------------------------
//函数名称： CNetEndpoint::Send
/// @brief 发送一个报文
///
/// @param _in PacketT* pPacket->一个指向报文对象的指针
/// @return true->成功,false->失败
/// @author 黄丽云
//--------------------------------------------------------------------------------------------------------------------
    virtual bool Send(CNetPacket* pPacket)
    {
        bool bResult = Send(pPacket->GetHeaderPtr(), pPacket->GetTotalLength());

        return bResult;
    }
    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetEndpoint::Close
    /// @brief 关闭当前的会话对象，主要包括关闭数据传输对象并发送关闭事件
    ///
    /// @param _in bool bRude->是否是粗鲁的关闭
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    virtual void Close(bool bRude = false)
    {
        SetContinueReceivingData(false);

        if (m_pTransfer)
        {
            m_pTransfer->Close();
        }

        ChanageStatus(RS_CLOSE);
        m_pNetEndpointEventSink->OnCloseEventHandle(shared_from_this());
    }

public:
    //-------------------------------------------------------------------------------
    ///@brief 更改数据报类型
    ///
    ///用户修改数据报类型，使其适应多种数据报格式的提取
    ///@author Leiyz
    ///@date20170829
    //-------------------------------------------------------------------------------
    template<typename NewPacketT>
    void ChangePacketType()
    {
        m_ptrPacket = std::make_shared<NewPacketT>();
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetEndpoint::ReceiveData
    /// @brief  用于接收用户的数据
    ///
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    virtual void  ReceiveData()
    {
        NET_EXTRACT_DATA_PACKET_RESULT nExtractPacketResult = NEDPR_INVALID;

        if (nullptr == m_ptrPacket)
        {
            m_ptrPacket = std::make_shared<PacketT>();
        }

        if (GetClient())
        {
            m_objInputBuffer.Empty();

            while (GetContinueReceivingData())
            {
                assert(m_objInputBuffer.GetFreeSize() > 0);

                const int nBytesRecved = m_pTransfer->Recv(m_objInputBuffer.GetBufferFreeHeadPtr(), m_objInputBuffer.GetFreeSize());
                
                //ATLTRACE(("The ReceiveData is runing...\r\n"));
                
                if (nBytesRecved > 0)
                {
                    m_objInputBuffer.IncreaseDataLen(nBytesRecved);
                }
                else if (-2 == nBytesRecved)
                {
                    /*
                    并且切换到重新连接状态
                    */
                    ChanageStatus(RS_RECONNECTING);
                }
                else if (-3 == nBytesRecved)
                {
                    // 有可能数据包的长度超过了最大的数据包长度，直接废弃
                    m_objInputBuffer.Empty();
                    continue;
                }

                switch (GetStatus())
                {
                case RS_ESTABLISH:
                    if (nBytesRecved > 0)
                    {
                        do
                        {
                            nExtractPacketResult = m_ptrPacket->ExtractData(m_objInputBuffer);

                            if (NEDPR_MORE_PACKETS == nExtractPacketResult
                                || NEDPR_JUST_A_PACKET == nExtractPacketResult)
                            {
                                m_pNetEndpointEventSink->OnReceivedPacketEventHandle(shared_from_this(), (CNetPacket*)m_ptrPacket.get());
                                m_ptrPacket->ResetContent();
                            }
                        } while (NEDPR_MORE_PACKETS == nExtractPacketResult);

                        m_ptrPacket->ResetContent();

                        switch (nExtractPacketResult)
                        {
                        case NEDPR_INCOMPLETE_PACKET:
                            m_objInputBuffer.MoveDataForward();
                            break;

                        case NEDPR_INVALID:
                        {
                            /*
                            如果数据是无效的，将数据清空
                            */
                            m_objInputBuffer.Empty();
//                            LOG_WARN("invalid datapacket from ip :%s"
//                                , CLYUtility::Int2Ip(m_pTransfer->GetIP()).c_str());
                        }
                        break;

                            // 恢复指针到0的位置
                        default:
                            m_objInputBuffer.Empty();
                        }
                    }
                    else
                    {
                        m_pNetEndpointEventSink->OnEventHandle(
                            shared_from_this()
                            , SL_INFORMATION
                            , SE_CHECK_HEART);
                    }
                    break;

                case RS_RECONNECTING:      //重连
                    /*
                    在这里重复的判断是否继续的采集数据是为了能及时的退出线程
                    */
                    if (GetContinueReceivingData())
                    {
                        m_pTransfer->Close();   //说明会话已经关闭,立即关闭传输对象，
                        Connect(m_pTransfer->GetIP(), m_pTransfer->GetPort(), 30);
                        m_pNetEndpointEventSink->OnReconnectEventHandle(shared_from_this());

                        if (RS_RECONNECTING == GetStatus())
                        {
                            std::chrono::milliseconds objSleepDyratuib(2 * 1000);
                            std::this_thread::sleep_for(objSleepDyratuib);
                        }
                    }
                    break;
                }
            }
        }
        else
        {
            // 一定要枷锁，因为接收可能重复放入线程池，所以有并发可能
            std::lock_guard<std::mutex> objLocker(m_objRecevingMutex);
            int nBytesRecved = 0;

            while (true)
            {
                assert(m_objInputBuffer.GetFreeSize() > 0);
                nBytesRecved = m_pTransfer->Recv(m_objInputBuffer.GetBufferFreeHeadPtr(), m_objInputBuffer.GetFreeSize());

                if (nBytesRecved > 0)
                {
                    m_objInputBuffer.IncreaseDataLen(nBytesRecved);

                    const bool bDone = m_pNetEndpointEventSink->OnProReceivedDataHandle(shared_from_this(), m_objInputBuffer);

                    if (bDone)
                    {
                        //数据已经被处理
                        m_objInputBuffer.Empty();
                        continue;
                    }

                    do
                    {
                        nExtractPacketResult = m_ptrPacket->ExtractData(m_objInputBuffer);

                        if (NEDPR_MORE_PACKETS == nExtractPacketResult
                            || NEDPR_JUST_A_PACKET == nExtractPacketResult)
                        {
                            m_pNetEndpointEventSink->OnReceivedPacketEventHandle(shared_from_this(), (CNetPacket*)m_ptrPacket.get());
                            m_ptrPacket->ResetContent();
                        }
                    } while (NEDPR_MORE_PACKETS == nExtractPacketResult);

                    m_ptrPacket->ResetContent();


                    switch (nExtractPacketResult)
                    {
                    case NEDPR_INCOMPLETE_PACKET:
                        m_objInputBuffer.MoveDataForward();
                        break;

                    case NEDPR_INVALID:
                        /*
                        如果数据是无效的，立即关闭当前的会话对象
                        */
                            
//                        LOG_WARN("NEDPR_INVALID");
                        printf("NEDPR_INVALID");
                        m_objInputBuffer.Empty();
                        Close();
                        break;


                        // 恢复指针到0的位置
                    default:
                        m_objInputBuffer.Empty();
                    }
                }
                else if (-2 == nBytesRecved)
                {
                    /*注意执行函数OnCloseEventHandle后，不能在引用任何类成员变量
                    因为它将释放类的空间
                    */
                    m_pNetEndpointEventSink->OnCloseEventHandle(shared_from_this());
                    break;
                }
                else if (-3 == nBytesRecved)
                {
                    // 有可能数据包的长度超过了最大的数据包长度，直接废弃
                    m_objInputBuffer.Empty();
                }
                else
                {
                    break;
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetEndpoint::ReceivingThead
    /// @brief  线程函数，用于接收用户的数据
    ///
    /// @param _in pParam ->一个指向网络端点的指针
    /// @return  当前始终返回0
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    unsigned int ReceivingThead()
    {
        SetContinueReceivingData(true);
        ReceiveData();

        return 0;
    }
public:
        // --------------------------------------------------------------------------------
    /// <summary>
    /// 增加一个等待任务
    /// </summary>
    /// <param name="nPacket">序号等待的数据包</param>
    /// <returns>一个等待任务对象</returns>
    /// <created>黄丽云,2019/3/1</created>
    /// <changed>黄丽云,2019/3/4</changed>
    // --------------------------------------------------------------------------------
    std::shared_ptr<CWaitingResponseTask<PacketT>> AppendWaitingResponseTask(
        const int nPacket
        , const int nTimeout = 5 * 1000)
    {
        std::shared_ptr<CWaitingResponseTask<PacketT>> objTask = std::make_shared<CWaitingResponseTask<PacketT>>(nPacket, nTimeout);

        if (objTask != nullptr)
        {
            std::lock_guard<std::mutex> objAutoLock(m_csWaitTaskLock);

            m_objWaitingTaskList[nPacket] = objTask;

        }

        return objTask;
    }


        // --------------------------------------------------------------------------------
        /// <summary>
        /// 等待指定的应答包
        /// </summary>
        /// <param name="objTask">需要等待的任务</param>
        /// <param name="nTimeOut">等待超时</param>
        /// <returns>应答数据包</returns>
        /// <created>黄丽云,2019/3/1</created>
        /// <changed>黄丽云,2019/3/4</changed>
        // --------------------------------------------------------------------------------
    std::shared_ptr<PacketT> WaitAck(
                                     std::shared_ptr<CWaitingResponseTask<PacketT>> objTask)
    {
        std::shared_ptr<PacketT> objResult = nullptr;
        
        if (objTask != nullptr)
        {
            if (objTask->Wait())
            {
                objResult = objTask->GetPacket();
                
                {
                    std::lock_guard<std::mutex> objAutoLock(m_csWaitTaskLock);
                    auto itr = m_objWaitingTaskList.find(objTask->GetPacketID());
                    m_objWaitingTaskList.erase(itr);
                }
            }
            else
            {
                objTask->SetDiscard(true);
            }
        }
        
        return objResult;
    }
    
    
    

        void ActivateWaitingTask(CNetPacket* pPacket)
        {
            std::lock_guard<std::mutex> objAutoLock(m_csWaitTaskLock);
            auto itr = m_objWaitingTaskList.begin();

            while (m_objWaitingTaskList.end() != itr)
            {
                std::shared_ptr<CWaitingResponseTask<PacketT>> objWaitingTask = (*itr).second;

                if (objWaitingTask->GetDiscard())
                {
                    itr = m_objWaitingTaskList.erase(itr);
                }
                else
                {
                    if (objWaitingTask->GetPacketID() == pPacket->GetPacketId())
                    {
                        objWaitingTask->SetPacket((PacketT*)pPacket);
                        objWaitingTask->Notify();
                    }

                    itr++;
                }
            }
        }
private:   //下面的代码是禁止该类的拷贝构造函数和赋值操作
    CNetEndpoint(const CNetEndpoint& rPacketIoManager) {}
    CNetEndpoint& operator=(const CNetEndpoint& rPacketIoManager) { return *this; }

protected:
    
    
    CDataStreamBuffer             m_objInputBuffer{ INIT_INPUT_BUFFE_SIZE };                    ///< 存放接收数据的缓冲区
    

    DECLARE_MEMBER_AND_METHOD_V11(CONVERSATION_STATE, m_nStatus, Status, RS_INIT);                             ///< 当前会话状态
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bContinueReceivingData, ContinueReceivingData, true);            ///< 连续的采集数据
    DECLARE_MEMBER_AND_GET_METHOD_V11(CNetEndpointEventSink*, m_pNetEndpointEventSink, NetEndpointEventSink, nullptr)               ///< 事件回调接口
    DECLARE_MEMBER_AND_GET_METHOD_V11(CTimerPtr, m_pTimer, CTimer, nullptr)               ///< 事件回调接口
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bClient, Client, true)               ///< 是否是客户端.
    

private :
    
    DECLARE_MEMBER_AND_GET_METHOD_V11(std::shared_ptr<TransferT>, m_pTransfer, Transfer, nullptr)                    ///< 数据传输对象执政
    //std::mutex                           m_csLock;                             ///< l临界区对象,确保接收缓冲区中的数据被成功处理，采集接收新的数据
    std::atomic<int>         m_nRefCount{ 0 };                 ///< 引用计数
    std::shared_ptr<CNetPacket> m_ptrPacket;

    std::mutex                m_csWaitTaskLock;                             ///< l临界区对象
    std::map<unsigned int, std::shared_ptr<CWaitingResponseTask<PacketT>>> m_objWaitingTaskList;  ///< 等待任务列表
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bInThreadPool, InThreadPool, false)               ///< 当前的端点是否在    线程池中

    std::mutex                m_objRecevingMutex;                             ///< 接收数据线程锁

};



//=============================================
/// @brief 可靠的通信端点类
///
/// 下面的类是一个可靠会话模板类，有心跳功能，并且能保证数据包百分之百正确的到达目标主机\n
/// 适应于服务器端和客户端
//=============================================
//class CNetTCPDataTransfer;
//
//template <typename PacketT,  class TransferT = CNetTCPDataTransfer>
//class CNetReliableEndpoint : public CNetEndpoint<PacketT,  TransferT>
//    , public std::enable_shared_from_this<CNetEndpointInterface>
//{
//public:
//    CNetReliableEndpoint(CNetEndpointEventSink* pNetConversationEventSink = nullptr)
//        : CNetEndpoint<PacketT,  TransferT>(pNetConversationEventSink)
//    {
//    }
//
//    virtual ~CNetReliableEndpoint()
//    {
//        Clear();
//    }
//
//public:
//    /*--------------------------------------------------------------------------------------------------------------------
//    //函数名称： SetHeartDelay
//    /// @brief 设置心跳间隔时间(单位：毫秒)
//    ///
//    /// @param _in unsigned int dwHeartDelay->心跳间隔时间
//    /// @author 黄丽云
//    *-------------------------------------------------------------------------------------------------------------------*/
//    void SetHeartDelay(unsigned int dwHeartDelay) { m_dwHeartDelay = dwHeartDelay; }
//public:
//
//    //--------------------------------------------------------------------------------------------------------------------
//    //函数名称： CNetClientImpl::Connect
//    /// @brief 连接到指定的服务器
//    ///
//    /// @param _in unsigned int nIP->服务器的IP地址
//    /// @param _in unsigned int nPort->服务器的端口
//    /// @param _in int nTimeout->连接超时时间(缺省是30秒)
//    /// @return  bool true: 成功, false: 失败
//    /// @author 黄丽云
//    //--------------------------------------------------------------------------------------------------------------------
//    virtual bool Connect(unsigned int nIP, unsigned int nPort, int nTimeout = 30)
//    {
//        std::lock_guard<std::mutex>   objAutoLock(m_objCritSec);//必须如此，该函数可能并发调用
//
//        return CNetEndpoint<PacketT,  TransferT>::Connect(nIP, nPort, nTimeout);
//    }
//
//
//    //--------------------------------------------------------------------------------------------------------------------
//    //函数名称：Resend
//    /// @brief  当重新连接后重新发送未发送队列和已发送队列中的数据包
//    ///
//    /// @param _in  bNeedTime->是否根据时间来判断重发
//    /// @author 黄丽云
//    //--------------------------------------------------------------------------------------------------------------------
//    void Resend(bool bNeedTime)
//    {
//        bool bResult = false;
//        std::lock_guard<std::mutex>   objAutoLock(m_objCritSec);
//
//        const unsigned int CURRENT_TIME = GetCurrentTime();
//
//        PACKETS_RECORD_LIST_ITR it = m_listSent.begin();
//        while (it != m_listSent.end())
//        {
//            CSentPacketRecord* pPacketRecord = *it;
//
//            if (true == bNeedTime)
//            {
//                //if (CURRENT_TIME - pPacketRecord->GetSentTime() < RESEND_TIME_OUT)
//                if (RESEND_TIME_OUT > CURRENT_TIME - pPacketRecord->GetSentTime())
//                {
//                    it++;
//                    continue;
//                }
//            }
//
//            if (CNetEndpoint<PacketT,  TransferT>::GetNetEndpointEventSink()->OnPreResendPacketEventHandle(shared_from_this(), pPacketRecord->GetDataPacket()))
//            {
//                bResult = CNetEndpoint<PacketT,  TransferT>::Send((PacketT*)pPacketRecord);
//                if (bResult)
//                {
//                    pPacketRecord->SetSentTime(CURRENT_TIME);
//                }
//                else
//                {
//                    CNetEndpoint<PacketT,  TransferT>::ChanageStatus(RS_RECONNECTING);
//                    return;
//                }
//
//                it++;
//            }
//            else
//            {
//                delete pPacketRecord;
//                it = m_listSent.erase(it);
//            }
//        }
//
//
//        for (it = m_listUnSend.begin(); it != m_listUnSend.end();)
//        {
//            CSentPacketRecord* pPacketRecord = *it;
//
//            if (CNetEndpoint<PacketT,  TransferT>::GetNetEndpointEventSink()->OnPreResendPacketEventHandle(shared_from_this(), pPacketRecord->GetDataPacket()))
//            {
//                bResult = CNetEndpoint<PacketT,  TransferT>::Send((PacketT*)pPacketRecord->GetDataPacket());
//
//                if (bResult)
//                {
//                    it = m_listUnSend.erase(it);
//                    pPacketRecord->SetSentTime(CURRENT_TIME);
//                    AppendPacketRecordIntoSentQuen(pPacketRecord);
//                }
//                else
//                {
//                    CNetEndpoint<PacketT,  TransferT>::ChanageStatus(RS_RECONNECTING);
//                    return;
//                }
//            }
//            else
//            {
//                delete pPacketRecord;
//                it = m_listUnSend.erase(it);
//            }
//        }
//    }
//
//protected:
//    //--------------------------------------------------------------------------------------------------------------------
//    //函数名称： CNetReliableEndpoint::Clear
//    /// @brief  清除三个队列并删除队列中的节点
//    ///
//    /// @author 黄丽云
//    //--------------------------------------------------------------------------------------------------------------------
//    void Clear()
//    {
//        std::lock_guard<std::mutex>   objAutoLock(m_objCritSec);
//
//        PACKETS_LIST_ITR itrPacket = m_listRecv.begin();
//        for (; itrPacket != m_listRecv.end(); ++itrPacket)
//        {
//            delete(*itrPacket);
//        }
//        m_listRecv.clear();
//
//        PACKETS_RECORD_LIST_ITR itrPacketRecord = m_listUnSend.begin();
//        for (; itrPacketRecord != m_listUnSend.end(); ++itrPacketRecord)
//        {
//            delete(*itrPacketRecord);
//        }
//        m_listUnSend.clear();
//
//        itrPacketRecord = m_listSent.begin();
//        for (; itrPacketRecord != m_listSent.end(); ++itrPacketRecord)
//        {
//            delete(*itrPacketRecord);
//        }
//        m_listSent.clear();
//    }
//
//
//
//
//
//
//    //--------------------------------------------------------------------------------------------------------------------
//    //函数名称：AppendPacketIntoSentQuen
//    /// @brief  把数据包发送信息增加到已发送队列
//    ///
//    /// @param _in CSentPacketRecord* pPacketRecord->一个指向数据包对象的指针
//    /// @author 黄丽云
//    //--------------------------------------------------------------------------------------------------------------------
//    void AppendPacketRecordIntoSentQuen(CSentPacketRecord* pPacketRecord)
//    {
//        assert(pPacketRecord);
//
//        if (m_listSent.size() > LIST_MAX)
//        {
//            CSentPacketRecord* pPacketRecord = m_listSent.front();
//            m_listSent.pop_front();
//            delete pPacketRecord;
//        }
//        m_listSent.push_back(pPacketRecord);
//    }
//
//
//    unsigned int GetCurrentTime()
//    {
//        using MICROSECONDS_TYPE = std::chrono::duration<double, std::ratio<1, 1000>>;
//        std::chrono::time_point<std::chrono::system_clock, MICROSECONDS_TYPE> t1
//            = std::chrono::time_point_cast<MICROSECONDS_TYPE>(std::chrono::system_clock::now());
//        return t1.time_since_epoch().count();
//    }
//
//protected:
//    unsigned int CreateID() { return ++m_nNextID; }
//
//private:
//    PACKETS_LIST                  m_listRecv;                      ///< 接收队列，提取以后删除
//    PACKETS_RECORD_LIST     m_listSent;                      ///< 已发送队列,收到应答数据报以后删除
//    PACKETS_RECORD_LIST     m_listUnSend;                  ///< 未发送队列，发送后删除
//
//
//    unsigned int                               m_dwHeartDelay{ 30 * 1000 }
//    ;            ///< 心跳延迟
//    unsigned int                               m_dwLastTimeReceviedPacket{ 0 }
//    ;   ///< 最后收到数据包的时间,用于发送心跳数据包
//    unsigned int                               m_uNoResponse{ 0 }
//    ;             ///< 无应答次数
//    std::lock_guard<std::mutex> m_objCritSec;                 ///< 会话链表临界区
//
//    unsigned int                               m_nNextID{ 0 }
//    ;     /// < 下一个数据包ID编号
//};
//

#endif

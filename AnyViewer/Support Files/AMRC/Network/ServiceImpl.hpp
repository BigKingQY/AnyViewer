//
//  ServiceImpl.hpp
//  AnyViewer
//
//  Created by Aomei on 2021/8/31.
//

#ifndef ServiceImpl_hpp
#define ServiceImpl_hpp

#include "TypeDefine.h"
#include "PacketBase.hpp"
#include "Timer.hpp"
#include "CLYUtility.hpp"
#include "CLogger.h"
#include "CCustomThreadPool.h"

#include <memory>
#include <list>
#include <map>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <string>
#include <sstream>
#include <memory>
#include <chrono>             // std::chrono::seconds

#define LOG_MODULE pcpp::UndefinedLogModule

#define MAX_USER  1000                                  ///< 服务器最大的并发连接数
#define LIST_MAX 1024                                     ///< 队列最大长度

#define RESEND_TIME_OUT 20 * 1000                ///< 重发超时时间20秒
#define SENDING_TIME_OUT      5                              ///< 发送超时5秒
#define RECEIVING_TIME_OUT    2                             ///< 接收超时2秒

#define SAX2_PACKET_HEADER_MAGIC     0XFCFDFEFF   ///< 消息包头标志
#define SAX2_PACKET_VERSION  1                              ///< 当前数据包格式的版本号
#define INIT_INPUT_BUFFE_SIZE 1024 * 10                    ///< 存放接收数据的缓冲区的最大长度

#include <sys/socket.h>
//#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define CONCURRENCY_CONNECTIONS_MAX 10240       ///< 最大的并发连接数
#define EPOLL_MAX 128                                              ///< 单次可以处理的最大连接数
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define M_LISTEN_MAX 1024
#define DEFAULT_POLL_TIMEOUT 5000
#define DEFAULT_KEEP_ALIVE_TIMEOUT 300    ///<单位秒


///   警告消息枚举定义
///
///    该枚举定义了警告消息的严重程度类型\n
#ifndef SEVERITY_LEVEL_
#define SEVERITY_LEVEL_
enum SEVERITY_LEVEL
{
    SL_INFORMATION = 0,       ///< 枚举，一般提示消息
    SL_NOTICE,                      ///< 枚举，需要引起注意
    SL_WARNING,                   ///< 枚举，警告消息
    SL_CRITICAL                    ///< 枚举，严重错误
};
#endif

///    服务器事件枚举定义
///
///    该枚举定义了服务器事务对象可能收到的事件类型\n
enum  SERVICE_EVENT
{
    SE_UNKNOWN = 0,
    SE_FAILT_TO_CREATE_SOCKET,      ///< 枚举，创建socket失败
    SE_FAILT_TO_BING,                      ///< 枚举，绑定socket失败
    SE_FAILT_TO_LISTEN,                   ///< 枚举，启动侦听错误
    SE_FAILT_TO_ACCEPT,                     ///< 枚举，接收客户端的连接请求失败
    SE_FAILT_TO_GET_DEST_HOST_IP,    ///< 枚举，得到目标主机的IP地址失败
    SE_FAILT_TO_CONNECT,                  ///< 枚举，连接服务器失败
    SE_FAILT_TO_WSASTARTUP,


    SE_CONNECTED,                       ///< 枚举，连接服务器成功
    SE_SWITCH_SENDING_HEART,    ///< 枚举，切换到发送心跳数据包状态
    SE_CLOSE_CONNECTION,            ///< 枚举，切换到关闭会话连接状态
    SE_RECV_COMPLETE,              ///<接收数据完成
    SE_CHECK_HEART,
};

///    服务器状态枚举定义
///
///    该枚举定义了服务器事务对象可能处于的状态类型\n
enum NET_TRANSACTION_STATUS
{
    SS_INVALID = 0,               ///< 枚举，无效状态
    SS_INITIALIZED,               ///< 枚举，已经成功初始化
    SS_RUNING,                     ///< 枚举，正在运行状态
    SS_STOP,                         ///< 枚举，停止状态
};



///   会话状态结构枚举定义
///
///    该枚举定义通信会话可能的状态类型\n
enum CONVERSATION_STATE
{
    RS_INIT = 1,       ///< 枚举，初始状态
    RS_ESTABLISH,            ///< 枚举，通信已经建立状态
    RS_SENDING_HEART_PACKET,  ///< 枚举，正在发送心跳数据包状态
    RS_RECONNECTING,     ///< 枚举，重新连接状态
    RS_CLOSE,                  ///< 枚举，关闭状态
};

///   数据包标志枚举定义
///
///    该枚举定义了常见的数据包标志类型\n
enum PACKET_FLAG
{
    PF_ACK = 1,                               ///< 枚举，应答数据包标志
    PF_HEARTBEAT = 2,                    ///< 枚举，心跳数据包
};

#define CAX3NETPACKET_ID 934

/*所有的消息都有相同的消息头,定义如下,整个消息头包含6个域：
1: 包头识别码'0XFCFDFEFF'
2: 消息唯一的ID
3: 版本号
4: 标志参考类型定义PACKET_FLAG
5: 消息所携带的载荷数据长度
6: 校验和.
----------------------------
|                          |
|                          |
| 消息头                |
-----------------------------
|                          |
| (可选数据)           |
----------------------------*/
/*------------------------消息头定义部分---------------------------------------------*/


#pragma pack(1)
/// @brief 数据包头
///
/// 专用的数据包头,用于可靠的数据通信
typedef struct  _sax2_packet_header
{
    U32                  nMagic;                  ///< 包头识别码'0xFFFEFDFC'
    U32                  nID;                      ///< 消息ID
    BYTE                nVersion;               ///< 版本号，当前是1
    BYTE                nFlags;                  ///< 标志， 前4位表示版本，紧接着4位是
    U32                  nChecksum;           ///< 校验和，采用与TCP相同计算方法，需要计算的数据包括包头和载荷数据
    U32                  nPayloadLen;          ///< 携带的数据长度
    U16                  nPaddingLen;          ///< 填充数据长度的长度
}SAX2_PACKET_HEADER;
#pragma pack()





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

//======================================================
/// @brief IP数据传输类
///
/// IP数据传输接口，该接口只针对IP家族的协议(比如: TCP & UDP),因为整个通信框架是假定
/// 基于TCP/IP通信的，这也是本通信框架的约束，应此也限制了数据传输对像的类型
//======================================================
class CNetIPDataTransfer
{
public:
    CNetIPDataTransfer(unsigned int nIP = 0, unsigned int nPort = 0)
        : m_nIP(nIP)
        , m_nPort(nPort)
    {
    }
public:


public:
    /// @brief 连接到指定的目标主机
    ///
    /// @param nTimeout 等待超时设置，函数等待的最长时间，一旦超时函数将立即返回,缺省是30秒
    /// @return  true: 成功，false:失败
    virtual bool Connect(int nTimeout = 30) = 0;

    /// @brief 发送数据
    ///
    /// @param pBuffer 一个指向被发送数据缓冲区的指针
    /// @param nLen 被发送数据的长度
    /// @return  成功发送数据的长度
    virtual int Send(const BYTE* pBuffer, int nLen) = 0;

    /// @brief 接收数据
    ///
    /// @param pBuffer 一个指向接收数据缓冲区的指针
    /// @param nLen 接收数据缓冲区的长度
    /// @return  成功接收数据的长度
    virtual int Recv(BYTE* pBuffer, int nLen) = 0;

    /// @brief 关闭数据传输对象
    virtual void Close() = 0;

    /// @brief The shutdown function disables sends or receives on a socket.
    ///
    /// @param how Flag that describes what types of operation will no longer be allowed.
    /// @return  If no error occurs, shutdown returns zero. Otherwise, a value of SOCKET_ERROR
    ///             is returned, and a specific error code can be retrieved by calling WSAGetLastError.
    virtual bool Shutdown(int how) = 0;

public:

protected:
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nIP, IP,0)                      ///< 目标地址
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nPort, Port,0)            ///< 目标端口
};

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


class CAx3NetPacket;


//=============================================
/// @brief 专用的数据包
///
/// 内部专用的数据包, 用于可靠的传输通信
//=============================================
class CAx3NetPacket : public CNetPacketBase
{
public:
    /// @brief 构造函数
    CAx3NetPacket();

    /// @brief 构造函数
    CAx3NetPacket(unsigned int nID, BYTE nFlags, const BYTE* pPayload, unsigned int nLength);

    /// @brief 析构函数
    ~CAx3NetPacket();

    /// @brief 启用对象缓冲池功能，初始化128个对象，每次分配32个
//    USE_CLASS_MEMORY_POOL(CAx3NetPacket, 128, 32);

public:
    /// @brief 得到数据包ID
    unsigned int GetID();

    /// @brief 得到数据包版本信息
    BYTE GetVersion();

    /// @brief 得到数据包标志信息
    BYTE GetFlags();

    /// @brief 得到数据包携带的有效数据的长度(=载荷数据的长度 - 填充数据的长度)
    unsigned int GetDataLen();

    /// @brief 得到数据包携带的校验和
    unsigned int GetChecksum();

    /// @brief 得到载荷数据的长度
    BYTE* GetPayloadPtr();

public:

    /// @brief 从数据流中提取数据包
    virtual NET_EXTRACT_DATA_PACKET_RESULT ExtractData(CDataStreamBuffer& refDataBuffer);

public:
    /// @brief 计算数据包CRC32值
    unsigned int CalculateCRC32();

    /// @brief 校验当前数据包的CRC32值是否合法
    bool VerifyCRC32();
};


//=============================================
/// @brief TCP数据传输对象
///
/// 该对象负责采用TCP协议通信的操作
/// @note 仅仅使用于linux平台
//=============================================

class CNetTCPDataTransfer : public CNetIPDataTransfer
{
public:

    /// @brief 构造函数
    CNetTCPDataTransfer();

    /// @brief 构造函数
    CNetTCPDataTransfer(unsigned int nIP, unsigned int nPort);

    /// @brief 析构函数
    virtual ~CNetTCPDataTransfer();


public:
    int GetSocketHandle() { return m_nFD; }

    virtual bool Initial(bool /*bClient*/) { return true; }

    /// @brief 连接到指定的目标主机
    virtual bool Connect(int nTimeout = 30);

    /// @brief 发送数据， 返回实际发送大小
    virtual int Send(const BYTE* pBuffer, int nLen);

    /// @brief 接收数据， 返回实际接收大小
    virtual int Recv(BYTE* pBuffer, int nLen);

    /// @brief 关闭当前的传输对象
    virtual void Close();

    /// @brief 关闭当前的传输对象
    virtual bool Shutdown(int how);

    bool EnableBlockSocket(bool bEnable);

public:

    /// @brief 关联当前的传输对象到指定的Socket
    virtual bool Attach(int nFD);

    /// @brief 解除关联
    virtual void Detach();

private:   //下面的代码是禁止该类的拷贝构造函数和赋值操作
    CNetTCPDataTransfer(const CNetTCPDataTransfer& /*rPacketIoManager*/) = delete;
    CNetTCPDataTransfer& operator=(const CNetTCPDataTransfer& /*rPacketIoManager*/) = delete;

private:
    /// @brief 查询Socket的状态
    bool QuerySocketStatus(timeval& refTimeout, bool bRead, int* pIOReady = nullptr);

    bool IgnoreError(int nErrCode);
private:
    DECLARE_MEMBER_AND_METHOD_V11(int, m_nFD, FD,-1)                                          ///< socket套接字ID
    DECLARE_MEMBER_AND_METHOD_V11(int, m_nRecvWaitingTime, RecvWaitingTime,0)  ///< 接收数据等待时间
    DECLARE_MEMBER_AND_METHOD_V11(bool,m_bIsClient, IsClient,false);            ///< 是否是客户端
};


//**********************************************************************
/// @brief UDP数据传输对象
///
/// 该对象负责采用UDP协议通信的操作
//**********************************************************************

class CNetUDPDataTransfer : public CNetIPDataTransfer
{
public:

    /// @brief 构造函数
    CNetUDPDataTransfer();

    /// @brief 构造函数
    CNetUDPDataTransfer(unsigned int nIP, USHORT nPort);

    /// @brief 析构函数
    virtual ~CNetUDPDataTransfer();

public:
    /// @brief 设置目标主机的IP地址
    ///
    /// @param nIP 目标主机的IP地址.
    virtual void SetIP(unsigned int nIP);

    /// @brief 设置目标主机的端口号
    ///
    /// @param nPort 目标主机的端口号.
    virtual void SetPort(USHORT nPort);

public:


    /// @brief 连接到指定的目标主机
    virtual bool Connect(int nTimeout = 30);

    /// @brief 发送数据， 返回实际发送大小
    virtual int Send(const BYTE* pBuffer, int nLen);

    /// @brief 接收数据， 返回实际接收大小
    virtual int Recv(BYTE* pBuffer, int nLen);

    /// @brief 关闭当前的传输对象
    virtual void Close();

    /// @brief 关闭当前的传输对象
    virtual bool Shutdown(int how);


public:

    /// @brief 关联当前的传输对象到指定的Socket
    virtual bool Attach(int nFD);

    /// @brief 解除关联
    virtual void Detach();

private:   //下面的代码是禁止该类的拷贝构造函数和赋值操作
    CNetUDPDataTransfer(const CNetUDPDataTransfer& /*rPacketIoManager*/) {}
    CNetUDPDataTransfer& operator=(const CNetUDPDataTransfer& /*rPacketIoManager*/) { return *this; }

private:
    /// @brief 查询Socket的状态
    bool QuerySocketStatus(timeval& refTimeout, bool bRead, int* pIOReady = nullptr);

    bool EnableBlockSocket(bool bEnable);

    bool IgnoreError(int nErrCode);
private:
    int                       m_nFD;                 ///< 用于通信的socket
    int                       m_nRecvWaitingTime;  ///< 接收数据等待时间
};

//=============================================
/// @brief 通信端点事件接口
///
/// 事务对象与通信端点之间的事件接口，通信端点将调用该接口方法来通知事务对象，所有
/// 的事务对象也必须实现该接口
//=============================================
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

//=============================================
/// @brief 普通的通信端点类
///
/// 下面的类是一个简单通用会话模板类,该类有三个模板化的参数，分别是:
/// 1、整个通信所有使用的数据包类，用户需要根据实际情况自定义类，但是必须从CNetPacket派生
/// 2、定义该通信端点是属于客户端还是服务端，默认是客户端
/// 3、定义该通信所使用数据传输类，默认是"CNetTCPDataTransfer", 用户可以根据自己的需要
/// 自己定义，但必须从类"CNetIPDataTransfer"派生
//=============================================

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
                            LOG_WARN("invalid datapacket from ip :%s"
                                , CLYUtility::Int2Ip(m_pTransfer->GetIP()).c_str());
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
                            
//                            std::this_thread::sleep_for(objSleepDyratuib);
    
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
                        LOG_WARN("NEDPR_INVALID");
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
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bClient, Client, true)               ///< 是否是客户端
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
/// @brief 数据包发送记录
///
//=============================================
class CSentPacketRecord
{
public:
    CSentPacketRecord(CAx3NetPacket* pDataPacket, unsigned int nSentTime = 0)
        : m_pDataPacket(pDataPacket)
        , m_nSentTime(nSentTime)
    {
    }
    ~CSentPacketRecord()
    {
        if (m_pDataPacket)
        {
            delete m_pDataPacket;
            m_pDataPacket = nullptr;
        }
    }

public:
    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： GetDataPacket
    /// @brief  放回当前发送记录关联的数据包对象指针
    ///
    /// @return  一个指向数据包对象的指针
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    CAx3NetPacket* GetDataPacket() { return m_pDataPacket; }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： SetSentTime
    /// @brief  设置数据包的发送时间
    ///
    /// @param _in unsigned int nTime->数据包的发送时间
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    void SetSentTime(unsigned int nTime) { m_nSentTime = nTime; }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： GetSentTime
    /// @brief  返回数据包的发送时间
    ///
    /// @return   数据包的发送时间
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    unsigned int  GetSentTime() { return m_nSentTime; }

private:
    CAx3NetPacket*           m_pDataPacket;               ///< 已经发送的数据包指针
    unsigned int                          m_nSentTime;                 ///< 发送时间，用于计算重发
};

typedef std::list<CNetPacket*>                              PACKETS_LIST;   ///< 数据包链表
typedef std::list<CNetPacket*>::iterator                   PACKETS_LIST_ITR;

typedef std::list<CSentPacketRecord*>                    PACKETS_RECORD_LIST; ///< 数据包记录链表
typedef std::list<CSentPacketRecord*>::iterator         PACKETS_RECORD_LIST_ITR;


//=============================================
/// @brief 可靠的通信端点类
///
/// 下面的类是一个可靠会话模板类，有心跳功能，并且能保证数据包百分之百正确的到达目标主机\n
/// 适应于服务器端和客户端
//=============================================
template <typename PacketT,  class TransferT = CNetTCPDataTransfer>
class CNetReliableEndpoint : public CNetEndpoint<PacketT,  TransferT>
    , public std::enable_shared_from_this<CNetEndpointInterface>
{
public:
    CNetReliableEndpoint(CNetEndpointEventSink* pNetConversationEventSink = nullptr)
        : CNetEndpoint<PacketT,  TransferT>(pNetConversationEventSink)
    {
    }

    virtual ~CNetReliableEndpoint()
    {
        Clear();
    }

public:
    /*--------------------------------------------------------------------------------------------------------------------
    //函数名称： SetHeartDelay
    /// @brief 设置心跳间隔时间(单位：毫秒)
    ///
    /// @param _in unsigned int dwHeartDelay->心跳间隔时间
    /// @author 黄丽云
    *-------------------------------------------------------------------------------------------------------------------*/
    void SetHeartDelay(unsigned int dwHeartDelay) { m_dwHeartDelay = dwHeartDelay; }
public:

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetClientImpl::Connect
    /// @brief 连接到指定的服务器
    ///
    /// @param _in unsigned int nIP->服务器的IP地址
    /// @param _in unsigned int nPort->服务器的端口
    /// @param _in int nTimeout->连接超时时间(缺省是30秒)
    /// @return  bool true: 成功, false: 失败
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    virtual bool Connect(unsigned int nIP, unsigned int nPort, int nTimeout = 30)
    {
        std::lock_guard<std::mutex>   objAutoLock(m_objCritSec);//必须如此，该函数可能并发调用

        return CNetEndpoint<PacketT,  TransferT>::Connect(nIP, nPort, nTimeout);
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetEndpoint::Send
    /// @brief 发送一个报文,该函数需要把被发送的报文转换为专用的报文格式，然后再发送出去
    ///
    /// @param _in CNetPacket* pPacket->一个指向报文对象的指针
    /// @return true->成功,false->失败
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    virtual bool Send(CNetPacket* pPacket)
    {
        bool bResult = false;

        assert(pPacket);
        if (pPacket)
        {
            if (!CNetEndpoint<PacketT,  TransferT>::GetNetEndpointEventSink()->OnPreSendPacketEventHandle(shared_from_this(), pPacket))
            {
                delete pPacket;   //直接删除

                return bResult;
            }

            std::lock_guard<std::mutex>   objAutoLock(m_objCritSec);   //必须如此，该函数可能被并发调用
            CAx3NetPacket* pSax2Packet = new CAx3NetPacket(CreateID(), 0, pPacket->GetHeaderPtr(), pPacket->GetTotalLength());
            assert(pSax2Packet);
            pSax2Packet->CalculateCRC32();

            if (RS_ESTABLISH != CNetEndpoint<PacketT,  TransferT>::GetStatus()
                && RS_SENDING_HEART_PACKET != CNetEndpoint<PacketT,  TransferT>::GetStatus())
            {
                AppendPacketIntoWaitingQuen(pSax2Packet);     //直接把数据包添加到等待发送队列
            }
            else
            {
                bResult = CNetEndpoint<PacketT,  TransferT>::Send((PacketT*)pSax2Packet);
                if (bResult) // 发送成功
                {
                    AppendPacketIntoSentQuen(pSax2Packet);        //只有成功发送的的请求数据包才增加到已经发送队列
                }
                else
                {
                    AppendPacketIntoWaitingQuen(pSax2Packet);     //直接把数据包添加到等待发送队列
                    CNetEndpoint<PacketT,  TransferT>::ChanageStatus(RS_RECONNECTING);
                }
            }
        }

        return bResult;
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称：Resend
    /// @brief  当重新连接后重新发送未发送队列和已发送队列中的数据包
    ///
    /// @param _in  bNeedTime->是否根据时间来判断重发
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    void Resend(bool bNeedTime)
    {
        bool bResult = false;
        std::lock_guard<std::mutex>   objAutoLock(m_objCritSec);

        const unsigned int CURRENT_TIME = GetCurrentTime();

        PACKETS_RECORD_LIST_ITR it = m_listSent.begin();
        while (it != m_listSent.end())
        {
            CSentPacketRecord* pPacketRecord = *it;

            if (true == bNeedTime)
            {
                //if (CURRENT_TIME - pPacketRecord->GetSentTime() < RESEND_TIME_OUT)
                if (RESEND_TIME_OUT > CURRENT_TIME - pPacketRecord->GetSentTime())
                {
                    it++;
                    continue;
                }
            }

            if (CNetEndpoint<PacketT,  TransferT>::GetNetEndpointEventSink()->OnPreResendPacketEventHandle(shared_from_this(), pPacketRecord->GetDataPacket()))
            {
                bResult = CNetEndpoint<PacketT,  TransferT>::Send((PacketT*)pPacketRecord);
                if (bResult)
                {
                    pPacketRecord->SetSentTime(CURRENT_TIME);
                }
                else
                {
                    CNetEndpoint<PacketT,  TransferT>::ChanageStatus(RS_RECONNECTING);
                    return;
                }

                it++;
            }
            else
            {
                delete pPacketRecord;
                it = m_listSent.erase(it);
            }
        }


        for (it = m_listUnSend.begin(); it != m_listUnSend.end();)
        {
            CSentPacketRecord* pPacketRecord = *it;

            if (CNetEndpoint<PacketT,  TransferT>::GetNetEndpointEventSink()->OnPreResendPacketEventHandle(shared_from_this(), pPacketRecord->GetDataPacket()))
            {
                bResult = CNetEndpoint<PacketT,  TransferT>::Send((PacketT*)pPacketRecord->GetDataPacket());

                if (bResult)
                {
                    it = m_listUnSend.erase(it);
                    pPacketRecord->SetSentTime(CURRENT_TIME);
                    AppendPacketRecordIntoSentQuen(pPacketRecord);
                }
                else
                {
                    CNetEndpoint<PacketT,  TransferT>::ChanageStatus(RS_RECONNECTING);
                    return;
                }
            }
            else
            {
                delete pPacketRecord;
                it = m_listUnSend.erase(it);
            }
        }
    }

public:

    /*--------------------------------------------------------------------------
    //函数名称：ExtractPacketfromBuffer
    /// @brief 从缓冲区中提取数据包
    ///
    /// @param _in CDataStreamBuffer& objDataBuffer->一个指向缓冲区对象的参考
    /// @return 成功返回提取数据包的个数
    /// @author 黄丽云
    *---------------------------------------------------------------------------*/
    unsigned int ExtractPacketfromBuffer(CDataStreamBuffer& objDataBuffer)
    {
        CAx3NetPacket objSax2NetPacket;
        PacketT objPacket;
        NET_EXTRACT_DATA_PACKET_RESULT nExtractPacketResult = NEDPR_INVALID;
        unsigned int nPackets = 0;

        do
        {
            nExtractPacketResult = objSax2NetPacket.ExtractData(CNetEndpoint<PacketT,  TransferT>::m_objInputBuffer);

            if (NEDPR_MORE_PACKETS == nExtractPacketResult
                || NEDPR_JUST_A_PACKET == nExtractPacketResult)
            {
                if (objSax2NetPacket.VerifyCRC32())
                {
                    if (objSax2NetPacket.GetFlags() & PF_HEARTBEAT == 0)   //不用处理心跳数据包
                    {
                        if (objSax2NetPacket.GetFlags() & PF_ACK)
                        {
                            /*
                            是应答数据包，不需要提交给用户处理，查找到被应答的数据包并将其删除掉
                            */
                            RemovePacketSent(objSax2NetPacket.GetID());
                        }
                        else
                        {
                            //如果发送应答数据包失败，只会导致对方重传数据包，应此没有处理失败的情况
                            SendAck(objSax2NetPacket.GetID());

                            objPacket.Copy(objSax2NetPacket.GetPayloadPtr(), objSax2NetPacket.GetDataLen());

                            CNetEndpoint<PacketT,  TransferT>::GetNetEndpointEventSink()->OnReceivedPacketEventHandle(this, (CNetPacket*)&objPacket);
                        }
                    }
                }

                objSax2NetPacket.ResetContent();
                nPackets++;
            }
        } while (NEDPR_MORE_PACKETS == nExtractPacketResult);

        if (NEDPR_INCOMPLETE_PACKET == nExtractPacketResult)
        {
            CNetEndpoint<PacketT,  TransferT>::m_objInputBuffer.MoveDataForward();
        }

        m_dwLastTimeReceviedPacket = GetCurrentTime();

        return nPackets;
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetReliableEndpoint::ReceiveData
    /// @brief  用于接收用户的数据
    ///
    /// @remarks :  受缓冲区的限制，数据包的长度不能大于1兆字节
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    virtual void ReceiveData()
    {
        CNetEndpoint<PacketT,  TransferT>::m_objInputBuffer.Empty();

        int nBytesRecved = 0;

        if (CNetEndpoint<PacketT, TransferT>::GetClient())
        {
            ;
        }
        else
        {
            nBytesRecved = CNetEndpoint<PacketT,  TransferT>::GetDataTransfer()->Recv(
                CNetEndpoint<PacketT,
                
                TransferT>::m_objInputBuffer.GetBufferFreeHeadPtr(),
                CNetEndpoint<PacketT,
                
                TransferT>::m_objInputBuffer.GetFreeSize());
            if (nBytesRecved > 0)
            {
                CNetEndpoint<PacketT,  TransferT>::m_objInputBuffer.IncreaseDataLen(nBytesRecved);
            }
            else if (-2 == nBytesRecved)
            {
                CNetEndpoint<PacketT,  TransferT>::Close();
            }

            switch (CNetEndpoint<PacketT,  TransferT>::GetStatus())
            {
            case RS_ESTABLISH:    // 正处于通信状态
            {
                if (nBytesRecved > 0)
                {
                    ExtractPacketfromBuffer(CNetEndpoint<PacketT,  TransferT>::m_objInputBuffer); //说明已经接受有数据，就立即提取数据包
                }
                else
                {
                    Resend(true);             //执行超时重新传输
                    unsigned int dwNowTime = GetCurrentTime();
                    if (dwNowTime - m_dwLastTimeReceviedPacket > m_dwHeartDelay)
                    {
                        if (SendHeartbeatPacket())
                        {
                            CNetEndpoint<PacketT,  TransferT>::ChanageStatus(RS_SENDING_HEART_PACKET);

                            m_uNoResponse = 1;
                        }
                        else
                        {
                            CNetEndpoint<PacketT,  TransferT>::Close();
                        }
                    }
                }
            }
            break;

            case RS_SENDING_HEART_PACKET:      // 发送心跳数据报
            {
                if (nBytesRecved > 0)
                {
                    //下面两行代码的顺序必须如此
                    CNetEndpoint<PacketT,  TransferT>::ChanageStatus(RS_ESTABLISH, false);           //说明服务器是正常的，立即切换到连接状态,不用发送通知,因为连接没有断开
                    ExtractPacketfromBuffer(CNetEndpoint<PacketT,  TransferT>::m_objInputBuffer);                //说明已经接受有数据，就立即提取数据包
                }
                else
                {
                    unsigned int dwDelay = m_uNoResponse * (5 * 1000);
                    unsigned int dwNowTime = GetCurrentTime();
                    if (dwNowTime - m_dwLastTimeReceviedPacket > dwDelay)
                    {
                        if (SendHeartbeatPacket())
                        {
                            if (++m_uNoResponse > 3)  //连续发3次心跳 仍然没有收到数据,跳转重连
                            {
                                CNetEndpoint<PacketT,  TransferT>::Close();
                            }
                        }
                        else
                        {
                            CNetEndpoint<PacketT,  TransferT>::Close();
                        }
                    }
                }
            }
            break;

            case RS_RECONNECTING:      //服务端不需要重连功能
            case RS_CLOSE:                //该状态什么都不做
                break;

            default:
                break;
            }
        }
    }

protected:


    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetReliableEndpoint::SendAck
    /// @brief  发送应答数据包
    ///
    /// @param _in unsigned int nID->被应答的数据包
    /// @return  bool true: 成功, false: 失败
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    bool SendAck(unsigned int nID)
    {
        CAx3NetPacket objACK(nID, PF_ACK, 0, 0);
        objACK.CalculateCRC32();

        const int nBytesSent = CNetEndpoint<PacketT,  TransferT>::GetDataTransfer()->Send((BYTE*)&objACK, objACK.GetTotalLength());

        return (nBytesSent == sizeof(SAX2_PACKET_HEADER));
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetReliableEndpoint::SendHeartbeatPacket
    /// @brief  发送心跳数据包
    ///
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    bool SendHeartbeatPacket()
    {
        CAx3NetPacket objHeartbeatPacket(CreateID(), PF_HEARTBEAT, 0, 0);
        objHeartbeatPacket.CalculateCRC32();

        const int nBytesSent = CNetEndpoint<PacketT,  TransferT>::GetDataTransfer()->Send((BYTE*)&objHeartbeatPacket, objHeartbeatPacket.GetTotalLength());

        return (nBytesSent == sizeof(SAX2_PACKET_HEADER));
    }

protected:
    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetReliableEndpoint::Clear
    /// @brief  清除三个队列并删除队列中的节点
    ///
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    void Clear()
    {
        std::lock_guard<std::mutex>   objAutoLock(m_objCritSec);

        PACKETS_LIST_ITR itrPacket = m_listRecv.begin();
        for (; itrPacket != m_listRecv.end(); ++itrPacket)
        {
            delete(*itrPacket);
        }
        m_listRecv.clear();

        PACKETS_RECORD_LIST_ITR itrPacketRecord = m_listUnSend.begin();
        for (; itrPacketRecord != m_listUnSend.end(); ++itrPacketRecord)
        {
            delete(*itrPacketRecord);
        }
        m_listUnSend.clear();

        itrPacketRecord = m_listSent.begin();
        for (; itrPacketRecord != m_listSent.end(); ++itrPacketRecord)
        {
            delete(*itrPacketRecord);
        }
        m_listSent.clear();
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称：LookupPacketinSentQuen
    /// @brief  根据包的序号在发送队列中查找数据包
    ///
    /// @param _in PacketT* pDataPacket->一个指向数据包对象的指针
    ///          必须主动的调用函数GetRecvPacket取提取应答包
    /// @return true->找到,false->未找到
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    bool LookupPacketinSentQuen(CAx3NetPacket* pDataPacket)
    {
        assert(pDataPacket);

        bool bResult = false;
        PACKETS_RECORD_LIST_ITR itr;

        std::lock_guard<std::mutex>   objAutoLock(m_objCritSec);

        for (itr = m_listSent.begin(); itr != m_listSent.end(); ++itr)
        {
            CSentPacketRecord* pPacketRecord = *itr;
            if (pDataPacket->GetID() == pPacketRecord->GetDataPacket()->GetID())
            {
                bResult = true;
                break;
            }
        }

        return bResult;
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称：RemovePacketSent
    /// @brief  根据包的序号在发送队列中删除数据包
    ///
    /// @param _in unsigned int nID>被删除数据包的ID
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    void RemovePacketSent(unsigned int nID)
    {
        PACKETS_RECORD_LIST_ITR itr;

        std::lock_guard<std::mutex>   objAutoLock(m_objCritSec);

        for (itr = m_listSent.begin(); itr != m_listSent.end(); ++itr)
        {
            CSentPacketRecord* pPacketRecord = *itr;
            if (nID == pPacketRecord->GetDataPacket()->GetID())
            {
                m_listSent.erase(itr);
                delete pPacketRecord;
                break;
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称：LookupPacketinWaitingQuen
    /// @brief  根据包的序号在等待队列中查找数据包
    ///
    /// @param _in PacketT* pDataPacket->一个指向数据包对象的指针
    ///          必须主动的调用函数GetRecvPacket取提取应答包
    /// @returnTRUE->找到,false->未找到
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    bool LookupPacketinWaitingQuen(CAx3NetPacket* pDataPacket)
    {
        assert(pDataPacket);

        bool bResult = false;
        PACKETS_RECORD_LIST_ITR itr;

        std::lock_guard<std::mutex>   objAutoLock(m_objCritSec);
        for (itr = m_listUnSend.begin(); itr != m_listUnSend.end(); ++itr)
        {
            CSentPacketRecord* pPacketRecord = *itr;
            if (pDataPacket->GetID() == pPacketRecord->GetDataPacket()->GetID())
            {
                bResult = true;
                break;
            }
        }

        return bResult;
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称：AppendPacketIntoSentQuen
    /// @brief  把数据包发送信息增加到已发送队列
    ///
    /// @param _in CSentPacketRecord* pPacketRecord->一个指向数据包对象的指针
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    void AppendPacketRecordIntoSentQuen(CSentPacketRecord* pPacketRecord)
    {
        assert(pPacketRecord);

        if (m_listSent.size() > LIST_MAX)
        {
            CSentPacketRecord* pPacketRecord = m_listSent.front();
            m_listSent.pop_front();
            delete pPacketRecord;
        }
        m_listSent.push_back(pPacketRecord);
    }
    //--------------------------------------------------------------------------------------------------------------------
    //函数名称：AppendPacketIntoSentQuen
    /// @brief  把当前的数据包添加到已发送队列
    ///
    /// @param _in CAx3NetPacket* pDataPacket->一个指向数据包对象的指针
    /// @param _in  HANDLE hEvent->事件句柄,如果句柄不为空，当收到服务器应答数据包的时候\n
    ///          会被设置为有信号
    /// @param _in  bool bNeedAck->是否需要ACK数据包,如果为true,那么hEvent不能为空，并且\n
    ///          必须主动的调用函数GetRecvPacket取提取应答包
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    void AppendPacketIntoSentQuen(CAx3NetPacket* pDataPacket)
    {
        if (!LookupPacketinSentQuen(pDataPacket)) //避免重复增加到队列
        {
            CSentPacketRecord* pSentPacket = new CSentPacketRecord(pDataPacket, GetCurrentTime());
            if (pSentPacket)
            {
                AppendPacketRecordIntoSentQuen(pSentPacket);
            }
        }
    }
    //--------------------------------------------------------------------------------------------------------------------
    //函数名称：AppendPacketIntoWaitingQuen
    /// @brief  把当前的数据包添加到等待发送队列
    ///
    /// @param _in CAx3NetPacket* pDataPacket->一个指向数据包对象的指针
    /// @param _in  HANDLE hEvent->事件句柄,如果句柄不为空，当收到服务器应答数据包的时候
    ///          会被设置为有信号
    /// @param _in  bool bNeedAck->是否需要ACK数据包,如果为true,那么hEvent不能为空，并且
    ///          必须主动的调用函数GetRecvPacket取提取应答包
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    void AppendPacketIntoWaitingQuen(CAx3NetPacket* pDataPacket)
    {
        if (!LookupPacketinWaitingQuen(pDataPacket))   //避免重复增加到队列
        {
            CSentPacketRecord* pTobeSentPacket = new CSentPacketRecord(pDataPacket, 0);
            if (pTobeSentPacket)
            {
                if (m_listUnSend.size() > LIST_MAX)
                {
                    CSentPacketRecord* pPacketRecord = m_listUnSend.front();
                    m_listUnSend.pop_front();
                    delete pPacketRecord;
                }

                m_listUnSend.push_back(pTobeSentPacket);
            }
        }
    }

    unsigned int GetCurrentTime()
    {
        using MICROSECONDS_TYPE = std::chrono::duration<double, std::ratio<1, 1000>>;
        std::chrono::time_point<std::chrono::system_clock, MICROSECONDS_TYPE> t1
            = std::chrono::time_point_cast<MICROSECONDS_TYPE>(std::chrono::system_clock::now());
        return t1.time_since_epoch().count();
    }

protected:
    unsigned int CreateID() { return ++m_nNextID; }

private:
    PACKETS_LIST                  m_listRecv;                      ///< 接收队列，提取以后删除
    PACKETS_RECORD_LIST     m_listSent;                      ///< 已发送队列,收到应答数据报以后删除
    PACKETS_RECORD_LIST     m_listUnSend;                  ///< 未发送队列，发送后删除


    unsigned int                               m_dwHeartDelay{ 30 * 1000 }
    ;            ///< 心跳延迟
    unsigned int                               m_dwLastTimeReceviedPacket{ 0 }
    ;   ///< 最后收到数据包的时间,用于发送心跳数据包
    unsigned int                               m_uNoResponse{ 0 }
    ;             ///< 无应答次数
    std::lock_guard<std::mutex> m_objCritSec;                 ///< 会话链表临界区

    unsigned int                               m_nNextID{ 0 }
    ;     /// < 下一个数据包ID编号
};


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
    //函数名称： CNetServiceImpl::InitService
    /// @brief  初始化当前的事务对象，主要的工作包括：1、一个epoll专用的文件描述符；
    /// 2、初始化socket库，然后在指定的端口开始侦听客户的连接请求。
    ///
    /// @return true->成功,false->失败
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    virtual bool Initial()
    {
        m_pTimerWhell = new CTimerManager;
        if (nullptr == m_pTimerWhell)
        {
            return false;
        }

        // 1、一个epoll专用的文件描述符
        
//        m_nEpollFD = epoll_create(CONCURRENCY_CONNECTIONS_MAX);
//        if (INVALID_SOCKET == m_nEpollFD)
//        {
//            return false;
//        }
//
//        m_nListeningFD = socket(AF_INET, SOCK_STREAM, 0);
//        if (INVALID_SOCKET == m_nListeningFD)
//        {
//            OnError(SL_CRITICAL, SE_FAILT_TO_CREATE_SOCKET, "Failt to create the server socket!");
//            return false;
//        }
//
//        if (!SetNoblockSocket(m_nListeningFD))
//        {
//            return false;
//        }
//
//        bool nResult = false;
//        int bReuse = 1;
//        setsockopt(m_nListeningFD, SOL_SOCKET, SO_REUSEADDR, (char*)&bReuse, sizeof(bReuse));
//
//        int nTryNumber = 0;
//        struct sockaddr_in objAddr;
//
//        bzero(&objAddr, sizeof(objAddr));
//        objAddr.sin_family = AF_INET;
//        objAddr.sin_port = htons(m_nListenPort);
//        objAddr.sin_addr.s_addr = INADDR_ANY;
//        do
//        {
//            if ((bind(m_nListeningFD, (struct sockaddr*)&objAddr, sizeof(struct sockaddr))) != -1)
//            {
//                LOG_DEBUG("service port:%d", m_nListenPort);
//                nResult = true;
//                break;
//            }
//            nTryNumber++;
//        } while (nTryNumber < 10);
//
//        if (true == nResult)
//        {
//            if (listen(m_nListeningFD, M_LISTEN_MAX) == -1)
//            {
//                std::ostringstream strMsg;
//
//                strMsg << "Fail to listen (error: " << errno << ")";
//                OnError(SL_CRITICAL, SE_FAILT_TO_LISTEN, strMsg.str().c_str());
//
//                nResult = false;
//            }
//        }
//        else
//        {
//            std::ostringstream strMsg;
//
//            strMsg << "Fail to bind (error: " << errno << ")";
//            OnError(SL_CRITICAL, SE_FAILT_TO_BING, strMsg.str().c_str());
//        }
//
//        if (true == nResult)
//        {
//            EpollAddFD(m_nListeningFD);
//
//            //ATLTRACE("Listening on all local ip address, waiting for connect...\n");
//            m_nStatus = SS_INITIALIZED;
//        }
//        else
//        {
//            close(m_nListeningFD);
//            m_nListeningFD = INVALID_SOCKET;
//        }
//
//        return (INVALID_SOCKET != m_nListeningFD);
        
        return true;
    }


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
//        m_objPollThread.join();


        // 停止工作线程池
//        m_objThreadPool.Stop();

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
        if (nullptr != pConversationClosed)
        {
            CNetIPDataTransferPtr pNetDataTransfer = pConversationClosed->GetTransfer();

            if (pNetDataTransfer)
            {
                sockaddr_in   objSocketAddr;
                objSocketAddr.sin_addr.s_addr = htonl(pNetDataTransfer->GetIP());
                LOG_DEBUG("The connection (%s: %d) will be closed"
                    , inet_ntoa(objSocketAddr.sin_addr)
                    , pNetDataTransfer->GetPort());
            }

            DiscardEndpoint(std::dynamic_pointer_cast<EndpointT>(pConversationClosed));
        }

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
                    LOG_DEBUG("client fd[%d] timeout, it,s will be closed!", nFd);
                }
            }
        }
    }

private:

    //----------------------------------------------------------------------------------------------------
    //  函数名称: EpollAddFD
    /// @brief  把一个socket句柄添加到epoll
    ///
    /// @param  nFD ->socket文件表述符
    /// @return true:成功，false:失败
    /// @author 黄丽云
    //----------------------------------------------------------------------------------------------------
//    bool EpollAddFD(int nFD, int nEvents = (EPOLLIN | EPOLLPRI | EPOLLET | EPOLLERR))
//    {
//        bool nResult = false;
//        struct epoll_event objEpollEvent;
//
//        memset(&objEpollEvent, 0, sizeof(objEpollEvent));
//        objEpollEvent.data.fd = nFD;
//        objEpollEvent.events = nEvents;
//
//        if (0 == epoll_ctl(m_nEpollFD, EPOLL_CTL_ADD, nFD, &objEpollEvent))
//        {
//            nResult = true;
//        }
//
//        return nResult;
//    }

    //----------------------------------------------------------------------------------------------------
    //  函数名称: EpollDeleteFD
    /// @brief  从epoll移除一个socket句柄
    ///
    /// @param  nFD ->socket文件表述符
    /// @return true:成功，false:失败
    /// @author 黄丽云
    //----------------------------------------------------------------------------------------------------
//    bool EpollDeleteFD(int nFD)
//    {
//        bool nResult = false;
//
//        if (0 == epoll_ctl(m_nEpollFD, EPOLL_CTL_DEL, nFD, nullptr))
//        {
//            nResult = true;
//        }
//
//        return nResult;
//    }

    //----------------------------------------------------------------------------------------------------
    //  函数名称: EpollSetFD
    /// @brief  设置一个epoll事件
    ///
    /// @param  nFD ->socket文件表述符
    /// @param  nEvents ->新的事件
    /// @return true:成功，false:失败
    /// @author 黄丽云
    //----------------------------------------------------------------------------------------------------
//    bool EpollSetFD(int nFD, int nEvents)
//    {
//        bool nResult = false;
//        struct epoll_event objEpollEvent;
//
//        memset(&objEpollEvent, 0, sizeof(objEpollEvent));
//        objEpollEvent.data.fd = nFD;
//        objEpollEvent.events = nEvents;
//        if (0 == epoll_ctl(m_nEpollFD, EPOLL_CTL_MOD, nFD, &objEpollEvent))
//        {
//            nResult = true;
//        }
//
//        return nResult;
//    }

    //----------------------------------------------------------------------------------------------------
    //  函数名称: EpollResetFD
    /// @brief  复位一个epoll表示
    ///
    /// @param  nFD ->socket文件表述符
    /// @return true:成功，false:失败
    /// @author 黄丽云
    //----------------------------------------------------------------------------------------------------
//    bool EpollResetFD(int nFD)
//    {
//        return (0 == EpollSetFD(nFD, EPOLLIN | EPOLLPRI | EPOLLET | EPOLLERR));
//    }

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

    //----------------------------------------------------------------------------------------------------
    //  函数名称: EpollThread
    /// @brief  扫描socket连接
    ///
    /// @return  RT_TRUE：成功，其它失败
    /// @author 黄丽云
    //----------------------------------------------------------------------------------------------------

//    bool EpollThread()
//    {
//        bool nResult = true;
//        static struct epoll_event objEvents[EPOLL_MAX];
//
//        memset(objEvents, 0, EPOLL_MAX * sizeof(struct epoll_event));
//
//        while (SS_RUNING == m_nStatus)
//        {
//            const int FDS_NUMBER = epoll_wait(m_nEpollFD, objEvents, EPOLL_MAX, m_nPollTimeout);
//
//            for (int i = 0; i < FDS_NUMBER; i++)
//            {
//                if (objEvents[i].events & (EPOLLHUP | EPOLLERR))
//                {
//                    LOG_DEBUG("FD:%d,Events(%s%s,%d:%s)",
//                        objEvents[i].data.fd,
//                        (objEvents[i].events & EPOLLPRI) ? "EPOLLHUP" : ".",
//                        (objEvents[i].events & EPOLLERR) ? "EPOLLERR" : ".",
//                        errno, strerror(errno));
//
//                    if (objEvents[i].data.fd != GetListeningFD())
//                    {
//                        auto it = m_mapAliveEndpoints.find(objEvents[i].data.fd);
//                        if (m_mapAliveEndpoints.end() != it)
//                        {
//                            OnCloseEventHandle(it->second);
//                        }
//                    }
//                    else
//                    {
//
//                        LOG_ERROR("ListenFd[%d] is error", m_nListeningFD);
//                    }
//
//                    continue;
//                }
//
//                if (objEvents[i].data.fd == GetListeningFD())
//                {
//                    while (true)
//                    {
//                        struct sockaddr_in objPeerAddr;
//
//                        memset(&objPeerAddr, 0, sizeof(objPeerAddr));
//
//                        int nNewFD = INVALID_SOCKET;
//                        socklen_t SOCKET_ADDR_LEN = sizeof(struct sockaddr_in);
//
//                        nNewFD = accept(GetListeningFD(), (struct sockaddr*)&objPeerAddr, &SOCKET_ADDR_LEN);
//
//                        if (nNewFD >= 0)
//                        {
//                            OnNewConnectionRequest(nNewFD, objPeerAddr);
//                        }
//                        else
//                        {
//                            if (errno == EAGAIN ||
//                                errno == ECONNABORTED ||
//                                errno == EPROTO ||
//                                errno == EINTR)
//                            {
//                                LOG_DEBUG("accept %d, %s", errno, strerror(errno));
//                                break;
//                            }
//                            else
//                            {
//                                LOG_FATAL("server accept error:%d, %s", errno, strerror(errno));
//                                exit(-1);
//                            }
//                        }
//                    }
//
//                }
//                else
//                {
//                    auto it = m_mapAliveEndpoints.find(objEvents[i].data.fd);
//
//                    if (m_mapAliveEndpoints.end() != it)
//                    {
//                        PTR_ENDPOINT pEndpoint = it->second;
//
//                        //if (!pEndpoint->GetInThreadPool())
//                        //{
//                        //LOG_DEBUG("Append new task (%d)", objEvents[i].data.fd);
//                        //    pEndpoint->SetInThreadPool(true);
//
//                            std::shared_ptr<CSimpleCTask> pTask = std::make_shared<CSimpleCTask>(
//                                [this, pEndpoint]()
//                                {
//                                    OnWorkerThread(pEndpoint);
//                                    //pEndpoint->SetInThreadPool(false);
//                                }
//                            );
//
//                            m_objThreadPool.AddTask(pTask);
//                    /*    }
//                        else
//                        {
//                            LOG_DEBUG("The Endpoint is in thread pool!");
//                        }*/
//
//                    }
//                }
//            }
//
//            //时间轮
//            {
//                auto nBeg = std::chrono::high_resolution_clock::now();
//
//                m_pTimerWhell->Run();
//                /*U32 nDelay = */std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - nBeg).count();
//
//                //LOG_TRACE("timerWhell time delay=%dmS", nDelay);
//            }
//        }
//
//        return nResult;
//    }

private:
    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetServiceImpl::OnNewConnectionRequest
    /// @brief  捕捉客户端的连接请求
    ///
    /// @param  nSocketFD ->socket标识
    /// @param  objPeerAdd ->socket标识
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    void OnNewConnectionRequest(int nSocketFD, struct sockaddr_in& objPeerAdd)
    {
        PTR_ENDPOINT pEndpoint = CreateEndpoint();
        if (pEndpoint != nullptr)
        {
            pEndpoint->Initial(nSocketFD);
            pEndpoint->SetClient(false);
            //pEndpoint->SetStatus(RS_ESTABLISH);

            CTimerPtr pTimer = pEndpoint->GetTimer();
            unsigned int nIp = NTOHL(objPeerAdd.sin_addr.s_addr);
            U16 nPort = NTOHS(objPeerAdd.sin_port);

            pTimer->Initialize(
                std::bind(&CNetServiceImpl<EndpointT>::ConnectTimeout, this, nSocketFD, nIp, nPort)
                , m_nKeepAliveTimeout * 1000
                , CTimer::TT_ONCE);

            m_pTimerWhell->CreateTimer(pTimer);

            CNetIPDataTransferPtr pNetDataTransfer = pEndpoint->GetTransfer();

            if (pNetDataTransfer)
            {
                pNetDataTransfer->SetIP(nIp);
                pNetDataTransfer->SetPort(nPort);
            }

            {
                std::lock_guard<std::recursive_mutex> objAutoLock(m_objEndpointMutex);
                m_mapAliveEndpoints[nSocketFD] = pEndpoint;
                LOG_DEBUG("new connect(%s:%d) fd=%d current aliveEndpoints=%d"
                    , inet_ntoa(objPeerAdd.sin_addr)
                    , nPort
                    , nSocketFD
                    , m_mapAliveEndpoints.size());
            }

            const int  nSockBufSize = 64*1024;

            setsockopt(nSocketFD, SOL_SOCKET, SO_SNDBUF,(char *)&nSockBufSize, sizeof(nSockBufSize));
            SetNoblockSocket(nSocketFD);
//            EpollAddFD(nSocketFD, EPOLLIN | EPOLLPRI | EPOLLET | EPOLLERR | EPOLLONESHOT);
            OnNewConversationEventHandle(pEndpoint);
        }
    }

    //--------------------------------------------------------------------------------------------------------------------
    //函数名称： CNetServiceImpl::OnWorkerThread
    /// @brief  真正处理客户端请求的成员函数
    ///
    /// @author 黄丽云
    //--------------------------------------------------------------------------------------------------------------------
    void OnWorkerThread(PTR_ENDPOINT pEndpoint)
    {
        if (!pEndpoint)
        {
            return;
        }

        pEndpoint->ReceiveData();
//        EpollSetFD(pEndpoint->GetFD(), EPOLLIN | EPOLLPRI | EPOLLET | EPOLLERR | EPOLLONESHOT);
        //std::this_thread::sleep_for(std::chrono::microseconds(500));
        
        //LOG_DEBUG("End task (%d)", pEndpoint->GetFD());
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
private:
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
        ;
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

#endif /* ServiceImpl_hpp */

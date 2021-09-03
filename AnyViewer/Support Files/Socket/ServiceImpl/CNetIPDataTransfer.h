//
//  CNetIPDataTransfer.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//

#ifndef CNetIPDataTransfer_h
#define CNetIPDataTransfer_h

#pragma once
#include "Typedef.h"

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
    
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nIP, IP,0);                      ///< 目标地址
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nPort, Port,0);           ///< 目标端口
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
    
    int m_nFD = -1;
    
    void SetFD(int value)
    {
        m_nFD = value;
    }
    
    int GetFD() {
        return m_nFD;
    }
    
    
    int m_nRecvWaitingTime = 0;
    
    void SetRecvWaitingTime(int value)
    {
        m_nRecvWaitingTime = value;
    }
    
    int GetRecvWaitingTime() {
        return m_nRecvWaitingTime;
    }
    
    bool m_bIsClient = false;
    
    void SetIsClient(bool value)
    {
        m_bIsClient = value;
    }
    
    bool GetIsClient()
    {
        return m_bIsClient;
    }
    
    
//    DECLARE_MEMBER_AND_METHOD_V11(int, m_nFD, FD,-1)                                          ///< socket套接字ID
//    DECLARE_MEMBER_AND_METHOD_V11(int, m_nRecvWaitingTime, RecvWaitingTime,0)  ///< 接收数据等待时间
//    DECLARE_MEMBER_AND_METHOD_V11(bool,m_bIsClient, IsClient,false);            ///< 是否是客户端
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

#endif

//
//  CNetIPDataTransfer.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//

#pragma once
#include "CNetIPDataTransfer.h"
#include <errno.h>
#import <fcntl.h>
#include "Log.h"
//#include "ServiceHeader.h"


//------------------------------------------------------------------------------------
// CNetTCPDataTransfer::CNetTCPDataTransfer
/// @brief 构造函数
///
/// @author 黄丽云
//------------------------------------------------------------------------------------
CNetTCPDataTransfer::CNetTCPDataTransfer()
    : CNetIPDataTransfer(0, 0)
    , m_nFD(INVALID_SOCKET)
    , m_nRecvWaitingTime(RECEIVING_TIME_OUT)
    , m_bIsClient(false)
{
}

//------------------------------------------------------------------------------------
// @fn  CNetTCPDataTransfer::CNetTCPDataTransfer(unsigned int nIP,unsigned int nPort)
/// @brief 构造函数
///
/// @param _in nIP -返目标主机的IP地址
/// @param _in nPort -目标主机的端口
/// @author 黄丽云
//------------------------------------------------------------------------------------
CNetTCPDataTransfer::CNetTCPDataTransfer(unsigned int nIP, unsigned int nPort)
    : CNetIPDataTransfer(nIP, nPort)
    , m_nFD(INVALID_SOCKET)
    , m_nRecvWaitingTime(RECEIVING_TIME_OUT)
    , m_bIsClient(false)
{
}

//------------------------------------------------------------------------------------
// CNetTCPDataTransfer::~CNetTCPDataTransfer
/// @brief 析构函数
///
/// @author 黄丽云
//------------------------------------------------------------------------------------
CNetTCPDataTransfer::~CNetTCPDataTransfer()
{
    try
    {
        if (INVALID_SOCKET != m_nFD)
        {
            close(m_nFD);
            m_nFD = INVALID_SOCKET;
        }
    }
    catch (...)
    {
        LOG_FATAL("~CNetTCPDataTransfer() 发生异常");
    }

}

//------------------------------------------------------------------------------------
// CNetTCPDataTransfer::Attach
/// @brief 把当前的对象关联到指定的Socket并设置发送超时
///
/// @param _in nFD -一个指向目标socket的指针
/// @return  成功返回为true,否则返回false
/// @author 黄丽云
//------------------------------------------------------------------------------------

bool CNetTCPDataTransfer::Attach(int nFD)
{
    bool bResult = false;

    assert(INVALID_SOCKET != nFD);
    assert(INVALID_SOCKET == m_nFD);
    if (INVALID_SOCKET == m_nFD)
    {
        m_nFD = nFD;
        if (INVALID_SOCKET != m_nFD)
        {
            struct timeval tv = { SENDING_TIME_OUT, 0 };

            int iRet = setsockopt(m_nFD, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval));
            if (SOCKET_ERROR == iRet)
            {
                //ATLTRACE("Set timeout error:%d\n", errno);
            }
            bResult = (SOCKET_ERROR != iRet);
        }
    }

    return bResult;
}

//------------------------------------------------------------------------------------
// CNetTCPDataTransfer::Detach
/// @brief 解除关联
///
/// @author 黄丽云
//------------------------------------------------------------------------------------
void CNetTCPDataTransfer::Detach()
{
    m_nFD = INVALID_SOCKET;
}

//------------------------------------------------------------------------------------
// CNetTCPDataTransfer::Shutdown
/// @brief 通过关闭socket来关闭当前对象
///
/// @param _in how -   该函数的行为依赖于howto的值
/// 1.SHUT_RD：值为0，关闭连接的读这一半。
/// 2.SHUT_WR：值为1，关闭连接的写这一半。
/// 3.SHUT_RDWR：值为2，连接的读和写都关闭。
/// @return  返回成功为0，出错为-1
/// @author 黄丽云
//------------------------------------------------------------------------------------
bool CNetTCPDataTransfer::Shutdown(int how)
{
    int nResult = 0;
    if (INVALID_SOCKET != m_nFD)
    {
        nResult = shutdown(m_nFD, how);
    }

    return (SOCKET_ERROR != nResult);
}

//----------------------------------------------------------------------------------------------------
//  函数名称: EnableBlockSocket
/// @brief   把一个socket设置为阻塞或非阻塞的模式
///
/// @param  bEnable ->使能标志(true:阻塞，false:非阻塞)
/// @return true:成功，false:失败
//----------------------------------------------------------------------------------------------------
bool CNetTCPDataTransfer::EnableBlockSocket(bool bEnable)
{
    int  nResult = -1;
    
    int nOption = fcntl(m_nFD, F_GETFL, 0);

    if (nOption >= 0)
    {
        if (bEnable)
        {
            nOption &= ~O_NONBLOCK;
        }
        else
        {
            nOption |= O_NONBLOCK;
        }
        nResult = fcntl(m_nFD, F_SETFL, nOption);
    }
    assert(nResult >= 0);

    return (nResult >= 0);
}

//--------------------------------------------------------------------------------------------------------------------
// CNetTCPDataTransfer::Connect
/// @brief 连接到指定的服务器
///
/// @param _in nTimeout -连接超时时间(缺省是30秒)
/// @return  true: 成功, false: 失败 (采用异步连接，返回结果无效)
/// @author 黄丽云
//--------------------------------------------------------------------------------------------------------------------
bool CNetTCPDataTransfer::Connect(int nTimeout/* = 30*/)
{
    bool bResult = false;

    if (INVALID_SOCKET == m_nFD)
    {
        m_nFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (INVALID_SOCKET == m_nFD)
        {
            LOG_ERROR("Fail to create socket:%s\n", strerror(errno));
            return bResult;
        }
    }

    //设置为非阻塞模式
    EnableBlockSocket(false);

    struct sockaddr_in sa;

    bzero(&sa, sizeof(struct sockaddr_in));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = HTONL(m_nIP);
    sa.sin_port = HTONS(m_nPort);

    //建立连接
    bResult = (connect(m_nFD, (struct sockaddr*)&sa, sizeof(sa)) != SOCKET_ERROR);

    if (!bResult)
    {
        // 非阻塞模式返回未完成
        if (errno == EINPROGRESS)
        {
            LOG_DEBUG("unblock mode socket is connecting...\n");
          
            fd_set objFDSet;

            FD_ZERO(&objFDSet);
            FD_SET(m_nFD, &objFDSet);

            timeval tm;

            tm.tv_sec = nTimeout;
            tm.tv_usec = 0;
            if (select(m_nFD + 1, nullptr, &objFDSet, nullptr, &tm) > 0
                && FD_ISSET(m_nFD, &objFDSet))
            {
                int nErr = 0;
                socklen_t len = sizeof(nErr);
                if (0 == getsockopt(m_nFD, SOL_SOCKET, SO_ERROR, &nErr, &len))
                {
                    if (0 == nErr)
                    {
                        bResult = true;
                    }
                    else
                    {
                        LOG_ERROR("Fail to connnect host:%s\n", strerror(nErr));
                    }
                }
            }
        }
        else
        {
            LOG_ERROR("Fail to connnect host:%s\n", strerror(errno));
        }
    }

    //恢复为阻塞模式
    EnableBlockSocket(true);

    // 此处必须设置接受超时，否则无法退出接收线程
    struct timeval timeout = { 2,0 };
    setsockopt(m_nFD, SOL_SOCKET, SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));

    return bResult;
}

//------------------------------------------------------------------------------------
// CNetTCPDataTransfer::IgnoreError
/// @brief 判断当前的错误是否是可以忽略的错误
///
/// @param _in nErrCode -错误代码
/// @return  true: 可以忽略的错误，false:严重错误，通常是连接已经断开
/// @author 黄丽云
//------------------------------------------------------------------------------------
bool CNetTCPDataTransfer::IgnoreError(int nErrCode)
{
    bool nResult = false;

    switch (nErrCode)
    {
    case EAGAIN:
    case EINTR:
    case EINPROGRESS:
        nResult = true;
        break;

    default:
        break;
    }

    return nResult;
}

//------------------------------------------------------------------------------------
// CNetTCPDataTransfer::Send
/// @brief 发送数据
///
/// @param _in pBuffer -一个指向被发送数据的头指针
/// @param _in nLen -被发送数据长度
/// @return  成功返回实际发送的数据，-2:表示连接已经断开，其它负数表示错误
/// @author 黄丽云
//------------------------------------------------------------------------------------
int CNetTCPDataTransfer::Send(const BYTE* pBuffer, int nLen)
{
    if (INVALID_SOCKET == m_nFD)
    {
        return -1;
    }

    int nResult = 0;
    int nBytesSent = 0;
    int nTotalBytesSent = 0;

    assert(nullptr != pBuffer && nLen > 0);
    if (nullptr != pBuffer && nLen > 0)
    {
        do
        {
            /* this function ‘send' returns the total number of bytes sent, which can be Less than
            the number indicated by len for nonblocking sockets, 所以必须循环发送，知道数据
            发送完毕或者，遇到错误.
            */
            nBytesSent = send(m_nFD, (const char*)&pBuffer[nTotalBytesSent], nLen - nTotalBytesSent, 0);

            if (nBytesSent > 0)
            {
                nTotalBytesSent += nBytesSent;
            }
            else
            {
                if (!IgnoreError(errno))
                {
                    LOG_DEBUG("Fail to send data (err: %d)", errno);
                    nResult = SOCKET_ERROR;
                    break;   /// 必须是严重错误才退出
                }
            }
        } while (nTotalBytesSent < nLen);

        if (nTotalBytesSent > 0)
        {
            nResult = nTotalBytesSent;
        }

#ifdef _DEBUG
        if (SOCKET_ERROR == nResult)
        {
            ATLTRACE(("Fail to send data (error = %d)\r\n"), errno);
        }
#endif
    }

    return nResult;
}

//------------------------------------------------------------------------------------
// CNetTCPDataTransfer::QuerySocketStatus
/// @brief 查询Socket的状态
///
/// @param _in refTimeout -超时间隔设置
/// @param _in bRead -查询状态类型(true:读，FASLE:写)
/// @param _in pIOReady - 非零表示已经可以读写
/// @return  true->成功,false->失败
/// @author 黄丽云
//------------------------------------------------------------------------------------
bool CNetTCPDataTransfer::QuerySocketStatus(timeval& refTimeout, bool bRead, int* pIOReady)
{
    fd_set objFDSet;

    FD_ZERO(&objFDSet);
    FD_SET(m_nFD, &objFDSet);

    int iRet = -1;
    if (bRead)    // 写
        {
            iRet = select(m_nFD + 1, &objFDSet, nullptr, nullptr, &refTimeout);
        }
    else         // 读
        {
            iRet = select(m_nFD + 1, nullptr, &objFDSet, nullptr, &refTimeout);
        }

    if (SOCKET_ERROR != iRet)
    {
        if (nullptr != pIOReady)
        {
            *pIOReady = FD_ISSET(m_nFD, &objFDSet);
        }
    }
    //else
    //{
        //ATLTRACE(("select error:%d"), errno);
    //}

    return (SOCKET_ERROR != iRet);
}

//------------------------------------------------------------------------------------
// CNetTCPDataTransfer::Recv
/// @brief 接收数据
///
/// @param _in pBuffer -一个指向数据缓冲区的头指针，用于存放接收的数据
/// @param _in nLen -缓冲区的长度
/// @return  成功返回实际接收的数据，小于0表示失败(-1：常规错误,-2:连接已经关闭)
/// @author 黄丽云
//------------------------------------------------------------------------------------
int CNetTCPDataTransfer::Recv(U8* pBuffer, int nLen)
{
    if (INVALID_SOCKET == m_nFD)
    {
        return -2;
    }

    if (nLen <= 0)
    {
        LOG_ERROR("The Buffer size is zero!");
        return -3;
    }

    int nBytesReceived = -1;

    nBytesReceived = recv(m_nFD, pBuffer, nLen, 0);

    if (SOCKET_ERROR == nBytesReceived)
    {
        //ATLTRACE(("Fail to recv data (IP = %u.%u.%u.%u,error = %d)\r\n"),
        //((U8*)&m_nIP)[0]
        //, ((U8*)&m_nIP)[1]
        //, ((U8*)&m_nIP)[2]
        //, ((U8*)&m_nIP)[3],
        //errno);

        nBytesReceived = (IgnoreError(errno)) ? -1 : -2;
    }
    else if (0 == nBytesReceived)
    {
        nBytesReceived = -2;
    }

    return nBytesReceived;
}

//------------------------------------------------------------------------------------
// CNetTCPDataTransfer::Close
/// @brief 关闭当前的数据传输对象
///
/// @author 黄丽云
//------------------------------------------------------------------------------------
void CNetTCPDataTransfer::Close()
{
    if (INVALID_SOCKET != m_nFD)
    {
        close(m_nFD);
        m_nFD = INVALID_SOCKET;
    }
}


//--------------------------------CNetUDPDataTransfer--------------------------------------------------------
//------------------------------------------------------------------------------------
// CNetUDPDataTransfer::CNetUDPDataTransfer
/// @brief 构造函数
///
/// @author 黄丽云
//------------------------------------------------------------------------------------
CNetUDPDataTransfer::CNetUDPDataTransfer()
    : CNetIPDataTransfer(0, 0)
    , m_nFD(INVALID_SOCKET)
    , m_nRecvWaitingTime(RECEIVING_TIME_OUT)
{

}

//------------------------------------------------------------------------------------
// @fn  CNetUDPDataTransfer::CNetUDPDataTransfer(unsigned int nIP,USHORT nPort)
/// @brief 构造函数
///
/// @param _in nIP -返目标主机的IP地址
/// @param _in nPort -目标主机的端口
/// @author 黄丽云
//------------------------------------------------------------------------------------
CNetUDPDataTransfer::CNetUDPDataTransfer(unsigned int nIP, USHORT nPort)
    : CNetIPDataTransfer(nIP, nPort)
    , m_nFD(INVALID_SOCKET)
    , m_nRecvWaitingTime(RECEIVING_TIME_OUT)
{
}

//------------------------------------------------------------------------------------
// CNetUDPDataTransfer::~CNetUDPDataTransfer
/// @brief 析构函数
///
/// @author 黄丽云
//------------------------------------------------------------------------------------
CNetUDPDataTransfer::~CNetUDPDataTransfer()
{
    try
    {
        if (INVALID_SOCKET != m_nFD)
        {
            close(m_nFD);
            m_nFD = INVALID_SOCKET;
        }
    }
    catch (...)
    {
        LOG_FATAL("~CNetTCPDataTransfer() 发生异常");
    }
}

/// @brief 设置目标主机的IP地址
///
/// @param nIP 目标主机的IP地址.
void CNetUDPDataTransfer::SetIP(unsigned int nIP)
{
    CNetIPDataTransfer::SetIP(nIP);
}

/// @brief 设置目标主机的端口号
///
/// @param nPort 目标主机的端口号.
void CNetUDPDataTransfer::SetPort(USHORT nPort)
{
    CNetIPDataTransfer::SetPort(nPort);
}

//------------------------------------------------------------------------------------
// CNetUDPDataTransfer::Attach
/// @brief 把当前的对象关联到指定的Socket并设置发送超时
///
/// @param _in pSocket -一个指向目标socket的指针
/// @return  成功返回为true,否则返回false
/// @author 黄丽云
//------------------------------------------------------------------------------------

bool CNetUDPDataTransfer::Attach(int nFD)
{
    bool bResult = false;

    assert(INVALID_SOCKET != nFD);
    assert(INVALID_SOCKET == m_nFD);

    if (INVALID_SOCKET == m_nFD)
    {
        m_nFD = nFD;
    
        if (INVALID_SOCKET != m_nFD)
        {
            struct timeval tv = { SENDING_TIME_OUT, 0 };
            int iRet = setsockopt(m_nFD, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval));

            if (SOCKET_ERROR == iRet)
            {
                //ATLTRACE("Set timeout error:%d\n", errno);
            }
            
            bResult = (SOCKET_ERROR != iRet);
        }
    }

    return bResult;
}

//------------------------------------------------------------------------------------
// CNetUDPDataTransfer::Detach
/// @brief 解除关联
///
/// @author 黄丽云
//------------------------------------------------------------------------------------
void CNetUDPDataTransfer::Detach()
{
    m_nFD = INVALID_SOCKET;
}

//------------------------------------------------------------------------------------
// CNetUDPDataTransfer::Shutdown
/// @brief 通过关闭socket来关闭当前对象
///
/// @param _in pSocket -Flag that describes what types of operation will no longer be allowed.
/// @return  If no error occurs, shutdown returns zero. Otherwise, a value of SOCKET_ERROR is returned
/// @author 黄丽云
//------------------------------------------------------------------------------------
bool CNetUDPDataTransfer::Shutdown(int how)
{
    int nResult = 0;
    if (INVALID_SOCKET != m_nFD)
    {
        nResult = shutdown(m_nFD, how);
    }

    return (SOCKET_ERROR != nResult);
}

//----------------------------------------------------------------------------------------------------
//  函数名称: EnableBlockSocket
/// @brief   把一个socket设置为阻塞或非阻塞的模式
///
/// @param  bEnable ->使能标志(true:阻塞，false:非阻塞)
/// @return true:成功，false:失败
//----------------------------------------------------------------------------------------------------
bool CNetUDPDataTransfer::EnableBlockSocket(bool bEnable)
{
    int  nResult = -1;

    int nOption = fcntl(m_nFD, F_GETFL, 0);

    if (nOption >= 0)
    {
        if (bEnable)
        {
            nOption &= ~O_NONBLOCK;
        }
        else
        {
            nOption |= O_NONBLOCK;
        }
        nResult = fcntl(m_nFD, F_SETFL, nOption);
    }
    assert(nResult >= 0);

    return (nResult >= 0);
}

//------------------------------------------------------------------------------------
// CNetTCPDataTransfer::IgnoreError
/// @brief 判断当前的错误是否是可以忽略的错误
///
/// @param _in nErrCode -错误代码
/// @return  true: 可以忽略的错误，false:严重错误，通常是连接已经断开
/// @author 黄丽云
//------------------------------------------------------------------------------------
bool CNetUDPDataTransfer::IgnoreError(int nErrCode)
{
    bool nResult = false;

    switch (nErrCode)
    {
    case EAGAIN:
    case EINTR:
    case EINPROGRESS:
        nResult = true;
        break;

    default:
        break;
    }

    return nResult;
}

//--------------------------------------------------------------------------------------------------------------------
// CNetUDPDataTransfer::Connect
/// @brief 连接到指定的服务器
///
/// @param _in nTimeout -连接超时时间(缺省是30秒)
/// @return  true: 成功, false: 失败 (采用异步连接，返回结果无效)
/// @author 黄丽云
//--------------------------------------------------------------------------------------------------------------------
bool CNetUDPDataTransfer::Connect(int  nTimeout/*= 30*/)
{
    bool bResult = false;

    if (INVALID_SOCKET == m_nFD)
    {
        m_nFD = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        if (INVALID_SOCKET == m_nFD)
        {
            LOG_ERROR("Fail to create socket:%s\n", strerror(errno));
            return bResult;
        }
    }

    //设置为非阻塞模式
    EnableBlockSocket(false);

    struct sockaddr_in sa;

    bzero(&sa, sizeof(struct sockaddr_in));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = HTONL(m_nIP);
    sa.sin_port = HTONS(m_nPort);

    //建立连接
    bResult = (connect(m_nFD, (struct sockaddr*)&sa, sizeof(sa)) != SOCKET_ERROR);

    if (!bResult)
    {
        // 非阻塞模式返回未完成
        if (errno == EINPROGRESS)
        {
            LOG_DEBUG("unblock mode socket is connecting...\n");

            fd_set objFDSet;

            FD_ZERO(&objFDSet);
            FD_SET(m_nFD, &objFDSet);

            timeval tm;

            tm.tv_sec = nTimeout;
            tm.tv_usec = 0;
            if (select(m_nFD + 1, nullptr, &objFDSet, nullptr, &tm) > 0
                && FD_ISSET(m_nFD, &objFDSet))
            {
                int nErr = 0;
                socklen_t len = sizeof(nErr);
                if (0 == getsockopt(m_nFD, SOL_SOCKET, SO_ERROR, &nErr, &len))
                {
                    if (0 == nErr)
                    {
                        bResult = true;
                    }
                    else
                    {
                        LOG_ERROR("Fail to connnect host:%s\n", strerror(nErr));
                    }
                }
            }
        }
        else
        {
            LOG_ERROR("Fail to connnect host:%s\n", strerror(errno));
        }
    }

    //恢复为阻塞模式
    EnableBlockSocket(true);

    // 此处必须设置接受超时，否则无法退出接收线程
    struct timeval timeout = { 2,0 };
    setsockopt(m_nFD, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));

    return bResult;
}


//------------------------------------------------------------------------------------
// CNetUDPDataTransfer::Send
/// @brief 发送数据
///
/// @param _in pBuffer -一个指向被发送数据的头指针
/// @param _in nLen -被发送数据长度
/// @return  成功返回实际发送的数据，失败返回-1
/// @author 黄丽云
//------------------------------------------------------------------------------------
int CNetUDPDataTransfer::Send(const BYTE* pBuffer, int nLen)
{
    if (INVALID_SOCKET == m_nFD)
    {
        return -1;
    }

    int nBytesSent = 0;
    int nTotalBytesSent = 0;

    assert(nullptr != pBuffer && nLen > 0);
    if (nullptr != pBuffer && nLen > 0)
    {
        do
        {
            /* this function ‘send' returns the total number of bytes sent, which can be Less than
            the number indicated by len for nonblocking sockets, 所以必须循环发送，知道数据
            发送完毕或者，遇到错误.
            */
            nBytesSent = send(m_nFD, (const char*)&pBuffer[nTotalBytesSent], nLen - nTotalBytesSent, 0);
            
            if (nBytesSent > 0)
            {
                nTotalBytesSent += nBytesSent;
            }
            else
            {
                break;
            }
        } while (SOCKET_ERROR != nBytesSent && nTotalBytesSent < nLen);



//#ifdef _DEBUG
//        if (SOCKET_ERROR == nResult)
//        {
//            ATLTRACE(("Fail to send data (error = %d)\r\n"), errno);
//        }
//#endif
    }

    return nTotalBytesSent;
}

//------------------------------------------------------------------------------------
// CNetUDPDataTransfer::QuerySocketStatus
/// @brief 查询Socket的状态
///
/// @param _in refTimeout -超时间隔设置
/// @param _in bRead -查询状态类型(true:读，FASLE:写)
/// @param _in pBytesTransferred -用于接收传输的字节数
/// @return  true->成功,false->失败
/// @author 黄丽云
//------------------------------------------------------------------------------------
bool CNetUDPDataTransfer::QuerySocketStatus(timeval& refTimeout, bool bRead, int* pIOReady)
{
    fd_set objFDSet;

    FD_ZERO(&objFDSet);
    FD_SET(m_nFD, &objFDSet);

    int iRet = -1;
    if (bRead)    // 写
    {
        iRet = select(m_nFD + 1, &objFDSet, nullptr, nullptr, &refTimeout);
    }
    else         // 读
    {
        iRet = select(m_nFD + 1, nullptr, &objFDSet, nullptr, &refTimeout);
    }

    if (SOCKET_ERROR != iRet)
    {
        if (nullptr != pIOReady)
        {
            *pIOReady = FD_ISSET(m_nFD, &objFDSet);
        }
    }
    //else
    //{
        //ATLTRACE(("select error:%d"), errno);
    //}

    return (SOCKET_ERROR != iRet);
}

//------------------------------------------------------------------------------------
// CNetUDPDataTransfer::Recv
/// @brief 接收数据
///
/// @param _in pBuffer -一个指向数据缓冲区的头指针，用于存放接收的数据
/// @param _in nLen -缓冲区的长度
/// @return  成功返回实际接收的数据，小于0表示失败(-1：常规错误,-2:连接已经关闭)
/// @note 该函数没有正确实现,只是延迟5秒钟就返回
/// @author 黄丽云
//------------------------------------------------------------------------------------
int CNetUDPDataTransfer::Recv(BYTE* pBuffer, int nLen)
{
    if (INVALID_SOCKET == m_nFD)
    {
        return -2;
    }

    int nBytesReceived = -1;

    nBytesReceived = recv(m_nFD, pBuffer, nLen, 0);
    if (SOCKET_ERROR == nBytesReceived)
    {
        //ATLTRACE(("Fail to recv data (IP = %u.%u.%u.%u,error = %d)\r\n"),
        //((U8*)&m_nIP)[0]
        //, ((U8*)&m_nIP)[1]
        //, ((U8*)&m_nIP)[2]
        //, ((U8*)&m_nIP)[3],
        //errno);

        nBytesReceived = (IgnoreError(errno)) ? -1 : -2;
    }
    else if (0 == nBytesReceived)
    {
        nBytesReceived = -2;
    }

    return nBytesReceived;
}

//------------------------------------------------------------------------------------
// CNetUDPDataTransfer::Close
/// @brief 关闭当前的数据传输对象
///
/// @author 黄丽云
//------------------------------------------------------------------------------------
void CNetUDPDataTransfer::Close()
{
    if (INVALID_SOCKET != m_nFD)
    {
        close(m_nFD);
        m_nFD = INVALID_SOCKET;
    }
}

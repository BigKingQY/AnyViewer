//
//  VNCClient.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/9.
//

#ifndef VNCClient_h
#define VNCClient_h

#pragma once
//#include "ServiceImpl.h"
//#include "CNetEndpoint.h"
#include "CNetClientImpl.h"
#include "MessageBusManager.h"
#include "VNCProtocol.h"
#include "EnumHeader.h"
#include "Typedef.h"
#include "Ax3Event.h"
#include "VNCPacket.h"
#include "VNCEndpointT.h"
#include "Log.h"
#include "CNetTimer.h"

#include <boost/timer.hpp>

const int HEART_TIMEOUT = 120; //心跳超时

//extern CMessageBus& GetVNCMessageBus();
extern CMessageBus& GetMessageBus();

template <typename EndpointT>
class CVNCClient :public CNetClientImpl< EndpointT >
{
public:
    CVNCClient(const UINT nIP, const USHORT nPort)
    {
        CNetClientImpl< EndpointT >::SetIP(nIP);
        CNetClientImpl< EndpointT >::SetPort(nPort);
    }

    ~CVNCClient()
    {
    }

public:
    double GetReceivingLasttime()
    {
        return m_objReceivingLasttime.elapsed();
    }

    // 得到连接消耗的时间
    double GetConnectionTime()
    {
        return m_objConnectionTime.elapsed();
    }

    CONVERSATION_STATE GetConversationStatus()
    {
        return CNetClientImpl< EndpointT >::m_objEndpoint->GetStatus();
    }
public:
    //------------------------------------------------------------------------------------
    //函数名称： Connect()
    /// @brief 连接到服务器
    ///
    /// @return true->成功,false->失败
    /// @author 黄丽云
    //------------------------------------------------------------------------------------
    bool Connect();

    //--------------------------------------------------------------------------------
    /// <summary>
    /// 重新连接到服务器
    /// </summary>
    /// <returns>true->成功,false->失败</returns>
    /// <created>Andy,2020/3/18</created>
    /// <changed>Andy,2020/3/18</changed>
    //--------------------------------------------------------------------------------
    bool ReConnect()
    {
        CNetClientImpl< EndpointT >::Stop();
        return Connect();
    }

public:
    //  发送前事件,对数据包进行加密和计算crc32校验码
    virtual bool OnPreSendPacketEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CNetPacket * pPacket) override;

    // 收到新的数据包事件
    virtual bool OnReceivedPacketEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CNetPacket* pPacket) override;

    /// 通用的事件
    virtual bool OnEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint, SEVERITY_LEVEL nSeverity, SERVICE_EVENT nEvent) override;
private:
    ///< 通信端点事件处理接口
    DECLARE_MEMBER_AND_METHOD_V11(CNetEndpointEventSink*, m_pEndpointEventSink, EndpointEventSink, nullptr)


    /// 最后发送数据包时间
    CNetTimer               m_objSendingLastTime;

    // 最后活动时间(收发数据包)
    CNetTimer              m_objReceivingLasttime;

    // 建立连接消耗的时间
    CNetTimer              m_objConnectionTime;

    CAx3Event                  m_objSysEvent;      ///< 收到同步数据包请求
};

using CVNCTCPClientPtr = std::shared_ptr<CVNCClient<CVNCTCPEndpoint>>;
using CVNCUDPClientPtr = std::shared_ptr<CVNCClient<CVNCUDPEndpoint>>;


//------------------------------------------------------------------------------------
//函数名称： CConsoleProxy::Connect()
/// @brief 连接到服务器
/// 控制端和被控制端同时发起连接，收到应答数据包或请求包都认为连接成功建立，因为有可能有一边
/// 无法成功建立
/// @return true->成功,false->失败
/// @author 黄丽云
//------------------------------------------------------------------------------------
template<typename EndpointT>
inline bool CVNCClient<EndpointT>::Connect()
{
    bool bResult = CNetClientImpl< EndpointT >::Start();

    if (bResult)
    {
        if (std::is_same<EndpointT, CVNCUDPEndpoint>::value
            && CNetClientImpl< EndpointT >::m_objEndpoint->GetStatus() != RS_ESTABLISH)
        {
            // 如果是UDP通信，需要发送同步数据包来建立连接，
            CVNCPacket objSynPacket;

            objSynPacket.Initial(VNCP::MT_SYN, 0, VNCP::OT_REQUEST, 0);
            bResult = CNetClientImpl< EndpointT >::Send(&objSynPacket);

            if (bResult)
            {
                bResult = m_objSysEvent.WaitForEvent(CNetClientImpl< EndpointT >::GetConnectionTimeout() * 1000);
            }
        }
    }

    return (CNetClientImpl< EndpointT >::m_objEndpoint->GetStatus() == RS_ESTABLISH);
}

//--------------------------------------------------------------------------------
/// <summary>
/// 发送前事件,对数据包进行加密和计算crc32校验码
/// </summary>
/// <param name=""></param>
/// <param name="pPacket"></param>
/// <returns></returns>
/// <created>Andy,2020/3/18</created>
/// <changed>Andy,2020/3/18</changed>
//--------------------------------------------------------------------------------
template<typename EndpointT>
inline bool CVNCClient<EndpointT>::OnPreSendPacketEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CNetPacket * pPacket)
{
    const CONVERSATION_STATE nStatus = CNetClientImpl< EndpointT >::m_objEndpoint->GetStatus();

    if (RS_INVALID == nStatus || RS_CLOSE == nStatus || RS_RECONNECTING == nStatus)
    {
        return false;
    }

    m_objSendingLastTime.restart();
    return CNetClientImpl< EndpointT >::OnPreSendPacketEventHandle(pEndpoint, pPacket);
}


template<typename EndpointT>
inline bool CVNCClient<EndpointT>::OnReceivedPacketEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CNetPacket * pPacket)
{
    m_objReceivingLasttime.restart();

    bool bResult = CNetClientImpl< EndpointT >::OnProReceivedPacketEventHandle(pEndpoint, pPacket);

    if (!bResult)
    {
        return bResult;
    }

    CVNCPacket* pDataPacket = dynamic_cast<CVNCPacket*>(pPacket);

    CHECK_POINTER_EX(pDataPacket, false);

    if (std::is_same<EndpointT, CVNCUDPEndpoint>::value)
    {
        if (pDataPacket->GetType() == VNCP::MT_SYN)
        {
            // 如果是同步数据包特殊处理

            if (!(pDataPacket->GetFlag() & VNCP::OT_RESPONSE))
            {
                CVNCPacket objSynPacket;

                objSynPacket.Initial(VNCP::MT_SYN, 0, VNCP::OT_RESPONSE, 0);
                bResult = CNetClientImpl< EndpointT >::Send(&objSynPacket);
            }
            
            if (CNetClientImpl< EndpointT >::m_objEndpoint->GetStatus() != RS_ESTABLISH)
            {
                CNetClientImpl< EndpointT >::m_objEndpoint->ChanageStatus(RS_ESTABLISH);
            }

            m_objSysEvent.Notify();

            return bResult;
        }
    }


    if (nullptr != m_pEndpointEventSink)
    {
        m_pEndpointEventSink->OnReceivedPacketEventHandle(pEndpoint, pPacket);
    }

    CMessageBus& refMessageBus = GetMessageBus();
    
    //LOG_DEBUG("Pre-distribute a vnc packet (type = %d)", pDataPacket->GetType());
    refMessageBus.SendReq<void, PTR_NET_ENDPOINT_INTERFACE, CVNCPacket*>(
        LMBS_RECEIVED_VNC_PACKET
        , std::forward <PTR_NET_ENDPOINT_INTERFACE>(pEndpoint)
        , std::forward <CVNCPacket*>(pDataPacket));
    //LOG_DEBUG("Post-distribute a vnc packet (type = %d)", pDataPacket->GetType());

    return bResult;
}


/// @brief 通用的事件
template<typename EndpointT>
inline bool  CVNCClient<EndpointT>::OnEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint, SEVERITY_LEVEL nSeverity, SERVICE_EVENT nEvent)
{
    if (nullptr != m_pEndpointEventSink)
    {
        switch (nEvent)
        {
        case SE_CHECK_HEART:
        {
            if (m_objSendingLastTime.elapsed() >= HEART_TIMEOUT)
            {
                LOG_DEBUG("send heart packet\n");
                std::static_pointer_cast<EndpointT>(pEndpoint)->SendCommonRequest(VNCP::MT_HEARTBEAT);
                m_objSendingLastTime.restart();
            }
        }
        break;

        default:
            break;
        }

        m_pEndpointEventSink->OnEventHandle(pEndpoint, nSeverity, nEvent);
    }

    return true;
}


#endif /* VNCClient_h */

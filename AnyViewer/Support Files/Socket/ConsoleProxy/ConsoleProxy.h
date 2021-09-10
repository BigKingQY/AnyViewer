//
//  ConsoleProxy.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//

#ifndef ConsoleProxy_h
#define ConsoleProxy_h

#pragma once
#include "EccCode.h"
#include "RCProtocol.h"
#include "CNetClientImpl.h"

#include <chrono>
#include <boost/timer.hpp>


#define ECC_KEY_LEN   24

using PTR_WAIT_TASK = std::shared_ptr<CWaitingResponseTask<CDataPacket>>;

class CConsoleEndpoint : public CNetEndpoint< CDataPacket>
{
public:
    CConsoleEndpoint(CNetEndpointEventSink* pNetEndpointEventSink);

public:
    virtual bool Send(CNetPacket* pPacket) override;

public:
    bool SendCommonResponse(CDataPacket* pDataPacket, bool bStatus, const U64 nStatusCode = 0);
};


class CConsoleProxy : public CNetClientImpl< CConsoleEndpoint >
{
public:
    CConsoleProxy(UINT nIP, USHORT nPort);
    ~CConsoleProxy(void);

    /// <summary>ª·ª∞◊¥Ã¨¿‡–Õ√∂æŸ∂®“Â</summary>
    enum RCSVR_PROXY_STATUS
    {
        RPS_INVALID = 0,
        RPS_ESTABLISHMENT = 1,
        RPS_REGISTERED = 2,       ///< √∂æŸ≥£¡ø,≥…π¶◊¢≤·µΩ∑˛ŒÒ∆˜
        RPS_CLOSE = 3,
    };

public:
    /// @brief ¡¨Ω”µΩ∑˛ŒÒ∆˜
    bool Connect();

    //  ÷ÿ–¬¡¨Ω”µΩ∑˛ŒÒ∆˜
    bool ReConnect();


    // ∑¢ÀÕÕ®”√«Î«Ûœ˚œ¢
    bool SendCommonRequest(UINT nMsg, U64 nStatus = 0);

    void SendCommonResponse(CDataPacket* pDataPacket, bool bStatus, const U64 nStatusCode = 0);

    template<class T>
    void SendCommonResponse(CDataPacket* pDataPacket, const T& refObj, const OPERATOR_RESULT nOpResult)
    {
        CDataPacket objResponsePacket;

        objResponsePacket.InitialJsonT<MESSAGE_HEADER>(
            pDataPacket->GetPacketId()
            , (WORD)pDataPacket->GetPacketType()
            , BYTE(OT_RESPONSE | nOpResult)
            , &refObj);

        Send(&objResponsePacket);
    }

    // œÚøÿ÷∆∑˛ŒÒ∆˜◊¢≤·
    bool Regist(const RCP::CRCClientInfo& refRCClientInfo);

    // ∏¸–¬øÕªß∂À–≈œ¢
    bool UpdateClient(const RCP::CRCClientInfo& refRCClientInfo);

    // ¡¨Ω”ƒø±Í…Ë±∏
    bool ConnectRemoteClient(const RCP::CConnectionRequest& refConnRequest);

    // Õ¨≤Ω¡¨Ω”ƒø±Í…Ë±∏
    bool SyncConnectRemoteClient(const RCP::CConnectionRequest& refConnRequest, int& nStatusCode1, int& nStatusCode2, const int nTimeout = 60);


    // ∑¢ÀÕ»œ÷§œ˚œ¢µΩ∑˛ŒÒ∆˜
    bool Authenticate(const RCP::CAuthenticationRequest& refRequest/*, const int nTimeout = 10 * 60*/);

    // Õ¨≤Ω∑¢ÀÕ»œ÷§œ˚œ¢µΩ∑˛ŒÒ∆˜
    bool SyncAuthenticate(const RCP::CAuthenticationRequest& refRequest, U32& nStatusCode1, U32& nStatusCode2, const int nTimeout = 10 * 60);

    // ∑¢ÀÕ∏ƒ±‰Õ®–≈∑Ω Ω«Î«Û
    bool SendChangeCommModeRequest(const U32 nSessionID);
protected:
    virtual bool OnPreSendPacketEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CNetPacket* pPacket);
    virtual bool OnProReceivedPacketEventHandle(PTR_NET_ENDPOINT_INTERFACE pNetEndpointInterface, CNetPacket* pNetPacket);
    virtual bool OnReceivedPacketEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CNetPacket* pPacket);
    virtual bool OnEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint, SEVERITY_LEVEL nSeverity, SERVICE_EVENT nEvent);
    //virtual bool OnReconnectEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint);
    //virtual bool OnCloseEventHandle(PTR_NET_ENDPOINT_INTERFACE);

protected:   // ”¶¥ ˝æ›∞¸

    // ◊¢≤·”¶¥ ˝æ›∞¸
    void OnRegistResponse(CDataPacket* pDataPacket);
    
    // 重置Timer
    void restartTimer();
    
    // 获取当前时间与上次记录的时间差
    long getTimerElapsed();

private:

    RCP::EXCEPTION_STATUS SyncSend(CDataPacket& refSend, PTR_WAIT_TASK ptrWaitTask, std::shared_ptr<CDataPacket>& refAckPacket);
private:    
    
private:
    //DECLARE_MEMBER_AND_METHOD(CString, m_strAccount, Account)      ///< ’À∫≈
    DECLARE_MEMBER_AND_METHOD(I64, m_nID, ID);
    DECLARE_MEMBER_AND_METHOD(unsigned int, m_nType, Type);  ///<@see COST_CLIENT_TYPE
    DECLARE_MEMBER_AND_METHOD_V11(RCSVR_PROXY_STATUS, m_nConversationStatus, ConversationStatus, RPS_INVALID);  ///< ◊¥Ã¨
    
    std::chrono::system_clock::time_point       m_objSendingLastTime;
//    boost::timer               m_objSendingLastTime;

    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nMsgType, MsgType, 0);

    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nEventType, EventType, 0);
};

using CConsoleProxyPtr = std::shared_ptr<CConsoleProxy>;

#endif

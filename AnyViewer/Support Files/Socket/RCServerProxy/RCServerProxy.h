//
//  RCServerProxy.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/2.
//

#ifndef RCServerProxy_h
#define RCServerProxy_h

#pragma once
//#include "RCProtocol.h"
#include "ConsoleProxy.h"

#include <boost/timer.hpp>


#define ECC_KEY_LEN   24

class CRCSvrProxy  : public CConsoleProxy
{
public:
    CRCSvrProxy(UINT nIP,USHORT nPort);
    ~CRCSvrProxy(void);
public:

    virtual bool OnPreSendPacketEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CNetPacket* pPacket) override;
    virtual bool OnProReceivedPacketEventHandle(PTR_NET_ENDPOINT_INTERFACE pNetEndpointInterface, CNetPacket* pNetPacket) override;
    virtual bool OnReceivedPacketEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CNetPacket* pPacket);
    virtual bool OnEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint, SEVERITY_LEVEL nSeverity, SERVICE_EVENT nEvent) override;
    virtual bool OnReconnectEventHandle(PTR_NET_ENDPOINT_INTERFACE) override;
    //virtual bool OnCloseEventHandle(PTR_NET_ENDPOINT_INTERFACE);

protected:   // ”¶¥ ˝æ›∞¸


private:
    void InitECCKey();

    void OnAESEncipherKey(CDataPacket* pDataPacket );

    void SetAESEncipherKey(const UCHAR* pKey);

    RCP::EXCEPTION_STATUS SyncSend(CDataPacket & refSend, PTR_WAIT_TASK ptrWaitTask, std::shared_ptr<CDataPacket>& refAckPacket);
private:


public:
    struct aes_ctx          m_objAESEncipherKey;             ///<aesº”√‹∂‘œÛ,”√”⁄º”√‹∑¢ÀÕ ˝æ›
private:
    // œ¬√Êµƒ≥Ã–Ú «”√”⁄ECCº”Ω‚√‹µƒkey
    // œ¬√Êµƒ3∏ˆKey”√”⁄º”√‹
    char                        m_szPublicKeyX[ECC_KEY_LEN];        ///< ECCπ≤”–KEYµƒX≤ø∑÷
    char                        m_szPublicKeyY[ECC_KEY_LEN];        ///< ECCπ≤”–KEYµƒY≤ø∑÷
    char                        m_szLittleKey[ECC_KEY_LEN];           ///< ECC–°KEY

    // œ¬√Êµƒ3∏ˆKey”√”⁄Ω‚√‹
    char                        m_szRKeyX[ECC_KEY_LEN];
    char                        m_szRKeyY[ECC_KEY_LEN];
    char                        m_szPrivateKey[ECC_KEY_LEN];
    //DECLARE_MEMBER_AND_METHOD(CString, m_strAccount, Account)      ///< ’À∫≈
    DECLARE_MEMBER_AND_METHOD(I64, m_nID, ID);
    DECLARE_MEMBER_AND_METHOD(unsigned int, m_nType, Type);  ///<@see COST_CLIENT_TYPE
    boost::timer               m_objSendingLastTime;
};

using CRCSvrProxyPtr = std::shared_ptr<CRCSvrProxy>;

#endif

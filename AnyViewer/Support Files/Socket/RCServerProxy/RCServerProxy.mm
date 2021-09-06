//
//  RCServerProxy.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/2.
//


#include "RCServerProxy.h"
#include <time.h>     //包含头文件time
//#include "CDataPacket.h"
//#include "resource.h"
#include "SDPMessageBus.h"
#include "GlobeFunction.h"
#include "Typedef.h"
#include "Log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int HEART_TIMEOUT = 120; //心跳超时

extern CMessageBus& GetMessageBus();

/*-------------------CRCSvrProxy-------------------------*/

CRCSvrProxy::CRCSvrProxy(
    UINT nIP
    , USHORT nPort)
    : CConsoleProxy(nIP, nPort)
{
    //SetIP(nIP);
    //SetPort(nPort);
    InitECCKey();
}


CRCSvrProxy::~CRCSvrProxy(void)
{
}



//------------------------------------------------------------------------------------
//函数名称： CRCSvrProxy::InitECCKey()
/// @brief 初始化用于ECC加密的KEY
///
/// @author 黄丽云
//------------------------------------------------------------------------------------
void CRCSvrProxy::InitECCKey()
{
    std::string strKey = ("FDEAB7BEB6119B28AF2C8069690DFD9E31826941CE38AB49");
    assert(strKey.size() == ECC_KEY_LEN * 2);
    ConvertHexTextToBin(strKey, m_szPublicKeyX, ECC_KEY_LEN);

    strKey = ("310B70A07716300EFC2D6761B3975E2AEC4E4C6097A7D12C");
    assert(strKey.size() == ECC_KEY_LEN * 2);
    ConvertHexTextToBin(strKey, m_szPublicKeyY, ECC_KEY_LEN);

    strKey = ("8E0A669C5A185F21119BC3D5CFEB11A293EAB39325AC9D48");
    assert(strKey.size() == ECC_KEY_LEN * 2);
    ConvertHexTextToBin(strKey, m_szLittleKey, ECC_KEY_LEN);

    strKey = ("EBDCA421B0A437B7A059D3EFC94B353B771F84DC09B6FFD6");
    assert(strKey.size() == ECC_KEY_LEN * 2);
    ConvertHexTextToBin(strKey, m_szRKeyX, ECC_KEY_LEN);

    strKey = ("A2564A78C9C6E3CF7983FDF89F0F66BC9927B2CAC98973E4");
    assert(strKey.size() == ECC_KEY_LEN * 2);
    ConvertHexTextToBin(strKey, m_szRKeyY, ECC_KEY_LEN);

    strKey = ("D77A7BEAE1724E0CBAB6B3412A3A4DCFC9C3BC256EF88920");
    assert(strKey.size() == ECC_KEY_LEN * 2);
    ConvertHexTextToBin(strKey, m_szPrivateKey, ECC_KEY_LEN);

#ifdef _DEBUG
    UCHAR szInputPlaintext[47] = "D77A7BEAE1724E0CBAB6B3412A3A4DCFC9C3BC256EF889";
    UCHAR szOutputCiphertext[49] = { 0 };
    UCHAR szOutputPlaintext[49] = { 0 };
    ecc_enc((unsigned char *)m_szPublicKeyX, (unsigned char *)m_szPublicKeyY, (unsigned char *)m_szLittleKey,
        szInputPlaintext, 46, szOutputCiphertext, 49);

    ecc_dec((unsigned char *)m_szRKeyX, (unsigned char *)m_szRKeyY, (unsigned char *)m_szPrivateKey,
        szOutputCiphertext, 48, szOutputPlaintext, 49);

#endif
}


void CRCSvrProxy::SetAESEncipherKey(const UCHAR* pKey)
{
    memset(&m_objAESEncipherKey, 0, sizeof(m_objAESEncipherKey));
    aes_set_key(&m_objAESEncipherKey, pKey, AES_KEY_LEN);
}




// ********************************************************************************
/// <summary>
/// 同步发送
/// </summary>
/// <param name="refSend"></param>
/// <param name="ptrWaitTask"></param>
/// <param name="refAckPacket"></param>
/// <returns>EXCEPTION_STATUS</returns>
/// <created>Leiyz,2019/8/6</created>
/// <changed>Leiyz,2019/8/28</changed>
// ********************************************************************************
RCP::EXCEPTION_STATUS CRCSvrProxy::SyncSend(
    CDataPacket & refSend
    , PTR_WAIT_TASK ptrWaitTask
    , std::shared_ptr<CDataPacket>& refAckPacket)
{
    RCP::EXCEPTION_STATUS nResult = RCP::ES_SEND_FAILED;

    if (Send(&refSend))
    {
        refAckPacket = m_objEndpoint->WaitAck(ptrWaitTask);

        if (nullptr != refAckPacket)
        {
            nResult = RCP::ES_SUCCESS;
        }
        else
        {
            nResult = RCP::ES_TIME_OUT;
        }
    }

    return nResult;
}



void CRCSvrProxy::OnAESEncipherKey(CDataPacket* pDataPacket)
{
    assert(nullptr != pDataPacket);
    if (nullptr != pDataPacket)
    {
        RCP::AES_ENCIPHER_KEY_REQUEST* pRequest = (RCP::AES_ENCIPHER_KEY_REQUEST*)pDataPacket->GetHeaderPtr();
        ecc_dec((unsigned char *)m_szRKeyX
            , (unsigned char *)m_szRKeyY
            , (unsigned char *)m_szPrivateKey
            , (unsigned char *)pRequest->szKeyData
            , 48
            , (unsigned char *)pRequest->szKeyData
            , 48);

        SetAESEncipherKey((unsigned char *)pRequest->szKeyData);
        SendCommonResponse(pDataPacket, true);
        SetConversationStatus(CConsoleProxy::RPS_ESTABLISHMENT);
    }
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
bool CRCSvrProxy::OnPreSendPacketEventHandle(
    PTR_NET_ENDPOINT_INTERFACE /*pEndpoint*/
    , CNetPacket * pPacket)
{
    CDataPacket* pDataPacket = static_cast<CDataPacket*>(pPacket);

    CHECK_POINTER_EX(pDataPacket, false);
    pDataPacket->Encipher(m_objAESEncipherKey);
    pDataPacket->CalculateCRC32();
    assert(pDataPacket->VerifyCRC32());

    m_objSendingLastTime.restart();

    return true;
}

//--------------------------------------------------------------------------------
/// <summary>
/// 刚接收到数据包,对数据包进行校验和解码处理
/// </summary>
/// <param name=""></param>
/// <param name="pNetPacket"></param>
/// <returns></returns>
/// <created>Andy,2020/3/18</created>
/// <changed>Andy,2020/3/18</changed>
//--------------------------------------------------------------------------------
bool CRCSvrProxy::OnProReceivedPacketEventHandle(
    PTR_NET_ENDPOINT_INTERFACE /*pNetEndpointInterface*/
    , CNetPacket * pNetPacket)
{
    CDataPacket* pDataPacket = dynamic_cast<CDataPacket*>(pNetPacket);

    CHECK_POINTER_EX(pNetPacket, false);

    bool bResult = pDataPacket->VerifyCRC32();

    assert(bResult);

    if (bResult && RCP::MT_AES_ENCIPHER_KEY != pDataPacket->GetPacketType())
    {
        pDataPacket->Decipherer(m_objAESEncipherKey);
        bResult = pDataPacket->IsValid();
    }

    return bResult;
}

bool CRCSvrProxy::OnReceivedPacketEventHandle(
    PTR_NET_ENDPOINT_INTERFACE pEndpoint
    , CNetPacket* pPacket)
{
    bool bResult = OnProReceivedPacketEventHandle(pEndpoint, pPacket);

    if (!bResult)
    {
        return bResult;
    }

    CDataPacket* pDataPacket = dynamic_cast<CDataPacket*>(pPacket);   //°²È«µÄÏòÏÂÀàÐÍ×ª»¯
    
    CHECK_POINTER_EX(pDataPacket,false);

    const U16 uMsgFlag = (U16)pDataPacket->GetPacketFlag();
    
    if ((uMsgFlag & OT_RESPONSE) == OT_RESPONSE)
    { //应答包
        
        switch (pDataPacket->GetPacketType())
        {
        case RCP::MT_REGIST_CLIENT:
            OnRegistResponse(pDataPacket);
            break;

/*        case RCP::MT_REGIST_CLIENT:
            OnRegistResponse(pDataPacket);
            break;*/
        }
    }
    else
    {
        switch (pDataPacket->GetPacketType())
        {
        case RCP::MT_AES_ENCIPHER_KEY:
            OnAESEncipherKey(pDataPacket);
            break;
        }
    }

    CMessageBus& refMessageBus = GetMessageBus();

    refMessageBus.SendReq<void, CDataPacket*>(LMBS_RECEIVED_RC_PACKET, std::forward <CDataPacket*>(pDataPacket));

    return bResult;
}

bool CRCSvrProxy::OnEventHandle(
    PTR_NET_ENDPOINT_INTERFACE /*pEndpoint*/
    , SEVERITY_LEVEL /*nSeverity*/
    , SERVICE_EVENT nEvent)
{
    switch (nEvent)
    {
    case SE_CONNECTED:
    {
        m_nConversationStatus = (RPS_ESTABLISHMENT);
    }
    break;

    case SE_CHECK_HEART:
    {
        if (m_objSendingLastTime.elapsed() >= HEART_TIMEOUT)
        {
            //TRACE("send heart packet\n");
            LOG_DEBUG("send heart packet");
            SendCommonRequest(RCP::MT_HEARTBEAT);
            
        }
    }
    break;

    case SE_FAILT_TO_CONNECT:
    {
        m_nConversationStatus = RPS_CLOSE;

        CMessageBus& refMessageBus = GetMessageBus();

        refMessageBus.SendReq<void>(LMBS_CLOSE_RCSERVER_CONNECTION);

        char szBuffer[20] = { 0 };
        
//        printf("Try to re-connect the RC server(%s:%d)...", FormatHostIPAddressAnsi(GetIP(), szBuffer), GetPort());
//        LOG_ERROR("Try to re-connect the RC server(%s:%d)...", FormatHostIPAddressAnsi(GetIP(), szBuffer), GetPort());
    }
    break;
    }

    return true;
}

bool CRCSvrProxy::OnReconnectEventHandle(PTR_NET_ENDPOINT_INTERFACE)
{
    CMessageBus& refMessageBus = GetMessageBus();

    refMessageBus.SendReq<void>(LMBS_RECONNECT_RC_SERVER);

    return true;
}


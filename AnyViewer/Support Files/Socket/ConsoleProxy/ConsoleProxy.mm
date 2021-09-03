//
//  ConsoleProxy.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//

#include "ConsoleProxy.h"
#include "CDataPacket.h"
#include "SDPMessageBus.h"
#include "RCProtocol.h"
#include "MessageBusManager.h"
#include "FuncHeader.h"
#include <time.h>     //包含头文件time

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int HEART_TIMEOUT = 240; //心跳超时

#pragma comment( lib, "ssleay32.lib" )
#pragma comment( lib, "libeay32.lib" )

extern CMessageBus& GetMessageBus();


CConsoleEndpoint::CConsoleEndpoint(CNetEndpointEventSink* pNetEndpointEventSink)
    :CNetEndpoint(pNetEndpointEventSink)
{
}

bool CConsoleEndpoint::Send(CNetPacket* pPacket)
{
    if (!GetNetEndpointEventSink()->OnPreSendPacketEventHandle(shared_from_this(), pPacket))
    {
        return false;
    }

    return CNetEndpoint::Send(pPacket);
}

bool CConsoleEndpoint::SendCommonResponse(CDataPacket* pDataPacket, bool bStatus, const U64 nStatusCode)
{
    CDataPacket objResponsePacket;
    objResponsePacket.InitialT<COMMON_RESPONSE>(
        pDataPacket->GetPacketId()
        , (WORD)pDataPacket->GetPacketType()
        , BYTE(OT_RESPONSE | (bStatus ? OR_FAILURE : OR_SUCCESS)));

    COMMON_RESPONSE* pResponsePacket = (COMMON_RESPONSE*)objResponsePacket.GetHeaderPtr();
    pResponsePacket->nStatusCode = ntohq(nStatusCode);

    return Send(&objResponsePacket);
}

/*-------------------CConsoleProxy-------------------------*/

CConsoleProxy::CConsoleProxy(
    UINT nIP
    , USHORT nPort)
    : m_nConversationStatus(RPS_INVALID)
{
    SetIP(nIP);
    SetPort(nPort);
}


CConsoleProxy::~CConsoleProxy(void)
{
}


// ********************************************************************************
/// <summary>
///
/// </summary>
/// <param name="pDataPacket"></param>
/// <created>Andy,2020/10/21</created>
/// <changed>Andy,2020/10/21</changed>
// ********************************************************************************
void CConsoleProxy::OnRegistResponse(CDataPacket* pDataPacket)
{
    //COMMON_RESPONSE* pRequest = (COMMON_RESPONSE*)pDataPacket->GetHeaderPtr();

    if ((pDataPacket->GetPacketFlag() & OR_SUCCESS) == OR_SUCCESS)
    {
        m_nConversationStatus = RPS_REGISTERED;
    }
}



//------------------------------------------------------------------------------------
//函数名称： CConsoleProxy::Connect()
/// @brief 连接到服务器
///
/// @return true->成功,false->失败
/// @author 黄丽云
//------------------------------------------------------------------------------------
bool CConsoleProxy::Connect()
{
    bool bResult = Start();
    return bResult;
}

//--------------------------------------------------------------------------------
/// <summary>
/// 重新连接到服务器
/// </summary>
/// <returns>true->成功,false->失败</returns>
/// <created>Andy,2020/3/18</created>
/// <changed>Andy,2020/3/18</changed>
//--------------------------------------------------------------------------------
bool CConsoleProxy::ReConnect()
{
    Stop();
    return Connect();
}

// ********************************************************************************
/// <summary>
/// 向控制服务器注册
/// </summary>
/// <param name="refRCClientInfo">客户端信息</param>
/// <param name="refDeviceID">返回设备ID</param>
/// <returns>true:成功，false:失败</returns>
/// <created>Andy,2020/10/20</created>
/// <changed>Andy,2020/10/20</changed>
// ********************************************************************************
bool CConsoleProxy::Regist(const RCP::CRCClientInfo& refRCClientInfo)
{
    bool bResult = false;

    assert(RPS_ESTABLISHMENT == GetConversationStatus());

    if (RPS_ESTABLISHMENT == GetConversationStatus())
    {
        CDataPacket objReuestPacket;
        const int PACKET_ID = GetMsgId();

        objReuestPacket.InitialJsonT<MESSAGE_HEADER>(
            PACKET_ID
            , RCP::MT_REGIST_CLIENT
            , OT_REQUEST
            , &refRCClientInfo);

        bResult = Send(&objReuestPacket);
    }

    return bResult;
}

// ********************************************************************************
/// <summary>
/// 更新客户端信息
/// </summary>
/// <param name="refRCClientInfo"></param>
/// <returns></returns>
/// <created>Andy,2020/10/22</created>
/// <changed>Andy,2020/10/22</changed>
// ********************************************************************************
bool CConsoleProxy::UpdateClient(const RCP::CRCClientInfo& refRCClientInfo)
{
    bool bResult = false;

    assert(RPS_REGISTERED == GetConversationStatus());

    if (RPS_REGISTERED == GetConversationStatus())
    {
        CDataPacket objReuestPacket;
        const int PACKET_ID = GetMsgId();

        objReuestPacket.InitialJsonT<MESSAGE_HEADER>(
            PACKET_ID
            , RCP::MT_UPDATE_CLIENT_INFO
            , OT_REQUEST
            , &refRCClientInfo);

        bResult = Send(&objReuestPacket);
    }

    return bResult;
}



// ********************************************************************************
/// <summary>
/// 发送通用应答
/// </summary>
/// <param name="pDataPacket"></param>
/// <param name="bStatus"></param>
/// <created>Andy,2020/11/26</created>
/// <changed>Andy,2020/11/26</changed>
// ********************************************************************************
void CConsoleProxy::SendCommonResponse(CDataPacket* pDataPacket, bool bStatus, const U64 nStatusCode)
{
    CDataPacket objResponsePacket;
    objResponsePacket.InitialT<COMMON_RESPONSE>(
        pDataPacket->GetPacketId()
        , (WORD)pDataPacket->GetPacketType()
        , BYTE(OT_RESPONSE | (bStatus ? OR_FAILURE : OR_SUCCESS)));

    COMMON_RESPONSE* pResponsePacket = (COMMON_RESPONSE*)objResponsePacket.GetHeaderPtr();
    pResponsePacket->nStatusCode = ntohq(nStatusCode);

    Send(&objResponsePacket);
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
RCP::EXCEPTION_STATUS CConsoleProxy::SyncSend(
    CDataPacket& refSend
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






//--------------------------------------------------------------------------------
/// <summary>
/// 发送通用请求消息
/// </summary>
/// <param name="nMsg">消息类型</param>
/// <param name="nStatus">参数代码</param>
/// <created>Andy,2019-03-25</created>
/// <changed>Andy,2019-03-25</changed>
//--------------------------------------------------------------------------------
bool CConsoleProxy::SendCommonRequest(UINT nMsg, U64 nStatus)
{
    CDataPacket objRequestPacket;
    objRequestPacket.InitialT<COMMON_REQUEST>(
        GetMsgId()
        , (WORD)nMsg
        , WORD(OT_REQUEST));

    COMMON_REQUEST* pRequestPacket = (COMMON_REQUEST*)objRequestPacket.GetHeaderPtr();
    pRequestPacket->nStatusCode = ntohq(nStatus);

    return Send(&objRequestPacket);
}


// ********************************************************************************
/// <summary>
/// 连接目标设备
/// </summary>
/// <param name="refConnRequest">连接请求</param>
/// <param name="nTimeout">超市时间(单位：秒，默认10分钟)</param>
/// <returns></returns>
/// <created>Andy,2020/10/22</created>
/// <changed>Andy,2020/10/23</changed>
// ********************************************************************************
bool CConsoleProxy::ConnectRemoteClient(const RCP::CConnectionRequest& refConnRequest)
{
    CDataPacket objReuestPacket;
    const int PACKET_ID = GetMsgId();

    objReuestPacket.InitialJsonT<MESSAGE_HEADER>(
        PACKET_ID
        , RCP::MT_CONNECT
        , OT_REQUEST
        , &refConnRequest);

    return Send(&objReuestPacket);
}

// ********************************************************************************
/// <summary>
/// 同步连接目标设备
/// </summary>
/// <param name="refConnRequest"></param>
/// <param name="nTimeout"></param>
/// <returns></returns>
/// <created>Andy,2021/7/19</created>
/// <changed>Andy,2021/7/19</changed>
// ********************************************************************************
bool CConsoleProxy::SyncConnectRemoteClient(
    const RCP::CConnectionRequest& refConnRequest
    , int& nStatusCode1
    , int& nStatusCode2
    , const int nTimeout)
{
    CDataPacket objReuestPacket;
    const int PACKET_ID = GetMsgId();

    objReuestPacket.InitialJsonT<MESSAGE_HEADER>(
        PACKET_ID
        , RCP::MT_CONNECT
        , OT_REQUEST
        , &refConnRequest);

    std::shared_ptr<CWaitingResponseTask<CDataPacket>> objTask =
        m_objEndpoint->AppendWaitingResponseTask(PACKET_ID, nTimeout * 1000);

    std::shared_ptr<CDataPacket> ptrAckPacket = nullptr;
    RCP::EXCEPTION_STATUS nReCode = SyncSend(objReuestPacket, objTask, ptrAckPacket);

    bool bResult = (RCP::ES_SUCCESS == nReCode);
    RCP::COMMON_RESPONSE_EX* pRequest = (RCP::COMMON_RESPONSE_EX*)ptrAckPacket->GetHeaderPtr();

    nStatusCode1 = NTOHL(pRequest->nStatusCode1);
    nStatusCode2 = NTOHL(pRequest->nStatusCode2);

    return bResult;
}

// ********************************************************************************
/// <summary>
/// 发送认证消息到服务器
/// </summary>
/// <param name="refRequest">指向认证请求对象的参考</param>
/// <param name="nTimeout">超市时间(单位：秒，默认10分钟)</param>
/// <returns></returns>
/// <created>Andy,2020/10/23</created>
/// <changed>Andy,2020/10/23</changed>
// ********************************************************************************
bool CConsoleProxy::Authenticate(const RCP::CAuthenticationRequest& refRequest/*, const int nTimeout*/)
{
    CDataPacket objReuestPacket;
    const int PACKET_ID = GetMsgId();

    objReuestPacket.InitialJsonT<MESSAGE_HEADER>(
        PACKET_ID
        , RCP::MT_AUTHENTICATE
        , OT_REQUEST
        , &refRequest);

    return Send(&objReuestPacket);
}

// ********************************************************************************
/// <summary>
/// 同步发送认证消息到服务器
/// </summary>
/// <param name="refRequest">指向认证请求对象的参考</param>
/// <param name="nTimeout">超市时间(单位：秒，默认10分钟)</param>
/// <returns>true:成功，false:失败</returns>
/// <created>Andy,2021/7/19</created>
/// <changed>Andy,2021/7/19</changed>
// ********************************************************************************
bool CConsoleProxy::SyncAuthenticate(
    const RCP::CAuthenticationRequest& refRequest
    , U32& nStatusCode1
    , U32& nStatusCode2
    , const int nTimeout)
{
    bool bResult = false;
    CDataPacket objReuestPacket;
    const int PACKET_ID = GetMsgId();

    objReuestPacket.InitialJsonT<MESSAGE_HEADER>(
        PACKET_ID
        , RCP::MT_AUTHENTICATE
        , OT_REQUEST
        , &refRequest);

    std::shared_ptr<CWaitingResponseTask<CDataPacket>> objTask =
        m_objEndpoint->AppendWaitingResponseTask(PACKET_ID, nTimeout * 1000);
    std::shared_ptr<CDataPacket> ptrAckPacket = nullptr;
    const RCP::EXCEPTION_STATUS nRetCode = SyncSend(objReuestPacket, objTask, ptrAckPacket);
    
    if (RCP::ES_SUCCESS == nRetCode)
    {
        RCP::COMMON_RESPONSE_EX* pRequest = (RCP::COMMON_RESPONSE_EX*)ptrAckPacket->GetHeaderPtr();

        nStatusCode1 = ntohl(pRequest->nStatusCode1);
        nStatusCode2 = ntohl(pRequest->nStatusCode2);
        bResult = (ptrAckPacket->GetPacketFlag() & OR_SUCCESS);
    }

    return bResult;
}

// ********************************************************************************
/// <summary>
/// 发送改变通信方式请求
/// </summary>
/// <param name="nSessionID">会话ID</param>
/// <returns></returns>
/// <created>Andy,2021/1/7</created>
/// <changed>Andy,2021/1/7</changed>
// ********************************************************************************
bool CConsoleProxy::SendChangeCommModeRequest(const U32 nSessionID)
{
    CDataPacket objRequestPacket;
    objRequestPacket.InitialT<RCP::CHANGE_COMM_MODE_REQUEST>(
        GetMsgId()
        , (WORD)RCP::MT_CHANGE_COMMUNICATION_MODE
        , WORD(OT_REQUEST));

    RCP::CHANGE_COMM_MODE_REQUEST* pRequestPacket = (RCP::CHANGE_COMM_MODE_REQUEST*)objRequestPacket.GetHeaderPtr();

    pRequestPacket->nSessionID = ntohl(nSessionID);
    pRequestPacket->nMode = 0;

    return Send(&objRequestPacket);
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
bool CConsoleProxy::OnPreSendPacketEventHandle(
    PTR_NET_ENDPOINT_INTERFACE /*pEndpoint*/
    , CNetPacket* pPacket)
{
    CDataPacket* pDataPacket = static_cast<CDataPacket*>(pPacket);

    CHECK_POINTER_EX(pDataPacket, false);

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
bool CConsoleProxy::OnProReceivedPacketEventHandle(
    PTR_NET_ENDPOINT_INTERFACE /*pNetEndpointInterface*/
    , CNetPacket* /*pNetPacket*/)
{
    //CDataPacket* pDataPacket = dynamic_cast<CDataPacket*>(pNetPacket);
    

    return true;
}

bool CConsoleProxy::OnReceivedPacketEventHandle(
    PTR_NET_ENDPOINT_INTERFACE pEndpoint
    , CNetPacket* pPacket)
{
    bool bResult = OnProReceivedPacketEventHandle(pEndpoint, pPacket);

    if (!bResult)
    {
        return bResult;
    }

    CDataPacket* pDataPacket = dynamic_cast<CDataPacket*>(pPacket);   //°²È«µÄÏòÏÂÀàÐÍ×ª»¯

    CHECK_POINTER_EX(pDataPacket, false);

    const U16 uMsgFlag = (U16)pDataPacket->GetPacketFlag();

    if ((uMsgFlag & OT_RESPONSE) == OT_RESPONSE)
    { //应答包

        switch (pDataPacket->GetPacketType())
        {
        case RCP::MT_REGIST_CLIENT:
            OnRegistResponse(pDataPacket);
            break;
        }
    }
    else
    {
    /*    switch (pDataPacket->GetPacketType())
        {
        }*/
    }

    CMessageBus& refMessageBus = GetMessageBus();

    refMessageBus.SendReq<void, CDataPacket*>(GetMsgType(), &(*pDataPacket));

    return bResult;
}

bool CConsoleProxy::OnEventHandle(
    PTR_NET_ENDPOINT_INTERFACE /*pEndpoint*/
    , SEVERITY_LEVEL /*nSeverity*/
    , SERVICE_EVENT nEvent
    , const char * /*lpTips*/)
{
    char szBuffer[20] = { 0 };

    switch (nEvent)
    {
    case SE_CONNECTED:
    {
//        LOG_DEBUG("Connect the console server(%s:%d) successfully!", FormatHostIPAddressAnsi(GetIP(), szBuffer), GetPort());
//        printf("Connect the console server(%s:%d) successfully!", FormatHostIPAddressAnsi(GetIP(), szBuffer), GetPort());
        m_nConversationStatus = (RPS_ESTABLISHMENT);
    }
    break;

    case SE_CHECK_HEART:
    {
        if (m_objSendingLastTime.elapsed() >= HEART_TIMEOUT)
        {
            //TRACE("send heart packet\n");
            SendCommonRequest(RCP::MT_HEARTBEAT);
        }
    }
    break;

    case SE_FAILT_TO_CONNECT:
    {
        m_nConversationStatus = RPS_CLOSE;
//        LOG_ERROR("Try to re-connect the console server(%s:%d)...", FormatHostIPAddressAnsi(GetIP(), szBuffer), GetPort());
//        printf("Try to re-connect the console server(%s:%d)...", FormatHostIPAddressAnsi(GetIP(), szBuffer), GetPort());
    }
    break;
    }

    CMessageBus& refMessageBus = GetMessageBus();

    refMessageBus.SendReq<void, SERVICE_EVENT>(GetEventType(), std::forward <SERVICE_EVENT>(nEvent));

    return true;
}

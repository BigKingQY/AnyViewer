//
//  CVNCProxy.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/9.
//

#include "CVNCProxy.h"
#include "CPrjSettings.h"
//#include "PixelFormat.h"
//#include "Point.h"
#include "Log.h"
#include "FuncHeader.h"
#include "GlobeFunction.h"
#include "PixelFormat.h"
#include "CCustomThreadPool.h"
#include "CNetTransactionEngine.h"

// ¥¥Ω®“ª∏ˆœﬂ≥Ã»ŒŒÒ
extern bool CreateThreadTask(const THREAD_POOL_FUN& refThreadFun, CTaskSink* pTaskSink = nullptr);


const unsigned int CONNECTION_TIMEOUT_MAX = 3;    ///< ◊Ó¥Û¡¨Ω”≥¨ ±„–÷µ(µ•Œª:√Î)


// ********************************************************************************
/// <summary>
/// 创建一个线程任务
/// </summary>
/// <param name="refThreadFun"></param>
/// <param name="pTaskSink"></param>
/// <returns></returns>
/// <created>Andy,2021/7/29</created>
/// <changed>Andy,2021/7/29</changed>
// ********************************************************************************
bool CreateThreadTask(const THREAD_POOL_FUN& refThreadFun, CTaskSink* pTaskSink)
{
    return GetTransactionInstance()->CreateThreadTask(refThreadFun, pTaskSink);
}


CVNCProxy::CVNCProxy(
    const U32 nSessionID /*= 0*/
    , const U32 nPeerID/* = 0*/
    , const U32 nPeerIP/* = 0*/
    , const U16 nPeerPort/* = 0*/
    , const U32 nCommunicationMode/* = 0*/
    , const U32 nRoleType/* = RCT_CONTROLLER*/
    , const unsigned int nProtocolVer)
    : m_nSessionID(nSessionID)
    , m_nPeerID(nPeerID)
    , m_nPeerIP(nPeerIP)
    , m_nPeerPort(nPeerPort)
    , m_nCommunicationMode(nCommunicationMode)
    , m_nRoleType(nRoleType)
    , m_nProtocolVer(nProtocolVer)
{
}

CVNCProxy::~CVNCProxy()
{
    if (nullptr != m_pVNCTCPClient)
    {
        m_pVNCTCPClient->Stop();
        m_pVNCTCPClient = nullptr;
    }

    if (nullptr != m_pVNCUDPClient)
    {
        m_pVNCUDPClient->Stop();
        m_pVNCUDPClient = nullptr;
    }
}

// ********************************************************************************
/// <summary>
///
/// </summary>
/// <returns></returns>
/// <created>Andy,2021/4/2</created>
/// <changed>Andy,2021/4/2</changed>
// ********************************************************************************
double CVNCProxy::GetReceivingLasttime()
{
    double nResult = 0.0;

    if (nullptr != m_pVNCTCPClient)
    {
        nResult = m_pVNCTCPClient->GetReceivingLasttime();
    }
    else if (nullptr != m_pVNCUDPClient)
    {
        nResult = m_pVNCUDPClient->GetReceivingLasttime();
    }

    return nResult;
}

// ********************************************************************************
/// <summary>
/// µ√µΩ¡¨Ω”œ˚∫ƒµƒ ±º‰
/// </summary>
/// <returns></returns>
/// <created>Andy,2021/6/18</created>
/// <changed>Andy,2021/6/18</changed>
// ********************************************************************************
double CVNCProxy::GetConnecitonTime()
{
    double nResult = 0.0;

    if (nullptr != m_pVNCTCPClient)
    {
        nResult = m_pVNCTCPClient->GetConnectionTime();
    }
    else if (nullptr != m_pVNCUDPClient)
    {
        nResult = m_pVNCUDPClient->GetConnectionTime();
    }

    return nResult;
}

CONVERSATION_STATE CVNCProxy::GetConversationStatus()
{
    CONVERSATION_STATE nResult = RS_INVALID;

    if (nullptr != m_pVNCTCPClient)
    {
        nResult = m_pVNCTCPClient->GetConversationStatus();
    }
    else if (nullptr != m_pVNCUDPClient)
    {
        nResult = m_pVNCUDPClient->GetConversationStatus();
    }

    return nResult;
}


// ********************************************************************************
/// <summary>
/// ◊¢≤·µΩ–≈ π∑˛ŒÒ∆˜,÷˜“™ƒøµƒ «∏ÊÀﬂ–≈ π∑˛ŒÒ∆˜NAT…Ë±∏µƒIPµÿ÷∑∫Õ∂Àø⁄
/// ◊¢≤·∑˛ŒÒ∆˜µƒ∂Àø⁄ «RC÷˜øÿ∑˛ŒÒ∆˜∂Àø⁄+1,’‚ «“ª∏ˆ‘º∂®
/// </summary>
/// <returns></returns>
/// <created>Andy,2021/1/5</created>
/// <changed>Andy,2021/1/5</changed>
// ********************************************************************************
//bool CVNCProxy::RegistUDPHoleServer()
//{
//    bool bResult = false;
//    CPrjSettings* pPrjSettings = GetPrjSettings();
//    CString strIP(pPrjSettings->GetServerIP().c_str());
//
//    if (!ValidIP(CT2A(strIP)))
//    {
//        std::string astrIP((char*)CT2A(strIP));
//
//        strIP = QueryDomainIP(astrIP).c_str();
//    }
//
//    // ◊¢“‚UDP∑˛ŒÒ∆˜µƒ∂Àø⁄ «TCP∂Àø⁄+1
//    m_pVNCUDPClient = std::make_shared<CVNCClient<CVNCUDPEndpoint>>(
//        IDE_IPv4_to_i(std::string(CT2A(strIP)))
//        , pPrjSettings->GetPort() + 1);
//
//    if (nullptr != m_pVNCUDPClient)
//    {
//        m_pVNCUDPClient->SetEndpointEventSink(this);
//        bResult = m_pVNCUDPClient->Initial();
//
//        if (bResult)
//        {
//            m_pEndpoint = m_pVNCUDPClient->GetEndpoint();
//
//
//            CNetUDPDataTransfer* pUDPDataTransfer = m_pVNCUDPClient->GetEndpoint()->GetTransfer();
//
//            if (nullptr != pUDPDataTransfer)
//            {
//                pUDPDataTransfer->Connect();
//                pUDPDataTransfer->SetIP(IDE_IPv4_to_i(std::string(CT2A(strIP))));
//                pUDPDataTransfer->SetPort(pPrjSettings->GetPort() + 1);
//
//                CVNCPacket objRequestPacket;
//
//                objRequestPacket.InitialT<VNCP::VNC_MESSAGE_HEADER, VNCP::REGIST_TO_HOLE_SERVER_REQUEST>(
//                    VNCP::MT_REGIST_TO_UDP_HOLE_SERVER
//                    , GetSessionID()
//                    , VNCP::OT_REQUEST
//                    , 0);
//
//                VNCP::REGIST_TO_HOLE_SERVER_REQUEST* pRequestPacket = (VNCP::REGIST_TO_HOLE_SERVER_REQUEST*)objRequestPacket.GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();
//
//                pRequestPacket->nDeviceID = ntohq(std::atoll(pPrjSettings->GetDeviceID().c_str()));
//
//                // ∑¢ÀÕ◊¢≤·«Î«Û£¨◊Ó∂‡¡¨–¯»˝¥Œ
//                int nCount = 3;
//                BYTE szBuffer[128] = { 0 };
//
//                do
//                {
//                    pUDPDataTransfer->Send(
//                        objRequestPacket.GetPacketBuffer().GetBufferHeadPtr()
//                        , objRequestPacket.GetPacketBuffer().GetDataLen());
//
//                    const int nByteRecved = pUDPDataTransfer->Recv(szBuffer, sizeof(szBuffer));
//
//                    if (nByteRecved > 0)
//                    {
//                        bResult = (((VNCP::VNC_MESSAGE_HEADER*)szBuffer)->nFlag & OR_SUCCESS);
//                        break;
//                    }
//
//                    ::Sleep(1000);
//                    nCount--;
//                } while (nCount > 0);
//            }
//        }
//    }
//
//    return bResult;
//}

bool CVNCProxy::OnEventHandle(
    PTR_NET_ENDPOINT_INTERFACE pEndpoint
    , SEVERITY_LEVEL nSeverity
    , SERVICE_EVENT nEvent)
{

    switch (nEvent)
    {
    case SE_CONNECTED:
    {
        // À‰»ª¥Ú∂¥∫Õ÷–◊™¡¨Ω”µƒ ±∫Ú£¨Õ®–≈…œ∑Ω∂º”–ø…ƒ‹∑¢ÀÕ¡¨Ω”£¨∂º”–ø…ƒ‹ª·≥…π¶£¨µ´ «≤ª»∑∂®£¨“Ú¥À
        // “Ú¥À–Ë“™∑¢ÀÕhelloœ˚œ¢
        SendHelloRequest();
    }
    break;

    case SE_FAILT_TO_CONNECT:
    {
        char szBuffer[20] = { 0 };

        LOG_ERROR("Try to re-connect the Controlled(%s:%d)...", FormatHostIPAddressAnsi(GetPeerIP(), szBuffer), GetPeerPort());
    }
    break;

    default:
        break;
    }

    return true;
}

// ********************************************************************************
/// <summary>
/// ≥…π¶Ω®¡¢Õ®µ¿ ¬º˛
/// </summary>
/// <created>Andy,2021/6/30</created>
/// <changed>Andy,2021/6/30</changed>
// ********************************************************************************
void CVNCProxy::OnEstablished()
{
    m_objDuration.restart();
    SetEstablishVNCConnection(true);
}

// ********************************************************************************
/// <summary>
/// ≤…”√TCP¡¨Ω”µΩƒø±Í…Ë±∏
/// </summary>
/// <param name="bTCP">true:≤…”√TCP,false:≤…”√UDP</param>
/// <returns></returns>
/// <created>Andy,2020/11/27</created>
/// <changed>Andy,2020/12/23</changed>
// ********************************************************************************
bool CVNCProxy::TCPConnect()
{
    CPrjSettings* pPrjSettings = GetPrjSettings();
    bool bResult = false;

    m_pVNCTCPClient = std::make_shared<CVNCClient<CVNCTCPEndpoint>>(GetPeerIP(), GetPeerPort());

    if (nullptr != m_pVNCTCPClient)
    {
        m_pVNCTCPClient->SetEndpointEventSink(this);
        bResult = m_pVNCTCPClient->Initial();

        if (bResult)
        {
//            m_pVNCTCPClient->SetConectionTimeout(CONNECTION_TIMEOUT_MAX);
            m_pEndpoint = m_pVNCTCPClient->GetEndpoint();
            bResult = m_pVNCTCPClient->Connect();
            
            {
                // ≤ªπ‹¡¨Ω”µƒΩ·π˚»Á∫Œ£¨∂º“™¡¢º¥∑¢ÀÕœ˚œ¢£¨ƒøµƒœ‘ æ¥∞ø⁄∫Õ∑÷≈‰◊¿√Ê£¨◊ˆ∫√¡¨Ω”◊™±≠£¨“≤ø…“‘
                // ‘⁄◊¿√Ê¥∞ø⁄œ‘ æ¡¨Ω”Ω¯∂»
                CreateThreadTask([this]()
                {
                    CMessageBus& refMessageBus = GetMessageBus();
                    refMessageBus.SendReq<void, CVNCProxyPtr>(
                        LMBS_ESTABLISH_VNC_CONNECTION
                        , std::forward <CVNCProxyPtr>(shared_from_this()));
                });


            }


            if (bResult)
            {
                //暂时不用
//                m_pEndpoint->GetTransfer()->EnableNaggleAlgorithm(false);

            }
            
            else if(GetCommunicationMode() != RCP::ST_RELAY)
            {
                // »Áπ˚¡¨Ω” ß∞‹£¨¡¢º¥Õ®÷™∑˛ŒÒ∆˜∏ƒ±‰Õ®–≈∑Ω ΩÕ®–≈∑Ω Ω
                m_pRCSvrProxy->SendChangeCommModeRequest(GetSessionID());
                
                // ±ÿ–Îµ˜”√Stop∫Ø ˝¿¥Õ£÷πøÕªß∂À£¨∑Ò‘ÚÀ¸Ω´≤ª∂œµƒ÷ÿ–¬≥¢ ‘¡¨Ω”
                m_pVNCTCPClient->Stop();
                m_pVNCTCPClient = nullptr;
            }
            
            char szBuffer[20] = { 0 };

            LOG_INFO("TCPConnect the Dst device (%s:%d) %s!"
                , FormatHostIPAddressAnsi(GetPeerIP(), szBuffer)
                , pPrjSettings->GetRFBPort()
                , bResult ? "successfully" : "unsuccessfully");
        }
    }

    return bResult;
}

// ********************************************************************************
/// <summary>
///
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/12/23</created>
/// <changed>Andy,2020/12/23</changed>
// ********************************************************************************
bool CVNCProxy::UDPConnect(const unsigned int nIP, const unsigned int nPort)
{
    bool bResult = false;

    if (nullptr != m_pVNCUDPClient)
    {
        SetPeerIP(nIP);
        SetPeerPort(nPort);
        m_pVNCUDPClient->SetIP(nIP);
        m_pVNCUDPClient->SetPort(nPort);
//        m_pVNCUDPClient->SetConectionTimeout(CONNECTION_TIMEOUT_MAX);
        bResult = m_pVNCUDPClient->Connect();

        if (bResult)
        {
            if (GetRoleType() == RCP::RCT_CONTROLLER)
            {
                CreateThreadTask([this]()
                {
                    CMessageBus& refMessageBus = GetMessageBus();
                    refMessageBus.SendReq<void, CVNCProxyPtr>(
                        LMBS_ESTABLISH_VNC_CONNECTION
                        , std::forward <CVNCProxyPtr>(shared_from_this()));
                }
                );
            }

        }
        else
        {
            // »Áπ˚¡¨Ω” ß∞‹£¨¡¢º¥Õ®÷™∑˛ŒÒ∆˜∏ƒ±‰Õ®–≈∑Ω ΩÕ®–≈∑Ω Ω
            m_pRCSvrProxy->SendChangeCommModeRequest(GetSessionID());

            // ±ÿ–Îµ˜”√Stop∫Ø ˝¿¥Õ£÷πøÕªß∂À£¨∑Ò‘ÚÀ¸Ω´≤ª∂œµƒ÷ÿ–¬≥¢ ‘¡¨Ω”
            m_pVNCUDPClient->Stop();
            m_pVNCUDPClient = nullptr;
        }

        char szBuffer[20] = { 0 };

        LOG_INFO("UDPConnect the Dst device (%s:%d) %s!"
            , FormatHostIPAddressAnsi(nIP, szBuffer)
            , nPort
            , bResult ? "successfully" : "unsuccessfully");

    }

    return bResult;
}


// ********************************************************************************
/// <summary>
/// ∆Ù∂Ø¥˙¿Ì
///
/// ∏˘æ›≤ªÕ¨µƒΩ«…´∫Õ≤ªÕ¨µƒÕ®–≈∑Ω Ω¿¥◊ˆœÏ”¶µƒ¥¶¿Ì
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/11/27</created>
/// <changed>Andy,2020/11/27</changed>
// ********************************************************************************
bool CVNCProxy::Start()
{
    bool bResult = true;
    CPrjSettings* pPrjSettings = GetPrjSettings();

    if (nullptr != m_pVNCTCPClient)
    {
        m_pVNCTCPClient->Stop();
        m_pEndpoint = nullptr;
        m_pVNCTCPClient = nullptr;
    }

    if (nullptr != m_pVNCUDPClient)
    {
        m_pVNCUDPClient->Stop();
        m_pEndpoint = nullptr;
        m_pVNCUDPClient = nullptr;
    }

    switch (GetCommunicationMode())
    {
    case RCP::ST_SRC_TO_DST:   ///< √∂æŸ≥£¡ø,‘¥œÚƒø±Í∑¢∆µƒ÷±¡¨
        if (GetRoleType() == RCP::RCT_CONTROLLER)
        {
            bResult = TCPConnect();
        }
        else
        {
            //∏√ª·ª∞∑Ω Ω£¨±ªøÿ÷∆’ﬂ÷ª–Ë“™æ≤æ≤µƒµ»¥˝øÿ÷∆’ﬂµƒ¡¨Ω”«Î«Û(‘⁄30193∂Àø⁄)
        }
        break;

    case RCP::ST_DST_TO_SRC:   ///< √∂æŸ≥£¡ø,ƒø±ÍœÚ‘¥∑¢∆µƒ÷±¡¨
        if (GetRoleType() == RCP::RCT_CONTROLLED)
        {
            bResult = TCPConnect();
        }
        else
        {
            //∏√ª·ª∞∑Ω Ω£¨øÿ÷∆’ﬂ÷ª–Ë“™æ≤æ≤µƒµ»¥˝±ªøÿ÷∆’ﬂµƒ¡¨Ω”«Î«Û(‘⁄30193∂Àø⁄)
        }
        break;

    case RCP::ST_HOLE:        ///< √∂æŸ≥£¡ø,¥Ú∂¥÷±¡¨
        //暂时不用打洞
//        bResult = RegistUDPHoleServer();
            bResult = false;
        break;

    case RCP::ST_RELAY:        ///< √∂æŸ≥£¡ø,÷–◊™
        bResult = TCPConnect();
        break;

    default:
        assert(false);
    }


    return bResult;
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕºÚµ•µƒœ˚œ¢
/// </summary>
/// <param name="nType">œ˚œ¢¿‡–Õ</param>
/// <returns></returns>
/// <created>Andy,2020/11/30</created>
/// <changed>Andy,2020/11/30</changed>
// ********************************************************************************
bool CVNCProxy::SendCommonRequest(const unsigned int nType)
{
    CHECK_POINTER_EX(m_pEndpoint, false);

    bool bResult = false;
    CVNCPacket objRequestPacket;
    
    objRequestPacket.Initial(nType, GetSessionID(), VNCP::OT_REQUEST, 0);
    bResult = m_pEndpoint->Send(&objRequestPacket);

    return bResult;
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕºÚµ•µƒ”¶¥œ˚œ¢
/// </summary>
/// <param name="nType">œ˚œ¢¿‡–Õ</param>
/// <returns></returns>
/// <created>Andy,2020/11/30</created>
/// <changed>Andy,2020/11/30</changed>
// ********************************************************************************
bool CVNCProxy::SendCommonResponse(const unsigned int nType)
{
    CHECK_POINTER_EX(m_pEndpoint, false);

    bool bResult = false;
    CVNCPacket objRequestPacket;

    objRequestPacket.Initial(nType, GetSessionID(), VNCP::OT_RESPONSE | VNCP::OR_SUCCESS, 0);
    bResult = m_pEndpoint->Send(&objRequestPacket);

    return bResult;
}

// ********************************************************************************
/// <summary>
/// ∑¢Hello«Î«Û
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/12/14</created>
/// <changed>Andy,2020/12/14</changed>
// ********************************************************************************
bool CVNCProxy::SendHelloRequest()
{
    CHECK_POINTER_EX(m_pEndpoint, false);

    bool bResult = false;
    CVNCPacket objRequestPacket;
    
//    objRequestPacket.InitialT<VNCP::VNC_MESSAGE_HEADER, VNCP::HELLO_MESSAGE_REQUEST>(
//        VNCP::MT_HELLO
//        , GetSessionID()
//        , VNCP::OT_REQUEST
//        , 0);

    objRequestPacket.Initial(VNCP::MT_HELLO, GetSessionID(), VNCP::OT_REQUEST, 0);
    

    VNCP::HELLO_MESSAGE_REQUEST* pHelloRequest = (VNCP::HELLO_MESSAGE_REQUEST*)objRequestPacket.GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();
    CPrjSettings* pPrjSettings = GetPrjSettings();

    assert(nullptr != pPrjSettings);
    pHelloRequest->nID = ntohq(std::atoll(pPrjSettings->GetDeviceID().c_str()));
    bResult = m_pEndpoint->Send(&objRequestPacket);

    return bResult;
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕøÕªß∂À≥ı ºªØ
/// </summary>
/// <param name="nSharedFlag">π≤œÌ±Í÷æ</param>
/// <param name="nHideWallPage">“˛≤ÿ◊¿√Ê…Ë÷√(1£∫“˛≤ÿ,0:≤ª“˛≤ÿ)</param>
/// <param name="nImageQuality">øÿ÷∆∂À∑¢ÀÕ¿¥Õº–Œ÷ ¡ø¿‡–Õ…Ë÷√£®≤ª–Ë“™±£¥Ê£©,@see IMAGE_QUALITY_TYPE</param>
/// <returns></returns>
/// <created>Andy,2020/11/27</created>
/// <changed>Andy,2021/2/22</changed>
// ********************************************************************************
bool CVNCProxy::SendClientInit(
    const U8 nSharedFlag
    , const U8 nHideWallPage
    , const U8 nImageQuality)
{
    CHECK_POINTER_EX(m_pEndpoint, false);

    bool bResult = false;
    CVNCPacket objRequestPacket;

    objRequestPacket.InitialT<VNCP::VNC_MESSAGE_HEADER, VNCP::CLIENT_INIT_REQUEST>(
        VNCP::MT_CLIENT_INIT_REQUEST
        , GetSessionID()
        , VNCP::OT_REQUEST
        , 0);

    VNCP::CLIENT_INIT_REQUEST* pClientInitRequest = (VNCP::CLIENT_INIT_REQUEST*)objRequestPacket.GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();

    pClientInitRequest->nSharedFlag = nSharedFlag;
    pClientInitRequest->nHideWallPage = nHideWallPage;
    pClientInitRequest->nImageQuality = nImageQuality;
    bResult = m_pEndpoint->Send(&objRequestPacket);

    return bResult;
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕøÕªß∂À…Ë÷√£®÷˜“™ «ª≠√Ê÷ ¡ø∫Õ◊¿√Ê±⁄÷Ω£©
/// </summary>
/// <param name="nHideWallPage">“˛≤ÿ◊¿√Ê…Ë÷√(1£∫“˛≤ÿ,0:≤ª“˛≤ÿ)</param>
/// <param name="nImageQuality">øÿ÷∆∂À∑¢ÀÕ¿¥Õº–Œ÷ ¡ø¿‡–Õ…Ë÷√£®≤ª–Ë“™±£¥Ê£©,@see IMAGE_QUALITY_TYPE</param>
/// <returns></returns>
/// <created>Andy,2021/2/22</created>
/// <changed>Andy,2021/2/22</changed>
// ********************************************************************************
bool CVNCProxy::SendClientSettings(const U8 nHideWallPage, const U8 nImageQuality)
{
    CHECK_POINTER_EX(m_pEndpoint, false);

    bool bResult = false;
    CVNCPacket objRequestPacket;

    objRequestPacket.InitialT<VNCP::VNC_MESSAGE_HEADER, VNCP::CLIENT_SETTINGS_REQUEST>(
        VNCP::MT_CLIENT_SETTING_REQUEST
        , GetSessionID()
        , VNCP::OT_REQUEST
        , 0);

    VNCP::CLIENT_SETTINGS_REQUEST* pSettingsRequest = (VNCP::CLIENT_SETTINGS_REQUEST*)objRequestPacket.GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();

    pSettingsRequest->nHideWallPage = nHideWallPage;
    pSettingsRequest->nImageQuality = nImageQuality;
    bResult = m_pEndpoint->Send(&objRequestPacket);

    return bResult;
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕ≥ı ºªØœ˚œ¢
/// </summary>
/// <param name="nResponse">1:”¶¥£¨0:«Î«Û</param>
/// <returns></returns>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2021/2/22</changed>
// ********************************************************************************
bool CVNCProxy::SendReadyRequest(const U8 nResponse)
{
    CHECK_POINTER_EX(m_pEndpoint, false);

    bool bResult = false;
    CVNCPacket objRequestPacket;

    objRequestPacket.InitialT<VNCP::VNC_MESSAGE_HEADER, VNCP::READY_REQUEST>(
        VNCP::MT_READY
        , GetSessionID()
        , VNCP::OT_REQUEST
        , 0);

    VNCP::READY_REQUEST* pReadyInfo = (VNCP::READY_REQUEST*)objRequestPacket.GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();

    pReadyInfo->bResponse = nResponse;
    bResult = m_pEndpoint->Send(&objRequestPacket);

    return bResult;
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕ∏¸–¬÷°ª∫≥Â«¯«Î«Û
/// </summary>
/// <param name="bIncremental">‘ˆ¡ø±Í÷æ</param>
/// <param name="refUpdateRect">«Î«Û±‰ªØµƒ«¯”Ú</param>
/// <returns>true:≥…π¶,false: ß∞‹</returns>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
bool CVNCProxy::SendFBUpdateRequest(const bool bIncremental, const CRect & refUpdateRect)
{
    bool bResult = false;
    CVNCPacket objRequestPacket;

    objRequestPacket.InitialT<VNCP::VNC_MESSAGE_HEADER, VNCP::FRAME_BUFFER_UPDATE_REQUEST>(
        VNCP::MT_CLIENT_FRAME_BUF_UPDATE_REQUEST
        , GetSessionID()
        , VNCP::OT_REQUEST
        ,0);

    VNCP::FRAME_BUFFER_UPDATE_REQUEST* pUpdateRequest = (VNCP::FRAME_BUFFER_UPDATE_REQUEST*)objRequestPacket.GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();

    pUpdateRequest->nInc = bIncremental;
    pUpdateRequest->nX = refUpdateRect.left;
    pUpdateRequest->nY = refUpdateRect.top;
    pUpdateRequest->nWidth = refUpdateRect.getWidth();
    pUpdateRequest->nHeight = refUpdateRect.getHeight();

    bResult = m_pEndpoint->Send(&objRequestPacket);

    return bResult;
}

// ********************************************************************************
/// <summary>
///
/// </summary>
/// <param name="refPixelFormat"></param>
/// <returns></returns>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
bool CVNCProxy::SendPixelFormatUpdateRequest(const CPixelFormat & refPixelFormat)
{
    bool bResult = false;
    CVNCPacket objRequestPacket;

    objRequestPacket.InitialT<VNCP::VNC_MESSAGE_HEADER, VNCP::PIXEL_FORMAT_SETTINGS_REQUEST>(
        VNCP::MT_CLIENT_SET_PIXEL_FORMAT
        , GetSessionID()
        , VNCP::OT_REQUEST
        ,0);

    VNCP::PIXEL_FORMAT_SETTINGS_REQUEST* pPixelFormatInfo = (VNCP::PIXEL_FORMAT_SETTINGS_REQUEST*)objRequestPacket.GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();


    
    pPixelFormatInfo->objPixelFormat.nBitsPerPixel = refPixelFormat.nBitsPerPixel;
    pPixelFormatInfo->objPixelFormat.nColorDepth = refPixelFormat.colorDepth;
    pPixelFormatInfo->objPixelFormat.nBigEndianFlag = refPixelFormat.bigEndian;
    pPixelFormatInfo->objPixelFormat.nTrueColourFlag = true;              ///< ◊‹ «Œ™true
    pPixelFormatInfo->objPixelFormat.nRedMax = refPixelFormat.redMax;
    pPixelFormatInfo->objPixelFormat.nGreenMax = refPixelFormat.greenMax;
    pPixelFormatInfo->objPixelFormat.nBlueMax = refPixelFormat.blueMax;

    pPixelFormatInfo->objPixelFormat.nRedShift = refPixelFormat.redShift;
    pPixelFormatInfo->objPixelFormat.nGreenShift = refPixelFormat.greenShift;
    pPixelFormatInfo->objPixelFormat.nBlueShift = refPixelFormat.blueShift;

    bResult = m_pEndpoint->Send(&objRequestPacket);

    return bResult;
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕº¸≈Ã ¬º˛«Î«Û
/// </summary>
/// <param name="bDownFlag">∞¥œ¬±Í÷æ</param>
/// <param name="nKey">º¸≈Ã±‡¬Î</param>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
bool CVNCProxy::SendKeyboardEventRequest(const bool bDownFlag, const U32 nKey)
{
    bool bResult = false;
    CVNCPacket objRequestPacket;

    objRequestPacket.InitialT<VNCP::VNC_MESSAGE_HEADER, VNCP::KEY_EVENT_REQUEST>(
        VNCP::MT_CLIENT_KEY_EVENT
        , GetSessionID()
        , VNCP::OT_REQUEST
        ,0);

    VNCP::KEY_EVENT_REQUEST* pKeyEventInfo = (VNCP::KEY_EVENT_REQUEST*)objRequestPacket.GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();

    pKeyEventInfo->nDowndFlag = bDownFlag;
    pKeyEventInfo->nKey = nKey;

    bResult = m_pEndpoint->Send(&objRequestPacket);

    return bResult;
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕ Û±Í ¬º˛«Î«Û
/// </summary>
/// <param name="nButtonMask"></param>
/// <param name="refPosition"></param>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
bool CVNCProxy::SendPointerEventRequest(const U8 nButtonMask, const CPoint & refPosition)
{
    bool bResult = false;
    CVNCPacket objRequestPacket;

    objRequestPacket.InitialT<VNCP::VNC_MESSAGE_HEADER, VNCP::MT_POINTER_EVENT_REQUEST>(
        VNCP::MT_CLIENT_POINTER_EVENT
        , GetSessionID()
        , VNCP::OT_REQUEST
        ,0);

    VNCP::MT_POINTER_EVENT_REQUEST* pPointerEventInfo = (VNCP::MT_POINTER_EVENT_REQUEST*)objRequestPacket.GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();

    pPointerEventInfo->nX = refPosition.x;
    pPointerEventInfo->nY = refPosition.y;
    pPointerEventInfo->nButtonMask = nButtonMask;

    bResult = m_pEndpoint->Send(&objRequestPacket);

    return bResult;
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕºÙ«–Œƒ±æ ¬º˛«Î«Û
/// </summary>
/// <param name="refCutText"></param>
/// <returns></returns>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
bool CVNCProxy::SendCutTextEventRequest(const std::string & refCutText)
{
    bool bResult = false;
    CVNCPacket objRequestPacket;

    objRequestPacket.InitialT<VNCP::VNC_MESSAGE_HEADER, VNCP::CUT_TEXT_EVENT>(
        VNCP::MT_CLIENT_CUT_TEXT
        , GetSessionID()
        , VNCP::OT_REQUEST
        , refCutText.size());

    VNCP::CUT_TEXT_EVENT* pCutTextInfo = (VNCP::CUT_TEXT_EVENT*)objRequestPacket.GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();

    pCutTextInfo->nLength = refCutText.size();
    memcpy(pCutTextInfo->pText, refCutText.c_str(), refCutText.size());
    bResult = m_pEndpoint->Send(&objRequestPacket);

    return bResult;
}

// ********************************************************************************
/// <summary>
/// ∑˛ŒÒ∂À∑¢ÀÕøÕªß∂À£¨æ≤÷πªÚ∆Ù”√”√ªß ‰»Îøÿ÷∆
/// </summary>
/// <param name="bEnableCtrl"></param>
/// <returns></returns>
/// <created>Andy,2021/1/5</created>
/// <changed>Andy,2021/1/5</changed>
// ********************************************************************************
bool CVNCProxy::SendEnableInputRequest(const bool bEnableCtrl)
{
    bool bResult = false;
    CVNCPacket objRequestPacket;

    objRequestPacket.InitialT<VNCP::VNC_MESSAGE_HEADER, VNCP::ENABLE_INPUT_REQUEST>(
        VNCP::MT_SERVER_ENABLE_REQUEST
        , GetSessionID()
        , VNCP::OT_REQUEST
        , 0);

    VNCP::ENABLE_INPUT_REQUEST* pEnableRequest = (VNCP::ENABLE_INPUT_REQUEST*)objRequestPacket.GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();

    pEnableRequest->bEnable = bEnableCtrl;
    bResult = m_pEndpoint->Send(&objRequestPacket);

    return bResult;
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕ“ª∏ˆ÷° ˝æ›
/// </summary>
/// <param name="pData">÷∏œÚ ˝æ›µƒ÷∏’Î</param>
/// <param name="nLen">–Ë“™∑¢ÀÕ ˝æ›µƒ≥§∂»</param>
/// <returns></returns>
/// <created>Andy,2021/4/6</created>
/// <changed>Andy,2021/4/6</changed>
// ********************************************************************************
bool CVNCProxy::SendFrameData(const BYTE* pData, const unsigned int nLen)
{
    bool bResult = false;
    const unsigned int PAYLOAD_LEN_MAX = CVNCPacket::PACKET_LEN_MAX - sizeof(VNCP::VNC_MESSAGE_HEADER) - sizeof(VNCP::FRAME_DATA_FRAG);
    unsigned int nLenSent = 0;     ///< “—æ≠∑¢ÀÕµƒ≥§∂»

    do
    {
        m_objFrameRequestPacket.InitialT<VNCP::VNC_MESSAGE_HEADER, VNCP::FRAME_DATA_FRAG>(
            VNCP::MT_SERVER_FRAME_DATA_FRAG
            , GetSessionID()
            , VNCP::OT_REQUEST
            , custom_min(nLen, PAYLOAD_LEN_MAX));

        VNCP::FRAME_DATA_FRAG* pDataFrag = (VNCP::FRAME_DATA_FRAG*)m_objFrameRequestPacket.GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();
        const int nFragLen = custom_min(nLen - nLenSent, PAYLOAD_LEN_MAX);

        pDataFrag->nTotal = htonl(nLen);
        pDataFrag->nPayloadLen = htonl(nFragLen);
        pDataFrag->nOffset = htonl(nLenSent);
        memcpy(pDataFrag->pData, &pData[nLenSent], nFragLen);
        bResult = m_pEndpoint->Send(&m_objFrameRequestPacket);

        nLenSent += nFragLen;
        m_objFrameRequestPacket.ResetContent();
    } while (nLenSent < nLen);

    return bResult;
}


// ********************************************************************************
/// <summary>
/// ∑¢ÀÕ∑˛ŒÒ∆˜≥ı ºªØ
/// </summary>
/// <param name="refDim"></param>
/// <param name="refPixelFormat"></param>
/// <created>Andy,2020/12/2</created>
/// <changed>Andy,2020/12/2</changed>
// ********************************************************************************
void CVNCProxy::SendFrameProperty(const CSize& refDim, const CPixelFormat& refPixelFormat)
{
    CVNCPacket objRequestPacket;

    //  ˝æ›∞¸ «ø’µƒæÕ≥ı ºªØ ˝æ›∞¸
    objRequestPacket.InitialT<VNCP::VNC_MESSAGE_HEADER, VNCP::SERVER_INIT_REQUEST>(
        VNCP::MT_SERVER_INIT_REQUEST
        , GetSessionID()
        , VNCP::OT_REQUEST
        , 0);

    VNCP::SERVER_INIT_REQUEST* pInitRequest = (VNCP::SERVER_INIT_REQUEST*)objRequestPacket.GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();

    pInitRequest->nFrameBufWidth = refDim.width;
    pInitRequest->nFrameBufHeight = refDim.height;

    pInitRequest->objServerPixelFormat.nBitsPerPixel = refPixelFormat.nBitsPerPixel;
    pInitRequest->objServerPixelFormat.nColorDepth = refPixelFormat.colorDepth;
    pInitRequest->objServerPixelFormat.nRedMax = refPixelFormat.redMax;
    pInitRequest->objServerPixelFormat.nGreenMax = refPixelFormat.greenMax;
    pInitRequest->objServerPixelFormat.nBlueMax = refPixelFormat.blueMax;
    pInitRequest->objServerPixelFormat.nRedShift = refPixelFormat.redShift;
    pInitRequest->objServerPixelFormat.nGreenShift = refPixelFormat.greenShift;
    pInitRequest->objServerPixelFormat.nBlueShift = refPixelFormat.blueShift;
    pInitRequest->objServerPixelFormat.nBigEndianFlag = refPixelFormat.bigEndian;

    m_pEndpoint->Send(&objRequestPacket);
}

// ********************************************************************************
/// <summary>
/// …Ë÷√–¬µƒ÷°ª∫≥Â«¯µƒ≥ﬂ¥Á
/// </summary>
/// <param name="pDim"></param>
/// <created>Andy,2020/9/28</created>
/// <changed>Andy,2020/9/28</changed>
// ********************************************************************************
void CVNCProxy::SendFrameBufSize(const CSize* pDim)
{
    CHECK_POINTER(pDim);

    if (pDim->Area() > 0)
    {
        CVNCPacket objRequestPacket;

        objRequestPacket.InitialT<VNCP::VNC_MESSAGE_HEADER, VNCP::DESKTOP_SIZE>(
            VNCP::MT_SERVER_DESKTOP_SIZE
            , GetSessionID()
            , VNCP::OT_REQUEST
            , 0);

        VNCP::DESKTOP_SIZE* pDeskSizeInfo = (VNCP::DESKTOP_SIZE*)objRequestPacket.GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();

        pDeskSizeInfo->nWidth = pDim->width;
        pDeskSizeInfo->nHeight = pDim->height;
        m_pEndpoint->Send(&objRequestPacket);
        LOG_DEBUG("Send new frame buffer size %d * %d", pDim->width, pDim->height);
    }
    else
    {
        LOG_ERROR("The frame buffer size is zero!");
    }
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕ÷°ª∫≥Â«¯œÒÀÿ∏Ò Ω
/// </summary>
/// <param name="refPixelFormat"></param>
/// <created>Andy,2021/6/15</created>
/// <changed>Andy,2021/6/15</changed>
// ********************************************************************************
void CVNCProxy::SendFramePixelFormat(const CPixelFormat& refPixelFormat)
{
    CVNCPacket objRequestPacket;

    objRequestPacket.InitialT<VNCP::VNC_MESSAGE_HEADER, VNCP::PIXEL_FORMAT>(
        VNCP::MT_SERVER_DESKTOP_PIXEL_FORMAT
        , GetSessionID()
        , VNCP::OT_REQUEST
        , 0);

    VNCP::PIXEL_FORMAT* pPixelFormat = (VNCP::PIXEL_FORMAT*)objRequestPacket.GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();

    pPixelFormat->nBitsPerPixel = refPixelFormat.nBitsPerPixel;
    pPixelFormat->nColorDepth = refPixelFormat.colorDepth;
    pPixelFormat->nRedMax = refPixelFormat.redMax;
    pPixelFormat->nGreenMax = refPixelFormat.greenMax;
    pPixelFormat->nBlueMax = refPixelFormat.blueMax;
    pPixelFormat->nRedShift = refPixelFormat.redShift;
    pPixelFormat->nGreenShift = refPixelFormat.greenShift;
    pPixelFormat->nBlueShift = refPixelFormat.blueShift;
    pPixelFormat->nBigEndianFlag = refPixelFormat.bigEndian;

    m_pEndpoint->Send(&objRequestPacket);
    LOG_DEBUG("Send new frame buffer pixel format");
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕ≤Ÿ◊˜œµÕ≥ Ù–‘
/// </summary>
/// <param name="refOSInfo">≤Ÿ◊˜œµÕ≥–≈œ¢</param>
/// <created>Andy,2021/7/14</created>
/// <changed>Andy,2021/7/14</changed>
// ********************************************************************************
bool CVNCProxy::SendOSInfo(const VNCP::COSInfo& refOSInfo)
{
    CVNCPacket objRequestPacket;

    objRequestPacket.InitialJsonT<>(
        VNCP::MT_SERVER_OS_INFO
        , GetSessionID()
        , VNCP::OT_REQUEST
        , &refOSInfo);
    LOG_DEBUG("Send OS information");

    return m_pEndpoint->Send(&objRequestPacket);
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕµ±«∞ª·ª∞◊¥Ã¨µΩøÿ÷∆∂À
/// </summary>
/// <param name="nState">µ±«∞µƒ◊¥Ã¨</param>
/// <returns></returns>
/// <created>Andy,2021/7/15</created>
/// <changed>Andy,2021/7/15</changed>
// ********************************************************************************
bool CVNCProxy::SendSessionStateRequest(const unsigned int nState)
{
    if (nullptr == m_pEndpoint)
    {
        return false;
    }

    CVNCPacket objRequestPacket;

    objRequestPacket.InitialT<VNCP::VNC_MESSAGE_HEADER, VNCP::SESSION_STATE_REQUEST>(
        VNCP::MT_SERVER_SESSION_STATE
        , GetSessionID()
        , VNCP::OT_REQUEST
        , 0);

    VNCP::SESSION_STATE_REQUEST* pSessionState = (VNCP::SESSION_STATE_REQUEST*)objRequestPacket.GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();

    pSessionState->nState = nState;
    LOG_DEBUG("Send session state");

    return m_pEndpoint->Send(&objRequestPacket);
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕ∏ƒ±‰±ªøÿ÷∆º∆À„∑÷±Ê¬ µƒ«Î«Û
/// </summary>
/// <param name="pDevMode"></param>
/// <created>Andy,2021/8/4</created>
/// <changed>Andy,2021/8/4</changed>
// ********************************************************************************
bool CVNCProxy::SendChangingResolutionRequest(const VNCP::CDevMode& refDevMode)
{
    CVNCPacket objRequestPacket;


    objRequestPacket.InitialJsonT<>(
        VNCP::MT_CLIENT_CHANGE_RESOLUTION
        , GetSessionID()
        , VNCP::OT_REQUEST
        , &refDevMode);

    return m_pEndpoint->Send(&objRequestPacket);
}

// ********************************************************************************
/// <summary>
/// πÿ±’µ±«∞VNC¥˙¿Ì
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/11/30</created>
/// <changed>Andy,2020/11/30</changed>
// ********************************************************************************
bool CVNCProxy::Close()
{
    CHECK_POINTER_EX(m_pEndpoint,false);

    m_pEndpoint->Close();

    return true;
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕ ˝æ›
/// </summary>
/// <param name="pData">÷∏œÚ±ª∑¢ÀÕµƒ÷∏’Î</param>
/// <param name="nLen">±ª∑¢ÀÕ ˝æ›µƒ≥§∂»</param>
/// <returns>true:≥…π¶,false: ß∞‹</returns>
/// <created>Andy,2020/12/1</created>
/// <changed>Andy,2020/12/1</changed>
// ********************************************************************************
bool CVNCProxy::Send(CNetPacket* pPacket)
{
    CHECK_POINTER_EX(m_pEndpoint, false);
    CHECK_POINTER_EX(pPacket, false);

    return m_pEndpoint->Send(pPacket);
}


// ********************************************************************************
/// <summary>
/// ∑¢ÀÕ ˝æ›
/// </summary>
/// <param name="pData">÷∏œÚ±ª∑¢ÀÕµƒ÷∏’Î</param>
/// <param name="nLen">±ª∑¢ÀÕ ˝æ›µƒ≥§∂»</param>
/// <returns>true:≥…π¶,false: ß∞‹</returns>
/// <created>Andy,2020/12/1</created>
/// <changed>Andy,2020/12/1</changed>
// ********************************************************************************
bool CVNCProxy::Send(const BYTE* pData, const unsigned int nLen)
{
    CHECK_POINTER_EX(m_pEndpoint, false);

    return m_pEndpoint->Send(pData, nLen);
}


bool CVNCProxy::Flush()
{
    return true;
}

bool CVNCProxy::PreClose()
{
    SetClosing(true);

    if (nullptr != m_pVNCTCPClient)
    {
        m_pVNCTCPClient->Stop();
    }

    if (nullptr != m_pVNCUDPClient)
    {
        m_pVNCUDPClient->Stop();
    }

    return true;
}

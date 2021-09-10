//
//  CVNCProxy.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/9.
//


#ifndef CVNCProxy_h
#define CVNCProxy_h

#pragma once
#include "CNetEndpoint.h"
#include "RCProtocol.h"
#include "Typedef.h"
#include "VNCClient.h"
#include "RCServerProxy.h"
#include "VNCPacket.h"
#include "Dimension.h"
#include "Point.h"
#include "Rect.h"

class CPixelFormat;
struct Point;
class Rect;

class CVNCProxy :public CNetEndpointEventSink
    ,public std::enable_shared_from_this<CVNCProxy>
{
public:
    CVNCProxy(const U32 nSessionID = 0
        , const U32 nPeerID = 0
        , const U32 nPeerIP = 0
        , const U16 nPeerPort = 0
        , const U32 nCommunicationMode = 0
        , const U32 nRoleType = RCP::RCT_CONTROLLER
        , const unsigned int nProtocolVer = RCP::PV_FIRST);
    ~CVNCProxy();

public:
    double GetReceivingLasttime();

    double GetConnecitonTime();

    CONVERSATION_STATE GetConversationStatus();

    // µ√µΩ¡¨Ω”≥÷–¯µƒ ±º‰(µ•Œª:√Í ”)
    double GetDurationTime() {    return m_objDuration.elapsed();    }
public:
    // ∆Ù∂Ø¥˙¿Ì
    bool Start();

    // ∑¢ÀÕºÚµ•µƒ«Î«Ûœ˚œ¢
    bool SendCommonRequest(const unsigned int nType);

    // ∑¢ÀÕºÚµ•µƒ”¶¥œ˚œ¢
    bool SendCommonResponse(const unsigned int nType);

    // ∑¢Hello«Î«Û
    bool SendHelloRequest();

    // ∑¢ÀÕøÕªß∂À≥ı ºªØ
    bool SendClientInit(const U8 nSharedFlag,const U8 nHideWallPage, const U8 nImageQuality);

    // ∑¢ÀÕøÕªß∂À…Ë÷√£®÷˜“™ «ª≠√Ê÷ ¡ø∫Õ◊¿√Ê±⁄÷Ω£©
    bool SendClientSettings(const U8 nHideWallPage, const U8 nImageQuality);

    // ∑¢ÀÕ≥ı ºªØœ˚œ¢
    bool SendReadyRequest(const U8 nResponse = 0);

    // ∑¢ÀÕ∏¸–¬÷°ª∫≥Â«¯«Î«Û
    bool SendFBUpdateRequest(const bool bIncremental, const CRect& refUpdateRect);

    // ∑¢ÀÕ∏¸–¬÷°ª∫≥Â«¯«Î«Û
    bool SendPixelFormatUpdateRequest(const CPixelFormat& refPixelFormat);
    
    // ∑¢ÀÕº¸≈Ã ¬º˛«Î«Û
    bool SendKeyboardEventRequest(const bool bDownFlag, const U32 nKey);

    // ∑¢ÀÕ Û±Í ¬º˛«Î«Û
    bool SendPointerEventRequest(const U8 nButtonMask, const CPoint& refPosition);

    // ∑¢ÀÕºÙ«–Œƒ±æ ¬º˛«Î«Û
    bool SendCutTextEventRequest(const std::string& refCutText);

    // ∑˛ŒÒ∂À∑¢ÀÕøÕªß∂À£¨Ω˚÷πªÚ∆Ù”√”√ªß ‰»Îøÿ÷∆
    bool SendEnableInputRequest(const bool bEnableCtrl);

    // ∑¢ÀÕ“ª∏ˆ÷° ˝æ›
    bool SendFrameData(const BYTE* pData, const unsigned int nLen);

    // ∑¢ÀÕ÷°ª∫≥Â«¯ Ù–‘
    void SendFrameProperty(const CSize& refDim, const CPixelFormat& refPixelFormat);

    // …Ë÷√–¬µƒ÷°ª∫≥Â«¯µƒ≥ﬂ¥Á
    void SendFrameBufSize(const CSize* pDim);
    
    // ∑¢ÀÕ÷°ª∫≥Â«¯œÒÀÿ∏Ò Ω
    void SendFramePixelFormat(const CPixelFormat& refPixelFormat);

    // ∑¢ÀÕ≤Ÿ◊˜œµÕ≥ Ù–‘
    bool SendOSInfo(const VNCP::COSInfo& refOSInfo);

    // ∑¢ÀÕµ±«∞ª·ª∞◊¥Ã¨µΩøÿ÷∆∂À
    bool SendSessionStateRequest(const unsigned int nState);

    // ∑¢ÀÕ∏ƒ±‰±ªøÿ÷∆º∆À„∑÷±Ê¬ µƒ«Î«Û
    bool SendChangingResolutionRequest(const VNCP::CDevMode& refDevMode);

    // ∑¢ÀÕ ˝æ›∞¸
    bool Send(CNetPacket* pPacket);

    // ∑¢ÀÕ ˝æ›
    bool Send(const BYTE* pData, const unsigned int nLen);

    

    bool Flush();

    // ◊º±∏πÿ±’µ±«∞VNC¥˙¿Ì
    bool PreClose();

    // πÿ±’µ±«∞VNC¥˙¿Ì
    bool Close();

    // ◊¢≤·µΩ–≈ π∑˛ŒÒ∆˜
//    bool RegistUDPHoleServer();

    // ≤…”√UDP¡¨Ω”µΩƒø±Í…Ë±∏
    bool UDPConnect(const unsigned int nIP, const unsigned int nPort);

public:
    bool OnEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint, SEVERITY_LEVEL nSeverity, SERVICE_EVENT nEvent);

    // ≥…π¶Ω®¡¢Õ®µ¿ ¬º˛
    void OnEstablished();
private:
    // ≤…”√TCP¡¨Ω”µΩƒø±Í…Ë±∏
    bool TCPConnect();


private:
    DECLARE_MEMBER_AND_METHOD_V11(U32, m_nSessionID, SessionID, 0)               ///< ª·ª∞ID
    DECLARE_MEMBER_AND_METHOD_V11(U64, m_nPeerID, PeerID, 0)                     ///< ∂‘∂ÀID
    DECLARE_MEMBER_AND_METHOD_V11(U32, m_nPeerIP, PeerIP, 0)                     ///< ∂‘∂ÀIP
    DECLARE_MEMBER_AND_METHOD_V11(U32, m_nPeerPort, PeerPort, 0)                 ///< µ√µΩ∂‘∂À∂Àø⁄
    DECLARE_MEMBER_AND_METHOD_V11(U32, m_nCommunicationMode, CommunicationMode, RCP::ST_HOLE)     ///< ª·ª∞¿‡–Õ
    DECLARE_MEMBER_AND_METHOD_V11(U32, m_nRoleType, RoleType, RCP::RCT_CONTROLLER)    ///< Ω«…´¿‡–Õ,@see RC_CLIENT_TYPE
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nProtocolVer, ProtocolVer, 0); ///< ª·ª∞À¯≤…”√µƒ–≠“È∞Ê±æ∫≈

    // ‘∂≥Ãøÿ÷∆∑˛ŒÒ¥˙¿Ì
    DECLARE_MEMBER_AND_METHOD_V11(CRCSvrProxyPtr, m_pRCSvrProxy, RCSvrProxy, nullptr);

     ///< µ±«∞µƒ¥˙¿Ì”√”⁄Õ®–≈µƒ∂Àµ„∂‘œÛ
    DECLARE_MEMBER_AND_METHOD_V11(PTR_NET_ENDPOINT_INTERFACE, m_pEndpoint, Endpoint, nullptr);

    ///< µ±«∞µƒ¥˙¿Ì”√”⁄Õ®–≈µƒTCPøÕªß∂À∂‘œÛ
    DECLARE_MEMBER_AND_METHOD_V11(CVNCTCPClientPtr, m_pVNCTCPClient, VNCTCPClient, nullptr);

    ///< µ±«∞µƒ¥˙¿Ì”√”⁄Õ®–≈µƒTCPøÕªß∂À∂‘œÛ
    DECLARE_MEMBER_AND_METHOD_V11(CVNCUDPClientPtr, m_pVNCUDPClient, VNCUDPClient, nullptr);

    ///< ’˝»∑±ªπÿ±’±Í÷æ
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bClosing, Closing, false);

    ///<  «∑Ò’˝»∑Ω®¡¢¡¨Ω”
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bEstablishVNCConnection, EstablishVNCConnection, false);

    DECLARE_MEMBER_AND_METHOD(std::string, m_strNickName, NickName)          ///< √˚≥∆

    ///< ’˝‘⁄πÿ±’,øÿ÷∆∂ŒªÚ±ªøÿ÷∆∂Àµƒ”√ªßµ„ª˜¡Àπÿ±’ª·ª∞∞¥≈•
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bProactiveClosing, ProactiveClosing,false)

    ///< øÿ÷∆∂À∑¢ÀÕ¿¥µƒ“˛≤ÿ◊¿√Ê…Ë÷√
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bHideWallPage, HideWallPage, true);

    ///< øÿ÷∆∂À∑¢ÀÕ¿¥Õº–Œ÷ ¡ø¿‡–Õ…Ë÷√,@see IMAGE_QUALITY_TYPE
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nImageQuality, ImageQuality, 0);

    CVNCPacket                  m_objFrameRequestPacket;

    // ¡¨Ω”≥÷–¯ ±º‰
    boost::timer              m_objDuration;

    // ≤…”√µƒ∆¡ƒª∑÷±Ê¬
    DECLARE_MEMBER_AND_METHOD(CSize, m_objScreanSize, ScreanSize);

    // ∏’Ω®¡¢øÿ÷∆Õ®µ¿ ±±ªøÿ∂Àµƒ∆¡ƒª∑÷±Ê¬
    DECLARE_MEMBER_AND_METHOD(CSize, m_objOriginalScreanSize, OriginalScreanSize);

    ///<  ±ªøÿ÷∆∂Œ≤Ÿ◊˜œµÕ≥¿‡–Õ£¨@see CEnvironment::OS_VERSION
    DECLARE_MEMBER_AND_METHOD_V11(U32, m_nControlledOS, ControlledOS, 0)

    /// < ±ªøÿ÷∆∂Œµ±«∞ª·ª∞µƒ◊¥Ã¨
    DECLARE_MEMBER_AND_METHOD_V11(UINT, m_nControlledSessionState, ControlledSessionState, WTS_SESSION_LOGON);

    ///<  «∑Ò“—æ≠ ’µΩµ⁄“ª∏ˆ ˝æ›∞¸
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bFirstPacketRecivied, FirstPacketRecivied, false);

    ///<  ±ªøÿ÷∆∂À»Ìº˛∞Ê±æ∫≈
    DECLARE_MEMBER_AND_METHOD_V11(U32, m_nVersion, Version, 0);
};

using CVNCProxyPtr = std::shared_ptr<CVNCProxy>;
using CVNCProxyArray = std::vector<CVNCProxyPtr>;



#endif

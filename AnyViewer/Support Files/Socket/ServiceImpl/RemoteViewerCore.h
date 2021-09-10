//
//  RemoteViewerCore.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/10.
//

#ifndef RemoteViewerCore_h
#define RemoteViewerCore_h

#pragma once
#include "FrameBuffer.h"
#include "Dimension.h"
#include "Point.h"
#include "CoreEventsAdapter.h"
//#include "FbUpdateNotifier.h"
#include "UpdateRequestSender.h"
#include "CVNCProxy.h"

#include "AVCodeHelper.h"

#include <map>
#include <atomic>

// --------------------------------------------------------------------------------
/// <summary>
/// CRemoteViewerCore implements a local representation of a live remote screen
/// updated in real time via the RFB protocol. It maintains screen data in its
/// own frame buffer and notifies the target class (the pCoreEventsAdapter) about various
/// events like changes in the frame buffer, incoming clipboard transfers from
/// the server, protocol state changes and so on.
/// Also, it implements remote control feature by passing arbitrary keyboard
/// and pointer events to the remote computer.
/// The interface of this class hides all the complexity of the RFB protocol
/// and provides the caller with an easy way to maintain an RFB connection and
/// get access to the screen data.
/// Each function of this class may be called from any thread and from any
/// number of threads simultaneously on the same object, unless documented
/// otherwise. Also, each function can be called on any stage of the protocol,
/// and even from callback functions of the pCoreEventsAdapter (see more about callbacks
/// and the pCoreEventsAdapter below), unless documented otherwise.
/// Finally, each function of this class may throw exceptions, unless it's
/// explicitly stated that it will never do so.
/// </summary>
// --------------------------------------------------------------------------------
class CRemoteViewerCore
{
public:
    CRemoteViewerCore();

    CRemoteViewerCore(
        CVNCProxyPtr pVNCProxy
        , CCoreEventsAdapter* pCoreEventsAdapter
        , bool bSaredFlag = true);

    virtual ~CRemoteViewerCore();

    // Do not allow copying objects.
    CRemoteViewerCore(const CRemoteViewerCore &) = delete;
    CRemoteViewerCore &operator=(const CRemoteViewerCore &) = delete;

public:
    VNCP::CDevModeArray& GetDevModes() { return  m_arrDevModes; }

    // ∞¥øÌ∂»≈≈–Ú∑÷±Ê¬ ƒ£ Ω
    void SortDevModes();

    /// < ≤È—Øµ±«∞∑÷±Ê¬ À˜“˝
    int GetCurResolutionIndex();

    //
    // Set the specified pixel format. The viewer will request that pixel format
    // from the server, as well as a full screen Update. The pixel format is not
    // guaranteed to be applied immediately. It will be applied when allowed by
    // the protocol, and pCoreEventsAdapter's onNewFrameBuffer() will be called after that.
    //
    void SetPixelFormat(const CPixelFormat& refViewerPixelFormat);

    //
    // Pause/Resume updating the frame buffer.
    //
    void StopUpdating(bool bStopped);

    //
    // Request full refresh of the framebuffer, so that the whole screen will be
    // re-sent from the server. The refresh is not guaranteed to happen
    // immediately, it will be completed when allowed by the protocol.
    //
    // This function does nothing when called before entering the main phase of
    // the protocol (when the framebuffer does not exist yet).
    //
    void RefreshFrameBuffer();

    //
    // Specifies whether viewer force full Update requests.
    //
    void ForceFullUpdateRequests(const bool& bForceUpdate);

    //
    // Defers an Update requests till timeout(in milliseconds) is expired.
    // Timeout starts when a previous request is sent. But anyway the next Update
    // request will not send until the response on the previous request is
    // recieved even if timeout is expired.
    //
    void DeferUpdateRequests(const int& nMilliseconds);


    //
    // Enable or disable cursor shape updates (bEnable by default). If bEnable,
    // then server sends information about the cursor shape. If disabled, cursor
    // is shown as a part of usual frame buffer updates.
    //
    void EnableCursorShapeUpdates(bool bEnable);

    //
    // Ignore or show cursor shape updates (shown by default). If cursor shape
    // updates are bEnable but bIgnored, remote cursor will not be shown. This
    // option probably should not be changed unless something goes really wrong.
    //
    void IgnoreCursorShapeUpdates(bool bIgnored);

    // …Ë÷√ ‰≥ˆª∫≥Â«¯µƒ≥ﬂ¥Á
    void SetOutImgSize(const unsigned int nWidth, const unsigned int nHeight);

    // ª÷∏¥‘≠ ºµƒª∫≥Â«¯µƒ≥ﬂ¥Á
    void RecoverOrgImgSize();
public:
    //
    // This function Includes common initialization code used by constructors.
    //
    bool Init(CCoreEventsAdapter *pCoreEventsAdapter);

    // ◊¢œ˙œ˚œ¢¥¶¿Ì∫Ø ˝µΩœ˚œ¢◊‹œﬂ
    void UnRegisterMsgRouting();

public:
    // ∑¢ÀÕ∞¥º¸ ¬º˛µΩ∑˛ŒÒ∆˜
    void SendKeyboardEvent(const bool bDownFlag, const U32 nKey);

    //
    // Send a pointer (mouse) event. Arguments specify the event as defined in
    // the RFB v.3 protocol specification.
    //
    void SendPointerEvent(const U8 nButtonMask, const CPoint& refPosition);

    // Send cut text (clipboard) to the server.
    void SendCutTextEvent(const std::string& refCutText);

    // ∑¢ÀÕºÚµ•µƒœ˚œ¢
    void SendSimpleEvent(const unsigned int nMsg);

    // ∑¢ÀÕ∏¸–¬œÒÀÿ∏Ò Ω«Î«Û
    bool SendPixelFormatUpdateRequest();

    // ∑¢ÀÕ∏˙–¬ª∫≥Â«¯«Î«Û
    void SendFbUpdateRequest(const bool bIncremental = true);


private:
    //  ’µΩ±ªøÿ÷∆∂À∑¢ÀÕ¿¥µƒ÷° ˝æ›∞¸
    bool OnFrameDataRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket);

    // ¥¶¿Ì∑˛ŒÒ∆˜∑¢ÀÕ¿¥µƒ◊¿√Ê≥ﬂ¥Á±‰ªØ«Î«Û
    void OnDeskSizeRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket);

    // ¥¶¿Ì∑˛ŒÒ∆˜∑¢ÀÕ¿¥µƒª∫≥Â»•œÒÀÿ∏Ò Ω«Î«Û
    void OnFramePixelFormatRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket);

    // ¥¶¿Ì∑˛ŒÒ∆˜∑¢ÀÕ¿¥π‚±Í–Œ◊¥∏ƒ±‰«Î«Û
    void OnCursorShapeChangedRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket);

    // ¥¶¿Ì∑˛ŒÒ∆˜∑¢ÀÕ¿¥π‚±ÍŒª÷√∏ƒ±‰«Î«Û
    void OnCursorPosChangedRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket);

    // ¥¶¿Ì∑˛ŒÒ∆˜∑¢ÀÕ¿¥µƒœÏ¡Â«Î«Û
    void OnBellRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket);

    // ¥¶¿Ì∑˛ŒÒ∆˜∑¢ÀÕµƒºÙÃ˘«Î«Û
    void OnServerCutTextRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket);

    // ¥¶¿Ì∑˛ŒÒ∆˜∑¢ÀÕµƒ≥ı ºªØ«Î«Û
    void OnInitRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket);

    // ¥¶¿Ì±ªøÿ÷∆’ﬂ∑¢ÀÕ¿¥µƒ◊º±∏æÕ–˜«Î«ÛªÚ”¶¥
    void OnReadyReponse(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket);

    // ∑˛ŒÒ∂À∑¢ÀÕøÕªß∂À£¨Ω˚÷πªÚ∆Ù”√”√ªß ‰»Îøÿ÷∆
    void OnEnableInputRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket);

    // ∑˛ŒÒ∂À∑¢ÀÕøÕªß∂À£¨±ªøÿ÷∆∂Àµƒ≤Ÿ◊˜œµÕ≥–≈œ¢
    void OnOSInfoRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket);

    // ¥¶¿Ìøÿ÷∆∂À∑¢ÀÕ¿¥µƒ◊¥Ã¨Õ®÷™
    void OnSendSessionStateRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket);

    //  ’µΩVNCª·ª∞ ˝æ›∞¸ ¬º˛
    void OnReceivedVNCPacket(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket);


protected:
    //
// Update properties (CSize and PixelfFormat) of m_objFrameBuffer.
//
    void SetFBProperties(const CSize& refFBDimension, const CPixelFormat& refPixelFormat);

private:
    // ◊¢≤·œ˚œ¢¥¶¿Ì∫Ø ˝µΩœ˚œ¢◊‹œﬂ
    void RegisterMsgRouting();

    // ∑¢ÀÕ◊º±∏æÕ–˜œﬂ≥Ã∫Ø ˝
    void SendReadyRequestThread();

    // Ω‚¬Îªÿµ˜∫Ø ˝
    bool OnDecode(const unsigned char* pData, unsigned int nWidth, unsigned int nHeight);

private:
    DECLARE_MEMBER_AND_METHOD_V11(CVNCProxyPtr, m_pVNCProxy, VNCProxy, nullptr);

    CCoreEventsAdapter         *m_pCoreEventsAdapter = nullptr;

//    CWatermarksController       m_objWatermarksController;

//    CFBUpdateNotifier           m_objFBUpdateNotifier;

    // This is general frame buffer of CRemoteViewerCore to change him.
    // This frame buffer contain actual state of remote desktop.
    // Cursor painted on him before calling CCoreEventsAdapter::OnFrameBufferUpdate()
    // and erased after (thread CFBUpdateNotifier).
    CFrameBuffer                m_objFrameBuffer;

    std::recursive_mutex        m_objPixelFormatLock;
    bool                        m_bNewPixelFormat = false;
    CPixelFormat                m_objViewerPixelFormat;

    std::atomic_bool            m_bRefreshing  {false};
    std::atomic_bool            m_bFreeze  {false};

    bool                        m_bForceFullUpdate = false;
    int                         m_nUpdateTimeout = 0;

    CUpdateRequestSender        m_objUpdateRequestSender;

    unsigned int                m_nReceivedVNCPacketCookie = 0;
    bool                        m_bInitInfoSent = false;        ///< ≥ı ºªØ«Î«Û «∑Ò±ª∑¢ÀÕ
    DECLARE_MEMBER_AND_GET_METHOD_V11(bool, m_bEnableInput, EnableInput, true);

    /// < “—æ≠ ’µΩ◊º±∏æÕ–˜”¶¥
    std::atomic_bool            m_bRecveciedReadyResponse{ false };

    /// < ∑¢ÀÕ◊º±∏æÕ–˜«Î«Ûœﬂ≥Ã
    std::thread                 m_objSendReadyRequestThread;

    /// < ÷°ª∫≥Â«¯
    CDataStreamBuffer           m_objRawFrameBuffer;

    DECLARE_MEMBER_AND_GET_METHOD_V11(CAVCodeHelperPtr,m_pAVCodeHelper, AVCodeHelper,nullptr);

    std::recursive_mutex        m_objDevModesLock;
    ///< œ‘ æƒ£ Ω¡–±Ì
    VNCP::CDevModeArray         m_arrDevModes;

    // ◊‘  ”¶£∫∏˘æ›µ±«∞¥∞ø⁄£®¥∞ø⁄ƒ£ Ω£©ªÚ∆¡ƒª£®»´∆¡ƒ£ Ω£©¥Û–°£¨‘⁄±£÷§ª≠√ÊøÌ∏ﬂ±»≤ª±‰µƒ«Èøˆœ¬£¨◊‘∂ØÀı–°ªÚ∑≈¥Ûª≠√Ê£¨“‘‘⁄µ±«∞¥∞ø⁄ªÚ∆¡ƒªƒ⁄◊Ó¥ÛªØ’π æ°£
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bAutoImgSize, AutoImgSize, true);

    //‘≠ ºµƒÕºœÒ≥ﬂ¥Á
    CSize                      m_objOrgFBSize;
};


#endif /* RemoteViewerCore_h */

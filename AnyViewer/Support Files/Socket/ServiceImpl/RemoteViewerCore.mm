//
//  RemoteViewerCore.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/10.
//

#include "RemoteViewerCore.h"

//#include "AnsiStringStorage.h"
//#include "WatermarksController.h"

#include "EnumHeader.h"
#include "CPrjSettings.h"
#include "log.h"
#include "CCustomThreadPool.h"


#include <string>
#include <vector>
#include <algorithm>
#include <thread>

using namespace std;

// ¥¥Ω®“ª∏ˆœﬂ≥Ã»ŒŒÒ
extern bool CreateThreadTask(const THREAD_POOL_FUN& refThreadFun, CTaskSink* pTaskSink = nullptr);

//extern CMessageBus& GetVNCMessageBus();

CRemoteViewerCore::CRemoteViewerCore()
//    : m_objFBUpdateNotifier(&m_objFrameBuffer, &m_objWatermarksController)
: m_objUpdateRequestSender( &m_objFrameBuffer)
{
}

CRemoteViewerCore::CRemoteViewerCore(
    CVNCProxyPtr pVNCProxy
    , CCoreEventsAdapterPtr pCoreEventsAdapter
    , bool bSaredFlag)
//    : m_objFBUpdateNotifier(&m_objFrameBuffer, &m_objWatermarksController)
: m_objUpdateRequestSender(&m_objFrameBuffer)
{
    m_pVNCProxy = pVNCProxy;
    m_spCoreEventsAdapter = pCoreEventsAdapter;
//    m_objFBUpdateNotifier.SetCoreEventsAdapter(m_pCoreEventsAdapter);
}

// ********************************************************************************
/// <summary>
/// ◊¢≤·œ˚œ¢¥¶¿Ì∫Ø ˝µΩœ˚œ¢◊‹œﬂ
/// </summary>
/// <created>Andy,2020/12/4</created>
/// <changed>Andy,2020/12/4</changed>
// ********************************************************************************
void CRemoteViewerCore::RegisterMsgRouting()
{
    CMessageBus& refMessageBus = GetMessageBus();

    m_nReceivedVNCPacketCookie = refMessageBus.Register(
        LMBS_RECEIVED_VNC_PACKET
        , [this](PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket) { OnReceivedVNCPacket(pEndpoint, pPacket); });
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕ◊º±∏æÕ–˜œﬂ≥Ã∫Ø ˝
/// </summary>
/// <created>Andy,2021/2/1</created>
/// <changed>Andy,2021/2/1</changed>
// ********************************************************************************
void CRemoteViewerCore::SendReadyRequestThread()
{
    assert(nullptr != m_pVNCProxy);

    do
    {
        m_pVNCProxy->SendReadyRequest();
//        ::sleep(3);
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    } while (!m_bRecveciedReadyResponse);
     
       LOG_DEBUG("Send ready request complete (Session ID=%u)", m_pVNCProxy->GetSessionID());
}

// ********************************************************************************
/// <summary>
/// Ω‚¬Îªÿµ˜∫Ø ˝
/// </summary>
/// <param name="pData"></param>
/// <param name="nWidth"></param>
/// <param name="nHeight"></param>
/// <returns></returns>
/// <created>Andy,2021/4/7</created>
/// <changed>Andy,2021/4/7</changed>
// ********************************************************************************
bool CRemoteViewerCore::OnDecode(const unsigned char* pData, unsigned int nWidth, unsigned int nHeight)
{
    bool bResult = false;
    std::lock_guard<std::recursive_mutex> objAutoLocker(m_objFrameBuffer.GetLock());
    const unsigned int nDataLen = (nWidth * 4) * nHeight;

    if (m_objFrameBuffer.GetBufferSize() == nDataLen)
    {
        memcpy(m_objFrameBuffer.GetBuffer(), pData, nDataLen);

        const CRect objRect(0, 0, nWidth, nHeight);

//        m_objFBUpdateNotifier.OnUpdate(&objRect);
        //m_objFrameBuffer.SaveBitmap(_T("e:\\FrameBuffer.bmp"));
        
//        const char *obj_Path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).lastObject stringByAppendingFormat:@"/FrameBuffer.bmp"].UTF8String;
//
//        m_objFrameBuffer.SaveBitmap(obj_Path);
    
        if (nullptr != m_spCoreEventsAdapter)
        {
            m_spCoreEventsAdapter->OnFrameBufferUpdate(&m_objFrameBuffer, &objRect);
        }
        
        bResult = true;
    }
    else
    {
        LOG_ERROR("Invalid frame buffer size (%u),expected size is (%u)"
            , m_objFrameBuffer.GetBufferSize()
            , nDataLen);
    }

    return bResult;
}

// ********************************************************************************
/// <summary>
/// ◊¢œ˙œ˚œ¢¥¶¿Ì∫Ø ˝µΩœ˚œ¢◊‹œﬂ
/// </summary>
/// <created>Andy,2020/12/4</created>
/// <changed>Andy,2020/12/4</changed>
// ********************************************************************************
void CRemoteViewerCore::UnRegisterMsgRouting()
{
    CMessageBus& refMessageBus = GetMessageBus();
    const bool bSuccess = refMessageBus.Unregister(LMBS_RECEIVED_VNC_PACKET, m_nReceivedVNCPacketCookie);
}

// ********************************************************************************
/// <summary>
///
/// </summary>
/// <param name="pVNCProxy"></param>
/// <param name="pViewerWnd"></param>
/// <returns></returns>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
bool CRemoteViewerCore::Init(CCoreEventsAdapterPtr pCoreEventsAdapter)
{
    CHECK_POINTER_EX(m_pVNCProxy,false);
    CHECK_POINTER_EX(pCoreEventsAdapter, false);

    m_spCoreEventsAdapter = pCoreEventsAdapter;
    m_bNewPixelFormat = false;
    m_bFreeze = false;
    m_bForceFullUpdate = false;
    m_nUpdateTimeout = 0;
    m_bInitInfoSent = false;

    // ±ÿ–Î‘⁄–¬µƒœﬂ≥Ã÷–◊¢≤·œ˚œ¢¥¶¿Ì∫Ø ˝£¨“ÚŒ™µ±«∞µƒµ˜”√–Ú¡– «¥”œ˚œ¢◊‹œﬂ∑¢≥ˆµƒ
    // ∑Ò‘Úª·≥ˆœ÷À¿À¯
    CreateThreadTask([this]() {RegisterMsgRouting(); });

    m_objUpdateRequestSender.SetVNCProxy(m_pVNCProxy);
    m_objUpdateRequestSender.Start();

//    m_objFBUpdateNotifier.SetCoreEventsAdapter(m_pCoreEventsAdapter);
//    m_objFBUpdateNotifier.Start();

    std::thread objSendReadyRequestThread([this]() {SendReadyRequestThread(); });

    m_objSendReadyRequestThread = std::move(objSendReadyRequestThread);
    LOG_DEBUG("CRemoteViewerCore::Init");

    return true;
}

CRemoteViewerCore::~CRemoteViewerCore()
{
//    m_objFBUpdateNotifier.Stop();
    m_bRecveciedReadyResponse = true;

    if (m_objSendReadyRequestThread.joinable())
    {
        m_objSendReadyRequestThread.join();
    }
}

// ********************************************************************************
/// <summary>
/// ∞¥øÌ∂»≈≈–Ú∑÷±Ê¬ ƒ£ Ω
/// </summary>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2021/8/3</changed>
// ********************************************************************************
void CRemoteViewerCore::SortDevModes()
{
    std::lock_guard<std::recursive_mutex> objAutoLocker(m_objDevModesLock);

    std::sort(
        m_arrDevModes.begin()
        , m_arrDevModes.end()
        , [](VNCP::CDevModePtr refDevMode1, VNCP::CDevModePtr refDevMode2)
    {
        bool bResult = false;

        if (refDevMode1->GetWidth() > refDevMode2->GetWidth())
        {
            bResult = true;
        }
        else if (refDevMode1->GetWidth() == refDevMode2->GetWidth())
        {
            bResult = (refDevMode1->GetHeight() > refDevMode2->GetHeight());
        }

        return !bResult;
    });
}
// ********************************************************************************
/// <summary>
/// ≤È—Øµ±«∞∑÷±Ê¬ À˜“˝
/// ∏√∫Ø ˝√ª”–≤…”√—œ∏Òµƒ∆•≈‰ «“ÚŒ™£¨øÿ÷∆∂À∑¢ÀÕ¿¥µƒ÷°ª∫≥Â«¯≥ﬂ¥Áæ≠π˝¡À≈º ˝ªØµ˜’˚£¨“Ú¥Àª·”–
/// ŒÛ≤Ó£¨À˘“‘Œ“√«—°‘Ò◊ÓΩ”Ω¸µƒ
/// </summary>
/// <returns></returns>
/// <created>Andy,2021/8/3</created>
/// <changed>Andy,2021/8/3</changed>
// ********************************************************************************
int CRemoteViewerCore::GetCurResolutionIndex()
{
    std::lock_guard<std::recursive_mutex> objAutoLocker(m_objDevModesLock);

    int nResult = -1;
    int nDiff = 4;
    int nIndex = 0;

    for (auto refEntry : m_arrDevModes)
    {
        const int nCurDiff = abs(int(refEntry->GetWidth() - m_objOrgFBSize.width)) + abs(int(refEntry->GetHeight() - m_objOrgFBSize.height));

        if (nCurDiff < nDiff)
        {
            nDiff = nCurDiff;
            nResult = nIndex;

            if (0 == nCurDiff)
            {
                break;
            }
        }

        nIndex++;
    }

    return nResult;
}
// ********************************************************************************
/// <summary>
/// …Ë÷√–¬µƒœÒÀÿ∏Ò Ω£¨µ´ «≤ªª·¡¢º¥…˙–ß£¨÷ª”–µ˜”ø∫Ø ˝SendPixelFormatUpdateRequest”Î±ªøÿ÷∆∂ÀÕ¨≤Ω∫Û
/// ≤≈ª·…˙–ß
/// </summary>
/// <param name="pPixelFormat"></param>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
void CRemoteViewerCore::SetPixelFormat(const CPixelFormat& refViewerPixelFormat)
{
    LOG_INFO("Pixel format will changed");

    std::lock_guard<std::recursive_mutex> objAutoLocker(m_objPixelFormatLock);

    m_bNewPixelFormat = true;
    m_objViewerPixelFormat = refViewerPixelFormat;
}


// ********************************************************************************
/// <summary>
///
/// </summary>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
void CRemoteViewerCore::RefreshFrameBuffer()
{
    LOG_INFO("Frame buffer will refreshed");
    m_bRefreshing = true;
    SendFbUpdateRequest(false);
}

// ********************************************************************************
/// <summary>
///
/// </summary>
/// <param name="bForceUpdate"></param>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
void CRemoteViewerCore::ForceFullUpdateRequests(const bool& bForceUpdate)
{
    m_bForceFullUpdate = bForceUpdate;
}

// ********************************************************************************
/// <summary>
///
/// </summary>
/// <param name="nMilliseconds"></param>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
void CRemoteViewerCore::DeferUpdateRequests(const int& nMilliseconds)
{
    m_nUpdateTimeout = nMilliseconds;
    m_objUpdateRequestSender.SetTimeout(nMilliseconds);
}


// ********************************************************************************
/// <summary>
/// Send a keyboard event. Arguments specify the event as defined in the
/// RFB v.3 protocol specification.
/// </summary>
/// <param name="bDownFlag">∞¥œ¬±Í÷æ</param>
/// <param name="nKey">º¸±‡¬Î</param>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
void CRemoteViewerCore::SendKeyboardEvent(const bool bDownFlag, const U32 nKey)
{
    if (m_bEnableInput)
    {
        LOG_INFO("Sending key event: %d, %d...", bDownFlag, nKey);
        m_pVNCProxy->SendKeyboardEventRequest(bDownFlag, nKey);
        LOG_INFO("Key event: %d, %d is sent", bDownFlag, nKey);
    }
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕ Û±Í ¬º˛œ˚œ¢(Arguments specify the event as defined in
/// the RFB v.3 protocol specification.)
/// </summary>
/// <param name="nButtonMask"></param>
/// <param name="refPosition"></param>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
void CRemoteViewerCore::SendPointerEvent(const U8 nButtonMask, const CPoint& refPosition)
{
    if (m_bEnableInput)
    {
        //LOG_INFO("Sending pointer event 0x%X, (%d, %d)...",
        //    static_cast<int>(nButtonMask), refPosition.x, refPosition.y);

        CPoint ptPos(refPosition.x, refPosition.y);

        if (nullptr != m_pAVCodeHelper)
        {
            ptPos.x = int(refPosition.x / m_pAVCodeHelper->GetXScale());
            ptPos.y = int(refPosition.y / m_pAVCodeHelper->GetYScale());
        }

        m_pVNCProxy->SendPointerEventRequest(nButtonMask, ptPos);

        // Update position
//        m_objFBUpdateNotifier.UpdatePointerPos(&refPosition);

        //LOG_INFO("Pointer event: 0x%X, (%d, %d) is sent",
        //    static_cast<int>(nButtonMask), refPosition.x, refPosition.y);
    }
}

// ********************************************************************************
/// <summary>
/// Send cut text (clipboard) to the server.
/// </summary>
/// <param name="refCutText"></param>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
void CRemoteViewerCore::SendCutTextEvent(const std::string& refCutText)
{
    if (GetEnableInput())
    {
        LOG_INFO("Sending clipboard cut text: \"%s\"...", refCutText.c_str());

        CreateThreadTask([this, refCutText]()
        {
            m_pVNCProxy->SendCutTextEventRequest(refCutText);
        });
        
        LOG_INFO("Clipboard cut text: \"%s\" is sent", refCutText.c_str());
    }
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕºÚµ•µƒœ˚œ¢
/// </summary>
/// <param name="nMsg">œ˚œ¢¿‡–Õ</param>
/// <created>Andy,2020/11/16</created>
/// <changed>Andy,2020/11/17</changed>
// ********************************************************************************
void CRemoteViewerCore::SendSimpleEvent(const unsigned int nMsg)
{
    if (GetEnableInput())
    {
        CreateThreadTask([this, nMsg]()
        {
            m_pVNCProxy->SendCommonRequest(nMsg);
        });
        
    }
}

// ********************************************************************************
/// <summary>
/// If m_bNewPixelFormat flag is set to true, then pixel format of the frame buffer
/// will be updated to m_objViewerPixelFormat.
/// This method must be called only from the pRFBInputGate thread, otherwise data corruption
/// is possible on the protocol level.
/// Returns true if pixel format was actually changed.
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
bool CRemoteViewerCore::SendPixelFormatUpdateRequest()
{
    CPixelFormat objPixelFormat;

    LOG_INFO("Check pixel format change...");

    {
        std::lock_guard<std::recursive_mutex> objAutoLocker(m_objPixelFormatLock);

        if (!m_bNewPixelFormat)
        {
            return false;
        }

        m_bNewPixelFormat = false;
        objPixelFormat = m_objViewerPixelFormat;
    }

    const int nBitsPerPixel = m_objViewerPixelFormat.nBitsPerPixel;

    if (nBitsPerPixel != 8 && nBitsPerPixel != 16 && nBitsPerPixel != 32)
    {
        assert("Only 8, 16 or 32 bits per pixel supported!");
//        throw CException(_T("Only 8, 16 or 32 bits per pixel supported!"));
    }

    {
        // FIXME: here isn't accept true-colour flag.
        // PixelFormats may be equal, if isn't.
        std::unique_lock<std::recursive_mutex> objAutoLocker(m_objFrameBuffer.GetLock());

        CPixelFormat objFramePixelFormat = m_objFrameBuffer.GetPixelFormat();
        
        if (objPixelFormat.IsEqualTo(&objFramePixelFormat))
        {
            return false;
        }

        
        if (m_objFrameBuffer.GetBuffer() != nullptr)
        {
            SetFBProperties(m_objFrameBuffer.GetDimension(), objPixelFormat);
        }
    }

    return m_pVNCProxy->SendPixelFormatUpdateRequest(objPixelFormat);
}

// ********************************************************************************
/// <summary>
/// ∑¢ÀÕ∏˙–¬ª∫≥Â«¯«Î«Û
/// </summary>
/// <param name="bIncremental">‘ˆ¡ø±Í÷æ</param>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
void CRemoteViewerCore::SendFbUpdateRequest(const bool bIncremental)
{
    bool bRefresh = false;
    bool bUpdateFbProperties = false;

    if (SendPixelFormatUpdateRequest())
    {
        bUpdateFbProperties = true;
    }

    if (m_bRefreshing)
    {
        m_bRefreshing = false;
        bRefresh = true;
    }

    if (bRefresh || bUpdateFbProperties || m_objUpdateRequestSender.GetTimeout() <= 0)
    {
        const bool bIsIncremental = bIncremental && !bRefresh && !bUpdateFbProperties;
        CRect objUpdateRect = m_objFrameBuffer.GetDimension().GetRect();
        
        LOG_DEBUG("Sending frame buffer %s Update request [%dx%d]..."
            ,bIsIncremental ? "incremental" : "full Update"
            ,objUpdateRect.getWidth()
            , objUpdateRect.getHeight());

        m_pVNCProxy->SendFBUpdateRequest(bIsIncremental, objUpdateRect);

        LOG_DEBUG("Frame buffer Update request is sent");
    }
    else
    {
        m_objUpdateRequestSender.SetWasUpdated();
    }
}




// ********************************************************************************
/// <summary>
///
/// </summary>
/// <param name="bEnable"></param>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
void CRemoteViewerCore::EnableCursorShapeUpdates(bool bEnable)
{
    bool bNeedUpdate = false;
}

// ********************************************************************************
/// <summary>
///
/// </summary>
/// <param name="bIgnored"></param>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
void CRemoteViewerCore::IgnoreCursorShapeUpdates(bool bIgnored)
{
//    m_objFBUpdateNotifier.SetIgnoreShapeUpdates(bIgnored);
}

// ********************************************************************************
/// <summary>
/// …Ë÷√ ‰≥ˆª∫≥Â«¯µƒ≥ﬂ¥Á£¨¿˚”√
/// </summary>
/// <param name="nWidth">øÌ∂»</param>
/// <param name="nHeight">∏ﬂ∂»</param>
/// <created>Andy,2021/8/19</created>
/// <changed>Andy,2021/8/19</changed>
// ********************************************************************************
void CRemoteViewerCore::SetOutImgSize(const unsigned int nWidth, const unsigned int nHeight)
{
    if (nullptr != m_pAVCodeHelper)
    {
        const CSize objDimension(nWidth - nWidth % 2, nHeight - nHeight % 2);

        m_objFrameBuffer.SetDimension(&objDimension);
        m_objFrameBuffer.SetColor(0, 0, 0);
        
        m_pAVCodeHelper->ChangeOutImgProperty(nWidth, nHeight);

        const CRect objUpdateRect(m_pAVCodeHelper->GetInWidth(), m_pAVCodeHelper->GetInHeight());

        m_pVNCProxy->SendFBUpdateRequest(false, objUpdateRect);
    }
}

// ********************************************************************************
/// <summary>
/// ª÷∏¥µΩ‘≠ ºµƒ ‰≥ˆÕºœÒ≥ﬂ¥Á
/// </summary>
/// <created>Andy,2021/8/19</created>
/// <changed>Andy,2021/8/19</changed>
// ********************************************************************************
void CRemoteViewerCore::RecoverOrgImgSize()
{
    if (nullptr != m_pAVCodeHelper)
    {
        const CSize objDimension(m_pAVCodeHelper->GetInWidth(), m_pAVCodeHelper->GetInHeight());

        m_objFrameBuffer.SetDimension(&objDimension);
        m_objFrameBuffer.SetColor(0, 0, 0);

        m_pAVCodeHelper->ChangeOutImgProperty(m_pAVCodeHelper->GetInWidth(), m_pAVCodeHelper->GetInHeight());

        const CRect objUpdateRect(m_pAVCodeHelper->GetInWidth(), m_pAVCodeHelper->GetInHeight());

        m_pVNCProxy->SendFBUpdateRequest(false, objUpdateRect);
    }
}

// ********************************************************************************
/// <summary>
///
/// </summary>
/// <param name="bStopped"></param>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
void CRemoteViewerCore::StopUpdating(bool bStopped)
{
    {
        if (bStopped == m_bFreeze)
        {
            return;
        }

        m_bFreeze = bStopped;
    }

    if (!bStopped)
    {
        LOG_INFO("Sending of frame buffer Update request...");
        SendFbUpdateRequest(!m_bForceFullUpdate);
    }
}

// ********************************************************************************
/// <summary>
/// …Ë÷√÷°ª∫≥Â«¯µƒ Ù–‘
/// </summary>
/// <param name="pFBDimension"></param>
/// <param name="pPixelFormat"></param>
/// <created>Andy,2020/12/2</created>
/// <changed>Andy,2020/12/2</changed>
// ********************************************************************************
void CRemoteViewerCore::SetFBProperties(const CSize& refFBDimension, const CPixelFormat& refPixelFormat)
{
#ifdef _DEMO_VERSION_
    m_objWatermarksController.setNewFbProperties(&pFBDimension->GetRect(), pPixelFormat);
#endif

    std::string strPixelFormatString;
    
    
    

//    strPixelFormatString.format(_T("[bits-per-pixel: %d, depth: %d, big-endian-flag: %d, ")
//        _T("true-color-flag: is set, ") // true color always is set
//        _T("red-max: %d, green-max: %d, blue-max: %d, ")
//        _T("red-shift: %d, green-shift: %d, blue-shift: %d]")
//        , refPixelFormat.nBitsPerPixel
//        , refPixelFormat.colorDepth
//        , refPixelFormat.bigEndian
//        , refPixelFormat.redMax
//        , refPixelFormat.greenMax
//        , refPixelFormat.blueMax
//        , refPixelFormat.redShift
//        , refPixelFormat.greenShift
//        , refPixelFormat.blueShift);
//
//    LOG_DEBUG("Setting frame buffer properties...");
//    LOG_DEBUG("Frame buffer dimension: (%d, %d)", refFBDimension.width, refFBDimension.height);
//    LOG_DEBUG("Frame buffer pixel format: %s", (char*)CT2A(strPixelFormatString.getString()));
//
    if (!m_objFrameBuffer.SetProperties(refFBDimension, refPixelFormat))
    {
//        LOG_ERROR("Failed to set property frame buffer. CSize: (%d, %d), Pixel format: %s"
//            , refFBDimension.width
//            , refFBDimension.height
//            , CT2A(strPixelFormatString.getString()));
    }

    m_objFrameBuffer.SetColor(0, 0, 0);
    RefreshFrameBuffer();
//    m_objFBUpdateNotifier.OnPropertiesFb();

    if (nullptr != m_pVNCProxy)
    {
        m_pVNCProxy->SetScreanSize(refFBDimension);
    }
}
// ********************************************************************************
/// <summary>
///  ’µΩ±ªøÿ÷∆∂À∑¢ÀÕ¿¥µƒ÷° ˝æ›∞¸
/// </summary>
/// <param name="pEndpoint"></param>
/// <param name="pPacket"></param>
/// <returns></returns>
/// <created>Andy,2021/4/6</created>
/// <changed>Andy,2021/4/6</changed>
// ********************************************************************************
bool CRemoteViewerCore::OnFrameDataRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket)
{
    CHECK_POINTER_EX(m_pAVCodeHelper,false);

    bool bResult = true;
    VNCP::FRAME_DATA_FRAG* pDataFrag = (VNCP::FRAME_DATA_FRAG*)pPacket->GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();
    const U32 nTotalLen = NTOHL(pDataFrag->nTotal);
    const U32 nPayloadLen = NTOHL(pDataFrag->nPayloadLen);

    if (nTotalLen == nPayloadLen)
    {
        ///< Àµ√˜÷° ˝æ›√ª”–∑÷∆¨£¨÷±Ω”Ã·Ωª¥¶¿Ì
        m_pAVCodeHelper->Decode(pDataFrag->pData, nPayloadLen);
    }
    else
    {
        if (NTOHL(pDataFrag->nOffset) == 0)
        {
            if (m_objRawFrameBuffer.GetSize() < int(nTotalLen))
            {
                m_objRawFrameBuffer.Allocate(nTotalLen);
            }

            m_objRawFrameBuffer.Empty();
        }

        m_objRawFrameBuffer.Write(pDataFrag->pData, nPayloadLen);

        if (m_objRawFrameBuffer.GetDataLen() == nTotalLen)
        {
            // Àµ√˜µ±«∞µƒ÷° ˝æ›“—æ≠Ω” ’Ω· ¯
            m_pAVCodeHelper->Decode(m_objRawFrameBuffer.GetBufferHeadPtr(), nTotalLen);

        }
    }


    return bResult;
}

// ********************************************************************************
/// <summary>
/// ¥¶¿Ì∑˛ŒÒ∆˜∑¢ÀÕµƒ≥ı ºªØ«Î«Û
/// </summary>
/// <param name="pEndpoint"> ’∑¢ ˝æ›∞¸∂Àµ„</param>
/// <param name="pPacket">÷∏œÚ ˝æ›∞¸∂‘œÛµƒ÷∏’Î</param>
/// <created>Andy,2020/12/2</created>
/// <changed>Andy,2020/12/2</changed>
// ********************************************************************************
void CRemoteViewerCore::OnInitRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket * pPacket)
{
    VNCP::SERVER_INIT_REQUEST* pInitInfo = (VNCP::SERVER_INIT_REQUEST*)pPacket->GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();
    const CSize objScreenDimension(pInitInfo->nFrameBufWidth, pInitInfo->nFrameBufHeight);
    CPixelFormat objServerPixelFormat;

    objServerPixelFormat.nBitsPerPixel = pInitInfo->objServerPixelFormat.nBitsPerPixel;
    objServerPixelFormat.colorDepth = pInitInfo->objServerPixelFormat.nColorDepth;
    objServerPixelFormat.redMax = pInitInfo->objServerPixelFormat.nRedMax;
    objServerPixelFormat.greenMax = pInitInfo->objServerPixelFormat.nGreenMax;
    objServerPixelFormat.blueMax = pInitInfo->objServerPixelFormat.nBlueMax;
    objServerPixelFormat.redShift = pInitInfo->objServerPixelFormat.nRedShift;
    objServerPixelFormat.greenShift = pInitInfo->objServerPixelFormat.nGreenShift;
    objServerPixelFormat.blueShift = pInitInfo->objServerPixelFormat.nBlueShift;
    objServerPixelFormat.bigEndian = pInitInfo->objServerPixelFormat.nBigEndianFlag;

    SetFBProperties(objScreenDimension, objServerPixelFormat);

    if (0 == objServerPixelFormat.nBitsPerPixel)
    {
        LOG_ERROR("Invalid pixel format(BitsPerPixel=%d,redMax=%d,greenMax=%d,blueMax=%d)"
            , objServerPixelFormat.nBitsPerPixel
            , objServerPixelFormat.redMax
            , objServerPixelFormat.greenMax
            , objServerPixelFormat.blueMax);
    }

    if (nullptr != m_spCoreEventsAdapter)
    {
        m_spCoreEventsAdapter->OnFrameBufferSizeChange(objScreenDimension.width, objScreenDimension.height);
    }

    //这里设置一下图片的输出宽高，根据当前设备的屏幕宽高
    CPrjSettings* pPrjSettings = GetPrjSettings();
    
    float nHeight = pPrjSettings->GetScreenWidth() / objScreenDimension.width * objScreenDimension.height;
    
    SetOutImgSize(pPrjSettings->GetScreenWidth(), nHeight);
    
    
    
    DecodeCallback pCallback = std::bind(&CRemoteViewerCore::OnDecode, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    m_pAVCodeHelper = std::make_shared<CAVCodeHelper>(pCallback);

    if (nullptr != m_pAVCodeHelper)
    {
        const bool bSuccess = m_pAVCodeHelper->Construct(
            CAVCodeHelper::WM_DECODE
            ,objScreenDimension.width
            , objScreenDimension.height);

        if (!bSuccess)
        {
            LOG_ERROR("Fail to construct avcodehelper!");
        }
    }

    m_pVNCProxy->SendCommonRequest(VNCP::MT_CLIENT_READY);
    RefreshFrameBuffer();
    m_objOrgFBSize = objScreenDimension;
}


// ********************************************************************************
/// <summary>
/// ¥¶¿Ì±ªøÿ÷∆’ﬂ∑¢ÀÕ¿¥µƒ◊º±∏æÕ–˜«Î«ÛªÚ”¶¥
/// </summary>
/// <param name="pEndpoint"> ’∑¢ ˝æ›∞¸∂Àµ„</param>
/// <param name="pPacket">÷∏œÚ ˝æ›∞¸∂‘œÛµƒ÷∏’Î</param>
/// <created>Andy,2020/12/3</created>
/// <changed>Andy,2020/12/3</changed>
// ********************************************************************************
void CRemoteViewerCore::OnReadyReponse(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket * pPacket)
{
    VNCP::READY_REQUEST* pReadyInfo = (VNCP::READY_REQUEST*)pPacket->GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();

    m_spCoreEventsAdapter->OnConnected();

    if (!m_bRecveciedReadyResponse)
    {
        CPrjSettings* pPrjSettings = GetPrjSettings();

        m_pVNCProxy->SendClientInit(
            pPrjSettings->GetSharedFlag()
            , pPrjSettings->GetHideWallPage()
            , pPrjSettings->GetImageQuality());
        m_bRecveciedReadyResponse = true;

        m_pVNCProxy->SetHideWallPage(pPrjSettings->GetHideWallPage());
        m_pVNCProxy->SetImageQuality(pPrjSettings->GetImageQuality());
    }

    m_pVNCProxy->SetFirstPacketRecivied(true);
}

// ********************************************************************************
/// <summary>
/// ∑˛ŒÒ∂À∑¢ÀÕøÕªß∂À£¨Ω˚÷πªÚ∆Ù”√”√ªß ‰»Îøÿ÷∆
/// </summary>
/// <param name="pEndpoint"></param>
/// <param name="pPacket"></param>
/// <created>Andy,2021/1/5</created>
/// <changed>Andy,2021/1/5</changed>
// ********************************************************************************
void CRemoteViewerCore::OnEnableInputRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket * pPacket)
{
    VNCP::ENABLE_INPUT_REQUEST* pEnableInputInfo = (VNCP::ENABLE_INPUT_REQUEST*)pPacket->GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();

    m_bEnableInput = pEnableInputInfo->bEnable;
}

// ********************************************************************************
/// <summary>
/// ∑˛ŒÒ∂À∑¢ÀÕøÕªß∂À£¨±ªøÿ÷∆∂Àµƒ≤Ÿ◊˜œµÕ≥–≈œ¢
/// </summary>
/// <param name="pEndpoint"></param>
/// <param name="pPacket"></param>
/// <created>Andy,2021/7/14</created>
/// <changed>Andy,2021/7/14</changed>
// ********************************************************************************
void CRemoteViewerCore::OnOSInfoRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket)
{
    VNCP::COSInfo objOSInfo;

    pPacket->ExtractJsonObjT<VNCP::VNC_MESSAGE_HEADER, VNCP::COSInfo>(objOSInfo);

    m_pVNCProxy->SetControlledOS(objOSInfo.GetOS());

    {
        std::lock_guard<std::recursive_mutex> objAutoLocker(m_objDevModesLock);
        m_arrDevModes = std::move(objOSInfo.GetModes());
    }

    const VNCP::CDevMode&& refDevMode = objOSInfo.GetCurDevMode();

    m_pVNCProxy->SetOriginalScreanSize(CSize(refDevMode.GetWidth(), refDevMode.GetHeight()));
    m_pVNCProxy->SetVersion(objOSInfo.GetVersion());
}

// ********************************************************************************
/// <summary>
/// ¥¶¿Ìøÿ÷∆∂À∑¢ÀÕ¿¥µƒ◊¥Ã¨Õ®÷™
/// </summary>
/// <param name="pEndpoint"></param>
/// <param name="pPacket"></param>
/// <created>Andy,2021/7/15</created>
/// <changed>Andy,2021/7/15</changed>
// ********************************************************************************
void CRemoteViewerCore::OnSendSessionStateRequest(
    PTR_NET_ENDPOINT_INTERFACE pEndpoint
    , CVNCPacket* pPacket)
{
    VNCP::SESSION_STATE_REQUEST* pSessionState = (VNCP::SESSION_STATE_REQUEST*)pPacket->GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();

    m_pVNCProxy->SetControlledSessionState(pSessionState->nState);
}

// ********************************************************************************
/// <summary>
/// ¥¶¿Ì∑˛ŒÒ∆˜∑¢ÀÕ¿¥µƒ◊¿√Ê≥ﬂ¥Á±‰ªØ«Î«Û
/// </summary>
/// <param name="pEndpoint"></param>
/// <param name="pPacket"></param>
/// <created>Andy,2021/4/12</created>
/// <changed>Andy,2021/4/12</changed>
// ********************************************************************************
void CRemoteViewerCore::OnDeskSizeRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket)
{
    VNCP::DESKTOP_SIZE* pDeskSizeInfo = (VNCP::DESKTOP_SIZE*)pPacket->GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();

    if(pDeskSizeInfo->nWidth != m_pAVCodeHelper->GetInWidth()
        || pDeskSizeInfo->nHeight != m_pAVCodeHelper->GetInHeight())
    {
        const int nOutWidth = GetAutoImgSize() ? m_pAVCodeHelper->GetOutWidth() : pDeskSizeInfo->nWidth;
        const int nOutHeight = GetAutoImgSize() ? m_pAVCodeHelper->GetOutHeight() : pDeskSizeInfo->nHeight;

        SetFBProperties(
            CSize(nOutWidth, nOutHeight)
            , m_objFrameBuffer.GetPixelFormat());

        m_pAVCodeHelper->Destroy();
        m_pAVCodeHelper->Construct(
            CAVCodeHelper::WM_DECODE
            , pDeskSizeInfo->nWidth
            , pDeskSizeInfo->nHeight
            , AV_PIX_FMT_RGB32
            , nOutWidth
            , nOutHeight);

        if (nullptr != m_spCoreEventsAdapter)
        {
            m_spCoreEventsAdapter->OnFrameBufferSizeChange(nOutWidth, nOutHeight);
        }
        
        //{
        //    const int nCurResolutionIndex = GetCurResolutionIndex();

        //    if (-1 == nCurResolutionIndex)
        //    {
        //        if (m_pVNCProxy->GetVersion() > (1 * 100 + 2 * 10))
        //        {
        //            std::lock_guard<std::recursive_mutex> objAutoLocker(m_objDevModesLock);

        //            // »Áπ˚–¬µƒ∑÷±Ê¬ ≤ª¥Ê‘⁄«“±ªøÿ÷∆∂Àµƒ∞Ê±æ∫≈¥Û”⁄1.2æÕÃÌº”µΩ∑÷±Ê¬ ¡–±Ì÷–£¨÷˜“™ «’Î∂‘–Èƒ‚ª˙
        //            m_arrDevModes.push_back(std::make_shared<VNCP::CDevMode>(pDeskSizeInfo->nWidth, pDeskSizeInfo->nHeight, 32));
        //        }
        //    }
        //}

        m_objOrgFBSize.width = pDeskSizeInfo->nWidth;
        m_objOrgFBSize.height = pDeskSizeInfo->nHeight;
    }

    m_pVNCProxy->SendCommonRequest(VNCP::MT_CLIENT_DESK_SIZE_CHANGED);
}

// ********************************************************************************
/// <summary>
///
/// </summary>
/// <param name="pEndpoint"></param>
/// <param name="pPacket"></param>
/// <created>Andy,2021/6/15</created>
/// <changed>Andy,2021/6/15</changed>
// ********************************************************************************
void CRemoteViewerCore::OnFramePixelFormatRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket)
{
    VNCP::PIXEL_FORMAT* pPixelFormat = (VNCP::PIXEL_FORMAT*)pPacket->GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();
    CPixelFormat objServerPixelFormat;

    objServerPixelFormat.nBitsPerPixel = pPixelFormat->nBitsPerPixel;
    objServerPixelFormat.colorDepth = pPixelFormat->nColorDepth;
    objServerPixelFormat.redMax = pPixelFormat->nRedMax;
    objServerPixelFormat.greenMax = pPixelFormat->nGreenMax;
    objServerPixelFormat.blueMax = pPixelFormat->nBlueMax;
    objServerPixelFormat.redShift = pPixelFormat->nRedShift;
    objServerPixelFormat.greenShift = pPixelFormat->nGreenShift;
    objServerPixelFormat.blueShift = pPixelFormat->nBlueShift;
    objServerPixelFormat.bigEndian = pPixelFormat->nBigEndianFlag;

    SetFBProperties(m_objFrameBuffer.GetDimension(), objServerPixelFormat);
}

// ********************************************************************************
/// <summary>
/// ¥¶¿Ì∑˛ŒÒ∆˜∑¢ÀÕ¿¥π‚±Í–Œ◊¥∏ƒ±‰«Î«Û
/// </summary>
/// <param name="pEndpoint"></param>
/// <param name="pPacket"></param>
/// <created>Andy,2021/4/12</created>
/// <changed>Andy,2021/4/12</changed>
// ********************************************************************************
void CRemoteViewerCore::OnCursorShapeChangedRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket)
{
    VNCP::CURSOR_SHAPE_INFO* pCursorSharpeInfo = (VNCP::CURSOR_SHAPE_INFO*)pPacket->GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();

    LOG_INFO("New rich cursor");

    const U16 nWidth = pCursorSharpeInfo->nWidth;
    const U16 nHeight = pCursorSharpeInfo->nHeight;
    const U8 nBytesPerPixel = m_objFrameBuffer.GetBytesPerPixel();
    vector<U8> arrCursor;
    vector<U8> arrBitmask;
    const size_t nCursorLen = nWidth * nHeight * nBytesPerPixel;

    pPacket->SetReadingOffset(sizeof(VNCP::VNC_MESSAGE_HEADER) + sizeof(VNCP::CURSOR_SHAPE_INFO));

    if (nCursorLen != 0)
    {
        arrCursor.resize(nCursorLen);
        pPacket->Read(&arrCursor.front(), nCursorLen);

        const size_t bitmaskLen = ((nWidth + 7) / 8) * nHeight;

        arrBitmask.resize(bitmaskLen);
        pPacket->Read(&arrBitmask.front(), bitmaskLen);
    }

    CPoint ptHotSpot(pCursorSharpeInfo->nX, pCursorSharpeInfo->nY);

    LOG_INFO("Setting new rich cursor...");
//    m_objFBUpdateNotifier.SetNewCursor(
//        &ptHotSpot
//        , nWidth
//        , nHeight
//        , &arrCursor
//        , &arrBitmask);

    m_spCoreEventsAdapter->OnCursorShapeChanged();
}

// ********************************************************************************
/// <summary>
/// ¥¶¿Ì∑˛ŒÒ∆˜∑¢ÀÕ¿¥π‚±ÍŒª÷√∏ƒ±‰«Î«Û
/// </summary>
/// <param name="pEndpoint"></param>
/// <param name="pPacket"></param>
/// <created>Andy,2021/4/12</created>
/// <changed>Andy,2021/4/12</changed>
// ********************************************************************************
void CRemoteViewerCore::OnCursorPosChangedRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket)
{
    VNCP::POINT_INFO* pPosInfo = (VNCP::POINT_INFO*)pPacket->GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();
    const CPoint ptPosition(pPosInfo->nX, pPosInfo->nY);

//    m_objFBUpdateNotifier.UpdatePointerPos(&ptPosition);
}

// ********************************************************************************
/// <summary>
/// ¥¶¿Ì∑˛ŒÒ∆˜∑¢ÀÕ¿¥µƒœÏ¡Â«Î«Û
/// </summary>
/// <param name="pEndpoint"> ’∑¢ ˝æ›∞¸∂Àµ„</param>
/// <param name="pPacket">÷∏œÚ ˝æ›∞¸∂‘œÛµƒ÷∏’Î</param>
/// <created>Andy,2020/12/2</created>
/// <changed>Andy,2020/12/2</changed>
// ********************************************************************************
void CRemoteViewerCore::OnBellRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket)
{
    LOG_INFO("Bell!");

    m_spCoreEventsAdapter->OnBell();
}

// ********************************************************************************
/// <summary>
/// ¥¶¿Ì∑˛ŒÒ∆˜∑¢ÀÕµƒºÙÃ˘«Î«Û
/// </summary>
/// <param name="pEndpoint"> ’∑¢ ˝æ›∞¸∂Àµ„</param>
/// <param name="pPacket">÷∏œÚ ˝æ›∞¸∂‘œÛµƒ÷∏’Î</param>
/// <created>Andy,2020/12/2</created>
/// <changed>Andy,2020/12/2</changed>
// ********************************************************************************
void CRemoteViewerCore::OnServerCutTextRequest(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket* pPacket)
{
    VNCP::CUT_TEXT_EVENT* pCutTextInfo = (VNCP::CUT_TEXT_EVENT*)pPacket->GetPayloadPtr<VNCP::VNC_MESSAGE_HEADER>();
    string strCutText((char*)pCutTextInfo->pText, pCutTextInfo->nLength);

    m_spCoreEventsAdapter->OnCutText(strCutText);

//    LOG_DEBUG("ServerCutText:%s",CT2A(strCutText));
}



// ********************************************************************************
/// <summary>
/// ¥¶¿Ì ’µΩ∑˛ŒÒ∆˜∑¢ÀÕµƒ∏¸–¬«¯”Úœ˚œ¢
/// </summary>
/// <param name="pEndpoint"> ’∑¢ ˝æ›∞¸∂Àµ„</param>
/// <param name="pPacket">÷∏œÚ ˝æ›∞¸∂‘œÛµƒ÷∏’Î</param>
/// <created>Andy,2020/12/2</created>
/// <changed>Andy,2020/12/2</changed>
// ********************************************************************************
void CRemoteViewerCore::OnReceivedVNCPacket(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CVNCPacket * pPacket)
{
    // “ª∂®“™ºÏ≤È
    CHECK_POINTER(m_pVNCProxy);

    // ≈–∂œµ±«∞µƒ ˝æ›∞¸ «∑Ò Ù”⁄∏√øÕªß∂ÀÀ˘ Ùµƒª·ª∞

    if (pPacket->GetSessionID() != m_pVNCProxy->GetSessionID())
    {
        LOG_ERROR("The packet's session ID is Invalid(type = %u)", pPacket->GetSessionID());
        return;
    }

    const U32 nType = pPacket->GetType();

    LOG_DEBUG("Received a vnc packet (type = %d)", nType);

    switch (nType)
    {
    case VNCP::MT_READY:
        OnReadyReponse(pEndpoint, pPacket);
        break;

    case VNCP::MT_SERVER_INIT_REQUEST:
        OnInitRequest(pEndpoint, pPacket);
        break;

    case VNCP::MT_SERVER_FRAME_DATA_FRAG:
        OnFrameDataRequest(pEndpoint, pPacket);
        break;

    case VNCP::MT_SERVER_DESKTOP_SIZE:
        OnDeskSizeRequest(pEndpoint, pPacket);
        break;

    case VNCP::MT_SERVER_DESKTOP_PIXEL_FORMAT:
        OnFramePixelFormatRequest(pEndpoint, pPacket);
        break;

    case VNCP::MT_SERVER_CURSOR_SHAPE_CHANGED:
        OnCursorShapeChangedRequest(pEndpoint, pPacket);
        break;

    case VNCP::MT_SERVER_CURSOR_POS_CHANGED:
        OnCursorPosChangedRequest(pEndpoint, pPacket);
        break;

    case VNCP::MT_SERVER_BELL:
        OnBellRequest(pEndpoint, pPacket);
        break;

    case VNCP::MT_SERVER_CUT_TEXT:
        OnServerCutTextRequest(pEndpoint, pPacket);
        break;

    case VNCP::MT_SERVER_ENABLE_REQUEST:
        OnEnableInputRequest(pEndpoint, pPacket);
        break;

    case VNCP::MT_SERVER_OS_INFO:
        OnOSInfoRequest(pEndpoint, pPacket);
        break;

    case VNCP::MT_SERVER_SESSION_STATE:
        OnSendSessionStateRequest(pEndpoint, pPacket);
        break;

    default:
        LOG_INFO("Unknown %d protocol code received", (int)nType);
        break;
    }
}


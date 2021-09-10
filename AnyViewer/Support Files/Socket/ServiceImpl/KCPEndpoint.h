//
//  KCPEndpoint.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/9.
//

#ifndef KCPEndpoint_h
#define KCPEndpoint_h

#pragma once

#include "CNetEndpoint.h"
#include "CAIMTime.h"
#include "ikcp.h"
#include "Log.h"
#include "VNCProtocol.h"

#include <atomic>


    //**********************************************************************
    /// @brief ø…øøµƒÕ®–≈∂Àµ„¿‡
    ///
    /// œ¬√Êµƒ¿‡ «“ª∏ˆø…øøª·ª∞ƒ£∞Â¿‡£¨”––ƒÃ¯π¶ƒ‹£¨≤¢«“ƒ‹±£÷§ ˝æ›∞¸∞Ÿ∑÷÷Æ∞Ÿ’˝»∑µƒµΩ¥Ôƒø±Í÷˜ª˙\n
    ///   ”¶”⁄∑˛ŒÒ∆˜∂À∫ÕøÕªß∂À
    //**********************************************************************
template <typename PacketT, class TransferT = CNetUDPDataTransfer,const U32 nIntputBufferSize = 10 * 1024>
class CKCPEndpoint
    : public CNetEndpoint<PacketT, TransferT, nIntputBufferSize>
    ,public CKCP
{
public:
    CKCPEndpoint(CNetEndpointEventSink* pNetConversationEventSink = NULL)
        : CNetEndpoint<PacketT, TransferT, nIntputBufferSize>(pNetConversationEventSink)
        , m_nNextID(0)
        , m_objKCPIntputBuffer(1024*1024)
    {
    }

    virtual ~CKCPEndpoint()
    {
        if (!m_bStopReceving)
        {
            m_bStopReceving = true;

            if (m_objRecevingThread.joinable())
            {
                m_objRecevingThread.join();
            }
        }
    }

public:
    virtual bool Initial(int pSocket = NULL) override;

    /// @brief ¡¨Ω”µΩ÷∏∂®µƒ∑˛ŒÒ∆˜
    virtual bool Connect(unsigned int nIP, unsigned int nPort, int nTimeout = 30) override;

    virtual bool Send(CNetPacket* pPacket) override;

    // ∑¢ÀÕ ˝æ›
    virtual bool Send(const BYTE* pData, const unsigned int nLen) override;


    virtual bool ExtractPacketfromBuffer(CDataStreamBuffer& objDataBuffer);

    virtual void Close(bool bRude = false)  override;

    // ∑¢ÀÕºÚµ•µƒœ˚œ¢
    bool SendCommonRequest(const unsigned int nType);
protected:

    //  ‰≥ˆ ˝æ›
    virtual int KCPOutput(const char *pBuffer, const int nLen) override;

    virtual void Writelog(const char *lpLog) override;
protected:
    
    void KCPRecevingThread();

protected:
    //--------------------------------------------------------------------------------------------------------------------
    //∫Ø ˝√˚≥∆£∫ CKCPEndpoint::Clear
    /// @brief  «Â≥˝»˝∏ˆ∂”¡–≤¢…æ≥˝∂”¡–÷–µƒΩ⁄µ„
    ///
    /// @author ª∆¿ˆ‘∆
    //--------------------------------------------------------------------------------------------------------------------
    void Clear()
    {
        //std::lock_guard<std::recursive_mutex> objAutoLocker(m_objCritSec);


    }

    UINT CreateID() { return ++m_nNextID; }



private:

    mutable std::recursive_mutex     m_objCritSec;                 ///< ª·ª∞¡¥±Ì¡ŸΩÁ«¯

    DECLARE_MEMBER_AND_GET_METHOD_V11(UINT, m_nNextID, NextID, 0);     /// < œ¬“ª∏ˆ ˝æ›∞¸ID±‡∫≈
    std::thread              m_objRecevingThread;
    std::atomic_bool         m_bStopReceving{false};
    CDataStreamBuffer        m_objKCPIntputBuffer;                    ///< ¥Ê∑≈¥”KCPΩ” ’ ˝æ›µƒª∫≥Â«¯
};


template<typename PacketT, class TransferT, const U32 nIntputBufferSize>
inline bool CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::Initial(int pSocket)
{
    bool bResult = CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::Initial(pSocket);

    if (bResult)
    {
        CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::m_objInputBuffer.Allocate(2 * 1024);
        bResult = KCPInit(0);

        if (bResult)
        {
            KCPSetNoDelay(1, 10, 2, 1);   ///< º´ÀŸƒ£ Ω
            SetStreamMod(CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::GetClient()? false:true);    /// ◊¢“‚øÕªß∂À∫Õ∑˛ŒÒ∂À≤…”√µƒƒ£ Ω≤ªÕ¨£¨
            m_bStopReceving = false;

            std::thread objRecevingThread(std::bind([this]() { KCPRecevingThread(); }));

            m_objRecevingThread = std::move(objRecevingThread);
        }
    }

    return bResult;
}

//--------------------------------------------------------------------------------------------------------------------
//∫Ø ˝√˚≥∆£∫ CNetClientImpl::Connect
/// @brief ¡¨Ω”µΩ÷∏∂®µƒ∑˛ŒÒ∆˜
///
/// @param _in UINT nIP->∑˛ŒÒ∆˜µƒIPµÿ÷∑
/// @param _in USHORT nPort->∑˛ŒÒ∆˜µƒ∂Àø⁄
/// @param _in int nTimeout->¡¨Ω”≥¨ ± ±º‰(»± ° «30√Î)
/// @return  bool true: ≥…π¶, false:  ß∞‹
/// @author ª∆¿ˆ‘∆
//--------------------------------------------------------------------------------------------------------------------
template<typename PacketT, class TransferT, const U32 nIntputBufferSize>
inline bool CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::Connect(unsigned int nIP, unsigned int nPort, int nTimeout)
{
    std::lock_guard<std::recursive_mutex> objAutoLocker(m_objCritSec); //±ÿ–Î»Á¥À£¨∏√∫Ø ˝ø…ƒ‹≤¢∑¢µ˜”√
    bool bResult = CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::Connect(nIP, nPort, nTimeout);

    return bResult;
}


// ********************************************************************************
/// <summary>
/// ∞— ˝æ›∑¢ÀÕµΩKCP“˝«Ê÷–»•
/// </summary>
/// <param name="pData">÷∏œÚ ˝æ›µƒ÷∏’Î</param>
/// <param name="nLen">±ª∑¢ÀÕ ˝æ›µƒ≥§∂»</param>
/// <returns></returns>
/// <created>Andy,2020/12/28</created>
/// <changed>Andy,2020/12/28</changed>
// ********************************************************************************
template<typename PacketT, class TransferT, const U32 nIntputBufferSize>
inline bool CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::Send(CNetPacket* pPacket)
{
    return CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::Send(pPacket);
}


// ********************************************************************************
/// <summary>
/// ∞— ˝æ›∑¢ÀÕµΩKCP“˝«Ê÷–»•
/// </summary>
/// <param name="pData">÷∏œÚ ˝æ›µƒ÷∏’Î</param>
/// <param name="nLen">±ª∑¢ÀÕ ˝æ›µƒ≥§∂»</param>
/// <returns></returns>
/// <created>Andy,2020/12/28</created>
/// <changed>Andy,2020/12/28</changed>
// ********************************************************************************
template<typename PacketT, class TransferT, const U32 nIntputBufferSize>
inline bool CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::Send(const BYTE * pData, const unsigned int nLen)
{
    const int nBytesSent = KCPSend((const char*)pData, nLen);

    return (nBytesSent >= 0);
}


// ********************************************************************************
/// <summary>
/// ¥”KCP“˝«Ê÷–∂¡»° ˝æ›£¨≤¢∏˙–¬KCPµƒ◊¥Ã¨
/// </summary>
/// <created>Andy,2020/12/28</created>
/// <changed>Andy,2020/12/28</changed>
// ********************************************************************************
template<typename PacketT, class TransferT, const U32 nIntputBufferSize>
inline void CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::KCPRecevingThread()
{
    while (!m_bStopReceving)
    {
        KCPUpdate((U32)(CAIMTime::GetCurMilliseconds()&0xfffffffful));

        const int nBytesReceived  = KCPRecv((char *)m_objKCPIntputBuffer.GetBufferFreeHeadPtr(), m_objKCPIntputBuffer.GetFreeSize());

        if (nBytesReceived > 0)
        {
            m_objKCPIntputBuffer.IncreaseDataLen(nBytesReceived);
            CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::ExtractPacketfromBuffer(m_objKCPIntputBuffer);
        }

        // »Áπ˚Ω” ’ª∫≥Â«¯∫Õ∑¢ÀÕª∫≥Â÷–∂º”– ˝æ›£¨ƒ«≤ª”√µ»¥˝
        if(GetRecevingQueueSize() == 0 && GetSendingQueueSize() == 0)
        {
            std::chrono::milliseconds objSleepDyratuib(10);

            std::this_thread::sleep_for(objSleepDyratuib);
        }
    }
}

// ********************************************************************************
/// <summary>
/// ∏√∫Ø ˝≤ª‘⁄ µº ∑÷Œˆ ˝æ›∞¸£¨∂¯ «∞— ’µΩµƒ ˝æ›÷±Ω”–¥»ÎKCP“˝«Ê
/// </summary>
/// <param name="objDataBuffer"> ˝æ›ª∫≥Â∂‘œÛµƒ≤Œøº</param>
/// <returns></returns>
/// <created>Andy,2020/12/28</created>
/// <changed>Andy,2020/12/28</changed>
// ********************************************************************************
template<typename PacketT, class TransferT, const U32 nIntputBufferSize>
inline bool CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::ExtractPacketfromBuffer(CDataStreamBuffer& objDataBuffer)
{
    const int nBytesInputed = KCPInput((const char*)objDataBuffer.GetBufferHeadPtr(), objDataBuffer.GetDataLen());

    objDataBuffer.Empty();

    return (nBytesInputed > 0);
}

template<typename PacketT, class TransferT, const U32 nIntputBufferSize>
inline void CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::Close(bool bRude)
{
    CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::Close();
}

template<typename PacketT, class TransferT, U32 nIntputBufferSize>
inline bool CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::SendCommonRequest(const unsigned int nType)
{
    PacketT objRequestPacket;

    objRequestPacket.Initial(nType, 0, VNCP::OT_REQUEST, 0);

    return CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::Send(&objRequestPacket);
}


// ********************************************************************************
/// <summary>
/// ±ªKCP“˝«Êµ˜”√∑¢ÀÕ ˝æ›µΩµ◊≤„Õ¯¬Á
/// </summary>
/// <param name="pBuffer">÷∏œÚ±ª∑¢ÀÕ ˝æ›µƒ÷∏’Î</param>
/// <param name="nLen">±ª∑¢ÀÕ ˝æ›µƒ≥§∂»</param>
/// <returns></returns>
/// <created>Andy,2020/12/28</created>
/// <changed>Andy,2020/12/28</changed>
// ********************************************************************************
template<typename PacketT, class TransferT, const U32 nIntputBufferSize>
inline int CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::KCPOutput(const char * pBuffer, const int nLen)
{
    bool bResult = false;

    assert(NULL != pBuffer);

    if (NULL != pBuffer && nLen > 0)
    {
        const int nBytesSent = CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::m_pDataTransfer->Send((const BYTE *)pBuffer, nLen);
        bResult = (nBytesSent == int(nLen));

        if (SOCKET_ERROR == nBytesSent)
        {
            if (std::is_same<TransferT, CNetTCPDataTransfer>::value)
            {
                CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::Connect(
                    CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::m_pDataTransfer->GetIP()
                    , CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::m_pDataTransfer->GetPort(), 5);
                CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::m_pNetEndpointEventSink->OnReconnectEventHandle(
                    CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::shared_from_this());
            }
        }
    }

    return bResult;
}

template<typename PacketT, class TransferT, const U32 nIntputBufferSize>
inline void CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::Writelog(const char * lpLog)
{
    LOG_DEBUG("%s",lpLog);
}


#endif /* KCPEndpoint_h */

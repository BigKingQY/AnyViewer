//
//  ikcp.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/9.
//

#ifndef ikcp_h
#define ikcp_h

#pragma once

#include "Typedef.h"
#include "Ax3Event.h"

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <atomic>


#ifndef INLINE
#if defined(__GNUC__)

#if (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1))
#define INLINE         __inline__ __attribute__((always_inline))
#else
#define INLINE         __inline__
#endif

#elif (defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__))
#define INLINE __inline
#else
#define INLINE
#endif
#endif

#if (!defined(__cplusplus)) && (!defined(inline))
#define inline INLINE
#endif



#ifdef _MSC_VER
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4996)
#endif


//---------------------------------------------------------------------
// BYTE ORDER & ALIGNMENT
//---------------------------------------------------------------------
#ifndef IWORDS_BIG_ENDIAN
    #ifdef _BIG_ENDIAN_
        #if _BIG_ENDIAN_
            #define IWORDS_BIG_ENDIAN 1
        #endif
    #endif
    #ifndef IWORDS_BIG_ENDIAN
        #if defined(__hppa__) || \
            defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
            (defined(__MIPS__) && defined(__MIPSEB__)) || \
            defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
            defined(__sparc__) || defined(__powerpc__) || \
            defined(__mc68000__) || defined(__s390x__) || defined(__s390__)
            #define IWORDS_BIG_ENDIAN 1
        #endif
    #endif
    #ifndef IWORDS_BIG_ENDIAN
        #define IWORDS_BIG_ENDIAN  0
    #endif
#endif

#ifndef IWORDS_MUST_ALIGN
    #if defined(__i386__) || defined(__i386) || defined(_i386_)
        #define IWORDS_MUST_ALIGN 0
    #elif defined(_M_IX86) || defined(_X86_) || defined(__x86_64__)
        #define IWORDS_MUST_ALIGN 0
    #elif defined(__amd64) || defined(__amd64__)
        #define IWORDS_MUST_ALIGN 0
    #else
        #define IWORDS_MUST_ALIGN 1
    #endif
#endif


//=====================================================================
//  ˝æ›∑÷∆¨–≈œ¢
//=====================================================================
typedef struct
{
    U32 nConversationID;    ///< Œ™“ª∏ˆ±Ì æª·ª∞±‡∫≈µƒ’˚ ˝£¨∫ÕTCPµƒ conv“ª—˘£¨Õ®–≈À´∑Ω–Ë±£÷§ convœ‡Õ¨£¨œ‡ª•µƒ ˝æ›∞¸≤≈ƒ‹πª±ªΩ” ‹°£convŒ®“ª±Í ∂“ª∏ˆª·ª∞£¨µ´Õ®–≈À´∑Ωø…“‘Õ¨ ±¥Ê‘⁄∂‡∏ˆª·ª∞°£
    U8 nCMD;               ///< ”√¿¥«¯∑÷∑÷∆¨µƒ◊˜”√°£IKCP_CMD_PUSH£∫ ˝æ›∑÷∆¨£ªIKCP_CMD_ACK£∫ack∑÷∆¨£ª IKCP_CMD_WASK£∫«Î«Û∏Ê÷™¥∞ø⁄¥Û–°£ªIKCP_CMD_WINS£∫∏Ê÷™¥∞ø⁄¥Û–°°£
    U8 nFrag;              ///< ”√ªß ˝æ›ø…ƒ‹ª·±ª∑÷≥…∂‡∏ˆKCP∞¸∑¢ÀÕ£¨frag±Í ∂segment∑÷∆¨ID£®‘⁄message÷–µƒÀ˜“˝£¨”…¥ÛµΩ–°£¨0±Ì æ◊Ó∫Û“ª∏ˆ∑÷∆¨£©
    U16 nWnd;               ///<  £”‡Ω” ’¥∞ø⁄¥Û–°£®Ω” ’¥∞ø⁄¥Û–°-Ω” ’∂”¡–¥Û–°£©£¨∑¢ÀÕ∑Ωµƒ∑¢ÀÕ¥∞ø⁄≤ªƒ‹≥¨π˝Ω” ’∑Ω∏¯≥ˆµƒ ˝÷µ°£
    U32 nTimestamp;         ///< ∑¢ÀÕ ±øÃµƒ ±º‰¥¡
    U32 nSN;                ///< ∑÷∆¨segmentµƒ–Ú∫≈£¨∞¥1¿€¥Œµ›‘ˆ°£
    U32 nUnAckSN;           ///< ¥˝Ω” ’œ˚œ¢–Ú∫≈(Ω” ’ª¨∂Ø¥∞ø⁄◊Û∂À)°£∂‘”⁄Œ¥∂™∞¸µƒÕ¯¬Á¿¥Àµ£¨una «œ¬“ª∏ˆø…Ω” ’µƒ–Ú∫≈£¨»Á ’µΩsn=10µƒ∞¸£¨unaŒ™11°£
    U32 nLen;               ///< ‘ÿ∫… ˝æ›≥§∂»°£
    U32 nResendTimestamp;   ///< œ¬¥Œ≥¨ ±÷ÿ¥´µƒ ±º‰¥¡°£
    U32 nRTO;               ///< ∏√∑÷∆¨µƒ≥¨ ±÷ÿ¥´µ»¥˝ ±º‰£¨∆‰º∆À„∑Ω∑®Õ¨TCP°£
    U32 nFastACK;           ///<  ’µΩack ±º∆À„µƒ∏√∑÷∆¨±ªÃ¯π˝µƒ¿€º∆¥Œ ˝£¨¥À◊÷∂Œ”√”⁄øÏÀŸ÷ÿ¥´£¨◊‘∂®“Â–Ë“™º∏¥Œ»∑»œø™ ºøÏÀŸ÷ÿ¥´°£
    U32 nXMIT;              ///< ∑¢ÀÕ∑÷∆¨µƒ¥Œ ˝£¨√ø∑¢ÀÕ“ª¥Œº”“ª°£∑¢ÀÕµƒ¥Œ ˝∂‘RTOµƒº∆À„”–”∞œÏ£¨µ´ «±»TCP¿¥Àµ£¨”∞œÏª·–°“ª–©£¨º∆À„ÀºœÎ¿‡À∆
    char pData[1];
}IKCP_FRAGMENT,*PIKCP_FRAGMENT;

using IKCP_FRAGMENT_LIST = std::vector<PIKCP_FRAGMENT>;


class CACKRec
{
public:
    CACKRec(U32 nTimestamp = 0,U32 nSN = 0)
        : m_nTimestamp(nTimestamp)
        , m_nSN(nSN)
    {}
    CACKRec(const CACKRec& refSrc)
        : m_nTimestamp(refSrc.m_nTimestamp)
        , m_nSN(refSrc.m_nSN)
    {
    }

    CACKRec& operator=(const CACKRec& refSrc)
    {
        if (this == &refSrc)
        {
            return *this;
        }

        m_nTimestamp = (refSrc.m_nTimestamp);
        m_nSN = (refSrc.m_nSN);

        return *this;
    }
public:
    U32 m_nTimestamp{0};         ///< ∑¢ÀÕ ±øÃµƒ ±º‰¥¡
    U32 m_nSN{0};                ///< ∑÷∆¨segmentµƒ–Ú∫≈£¨∞¥1¿€¥Œµ›‘ˆ°£
};

//using CACKRecQueue = moodycamel::BlockingConcurrentQueue<CACKRec>;


class CKCP
{
public:
    CKCP();

    virtual ~CKCP();

    /// <summary>»’÷æ¿‡–Õ√∂æŸ∂®“Â</summary>
    enum LOG_TYPE
    {
        IKCP_LOG_OUTPUT = 1,
        IKCP_LOG_INPUT = 2,
        IKCP_LOG_SEND = 4,
        IKCP_LOG_RECV = 8,
        IKCP_LOG_IN_DATA = 16,
        IKCP_LOG_IN_ACK = 32,
        IKCP_LOG_IN_PROBE = 64,
        IKCP_LOG_IN_WINS = 128,
        IKCP_LOG_OUT_DATA = 256,
        IKCP_LOG_OUT_ACK = 512,
        IKCP_LOG_OUT_PROBE = 1024,
        IKCP_LOG_OUT_WINS = 2048,
    };
public:
    // ªÒ»°Ω” ’∂”¡–µƒ¥Û–°
    size_t GetRecevingQueueSize();

    // ªÒ»°∑¢ÀÕ∂”¡–µƒ¥Û–°
    size_t GetSendingQueueSize();

public:

    // create a new kcp control object, 'nCoonversationID' must equal in two endpoint
    // from the same connection. 'user' will be passed to the output callback
    // output callback can be setup like this: 'kcp->output = my_udp_output'
    bool KCPInit(const U32 nCoonversationID);

    // user/upper level recv: returns size, returns below zero for EAGAIN
    int KCPRecv(char *pBuffer, const int nLen);

    // user/upper level send, returns below zero for error
    int KCPSend(const char *pBuffer, const int nLen);

    // update m_nState (call it repeatedly, every 10ms-100ms), or you can ask
    // KCPCheck when to call it again (without KCPInput/_send calling).
    // 'm_nCurrentTime' - m_nCurrentTime timestamp in millisec.
    void KCPUpdate(const U32 nCurrentTime);

    // Determine when should you invoke KCPUpdate:
    // returns when you should invoke KCPUpdate in millisec, if there
    // is no KCPInput/_send calling. you can call KCPUpdate in that
    // time, instead of call update repeatly.
    // Important to reduce unnacessary KCPUpdate invoking. use it to
    // schedule KCPUpdate (eg. implementing an epoll-like mechanism,
    // or optimize KCPUpdate when handling massive kcp connections)
    U32 KCPCheck(U32 m_nCurrentTime);

    // when you received a low level packet (eg. UDP packet), call it
    int KCPInput(const char *pData, const int nInLen);


    // change MTU size, default is 1400
    int KCPSetMTU(const int nMTU);

    // set maximum window size: sndwnd=32, rcvwnd=32 by default
    int KCPSetWinSize(const int nSendingWnd, const int nRecevingWin);

    // fastest: KCPSetNoDelay(kcp, 1, 20, 2, 1)
    // nodelay: 0:disable(default), 1:enable
    // interval: internal update timer interval in millisec, default is 100ms
    // nResend: 0:disable fast nResend(default), 1:enable fast nResend
    // nDisenableStreamCtrl: 0:normal congestion control(default), 1:disable congestion control
    int KCPSetNoDelay(int nNodelay, int nInterval, int nResend, int nDisenableStreamCtrl);

    // setup allocator
    void KCPSetAllocator(void* (*new_malloc)(size_t), void(*new_free)(void*));

    // read nCoonversationID
    U32 KCPGetConversationID(const void *ptr);

protected:

    //  ‰≥ˆ ˝æ›
    virtual int KCPOutput(const char *pBuffer, const int nLen) = 0;

    // –¥»’÷æ
    virtual void Writelog(const char * /*lpLog*/) {}

private:  // √¸¡Ó¥¶¿Ì∫Ø ˝◊È

    // ¥¶¿Ì ’µΩµƒACK√¸¡Ó
    void OnACK(IKCP_FRAGMENT objFragment, U32& nMaxACK, int& nFlag, U32& nLastTimestamp);

    // ¥¶¿Ì ’µΩµƒPush√¸¡Ó
    void OnPush(IKCP_FRAGMENT objFragment, const char *pData);
private:
    // flush pending data
    void Flush();

    // check the size of pNext message in the recv queue
    int CalcNextMsgSize();

    // ≈–∂œ «∑ÒøÕªß ‰≥ˆµ±«∞µƒ»’÷æ¿‡–Õ
    int Canlog(const int nMask);

    // º∆À„÷ÿ¥´≥¨ ±
    void CaleRTO(const int nRTT);

    // ÷ÿ–¬º∆À„µ⁄“ª∏ˆ√ª”–”¶¥µƒ ˝æ›∞¸
    void CaleSNFirstPacketNAck();

    // ∞—“—æ≠±ªΩ” ’’ﬂ»∑»œµƒ ˝æ›∞¸¥”∑¢ÀÕ∂”¡–÷–…æ≥˝
    void RemoveFragAcked(const U32 nSN);

    // ∞—“—æ≠±ªΩ” ’’ﬂ»∑»œµƒ ˝æ›∞¸¥”∑¢ÀÕ∂”¡–÷–…æ≥˝
    void RemoveFragUnAcked(const U32 nSN);

    // º∆À„∑÷∆¨±ªÃ¯π˝µƒ¥Œ ˝
    void CalcFaskAck(const U32 nSN, const U32 nTimestamp);

    // ÃÌº”“ª∏ˆACKµΩ¡–±Ì÷–
    void PushACK(const U32 nSN, const U32 nTimestamp);

    // ¥”ACK¡–±Ì÷–ªÒ»°“ª∏ˆ÷∏∂®µƒACK
    bool GetACK(CACKRec& refACKRec);

    //  ±£¥Ê–¬µƒ∑÷∆¨µΩΩ” ’ª∫≥Â«¯
    void StoreNewFrag(IKCP_FRAGMENT *pNewSegment);

    // ∞—–¬µƒ∑÷∆¨–≈–ƒ±‡¬Î¥Û÷∏∂®µƒª∫≥Â«¯÷–
    char *EncodeFrag(const IKCP_FRAGMENT *pFragment, char *pData);

    // µ√µΩø’œ–¥∞ø⁄µƒ≥ﬂ¥Á
    int GetFreeRecevingWinSize();

    // …Ë÷√ ±º‰º‰∏Ù
    int SetInterval(const int nInterval);

    // µ√µΩµ»¥˝∑¢ÀÕµƒ∑÷∆¨ ˝¡ø
    int GetFragsBeSent();

    // «Â≥˝∑÷∆¨¡–±Ì
    void ClearFragList(IKCP_FRAGMENT_LIST& refFragList);

    //  ‰≥ˆ»’÷æ
    void Log(int nMask, const char *fmt, ...);
private:
    ///< ≥ı ºªØ kcp∂‘œÛ£¨nCoonversationIDŒ™“ª∏ˆ±Ì æª·ª∞±‡∫≈µƒ’˚ ˝£¨∫Õtcpµƒ nCoonversationID“ª—˘£¨Õ®–≈À´
    ///< ∑Ω–Ë±£÷§ nCoonversationIDœ‡Õ¨£¨œ‡ª•µƒ ˝æ›∞¸≤≈ƒ‹πª±ª»œø…£¨user «“ª∏ˆ∏¯ªÿµ˜∫Ø ˝µƒ÷∏’Î
    U32                  m_nConversationID{ 0 };
    
    ///< ◊Ó¥Û¥´ ‰µ•‘™£¨ƒ¨»œ ˝æ›Œ™1400£¨◊Ó–°Œ™50£ª
    DECLARE_MEMBER_AND_METHOD_V11(U32, m_nMTU, MTU, 1400);

    ///< ◊Ó¥Û∑÷∆¨¥Û–°£¨≤ª¥Û”⁄mtu£ª
    DECLARE_MEMBER_AND_METHOD_V11(U32,m_nMSS, MSS, 0);

    ///<  ¡¨Ω”◊¥Ã¨£®0xFFFFFFFF±Ì æ∂œø™¡¨Ω”£©£ª
    U32                  m_nState{ 0 };

    ///< µ⁄“ª∏ˆŒ¥»∑»œµƒ∞¸£ª
    U32                  m_nSNFirstPacketNAck{ 0 };

    ///< œ¬“ª∏ˆ¥˝∑÷≈‰µƒ∞¸µƒ–Ú∫≈£ª
    std::atomic<U32>     m_nSNNext{ 0 };

    ///< ¥˝Ω” ’œ˚œ¢–Ú∫≈°£Œ™¡À±£÷§∞¸µƒÀ≥–Ú£¨Ω” ’∑Ωª·Œ¨ª§“ª∏ˆΩ” ’¥∞ø⁄£¨Ω” ’¥∞ø⁄”–“ª∏ˆ∆ º–Ú∫≈m_nSNNextRecving£®¥˝Ω” ’œ˚œ¢–Ú∫≈£©“‘º∞Œ≤–Ú∫≈ m_nSNNextRecving + m_nRecevingWin£®Ω” ’¥∞ø⁄¥Û–°£©£ª
    U32                  m_nSNNextRecving{ 0 };

    
    U32                  m_nRecentTimestamp{ 0 };


    U32                  m_nLastACKTimestamp{ 0 };

    ///< ”µ»˚¥∞ø⁄„–÷µ£¨“‘∞¸Œ™µ•Œª£®TCP“‘◊÷Ω⁄Œ™µ•Œª£©£ª
    U32                  m_nCongestionWinthrd{ 0 };

    ///<  RTTµƒ±‰ªØ¡ø£¨¥˙±Ì¡¨Ω”µƒ∂∂∂Ø«Èøˆ£®ΩˆΩˆ”√”⁄º∆À„÷ÿ¥´≥¨ ±£©£ª
    int                   m_nRXRTTVal{ 0 };

    ///< smoothed round trip time£¨∆Ωª¨∫ÛµƒRTT£®ΩˆΩˆ”√”⁄º∆À„÷ÿ¥´≥¨ ±£©£ª
    U32                  m_nRXSmoothedRTT{ 0 };

    ///<  ”…ACKΩ” ’—”≥Ÿº∆À„≥ˆ¿¥µƒ÷ÿ¥´≥¨ ± ±º‰£ª
    U32                  m_nRXRetrTimeout{ 0 };

    ///< ◊Ó–°÷ÿ¥´≥¨ ± ±º‰£ª
    U32                  m_nRXMinRetrTimeout{ 0 };

    ///< ∑¢ÀÕ¥∞ø⁄¥Û–°£ª
    U32                  m_nSendingWin{ 0 };

    ///< Ω” ’¥∞ø⁄¥Û–°£ª
    U32                  m_nRecevingWin{ 0 };

    ///< ‘∂∂ÀΩ” ’¥∞ø⁄¥Û–°£ª
    U32                  m_nPeerRecevingWin{ 0 };
    
    ///< ”µ»˚¥∞ø⁄¥Û–°
    U32                  m_nCongestionWin{ 0 };

    ///< ÃΩ≤È±‰¡ø£¨IKCP_ASK_TELL±Ì æ∏Ê÷™‘∂∂À¥∞ø⁄¥Û–°°£IKCP_ASK_SEND±Ì æ«Î«Û‘∂∂À∏Ê÷™¥∞ø⁄¥Û–°£ª
    U32                  m_nProbe{ 0 };

    ///< µ±«∞µƒ ±º‰
    U32                  m_nCurrentTime{ 0 };

    ///< ƒ⁄≤øflushÀ¢–¬º‰∏Ù£¨∂‘œµÕ≥—≠ª∑–ß¬ ”–∑«≥£÷ÿ“™”∞œÏ£ª
    U32                  m_nInterval{ 0 };

    ///< œ¬¥ŒflushÀ¢–¬ ±º‰¥¡£ª
    U32                  m_nNextFlushTimestamp{ 0 };

    ///< ∑¢ÀÕsegmentµƒ¥Œ ˝£¨µ±segmentµƒm_nXMIT‘ˆº” ±£¨m_nXMIT‘ˆº”£®µ⁄“ª¥ŒªÚ÷ÿ¥´≥˝Õ‚£©£ª
    U32                  m_nXMIT{ 0 };


    ///<  «∑Ò∆Ù∂ØŒﬁ—”≥Ÿƒ£ Ω°£Œﬁ—”≥Ÿƒ£ ΩrtominΩ´…Ë÷√Œ™0£¨”µ»˚øÿ÷∆≤ª∆Ù∂Ø£ª
    int                  m_bNodelay{ 0 };

    ///<  «∑Òµ˜”√π˝update∫Ø ˝µƒ±Í ∂£ª
    U32                  m_bUpdated{ 0 };

    ///< œ¬¥ŒÃΩ≤È¥∞ø⁄µƒ ±º‰¥¡£ª
    U32                  m_nNextProbeTimestamp{ 0 };

    ///< ÃΩ≤È¥∞ø⁄–Ë“™µ»¥˝µƒ ±º‰£ª
    U32                  m_nProbeWaitingTime{ 0 };

    ///< ◊Ó¥Û÷ÿ¥´¥Œ ˝£¨±ª»œŒ™¡¨Ω”÷–∂œ£ª
    U32                  m_nDeadLink{ 0 };
    
    ///< ø…∑¢ÀÕµƒ◊Ó¥Û ˝æ›¡ø£ª
    U32                  m_nIncr{ 0 };

    
    std::recursive_mutex      m_objSendingQueueMutex;
    
    // ∑¢ÀÕÀ¯
    std::mutex               m_objSendingMutex;

    ///< ∑¢ÀÕ∂”¡–µƒ◊Ó¥Û÷µ,µ±µΩ¥Ô◊Ó¥Û÷µ∫ÛΩ´Õ£÷πΩ” ’”√ªß ‰»Î
    DECLARE_MEMBER_AND_METHOD_V11(U32, m_nSendingQueueSizeMax, SendingQueueSizeMax, 1024);
    
    CAx3Event                 m_objSendingQueueEvents;

    ///< ∑¢ÀÕœ˚œ¢µƒ∂”¡–£ª
    IKCP_FRAGMENT_LIST        m_objSendingQueue;

    ///< ∑¢ÀÕœ˚œ¢µƒª∫¥Ê£ª
    IKCP_FRAGMENT_LIST        m_objSendingBuffer;


    std::recursive_mutex      m_objRecevingMutex;

    ///< Ω” ’œ˚œ¢µƒ∂”¡–£ª
    IKCP_FRAGMENT_LIST        m_objRecevingQueue;

    ///< Ω” ’œ˚œ¢µƒ∂”¡–
    IKCP_FRAGMENT_LIST        m_objRecevingBuffer;

    ///<  ¥˝∑¢ÀÕµƒack¡–±Ì£ª
//    CACKRecQueue              m_queACKList{1024};

    ///<  ¥Ê¥¢∏˜÷÷ ˝æ›∞¸œ˚œ¢◊÷Ω⁄¡˜£ª
    char                     *m_pBuffer{ nullptr };
    
    ///<  ¥•∑¢øÏÀŸ÷ÿ¥´µƒ÷ÿ∏¥ACK∏ˆ ˝£ª
    int                      m_nFastResendAckThd{ 0 };

    
    int                      m_nFastLimit{ 0 };
    
    ///< »°œ˚”µ»˚øÿ÷∆
    int                      m_bCancelCongestionCtrl{ 0 };

    ///<   «∑Ò≤…”√¡˜¥´ ‰ƒ£ Ω£ª
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bStreamMod, StreamMod, true);

    int                      m_nLogMask{ 0x00FFFFFF };


};


template<class T>
inline T safe_pop_front(std::vector<T>& refList, std::recursive_mutex& refMutex)
{
    std::unique_lock<std::recursive_mutex> objAutoLocker(refMutex);
    T objResult = nullptr;

    if (refList.size() > 0)
    {
        objResult = *(refList.begin());
        refList.erase(refList.begin());
    }

    return objResult;
}

template<class T>
inline  T safe_pop_back(std::vector<T>& refList, std::recursive_mutex& refMutex)
{
    std::unique_lock<std::recursive_mutex> objAutoLocker(refMutex);
    T objResult = nullptr;

    if (refList.size() > 0)
    {
        objResult = *(refList.rbegin());
        refList.pop_back();
    }

    return objResult;
}

template<class T>
inline  void safe_push_back(std::vector<T>& refList, T objNewEntry, std::recursive_mutex& refMutex)
{
    std::unique_lock<std::recursive_mutex> objAutoLocker(refMutex);

    refList.push_back(objNewEntry);
}

template<class T>
inline  size_t safe_containers_size(std::vector<T>& refList,std::recursive_mutex& refMutex)
{
    std::unique_lock<std::recursive_mutex> objAutoLocker(refMutex);

    return refList.size();
}


#endif /* ikcp_h */

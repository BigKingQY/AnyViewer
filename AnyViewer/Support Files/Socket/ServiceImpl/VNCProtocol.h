//
//  VNCProtocol.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/9.
//

#ifndef VNCProtocol_h
#define VNCProtocol_h

#pragma once
#pragma warning(disable : 4200)
#include "Typedef.h"
#include "JsonSerializer.h"


#ifndef WIN32
#include <mutex>
#endif
#include <memory>

#define VNCP_PAYLOAD_LEN_MAX 0xFFFFFF


namespace VNCP
{
    /// ≤Ÿ◊˜¿‡–Õ√∂æŸ
    enum OPERATOR_TYPE
    {
        OT_REQUEST = 0,            ///< √∂æŸ£¨«Î«Û≤Ÿ◊˜
        OT_RESPONSE = 1,            ///< √∂æŸ£¨”¶¥≤Ÿ◊˜
    };

    /// ≤Ÿ◊˜Ω·π˚√∂æŸ
    enum OPERATOR_RESULT
    {
        OR_FAILURE = 0, ///< √∂æŸ£¨µ±«∞≤Ÿ◊˜ ß∞‹
        OR_SUCCESS = 2  ///< √∂æŸ£¨µ±«∞≤Ÿ◊˜≥…π¶
    };

    /// ¥ÌŒÛ¿‡–Õ√∂æŸ
    ///
    /// ŒÂ∞Ÿ“‘«∞µƒ¥ÌŒÛ∂®Œª—œ÷ÿ¥ÌŒÛ£¨¡¨Ω”Ω´∂œø™
    enum VNCP_EXCEPTION_STATUS
    {
        // ”√ªßµ«¬ºœ‡πÿ
        ES_SUCCESS = 0,
        ES_WAIT_RECONVERTING_RELAY_TASK_REQ = 1,    ///< √∂æŸ£¨√ª”– ’µΩª÷∏¥÷–◊™»ŒŒÒ«Î«Û
        ES_UNKNOWN_ERR,                             ///< √∂æŸ£¨Œ¥÷™µƒ¥ÌŒÛ
    };

    struct STATUS_CODE_DESC
    {
        VNCP_EXCEPTION_STATUS m_nStatusCode;
        std::string m_strDesc;
    };

    /// <summary>À˘”–µƒœ˚œ¢¿‡–Õ“ª¿¿±Ì</summary>
    enum  MESSAGE_TYPE
    {
        MT_UNKNOWN = 0,

        // øÕªß∂À∑¢Õ˘∑˛ŒÒ∆˜µƒœ˚œ¢
        MT_CLIENT_SET_PIXEL_FORMAT = 1,                 ///< √∂æŸ£¨…Ë÷√œÛÀÿ∏Ò Ωœ˚œ¢
        MT_CLIENT_READY = 2,            ///< √∂æŸ≥£¡ø,øÕªß∂À“—æ≠◊º±∏
        MT_CLIENT_FRAME_BUF_UPDATE_REQUEST = 3,         ///< √∂æŸ£¨«Î«Û÷°ª∫¥Ê∏¸–¬
        MT_CLIENT_KEY_EVENT = 4,                         ///< √∂æŸ£¨∞¥º¸ ¬º˛
        MT_CLIENT_POINTER_EVENT = 5,                     ///< √∂æŸ£¨ Û±Í(÷∏’Î) ¬º˛
        MT_CLIENT_CUT_TEXT = 6,                     ///< √∂æŸ£¨øÕªß∂ÀŒƒ±æºÙ«–
        MT_CLIENT_INIT_REQUEST = 8,                 ///< √∂æŸ£¨øÕªß∂À≥ı ºªØ«Î«Û
        MT_CLIENT_LOCK_DESKTOP = 9,     ///< √∂æŸ≥£¡ø,À¯∂®◊¿√Ê
        MT_CLIENT_LOGOFF = 10,     ///< √∂æŸ≥£¡ø,◊¢œ˙µ±«∞”√ªß
        MT_CLIENT_RESTART = 11,    ///< √∂æŸ≥£¡ø,÷ÿ∆Ùº∆À„ª˙
        MT_CLIENT_SHUTDOWN = 12,  ///< √∂æŸ≥£¡ø,πÿ±’º∆À„ª˙
        MT_CLIENT_OPEN_COMPUTER = 13,  ///< √∂æŸ≥£¡ø,¥Úø™Œ“µƒµÁƒ‘
        MT_CLIENT_OPEN_TASKMANAGER = 14,  ///< √∂æŸ≥£¡ø,¥Úø™»ŒŒÒπ‹¿Ì∆˜
        MT_CLIENT_OPEN_CMD = 15,  ///< √∂æŸ≥£¡ø,¥Úø™√¸¡Ó––¥∞ø⁄
        MT_CLIENT_VIDEO_FREEZE = 16,  ///< √∂æŸ≥£¡ø,∂≥Ω·◊¿√Êœ˚œ¢
        MT_CLIENT_SETTING_REQUEST = 17,  ///< √∂æŸ≥£¡ø,…Ë÷√«Î«Û£®÷˜“™ «ª≠√Ê÷ ¡ø∫Õ◊¿√Ê±⁄÷Ω£©
        MT_CLIENT_DESK_SIZE_CHANGED = 18,  ///< √∂æŸ≥£¡ø,◊Û√Ê≥ﬂ¥Á±ª≥…π¶∏ƒ±‰
        MT_CLIENT_SIMULATE_CTRL_ALT_DEL = 19,     ///< ƒ£ƒ‚Ctrl+Alt+Delº¸
        MT_CLIENT_CHANGE_RESOLUTION = 20,     ///< ∏ƒ±‰∑÷±Ê¬

        MT_HEARTBEAT = 95,           ///< –ƒÃ¯ ˝æ›∞¸
        MT_CLOSE_SESSION = 96,       ///< πÿ±’ª·ª∞¡¨Ω”
        MT_SYN = 97,                 ///< √∂æŸ≥£¡ø,Õ¨≤Ω«Î«Û£®ΩˆΩˆœﬁ”⁄UDPÕ®–≈£©
        MT_INVALID_SESSION = 98,     ///< √∂æŸ≥£¡ø,Œﬁ–ßµƒª·ª∞
        MT_READY = 99,               ///< √∂æŸ≥£¡ø,øÕªß∂ÀªÚ∑˛ŒÒ∆˜◊º±∏œ˚œ¢
        MT_HELLO = 100,                 ///< √∂æŸ£¨øÕªß∂À≥ı ºªØ«Î«Û

        // ∑˛ŒÒ∂À∑¢Õ˘øÕªß∂Àµƒœ˚œ¢
         
        MT_SERVER_CURSOR_SHAPE_CHANGED = 101,     ///< √∂æŸ£¨π‚±Í–Œ◊¥“—æ≠∏ƒ±‰
        MT_SERVER_DESKTOP_SIZE = 102,             ///< √∂æŸ£¨–¬µƒ◊¿√Ê≥ﬂ¥Á
        MT_SERVER_BELL = 103,                     ///< √∂æŸ£¨‘⁄øÕªß∂À…œœÏ¡Â
        MT_SERVER_CUT_TEXT = 104,                 ///< √∂æŸ£¨∑˛ŒÒ∆˜ºÙ«–Œƒ±æ
        MT_SERVER_INIT_REQUEST = 105,             ///< √∂æŸ£¨∑˛ŒÒ∆˜≥ı ºªØ«Î«Û
        MT_SERVER_CURSOR_POS_CHANGED = 106,         ///< √∂æŸ£¨π‚±ÍŒª÷√“—æ≠∏ƒ±‰
        MT_SERVER_READY = 107,                   ///< √∂æŸ≥£¡ø,∑˛ŒÒ∆˜“—æ≠◊º±∏
        MT_SERVER_ENABLE_REQUEST = 108,          ///< √∂æŸ≥£¡ø,Ω˚÷πªÚ πƒ‹”√ªß ‰»Î
        MT_SERVER_FRAME_DATA_FRAG = 109,         ///< √∂æŸ£¨∑÷∆¨÷° ˝æ›

        MT_REGIST_TO_UDP_HOLE_SERVER = 109,      ///< ◊¢≤·µΩUDP¥Ú∂¥∑˛ŒÒ∆˜

        MT_SERVER_DESKTOP_PIXEL_FORMAT = 111,     ///< √∂æŸ£¨∏¸–¬ª∫≥Â«¯œÒÀÿ∏Ò Ω
        MT_SERVER_OS_INFO = 112,                 ///< √∂æŸ£¨≤Ÿ◊˜œµÕ≥–≈œ¢
        MT_SERVER_SESSION_STATE = 113,             ///< √∂æŸ£¨∑µªÿøÿ÷∆∑˛ŒÒ∆˜µ±«∞Session◊¥Ã¨
        MT_MAX_MSG_COUNT,                         ///< √∂æŸ£¨œ˚œ¢¿‡–Õ◊Ó¥Û ˝, ∏√÷µŒª”⁄MESSAGE_TYPE◊Ó∫Û“ªŒª
    };



    /// <summary>–≠“È∞Ê±æ</summary>
    enum PROTOCOL_VER
    {
        PV_FIRST = 1,
    };

    /// Õ®–≈–≠“È±Í ∂√∂æŸ
///
    enum MAGIC_FLAG
    {
        HEADER_MAGIC = 0XFFFE, ///< √∂æŸ£¨ ”√”⁄ ∂±Õ®–≈ ˝æ›∞¸
    };

    /// <summary>±‡¬Î¿‡–Õ</summary>
    enum ENCODING_TYPE
    {
        ET_RAW = 0,         ///< √∂æŸ£¨ ‘≠ º±‡¬Î
        ET_COPYRECT = 1,    ///< √∂æŸ£¨ ∏¥÷∆æÿ–Œ±‡¬Î
        ET_RRE = 2,         ///< √∂æŸ£¨ ∂˛Œ¨––≥Ã±‡¬Î
        ET_HEXTILE = 5,     ///< √∂æŸ£¨ Hextile  «RRE±‡¬Îµƒ±‰÷÷
        ET_ZRLE = 16,       ///< √∂æŸ£¨ ZRLE(Zlib Run - Length Encoding),À¸Ω·∫œ¡Àzlib —πÀı£¨∆¨ºº ı°¢µ˜…´∞Â∫Õ‘À––≥§∂»±‡¬Î
    };

    // ±ÿ–Î”√œ¬√Ê’‚÷÷∂‘∆Î∑Ω Ω£¨≤ªƒ‹”√#pragma pack(1)
#pragma  pack (push,1)
    // --------------------------------------------------------------------------------
/// <summary>
/// @brief œ˚œ¢Õ∑
/// ∏√Õ®–≈÷–√ø∏ˆ ˝æ›∞¸∂ºΩ´–Ø¥¯“ª∏ˆœ˚œ¢Õ∑
/// </summary>
// --------------------------------------------------------------------------------
    typedef struct  _message_header
    {
        U16               nMagic;                ///< ∞¸Õ∑ ∂±¬Î
        U8                nType;                 ///< œ˚œ¢¿‡–ÕMESSAGE_TYPE
        U32               nSessionID;            ///< œ˚œ¢≤…”√µƒÕ®µ¿£® «øÿ÷∆∑˛ŒÒ∆˜∑÷≈‰µƒ…œœ¬ŒƒID,’˚∏ˆœµÕ≥Œ®“ª£©
        U32               nFlag : 8;             ///< ±Í÷æ£®µ⁄“ªŒª:≤Ÿ◊˜¿‡–Õ,@see OPERATOR_TYPE,µ⁄∂˛Œª≤Ÿ◊˜Ω·π˚,@see OPERATOR_RESULT£©
        U32               nPayloadLen:24;        ///< –Ø¥¯ ˝æ›µƒ≥§∂»(0xFFFFFF)
    }VNC_MESSAGE_HEADER;

    // --------------------------------------------------------------------------------
/// <summary>
/// @brief HELLOœ˚œ¢
/// µ±øÿ÷∆∂ÀªÚ±ªøÿ÷∆∂À¡¨Ω”…œ÷–◊™∑˛ŒÒ∆˜ªÚ∂‘∑Ωµƒ ˝æ›¥´ ‰∑˛ŒÒ∆˜∫Û£¨Ω´¡¢º¥∑¢ÀÕ∏√œ˚œ¢
/// ∏√œ˚œ¢–Ë“™”¶¥
/// </summary>
// --------------------------------------------------------------------------------
    typedef struct
    {
        VNC_MESSAGE_HEADER     objMsgHeader;
        U64                    nID;           ///< œ˚œ¢–Ú∫≈,”√”⁄÷ÿ¥´∫Õ¬“–Ú÷ÿ◊È
    }HELLO_MESSAGE_REQUEST;


    
    /// @brief øÕªß∂À≥ı ºªØ«Î«Û
    /// µ±Ω®¡¢¡¨Ω”∫Û£¨øÕªß∂À–Ë“™∑¢ÀÕ∏√œ˚œ¢
    /// øÕªß∂À->∑˛ŒÒ∂À
    typedef struct
    {
        U8             nSharedFlag;     ///< »Áπ˚‘ –Ì∑˛ŒÒ∆˜∆‰À˚øÕªßºÃ–¯¡¨Ω”£¨ƒ«√¥π≤œÌ±Í÷æ”¶∏√ «∑«¡„£®’Ê£©°£∑Ò‘Ú£¨∑˛ŒÒ∆˜Ω´∂œø™∆‰À˚øÕªßµƒ¡¨Ω”°£
        U8             nHideWallPage;   ///< “˛≤ÿ◊¿√Ê…Ë÷√(1£∫“˛≤ÿ,0:≤ª“˛≤ÿ)
        U8             nImageQuality;   ///< ÕºœÒ÷ ¡ø“™«Û,@see IMAGE_QUALITY_TYPE
    }CLIENT_INIT_REQUEST;

    /// @brief øÕªß∂À≥ı ºªØ«Î«Û
    /// ”√ªß∑¢ÀÕ¿¥µƒ…Ë÷√«Î«Û
    /// øÕªß∂À->∑˛ŒÒ∂À
    typedef struct
    {
        U8             nHideWallPage;   ///< “˛≤ÿ◊¿√Ê…Ë÷√(1£∫“˛≤ÿ,0:≤ª“˛≤ÿ)
        U8             nImageQuality;   ///< ÕºœÒ÷ ¡ø“™«Û,@see IMAGE_QUALITY_TYPE
    }CLIENT_SETTINGS_REQUEST;

    /// @brief œÒÀÿ∏Ò Ω∂®“Â
    typedef struct
    {
        U16 nBitsPerPixel;        ///< ∏ˆœÒÀÿ÷µ–Ë“™µƒŒª ˝°£’‚∏ˆ ˝◊÷±ÿ–Î¥Û”⁄µ»”⁄nColorDepth,ƒø«∞Œª√øœÛÀÿ±ÿ–Î «8,16 ªÚ32°™°™–°”⁄8 ŒªœÛÀÿ≤ª±ª÷ß≥÷
        U16 nColorDepth;          ///< ∂¯depth”√¿¥±Ì æœÒÀÿ÷µ÷–”–”√µƒŒª ˝
        U8 nBigEndianFlag;       ///< ∂‡◊÷Ω⁄œÛÀÿ±ªø¥◊ˆbig-endian,ƒ«√¥Big-endian ±Í÷æ∑«¡„
        U8 nTrueColourFlag;      ///< »Áπ˚’Ê≤ ±Í÷æ∑«¡„£¨ƒ«√¥◊Ó∫Û6 œÓπÊ∂®»Á∫Œ∞¥’’œÛÀÿ÷µ¿¥»∑∂®∫Ï°¢¬Ã°¢¿∂µƒ¡¡∂»

        U16 nRedMax;             ///<
        U16 nGreenMax;           ///<
        U16 nBlueMax;            ///<

        U16 nRedShift;            ///<
        U16 nGreenShift;          ///<
        U16 nBlueShift;           ///<
    }PIXEL_FORMAT;

    /// @brief ∑˛ŒÒ∂À≥ı ºªØ«Î«Û
    /// µ±Ω®¡¢¡¨Ω”∫Û£¨∑˛ŒÒ∂À–Ë“™∑¢ÀÕ∏√œ˚œ¢,÷˜“™∏Ê÷™øÕªß∂À∑˛ŒÒ∆˜µƒ÷°ª∫¥Ê£®◊¿√Ê∆¡ƒª£©µƒ∏ﬂ°¢øÌ°¢œÛÀÿ∏Ò Ω∫Õ◊¿√Êœ‡πÿµƒ√˚≥∆°£
    /// ∑˛ŒÒ∂À->øÕªß∂À
    typedef struct
    {
        U16            nFrameBufWidth;    ///< ÷°ª∫¥Ê£®◊¿√Ê∆¡ƒª£©µƒøÌ
        U16            nFrameBufHeight;   ///< ÷°ª∫¥Ê£®◊¿√Ê∆¡ƒª£©µƒ∏ﬂ
        PIXEL_FORMAT   objServerPixelFormat;   ///< œÛÀÿ∏Ò Ω
    }SERVER_INIT_REQUEST;
    

    /// @brief …Ë÷√œÛÀÿ∏Ò Ωœ˚œ¢
    /// øÕªß∂À∑¢ÀÕ∏√œ˚œ¢µΩ∑˛ŒÒ”√”⁄∏ƒ±‰ª∫≥Â«¯÷–œÒÀÿµƒ∏Ò Ω
    /// øÕªß∂À->∑˛ŒÒ∂À
    typedef struct
    {
        PIXEL_FORMAT   objPixelFormat;   ///< œÛÀÿ∏Ò Ω
    }PIXEL_FORMAT_SETTINGS_REQUEST;

    /// @brief …Ë÷√±‡¬Î∏Ò Ω
    /// …Ë÷√±‡¬Î∑Ω Ωø…“‘¿¥»∑∂®∑˛ŒÒ∆˜∑¢ÀÕœÛÀÿ ˝æ›µƒ¿‡–Õ
    /// øÕªß∂À->∑˛ŒÒ∂À
    typedef struct
    {
        U16            nEncodingNum;   ///< ±‡¬Î∏ˆ ˝£¨÷∏∂®¡ÀpEncodingµƒÃıƒø∏ˆ ˝
        I32            pEncoding[0];    ///< ±‡¬Î ˝æ›
    }ENCODING_SETTINGS_REQUEST;



    /// @brief «Î«Û÷°ª∫¥Ê∏¸–¬
/// Õ®÷™∑˛ŒÒ∆˜£¨øÕªß∂‘÷°ª∫≥Â«¯÷–µƒƒ≥∏ˆ«¯”Ú∏––À»§£¨’‚∏ˆ«¯”Ú”…x◊¯±Í°¢y◊¯±Í°¢øÌ∂»∫Õ∏ﬂ∂»º∏∏ˆ≤Œ ˝œﬁ∂®
/// øÕªß∂À->∑˛ŒÒ∂À
    typedef struct
    {
        U8             nInc;      ///< øÕªß∂™ ß¡ÀÀ¸À˘–Ë“™µƒ“ª∏ˆÃÿ∂®«¯”Úµƒƒ⁄»›£¨æÕ∑¢ÀÕ“ªÃıFramebufferUpdateRequestœ˚œ¢£¨∞—œ˚œ¢÷–µƒnInc(‘ˆ¡ø)÷√Œ™0£®false£©
        U16            nX;        ///< x◊¯±Í
        U16            nY;        ///< y◊¯±Í
        U16            nWidth;    ///< øÌ∂»
        U16            nHeight;   ///< ∏ﬂ∂»
    }FRAME_BUFFER_UPDATE_REQUEST;


    /// @brief ∞¥º¸ ¬º˛
    /// ƒ≥“ª∏ˆº¸µƒ∞¥œ¬”Î Õ∑≈,øÕªß∂ÀΩ´∑¢ÀÕ∏√œ˚œ¢µΩ∑˛ŒÒ∆˜
    /// øÕªß∂À->∑˛ŒÒ∂À
    typedef struct
    {
        U8             nDowndFlag;     ///< º¸∞¥œ¬±Í÷¬
        U32            nKey;           ///< ∂‘”⁄¥Û∂‡ ˝∆’Õ®º¸£¨°∞keysym°±∫ÕASCII¬Îµƒ÷µ «“ª÷¬µƒ£®«∞√Ê3∏ˆ◊÷Ω⁄Œ™0£¨◊Ó∫Û“ª∏ˆ◊÷Ω⁄Œ™ASCII¬Î£©
    }KEY_EVENT_REQUEST;

    /// @brief  Û±Í(÷∏’Î) ¬º˛
    /// ºÏ≤‚÷∏’Î“∆∂ØªÚ’ﬂƒ≥“ª∏ˆº¸µƒ∞¥œ¬ªÚ Õ∑≈,øÕªß∂ÀΩ´∑¢ÀÕ∏√œ˚œ¢µΩ∑˛ŒÒ∆˜
    /// ÷∏’Îƒø«∞‘⁄(x◊¯±Í°¢y ◊¯±Í£©£¨ Û±Í∞¥≈•µƒ∏˜º¸≤…”√1µΩ8Œª—⁄¬Î±Í ∂£¨
    /// 0 ±Ì æÀ…ø™£¨1 ±Ì æ∞¥œ¬°£ƒ√∆’Õ® Û±Í¿¥Àµ£¨»´¡„±Ì æ Û±Í“∆∂Ø£¨µ⁄1£¨2£¨3
    /// ∑÷±∂‘”¶◊Û°¢÷–°¢”“º¸°£∂‘”⁄ª¨¬÷ Û±Í¿¥Àµ£¨πˆ¬÷œÚ…œ∂‘”¶µ⁄4Œª£¨πˆ¬÷œÚœ¬∂‘
    /// ”¶µ⁄5Œª°£Õœ∂Ø≤Ÿ◊˜ «≤ª∂œµƒ∑¢ÀÕ◊Ûº¸∞¥œ¬µƒœ˚œ¢£¨≤¢±‰ªª Û±Íµƒ◊¯±Í°£
    /// øÕªß∂À->∑˛ŒÒ∂À
    typedef struct
    {
        U8             nButtonMask;     ///< º¸∞¥œ¬±Í÷¬
        U16            nX;        ///< x◊¯±Í
        U16            nY;        ///< y◊¯±Í
    }MT_POINTER_EVENT_REQUEST;


    /// @brief øÕªß∂ÀŒƒ±æºÙ«–
    /// øÕªß∂À”––¬µƒISO8859 - 1(Latin - 1) Œƒ±æ‘⁄À¸µƒºÙ«–ª∫¥Ê¿Ô£¨––µƒƒ©Œ≤Õ®π˝–¬––◊÷∑˚(÷µŒ™10)¿¥±Ì æ°£
    /// –Ë“™Œﬁªÿ≥µ£®÷µŒ™13)°£ƒø«∞ªπ√ª”–’“µΩ¥´ ‰∑«Latin - 1 ◊÷∑˚ºØµƒ∑Ω∑®,øÕªß∂ÀΩ´∑¢ÀÕ∏√œ˚œ¢µΩ∑˛ŒÒ∆˜
    /// øÕªß∂À<->∑˛ŒÒ∂À
    typedef struct
    {
        U32            nLength;     ///< Œƒ±æµƒ≥§∂»
        U8             pText[0];       ///< Œƒ±æ ˝æ›
    }CUT_TEXT_EVENT;

    using STRING_AGRS = CUT_TEXT_EVENT;

    typedef struct
    {
        U16            nX;        ///< x◊¯±Í
        U16            nY;        ///< y◊¯±Í
        U16            nWidth;    ///< øÌ∂»
        U16            nHeight;   ///< ∏ﬂ∂»
        U8             nEncodingType;   ///< ±‡¬Î¿‡–Õ
        U8             pData[0];    ///<  ˝æ›
    }RECT_INFO;


    /// @brief ÷°ª∫¥Ê∏¸–¬
///
/// ∑˛ŒÒ∂À->øÕªß∂À
    typedef struct
    {
        U16            nRects;     ///< æÿ–Œ«¯”Úµƒ∏ˆ ˝
        U8             pRectData[0]; ///<  ˝æ›
    }FRAME_BUFFER_UPDATE_INFO;


    /// @brief …Ë÷√—’…´√Ê∞Âµƒ÷µ
    typedef struct
    {
        U16            nRed;      ///< ∫Ï…´
        U16            nGreen;    ///< ¬Ã…´
        U16            nBlue;     ///< ¿∂…´
    }COLOUR_ENTRY;


    /// @brief …Ë÷√—’…´√Ê∞Âµƒ÷µ
    ///
    /// ∑˛ŒÒ∂À->øÕªß∂À
    typedef struct
    {
        U16            nFirstColour;     ///< æÿ–Œ«¯”Úµƒ∏ˆ ˝
        U16            nColours;         ///< æÿ–Œ«¯”Úµƒ∏ˆ ˝
        COLOUR_ENTRY   pColourData[0];   ///< —’…´ ˝æ›,∞¸∫¨”–nColours∏ˆ—’…´ÃıƒøCOLOUR_ENTRY
    }COLOR_MAP_ENTRIES_SETINGS_REQUEST;


    /// @brief ∂≥Ω· ”∆µ«Î«Û
///
/// øÕªß∂À->∑˛ŒÒ∂À
    typedef struct
    {
        U8             nFreeze;   ///< 1:∂≥Ω·£¨0£∫Ω‚∂≥
    }VIDEO_FREEZE_REQUEST;

    /// @brief ◊º±∏æÕ–˜Õ®÷™«Î«Û
/// øÕªß∂ÀªÚ∑˛ŒÒ∂À◊º±∏æÕ–˜∫Û∂º–Ë“™œÚ∂‘∑Ω∑¢ÀÕœ˚œ¢£¨≤¢«“ ’µΩ∫Û“ª∂®“™ªÿ∏¥
/// ∑˛ŒÒ∂À<->øÕªß∂À
    typedef struct
    {
        U8             bResponse;   ///< 1:∂≥Ω·£¨0£∫Ω‚∂≥
    }READY_REQUEST;


    /// @brief ∂≥Ω· ”∆µ«Î«Û
///
/// øÕªß∂À->∑˛ŒÒ∂À
    typedef struct
    {
        U8             bEnable;   ///< 1:‘ –Ì£¨0£∫Ω˚÷π
    }ENABLE_INPUT_REQUEST;


    /// @brief ◊¢≤·µΩ¥Ú∂¥∑˛ŒÒ∆˜«Î«Û
    typedef struct
    {
        U64  nDeviceID;          ///< …Ë±∏ID
    }REGIST_TO_HOLE_SERVER_REQUEST;

    /// @brief …Ë÷√±‡¬Î∏Ò Ω
/// …Ë÷√±‡¬Î∑Ω Ωø…“‘¿¥»∑∂®∑˛ŒÒ∆˜∑¢ÀÕœÛÀÿ ˝æ›µƒ¿‡–Õ
/// øÕªß∂À->∑˛ŒÒ∂À
    typedef struct
    {
        U8            pData[0];    ///< ±‡¬Î ˝æ›
    }SYN_FRAME_BUFFER_REQUEST;

    /// @brief ◊¯√Ê–≈œ¢
    ///
    /// ≤…ºØøÕªß∂À<-∑˛ŒÒ∂À
    typedef struct
    {
        U8             szDesktop[64];    ///< ◊¿√Ê√˚≥∆
        U8             szUser[64];       ///< ”√ªß√˚≥∆
    }USER_INFO;

    /// @brief ◊¿√Ê◊¯±Í–≈œ¢
    ///
    /// ≤…ºØøÕªß∂À<-∑˛ŒÒ∂À
    typedef struct
    {
        U16            nLeft;       ///< ◊Û
        U16            nTop;        ///< …œ
        U16            nRight;      ///< ”“
        U16            nBottom;     ///< œ¬
    }DESKTOP_COORDS_INFO;

    /// @brief ◊¯±Í–≈œ¢
///
/// ≤…ºØøÕªß∂À<-∑˛ŒÒ∂À
    typedef struct
    {
        U16            nX;         ///<
        U16            nY;         ///<
    }POINT_INFO;


    /// @brief ◊¿√Ê◊¯±Í–≈œ¢
///
/// ≤…ºØøÕªß∂À<-∑˛ŒÒ∂À
    typedef struct
    {
        U16            nX;         ///< X◊¯±Í
        U16            nY;         ///< Y◊¯±Í
        U16            nWidth;     ///< øÌ∂»
        U16            nHeight;    ///< ∏ﬂ∂»
        U8             pData[0];    ///< ±‡¬Î ˝æ›
    }CURSOR_SHAPE_INFO;

/// @brief ∑÷∆¨÷° ˝æ›
///
/// »Áπ˚“ª∏ˆ÷°Ã´¥Û£¨“‘÷¡”⁄¥Û”⁄¡À ˝æ›∞¸µƒ◊Ó¥Û≥§∂»£¨ƒ«√¥æÕ–Ë“™∑÷∆¨
/// ≤…ºØøÕªß∂À<-∑˛ŒÒ∂À
    typedef struct
    {
        U32            nTotal;      ///< ’˚∏ˆ÷° ˝æ›µƒ◊‹≥§∂»
        U32            nPayloadLen; ///< ‘ÿ∫… ˝æ›µƒ≥§∂»
        U32            nOffset;     ///< µ±«∞∑÷∆¨ ˝æ›‘⁄’˚∏ˆ÷° ˝æ›¡˜÷–µƒ∆´“∆
        U8             pData[0];    ///<  ˝æ›
    }FRAME_DATA_FRAG;


    /// @brief ◊¿√Ê–≈œ¢
///
/// ≤…ºØøÕªß∂À<-∑˛ŒÒ∂À
    typedef struct
    {
        U32            nWidth;     ///< øÌ∂»
        U32            nHeight;    ///< ∏ﬂ∂»
    }DESKTOP_SIZE;

    // --------------------------------------------------------------------------------
/// <summary>
/// @brief ª·ª∞◊¥Ã¨œ˚œ¢
/// ±ªøÿ÷∆∂ÀÕ®π˝∏√œ˚œ¢Õ®÷™øÿ÷∆∂À◊‘…Ìµ±«∞µƒ◊¥Ã¨
/// </summary>
// --------------------------------------------------------------------------------
    typedef struct
    {
        VNC_MESSAGE_HEADER     objMsgHeader;
        U16                    nState;           ///< ◊¥Ã¨
    }SESSION_STATE_REQUEST;

#pragma pack(pop)

    // --------------------------------------------------------------------------------
    /// <summary>
    /// ±‡¬ÎÃıƒø–≈œ¢
    /// </summary>
    // --------------------------------------------------------------------------------
    class CDevMode :public CSerializeEntry
    {
    public:
        CDevMode(const U32 nWidth = 0, const U32 nHeight = 0, const U32 nBitsPerPel = 32);
        ~CDevMode(){}
        CDevMode(const CDevMode& refSrc);

        ENABLE_SERIALIZE(CDevMode)

    public:
        bool operator==(const CDevMode& refSrc);

    public:
        virtual void Serialize(CJSONSerializer& refJsonSerializer) const override;

        virtual void DeSerialize(CJSONSerializer& refJsonSerializer) override;
    public:
        DECLARE_MEMBER_AND_METHOD_V11(U32, m_nWidth, Width,0)             ///<  øÌ
        DECLARE_MEMBER_AND_METHOD_V11(U32, m_nHeight, Height, 0)          ///<  ∏ﬂ
        DECLARE_MEMBER_AND_METHOD_V11(U32, m_nBitsPerPel, BitsPerPel, 0)  ///<  Œª…´
    };

    using CDevModePtr = std::shared_ptr<CDevMode>;
    using CDevModeArray = CSharedDynamicJsonVector<CDevMode>;

    // --------------------------------------------------------------------------------
    /// <summary>
    /// ≤Ÿ◊˜œµÕ≥–≈œ¢
    /// </summary>
    // --------------------------------------------------------------------------------
    class COSInfo :public CSerializeEntry
    {
    public:
        COSInfo() {}
        ~COSInfo() {}

        ENABLE_SERIALIZE(COSInfo)

    public:
        CDevModeArray& GetModes() { return  m_arrDevModes; }

    public:
        // ‘ˆº”“ª∏ˆ–¬µƒœ‘ æƒ£ Ω
        void AppendDevMode(const U32 nWidth = 0, const U32 nHeight = 0, const U32 nBitsPerPel = 0);

    public:
        virtual void Serialize(CJSONSerializer& refJsonSerializer) const override;

        virtual void DeSerialize(CJSONSerializer& refJsonSerializer) override;

    private:
        DECLARE_MEMBER_AND_METHOD_V11(U32, m_nOS, OS, 0)  ///<  ≤Ÿ◊˜œµÕ≥¿‡–Õ£¨@see CEnvironment::OS_VERSION
        CDevModeArray              m_arrDevModes;   ///< œ‘ æƒ£ Ω¡–±Ì
        DECLARE_MEMBER_AND_METHOD(CDevMode,m_objCurDevMode, CurDevMode); ///< µ±«∞µƒœ‘ æƒ£ Ω
        DECLARE_MEMBER_AND_METHOD_V11(U32, m_nVersion, Version, 0);  ///<  ±ªøÿ÷∆∂À»Ìº˛∞Ê±æ∫≈
    };


    extern std::string GetStatusCodeDesc(VNCP_EXCEPTION_STATUS nStatusCode);

}



#endif /* VNCProtocol_h */

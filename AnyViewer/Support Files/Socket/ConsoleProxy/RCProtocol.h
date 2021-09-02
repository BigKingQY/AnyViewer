//
//  RCProtocol.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//

#pragma once
#pragma warning(disable : 4200)
#include "MessageHeadDef.h"
//#include "TreeManagerBase.h"
//#include "MiscellaneousFunctions.h"
#include <memory>

#ifndef WIN32
#include <mutex>
#endif

namespace RCP
{
#define ACCOUNT_LENGTH_MAX 32
#define PASSWORD_LENGTH_MAX 32
#define FILE_NAME_LEN_MAX 255
#define TRANSACTION_STATUS_MSG_LEN_MAX 200   ///<  ¬ŒÒ◊¥Ã¨œ˚œ¢µƒ◊Ó¥Û≥§∂»

    /// ¥ÌŒÛ¿‡–Õ√∂æŸ
    ///
    /// ŒÂ∞Ÿ“‘«∞µƒ¥ÌŒÛ∂®Œª—œ÷ÿ¥ÌŒÛ£¨¡¨Ω”Ω´∂œø™
    enum EXCEPTION_STATUS
    {
        // ”√ªßµ«¬ºœ‡πÿ
        ES_SUCCESS = 0,
        ES_INVALID_ACCOUNT = 1,     ///< √∂æŸ£¨’À∫≈≤ª¥Ê‘⁄
        ES_CREATE_ACCOUNT_FAILED = 2,  ///<¥¥Ω®’À∫≈ ß∞‹
        ES_INVALID_NAME = 3,         ///< √∂æŸ£¨Œﬁ–ßµƒ√˚≥∆
        ES_EXIST_NAME = 4,             ///< √∂æŸ£¨√˚≥∆“—æ≠¥Ê‘⁄
        ES_INVALID_PASSWD = 5,         ///< √∂æŸ£¨√‹¬Î¥ÌŒÛ
        ES_REPEAT_LOGIN = 6,         ///< √∂æŸ£¨÷ÿ∏¥µ«¬º
        ES_TIME_OUT = 7,             ///< √∂æŸ£¨≥¨ ±
        ES_NOT_LOGGED_IN = 8,        ///< √∂æŸ£¨Œ¥µ«¬º
        ES_OPEN_FILE_UNSUCCESSFULLY = 12,      ///< √∂æŸ£¨¥Úø™Œƒº˛ ß∞‹
        ES_SERVICE_ERR = 13,          ///<∑˛ŒÒ∆˜¥ÌŒÛ
        ES_INVALID_SN = 14,           ///<Œﬁ–ßº§ªÓ¬Î
        ES_REACH_AUTHORIZATION_LIMIT = 15,          ///<  ⁄»® ˝“—æ≠¬˙,Œﬁ∑®‘⁄º§ªÓ
        ES_SEND_FAILED = 16,
        ES_DB_ERROR = 17,        ///< √∂æŸ£¨ ˝æ›ø‚¥ÌŒÛ
        ES_CREATE_CUSTORM_UNSUCCESSFULLY = 23,  ///< √∂æŸ£¨¥¥Ω®øÕªß∂À–≈œ¢ ß∞‹
        ES_NOT_FOUND_CUSTORM = 24,              ///< √∂æŸ£¨√ª”–’“µΩøÕªß–≈œ¢
        ES_CUSTORM_OFFLINE = 25,                ///< √∂æŸ£¨øÕªß≤ª‘⁄œﬂ
        ES_PROHIBITION_CONTROL = 26,            ///< √∂æŸ£¨…Ë±∏Ω˚÷π±ªøÿ÷∆
        ES_NEED_PWD_AUTH = 27,                  ///< √∂æŸ£¨–Ë“™√‹¬Î»œ÷§
        ES_SELECT_AUTH_METHOD = 28,             ///< √∂æŸ£¨—°‘Ò»œ÷§∑Ω Ω
        ES_REJECT_CTRL_REQUEST = 29,            ///< √∂æŸ£¨æ‹æ¯øÿ÷∆«Î«Û
        ES_SRC_DEVICE_FROZEN = 30,              ///< √∂æŸ£¨‘¥…Ë±∏±ª∂≥Ω·
        ES_DST_DEVICE_FROZEN = 31,              ///< √∂æŸ£¨ƒø±Í…Ë±∏±ª∂≥Ω·
        ES_READY_CONN_UNSUCCESSFULLY = 32,      ///< √∂æŸ£¨◊º±∏¡¨Ω” ß∞‹
        ES_INVALID_CTRL_SESSION_CONTEXT = 33,   ///< √∂æŸ£¨Œﬁ–ßµƒøÿ÷∆ª·ª∞…œœ¬Œƒ
        ES_INVALID_RELAING_SERVER = 34,         ///< √∂æŸ£¨’“≤ªµΩ”––ßµƒ÷–◊™∑˛ŒÒ∆˜
        ES_SEND_READY_CONN_REQUEST_UNSUCCESSFULLY = 35,        ///< √∂æŸ£¨∑¢ÀÕ◊º±∏¡¨Ω”«Î«ÛµΩ±ªøÿ÷∆∂À ß∞‹
        ES_TOO_LOW_APP_VERISON = 36,            ///< √∂æŸ£¨Ã´µÕµƒAPP∞Ê±æ«Î…˝º∂
        ES_UNKNOWN_ERR,                         ///< √∂æŸ£¨Œ¥÷™µƒ¥ÌŒÛ
    };

    struct STATUS_CODE_DESC
    {
        EXCEPTION_STATUS m_nStatusCode;
        std::string m_strDesc;
    };

    /// <summary>À˘”–µƒœ˚œ¢¿‡–Õ“ª¿¿±Ì</summary>
    enum  MESSAGE_TYPE
    {
        MT_UNKNOWN = 0,
        MT_AES_ENCIPHER_KEY = 1001,
        MT_REGIST_RELAY_SERVER = 1002,              ///< ÷–◊™∑˛ŒÒ∆˜◊¢≤·œ˚œ¢
        MT_UPDATE_RELAY_SERVER_STATUS = 1003,       ///< ∏¸–¬∑˛ŒÒ∆˜◊¢≤·œ˚œ¢
        MT_REGIST_CLIENT = 1004,                    ///< øÕªß∂À◊¢≤·œ˚œ¢
        MT_UPDATE_CLIENT_INFO = 1005,               ///< ∏¸–¬øÕªß∂À–≈œ¢
        MT_CONNECT = 1006,                          ///< ¡¨Ω”«Î«Û
        MT_AUTHENTICATE = 1007,                     ///< »œ÷§«Î«Û«Î«Û
        MT_READY_CONN_REQUEST = 1008,               ///< ◊º±∏¡¨Ω”«Î«Û(∑˛ŒÒ∆˜∑¢Õ˘øÿ÷∆∂À∫Õ±ªøÿ÷∆∂À)
        MT_MSG = 1009,                              ///< ∑˛ŒÒ∆˜∑¢Õ˘øÕªß∂Àµƒœ˚œ¢£¨≤…”√Õ®”√«Î«Û
        MT_CHANGE_COMMUNICATION_MODE = 1010,       ///< ∏ƒ±‰Õ®–≈ƒ£ Ω
        MT_NEW_RELAING_TASK = 1011,                ///< –¬µƒ◊™∑¢»ŒŒÒ
        MT_CLOSE_SESSION = 1012,                   ///< πÿ±’ª·ª∞¡¨Ω”
        MT_VNC_CONNECT = 1013,                     ///< ∑¢∆VNC¡¨Ω”(∑˛ŒÒ∆˜∑¢ÀÕ∏¯øÿ÷∆∂À∫Õ±ªøÿ÷∆∂À)
        MT_PEER_ADDR_INFO = 1014,                  ///< ∂‘∂Àµÿ÷∑–≈œ¢
        MT_HEARTBEAT = 1018,                       ///< –ƒÃ¯ ˝æ›
        MT_CONNECTION_STATUS_CHANGED = 1019,       ///< RCøÕªß∂À”Îøÿ÷∆∑˛ŒÒ∆˜µƒ¡¨Ω”◊¥Ã¨∏ƒ±‰Õ®÷™
        MT_SHOW_CONTROLLED_TOOLBAR = 1020,         ///< œ‘ æøÿ÷∆π§æﬂÃı
        MT_CLOSE_CONTROLLED_TOOLBAR = 1021,        ///< πÿ±’øÿ÷∆π§æﬂÃı

        MT_CLOSE_ALL_SESSION = 1023,               ///< πÿ±’À˘”–µƒª·ª∞¡¨Ω”
        MT_PRE_CLOSE_SESSION = 1024,               ///< ◊º±∏πÿ±’ª·ª∞¡¨Ω”
        MT_ABORT_CONNECTION = 1025,                ///< ÷’÷π¡¨Ω”
        MT_PUBLIC_IP_REQUEST = 1026,               ///< ∑˛ŒÒ∆˜∑¢ÀÕ¿¥µƒπ´Õ¯IPÕ®÷™
        MT_RECONVERT_RELAY_TASK_REQUEST = 1027,    ///< µ±«∞’˝‘⁄Ω¯––µƒ÷–◊™∑˛ŒÒ

        MT_MAX_MSG_COUNT,                           ///< √∂æŸ£¨œ˚œ¢¿‡–Õ◊Ó¥Û ˝, ∏√÷µŒª”⁄MESSAGE_TYPE◊Ó∫Û“ªŒª
    };


    // --------------------------------------------------------------------------------
    /// <summary>
    /// ±æµÿœ˚œ¢¿‡–Õ£¨”√”⁄±æµÿøÕªß∂À∫Õ±æµÿ∑˛ŒÒ÷Æº‰µƒÕ®–≈
    /// </summary>
    // --------------------------------------------------------------------------------
    enum LOCAL_MESSAGE_TYPE
    {
        LMT_START_CLIENT = 2001,                    ///< ∆Ù∂ØøÕªß∂À
        LMT_REGIST_CLIENT = 2002,                   ///< øÕªß∂À◊¢≤·œ˚œ¢
        LMT_SHOW_CLIENT = 2003,                     ///< œ‘ æøÕªß∂À
        LMT_QUERY_CLIENT = 2004,                    ///< ≤È—ØøÕªß∂Àµƒ◊¥Ã¨
        LMT_LOGOFF_CLIENT = 2005,                   ///< ◊¢œ˙øÕªß∂À
        LMT_SIMULATE_CTRL_ALT_DEL = 2006,           ///< ƒ£ƒ‚Ctrl+Alt+Delº¸
        LMT_SESSION_STATE = 2007,                   ///< ±æµÿ∑˛ŒÒ∆˜∑¢ÀÕµ±«∞ª·ª∞◊¥Ã¨µΩøÕªß∂À(±æµÿ∑˛ŒÒ∆˜->øÕªß∂À)
        LMT_QUERY_SESSION_STATE = 2008,             ///< ≤È—Øµ±«∞ª·ª∞◊¥Ã¨µΩøÕªß∂À(øÕªß∂À->±æµÿ∑˛ŒÒ∆˜)
        LMT_RELOAD_PRJ_SETTINGS = 2009,             ///< ÷ÿ–¬º”‘ÿπ§≥Ã…Ë÷√
        LMT_DELAYED_START_CLIENT = 2010,            ///< —” ±∆Ù∂ØøÕªß∂À
    };

    /// <summary>‘∂≥Ãøÿ÷∆øÕªß∂À¿‡–Õ√∂æŸ∂®“Â</summary>
    enum  RC_CLIENT_TYPE
    {
        RCT_UNKNOWN = 0,               ///< √∂æŸ≥£¡ø£¨Œ¥÷™
        RCT_RELAY_SERVER = 1,          ///< √∂æŸ≥£¡ø£¨÷–◊™∑˛ŒÒ∆˜
        RCT_NORMAL_CLIENT = 2,         ///< √∂æŸ≥£¡ø£¨∆’Õ®øÕªß∂À
        RCT_CONTROLLER = 3,            ///< √∂æŸ≥£¡ø£¨øÿ÷∆∂À
        RCT_CONTROLLED = 4,            ///< √∂æŸ≥£¡ø£¨±ªøÿ÷∆∂À
    };


    /// <summary>øÕªß∂À◊¥Ã¨¿‡–Õ√∂æŸ∂®“Â</summary>
    enum  RC_CLIENT_STATUS
    {
        CCS_OFF_LINE = 0,                 ///< √∂æŸ£¨¿Îœﬂ
        CCS_ON_LINE = 0X01,               ///< √∂æŸ£¨‘⁄œﬂ
        CCS_BUSY = 0X02,                  ///< √∂æŸ£¨√¶¬µ
        CCS_HIDE = 0X03,                  ///< √∂æŸ£¨“˛≤ÿ
    };

    
    /// <summary>»œ÷§ƒ£ Ω¿‡–Õ√∂æŸ</summary>
    enum AUTHENTICATION_METHOD
    {
        AM_AUTO   = 0,        ///< √∂æŸ≥£¡ø,◊‘∂Ø
        AM_MANUAL = 1,        ///< √∂æŸ≥£¡ø,“™«Û ÷π§»œ÷§
    };

    /// <summary>Õ®–≈∑Ω Ω</summary>
    enum COMMUNICATION_MODE
    {
        ST_UNKNOWN = 0,      ///< √∂æŸ≥£¡ø,Œﬁ–ßµƒ¡¨Ω”¿‡–Õ
        ST_SRC_TO_DST = 1,   ///< √∂æŸ≥£¡ø,‘¥œÚƒø±Í∑¢∆µƒ÷±¡¨
        ST_DST_TO_SRC = 2,   ///< √∂æŸ≥£¡ø,ƒø±ÍœÚ‘¥∑¢∆µƒ÷±¡¨
        ST_HOLE = 3,         ///< √∂æŸ≥£¡ø,¥Ú∂¥÷±¡¨(‘¥œÚƒø±Í∑¢∆µƒ÷±¡¨)
        ST_RELAY = 4,        ///< √∂æŸ≥£¡ø,÷–◊™
    };

    /// <summary>–≠“È∞Ê±æ</summary>
    enum PROTOCOL_VER
    {
        PV_FIRST = 1,
    };

    /// <summary>æ‹æ¯«Î«Ûµƒ∑Ω Ω</summary>
    enum REJECT_MODE
    {
        RM_UNKNOWN = 0,     ///< √∂æŸ≥£¡ø,Œ¥÷™
        RM_ACTIVE  = 1,     ///< √∂æŸ≥£¡ø,÷˜∂Øæ‹æ¯
        RM_TIMEOUT = 2,     ///< √∂æŸ≥£¡ø,≥¨ ±
    };

    /// <summary>√‹¬Î¿‡–Õ</summary>
    enum PWD_TYPE
    {
        PT_UNKNOWN = 0,     ///< √∂æŸ≥£¡ø,Œ¥÷™
        PT_TMP_PWD = 1,     ///< √∂æŸ≥£¡ø,¡Ÿ ±µƒ
        PT_FIX_PWD = 2,     ///< √∂æŸ≥£¡ø,πÃ∂®µƒ√‹¬Î
    };

    // ±ÿ–Î”√œ¬√Ê’‚÷÷∂‘∆Î∑Ω Ω£¨≤ªƒ‹”√#pragma pack(1)
#pragma  pack (push,1)

    // -------------------------¡Ω∏ˆÕ®”√µƒœ˚œ¢∂®“Â£¨≤ª–Ø¥¯»Œ∫Œ ˝æ›---------------



    /// @brief ¡¨Ω””¶¥
    typedef struct  _common_response_ex
    {
        MESSAGE_HEADER objMsgHeader;
        U32                nStatusCode1;   ///< ∑µªÿœÏ”¶Ω·π˚◊¥Ã¨¬Î1
        U32                nStatusCode2;   ///< ∑µªÿœÏ”¶Ω·π˚◊¥Ã¨¬Î2
    }COMMON_RESPONSE_EX;



    /// <summary>◊™∑¢Õ∑</summary>
    typedef struct _transpond_header
    {
        MESSAGE_HEADER objMsgHeader;
        U32 nSrcAccount;
        U32 nDstAccount;
    }TRANSPOND_HEADER;


    typedef struct  _common_transpond_response
    {
        TRANSPOND_HEADER objTranspondHeader;
        U32                     nStatusCode;   ///< ∑µªÿœÏ”¶Ω·π˚◊¥Ã¨¬Î
    }COMMON_TRANSPOND_RESPONSE;


    /// @brief ¡¨Ω”◊º±∏”¶¥ ˝æ›∞¸
    /// øÕªß∂À->∑˛ŒÒ∆˜
    typedef struct
    {
        MESSAGE_HEADER objMsgHeader;
        U64            nID;              ///< ª·ª∞…œœ¬ŒƒID±Í ∂
        I8             nRoleType;    ///< ∂Àµ„¿‡–Õ(øÿ÷∆∂ÀªÚ±ªøÿ÷∆∂À,@see RC_CLIENT_TYPE)
    }READY_CONN_RESPONSE;

    /// @brief –¬µƒ÷–◊™»ŒŒÒ”¶¥ ˝æ›∞¸
/// øÕªß∂À->∑˛ŒÒ∆˜
    typedef struct
    {
        MESSAGE_HEADER objMsgHeader;
        U32            nSessionID;     ///< ª·ª∞…œœ¬ŒƒID±Í ∂
        U32            nRelaySvrID;    ///< ÷–◊™∑˛ŒÒ∆˜ID
    }NEW_RELAY_TASK_RESPONSE;

    /*-------------------------AESº”√‹KEYœ˚œ¢∏Ò Ω∂®“Â---------------------------------------------*/

    /// @brief ≤È—Øµ«¬º◊¥Ã¨œÏ”¶
    ///
    /// ¥´ ‰∑ΩœÚ£∫øÕªß∂À-->∑˛ŒÒ∆˜\n
    /// œÏ”¶£∫œ˚œ¢AES_ENCIPHER_KEY_REQUESTµƒœÏ”¶œ˚œ¢∏Ò Ω «AES_ENCIPHER_KEY_RESPONSE
    ///            À¸√« «œ‡Õ¨
    typedef struct  _aes_encipher_key
    {
        MESSAGE_HEADER objMsgHeader;
        char  szKeyData[48];                                  ///< Ω‚√‹∫Ûµƒ ˝æ›÷ª”–«∞√Ê16∏ˆ”––ß£¨ £œ¬»´ «ÃÓ≥‰ ˝æ›
    }AES_ENCIPHER_KEY_REQUEST, AES_ENCIPHER_KEY_RESPONSE;

    



    /// @brief ∂Àµ„µÿ÷∑–≈œ¢
/// ∑˛ŒÒ∆˜->øÕªß∂À
    typedef struct
    {
        MESSAGE_HEADER objMsgHeader;
        U32            nSessionID;     ///< ª·ª∞…œœ¬ŒƒID±Í ∂
        U32            nIP;      ///< IPµÿ÷∑
        U16            nPort;    ///< ∂Àø⁄
    }PEER_ADDR_INFO;

    /// @brief ∏ƒ±‰Õ®–≈∑Ω Ω«Î«Û
    typedef struct
    {
        MESSAGE_HEADER objMsgHeader;
        U32            nSessionID;     ///< ª·ª∞…œœ¬ŒƒID±Í ∂
        U8             nMode;          ///< –¬µƒÕ®–≈∑Ω Ω
    }CHANGE_COMM_MODE_REQUEST;
#pragma pack(pop)

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// ±‡∫≈”Î√˚≥∆”≥…‰¿‡∂®“Â
/// </summary>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class CRelaySererStatus :public CSerializeEntry
    {
    public:
        CRelaySererStatus(const double nCPUOccupy = 0, const double nMemTotal = 0, const double nMemFree = 0)
            :m_nCPUOccupy(nCPUOccupy)
            , m_nMemTotal(nMemTotal)
            , m_nMemFree(nMemFree)
        {
        }
        ~CRelaySererStatus() {}

        ENABLE_SERIALIZE(CRelaySererStatus)
    public:
        virtual void Serialize(CJSONSerializer& refJsonSerializer) const override;

        virtual void DeSerialize(CJSONSerializer& refJsonSerializer) override;

    private:
        DECLARE_MEMBER_AND_METHOD_V11(double, m_nCPUOccupy, CPUOccupy, 0.0)                             ///< CPU π”√¬
        DECLARE_MEMBER_AND_METHOD_V11(double, m_nMemTotal, MemTotal, 0.0)          ///< ◊‹ƒ⁄¥Ê
        DECLARE_MEMBER_AND_METHOD_V11(double, m_nMemFree, MemFree, 0.0)            ///< ø’œ–ƒ⁄¥Ê
        DECLARE_MEMBER_AND_METHOD_V11(int, m_nFreeBand, FreeBand, 0)               ///< ø’œ–¥¯øÌ
        DECLARE_MEMBER_AND_METHOD_V11(int, m_nSessions, Sessions, 0)               ///< µ±«∞÷–◊™µƒª·ª∞
    };


    class CRelaySererInfo :public CSerializeEntry
    {
    public:
        CRelaySererInfo(const char* lpName = nullptr,const double nCPUOccupy = 0, const double nMemTotal = 0, const double nMemFree = 0)
            :m_strName(lpName)
            , m_objStatus(nCPUOccupy, nMemTotal, nMemFree)
        {
        }
        ~CRelaySererInfo() {}

        ENABLE_SERIALIZE(CRelaySererInfo)

    public:
        CRelaySererStatus& GetStatus() { return m_objStatus; }

    public:
        virtual void Serialize(CJSONSerializer& refJsonSerializer) const override;

        virtual void DeSerialize(CJSONSerializer& refJsonSerializer) override;

    private:
        DECLARE_MEMBER_AND_METHOD(std::string, m_strName, Name)                          ///< √˚≥∆
        DECLARE_MEMBER_AND_METHOD(std::string, m_strRegion, Region)                      ///< ∑˛ŒÒ∆˜À˘‘⁄µƒµÿ”Ú(÷–Œƒ)
        DECLARE_MEMBER_AND_METHOD(U16, m_nPort, Port)                                    ///< ÷–◊™∑˛ŒÒ∂Àø⁄
        DECLARE_MEMBER_AND_METHOD(std::string, m_strGUID, GUID)                          ///< »´«ÚŒ®“ª±‡∫≈,»Áπ˚√ª”–…Ë÷√£¨Ω´≤…”√CPU–Ú∫≈
        CRelaySererStatus          m_objStatus;                ///< ◊¥Ã¨
    };


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// øÕªß∂À–≈œ¢
/// µ±◊¢≤·µƒ ±∫Ú∑¢ÀÕ∏√œ˚œ¢£¨∑˛ŒÒ∆˜∏˘æ›∏√œ˚œ¢¿¥º∆À„…Ë±∏id
/// </summary>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class CRCClientInfo :public CSerializeEntry
    {
    public:
        CRCClientInfo()
        {
        }
        ~CRCClientInfo() {}


        /// <summary>»œ÷§∑Ω Ω√∂æŸ¿‡–Õ</summary>
        enum  AUTHENTICATION_TYPE
        {
            AT_MANUAL     = 1,       ///< √∂æŸ≥£¡ø, »Àπ§»œ÷§
            AT_TMP_PWD    = 2,       ///< √∂æŸ≥£¡ø, ¡Ÿ ±ø⁄¡Ó
            AT_FIX_PWD    = 4,       ///< √∂æŸ≥£¡ø, πÃ∂®ø⁄¡Ó
        };

        ENABLE_SERIALIZE(CRCClientInfo)
    public:
        // ¥¥Ω®“ª∏ˆkey
        std::string CreateKey();

    public:
        virtual void Serialize(CJSONSerializer& refJsonSerializer) const override;

        virtual void DeSerialize(CJSONSerializer& refJsonSerializer) override;

    private:
        DECLARE_MEMBER_AND_METHOD(std::string, m_strIPs, IPs)              ///< ipµÿ÷∑£¨∂‡∏ˆµÿ÷∑”√∑÷∫≈∑÷∏Ó
        DECLARE_MEMBER_AND_METHOD(std::string, m_strIPMasks, IPMasks)      ///< ipµÿ÷∑∂‘”¶µƒ—⁄¬Î£¨∂‡∏ˆ—⁄¬Î”√∑÷∫≈∑÷∏Ó
        DECLARE_MEMBER_AND_METHOD(std::string, m_strMachineCode, MachineCode) ///< ª˙–µ¬Î
        DECLARE_MEMBER_AND_METHOD(std::string, m_strNickName,NickName) ///< √˚≥∆
        DECLARE_MEMBER_AND_METHOD_V11(U64, m_nMac, Mac, 0)                 ///< Õ¯ø®ŒÔ¿Ìµÿ÷∑
        DECLARE_MEMBER_AND_METHOD_V11(U16, m_nRFBPort1, RFBPort1, 0)       ///< RFB ˝æ›Õ∆ÀÕ∑˛ŒÒ∆˜∂Àø⁄(÷˜∂Ø÷±¡¨)
        DECLARE_MEMBER_AND_METHOD_V11(U16, m_nRFBPort2, RFBPort2, 0)       ///< RFB ˝æ›Ω” ’∑˛ŒÒ∆˜∂Àø⁄(±ª∂Ø÷±¡¨)
        DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nAuthMethodMask, AuthMethodMask, 0); ///< »œ÷§∑Ω Ω—⁄¬Î,@see AUTHENTICATION_TYPE
        DECLARE_MEMBER_AND_METHOD(std::string, m_strPwd, Pwd); ///< πÃ∂®ø⁄¡Ó£¨”√ªß…Ë÷√
        DECLARE_MEMBER_AND_METHOD(std::string, m_strTmpPwd, TmpPwd); ///< ¡Ÿ ±ø⁄¡Ó£¨≤ª–Ë“™±£¥Ê
        DECLARE_MEMBER_AND_METHOD_V11(U64, m_nDeviceID, DeviceID,0); ///< ¥”∑˛ŒÒ∆˜ªÒ»°µƒ…Ë±∏ID£¨≤ª–Ë“™±£¥Ê
        DECLARE_MEMBER_AND_METHOD(std::string, m_strRouteIPs, RouteIPs)  ///< ¬∑”…ipµÿ÷∑£¨∂‡∏ˆµÿ÷∑”√∑÷∫≈∑÷∏Ó
        DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nProtocolVer, ProtocolVer, 0); ///< øÕªß∂À÷ß≥÷µƒ–≠“È∞Ê±æ
        DECLARE_MEMBER_AND_METHOD_V11(double, m_nAppVer, AppVer, 1); ///< ”¶”√≥Ã–Ú∞Ê±æ–≈œ¢
        DECLARE_MEMBER_AND_METHOD(std::string, m_strRegion, Region);               ///< øÕªß∂ÀÀ˘‘⁄µƒ«¯”Ú
    };

    // --------------------------------------------------------------------------------
    /// <summary>
    /// ¡¨Ω”«Î«Û
    /// </summary>
    // --------------------------------------------------------------------------------
    class CConnectionRequest :public CSerializeEntry
    {
    public:
        CConnectionRequest(const char* lpName = "", const U64 nSrcID = 0, const U64 nDstID = 0, const U32 nAuthMothed = AM_AUTO)
            :m_strNickName(lpName)
            , m_nSrcID(nSrcID)
            , m_nDstID(nDstID)
            , m_nAuthMothed(nAuthMothed)
        {
        }
        ~CConnectionRequest() {}

        ENABLE_SERIALIZE(CConnectionRequest)

    public:
        virtual void Serialize(CJSONSerializer& refJsonSerializer) const override;

        virtual void DeSerialize(CJSONSerializer& refJsonSerializer) override;

    private:
        DECLARE_MEMBER_AND_METHOD(std::string, m_strNickName, NickName)          ///< √˚≥∆
        DECLARE_MEMBER_AND_METHOD_V11(U64, m_nSrcID, SrcID,0)                    ///<  ‘¥ID
        DECLARE_MEMBER_AND_METHOD_V11(U64, m_nDstID, DstID,0)                    ///< ƒø±ÍID
        DECLARE_MEMBER_AND_METHOD_V11(U32, m_nAuthMothed,AuthMothed, AM_AUTO)    ///< »œ÷§ƒ£ Ω @see AUTHENTICATION_METHOD
        DECLARE_MEMBER_AND_METHOD_V11(U16, m_nRejectMode, RejectMode, RCP::RM_UNKNOWN)    ///< »œ÷§ƒ£ Ω @see AUTHENTICATION_METHOD
    };


    // --------------------------------------------------------------------------------
/// <summary>
/// »œ÷§«Î«Û
/// </summary>
// --------------------------------------------------------------------------------
    class CAuthenticationRequest :public CSerializeEntry
    {
    public:
        CAuthenticationRequest(const U64 nDstID = 0,const char* lpPwd = "")
            : m_nDstID(nDstID)
            , m_strPwd(lpPwd)
        {
        }
        ~CAuthenticationRequest() {}

        ENABLE_SERIALIZE(CAuthenticationRequest)

    public:
        virtual void Serialize(CJSONSerializer& refJsonSerializer) const override;

        virtual void DeSerialize(CJSONSerializer& refJsonSerializer) override;

    private:
        DECLARE_MEMBER_AND_METHOD_V11(U64, m_nDstID, DstID, 0)         ///<  ƒø±ÍID
        DECLARE_MEMBER_AND_METHOD(std::string, m_strPwd, Pwd)          ///<  ø⁄¡Ó
    };


    // --------------------------------------------------------------------------------
/// <summary>
/// Õ®÷™øÿ÷∆∂À∫Õ±ªøÿ÷∆∂À◊º±∏∫√¡¨Ω”
/// </summary>
// --------------------------------------------------------------------------------
    class CReadyConnRequest :public CSerializeEntry
    {
    public:
        CReadyConnRequest(
            const U32 nSessionID = 0
            , const U32 nPeerID = 0
            , const U32 nPeerIP = 0
            , const U32 nPeerPort = 0
            , const U32 nCommunicationMode = 0
            , const U32 nRoleType = RCT_CONTROLLER
            , const U32 nProtocolVer = PV_FIRST)
            : m_nSessionID(nSessionID)
            , m_nPeerID(nPeerID)
            , m_nPeerIP(nPeerIP)
            , m_nPeerPort(nPeerPort)
            , m_nCommunicationMode(nCommunicationMode)
            , m_nRoleType(nRoleType)
            , m_nProtocolVer(nProtocolVer)
        {
        }
        ~CReadyConnRequest() {}

        ENABLE_SERIALIZE(CReadyConnRequest)

    public:
        virtual void Serialize(CJSONSerializer& refJsonSerializer) const override;

        virtual void DeSerialize(CJSONSerializer& refJsonSerializer) override;

    private:
        DECLARE_MEMBER_AND_METHOD_V11(U32, m_nSessionID, SessionID, 0)         ///<  ª·ª∞ID
        DECLARE_MEMBER_AND_METHOD_V11(U64, m_nPeerID, PeerID, 0)          ///<  øÿ÷∆…Ë±∏ID
        DECLARE_MEMBER_AND_METHOD_V11(U32, m_nPeerIP, PeerIP, 0)         ///<  ƒø±ÍIP
        DECLARE_MEMBER_AND_METHOD_V11(U32, m_nPeerPort, PeerPort, 0)         ///< ƒø±Í∂Àø⁄
        DECLARE_MEMBER_AND_METHOD_V11(U32, m_nCommunicationMode, CommunicationMode, ST_HOLE)         ///<  ª·ª∞¿‡–Õ
        DECLARE_MEMBER_AND_METHOD_V11(U32, m_nRoleType, RoleType, RCT_CONTROLLER)         ///<  Ω«…´¿‡–Õ,@see RC_CLIENT_TYPE
        DECLARE_MEMBER_AND_METHOD_V11(U32, m_nProtocolVer, ProtocolVer, PV_FIRST)         ///<  –≠“È∞Ê±æ∫≈,@see PROTOCOL_VER
        DECLARE_MEMBER_AND_METHOD(std::string, m_strNickName, NickName)          ///< √˚≥∆
    };

    // --------------------------------------------------------------------------------
/// <summary>
/// –¬µƒ÷–◊™»ŒŒÒ«Î«Û
/// </summary>
// --------------------------------------------------------------------------------
    class CNewRelayTaskRequest :public CSerializeEntry
    {
    public:
        CNewRelayTaskRequest(
            const U32 nSessionID = 0
            , const U64 nSrcID = 0
            , const U64 nDstID = 0
            , const U32 nRelaySvrID = 0)
            : m_nSessionID(nSessionID)
            , m_nSrcID(nSrcID)
            , m_nDstID(nDstID)
            , m_nRelaySvrID(nRelaySvrID)
        {
        }
        ~CNewRelayTaskRequest() {}

        ENABLE_SERIALIZE(CNewRelayTaskRequest)

    public:
        virtual void Serialize(CJSONSerializer& refJsonSerializer) const override;

        virtual void DeSerialize(CJSONSerializer& refJsonSerializer) override;

    private:
        DECLARE_MEMBER_AND_METHOD_V11(U32, m_nSessionID, SessionID, 0)  ///<  ª·ª∞ID
        DECLARE_MEMBER_AND_METHOD_V11(U64, m_nSrcID, SrcID, 0)          ///<  øÿ÷∆…Ë±∏ID
        DECLARE_MEMBER_AND_METHOD_V11(U64, m_nDstID, DstID, 0)          ///<  ±ªøÿ÷∆…Ë±∏ID
        DECLARE_MEMBER_AND_METHOD_V11(U32, m_nRelaySvrID, RelaySvrID, 0)  ///<  ÷–◊™∑˛ŒÒ∆˜ID
    };

    extern std::string GetStatusCodeDesc(EXCEPTION_STATUS nStatusCode);
    using CRelayTaskSharedArray = CSharedDynamicJsonVector<CNewRelayTaskRequest>;
}

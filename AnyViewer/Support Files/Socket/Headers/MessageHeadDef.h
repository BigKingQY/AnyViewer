//
//  MessageHeadDef.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//

#ifndef MessageHeadDef_h
#define MessageHeadDef_h

#pragma once
#include "Typedef.h"

#define ACCOUNT_LENGTH_MAX 32
#define PASSWORD_LENGTH_MAX 32
#define MD5_LEN_MAX 32

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

/// π„≤•¿‡–Õ√∂æŸ
enum BROADCAST_TYPE
{
    BT_SINGLECAST = 0,    ///< √∂æŸ£¨µ•≤•
    BT_BROADCAST = 4      ///< √∂æŸ£¨π„≤•
};


/// Õ®–≈–≠“È√∂æŸ
///
/// ”Î∑˛ŒÒ∆˜Õ®–≈µƒ–≠“È¡–±Ì
enum AX3SOFT_PROTOCOL_VERSION
{
    VERSION_ONE = 0X10,
    VERSION_MAX,
};

/// <summary></summary>
enum STREAM_MSG
{
    MT_TRANSFER_STREAM = 4096,                    ///< √∂æŸ£¨¥´ ‰ ˝æ›¡˜
    MT_TRANSFER_STREAM_DATA,            ///< √∂æŸ£¨¥´ ‰ ˝æ›¡˜ ˝æ›
    MT_SYN_STREAM,                        ///< √∂æŸ£¨Õ¨≤Ω ˝æ›¡˜
    MT_ABORT_TRANSFER_STREAM,          ///< √∂æŸ£¨÷’÷π ˝æ›¡˜ ˝æ›¥´ ‰
    MT_MOVE_STREAM_OFFSERT,            ///< √∂æŸ£¨“∆∂Ø¥´ ‰ ˝æ›¡˜ ˝æ›∆´“∆
};


/// ¥ÌŒÛ¿‡–Õ√∂æŸ
///
/// ŒÂ∞Ÿ“‘«∞µƒ¥ÌŒÛ∂®Œª—œ÷ÿ¥ÌŒÛ£¨¡¨Ω”Ω´∂œø™
enum STREAM_EXCEPTION_STATUS
{
    ES_NOT_FOUND_TRANS = 4096,      ///< √∂æŸ£¨√ª”–’“µΩ÷∏∂®µƒ ¬ŒÔ
    ES_RETRANSFER,      ///< √∂æŸ£¨¥´ ‰ ˝æ› ß∞‹
    ES_ALLOCATE_TRANS_UNSUCCESSFULLY,      ///< √∂æŸ£¨∑÷≈‰ ¬ŒÒ…œœ¬Œƒ ß∞‹
};

/// Õ®–≈–≠“È±Í ∂√∂æŸ
///
enum MAGIC_FLAG
{
    HEADER_MAGIC = 0XFFFE, ///< √∂æŸ£¨ ”√”⁄ ∂±Õ®–≈ ˝æ›∞¸
};

// À˘”–µƒœ˚œ¢∂º”–œ‡Õ¨µƒœ˚œ¢Õ∑,∂®“Â»Áœ¬,’˚∏ˆœ˚œ¢Õ∑∞¸∫¨4”Ú£∫
// 1: ∞¸Õ∑ ∂±¬Î
// 2: œ˚œ¢Œ®“ªµƒID
// 3: œ˚œ¢¿‡–Õ
// 4: ±Í÷æ£¨µ⁄“ªŒª «≤Ÿ◊˜¿‡–Õ∂®“Â(0:«Î«Û£¨1£∫œÏ”¶),µ⁄∂˛Œª≤Ÿ◊˜Ω·π˚∂®“Â(0: ß∞‹£¨1£∫≥…π¶),∆‰À¸µƒŒª±£¡Ù
// 5: œ˚œ¢À˘–Ø¥¯µƒ ˝æ›∞¸≥§∂»
// 6: –£—È∫Õ.
// 7: ‘ÿ∫… ˝æ›Õ∑£®÷˜“™”√”⁄–£—È£©
// ----------------------------
// |                          |
// |                          |
// | œ˚œ¢Õ∑                |
// -----------------------------
// |                          |
// | (ø…—° ˝æ›)           |
// ----------------------------


// ------------------------œ˚œ¢Õ∑∂®“Â≤ø∑÷---------------------------------------------

#pragma pack(1)

// --------------------------------------------------------------------------------
/// <summary>
/// @brief œ˚œ¢Õ∑
/// ∏√Õ®–≈÷–√ø∏ˆ ˝æ›∞¸∂ºΩ´–Ø¥¯“ª∏ˆœ˚œ¢Õ∑
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _message_header
{
    U16                  nMagic;                  ///< ∞¸Õ∑ ∂±¬Î
    U32                  nID;                     ///< œ˚œ¢ID
    U16                  nType;                   ///< œ˚œ¢¿‡–ÕMESSAGE_TYPE
    U8                   nFlags;                  ///< µ⁄15 -12Œª±Ì æ∞Ê±æ£¨µ⁄“ªŒª±Ì æ≤Ÿ◊˜¿‡–Õ°¢µ⁄∂˛Œª±Ì æ≤Ÿ◊˜Ω·π˚£¨µ⁄»˝Œª±Ì æ «∑Ò «π„≤•∞¸
    U32                  nPayloadLen;             ///< –Ø¥¯µƒ ˝æ›≥§∂»
    U8                   nPadding;                ///< ÃÓ≥‰µƒ◊÷Ω⁄ ˝
    U32                  nChecksum;               ///< –£—È∫Õ£¨≤…”√”ÎTCPœ‡Õ¨º∆À„∑Ω∑®£¨–Ë“™º∆À„µƒ ˝æ›∞¸¿®∞¸Õ∑∫Õ‘ÿ∫… ˝æ›
}MESSAGE_HEADER;


/// @brief ≤È—Øµ«¬º◊¥Ã¨«Î«Û
typedef struct _common_request
{
    MESSAGE_HEADER objMsgHeader;
    U64                     nStatusCode;   ///< ∑µªÿœÏ”¶Ω·π˚◊¥Ã¨¬Î
}COMMON_REQUEST;

/// @brief µ«¬ºœÏ”¶
typedef struct  _common_response
{
    MESSAGE_HEADER objMsgHeader;
    U64                     nStatusCode;   ///< ∑µªÿœÏ”¶Ω·π˚◊¥Ã¨¬Î
}COMMON_RESPONSE;


/// @brief –ƒÃ¯±®
typedef struct _common_heartbeat
{
    MESSAGE_HEADER objMsgHeader;
    U64                     nStatusCode;   ///< ∑µªÿœÏ”¶Ω·π˚◊¥Ã¨¬Î
}COMMON_HEARTBEAT;

// --------------------------------------------------------------------------------
/// <summary>
///  ¬ŒÒ«Î«Û
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _transfer_stream_request
{
    MESSAGE_HEADER objMsgHeader;
    U16 nType;                   ///<  ˝æ›¿‡–Õ @see STREAM_DATA_TYPE
    U64 nId;                   ///<  ¬ŒÒid
    U32 nSrcAccount;           ///<‘¥’À∫≈
    U32 nDstAccount;           ///<ƒøµƒ’À∫≈
    I8 szMd5[MD5_LEN_MAX + 1];              ///< md5
    U64 nSize;                 ///< ¥Û–°
    I8 bQuickMode;
}TRANSFER_STREAM_REQUEST;


// --------------------------------------------------------------------------------
/// <summary>
/// Õ¨≤Ω¡˜ ¬ŒÒ«Î«Û
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _syn_stream_request
{
    MESSAGE_HEADER objMsgHeader;
    U16 nType;                   ///<  ˝æ›¿‡–Õ @see STREAM_DATA_TYPE
    U64 nId;                   ///< ‘¥ ¬ŒÒid
    U32 nSrcAccount;            ///<‘¥’À∫≈
    U32 nDstAccount;            ///<ƒøµƒ’À∫≈
    I8 szMd5[MD5_LEN_MAX + 1];              ///< md5
    U64 nSize;                 ///< ¥Û–°
}SYN_STREAM_REQUEST;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// ¡˜”¶¥ ˝æ›∞¸
/// </summary>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct  _transfer_stream_response
{
    MESSAGE_HEADER objMsgHeader;
    U64 nId;       ///< ¡˜‘¥ID
    U64 nStatusCode;     ///< ∑µªÿœÏ”¶Ω·π˚◊¥Ã¨¬Î
}TRANSFER_STREAM_RESPONSE;

// --------------------------------------------------------------------------------
/// <summary>
/// ÷–÷π ˝æ›¡˜¥´ ‰ ¬ŒÒ
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _abort_transfer_stream_request
{
    MESSAGE_HEADER objMsgHeader;
    U64 nId;                   ///<  ¬ŒÒid
}ABORT_TRANSFER_STREAM_REQUEST;

// --------------------------------------------------------------------------------
/// <summary>
///  ¬ŒÒ µÃÂ
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _transfer_stream_data_request
{
    MESSAGE_HEADER objMsgHeader;
    U64 nId;                   ///<  ¬ŒÒid
    U32 nSize;                 ///< ¥Û–°
    U64 nOffset;               ///< ∆´“∆
}TRANSFER_STREAM_DATA_REQUEST;



/// @brief ¥´ ‰ ˝æ›¡˜”¶¥ ˝æ›∞¸
typedef struct  _transfer_stream_data_response
{
    MESSAGE_HEADER objMsgHeader;
    U64 nId;             ///<  ¬ŒÒid
    U64 nOffset;         ///< œ¬“ª∏ˆ∆´“∆
    U32 nStatusCode;     ///< ∑µªÿœÏ”¶Ω·π˚◊¥Ã¨¬Î
}TRANSFER_STREAM_DATA_RESPONSE;


// --------------------------------------------------------------------------------
/// <summary>
/// “∆∂Ø ˝æ›¡˜∂¡∆´“∆,”√”⁄∂œµ„–¯¥´
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _move_stream_offsert_request
{
    MESSAGE_HEADER objMsgHeader;
    U64 nId;             ///<  ¬ŒÒid
    U64 nOffset;               ///< ∆´“∆
}MOVE_STREAM_OFFSET_REQUEST;


// --------------------------------------------------------------------------------
/// <summary>
/// “∆∂Ø ˝æ›¡˜∂¡∆´“∆,”√”⁄∂œµ„–¯¥´
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _move_stream_offsert_reponse
{
    MESSAGE_HEADER objMsgHeader;
    U64 nId;             ///<  ¬ŒÒid
    U64 nOffset;      ///< ∆´“∆
    U32 nStatusCode;  ///< ∑µªÿœÏ”¶Ω·π˚◊¥Ã¨¬Î
}MOVE_STREAM_OFFSET_REPONSE;

#pragma pack()


//************************************************************************************************//
//œ¬√Ê «“ª–©∏®÷˙∫Ø ˝

#define MESSAGE_HEADER_LEN     sizeof(MESSAGE_HEADER)
#define MESSAGE_FIXED_PAYLOAD_LEN(a)    (sizeof(a)  -  sizeof(MESSAGE_HEADER)) ///< πÃ∂® ˝æ›µƒ≥§∂»£¨“ª∏ˆ ˝æ›∞¸–Ø¥¯µƒ ˝æ›”…πÃ∂® ˝æ›+ø…±‰ ˝æ›£®ø…—°£©
#define REQUEST_SUCCESSS(a)    ((OR_SUCCESS & a) == OR_SUCCESS)
#define MESSAGE_DATA_PTR(a)    (((unsigned char*)a) + MESSAGE_HEADER_LEN)     ///<  ∑µªÿ ˝æ›Õ∑÷∏’Î



// --------------------------------------------------------------------------------
/// <summary>
/// º∆À„ECC√‹Œƒ≥§∂»
/// </summary>
/// <param name="nPlaintextLen">√˜Œƒ≥§∂»</param>
/// <returns>√‹Œƒ≥§∂»</returns>
/// <created>ª∆¿ˆ‘∆,2019/2/19</created>
/// <changed>ª∆¿ˆ‘∆,2019/2/19</changed>
// --------------------------------------------------------------------------------
inline U32 CalcECCCiphertextLength(U32 nPlaintextLen)
{
    return (nPlaintextLen + 45) / 46 * 48 + 1;
}

inline U32 GetMsgId()
{
    static U32 uId = 1;
    return uId++;
}

template<class T>
inline unsigned char* GetVarDataPtr(unsigned char * pPacket)
{
    return pPacket + sizeof(T);
}

#endif /* MessageHeadDef_h */

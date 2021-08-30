///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  
/// @brief 防水墙探头部分
/// 
/// 防水墙探头与服务器之间通信所需要的消息类型定义.所有的消息都由消息头和消息\n
/// 数据量部分组成，消息头的结构都是相同的，但是消息数据根据消息类型的不同而\n
/// 不同.整个协议请求&应答的模式。
///
/// @author 黄丽云
/// @version 1.0
/// @date        2016-9-1
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifdef WIN32
#pragma warning(disable : 4200) 
#endif

#include "TypeDefine.h"

#define ACCOUNT_LENGTH_MAX 32
#define PASSWORD_LENGTH_MAX 32
#define MD5_LEN_MAX 32

/// 操作类型枚举
enum OPERATOR_TYPE
{
	OT_REQUEST = 0,            ///< 枚举，请求操作
	OT_RESPONSE = 1,	        ///< 枚举，应答操作
};

/// 操作结果枚举
enum OPERATOR_RESULT
{
	OR_FAILURE = 0, ///< 枚举，当前操作失败
	OR_SUCCESS = 2  ///< 枚举，当前操作成功
};

/// 广播类型枚举
enum BROADCAST_TYPE
{
	BT_SINGLECAST = 0,    ///< 枚举，单播
	BT_BROADCAST = 4      ///< 枚举，广播
};


/// 通信协议枚举
/// 
/// 与服务器通信的协议列表
enum AX3SOFT_PROTOCOL_VERSION
{
	VERSION_ONE = 0X10,
	VERSION_MAX,
};

/// <summary></summary>
enum STREAM_MSG
{
	MT_TRANSFER_STREAM = 4096,	                ///< 枚举，传输数据流
	MT_TRANSFER_STREAM_DATA,	        ///< 枚举，传输数据流数据
	MT_SYN_STREAM,	                    ///< 枚举，同步数据流
	MT_ABORT_TRANSFER_STREAM,          ///< 枚举，终止数据流数据传输
	MT_MOVE_STREAM_OFFSERT,            ///< 枚举，移动传输数据流数据偏移
};


/// 错误类型枚举
/// 
/// 五百以前的错误定位严重错误，连接将断开
enum STREAM_EXCEPTION_STATUS
{
	ES_NOT_FOUND_TRANS = 4096,      ///< 枚举，没有找到指定的事物
	ES_RETRANSFER,      ///< 枚举，传输数据失败
	ES_ALLOCATE_TRANS_UNSUCCESSFULLY,      ///< 枚举，分配事务上下文失败
};

/// 通信协议标识枚举
/// 
enum MAGIC_FLAG
{
	HEADER_MAGIC = 0XFFFE, ///< 枚举， 用于识别通信数据包
};

// 所有的消息都有相同的消息头,定义如下,整个消息头包含4域：
// 1: 包头识别码
// 2: 消息唯一的ID
// 3: 消息类型
// 4: 标志，第一位是操作类型定义(0:请求，1：响应),第二位操作结果定义(0:失败，1：成功),其它的位保留
// 5: 消息所携带的数据包长度
// 6: 校验和.
// 7: 载荷数据头（主要用于校验）
// ----------------------------
// |                          |
// |                          |
// | 消息头                |
// -----------------------------
// |                          |
// | (可选数据)           |
// ----------------------------


// ------------------------消息头定义部分---------------------------------------------

#pragma pack(1)

// --------------------------------------------------------------------------------
/// <summary>
/// @brief 消息头
/// 该通信中每个数据包都将携带一个消息头
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _message_header
{
	U16                  nMagic;                  ///< 包头识别码
	U32                  nID;                     ///< 消息ID
	U16                  nType;                   ///< 消息类型MESSAGE_TYPE
	U8                   nFlags;                  ///< 第15 -12位表示版本，第一位表示操作类型、第二位表示操作结果，第三位表示是否是广播包
	U32                  nPayloadLen;             ///< 携带的数据长度
	U8                   nPadding;                ///< 填充的字节数
	U32                  nChecksum;               ///< 校验和，采用与TCP相同计算方法，需要计算的数据包括包头和载荷数据
}MESSAGE_HEADER;


/// @brief 查询登录状态请求
typedef struct _common_request
{
	MESSAGE_HEADER objMsgHeader;
	U64                     nStatusCode;   ///< 返回响应结果状态码
}COMMON_REQUEST;

/// @brief 登录响应
typedef struct  _common_response
{
	MESSAGE_HEADER objMsgHeader;
	U64                     nStatusCode;   ///< 返回响应结果状态码
}COMMON_RESPONSE;


/// @brief 心跳报
typedef struct _common_heartbeat
{
	MESSAGE_HEADER objMsgHeader;
	U64                     nStatusCode;   ///< 返回响应结果状态码
}COMMON_HEARTBEAT;

// --------------------------------------------------------------------------------
/// <summary>
/// 事务请求
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _transfer_stream_request
{
	MESSAGE_HEADER objMsgHeader;
	U16 nType;                   ///< 数据类型 @see STREAM_DATA_TYPE
	U64 nId;                   ///< 事务id
	U32 nSrcAccount;           ///<源账号
	U32 nDstAccount;           ///<目的账号
	I8 szMd5[MD5_LEN_MAX + 1];              ///< md5
	U64 nSize;                 ///< 大小
	I8 bQuickMode;
}TRANSFER_STREAM_REQUEST;


// --------------------------------------------------------------------------------
/// <summary>
/// 同步流事务请求
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _syn_stream_request
{
	MESSAGE_HEADER objMsgHeader;
	U16 nType;                   ///< 数据类型 @see STREAM_DATA_TYPE
	U64 nId;                   ///< 源事务id
	U32 nSrcAccount;            ///<源账号
	U32 nDstAccount;            ///<目的账号
	I8 szMd5[MD5_LEN_MAX + 1];              ///< md5
	U64 nSize;                 ///< 大小
}SYN_STREAM_REQUEST;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 流应答数据包
/// </summary>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct  _transfer_stream_response
{
	MESSAGE_HEADER objMsgHeader;
	U64 nId;       ///< 流源ID
	U64 nStatusCode;     ///< 返回响应结果状态码
}TRANSFER_STREAM_RESPONSE;

// --------------------------------------------------------------------------------
/// <summary>
/// 中止数据流传输事务
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _abort_transfer_stream_request
{
	MESSAGE_HEADER objMsgHeader;
	U64 nId;                   ///< 事务id
}ABORT_TRANSFER_STREAM_REQUEST;

// --------------------------------------------------------------------------------
/// <summary>
/// 事务实体
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _transfer_stream_data_request
{
	MESSAGE_HEADER objMsgHeader;
	U64 nId;                   ///< 事务id
	U32 nSize;                 ///< 大小
	U64 nOffset;               ///< 偏移
}TRANSFER_STREAM_DATA_REQUEST;



/// @brief 传输数据流应答数据包
typedef struct  _transfer_stream_data_response
{
	MESSAGE_HEADER objMsgHeader;
	U64 nId;             ///< 事务id
	U64 nOffset;         ///< 下一个偏移
	U32 nStatusCode;     ///< 返回响应结果状态码
}TRANSFER_STREAM_DATA_RESPONSE;


// --------------------------------------------------------------------------------
/// <summary>
/// 移动数据流读偏移,用于断点续传
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _move_stream_offsert_request
{
	MESSAGE_HEADER objMsgHeader;
	U64 nId;             ///< 事务id
	U64 nOffset;               ///< 偏移
}MOVE_STREAM_OFFSET_REQUEST;


// --------------------------------------------------------------------------------
/// <summary>
/// 移动数据流读偏移,用于断点续传
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _move_stream_offsert_reponse
{
	MESSAGE_HEADER objMsgHeader;
	U64 nId;             ///< 事务id
	U64 nOffset;      ///< 偏移
	U32 nStatusCode;  ///< 返回响应结果状态码
}MOVE_STREAM_OFFSET_REPONSE;

#pragma pack()


//************************************************************************************************//
//下面是一些辅助函数

#define MESSAGE_HEADER_LEN     sizeof(MESSAGE_HEADER)
#define MESSAGE_FIXED_PAYLOAD_LEN(a)    (sizeof(a)  -  sizeof(MESSAGE_HEADER)) ///< 固定数据的长度，一个数据包携带的数据由固定数据+可变数据（可选）
#define REQUEST_SUCCESSS(a)    ((OR_SUCCESS & a) == OR_SUCCESS)
#define MESSAGE_DATA_PTR(a)    (((unsigned char*)a) + MESSAGE_HEADER_LEN)     ///<  返回数据头指针



// --------------------------------------------------------------------------------
/// <summary>
/// 计算ECC密文长度
/// </summary>
/// <param name="nPlaintextLen">明文长度</param>
/// <returns>密文长度</returns>
/// <created>黄丽云,2019/2/19</created>
/// <changed>黄丽云,2019/2/19</changed>
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

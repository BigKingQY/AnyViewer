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
#pragma warning(disable : 4200) 
#include "MessageHeadDef.h"
//#include "TreeManagerBase.h"
#include "MiscellaneousFunctions.h"
#include "JsonSerializer.h"
#include <memory>

#ifndef WIN32
#include <mutex>
#endif

namespace RCP
{
#define ACCOUNT_LENGTH_MAX 32
#define PASSWORD_LENGTH_MAX 32
#define FILE_NAME_LEN_MAX 255
#define TRANSACTION_STATUS_MSG_LEN_MAX 200   ///< 事务状态消息的最大长度

	/// 错误类型枚举
	/// 
	/// 五百以前的错误定位严重错误，连接将断开
	enum EXCEPTION_STATUS
	{
		// 用户登录相关
		ES_SUCCESS = 0,
		ES_INVALID_ACCOUNT = 1, 	///< 枚举，账号不存在
		ES_CREATE_ACCOUNT_FAILED = 2,  ///<创建账号失败
		ES_INVALID_NAME = 3, 	    ///< 枚举，无效的名称
		ES_EXIST_NAME = 4, 	        ///< 枚举，名称已经存在
		ES_INVALID_PASSWD = 5, 	    ///< 枚举，密码错误
		ES_REPEAT_LOGIN = 6, 		///< 枚举，重复登录
		ES_TIME_OUT = 7,             ///< 枚举，超时
		ES_NOT_LOGGED_IN = 8,        ///< 枚举，未登录
		ES_OPEN_FILE_UNSUCCESSFULLY = 12,      ///< 枚举，打开文件失败
		ES_SERVICE_ERR = 13,          ///<服务器错误
		ES_INVALID_SN = 14,           ///<无效激活码
		ES_REACH_AUTHORIZATION_LIMIT = 15,          ///< 授权数已经满,无法在激活
		ES_SEND_FAILED = 16,
		ES_DB_ERROR = 17,        ///< 枚举，数据库错误	
		ES_CREATE_CUSTORM_UNSUCCESSFULLY = 23,  ///< 枚举，创建客户端信息失败
		ES_NOT_FOUND_CUSTORM = 24,              ///< 枚举，没有找到客户信息
		ES_CUSTORM_OFFLINE = 25,                ///< 枚举，客户不在线
		ES_PROHIBITION_CONTROL = 26,            ///< 枚举，设备禁止被控制
		ES_NEED_PWD_AUTH = 27,                  ///< 枚举，需要密码认证
		ES_SELECT_AUTH_METHOD = 28,             ///< 枚举，选择认证方式
		ES_REJECT_CTRL_REQUEST = 29,            ///< 枚举，拒绝控制请求
		ES_SRC_DEVICE_FROZEN = 30,              ///< 枚举，源设备被冻结
		ES_DST_DEVICE_FROZEN = 31,              ///< 枚举，目标设备被冻结
		ES_READY_CONN_UNSUCCESSFULLY = 32,      ///< 枚举，准备连接失败
		ES_INVALID_CTRL_SESSION_CONTEXT = 33,   ///< 枚举，无效的控制会话上下文
		ES_INVALID_RELAING_SERVER = 34,         ///< 枚举，找不到有效的中转服务器
		ES_SEND_READY_CONN_REQUEST_UNSUCCESSFULLY = 35,        ///< 枚举，发送准备连接请求到被控制端失败
		ES_TOO_LOW_APP_VERISON = 36,            ///< 枚举，太低的APP版本请升级		
		ES_UNKNOWN_ERR,                         ///< 枚举，未知的错误	
	};

	struct STATUS_CODE_DESC
	{
		EXCEPTION_STATUS m_nStatusCode;
		std::string m_strDesc;
	};

	/// <summary>所有的消息类型一览表</summary>
	enum  MESSAGE_TYPE
	{
		MT_UNKNOWN = 0,
		MT_AES_ENCIPHER_KEY = 1001,
		MT_REGIST_RELAY_SERVER = 1002,              ///< 中转服务器注册消息
		MT_UPDATE_RELAY_SERVER_STATUS = 1003,       ///< 更新服务器注册消息
		MT_REGIST_CLIENT = 1004,                    ///< 客户端注册消息
		MT_UPDATE_CLIENT_INFO = 1005,               ///< 更新客户端信息
		MT_CONNECT = 1006,                          ///< 连接请求
		MT_AUTHENTICATE = 1007,                     ///< 认证请求请求
		MT_READY_CONN_REQUEST = 1008,               ///< 准备连接请求(服务器发往控制端和被控制端)
		MT_MSG = 1009,                              ///< 服务器发往客户端的消息，采用通用请求
		MT_CHANGE_COMMUNICATION_MODE = 1010,       ///< 改变通信模式
		MT_NEW_RELAING_TASK = 1011,                ///< 新的转发任务		
		MT_CLOSE_SESSION = 1012,                   ///< 关闭会话连接
		MT_VNC_CONNECT = 1013,                     ///< 发起VNC连接(服务器发送给控制端和被控制端)
		MT_PEER_ADDR_INFO = 1014,                  ///< 对端地址信息
		MT_HEARTBEAT = 1018,                       ///< 心跳数据
		MT_CONNECTION_STATUS_CHANGED = 1019,       ///< RC客户端与控制服务器的连接状态改变通知
		MT_SHOW_CONTROLLED_TOOLBAR = 1020,         ///< 显示控制工具条
		MT_CLOSE_CONTROLLED_TOOLBAR = 1021,        ///< 关闭控制工具条

		MT_CLOSE_ALL_SESSION = 1023,               ///< 关闭所有的会话连接
		MT_PRE_CLOSE_SESSION = 1024,               ///< 准备关闭会话连接		
		MT_ABORT_CONNECTION = 1025,                ///< 终止连接
		MT_PUBLIC_IP_REQUEST = 1026,               ///< 服务器发送来的公网IP通知
		MT_RECONVERT_RELAY_TASK_REQUEST = 1027,    ///< 当前正在进行的中转服务

		MT_MAX_MSG_COUNT,			               ///< 枚举，消息类型最大数, 该值位于MESSAGE_TYPE最后一位
	};


	// --------------------------------------------------------------------------------
	/// <summary>
	/// 本地消息类型，用于本地客户端和本地服务之间的通信
	/// </summary>
	// --------------------------------------------------------------------------------
	enum LOCAL_MESSAGE_TYPE
	{
		LMT_START_CLIENT = 2001,                    ///< 启动客户端
		LMT_REGIST_CLIENT = 2002,                   ///< 客户端注册消息
		LMT_SHOW_CLIENT = 2003,                     ///< 显示客户端
		LMT_QUERY_CLIENT = 2004,                    ///< 查询客户端的状态
		LMT_LOGOFF_CLIENT = 2005,                   ///< 注销客户端
		LMT_SIMULATE_CTRL_ALT_DEL = 2006,           ///< 模拟Ctrl+Alt+Del键		
		LMT_SESSION_STATE = 2007,                   ///< 本地服务器发送当前会话状态到客户端(本地服务器->客户端)
		LMT_QUERY_SESSION_STATE = 2008,             ///< 查询当前会话状态到客户端(客户端->本地服务器)
		LMT_RELOAD_PRJ_SETTINGS = 2009,             ///< 重新加载工程设置
		LMT_DELAYED_START_CLIENT = 2010,            ///< 延时启动客户端
	};

	/// <summary>远程控制客户端类型枚举定义</summary>
	enum  RC_CLIENT_TYPE
	{
		RCT_UNKNOWN = 0,               ///< 枚举常量，未知
		RCT_RELAY_SERVER = 1,          ///< 枚举常量，中转服务器
		RCT_NORMAL_CLIENT = 2,         ///< 枚举常量，普通客户端
		RCT_CONTROLLER = 3,            ///< 枚举常量，控制端
		RCT_CONTROLLED = 4,            ///< 枚举常量，被控制端
	};


	/// <summary>客户端状态类型枚举定义</summary>
	enum  RC_CLIENT_STATUS
	{
		CCS_OFF_LINE = 0,                 ///< 枚举，离线
		CCS_ON_LINE = 0X01,               ///< 枚举，在线
		CCS_BUSY = 0X02,                  ///< 枚举，忙碌
		CCS_HIDE = 0X03,                  ///< 枚举，隐藏	
	};	

	
	/// <summary>认证模式类型枚举</summary>
	enum AUTHENTICATION_METHOD
	{
		AM_AUTO   = 0,        ///< 枚举常量,自动
		AM_MANUAL = 1,        ///< 枚举常量,要求手工认证
	};

	/// <summary>通信方式</summary>
	enum COMMUNICATION_MODE
	{
		ST_UNKNOWN = 0,      ///< 枚举常量,无效的连接类型
		ST_SRC_TO_DST = 1,   ///< 枚举常量,源向目标发起的直连
		ST_DST_TO_SRC = 2,   ///< 枚举常量,目标向源发起的直连
		ST_HOLE = 3,         ///< 枚举常量,打洞直连(源向目标发起的直连)
		ST_RELAY = 4,        ///< 枚举常量,中转
	};

	/// <summary>协议版本</summary>
	enum PROTOCOL_VER
	{
		PV_FIRST = 1,
	};

	/// <summary>拒绝请求的方式</summary>
	enum REJECT_MODE
	{
		RM_UNKNOWN = 0,     ///< 枚举常量,未知
		RM_ACTIVE  = 1,     ///< 枚举常量,主动拒绝
		RM_TIMEOUT = 2,     ///< 枚举常量,超时
	};

	/// <summary>密码类型</summary>
	enum PWD_TYPE
	{
		PT_UNKNOWN = 0,     ///< 枚举常量,未知
		PT_TMP_PWD = 1,     ///< 枚举常量,临时的
		PT_FIX_PWD = 2,     ///< 枚举常量,固定的密码
	};

	// 必须用下面这种对齐方式，不能用#pragma pack(1)
#pragma  pack (push,1)

	// -------------------------两个通用的消息定义，不携带任何数据---------------



	/// @brief 连接应答
	typedef struct  _common_response_ex
	{
		MESSAGE_HEADER objMsgHeader;
		U32            	nStatusCode1;   ///< 返回响应结果状态码1
		U32            	nStatusCode2;   ///< 返回响应结果状态码2
	}COMMON_RESPONSE_EX;



	/// <summary>转发头</summary>
	typedef struct _transpond_header
	{
		MESSAGE_HEADER objMsgHeader;
		U32 nSrcAccount;
		U32 nDstAccount;
	}TRANSPOND_HEADER;


	typedef struct  _common_transpond_response
	{
		TRANSPOND_HEADER objTranspondHeader;
		U32                     nStatusCode;   ///< 返回响应结果状态码
	}COMMON_TRANSPOND_RESPONSE;


	/// @brief 连接准备应答数据包
	/// 客户端->服务器
	typedef struct  
	{
		MESSAGE_HEADER objMsgHeader;
		U64            nID;              ///< 会话上下文ID标识
		I8             nRoleType;    ///< 端点类型(控制端或被控制端,@see RC_CLIENT_TYPE)		
	}READY_CONN_RESPONSE;

	/// @brief 新的中转任务应答数据包
/// 客户端->服务器
	typedef struct
	{
		MESSAGE_HEADER objMsgHeader;
		U32            nSessionID;     ///< 会话上下文ID标识
		U32            nRelaySvrID;    ///< 中转服务器ID
	}NEW_RELAY_TASK_RESPONSE;

	/*-------------------------AES加密KEY消息格式定义---------------------------------------------*/

	/// @brief 查询登录状态响应
	/// 
	/// 传输方向：客户端-->服务器\n
	/// 响应：消息AES_ENCIPHER_KEY_REQUEST的响应消息格式是AES_ENCIPHER_KEY_RESPONSE
	///            它们是相同
	typedef struct  _aes_encipher_key
	{
		MESSAGE_HEADER objMsgHeader;
		char  szKeyData[48];                                  ///< 解密后的数据只有前面16个有效，剩下全是填充数据
	}AES_ENCIPHER_KEY_REQUEST, AES_ENCIPHER_KEY_RESPONSE;

	



	/// @brief 端点地址信息
/// 服务器->客户端
	typedef struct
	{
		MESSAGE_HEADER objMsgHeader;
		U32            nSessionID;     ///< 会话上下文ID标识
		U32            nIP;      ///< IP地址 
		U16            nPort;    ///< 端口
	}PEER_ADDR_INFO;

	/// @brief 改变通信方式请求
	typedef struct 
	{
		MESSAGE_HEADER objMsgHeader;
		U32            nSessionID;     ///< 会话上下文ID标识
		U8             nMode;          ///< 新的通信方式
	}CHANGE_COMM_MODE_REQUEST;
#pragma pack(pop)

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 编号与名称映射类定义
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

        ENABLE_SERIALIZE(CRelaySererStatus);
	public:
		virtual void Serialize(CJSONSerializer& refJsonSerializer) const override;

		virtual void DeSerialize(CJSONSerializer& refJsonSerializer) override;

	private:
		DECLARE_MEMBER_AND_METHOD_V11(double, m_nCPUOccupy, CPUOccupy, 0.0)                             ///< CPU使用率
		DECLARE_MEMBER_AND_METHOD_V11(double, m_nMemTotal, MemTotal, 0.0)          ///< 总内存
		DECLARE_MEMBER_AND_METHOD_V11(double, m_nMemFree, MemFree, 0.0)            ///< 空闲内存	
		DECLARE_MEMBER_AND_METHOD_V11(int, m_nFreeBand, FreeBand, 0)               ///< 空闲带宽
		DECLARE_MEMBER_AND_METHOD_V11(int, m_nSessions, Sessions, 0)               ///< 当前中转的会话
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
		DECLARE_MEMBER_AND_METHOD(std::string, m_strName, Name)                          ///< 名称
		DECLARE_MEMBER_AND_METHOD(std::string, m_strRegion, Region)                      ///< 服务器所在的地域(中文)
		DECLARE_MEMBER_AND_METHOD(U16, m_nPort, Port)                                    ///< 中转服务端口
		DECLARE_MEMBER_AND_METHOD(std::string, m_strGUID, GUID)                          ///< 全球唯一编号,如果没有设置，将采用CPU序号
		CRelaySererStatus          m_objStatus;                ///< 状态
	};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 客户端信息
/// 当注册的时候发送该消息，服务器根据该消息来计算设备id
/// </summary>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class CRCClientInfo :public CSerializeEntry
	{
	public:
		CRCClientInfo()
		{
		}
		~CRCClientInfo() {}


		/// <summary>认证方式枚举类型</summary>
		enum  AUTHENTICATION_TYPE
		{
			AT_MANUAL     = 1,       ///< 枚举常量, 人工认证
			AT_TMP_PWD    = 2,       ///< 枚举常量, 临时口令
			AT_FIX_PWD    = 4,       ///< 枚举常量, 固定口令
		};

		ENABLE_SERIALIZE(CRCClientInfo)			
	public:
		// 创建一个key
		std::string CreateKey();

	public:
		virtual void Serialize(CJSONSerializer& refJsonSerializer) const override;

		virtual void DeSerialize(CJSONSerializer& refJsonSerializer) override;

	private:
		DECLARE_MEMBER_AND_METHOD(std::string, m_strIPs, IPs)              ///< ip地址，多个地址用分号分割
		DECLARE_MEMBER_AND_METHOD(std::string, m_strIPMasks, IPMasks)      ///< ip地址对应的掩码，多个掩码用分号分割
		DECLARE_MEMBER_AND_METHOD(std::string, m_strMachineCode, MachineCode) ///< 机械码
		DECLARE_MEMBER_AND_METHOD(std::string, m_strNickName,NickName) ///< 名称
		DECLARE_MEMBER_AND_METHOD_V11(U64, m_nMac, Mac, 0)                 ///< 网卡物理地址		
		DECLARE_MEMBER_AND_METHOD_V11(U16, m_nRFBPort1, RFBPort1, 0)       ///< RFB数据推送服务器端口(主动直连)
		DECLARE_MEMBER_AND_METHOD_V11(U16, m_nRFBPort2, RFBPort2, 0)       ///< RFB数据接收服务器端口(被动直连)
		DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nAuthMethodMask, AuthMethodMask, 0); ///< 认证方式掩码,@see AUTHENTICATION_TYPE
		DECLARE_MEMBER_AND_METHOD(std::string, m_strPwd, Pwd); ///< 固定口令，用户设置		
		DECLARE_MEMBER_AND_METHOD(std::string, m_strTmpPwd, TmpPwd); ///< 临时口令，不需要保存		
		DECLARE_MEMBER_AND_METHOD_V11(U64, m_nDeviceID, DeviceID,0); ///< 从服务器获取的设备ID，不需要保存
		DECLARE_MEMBER_AND_METHOD(std::string, m_strRouteIPs, RouteIPs)  ///< 路由ip地址，多个地址用分号分割
		DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nProtocolVer, ProtocolVer, 0); ///< 客户端支持的协议版本
		DECLARE_MEMBER_AND_METHOD_V11(double, m_nAppVer, AppVer, 1); ///< 应用程序版本信息
		DECLARE_MEMBER_AND_METHOD(std::string, m_strRegion, Region);               ///< 客户端所在的区域
	};

	// --------------------------------------------------------------------------------
	/// <summary>
	/// 连接请求
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
		DECLARE_MEMBER_AND_METHOD(std::string, m_strNickName, NickName)          ///< 名称
		DECLARE_MEMBER_AND_METHOD_V11(U64, m_nSrcID, SrcID,0)                    ///<  源ID
		DECLARE_MEMBER_AND_METHOD_V11(U64, m_nDstID, DstID,0)                    ///< 目标ID
		DECLARE_MEMBER_AND_METHOD_V11(U32, m_nAuthMothed,AuthMothed, AM_AUTO)    ///< 认证模式 @see AUTHENTICATION_METHOD
		DECLARE_MEMBER_AND_METHOD_V11(U16, m_nRejectMode, RejectMode, RCP::RM_UNKNOWN)    ///< 认证模式 @see AUTHENTICATION_METHOD
	};


	// --------------------------------------------------------------------------------
/// <summary>
/// 认证请求
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
		DECLARE_MEMBER_AND_METHOD_V11(U64, m_nDstID, DstID, 0)         ///<  目标ID
		DECLARE_MEMBER_AND_METHOD(std::string, m_strPwd, Pwd)          ///<  口令
	};


	// --------------------------------------------------------------------------------
/// <summary>
/// 通知控制端和被控制端准备好连接
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
		DECLARE_MEMBER_AND_METHOD_V11(U32, m_nSessionID, SessionID, 0)         ///<  会话ID
		DECLARE_MEMBER_AND_METHOD_V11(U64, m_nPeerID, PeerID, 0)          ///<  控制设备ID
		DECLARE_MEMBER_AND_METHOD_V11(U32, m_nPeerIP, PeerIP, 0)         ///<  目标IP
		DECLARE_MEMBER_AND_METHOD_V11(U32, m_nPeerPort, PeerPort, 0)         ///< 目标端口
		DECLARE_MEMBER_AND_METHOD_V11(U32, m_nCommunicationMode, CommunicationMode, ST_HOLE)         ///<  会话类型
		DECLARE_MEMBER_AND_METHOD_V11(U32, m_nRoleType, RoleType, RCT_CONTROLLER)         ///<  角色类型,@see RC_CLIENT_TYPE
		DECLARE_MEMBER_AND_METHOD_V11(U32, m_nProtocolVer, ProtocolVer, PV_FIRST)         ///<  协议版本号,@see PROTOCOL_VER
		DECLARE_MEMBER_AND_METHOD(std::string, m_strNickName, NickName)          ///< 名称
	};

	// --------------------------------------------------------------------------------
/// <summary>
/// 新的中转任务请求
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
		DECLARE_MEMBER_AND_METHOD_V11(U32, m_nSessionID, SessionID, 0)  ///<  会话ID
		DECLARE_MEMBER_AND_METHOD_V11(U64, m_nSrcID, SrcID, 0)          ///<  控制设备ID
		DECLARE_MEMBER_AND_METHOD_V11(U64, m_nDstID, DstID, 0)          ///<  被控制设备ID
		DECLARE_MEMBER_AND_METHOD_V11(U32, m_nRelaySvrID, RelaySvrID, 0)  ///<  中转服务器ID
	};

	extern std::string GetStatusCodeDesc(EXCEPTION_STATUS nStatusCode);
	using CRelayTaskSharedArray = CSharedDynamicJsonVector<CNewRelayTaskRequest>;
}



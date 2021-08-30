///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  
/// @brief VNC协议定义
/// 
/// 详细定义了各种消息
/// 注意：为了效率整个协议采用的是windows的大端编码
/// RFB协议具体通信说明:https://www.cnblogs.com/szBeginner/p/7887686.html
/// @author 黄丽云
/// @version 1.0
/// @date        2020-11-26
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#pragma warning(disable : 4200) 
#include "TypeDefine.h"
#include "JsonSerializer.h"
#include "MiscellaneousFunctions.h"



#ifndef WIN32
#include <mutex>
#endif
#include <memory>

#define VNCP_PAYLOAD_LEN_MAX 0xFFFFFF


namespace VNCP
{
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

	/// 错误类型枚举
	/// 
	/// 五百以前的错误定位严重错误，连接将断开
	enum VNCP_EXCEPTION_STATUS
	{
		// 用户登录相关
		ES_SUCCESS = 0,
		ES_WAIT_RECONVERTING_RELAY_TASK_REQ = 1,    ///< 枚举，没有收到恢复中转任务请求
		ES_UNKNOWN_ERR,                             ///< 枚举，未知的错误	
	};

	struct STATUS_CODE_DESC
	{
		VNCP_EXCEPTION_STATUS m_nStatusCode;
		std::string m_strDesc;
	};

	/// <summary>所有的消息类型一览表</summary>
	enum  MESSAGE_TYPE
	{
		MT_UNKNOWN = 0,

		// 客户端发往服务器的消息
		MT_CLIENT_SET_PIXEL_FORMAT = 1,	             ///< 枚举，设置象素格式消息
		MT_CLIENT_READY = 2,            ///< 枚举常量,客户端已经准备
		MT_CLIENT_FRAME_BUF_UPDATE_REQUEST = 3,	     ///< 枚举，请求帧缓存更新
		MT_CLIENT_KEY_EVENT = 4,	                     ///< 枚举，按键事件
		MT_CLIENT_POINTER_EVENT = 5,	                 ///< 枚举，鼠标(指针)事件
		MT_CLIENT_CUT_TEXT = 6,	                 ///< 枚举，客户端文本剪切
		MT_CLIENT_INIT_REQUEST = 8,	             ///< 枚举，客户端初始化请求
		MT_CLIENT_LOCK_DESKTOP = 9,     ///< 枚举常量,锁定桌面
		MT_CLIENT_LOGOFF = 10,     ///< 枚举常量,注销当前用户
		MT_CLIENT_RESTART = 11,    ///< 枚举常量,重启计算机
		MT_CLIENT_SHUTDOWN = 12,  ///< 枚举常量,关闭计算机
		MT_CLIENT_OPEN_COMPUTER = 13,  ///< 枚举常量,打开我的电脑
		MT_CLIENT_OPEN_TASKMANAGER = 14,  ///< 枚举常量,打开任务管理器
		MT_CLIENT_OPEN_CMD = 15,  ///< 枚举常量,打开命令行窗口
		MT_CLIENT_VIDEO_FREEZE = 16,  ///< 枚举常量,冻结桌面消息
		MT_CLIENT_SETTING_REQUEST = 17,  ///< 枚举常量,设置请求（主要是画面质量和桌面壁纸）
		MT_CLIENT_DESK_SIZE_CHANGED = 18,  ///< 枚举常量,左面尺寸被成功改变
		MT_CLIENT_SIMULATE_CTRL_ALT_DEL = 19,     ///< 模拟Ctrl+Alt+Del键
		MT_CLIENT_CHANGE_RESOLUTION = 20,     ///< 改变分辨率

		MT_HEARTBEAT = 95,           ///< 心跳数据包
		MT_CLOSE_SESSION = 96,       ///< 关闭会话连接
		MT_SYN = 97,                 ///< 枚举常量,同步请求（仅仅限于UDP通信）
		MT_INVALID_SESSION = 98,     ///< 枚举常量,无效的会话
		MT_READY = 99,               ///< 枚举常量,客户端或服务器准备消息
		MT_HELLO = 100,	             ///< 枚举，客户端初始化请求

		// 服务端发往客户端的消息
		 
		MT_SERVER_CURSOR_SHAPE_CHANGED = 101,	 ///< 枚举，光标形状已经改变
		MT_SERVER_DESKTOP_SIZE = 102,	         ///< 枚举，新的桌面尺寸
		MT_SERVER_BELL = 103,	                 ///< 枚举，在客户端上响铃
		MT_SERVER_CUT_TEXT = 104,	             ///< 枚举，服务器剪切文本
		MT_SERVER_INIT_REQUEST = 105,	         ///< 枚举，服务器初始化请求
		MT_SERVER_CURSOR_POS_CHANGED = 106,	     ///< 枚举，光标位置已经改变
		MT_SERVER_READY = 107,                   ///< 枚举常量,服务器已经准备
		MT_SERVER_ENABLE_REQUEST = 108,          ///< 枚举常量,禁止或使能用户输入
		MT_SERVER_FRAME_DATA_FRAG = 109,	     ///< 枚举，分片帧数据

		MT_REGIST_TO_UDP_HOLE_SERVER = 109,      ///< 注册到UDP打洞服务器

		MT_SERVER_DESKTOP_PIXEL_FORMAT = 111,	 ///< 枚举，更新缓冲区像素格式
		MT_SERVER_OS_INFO = 112,	             ///< 枚举，操作系统信息
		MT_SERVER_SESSION_STATE = 113,	         ///< 枚举，返回控制服务器当前Session状态
		MT_MAX_MSG_COUNT,			             ///< 枚举，消息类型最大数, 该值位于MESSAGE_TYPE最后一位
	};



	/// <summary>协议版本</summary>
	enum PROTOCOL_VER
	{
		PV_FIRST = 1,
	};

	/// 通信协议标识枚举
/// 
	enum MAGIC_FLAG
	{
		HEADER_MAGIC = 0xFFFE, ///< 枚举， 用于识别通信数据包
	};

	/// <summary>编码类型</summary>
	enum ENCODING_TYPE
	{
		ET_RAW = 0,         ///< 枚举， 原始编码
		ET_COPYRECT = 1,    ///< 枚举， 复制矩形编码
		ET_RRE = 2,         ///< 枚举， 二维行程编码
		ET_HEXTILE = 5,     ///< 枚举， Hextile 是RRE编码的变种
		ET_ZRLE = 16,       ///< 枚举， ZRLE(Zlib Run - Length Encoding),它结合了zlib 压缩，片技术、调色板和运行长度编码
	};

	// 必须用下面这种对齐方式，不能用#pragma pack(1)
#pragma  pack (push,1)
	// --------------------------------------------------------------------------------
/// <summary>
/// @brief 消息头
/// 该通信中每个数据包都将携带一个消息头
/// </summary>
// --------------------------------------------------------------------------------
	typedef struct  _message_header
	{
		U16               nMagic;                ///< 包头识别码		
		U8                nType;                 ///< 消息类型MESSAGE_TYPE	
		U32               nSessionID;            ///< 消息采用的通道（是控制服务器分配的上下文ID,整个系统唯一）
		U32               nFlag : 8;             ///< 标志（第一位:操作类型,@see OPERATOR_TYPE,第二位操作结果,@see OPERATOR_RESULT）
		U32               nPayloadLen:24;        ///< 携带数据的长度(0xFFFFFF)		
	}VNC_MESSAGE_HEADER;

	// --------------------------------------------------------------------------------
/// <summary>
/// @brief HELLO消息
/// 当控制端或被控制端连接上中转服务器或对方的数据传输服务器后，将立即发送该消息
/// 该消息需要应答
/// </summary>
// --------------------------------------------------------------------------------
	typedef struct
	{
		VNC_MESSAGE_HEADER     objMsgHeader;
		U64                    nID;           ///< 消息序号,用于重传和乱序重组
	}HELLO_MESSAGE_REQUEST;


	
	/// @brief 客户端初始化请求
	/// 当建立连接后，客户端需要发送该消息
	/// 客户端->服务端
	typedef struct 
	{
		U8             nSharedFlag;     ///< 如果允许服务器其他客户继续连接，那么共享标志应该是非零（真）。否则，服务器将断开其他客户的连接。
		U8             nHideWallPage;   ///< 隐藏桌面设置(1：隐藏,0:不隐藏)
		U8             nImageQuality;   ///< 图像质量要求,@see IMAGE_QUALITY_TYPE
	}CLIENT_INIT_REQUEST;

	/// @brief 客户端初始化请求
    /// 用户发送来的设置请求
    /// 客户端->服务端
	typedef struct
	{
		U8             nHideWallPage;   ///< 隐藏桌面设置(1：隐藏,0:不隐藏)
		U8             nImageQuality;   ///< 图像质量要求,@see IMAGE_QUALITY_TYPE
	}CLIENT_SETTINGS_REQUEST;

	/// @brief 像素格式定义
	typedef struct 
	{
		U16 nBitsPerPixel;        ///< 个像素值需要的位数。这个数字必须大于等于nColorDepth,目前位每象素必须是8,16 或32——小于8 位象素不被支持
		U16 nColorDepth;          ///< 而depth用来表示像素值中有用的位数
		U8 nBigEndianFlag;       ///< 多字节象素被看做big-endian,那么Big-endian 标志非零
		U8 nTrueColourFlag;      ///< 如果真彩标志非零，那么最后6 项规定如何按照象素值来确定红、绿、蓝的亮度

		U16 nRedMax;             ///< 
		U16 nGreenMax;           ///< 
		U16 nBlueMax;            ///< 

		U16 nRedShift;            ///< 
		U16 nGreenShift;          ///< 
		U16 nBlueShift;           ///< 
	}PIXEL_FORMAT;

	/// @brief 服务端初始化请求
	/// 当建立连接后，服务端需要发送该消息,主要告知客户端服务器的帧缓存（桌面屏幕）的高、宽、象素格式和桌面相关的名称。
	/// 服务端->客户端
	typedef struct
	{
		U16            nFrameBufWidth;    ///< 帧缓存（桌面屏幕）的宽
		U16            nFrameBufHeight;   ///< 帧缓存（桌面屏幕）的高
		PIXEL_FORMAT   objServerPixelFormat;   ///< 象素格式
	}SERVER_INIT_REQUEST;
	

	/// @brief 设置象素格式消息
    /// 客户端发送该消息到服务用于改变缓冲区中像素的格式
	/// 客户端->服务端
	typedef struct
	{
		PIXEL_FORMAT   objPixelFormat;   ///< 象素格式
	}PIXEL_FORMAT_SETTINGS_REQUEST;

	/// @brief 设置编码格式
    /// 设置编码方式可以来确定服务器发送象素数据的类型
    /// 客户端->服务端
	typedef struct
	{
		U16            nEncodingNum;   ///< 编码个数，指定了pEncoding的条目个数
		I32            pEncoding[0];    ///< 编码数据
	}ENCODING_SETTINGS_REQUEST;



	/// @brief 请求帧缓存更新
/// 通知服务器，客户对帧缓冲区中的某个区域感兴趣，这个区域由x坐标、y坐标、宽度和高度几个参数限定
/// 客户端->服务端
	typedef struct
	{
		U8             nInc;      ///< 客户丢失了它所需要的一个特定区域的内容，就发送一条FramebufferUpdateRequest消息，把消息中的nInc(增量)置为0（false）
		U16            nX;        ///< x坐标
		U16            nY;        ///< y坐标
		U16            nWidth;    ///< 宽度
		U16            nHeight;   ///< 高度
	}FRAME_BUFFER_UPDATE_REQUEST;


	/// @brief 按键事件
    /// 某一个键的按下与释放,客户端将发送该消息到服务器
    /// 客户端->服务端
	typedef struct
	{
		U8             nDowndFlag;     ///< 键按下标致
		U32            nKey;           ///< 对于大多数普通键，“keysym”和ASCII码的值是一致的（前面3个字节为0，最后一个字节为ASCII码）
	}KEY_EVENT_REQUEST;

	/// @brief 鼠标(指针)事件
    /// 检测指针移动或者某一个键的按下或释放,客户端将发送该消息到服务器
	/// 指针目前在(x坐标、y 坐标），鼠标按钮的各键采用1到8位掩码标识，
	/// 0 表示松开，1 表示按下。拿普通鼠标来说，全零表示鼠标移动，第1，2，3 
	/// 分别对应左、中、右键。对于滑轮鼠标来说，滚轮向上对应第4位，滚轮向下对
	/// 应第5位。拖动操作是不断的发送左键按下的消息，并变换鼠标的坐标。
    /// 客户端->服务端
	typedef struct
	{
		U8             nButtonMask;     ///< 键按下标致
		U16            nX;        ///< x坐标
		U16            nY;        ///< y坐标
	}MT_POINTER_EVENT_REQUEST;


	/// @brief 客户端文本剪切
    /// 客户端有新的ISO8859 - 1(Latin - 1) 文本在它的剪切缓存里，行的末尾通过新行字符(值为10)来表示。
	/// 需要无回车（值为13)。目前还没有找到传输非Latin - 1 字符集的方法,客户端将发送该消息到服务器
    /// 客户端<->服务端
	typedef struct
	{
		U32            nLength;     ///< 文本的长度
		U8             pText[0];       ///< 文本数据
	}CUT_TEXT_EVENT;

	using STRING_AGRS = CUT_TEXT_EVENT;

	typedef struct
	{
		U16            nX;        ///< x坐标
		U16            nY;        ///< y坐标
		U16            nWidth;    ///< 宽度
		U16            nHeight;   ///< 高度
		U8             nEncodingType;   ///< 编码类型
		U8             pData[0];    ///< 数据
	}RECT_INFO;


	/// @brief 帧缓存更新
/// 
/// 服务端->客户端
	typedef struct
	{
		U16            nRects;     ///< 矩形区域的个数
		U8             pRectData[0]; ///< 数据
	}FRAME_BUFFER_UPDATE_INFO;


	/// @brief 设置颜色面板的值
	typedef struct
	{		
		U16            nRed;      ///< 红色
		U16            nGreen;    ///< 绿色
		U16            nBlue;     ///< 蓝色
	}COLOUR_ENTRY;


	/// @brief 设置颜色面板的值
    /// 
    /// 服务端->客户端
	typedef struct
	{
		U16            nFirstColour;     ///< 矩形区域的个数
		U16            nColours;         ///< 矩形区域的个数
		COLOUR_ENTRY   pColourData[0];   ///< 颜色数据,包含有nColours个颜色条目COLOUR_ENTRY
	}COLOR_MAP_ENTRIES_SETINGS_REQUEST;


	/// @brief 冻结视频请求
/// 
/// 客户端->服务端
	typedef struct
	{
		U8             nFreeze;   ///< 1:冻结，0：解冻
	}VIDEO_FREEZE_REQUEST;

	/// @brief 准备就绪通知请求 
/// 客户端或服务端准备就绪后都需要向对方发送消息，并且收到后一定要回复
/// 服务端<->客户端
	typedef struct
	{
		U8             bResponse;   ///< 1:冻结，0：解冻
	}READY_REQUEST;


	/// @brief 冻结视频请求
/// 
/// 客户端->服务端
	typedef struct
	{
		U8             bEnable;   ///< 1:允许，0：禁止
	}ENABLE_INPUT_REQUEST;


	/// @brief 注册到打洞服务器请求
	typedef struct
	{
		U64  nDeviceID;          ///< 设备ID
	}REGIST_TO_HOLE_SERVER_REQUEST;

	/// @brief 设置编码格式
/// 设置编码方式可以来确定服务器发送象素数据的类型
/// 客户端->服务端
	typedef struct
	{
		U8            pData[0];    ///< 编码数据
	}SYN_FRAME_BUFFER_REQUEST;

	/// @brief 坐面信息
    ///
    /// 采集客户端<-服务端
	typedef struct
	{
		U8             szDesktop[64];    ///< 桌面名称
		U8             szUser[64];       ///< 用户名称
	}USER_INFO;

	/// @brief 桌面坐标信息
    /// 
	/// 采集客户端<-服务端
	typedef struct
	{
		U16            nLeft;       ///< 左
		U16            nTop;        ///< 上
		U16            nRight;      ///< 右
		U16            nBottom;     ///< 下
	}DESKTOP_COORDS_INFO;

	/// @brief 坐标信息
/// 
/// 采集客户端<-服务端
	typedef struct
	{
		U16            nX;         ///< 
		U16            nY;         ///< 
	}POINT_INFO;


	/// @brief 桌面坐标信息
/// 
/// 采集客户端<-服务端
	typedef struct
	{
		U16            nX;         ///< X坐标
		U16            nY;         ///< Y坐标
		U16            nWidth;     ///< 宽度
		U16            nHeight;    ///< 高度
		U8             pData[0];    ///< 编码数据
	}CURSOR_SHAPE_INFO;

/// @brief 分片帧数据
/// 
/// 如果一个帧太大，以至于大于了数据包的最大长度，那么就需要分片
/// 采集客户端<-服务端
	typedef struct
	{
		U32            nTotal;      ///< 整个帧数据的总长度
		U32            nPayloadLen; ///< 载荷数据的长度
		U32            nOffset;     ///< 当前分片数据在整个帧数据流中的偏移
		U8             pData[0];    ///< 数据
	}FRAME_DATA_FRAG;


	/// @brief 桌面信息
/// 
/// 采集客户端<-服务端
	typedef struct
	{
		U32            nWidth;     ///< 宽度
		U32            nHeight;    ///< 高度
	}DESKTOP_SIZE;

	// --------------------------------------------------------------------------------
/// <summary>
/// @brief 会话状态消息
/// 被控制端通过该消息通知控制端自身当前的状态
/// </summary>
// --------------------------------------------------------------------------------
	typedef struct
	{
		VNC_MESSAGE_HEADER     objMsgHeader;
		U16                    nState;           ///< 状态
	}SESSION_STATE_REQUEST;

#pragma pack(pop)

	// --------------------------------------------------------------------------------
	/// <summary>
	/// 编码条目信息
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
		DECLARE_MEMBER_AND_METHOD_V11(U32, m_nWidth, Width,0)             ///<  宽		
		DECLARE_MEMBER_AND_METHOD_V11(U32, m_nHeight, Height, 0)          ///<  高
		DECLARE_MEMBER_AND_METHOD_V11(U32, m_nBitsPerPel, BitsPerPel, 0)  ///<  位色		
	};

	using CDevModePtr = std::shared_ptr<CDevMode>;
	using CDevModeArray = CSharedDynamicJsonVector<CDevMode>;

	// --------------------------------------------------------------------------------
	/// <summary>
	/// 操作系统信息
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
		// 增加一个新的显示模式
		void AppendDevMode(const U32 nWidth = 0, const U32 nHeight = 0, const U32 nBitsPerPel = 0);

	public:
		virtual void Serialize(CJSONSerializer& refJsonSerializer) const override;

		virtual void DeSerialize(CJSONSerializer& refJsonSerializer) override;

	private:
		DECLARE_MEMBER_AND_METHOD_V11(U32, m_nOS, OS, 0)  ///<  操作系统类型，@see CEnvironment::OS_VERSION
		CDevModeArray              m_arrDevModes;   ///< 显示模式列表
		DECLARE_MEMBER_AND_METHOD(CDevMode,m_objCurDevMode, CurDevMode); ///< 当前的显示模式
		DECLARE_MEMBER_AND_METHOD_V11(U32, m_nVersion, Version, 0);  ///<  被控制端软件版本号
	};


	extern std::string GetStatusCodeDesc(VNCP_EXCEPTION_STATUS nStatusCode);

}



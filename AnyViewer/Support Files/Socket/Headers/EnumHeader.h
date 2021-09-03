//
//  EnumHeader.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/2.
//

#ifndef EnumHeader_h
#define EnumHeader_h

///   警告消息枚举定义
///
///    该枚举定义了警告消息的严重程度类型\n
#ifndef SEVERITY_LEVEL_
#define SEVERITY_LEVEL_
enum SEVERITY_LEVEL
{
    SL_INFORMATION = 0,       ///< 枚举，一般提示消息
    SL_NOTICE,                      ///< 枚举，需要引起注意
    SL_WARNING,                   ///< 枚举，警告消息
    SL_CRITICAL                    ///< 枚举，严重错误
};
#endif

///    服务器事件枚举定义
///
///    该枚举定义了服务器事务对象可能收到的事件类型\n
enum  SERVICE_EVENT
{
    SE_UNKNOWN = 0,
    SE_FAILT_TO_CREATE_SOCKET,      ///< 枚举，创建socket失败
    SE_FAILT_TO_BING,                      ///< 枚举，绑定socket失败
    SE_FAILT_TO_LISTEN,                   ///< 枚举，启动侦听错误
    SE_FAILT_TO_ACCEPT,                     ///< 枚举，接收客户端的连接请求失败
    SE_FAILT_TO_GET_DEST_HOST_IP,    ///< 枚举，得到目标主机的IP地址失败
    SE_FAILT_TO_CONNECT,                  ///< 枚举，连接服务器失败
    SE_FAILT_TO_WSASTARTUP,


    SE_CONNECTED,                       ///< 枚举，连接服务器成功
    SE_SWITCH_SENDING_HEART,    ///< 枚举，切换到发送心跳数据包状态
    SE_CLOSE_CONNECTION,            ///< 枚举，切换到关闭会话连接状态
    SE_RECV_COMPLETE,              ///<接收数据完成
    SE_CHECK_HEART,
};

///    服务器状态枚举定义
///
///    该枚举定义了服务器事务对象可能处于的状态类型\n
enum NET_TRANSACTION_STATUS
{
    SS_INVALID = 0,               ///< 枚举，无效状态
    SS_INITIALIZED,               ///< 枚举，已经成功初始化
    SS_RUNING,                     ///< 枚举，正在运行状态
    SS_STOP,                         ///< 枚举，停止状态
};



///   会话状态结构枚举定义
///
///    该枚举定义通信会话可能的状态类型\n
enum CONVERSATION_STATE
{
    RS_INIT = 1,       ///< 枚举，初始状态
    RS_ESTABLISH,            ///< 枚举，通信已经建立状态
    RS_SENDING_HEART_PACKET,  ///< 枚举，正在发送心跳数据包状态
    RS_RECONNECTING,     ///< 枚举，重新连接状态
    RS_CLOSE,                  ///< 枚举，关闭状态
};

///   数据包标志枚举定义
///
///    该枚举定义了常见的数据包标志类型\n
enum PACKET_FLAG
{
    PF_ACK = 1,                               ///< 枚举，应答数据包标志
    PF_HEARTBEAT = 2,                    ///< 枚举，心跳数据包
};


/// <summary>œ˚œ¢◊‹œﬂ±æµÿœ˚œ¢¿‡–Õ√∂æŸ∂®“Â£¨Œ™¡À”Î ’µΩµƒœ˚œ¢¿‡–Õ«¯∑÷£¨À˘”–±æµÿœ˚œ¢¥”10000ø™ º±‡∫≈</summary>
enum LOCAL_MESSAGE_BUS_MSG
{
    LMBS_UPDATE_PRJ_SETTINGS = 10000,            ///< √∂æŸ≥£¡ø£¨∏˙–¬¡Àπ§≥Ã…Ë÷√
    LMBS_CLOSE_RCSERVER_CONNECTION = 10001,      ///< √∂æŸ≥£¡ø£¨”Îøÿ÷∆∑˛ŒÒ∆˜∂œø™¡¨Ω” ¬º˛
    LMBS_RECEIVED_RC_PACKET = 10002,             ///< √∂æŸ≥£¡ø£¨ ’µΩøÿ÷∆ª·ª∞∞¸ ¬º˛
    LMBS_RECEIVED_VNC_PACKET = 10003,            ///< √∂æŸ≥£¡ø£¨ ’µΩVNCª·ª∞ ˝æ›∞¸ ¬º˛
    LMBS_ESTABLISH_VNC_CONNECTION = 10004,       ///< √∂æŸ≥£¡ø£¨≥…π¶Ω®¡¢¡ÀVNC¡¨Ω”
    LMBS_CLOSE_VNC_CONNECTION = 10005,           ///< √∂æŸ≥£¡ø£¨πÿ±’VNC¡¨Ω”
    
    LMBS_ESTABLISH_SVR_CONNECTION = 10006,   ///< √∂æŸ≥£¡ø£¨Ω®¡¢µΩ∑˛ŒÒ≥Ã–Ú–¬µƒøÿ÷∆¡¨Ω” ¬º˛
    LMBS_RECEIVED_SVR_PACKET = 10007,        ///< √∂æŸ≥£¡ø£¨ ’µΩ∑˛ŒÒ≥Ã–Ú ˝æ›∞¸
    LMBS_CLOSE_SVR_CONNECTION = 10008,       ///< √∂æŸ≥£¡ø£¨”Î∑˛ŒÒ≥Ã–Ú∂œø™¡¨Ω” ¬º˛
    LMBS_RECONNECT_SVR_SERVER = 10009,       ///< √∂æŸ≥£¡ø£¨ ‘Õº÷ÿ–¬¡¨Ω”∑˛ŒÒ∆˜

    LMBS_RECONNECT_RC_SERVER = 10010,            ///< √∂æŸ≥£¡ø£¨ ‘Õº÷ÿ–¬¡¨Ω”øÿ÷∆∑˛ŒÒ∆˜ ¬º˛
    LMBS_CHANGED_RESOLUTION = 10011,             ///< √∂æŸ≥£¡ø£¨∑÷±Ê¬ “—æ≠±ª∏ƒ±‰

    LMBS_ESTABLISH_CONSOLE_CONNECTION = 10020,   ///< √∂æŸ≥£¡ø£¨Ω®¡¢–¬µƒøÿ÷∆¡¨Ω” ¬º˛
    LMBS_RECEIVED_CONSOLE_PACKET = 10021,        ///< √∂æŸ≥£¡ø£¨ ’µΩøÿ÷∆Ã® ˝æ›∞¸
    LMBS_CLOSE_CONSOLE_CONNECTION = 10022,       ///< √∂æŸ≥£¡ø£¨”ÎAVCore∑˛ŒÒ∆˜∂œø™¡¨Ω” ¬º˛
    LMBS_RECONNECT_CONSOLE_SERVER = 10023,       ///< √∂æŸ≥£¡ø£¨ ‘Õº÷ÿ–¬¡¨Ω”AVCore∑˛ŒÒ∆˜
};


#endif /* EnumHeader_h */

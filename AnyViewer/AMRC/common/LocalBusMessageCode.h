#pragma once

/// <summary>消息总线本地消息类型枚举定义，为了与收到的消息类型区分，所有本地消息从10000开始编号</summary>
enum LOCAL_MESSAGE_BUS_MSG
{
	LMBS_UPDATE_PRJ_SETTINGS = 10000,            ///< 枚举常量，跟新了工程设置
	LMBS_CLOSE_RCSERVER_CONNECTION = 10001,      ///< 枚举常量，与控制服务器断开连接事件
	LMBS_RECEIVED_RC_PACKET = 10002,             ///< 枚举常量，收到控制会话包事件
	LMBS_RECEIVED_VNC_PACKET = 10003,            ///< 枚举常量，收到VNC会话数据包事件
	LMBS_ESTABLISH_VNC_CONNECTION = 10004,       ///< 枚举常量，成功建立了VNC连接
	LMBS_CLOSE_VNC_CONNECTION = 10005,           ///< 枚举常量，关闭VNC连接
	
	LMBS_ESTABLISH_CONSOLE_CONNECTION = 10006,   ///< 枚举常量，建立新的控制连接事件
	LMBS_RECEIVED_CONSOLE_PACKET = 10007,        ///< 枚举常量，收到控制台数据包
	LMBS_CLOSE_CONSOLE_CONNECTION = 10008,       ///< 枚举常量，与控制台服务器断开连接事件
	LMBS_RECONNECT_CONSOLE_SERVER = 10009,       ///< 枚举常量，试图重新连接控制台服务器

	LMBS_RECONNECT_RC_SERVER = 10010,            ///< 枚举常量，试图重新连接控制服务器事件
	LMBS_CHANGED_RESOLUTION = 10011,             ///< 枚举常量，分辨率已经被改变
};

//
//  LocalMessageBus.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/6.
//
//  本类的作用是将c++的消息转发给OC的消息代理对象

#ifndef LocalMessageBus_h
#define LocalMessageBus_h

#pragma once
#include "SDPMessageBus.h"
#include "RCProtocol.h"

#import <UIKit/UIKit.h>

using namespace RCP;

extern CMessageBus& GetLocalMessageBus();

class LocalMessageBusManager
{
public:
          
    ///用来注册消息的消息总线
    CMessageBus m_LocalMessageBus;
    
    
    ///这里放所有需要注册的消息，跟OC来通讯
    void RegistMessageBus();
    
private:
            
    /// 收到注册后的消息
    /// @param devieId 设备ID
    void OnRegistResponse(const uint64_t devieId);
    
    
    /// 收到连接后的回复
    /// @param status 状态1
    /// @param otherStatus 状态2
    void OnConnectResponse(const int status, const int otherStatus);
    
    
    
    /// 收到发起认证后的回复
    /// @param status 状态1
    /// @param otherStatus 被冻结时长，单位秒
    void OnAuthenticatResponse(const int status, const int otherStatus);
    
    
    /// 连接回调，需要将会话ID关联到视图，用于关闭连接
    /// @param sessionId 会话Id
    /// @param success 是否成功
    void OnVNCConnectResponse(const U32 sessionId, const bool success);
    
    
    /// 视图更新时的回调
    /// @param image image
    void OnFrameBufferUpdate(UIImage *image);
    
    
    /// 视图大小改变时的回调
    /// @param nWidth 宽度
    /// @param nHeight 高度
    void OnFrameBufferSizeChange(const unsigned int nWidth, const unsigned int nHeight);
    
    
    /// 根据设备ID更新昵称
    /// @param deviceId 设备ID
    /// @param nickName 昵称
    void OnUpdateNickName(const U64 deviceId, const std::string nickName);
    
};

///定义一个本地使用的注册消息的枚举
enum LOCAL_MESSAGEBUS_MSG
{
    ///设备注册成功后的方法
    MSG_DEVICE_REGIST = 1001,
    
    ///设备请求连接后的回复
    MSG_CONNECT_RESPONSE = 1002,
    
    ///发起认证后的回复
    MSG_AUTHENTICAT_RESPONSE = 1003,
    
    ///连接成功的消息回复
    MSG_VNC_CONNECT = 1004,
    
    ///刷新界面
    MSG_FRAME_UPDATE = 1005,
    
    ///界面大小改变
    MSG_FRAME_SIZE = 1006,
    
    ///获取昵称
    MSG_NICK_NAME = 1007,
};

///定义全局单例对象
extern LocalMessageBusManager* GetLocalMessageBusInstance();

#endif

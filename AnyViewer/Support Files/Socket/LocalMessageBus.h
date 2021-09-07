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
    /// @param status 状态值1
    /// @param otherStatus 状态值2
    void OnConnectResponse(const int status, const int otherStatus);
    
    
};

///定义一个本地使用的注册消息的枚举
enum LOCAL_MESSAGEBUS_MSG
{
    ///设备注册成功后的方法
    MSG_DEVICE_REGIST = 1001,
    
    ///设备请求连接后的回复
    MSG_CONNECT_RESPONSE = 1002
};

///定义全局单例对象
extern LocalMessageBusManager* GetLocalMessageBusInstance();

#endif

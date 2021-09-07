//
//  LocalMessageBus.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/6.
//

#import "LocalMessageBus.h"
#include "SDPSingleton.h"
#import "BKMessageManager.h"

///全局的本地消息总线管理对象
LocalMessageBusManager m_LocalMessageBusManager;

/// 获取本地的消息总线对象
CMessageBus& GetLocalMessageBus()
{
    return GetLocalMessageBusInstance()->m_LocalMessageBus;
}



/// 注册成功后的消息回调
/// @param devieId 本机的deviceId
void LocalMessageBusManager::OnRegistResponse(const uint64_t devieId)
{
    if ([BKMessageManager.shared.delegate respondsToSelector:@selector(onRegistResponse:)])
    {
        [BKMessageManager.shared.delegate onRegistResponse:devieId];
    }
}


void LocalMessageBusManager::OnConnectResponse(const int status, const int otherStatus)
{
    if ([BKMessageManager.shared.delegate respondsToSelector:@selector(onConnectResponse:otherStatus:)])
    {
        [BKMessageManager.shared.delegate onConnectResponse:status otherStatus:otherStatus];
    }
}

/// 注册所有消息
void LocalMessageBusManager::RegistMessageBus()
{
    CMessageBus& refMessageBus = GetLocalMessageBus();
    
    {
        unsigned int msgId = refMessageBus.Register(MSG_DEVICE_REGIST, [this](const uint64_t devieId)
                                                    {  OnRegistResponse(devieId); });
    }
    {
        unsigned int msgId = refMessageBus.Register(MSG_CONNECT_RESPONSE, [this](const int status, const int otherStatus)
                                                    {  OnConnectResponse(status, otherStatus); });
    }
    
    
}


/// 定义全局的获取消息管理单例对象
LocalMessageBusManager* GetLocalMessageBusInstance()
{
    return CGlobleSingleton<LocalMessageBusManager>::Instance();
}

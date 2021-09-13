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
/// @param deviceId 本机的deviceId
void LocalMessageBusManager::OnRegistResponse(const uint64_t deviceId)
{
//    if ([BKMessageManager.shared.delegate respondsToSelector:@selector(onRegistResponse:)])
//    {
//        [BKMessageManager.shared.delegate onRegistResponse:deviceId];
//    }
    
    NSLog(@"==== 已经获取到设备ID：%llu ====", deviceId);
    
    if (BKUserManager.shared.user) {
        
        BKUserManager.shared.user.deviceId = deviceId;
        
        [BKUserManager.shared update];
        
    }else {
        
        BKUser *user = [BKUser new];
        user.deviceId = deviceId;
        
        [BKUserManager.shared updateUser:user];
    }
}


void LocalMessageBusManager::OnConnectResponse(const int status, const int otherStatus)
{
    if ([BKMessageManager.shared.delegate respondsToSelector:@selector(onConnectResponse:otherStatus:)])
    {
        [BKMessageManager.shared.delegate onConnectResponse:status otherStatus:otherStatus];
    }
}


void LocalMessageBusManager::OnAuthenticatResponse(const int status, const int otherStatus)
{
    if ([BKMessageManager.shared.delegate respondsToSelector:@selector(onAuthenticatResponse:otherStatus:)])
    {
        [BKMessageManager.shared.delegate onAuthenticatResponse:status otherStatus:otherStatus];
    }
}


void LocalMessageBusManager::OnVNCConnectResponse(const U32 sessionId, const bool success)
{
    if ([BKMessageManager.shared.delegate respondsToSelector:@selector(onVNCConnectResponse:success:)])
    {
        [BKMessageManager.shared.delegate onVNCConnectResponse:sessionId success:success];
    }
}


void LocalMessageBusManager::OnFrameBufferUpdate(UIImage *image)
{
    if ([BKMessageManager.shared.delegate respondsToSelector:@selector(onFrameBufferUpdate:)])
    {
        [BKMessageManager.shared.delegate onFrameBufferUpdate:image];
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
    {
        unsigned int msgId = refMessageBus.Register(MSG_AUTHENTICAT_RESPONSE, [this](const int status, const int otherStatus)
                                                    {  OnAuthenticatResponse(status, otherStatus); });
    }
    {
        unsigned int msgId = refMessageBus.Register(MSG_VNC_CONNECT, [this](const U32 sessionId , const bool success)
                                                    {  OnVNCConnectResponse(sessionId, success); });
    }
    {
        unsigned int msgId = refMessageBus.Register(MSG_FRAME_UPDATE, [this](UIImage *image)
                                                    {  OnFrameBufferUpdate(image); });
    }
}


/// 定义全局的获取消息管理单例对象
LocalMessageBusManager* GetLocalMessageBusInstance()
{
    return CGlobleSingleton<LocalMessageBusManager>::Instance();
}

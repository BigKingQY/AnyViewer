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
void LocalMessageBusManager::OnRegistResponse(const U64 deviceId)
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

/// 收到连接后的回复
/// @param status 状态1
/// @param otherStatus 状态2
void LocalMessageBusManager::OnConnectResponse(const int status, const int otherStatus)
{
    if ([BKMessageManager.shared.delegate respondsToSelector:@selector(onConnectResponse:otherStatus:)])
    {
        [BKMessageManager.shared.delegate onConnectResponse:status otherStatus:otherStatus];
    }
}


/// 收到发起认证后的回复
/// @param status 状态1
/// @param otherStatus 被冻结时长，单位秒
void LocalMessageBusManager::OnAuthenticatResponse(const int status, const int otherStatus)
{
    if ([BKMessageManager.shared.delegate respondsToSelector:@selector(onAuthenticatResponse:otherStatus:)])
    {
        [BKMessageManager.shared.delegate onAuthenticatResponse:status otherStatus:otherStatus];
    }
}


/// 连接回调，需要将会话ID关联到视图，用于关闭连接
/// @param sessionId 会话Id
/// @param success 是否成功
void LocalMessageBusManager::OnVNCConnectResponse(const U32 sessionId, const bool success)
{
    if ([BKMessageManager.shared.delegate respondsToSelector:@selector(onVNCConnectResponse:success:)])
    {
        [BKMessageManager.shared.delegate onVNCConnectResponse:sessionId success:success];
    }
}


/// 视图更新时的回调
/// @param image image
void LocalMessageBusManager::OnFrameBufferUpdate(UIImage *image)
{
    if ([BKMessageManager.shared.delegate respondsToSelector:@selector(onFrameBufferUpdate:)])
    {
        [BKMessageManager.shared.delegate onFrameBufferUpdate:image];
    }
}

/// 视图大小改变时的回调
/// @param nWidth 宽度
/// @param nHeight 高度
void LocalMessageBusManager::OnFrameBufferSizeChange(const unsigned int nWidth, const unsigned int nHeight)
{
    if ([BKMessageManager.shared.delegate respondsToSelector:@selector(onFrameBufferSizeChange:height:)])
    {
        [BKMessageManager.shared.delegate onFrameBufferSizeChange:nWidth height:nHeight];
    }
}


/// 根据设备ID更新昵称
/// @param deviceId 设备ID
/// @param nickName 昵称
void LocalMessageBusManager::OnUpdateNickName(const U64 deviceId, const std::string nickName)
{
    BKUser *user = [BKUserManager.shared findConnectHistory:deviceId];
    if (user) {
        user.nickName = [NSString stringWithCString:nickName.c_str() encoding:NSUTF8StringEncoding];
        
        [BKUserManager.shared addConnectHistory:user];
    }
}

/// 注册所有消息
void LocalMessageBusManager::RegistMessageBus()
{
    CMessageBus& refMessageBus = GetLocalMessageBus();
    
    {
        unsigned int msgId = refMessageBus.Register(MSG_DEVICE_REGIST, [this](const U64 devieId)
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
    {
        unsigned int msgId = refMessageBus.Register(MSG_FRAME_SIZE, [this](const unsigned int nWidth, const unsigned int nHeight)
                                                    {  OnFrameBufferSizeChange(nWidth, nHeight); });
    }
    {
        unsigned int msgId = refMessageBus.Register(MSG_NICK_NAME, [this](const U64 deviceId, const std::string nickName)
                                                    {  OnUpdateNickName(deviceId, nickName); });
    }
}


/// 定义全局的获取消息管理单例对象
LocalMessageBusManager* GetLocalMessageBusInstance()
{
    return CGlobleSingleton<LocalMessageBusManager>::Instance();
}

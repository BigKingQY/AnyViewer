//
//  LocalMessageBus.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/6.
//

#import "LocalMessageBus.h"

NSString *const kOnRegistResponseNotification = @"kOnRegistResponseNotification";

LocalMessageBusManager m_LocalMessageBusManager;

CMessageBus& GetLocalMessageBus()
{
    return LocalMessageBusManager::Instance()->m_LocalMessageBus;
}



void LocalMessageBusManager::OnRegistResponse(const uint64_t devieId)
{
    NSLog(@"%llu", devieId);
    
    
}

void LocalMessageBusManager::RegistMessageBus()
{
    CMessageBus& refMessageBus = GetLocalMessageBus();
    
    
    unsigned int msgId = refMessageBus.Register(DEVICE_REGIST, [this](const uint64_t devieId)
                                                {  OnRegistResponse(devieId); });
}


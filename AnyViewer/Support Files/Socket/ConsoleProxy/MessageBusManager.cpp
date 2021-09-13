//
//  MessageBusManager.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/2.
//

#import "MessageBusManager.h"


MessageBusManager m_MessageBusManager;

CMessageBus& GetMessageBus()
{
    return m_MessageBusManager.m_MessageBus;
}


MessageBusManager::MessageBusManager()
{
    
}

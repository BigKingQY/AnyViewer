//
//  MessageBusManager.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/2.
//

#ifndef MessageBusManager_h
#define MessageBusManager_h

#include "SDPMessageBus.h"

extern CMessageBus& GetMessageBus();

class MessageBusManager
{
    
public:
    MessageBusManager();
    
    CMessageBus m_MessageBus;
    
};

#endif




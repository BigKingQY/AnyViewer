//
//  MessageBus.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/2.
//

#include "SDPMessageBus.h"

extern CMessageBus& GetMessageBus();

class MessageBusManager
{
    
public:
    MessageBusManager();
    
    CMessageBus m_MessageBus;
    
};





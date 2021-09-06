//
//  LocalMessageBus.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/6.
//

#ifndef LocalMessageBus_h
#define LocalMessageBus_h

#pragma once
#include "SDPMessageBus.h"

extern NSString *const kOnRegistResponseNotification;

extern CMessageBus& GetLocalMessageBus();

class LocalMessageBusManager
{
public:
    
    ///单例对象
    static LocalMessageBusManager * Instance()
    {
        static LocalMessageBusManager * pT = nullptr;

        if (nullptr == pT)
        {
            pT = new LocalMessageBusManager;
        }

        return pT;
    }
    
    ///收到注册后的消息
    void OnRegistResponse(const uint64_t devieId);
    
    ///这里放所有需要注册的消息，跟OC来通讯
    void RegistMessageBus();
      
    
    CMessageBus m_LocalMessageBus;
};

//定义一个本地使用的注册消息的枚举
enum LOCAL_MESSAGEBUS_MSG
{
    DEVICE_REGIST = 1001        //设备注册成功后的方法
};

#endif

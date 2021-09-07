//
//  AMSocketService.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//
//

/*
 这是一个用来处理socket通讯的类，主要负责对外提供接口，兼容iOS和Android平台
 1.需要初始化一些内容
 2.对消息内容会使用另外的一个类进行处理
 */


#ifndef AMSocketService_h
#define AMSocketService_h

#include "RCServerProxy.h"
#include "RCProtocol.h"

#include <vector>
#include <utility>

using namespace std;

//自定义一个pair类型，类型为：<消息注册类型, 消息注册ID>
#ifndef MSG_PAIR
using MSG_PAIR = pair<LOCAL_MESSAGE_BUS_MSG, unsigned int>;
#endif



class CNetTransactionEngine
{
public:
    
    ///开始连接控制服务器
    bool StartConnect();
    
    ///通过对方的设备ID进行连接
    bool ConnectControlled(const U64 deviceId);
    
    ///发起认证请求
    void SendAuthenticationRequest(const U64 deviceId, const char *secCode);
    
        
    //========== 必须要调用的方法 ==========
    
    ///初始化日志文件存储路径
    void InitLogFilePath(const char *path);
    
    ///初始化设置文件存储路径
    void InitSettingFilePath(const char *path);
    
    ///设置设备唯一ID
    ///通过调用SetDeviceUUID方法

    ///设置APP版本号
    ///通过调用SetAppVersion方法
    
    //==========      END      ==========
    
private:
        
    ///消息注册到消息总线
    void RegisterMessageBus();
    
    ///收到消息的回调
    void OnReceivedRCPacket(CDataPacket *packet);
    
    ///处理服务器转发来的手动应答注册应答
    void OnConnectResponse(CDataPacket* pDataPacket);
    
    ///查询IP地址的区域
    void QueryIPRegion(CDataPacket *packet);
    
    ///注册设备
    void Regist();
    
    ///消息反注册
    bool UnRegist(const unsigned int msgId);
    
    ///注册成功后的回复
    void OnRegistResponse(CDataPacket* pDataPacket);
    
private:

    ///连接代理对象
    CRCSvrProxyPtr           m_pRCSvrProxy;
    
    ///用于存储注册的消息ID和类型
    vector<MSG_PAIR>         m_MsgIDs;
    
    ///系统APP版本号
    DECLARE_MEMBER_AND_SET_METHOD_V11(double, m_AppVersion, AppVersion, 0);
    
    ///设备唯一标识，当做机器码使用
    DECLARE_MEMBER_AND_SET_METHOD_V11(string, m_DeviceUUID, DeviceUUID, "");
    
    ///记录当前的Setting保存的目录
    DECLARE_MEMBER_AND_METHOD_V11(string, m_PrjSettingPath, PrjSettingPath, "");

    
};

///定义全局单例对象
extern CNetTransactionEngine* GetTransactionInstance();

#endif





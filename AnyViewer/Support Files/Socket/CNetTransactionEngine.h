//
//  AMSocketService.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//
//

#ifndef AMSocketService_h
#define AMSocketService_h

#include "RCServerProxy.h"
#include <vector>
#include <utility>
#include "RCProtocol.h"

using namespace std;

#ifndef MSG_PAIR
typedef pair<LOCAL_MESSAGE_BUS_MSG, unsigned int>   MSG_PAIR;
#endif


typedef void(*RegistCallback)(string *deviceId, int result);

class CNetTransactionEngine
{

public:
    
    ///单例对象
    static CNetTransactionEngine * Instance()
    {
        static CNetTransactionEngine * pT = nullptr;

        if (nullptr == pT)
        {
            pT = new CNetTransactionEngine;
        }

        return pT;
    }
    
    ///开始连接控制服务器
    bool StartConnect();
    
        
    //========== 必须要调用的方法 ==========
    ///初始化日志文件
    void InitLogWithPath(const char *path);
  
    ///设置查询IP地址文件的下载路径
    void SetIPRegionPath(const char *path);
    
    ///设置设备唯一ID
    void SetDeviceId(const char *deviceId);
    
    ///设置APP版本号
    void SetAppVersion(double appVersion);
    
    //==========      END      ==========
    
private:
        
    ///消息注册到消息总线
    void RegisterMessageBus();
    
    ///收到消息的回调
    void OnReceivedRCPacket(CDataPacket *packet);
    
    ///查询IP地址的区域
    void QueryIPRegion(CDataPacket *packet);
    
    ///注册设备
    void Regist();
    
    ///消息反注册
    bool UnRegist(const unsigned int msgId);
    
    ///注册成功后的回复
    void OnRegistResponse(CDataPacket* pDataPacket);
    
    
    //成员变量列表
    //区域文件路径，暂时没用了
    string m_IPRegionPath;
    //设备唯一标识，当做机器码使用
    string m_DeviceUUID;
    //系统APP版本号
    double m_AppVersion;
    
    //用于存储注册的消息ID和类型
    vector<MSG_PAIR> m_MsgIDs;
    
    ///连接代理对象
    CRCSvrProxyPtr m_pRCSvrProxy;
    
};




#endif





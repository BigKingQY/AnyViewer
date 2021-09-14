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
#include "CCustomThreadPool.h"
#include "CVNCProxy.h"
#include "RemoteViewerCore.h"
#include "CViewEvent.h"

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
    
    ///结束连接
    void CloseConnect(const U32 nID);
    
    ///通过对方的设备ID进行连接
    bool ConnectControlled(const U64 deviceId, RCP::AUTHENTICATION_METHOD contrlMethod = RCP::AM_AUTO);
    
    ///发起认证请求
    void SendAuthenticationRequest(const U64 deviceId, const char *secCode);
    
    ///需要刷新界面时调用
    void SetNeedUpdateFrameBuffer();
    
        
    //========== 必须要调用的方法 ==========
    
    ///初始化日志文件存储路径
    void InitLogFilePath(const char *path);
    
    ///初始化设置文件存储路径
    void InitSettingFilePath(const char *path);
    
    
    
    void InitScreenSize(const float nWidth, const float nHeight);
    
    ///设置设备唯一ID
    ///通过调用SetDeviceUUID方法，这里注释是因为不需要申明方法

    ///设置APP版本号
    ///通过调用SetAppVersion方法，这里注释是因为不需要申明方法
    
    //==========      END      ==========
    
    
public:/* 任务线程池相关 */
    
    ///将任务加入线程池
    bool CreateThreadTask(const THREAD_POOL_FUN& refThreadFun, CTaskSink* pTaskSink = nullptr);
    
    
private:/* 消息相关 */
    
    ///初始化线程池对象
    void InitThreadPool();
        
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
    
    ///所有消息反注册
    void UnRegistAllMessage();
    
    ///注册成功后的回复
    void OnRegistResponse(CDataPacket* pDataPacket);
    
    ///准备连接的回复
    void OnReadyConnRequest(CDataPacket* pPacket);
    
    ///收到连接的消息
    void OnVNCConnRequest(CDataPacket* pPacket);
    
    ///处理客户端改变通信模式请求
    bool OnChangeCommunicationModeRequest(CDataPacket* pPacket);
    
    ///收到服务器发送来的对端的地址信息请求
    void OnPeerAddrInfoRequest(CDataPacket* pPacket);
    
    ///预关闭会话
    void OnPreCloseSessionRequest(CDataPacket* pPacket);
    
    ///关闭会话
    void OnCloseSessionRequest(CDataPacket* pPacket);
    
    
private:/* 端点连接相关 */
    
    // 通过端点对象找到对应的VNC代理
    CVNCProxyPtr LookupVNCProxy(PTR_NET_ENDPOINT_INTERFACE pEndpoint);
    
    // 通过设备ID来找到对应的VNC代理
    CVNCProxyPtr LookupVNCProxybyID(const U64 nPeerID);
    
    //通过会话ID来查找对应的VNC代理
    CVNCProxyPtr LookupVNCProxy(const U32 nID) const;

    //获取当前会话的VNC代理
    CVNCProxyPtr GetCurrentVNCProxy();
    
    // 删除VNC代理
    bool RemoveVNCProxy(CVNCProxyPtr pVNCProxy);

    
private:/* 自定义属性 */

    ///连接代理对象
    CRCSvrProxyPtr                  m_pRCSvrProxy;
    
    ///用于存储注册的消息ID和类型
    vector<MSG_PAIR>                m_arrMsgIDArray;
    
    ///任务线程池
    CCustomThreadPool               m_objThreadPool;
    
    ///锁
    mutable std::recursive_mutex    m_mxVNCProxy;

    ///所有的远程控制
    CVNCProxyArray                  m_arrVNCProxyArray;

    ///负责处理图像
    CRemoteViewerCorePtr            m_spRemoteViewerCore;
    
    ///处理图像事件
    CViewEventPtr                   m_spViewEvent;
    
private:/* 宏属性 */
    
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





//
//  AMSocketService.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//

#include "CNetTransactionEngine.h"
#include "CPrjSettings.h"
#include "GlobeFunction.h"
#include "MessageBusManager.h"
#include "Log.h"
#include "HttpFileDownloader.h"
#include "FuncHeader.h"
#import "BKIPRegionTool.h"
#include "RCProtocol.h"
#include "LocalMessageBus.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <cstdio>
#include <boost/format.hpp>
#include <utility>

//deviceID 长度
#define DEVICE_ID_LEN 32


/// 查询公网IP的所属区域
/// @param packet 数据包
void CNetTransactionEngine::QueryIPRegion(CDataPacket *packet)
{
    //获取公网IP
    const COMMON_REQUEST* pRequestPacket = (const COMMON_REQUEST*)packet->GetHeaderPtr();
    char szBuffer[20] = { 0 };
    NSString *ipAddr = [[NSString alloc] initWithCString:FormatHostIPAddressAnsi(int(ntohq(pRequestPacket->nStatusCode)), szBuffer) encoding:NSUTF8StringEncoding];
    
    //当获取到的IP与之前的不一样或者从来没有获取过的，需要重新获取区域
    [BKIPRegionTool.shared getRegionWithPublicIP:ipAddr complete:^(NSString * _Nonnull region)
     {
        
        if (region.length > 0)
        {
            CPrjSettings* pPrjSettings = GetPrjSettings();
            
            pPrjSettings->SetRegion(string(region.UTF8String));
        }
        
        //获取到IP区域后再进行注册
        Regist();
        
    }];
}


/// 设备注册
void CNetTransactionEngine::Regist()
{
    RCP::CRCClientInfo objRCClientInfo;
    unsigned int nAuthMethodMask = 0;
    CPrjSettings* pPrjSettings = GetPrjSettings();
    
    CHECK_POINTER(pPrjSettings);
    
    //手动认证，不需要密码
    if (pPrjSettings->GetEnableCtrl())
    {
        nAuthMethodMask = RCP::CRCClientInfo::AT_MANUAL;
    }
    
    //临时密码认证
    if (pPrjSettings->GetEnableTmpPwd())
    {
        nAuthMethodMask |= RCP::CRCClientInfo::AT_TMP_PWD;
    }
    
    //密码认证
    if (pPrjSettings->GetEnableFixPwd())
    {
        nAuthMethodMask |= RCP::CRCClientInfo::AT_FIX_PWD;
    }
    
    objRCClientInfo.SetMachineCode(m_DeviceUUID);
    objRCClientInfo.SetAuthMethodMask(RCP::CRCClientInfo::AT_MANUAL);
    objRCClientInfo.SetPwd(pPrjSettings->GetPwd());
    objRCClientInfo.SetTmpPwd(pPrjSettings->GetTmpPwd());
    objRCClientInfo.SetProtocolVer(RCP::PV_FIRST);
    objRCClientInfo.SetRFBPort1(U16(pPrjSettings->GetRFBPort()));
    objRCClientInfo.SetRFBPort2(U16(pPrjSettings->GetRFBPort()));
    objRCClientInfo.SetNickName(pPrjSettings->GetNickName());
    objRCClientInfo.SetAppVer(m_AppVersion);
    objRCClientInfo.SetRegion(pPrjSettings->GetRegion());
    objRCClientInfo.SetDeviceType(RCP::RDT_MOBILE_IOS);

    bool bResult = m_pRCSvrProxy->Regist(objRCClientInfo);
    
    if (bResult)
    {
        LOG_DEBUG("Regist successful~");
    }
    else
    {
        LOG_ERROR("Regist failred~");
    }

}


/// 消息反注册
/// @param msgId 消息ID
bool CNetTransactionEngine::UnRegist(const unsigned int msgId)
{
    
    for (int i = 0; i < m_MsgIDs.size(); i++)
    {
        
        if (msgId == m_MsgIDs[i].second)
        {
            
            CMessageBus& refMessageBus = GetMessageBus();
            
            vector<MSG_PAIR>::iterator it = m_MsgIDs.begin()+i;
            
            refMessageBus.Unregister(it->first, it->second);
            
            m_MsgIDs.erase(it);
            
            LOG_DEBUG("UnRegist a message~, msgId: %d", msgId);
            
            break;
        }
    }
    
    return false;
}



/// 注册后的回复
/// @param pDataPacket 数据包
void CNetTransactionEngine::OnRegistResponse(CDataPacket* pDataPacket)
{
    COMMON_RESPONSE* pRequest = (COMMON_RESPONSE*)pDataPacket->GetHeaderPtr();

    if ((pDataPacket->GetPacketFlag() & OR_SUCCESS) == OR_SUCCESS)
    {
        string strDeviceID;
        const U64 nDeviceID = ntohq(pRequest->nStatusCode);
        
        CMessageBus& refMessageBus = GetLocalMessageBus();
        
        //发送一个自定义的消息到本地的消息总线
        refMessageBus.SendReq<void, const uint64_t>(MSG_DEVICE_REGIST, std::forward <const uint64_t>(nDeviceID));
                
        CPrjSettings* pPrjSettings = GetPrjSettings();
        pPrjSettings->SetDeviceID(to_string(nDeviceID));

        LOG_ERROR("OnRegistResponse: OR_SUCCESS");
        
//        strDeviceID.Format(_T("%I64u"), nDeviceID);
        
//        if (strDeviceID.GetLength() == int(DEVICE_ID_LEN))
//        {
//            CPrjSettings* pPrjSettings = GetPrjSettings();
//
//            CHECK_POINTER(pPrjSettings);
//            pPrjSettings->SetDeviceID((LPCSTR)CT2A(strDeviceID));
//            LOG_ERROR("Local Device ID = %I64u", nDeviceID);
//            SetRegistedStatus(RCP::ES_SUCCESS);
//        }
//        else
//        {
//            // 如果返回了错误的注册码，重新注册一下
//            // 这是一个补救的方法，关键还是要找到原因
//            Regist();
//            SetRegistedStatus(RCP::ES_INVALID_ACCOUNT);
//        }
    }
    else
    {
//        SetRegistedStatus((unsigned int)NTOHQ(pRequest->nStatusCode));
        LOG_ERROR("OnRegistResponse: OR_FAILURE");
    }

}


// ********************************************************************************
/// <summary>
/// 发送请求试图与被控制者建立连接
/// </summary>
/// <param name="nAuthMothed"></param>
/// <returns></returns>
/// <created>Andy,2020/10/26</created>
/// <changed>Andy,2020/10/26</changed>
// ********************************************************************************
bool CNetTransactionEngine::ConnectControlled(const U64 deviceId)
{
    CPrjSettings* pPrjSettings = GetPrjSettings();
//    CEditUI* pEditUI = static_cast<CEditUI*>(m_objPaintManager.FindControl(ID_PARTNER_ID_EDIT));
//
//    CHECK_POINTER_EX(pEditUI, false);
    CHECK_POINTER_EX(pPrjSettings, false);

//    const std::string strDstDeviceID(CT2A(pEditUI->GetText()));
    const RCP::CConnectionRequest objConnRequest(
        pPrjSettings->GetNickName().c_str()
        , std::atoll(pPrjSettings->GetDeviceID().c_str())
        , std::atoll(to_string(deviceId).c_str())
        , RCP::AM_AUTO);

    return m_pRCSvrProxy->ConnectRemoteClient(objConnRequest);
}


// ********************************************************************************
/// <summary>
/// 处理服务器转发来的手动应答注册应答
/// 该函数中不能使用ShowMsgBox，它可能会阻塞线程 ，如果用户没有及时确定就会导致通信终端
/// 因为无法发送心跳数据包
/// </summary>
/// <param name="pDataPacket"></param>
/// <created>Andy,2021/3/9</created>
/// <changed>Andy,2021/3/9</changed>
// ********************************************************************************
void CNetTransactionEngine::OnConnectResponse(CDataPacket* pDataPacket)
{
//    bool bEnableConnectButton = false;
    RCP::COMMON_RESPONSE_EX* pRequest = (RCP::COMMON_RESPONSE_EX*)pDataPacket->GetHeaderPtr();
    const int nStatusCode1 = NTOHL(pRequest->nStatusCode1);
    const int nStatusCode2 = NTOHL(pRequest->nStatusCode2);
    
    CMessageBus& refMessageBus = GetLocalMessageBus();
    
    //发送一个自定义的消息到本地的消息总线
    refMessageBus.SendReq<void, const int, const int>(MSG_CONNECT_RESPONSE, std::forward <const int>(nStatusCode1), std::forward <const int>(nStatusCode2));
}


void CNetTransactionEngine::SendAuthenticationRequest(const U64 deviceId, const char *secCode)
{
    CPrjSettings* pPrjSettings = GetPrjSettings();
    
    CHECK_POINTER(pPrjSettings);
    
    //将设备ID转换成string
    string m_strDstDeviceID = to_string(deviceId);
    
    //初始化请求
    RCP::CAuthenticationRequest objRequest = RCP::CAuthenticationRequest(deviceId, secCode);
    U32 nStatusCode1 = 0;
    U32 nStatusCode2 = 0;
    
    //发送一个同步请求，并记录目标设备ID到本地
    const bool bResult = m_pRCSvrProxy->SyncAuthenticate(objRequest, nStatusCode1, nStatusCode2);
    CHistoryPartnerPtr pHistoryPartner = pPrjSettings->LookupHistoryPartner(m_strDstDeviceID);
    
    
    if (bResult)
    {
        if (pPrjSettings->GetSaveSecCode())
        {
            if (nullptr == pHistoryPartner)
            {
                pHistoryPartner = std::make_shared<CHistoryPartner>(m_strDstDeviceID);
                pPrjSettings->AppendHistoryPartner(pHistoryPartner);
            }
            
            if (nullptr != pHistoryPartner)
            {
                pHistoryPartner->SetPwd(std::string(secCode));
            }
            
        }
    }
    else
    {
        if (nullptr != pHistoryPartner)
        {
            pHistoryPartner->SetPwd("");
        }
        
        
        CMessageBus& refMessageBus = GetLocalMessageBus();
        
        //发送一个自定义的消息到本地的消息总线
        refMessageBus.SendReq<void, const int, const int>(MSG_CONNECT_RESPONSE, std::forward <const int>(nStatusCode1), std::forward <const int>(nStatusCode2));

    }
    
}



/// 收到数据包后自动调用
/// @param pPacket 数据包
void CNetTransactionEngine::OnReceivedRCPacket(CDataPacket *pPacket)
{
    LOG_DEBUG("Received a packet~");
    
    switch (pPacket->GetPacketType())
    {
        case RCP::MT_AES_ENCIPHER_KEY:
            //Regist();
            break;
            
        case RCP::MT_PUBLIC_IP_REQUEST:
            QueryIPRegion(pPacket);
            break;
            
        case RCP::MT_REGIST_CLIENT:
            OnRegistResponse(pPacket);
            break;
            
        case RCP::MT_CONNECT:
            OnConnectResponse(pPacket);
            break;
        case RCP::MT_AUTHENTICATE:
            //收到同步认证请求的回复，同步不需要处理，异步才需要
            LOG_DEBUG("Recive Authenticate packet");
            break;
            
        default:
            break;
            
    }
}


/// 注册消息到消息总线
void CNetTransactionEngine::RegisterMessageBus()
{
    CMessageBus& refMessageBus = GetMessageBus();
    unsigned int msgId = refMessageBus.Register(LMBS_RECEIVED_RC_PACKET,
                                                [this](CDataPacket* pPacket)
                                                {  OnReceivedRCPacket(pPacket); });
    MSG_PAIR msgPair = make_pair(LMBS_RECEIVED_RC_PACKET, msgId);
    m_MsgIDs.push_back(msgPair);
    
}



/// 初始化日志的存储路径
/// @param path 路径
void CNetTransactionEngine::InitLogFilePath(const char *path)
{
    // 打开日志文件
    LOG_DEBUG("日志存储路径：%s", path);
    
    CPrjSettings* pPrjSettings = GetPrjSettings();
    
    CLoggerHelper::Instance()->Open(path);
    
#ifdef DEBUG
    CLoggerHelper::Instance()->SetLevel(CLogger::LEVEL_ALL);
#else
    CLoggerHelper::Instance()->SetLevel((CLogger::LEVEL_TYPE)pPrjSettings->GetLogLevel());
#endif
}


void CNetTransactionEngine::InitSettingFilePath(const char *path)
{
    SetPrjSettingPath(string(path));
    
    CPrjSettings* pPrjSettings = GetPrjSettings();
    
    pPrjSettings->Load(m_PrjSettingPath);
}

/// 开始连接控制服务器
bool CNetTransactionEngine::StartConnect()
{
    
    // 初始化连接控制服务器代码，必须在函数CreateNewTmpSecCode之后
    // 因为连接成功会发临时口令往服务器
    CPrjSettings* pPrjSettings = GetPrjSettings();

    //将域名转换为IP地址
    std::string str_IP = getIPWithHostName(pPrjSettings->GetServerIP());
    
    //判断地址是否解析成功
    if (str_IP.length() == 0) {
        LOG_ERROR("IP address resolution failed");
        return false;
    }
                                                    
    // 启动连接控制服务器的代理
    m_pRCSvrProxy = std::make_shared<CRCSvrProxy>(
                IDE_IPv4_to_i(str_IP)
                , (USHORT)pPrjSettings->GetPort());

    if (nullptr != m_pRCSvrProxy)
    {
        bool bResult = m_pRCSvrProxy->Initial();

        if (bResult)
        {
            //用用户设置的域名来判断是否启动连接，不能用获取后ip地址，因为随操作系统
            //启动的时候可能失败，重新连接的时候会重新查询ip地址
            if (pPrjSettings->GetServerIP().size())
            {
//                m_pRCSvrProxy->SetConectionTimeout(3);
                bool ret = m_pRCSvrProxy->Connect();
                if (ret)
                {
                    RegisterMessageBus();
                    LOG_DEBUG("Connect successful!");
                }else
                {
                    LOG_ERROR("Connect failred!");
                }
                return ret;
            }
            else
            {
                LOG_ERROR("Connect RCSvrProxy (%s)!", str_IP.c_str());
            }
        }
        else
        {
            LOG_ERROR("Fail to init RCSvrProxy!");
        }
    }
    
    return false;
}




/// 通过模板实现单例
CNetTransactionEngine* GetTransactionInstance()
{
    return CGlobleSingleton<CNetTransactionEngine>::Instance();
}

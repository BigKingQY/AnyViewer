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


void CNetTransactionEngine::QueryIPRegion(CDataPacket *packet)
{
    
    const COMMON_REQUEST* pRequestPacket = (const COMMON_REQUEST*)packet->GetHeaderPtr();
    char szBuffer[20] = { 0 };
    NSString *ipAddr = [[NSString alloc] initWithCString:FormatHostIPAddressAnsi(int(ntohq(pRequestPacket->nStatusCode)), szBuffer) encoding:NSUTF8StringEncoding];
    
    //当获取到的IP与之前的不一样或者从来没有获取过的，需要重新获取区域
    [BKIPRegionTool.shared getRegionWithPublicIP:ipAddr complete:^(NSString * _Nonnull region) {
        
        if (region.length > 0) {
            CPrjSettings* pPrjSettings = GetPrjSettings();
            pPrjSettings->SetRegion(string(region.UTF8String));
        }
        
        //获取到IP区域后再进行注册
        Regist();
        
    }];
}

void CNetTransactionEngine::SetDeviceId(const char *deviceId)
{
    
    m_DeviceUUID = deviceId;
}

void CNetTransactionEngine::SetAppVersion(double appVersion)
{
    m_AppVersion = appVersion;
}

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
    
    if (bResult) {
        LOG_DEBUG("Regist successful~");
    }else
    {
        LOG_ERROR("Regist failred~");
    }

}

bool CNetTransactionEngine::UnRegist(const unsigned int msgId)
{
    
    for (int i = 0; i < m_MsgIDs.size(); i++) {
        
        if (msgId == m_MsgIDs[i].second) {
            
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


void CNetTransactionEngine::OnRegistResponse(CDataPacket* pDataPacket)
{
    COMMON_RESPONSE* pRequest = (COMMON_RESPONSE*)pDataPacket->GetHeaderPtr();

    if ((pDataPacket->GetPacketFlag() & OR_SUCCESS) == OR_SUCCESS)
    {
        string strDeviceID;
        const U64 nDeviceID = ntohq(pRequest->nStatusCode);
        
        CMessageBus& refMessageBus = GetLocalMessageBus();
        
        //发送一个自定义的消息到本地的消息总线
        refMessageBus.SendReq<void, const uint64_t>(DEVICE_REGIST, std::forward <const uint64_t>(nDeviceID));
                
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
            
        default:
            break;
            
    }
}

void CNetTransactionEngine::RegisterMessageBus()
{
    CMessageBus& refMessageBus = GetMessageBus();
    unsigned int msgId = refMessageBus.Register(LMBS_RECEIVED_RC_PACKET,
                                                [this](CDataPacket* pPacket)
                                                {  OnReceivedRCPacket(pPacket); });
    MSG_PAIR msgPair = make_pair(LMBS_RECEIVED_RC_PACKET, msgId);
    m_MsgIDs.push_back(msgPair);
    
}



void CNetTransactionEngine::InitLogWithPath(const char *path)
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


void CNetTransactionEngine::SetIPRegionPath(const char *path)
{
    if (nullptr == path) {
        LOG_ERROR("File path is error, check it");
        return;
    }
        
    m_IPRegionPath = string(path);
}


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




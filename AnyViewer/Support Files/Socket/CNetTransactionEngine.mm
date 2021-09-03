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

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <cstdio>
#include <boost/format.hpp>


string CNetTransactionEngine::GetIPRegion() {
    
    string region;
    
    
    
//    [data writeToFile:[NSString stringWithCString:m_IPRegionPath.c_str() encoding:NSUTF8StringEncoding] atomically:YES];
    
    
//    [NetworkRequest GET:@"https://ip138.com/iplookup.asp?ip=%1%&action=2" paramters:nil success:^(id  _Nullable responseObject) {
//
//        NSLog(@"%@", responseObject);
//
//    } failred:^(NSError * _Nullable error) {
//
//        NSLog(@"%@", error.localizedDescription);
//    }];
    
    
    return region;
    
}


void CNetTransactionEngine::QueryIPRegion(CDataPacket *packet)
{
    
    CPrjSettings* pPrjSettings = GetPrjSettings();
    
    if (pPrjSettings->GetRegion().size() > 0)
    {
        return;
    }
    
    const COMMON_REQUEST* pRequestPacket = (const COMMON_REQUEST*)packet->GetHeaderPtr();
    
    CHttpFileDownloader objDowndloader(true);
    std::string strUrl;
    char szBuffer[20] = { 0 };
//    const std::string strTmpIPRegion("TmpIPRegion.htm");
    
    const std::string strTmpIPRegion = m_IPRegionPath;
    
    boost::format fmt("https://ip138.com/iplookup.asp?ip=%1%&action=2");
    fmt %FormatHostIPAddressAnsi(int(ntohq(pRequestPacket->nStatusCode)), szBuffer);
    //https://ip138.com/iplookup.asp?ip=118.113.100.39&action=2
    strUrl = fmt.str();
    
//    strUrl.Format("https://ip138.com/iplookup.asp?ip=%s&action=2"
//                  , FormatHostIPAddressAnsi(int(NTOHQ(pRequestPacket->nStatusCode)), szBuffer));
    
    bool bSuccess = objDowndloader.Downdload(strUrl.c_str(), strTmpIPRegion);
    
    if (bSuccess)
    {
        std::ifstream objFileRead(strTmpIPRegion, std::ios::in | std::ios::out | std::ios::binary);
        
        try
        {
            if (objFileRead.is_open())
            {
                objFileRead.seekg(0, std::ios::end);       //设置文件指针到文件流的尾部
                std::streampos ps = objFileRead.tellg();   //读取文件指针的位置
                
                char* lpBuffer = new char[(int)ps + 1];
                
                if (nullptr != lpBuffer)
                {
                    objFileRead.seekg(0, std::ios::beg);          //设置文件指针到文件流的头部
                    objFileRead.read(lpBuffer, ps);
                    
                    const std::string astrBody = (std::string(lpBuffer)); /* CLYCodeCvt::UTF8ToANSI*///(std::string(lpBuffer));
                    std::string astrCountry;
                    std::string strTag(R"("ct":")");
                    
                    auto nPos1 = astrBody.find(strTag);
                    
                    if (nPos1 != astrBody.npos)
                    {
                        const auto nPos2 = astrBody.find("\"", nPos1 + strTag.size());
                        
                        if (nPos2 > nPos1)
                        {
                            astrCountry = astrBody.substr(nPos1 + strTag.size(), nPos2 - nPos1 - strTag.size());
                        }
                    }
                    
                    strTag = R"("ASN归属地":")";
                    
                    nPos1 = astrBody.find(strTag);
                    
                    if (nPos1 != astrBody.npos)
                    {
                        const auto nPos2 = astrBody.find("\"", nPos1 + strTag.size());
                        
                        if (nPos2 > nPos1)
                        {
                            std::string strRegion = astrCountry;
                            
                            strRegion += astrBody.substr(nPos1 + strTag.size(), nPos2 - nPos1 - strTag.size());
                            strRegion.erase(std::remove(strRegion.begin(), strRegion.end(), ' '), strRegion.end());
                            
                            pPrjSettings->SetRegion(strRegion);
                            LOG_DEBUG("%s", strRegion.c_str());
                        }
                    }
                    
                    delete[]lpBuffer;
                }
                
                objFileRead.close();
            }
        }
        catch (...)
        {
            //TRACE(_T("Fail to load the file (%s)\r\n"), static_cast<LPCSTR>(strName));
        }
        
        //删除下载的文件
        remove(m_IPRegionPath.c_str());
    }
}

void CNetTransactionEngine::Regist()
{
//    CRCClientInfo objRCClientInfo;
//    MAC arrMac[10];
//    UINT nBufSize = sizeof(arrMac);
//    std::shared_ptr<IHardwareInfo> pHardwareInfo = CreateHardwareInfoObject();
//
//    objRCClientInfo.SetMachineCode(pHardwareInfo->GetMachineCode());
//
//    bool bResult = pHardwareInfo->GetRealMac(arrMac, nBufSize);
//
//    assert(bResult);
//
//    if (bResult)
//    {
//        U64 nMac = 0;
//
//        memcpy((char*)&nMac, arrMac[0].Mac, MAC_LENGTH_MAX);
//        objRCClientInfo.SetMac(nMac);
//
//        // 获取本地所有的IP地址
//        std::string strIPs;
//        std::string strMasks;
//        std::vector<string> lstIPs;
//        std::vector<string> lstMask;
//
//        DWORD arrLocalHostIPAddr[10] = { 0 };
//        const size_t nIPs = GetLocalIPMask(lstIPs, lstMask, true);
//
//        for (size_t i = 0; i < nIPs; i++)
//        {
//            if (strIPs.size() > 0)
//            {
//                strIPs += ";";
//                strIPs += lstIPs[i];
//
//                strMasks += ";";
//                strMasks += lstMask[i];
//            }
//            else
//            {
//                strIPs = lstIPs[i];
//                strMasks = lstMask[i];
//            }
//        }
//
//        objRCClientInfo.SetIPs(strIPs);
//        objRCClientInfo.SetIPMasks(strMasks);
//
//        unsigned int nAuthMethodMask = 0;
//        CPrjSettings* pPrjSettings = GetPrjSettings();
//
//        CHECK_POINTER(pPrjSettings);
//
//        if (pPrjSettings->GetEnableCtrl())
//        {
//            nAuthMethodMask = RCP::CRCClientInfo::AT_MANUAL;
//        }
//
//        if (pPrjSettings->GetEnableTmpPwd())
//        {
//            nAuthMethodMask |= RCP::CRCClientInfo::AT_TMP_PWD;
//        }
//
//        if (pPrjSettings->GetEnableFixPwd())
//        {
//            nAuthMethodMask |= RCP::CRCClientInfo::AT_FIX_PWD;
//        }
//
//        objRCClientInfo.SetAuthMethodMask(nAuthMethodMask);
//        objRCClientInfo.SetPwd(pPrjSettings->GetPwd());
//        objRCClientInfo.SetTmpPwd(pPrjSettings->GetTmpPwd());
//        objRCClientInfo.SetDeviceID(std::atoll(pPrjSettings->GetDeviceID().c_str()));
//        objRCClientInfo.SetRouteIPs(pPrjSettings->GetRouteIPsAsString());
//        objRCClientInfo.SetProtocolVer(RCP::PV_FIRST);
//        objRCClientInfo.SetRFBPort1(U16(pPrjSettings->GetRFBPort()));
//        objRCClientInfo.SetRFBPort2(U16(pPrjSettings->GetRFBPort()));
//        objRCClientInfo.SetNickName(pPrjSettings->GetNickName());
////        objRCClientInfo.SetAppVer(GetVersionAsNum());
//        objRCClientInfo.SetRegion(pPrjSettings->GetRegion());
//
//        bResult = m_pRCSvrProxy->Regist(objRCClientInfo);
//
//        LOG_DEBUG("%s", (bResult ? ("Send regist request successfully!") : ("Fail to send regist request!")));
//    }
//    else
//    {
//        LOG_DEBUG("Fail to get mac address!");
//    }
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
            Regist();
            break;
            
        case RCP::MT_REGIST_CLIENT:
//            SetConnectionStatus(CS_CONNECTED);
//            OnRegistResponse(pPacket);
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
    
#ifdef _DEBUG
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
                    LOG_ERROR("Connect successful!");
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




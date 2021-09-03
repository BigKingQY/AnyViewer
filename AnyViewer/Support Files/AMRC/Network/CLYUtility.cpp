//
//  CLYUtility.cpp
//  AnyViewer
//
//  Created by Aomei on 2021/8/31.
//

#include "CLYUtility.hpp"
#include <chrono>
#include <cassert>
#include <vector>
#include <string.h>

CLYUtility::CLYUtility()
{
}

CLYUtility::~CLYUtility()
{
}

// ********************************************************************************
/// <summary>
/// ªÒ»°µ±«∞ ±º‰
/// </summary>
/// <param name="pTm"></param>
/// <param name="pMesc">∫¡√Îoptional</param>
/// <created>Leiyz,2019/8/7</created>
/// <changed>Leiyz,2019/8/7</changed>
// ********************************************************************************
void CLYUtility::GetCurSystemTime(tm*& pTm, int* pMsec)
{
    using namespace std::chrono;

    assert(nullptr != pTm);
    if (nullptr == pTm)
    {
        return;
    }

    system_clock::time_point nowPoint =system_clock::now();
    std::time_t nowTime = system_clock::to_time_t(nowPoint);
    pTm = localtime(&nowTime);
    if (nullptr != pMsec)
    {
        auto curMillseconds = duration_cast<milliseconds>(nowPoint.time_since_epoch());
        *pMsec = curMillseconds.count()%1000;
    }
}

// ********************************************************************************
/// <summary>
/// ’˚–Œipµÿ÷∑◊™◊÷∑˚¥Æ–Œ Ω
/// </summary>
/// <param name="nIp">–°∂À–Ú£®∏ﬂ◊÷Ω⁄¥Ê‘⁄∏ﬂµÿ÷∑£©</param>
/// <returns></returns>
/// <created>Leiyz,2019/8/8</created>
/// <changed>Leiyz,2019/8/8</changed>
// ********************************************************************************
std::string CLYUtility::Int2Ip(U32 nIp)
{
    U8* pIp = (U8*)&nIp;
    char szIp[16];
    memset(szIp, 0, sizeof(szIp));
    sprintf(szIp, "%d.%d.%d.%d", pIp[3], pIp[2], pIp[1], pIp[0]);
    return std::string(szIp);
}

// ********************************************************************************
/// <summary>
/// ip◊™’˚–Œ
/// </summary>
/// <param name="strIp"></param>
/// <param name="nIp">[out]–°∂À–Ú£®∏ﬂ◊÷Ω⁄¥Ê‘⁄∏ﬂµÿ÷∑£©</param>
/// <returns></returns>
/// <created>Leiyz,2019/8/8</created>
/// <changed>Leiyz,2019/8/8</changed>
// ********************************************************************************
bool CLYUtility::Ip2Int(const std::string & strIp, U32& nIp)
{
    bool bResult = true;
    std::vector<std::string> vecIp;
    CLYUtility::SplitString(strIp, '.', vecIp);
    
    if (vecIp.size() == 4)
    {
        int nPos = 3;
        U8* pIp = (U8*)&nIp;
        for (auto& ipValue : vecIp)
        {
            if (ipValue.empty())
            {
                bResult = false;
                break;
            }

            pIp[nPos--] = atoi(ipValue.c_str());
        }
    }

    return bResult;
}

//
//  CLYUtility.hpp
//  AnyViewer
//
//  Created by Aomei on 2021/8/31.
//

#pragma once
// µ”√–°π§æﬂ

#include <string>
#include "TypeDefine.h"

class CLYUtility
{
public:
    CLYUtility();
    ~CLYUtility();

public:
    static void GetCurSystemTime(tm*& pTm, int* pMsec);
    static std::string Int2Ip(U32 nIp);
    static bool Ip2Int(const std::string& strIp, U32& nIp);
    
    // ********************************************************************************
    /// <summary>
    /// »›∆˜ƒ£∞Â£¨ø…“‘»•vector£¨ listµ»À≥–Ú»›∆˜
    /// </summary>
    /// <param name="strSrc"></param>
    /// <param name="cSplit"></param>
    /// <param name="refContainer"></param>
    /// <created>Leiyz,2019/8/8</created>
    /// <changed>Leiyz,2019/8/8</changed>
    // ********************************************************************************
    template<typename T>
    static void SplitString(const std::string strSrc, char cSplit, T& refContainer, bool bEmpytValid = true)
    {
        size_t nPos = 0;
        for (size_t i = 0; i < strSrc.length(); ++i)
        {
            if (cSplit == strSrc.at(i))
            {
                std::string strSub = strSrc.substr(nPos, i - nPos);
                if (bEmpytValid)
                {
                    refContainer.emplace_back(strSub);
                }
                else if (!strSub.empty())
                {
                    refContainer.emplace_back(strSub);
                }
                nPos = i+1;
            }
        }

        if (nPos < strSrc.length())
        {
            refContainer.emplace_back(strSrc.substr(nPos, strSrc.length() - nPos));
        }
    }
};



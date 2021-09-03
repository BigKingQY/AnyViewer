//
//  LYCodeCvt.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//

#ifndef LYCodeCvt_h
#define LYCodeCvt_h

#pragma once

#include <string>

//=======================================================================================
/// @file  common.h
/// @brief ◊÷∑˚±‡¬Î◊™ªª
///
/// @author leiyz
/// @version 1.0
/// @date 20180209
//=======================================================================================
class CLYCodeCvt
{
public:
    CLYCodeCvt();
    ~CLYCodeCvt();

private:
    CLYCodeCvt& operator=(const CLYCodeCvt&) {return *this;}
    CLYCodeCvt(const CLYCodeCvt&) {}

public:
    static std::string UnicodeToUTF8(const std::wstring & wstr);
   
    static std::wstring UTF8ToUnicode(const std::string & str);
    
    static std::string UnicodeToANSI(const std::wstring & wstr);
    
    static std::wstring ANSIToUnicode(const std::string & str);
   
    static std::string UTF8ToANSI(const std::string & str);

    static std::string ANSIToUTF8(const std::string & str);

//    static bool Base64Encode(const std::string & refInput, std::string&  refOuput);

//    static bool Base64Decode(const std::string & refInput, std::string&  refOuput);

//    static int DeescapeURL(const std::string &strURL, char* pOutBuffer);

//    static std::string EscapeURL(const std::string &strURL);

    // ◊™ªØ Æ¡˘Ω¯÷∆µΩ ÆΩ¯÷∆
//    static int Hex2Dec(std::string& strValue);

    // ≈–∂œ◊÷∑˚¥Æ «∑Ò «GBK±‡¬Î
//    static bool IsGBK(const char* pString, const int nLen);

    //≈–∂œ◊÷∑˚¥Æ «∑Ò «UTF8±‡¬Î
//    static bool IsUtf8(const char* pStr, const int nLen);

//    static unsigned char HexToByte(const unsigned int chHexChar);

//    static char Dec2HexChar(const int n);

//    static int HexChar2Dec(char chCur);

    // ◊™ªª∂˛Ω¯÷∆µΩ ÆΩ¯÷∆
//    static int ConvertBinToDec(const std::string& refValue);

//    static int EncodeQuoted(const std::string& refSrc, char* pDst);

//    static int DecodeQuoted(const char* pSrc, const int nSrcLen, std::string& refOut);

    // Ω‚¬ÎMime” º˛Õ∑µƒ◊÷∂Œ
//    static const char* MimeDecodeMailHeaderField(const char *pSrc, const int nLen, std::string& refRet);

};

#endif

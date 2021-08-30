#pragma once

#include <string>

//=======================================================================================
/// @file  common.h
/// @brief 字符编码转换
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

	static bool Base64Encode(const std::string & refInput, std::string&  refOuput);

	static bool Base64Decode(const std::string & refInput, std::string&  refOuput);

	static int DeescapeURL(const std::string &strURL, char* pOutBuffer);

	static std::string EscapeURL(const std::string &strURL);

	// 转化十六进制到十进制
	static int Hex2Dec(std::string& strValue);

	// 判断字符串是否是GBK编码
	static bool IsGBK(const char* pString, const int nLen);

	//判断字符串是否是UTF8编码
	static bool IsUtf8(const char* pStr, const int nLen);

	static unsigned char HexToByte(const unsigned int chHexChar);

	static char Dec2HexChar(const int n);

	static int HexChar2Dec(char chCur);

	// 转换二进制到十进制
	static int ConvertBinToDec(const std::string& refValue);

	static int EncodeQuoted(const std::string& refSrc, char* pDst);	

	static int DecodeQuoted(const char* pSrc, const int nSrcLen, std::string& refOut);

	// 解码Mime邮件头的字段
	static const char* MimeDecodeMailHeaderField(const char *pSrc, const int nLen, std::string& refRet);

#ifndef  WIN32
	static std::string gb2312_to_utf8(std::string const &strGb2312);

	static std::string utf8_to_gb2312(std::string const &strUtf8);
#endif
};


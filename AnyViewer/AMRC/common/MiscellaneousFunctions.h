/******************************************************************************
* 
* SaxListCtrl
* Version
* Copyright (C) 2007 Ax3soft Co., Ltd.
* All Rights Reserved
* $Header: /cvsdata/vc/Common/GeneralFunctions.h,v 1.17 2014/12/13 01:42:27 administrator Exp $
* 
*******************************************************************************
* 
* Description:General functions define,Dependent MFC library
******************************************************************************/
#pragma once
#ifndef WIN32
//#include "public.h"
//#include "Typedef.h"
//#include "cstring.h"
#endif // !1

#include "OutBuffer.h"

#include <string>
#include <vector>
#include <assert.h>
#include <regex>
#include <stdint.h>

#ifndef BYTE
typedef unsigned char       BYTE;
#endif


typedef struct 
{
	unsigned int    nValue;
	const char*     lpDesc;
}ID2DESC;




#define DECLARE_REF_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
public: \
	TYPE Get##PROPERTY_NAME() const\
	{ \
		return MEMBER_NAME; \
	} \
	
#define DECLARE_REF_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
public: \
	void Set##PROPERTY_NAME(const TYPE& nVal) \
	{ \
		MEMBER_NAME = nVal; \
	} \

#define DECLARE_REF_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_REF_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_REF_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \

#define DECLARE_REF_MEMBER_AND_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
protected: \
	TYPE MEMBER_NAME; \
DECLARE_REF_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_REF_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \

#define DECLARE_REF_MEMBER_AND_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
protected: \
	TYPE MEMBER_NAME; \
DECLARE_REF_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \

#define DECLARE_REF_MEMBER_AND_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
protected: \
	TYPE MEMBER_NAME; \
DECLARE_REF_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \



#ifndef DECLARE_GET_METHOD
#define DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
public: \
	TYPE Get##PROPERTY_NAME() const\
	{ \
		return MEMBER_NAME; \
	} \

#endif
	
#define DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
public: \
	void Set##PROPERTY_NAME(TYPE nVal) \
	{ \
		MEMBER_NAME = nVal; \
	} \

#define DECLARE_SET_METHOD_EX(TYPE, MEMBER_NAME, PROPERTY_NAME,MODIFYING) \
public: \
	void Set##PROPERTY_NAME(TYPE nVal) \
	{ \
	if(MEMBER_NAME != nVal) { MODIFYING = true;} \
		MEMBER_NAME = nVal; \
	} \

#define DECLARE_PROPERTY_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
protected: \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \

#define DECLARE_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
protected: \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \

#define DECLARE_MEMBER_AND_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
protected: \
	TYPE MEMBER_NAME; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \




#if(_MSC_VER >= 1700  || __GNUC__ > 3)

#define DECLARE_MEMBER_AND_METHOD_V11(TYPE, MEMBER_NAME, PROPERTY_NAME,DEFAULT_VALUE) \
protected: \
	TYPE MEMBER_NAME {DEFAULT_VALUE}; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \

#define DECLARE_MEMBER_AND_GET_METHOD_V11(TYPE, MEMBER_NAME, PROPERTY_NAME,DEFAULT_VALUE) \
protected: \
	TYPE MEMBER_NAME {DEFAULT_VALUE}; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \

#define DECLARE_MEMBER_AND_SET_METHOD_V11(TYPE, MEMBER_NAME, PROPERTY_NAME,DEFAULT_VALUE) \
protected: \
	TYPE MEMBER_NAME {DEFAULT_VALUE}; \
DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \


#define DECLARE_MEMBER_AND_METHOD11_EX(TYPE, MEMBER_NAME, PROPERTY_NAME,DEFAULT_VALUE,MODIFYING) \
protected: \
	TYPE MEMBER_NAME; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_SET_METHOD_EX(TYPE, MEMBER_NAME, PROPERTY_NAME,MODIFYING) \

#else
#define DECLARE_MEMBER_AND_METHOD_V11(TYPE, MEMBER_NAME, PROPERTY_NAME,DEFAULT_VALUE) \
protected: \
	TYPE MEMBER_NAME; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \

#define DECLARE_MEMBER_AND_GET_METHOD_V11(TYPE, MEMBER_NAME, PROPERTY_NAME,DEFAULT_VALUE) \
protected: \
	TYPE MEMBER_NAME; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \

#define DECLARE_MEMBER_AND_SET_METHOD_V11(TYPE, MEMBER_NAME, PROPERTY_NAME,DEFAULT_VALUE) \
protected: \
	TYPE MEMBER_NAME; \
DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \


#define DECLARE_MEMBER_AND_METHOD11_EX(TYPE, MEMBER_NAME, PROPERTY_NAME,DEFAULT_VALUE,MODIFYING) \
protected: \
	TYPE MEMBER_NAME; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_SET_METHOD_EX(TYPE, MEMBER_NAME, PROPERTY_NAME,MODIFYING) \

#endif

#define DECLARE_MEMBER_AND_METHOD_EX(TYPE, MEMBER_NAME, PROPERTY_NAME,MODIFYING) \
protected: \
	TYPE MEMBER_NAME; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_SET_METHOD_EX(TYPE, MEMBER_NAME, PROPERTY_NAME,MODIFYING) \

#define DECLARE_MEMBER_AND_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
protected: \
	TYPE MEMBER_NAME; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME)

#define DECLARE_MEMBER_AND_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
protected: \
	TYPE MEMBER_NAME; \
DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME)


#define DECLARE_VIRTUAL_GET_METHOD(TYPE, PROPERTY_NAME) \
public: \
	virtual TYPE Get##PROPERTY_NAME() const = 0;

#define DECLARE_VIRTUAL_SET_METHOD(TYPE, PROPERTY_NAME) \
public: \
	virtual void Set##PROPERTY_NAME(TYPE nVal) =0;

#define DECLARE_VIRTUAL_REF_SET_METHOD(TYPE, PROPERTY_NAME) \
public: \
	virtual void Set##PROPERTY_NAME(const TYPE& nVal) =0;

#define DECLARE_VIRTUAL_METHOD(TYPE, PROPERTY_NAME) \
protected: \
DECLARE_VIRTUAL_GET_METHOD(TYPE, PROPERTY_NAME) \
DECLARE_VIRTUAL_SET_METHOD(TYPE, PROPERTY_NAME) \

#define DECLARE_VIRTUAL_REF_METHOD(TYPE, PROPERTY_NAME) \
protected: \
DECLARE_VIRTUAL_GET_METHOD(TYPE, PROPERTY_NAME) \
DECLARE_VIRTUAL_REF_SET_METHOD(TYPE, PROPERTY_NAME) \



#ifndef custom_max
#define custom_max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef custom_min
#define custom_min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define CHECK_POINTER(POINTER) \
assert(nullptr != POINTER); \
if(nullptr == POINTER) \
{ return;}

#define CHECK_POINTER_EX(POINTER,RESULT) \
assert(nullptr != POINTER); \
if(nullptr == POINTER) \
{ return RESULT;}


#ifndef WIN32
#define _T(x)     x
#define _s(x)     x
#endif

#ifndef string_list
typedef std::vector<std::string> string_list;
#endif


inline uint64_t NTOHQ(uint64_t nData)
{
	return ((uint64_t)*((const BYTE *)(&nData))   << 56 |  
		(uint64_t)*((const BYTE *)(&nData)+1)     << 48 |  
		(uint64_t)*((const BYTE *)(&nData)+2)     << 40 |   
		(uint64_t)*((const BYTE *)(&nData)+3)     << 32 | 
		(uint64_t)*((const BYTE *)(&nData)+4)     << 24 |  
		(uint64_t)*((const BYTE *)(&nData)+5)     << 16 |  
		(uint64_t)*((const BYTE *)(&nData)+6)     << 8  |   
		(uint64_t)*((const BYTE *)(&nData)+7));
}

//inline uint32_t ntohl(uint32_t nData)
//{
//    return ((uint32_t)*((const BYTE *)(&nData)) << 24 |
//        (uint32_t)*((const BYTE *)(&nData) + 1) << 16 |
//        (uint32_t)*((const BYTE *)(&nData) + 2) << 8 |
//        (uint32_t)*((const BYTE *)(&nData) + 3));
//}
//
//
//
//inline unsigned int HTONL(uint64_t nData)
//{
//	return ((unsigned int)*((const BYTE *)(&nData)) << 24 |
//		(unsigned int)*((const BYTE *)(&nData) + 1) << 16 |
//		(unsigned int)*((const BYTE *)(&nData) + 2) << 8 |
//		(unsigned int)*((const BYTE *)(&nData) + 3));
//}
//
//inline uint16_t NTOHS(uint16_t nData)
//{
//	return ((uint16_t)((uint16_t)*((const BYTE *)(&nData)) << 8 |
//		(uint16_t)*((const BYTE *)(&nData) + 1)));
//}
//
//inline uint16_t HTONS(uint16_t nData)
//{
//	return ((uint16_t)((uint16_t)*((const BYTE *)(&nData)) << 8 |
//		(uint16_t)*((const BYTE *)(&nData) + 1)));
//}


inline unsigned int HTON3B(unsigned int nData)
{
	return ((unsigned int)*((const BYTE *)(&nData)) << 16 |
		(unsigned int)*((const BYTE *)(&nData) + 1) << 8 |
		(unsigned int)*((const BYTE *)(&nData) + 2));
}

inline unsigned int NTOH3B(unsigned int nData)
{
	return ((unsigned int)*((const BYTE *)(&nData)) << 16 |
		(unsigned int)*((const BYTE *)(&nData) + 1) << 8 |
		(unsigned int)*((const BYTE *)(&nData) + 2));
}


extern BYTE ConverHexCharToByte(BYTE chHexChar);

extern int ConvertHexTextToBin(const std::string& strSource, char* pOut, const int nLen);

extern void ConvertBinToHexText(const unsigned char* pInBuffer, const int nLen, std::string& strOut);

// 判断字符串是否是数字
extern bool IsDigits(const std::string& strNum);



extern void CreateSN(int nParts, std::string& strSN);

// 按回车换行分割字符串
extern int SplitString(const std::string& strSrc, string_list& refRet);

inline unsigned int Sax_tolower(unsigned int chChar)
{
	return (chChar >= 65 && chChar <= 90) ? chChar += 32 : chChar;
}

inline unsigned int Sax_toupper(unsigned int chChar)
{
	return (chChar >= 97 && chChar <= 122) ? chChar -= 32 : chChar;
}

// 从sql脚本文件中提取sql命令
extern int ExtractSqlStatement(const std::string& strFile, string_list& refSqlStatement);

// 判断字符串中是否存在通配符"*"和"?"
extern bool ExistsWildcard(const std::string& strString);

extern unsigned int IDE_IPv4_to_i(const std::string& strIpAddress);

// 校验MAC地址是否合法
extern bool ValidMAC(const char* pMAC);

// 校验IP地址是否合法
extern bool ValidIP(const char* pIP);

// 校验邮件地址是否合法
extern bool ValidEmail(const std::string& refEmail);

#ifdef WIN32
#ifdef ENABLE_MFC

extern LPCTSTR FormatMacAddress(BYTE* pMac, TCHAR szRet[]);

extern LPCTSTR FormatNetIPAddress(unsigned int nIP, TCHAR szRet[]);

extern LPCTSTR FormatHostIPAddress(unsigned int nIP, TCHAR szRet[]);

extern LPCTSTR FormatIPv6Address(uint16_t* pIPAddr, TCHAR szRet[]);

extern unsigned int IDE_IPv4_to_i(CString& strIpAddress);

extern unsigned int IDE_IPv4_to_i_A(CStringA& strIpAddress);

extern bool IDE_IPv6_to_i(CString& strMacAddress,  uint16_t pIPAddr[]);

extern uint64_t IDE_MAC_to_i(CString& strMacAddress);
#endif

#else

#ifdef ENABLE_AIM_STRING

extern unsigned int IDE_IPv4_to_i(AIM::CString& strIpAddress);

extern bool IDE_IPv6_to_i(AIM::CString& strMacAddress, unsigned int pIPAddr[]);

extern void FormatTimeUMillisecond(int64_t nTime, AIM::CString& strResult);

extern void FormatTimeUSecond(int64_t nTime, AIM::CString& strResult);

extern void FormatNumber(int64_t nNumber, AIM::CString& strResult);

extern AIM::CString FormatByte(int64_t nBytes, AIM::CString& strResult);

extern void FormatBit(int64_t nBytes, AIM::CString& strResult);

extern void FormatBytePerSecond(int64_t nBytes, AIM::CString& strResult);

extern void FormatBitPerSecond(int64_t nBytes, AIM::CString& strResult);

extern unsigned int IDE_IPv4_to_i_A(const std::string& strIpAddress);
#endif

#endif

extern uint64_t IDE_MAC_to_i(const char* lpMacAddress);

extern char* FormatMacAddressAnsi(const BYTE* pMac, char szRet[]);
extern char* FormatNetIPAddressAnsi(unsigned int nIP, char szRet[]);
extern char* FormatHostIPAddressAnsi(unsigned int nIP, char szRet[]);
extern char* FormatIPv6AddressAnsi(unsigned int* pIPAddr, char szRet[]);

extern int GetOUICode(BYTE* pMacAddr);

/// 替换字符串
extern void StrReplace(std::string & strString, const std::string & strSrc, const std::string &strDst);



// 不分大小写查找
extern char* Ax3_stristr_ansi(const char *str1, const char *str2);

// 描述：内存对齐算法
extern unsigned int CalcAlign(unsigned int n, unsigned align);

// 查询指定域名的IP地址
extern std::string QueryDomainIP(const std::string& strDomain);

// 查询本地ip地址
extern size_t QueryLocalIP(string_list& refIPs);

template<class T>
void SwitchVarTmpl(T& var1, T& var2)
{
	T varTmp = var1;
	var1 = var2;
	var2 = varTmp;
}

// 替换字符串
extern std::string& ReplaceString(std::string& strSrc, const std::string& strOld, const std::string& strNew);

// 查找字符串串，成功返回开始位置
extern int strupos(const char *pSource, const char *sPat);

// 查找字符串串，成功返回开始位置
extern int striupos(const char *pSource, const char *sPat);

// 构造Sunday查询算法的移动表
extern void ConstructshitTable(int* pShitTable, const int nTableSize, const char* pPattern, int nPatternLen = -1);

// 得到类型描述
extern const char* GetTypeDesc(const ID2DESC* pValuDescMap, const unsigned int nType);

extern std::string& Trim(std::string &s);

// 从磁盘读取文件
extern int ReadFile(const char* lpFile, COutBuffer& refOut);

// 统计asc码字符的个数
extern int SumAscCount(const std::wstring& refStr);



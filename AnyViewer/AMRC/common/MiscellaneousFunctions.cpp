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
#include "stdafx.h"
#include "MiscellaneousFunctions.h"
#include "LYCodeCvt.h"

#ifndef DISENABLE_BOOT

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>
#include "boost/asio.hpp" 
#include "boost/shared_ptr.hpp" 
//#include "boost/thread.hpp" 
#include <boost/lexical_cast.hpp>//使用字符串转换功能 

#endif

#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <fstream>

using namespace std;
using namespace boost::archive::iterators;

BYTE ConverHexCharToByte(BYTE chHexChar)
{
	const static BYTE nValueTable[123] =
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,0,0
	,0,0,0,0,0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0
	,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,11,12
	,13,14,15 };

	if (chHexChar >= 48 && chHexChar <= 123)
	{
		return nValueTable[chHexChar];
	}

	return 0;
}



//--------------------------------------------------------------------------------
/// <summary>
/// 转换16进制的文本为二进制流
/// </summary>
/// <param name="strSource">需要转换的字符串</param>
/// <param name="pOut">输出缓冲区</param>
/// <param name="nLen">输出缓冲区长度</param>
/// <returns>输出数据的长度</returns>
/// <created>Andy,2019/9/26</created>
/// <changed>Andy,2019/9/26</changed>
//--------------------------------------------------------------------------------
int ConvertHexTextToBin(const std::string& strSource, char* pOut, const int nLen)
{
	const int nStrLen = custom_min(int(strSource.size()), nLen * 2);  //确保不能越界

	for (int i = 0; i < nStrLen;)
	{
		*pOut++ = ConverHexCharToByte((BYTE)strSource.at(i)) * 16
			+ ConverHexCharToByte((BYTE)strSource.at(i + 1));

		i += 2;
	}

	return (nStrLen / 2);
}


static const char chHexTable[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
#define TOHEX(a, b) {*b++ = chHexTable[a >> 4]; *b++ = chHexTable[a & 0xf];}

// --------------------------------------------------------------------------------
/// <summary>
/// 转换二进制数据到字符串
/// </summary>
/// <param name="pInBuffer">需要被转换的数据</param>
/// <param name="nLen">被转换数据长度</param>
/// <param name="strOut">存储转换结果</param>
/// <created>黄丽云,2019/2/28</created>
/// <changed>黄丽云,2019/2/28</changed>
// --------------------------------------------------------------------------------
void ConvertBinToHexText(const unsigned char* pInBuffer, const int nLen, std::string& strOut)
{
	strOut.clear();

	if (nullptr != pInBuffer)
	{
		for (int i = 0; i < nLen; i++)
		{
			strOut += chHexTable[*pInBuffer >> 4];
			strOut += chHexTable[*pInBuffer & 0x0f];
			pInBuffer++;
		}
	}
}





void CreateSN(int nParts, std::string& strSN)
{
	char LETTER_TABLE[] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	strSN.clear();
	std::string strPart;

	srand((unsigned int)time(nullptr));

	for (int i = 0; i < nParts; i++)
	{
		strPart.clear();

		for (int j = 0; j < 5; j++)
		{
			int nIndex = rand() % 62;
			strPart += LETTER_TABLE[nIndex];
		}
		if (0 == i)
		{
			strSN = strPart;
		}
		else
		{
			strSN += "-";
			strSN += strPart;
		}
	}
}

//--------------------------------------------------------------------------------
/// <summary>
/// 按回车换行分割字符串
/// </summary>
/// <param name="strSrc">被分割的字符串</param>
/// <param name="refRet">存储分割结果</param>
/// <returns>成功分割的个数</returns>
/// <created>Andy,2019/10/24</created>
/// <changed>Andy,2019/10/24</changed>
//--------------------------------------------------------------------------------
int SplitString(const std::string & strSrc, string_list & refRet)
{
	refRet.clear();

	std::string strSingle;
	size_t STR_LEN = strSrc.size();

	for (size_t i = 0; i < STR_LEN; i++)
	{
		const char chCur = strSrc[i];

		if (i + 1 == STR_LEN)
		{
			if (chCur != '\n' && chCur != '\r')
			{
				strSingle += chCur;
			}

			if (strSingle.size() > 0)
			{
				refRet.push_back(strSingle);
				strSingle.clear();
			}
		}
		else if (chCur == '\n' || chCur == '\r')
		{
			if (strSingle.size() > 0)
			{
				refRet.push_back(strSingle);
				strSingle.clear();
			}
		}
		else
		{
			strSingle += chCur;
		}
	}

	return refRet.size();
}

//--------------------------------------------------------------------------------
/// <summary>
/// 从sql脚本文件中提取sql命令
/// </summary>
/// <param name="strFile">sql脚本文件</param>
/// <param name="refSqlStatement">存放提取的命令</param>
/// <returns>命令个数</returns>
/// <created>Andy,2020/6/5</created>
/// <changed>Andy,2020/6/5</changed>
//--------------------------------------------------------------------------------
int ExtractSqlStatement(const std::string & strFile, string_list & refSqlStatement)
{
	std::ifstream objFileRead(strFile, std::ios::in);

	enum ANALYSIS_STATUS
	{
		AS_INIT = 0,
		AS_CMD  = 1,
	};


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


				ANALYSIS_STATUS nStatus = AS_INIT;
				std::string strCMD;
				std::string strSqlStatement;

				for (int i = 0; i < (int)ps; i++)
				{
					const char chCur = lpBuffer[i];

					switch (nStatus)
					{
					case AS_INIT:
					{
						if (chCur == ' ' || chCur == '\r' || chCur == '\n')
						{
							if (chCur == ' ' && (strCMD == "SET"
								|| strCMD == "DROP"
								|| strCMD == "CREATE"
								|| strCMD == "INSERT"))
							{
								strSqlStatement = strCMD;
								strSqlStatement += chCur;
								nStatus = AS_CMD;
							}
							else
							{
								strCMD.clear();
							}
						}
						else
						{
							strCMD += chCur;
						}
					}
					break;

					case AS_CMD:
					{
						if (chCur == ';')
						{
							refSqlStatement.push_back(strSqlStatement);
							nStatus = AS_INIT;
						}
						else
						{
							strSqlStatement += chCur;
						}
					}
					break;

					default:
						break;
					}

				}				

				delete[]lpBuffer;
			}

			objFileRead.close();
		}
	}
	catch (...)
	{
		//TRACE(("Fail to load the file (%s)\r\n"), static_cast<char*>(strName));
	}

	return 0;
}

///----------------------------------------------------------------------------
///@brief 判断是否都为数字   
///
///@param
///@return
///@author :Leiyz
///@date 20180905
///----------------------------------------------------------------------------
bool IsDigits(const std::string& strNum)
{
	std::regex rgx("[0-9]+");
	return std::regex_match(strNum.begin(), strNum.end(), rgx);
}



//--------------------------------------------------------------------------------
/// <summary>
/// 判断字符串中是否存在通配符"*"和"?"
/// </summary>
/// <param name="strString">被检测的字符串</param>
/// <returns>true:存在,false:不存在</returns>
/// <created>Andy,2020/6/6</created>
/// <changed>Andy,2020/6/6</changed>
//--------------------------------------------------------------------------------
bool ExistsWildcard(const std::string& strString)
{
	bool bResult = false;
	size_t STR_LEN = strString.size();

	for (size_t i = 0; i < STR_LEN; i++)
	{
		const char chCur = strString[i];

		if (chCur == '*' || chCur == '?')
		{
			bResult = true;
			break;
		}
	}

	return bResult;
}











//--------------------------------------------------------------------------------
/// <summary>
/// 校验IP地址是否合法
/// </summary>
/// <param name="strIP"></param>
/// <returns></returns>
/// <created>Andy,2019/10/16</created>
/// <changed>Andy,2019/10/16</changed>
//--------------------------------------------------------------------------------
bool ValidIP(const char* pIP)
{
#ifndef DISENABLE_BOOT
	using namespace boost::xpressive;

	/* 定义正则表达式 */
	cregex reg_ip = cregex::compile("(25[0-4]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[1-9])[.](25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[.](25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[.](25[0-4]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])");

	return 	regex_match(pIP, reg_ip);
#else
	std::regex rgx("((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\\.){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])");
	return std::regex_match(strIP.begin(), strIP.end(), rgx);
#endif // ! #ifndef DISENABLE_BOOT
}

// ********************************************************************************
/// <summary>
/// 校验邮件地址是否合法
/// </summary>
/// <param name="pEmail"></param>
/// <returns></returns>
/// <created>Andy,2021/3/22</created>
/// <changed>Andy,2021/3/22</changed>
// ********************************************************************************
extern bool ValidEmail(const std::string& refEmail)
{
	std::regex objPattern("([0-9A-Za-z\\-_\\.]+)@([0-9A-Za-z]+\\.[A-Za-z]{2,3}(\\.[A-Za-z]{2})?)");

	return std::regex_match(refEmail, objPattern);
}

unsigned int IDE_IPv4_to_i(const std::string & strIpAddress)
{
	const unsigned int LENGTH = strIpAddress.size();
	BYTE chIpAddr[4] = { 0 };
	int nCount = 3;
	unsigned int nCurChar = 0;
	unsigned int i = 0;
	BYTE nSegmentVal = 0;

	while (i < LENGTH)
	{
		nCurChar = strIpAddress[i];

		if (46/*'.'*/ != nCurChar)
		{
			nSegmentVal *= 10;
			nSegmentVal += CLYCodeCvt::HexToByte(nCurChar); 
		}
		else
		{
			chIpAddr[nCount--] = nSegmentVal;
			nSegmentVal = 0;
			if (-1 == nCount)
			{
				break;
			}
		}
		i++;
	}

	chIpAddr[nCount] = nSegmentVal;

	if (i == LENGTH && 0 == nCount)
	{
		//ATLTRACE(("%s\n"),FormatNetIPAddressAnsi(*(unsigned int*)chIpAddr));
		return *(unsigned int*)chIpAddr;
	}

	return 0;
}

const BYTE HEX_CHAR_TABLE[] = { 48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70 };


#ifdef WIN32
LPCTSTR FormatMacAddress(BYTE* pMac, TCHAR szRet[])
{
	assert(nullptr != pMac);

	if (nullptr != pMac)
	{

		//StringCbPrintf(szRet,40,("%02X:%02X:%02X:%02X:%02X:%02X")  
		//	, *((const BYTE *)(pMac))
		//	, *((const BYTE *)(pMac + 1))
		//	, *((const BYTE *)(pMac + 2))
		//	, *((const BYTE *)(pMac + 3))
		//	, *((const BYTE *)(pMac + 4))
		//	, *((const BYTE *)(pMac + 5)));	
		//下面是优化算法，比上面的方法快几十倍
		szRet[0] = HEX_CHAR_TABLE[pMac[0] >> 4];
		szRet[1] = HEX_CHAR_TABLE[pMac[0] & 0xF];
		szRet[2] = ':';

		szRet[3] = HEX_CHAR_TABLE[pMac[1] >> 4];
		szRet[4] = HEX_CHAR_TABLE[pMac[1] & 0xF];
		szRet[5] = ':';

		szRet[6] = HEX_CHAR_TABLE[pMac[2] >> 4];
		szRet[7] = HEX_CHAR_TABLE[pMac[2] & 0xF];
		szRet[8] = ':';

		szRet[9] = HEX_CHAR_TABLE[pMac[3] >> 4];
		szRet[10] = HEX_CHAR_TABLE[pMac[3] & 0xF];
		szRet[11] = ':';

		szRet[12] = HEX_CHAR_TABLE[pMac[4] >> 4];
		szRet[13] = HEX_CHAR_TABLE[pMac[4] & 0xF];
		szRet[14] = ':';

		szRet[15] = HEX_CHAR_TABLE[pMac[5] >> 4];
		szRet[16] = HEX_CHAR_TABLE[pMac[5] & 0xF];
	}

	return szRet;
}
#endif


char* FormatMacAddressAnsi(const BYTE* pMac, char szRet[])
{
	assert(nullptr != pMac);

	if (nullptr != pMac)
	{

		//StringCbPrintf(szRet,40,("%02X:%02X:%02X:%02X:%02X:%02X")  
		//	, *((const BYTE *)(pMac))
		//	, *((const BYTE *)(pMac + 1))
		//	, *((const BYTE *)(pMac + 2))
		//	, *((const BYTE *)(pMac + 3))
		//	, *((const BYTE *)(pMac + 4))
		//	, *((const BYTE *)(pMac + 5)));	
		//下面是优化算法，比上面的方法快几十倍
		szRet[0] = HEX_CHAR_TABLE[pMac[0] >> 4];
		szRet[1] = HEX_CHAR_TABLE[pMac[0] & 0xF];
		szRet[2] = ':';

		szRet[3] = HEX_CHAR_TABLE[pMac[1] >> 4];
		szRet[4] = HEX_CHAR_TABLE[pMac[1] & 0xF];
		szRet[5] = ':';

		szRet[6] = HEX_CHAR_TABLE[pMac[2] >> 4];
		szRet[7] = HEX_CHAR_TABLE[pMac[2] & 0xF];
		szRet[8] = ':';

		szRet[9] = HEX_CHAR_TABLE[pMac[3] >> 4];
		szRet[10] = HEX_CHAR_TABLE[pMac[3] & 0xF];
		szRet[11] = ':';

		szRet[12] = HEX_CHAR_TABLE[pMac[4] >> 4];
		szRet[13] = HEX_CHAR_TABLE[pMac[4] & 0xF];
		szRet[14] = ':';

		szRet[15] = HEX_CHAR_TABLE[pMac[5] >> 4];
		szRet[16] = HEX_CHAR_TABLE[pMac[5] & 0xF];
	}

	return szRet;
}

#ifdef WIN32
LPCTSTR FormatIPv6Address(uint16_t* pIPAddr, TCHAR szRet[])
{
	assert(nullptr != pIPAddr);
	if (nullptr != pIPAddr)
	{
		bool bCompression = false;
		uint16_t nTmp = 0;
		int nIndex = 0;

		for (int i = 0; i < 8; i++)
		{
			nTmp = pIPAddr[i];
			if (0 == nTmp && !bCompression)
			{
				int j = i + 1;
				while (j < 8)
				{
					if (0 != pIPAddr[j])
					{
						break;
					}
					j++;
				}

				if (j > i + 1)   //说明有压缩
				{
					if (0 == i)
					{
						szRet[nIndex++] = ':';
					}
					szRet[nIndex++] = ':';
					if (8 == j)
					{
						break;
					}
					i = j;
					nTmp = pIPAddr[i];

					bCompression = true;
				}
			}

			//将每一段的前导零省略，但是每一段都至少应该有一个数字
			nTmp = ntohs(nTmp);
			if (nTmp >> 12)
			{
				szRet[nIndex++] = HEX_CHAR_TABLE[nTmp >> 12];
			}
			if (nTmp >> 8)
			{
				szRet[nIndex++] = HEX_CHAR_TABLE[(nTmp & 0xF00) >> 8];
			}
			if (nTmp >> 4)
			{
				szRet[nIndex++] = HEX_CHAR_TABLE[(nTmp & 0xF0) >> 4];
			}
			szRet[nIndex++] = HEX_CHAR_TABLE[nTmp & 0x0F];
			if (i < 7)
			{
				szRet[nIndex++] = ':';
			}
		}
		szRet[nIndex++] = 0;
	}

	return szRet;
}

#endif





//--------------------------------------------------------------------------------
/// <summary>
/// 校验MAC地址是否合法
/// </summary>
/// <param name="pMAC">被校验的MAC地址</param>
/// <returns></returns>
/// <created>Andy,2019/10/23</created>
/// <changed>Andy,2019/10/23</changed>
//--------------------------------------------------------------------------------
bool ValidMAC(const char * pMAC)
{
#ifndef DISENABLE_BOOT
	using namespace boost::xpressive;

	/* 定义正则表达式 */
	cregex reg_mac = cregex::compile("(([A-F0-9]{2}:)|([A-F0-9]{2}-)){5}[A-F0-9]{2}");

	return 	regex_match(pMAC, reg_mac);
#else
	return 	false;
#endif // ! #ifndef DISENABLE_BOOT
}

#ifndef WIN32
/*-----------------------------------------------------------------
|  函数名称  : IDE_IPv4_to_i
|  描    述  : 转换IP地址到整型
|  参    数  : AIM::CString& strIpAddress——
|  返 回 值  : 主机字节序
|  修改记录  : 2007-10-27 16:38:57   -huangdy-   创建
-----------------------------------------------------------------*/
unsigned int IDE_IPv4_to_i_A(AIM::CString& strIpAddress)
{
	unsigned int LENGTH       = strIpAddress.GetLength();
	BYTE chIpAddr[4]  = {0};
	int nCount        = 3;
	unsigned int nCurChar     = 0;
	unsigned int i            = 0;
	BYTE nSegmentVal  = 0;
	while(i < LENGTH)
	{
		nCurChar = strIpAddress.GetAt(i);

		if(46/*'.'*/ != nCurChar)
		{
			nSegmentVal *= 10;
			nSegmentVal += CLYCodeCvt::HexToByte((BYTE)nCurChar);	
		}
		else
		{
			chIpAddr[nCount--] = nSegmentVal;
			nSegmentVal = 0;
			if(-1 == nCount)
			{
				break;
			}
		}
		i++;
	}

	chIpAddr[nCount] = nSegmentVal;

	if(i == LENGTH && 0 == nCount)
	{
		//ATLTRACE(("%s\n"),FormatNetIPAddressAnsi(*(unsigned int*)chIpAddr));
		return *(unsigned int*)chIpAddr;
	}	

	return 0;
}
#endif




#ifndef WIN32

/*-----------------------------------------------------------------
|  函数名称  : IDE_IPv4_to_i
|  描    述  : 转换IP地址到整型
|  参    数  : AIM::CString& strIpAddress——
|  返 回 值  : 主机字节序
|  修改记录  : 2007-10-27 16:38:57   -huangdy-   创建
-----------------------------------------------------------------*/
unsigned int IDE_IPv4_to_i(AIM::CString& strIpAddress)
{
	unsigned int LENGTH = strIpAddress.GetLength();
	BYTE chIpAddr[4] = { 0 };
	int nCount = 3;
	unsigned int nCurChar = 0;
	unsigned int i = 0;
	BYTE nSegmentVal = 0;
	while (i < LENGTH)
	{
		nCurChar = strIpAddress.GetAt(i);

		if (46/*'.'*/ != nCurChar)
		{
			nSegmentVal *= 10;
			nSegmentVal += CLYCodeCvt::HexToByte((BYTE)nCurChar);
		}
		else
		{
			chIpAddr[nCount--] = nSegmentVal;
			nSegmentVal = 0;
			if (-1 == nCount)
			{
				break;
			}
		}
		i++;
	}

	chIpAddr[nCount] = nSegmentVal;

	if (i == LENGTH && 0 == nCount)
	{
		//ATLTRACE(("%s\n"),FormatHostIPAddress(*(unsigned int*)chIpAddr));
		return *(unsigned int*)chIpAddr;
	}

	return 0;
}

/*-----------------------------------------------------------------
|  函数名称  : IDE_IPv4_to_i
|  描    述  : 转换IP地址到整型
|  参    数  : CString& strIpAddress——
|  返 回 值  : 主机字节序
|  修改记录  : 2007-10-27 16:38:57   -huangdy-   创建
-----------------------------------------------------------------*/
unsigned int IDE_IPv4_to_i_A(const std::string& strIpAddress)
{
	unsigned int LENGTH = strIpAddress.size();
	BYTE chIpAddr[4] = { 0 };
	int nCount = 3;
	unsigned int nCurChar = 0;
	unsigned int i = 0;
	BYTE nSegmentVal = 0;

	while (i < LENGTH)
	{
		nCurChar = strIpAddress[i];

		if (46/*'.'*/ != nCurChar)
		{
			nSegmentVal *= 10;
			nSegmentVal += CLYCodeCvt::HexToByte((BYTE)nCurChar);
		}
		else
		{
			chIpAddr[nCount--] = nSegmentVal;
			nSegmentVal = 0;
			if (-1 == nCount)
			{
				break;
			}
		}
		i++;
	}

	chIpAddr[nCount] = nSegmentVal;

	if (i == LENGTH && 0 == nCount)
	{
		//ATLTRACE(("%s\n"),FormatNetIPAddressAnsi(*(unsigned int*)chIpAddr));
		return *(unsigned int*)chIpAddr;
	}

	return 0;
}

bool IDE_IPv6_to_i(AIM::CString& strMacAddress, unsigned int pIPAddr[])
{
	memset(pIPAddr, 0, 16);

	bool nCompression = false;
	const unsigned int LEN = strMacAddress.GetLength();
	unsigned int  nIndex = 0;
	unsigned char nChar = 0;
	unsigned int  i = 0;
	unsigned int  nTmp = 0;
	for (; i < LEN; i++)
	{
		nChar = strMacAddress.GetAt(i);
		if (nChar != ':')
		{
			nTmp <<= 4;
			nTmp += CLYCodeCvt::HexToByte(nChar);
			if (i + 1 == LEN)
			{
				pIPAddr[nIndex] = ntohs(nTmp);
			}
		}
		else
		{
			pIPAddr[nIndex] = ntohs(nTmp);
			nIndex++;
			nTmp = 0;
			if (i + 1 < LEN)
			{
				if (strMacAddress.GetAt(i + 1) == ':') // 说明是有压缩
				{
					i++;
					nCompression = true;
					break;
				}
			}
		}
	}
	unsigned int nShift = 0;
	
	if (nCompression)   //如果有压缩就分为两部分转化
	{
		nIndex = 7;
		
		for (unsigned int j = LEN - 1; j >= i; j--)
		{
			nChar = strMacAddress.GetAt(j);
			if (nChar != ':')
			{
				nTmp += CLYCodeCvt::HexToByte(nChar) << nShift;
				nShift += 4;

			}
			else
			{
				pIPAddr[nIndex] = ntohs(nTmp);
				nIndex--;
				nShift = 0;
				nTmp = 0;
			}
		}
	}

	return true;
}



#endif

char* FormatNetIPAddressAnsi(unsigned int nNetIP, char szRet[])
{
	//StringCbPrintf(szRet,32,("%d.%d.%d.%d")  
	//, *((const BYTE *)(&nNetIP))
	//, *((const BYTE *)(&nNetIP) + 1)
	//, *((const BYTE *)(&nNetIP) + 2)
	//, *((const BYTE *)(&nNetIP) + 3));	

	unsigned int i = 0;
	BYTE nByte = *((const BYTE *)(&nNetIP));
	if (nByte / 100)
	{
		szRet[i++] = nByte / 100 + 48;
		nByte %= 100;

		szRet[i++] = nByte / 10 + 48;
		szRet[i++] = nByte % 10 + 48;
		szRet[i++] = '.';
	}
	else
	{
		if (nByte / 10)
		{
			szRet[i++] = nByte / 10 + 48;
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
		else
		{
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
	}

	nByte = *((const BYTE *)(&nNetIP) + 1);
	if (nByte / 100)
	{
		szRet[i++] = nByte / 100 + 48;
		nByte %= 100;

		szRet[i++] = nByte / 10 + 48;
		szRet[i++] = nByte % 10 + 48;
		szRet[i++] = '.';
	}
	else
	{
		if (nByte / 10)
		{
			szRet[i++] = nByte / 10 + 48;
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
		else
		{
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
	}

	nByte = *((const BYTE *)(&nNetIP) + 2);
	if (nByte / 100)
	{
		szRet[i++] = nByte / 100 + 48;
		nByte %= 100;

		szRet[i++] = nByte / 10 + 48;
		szRet[i++] = nByte % 10 + 48;
		szRet[i++] = '.';
	}
	else
	{
		if (nByte / 10)
		{
			szRet[i++] = nByte / 10 + 48;
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
		else
		{
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
	}

	nByte = *((const BYTE *)(&nNetIP) + 3);
	if (nByte / 100)
	{
		szRet[i++] = nByte / 100 + 48;
		nByte %= 100;

		szRet[i++] = nByte / 10 + 48;
		szRet[i++] = nByte % 10 + 48;
	}
	else
	{
		if (nByte / 10)
		{
			szRet[i++] = nByte / 10 + 48;
			szRet[i++] = nByte % 10 + 48;
		}
		else
		{
			szRet[i++] = nByte % 10 + 48;
		}
	}
	szRet[i++] = 0;

	return szRet;
}

char* FormatHostIPAddressAnsi(unsigned int nHostIP, char szRet[])
{
	//StringCbPrintf(szRet,32,("%d.%d.%d.%d") 
	//	, *((const BYTE *)(&nHostIP) + 3)
	//	, *((const BYTE *)(&nHostIP) + 2)
	//	, *((const BYTE *)(&nHostIP) + 1)
	//	, *((const BYTE *)(&nHostIP)));


	unsigned int i = 0;
	BYTE nByte = *((const BYTE *)(&nHostIP) + 3);
	if (nByte / 100)
	{
		szRet[i++] = nByte / 100 + 48;
		nByte %= 100;

		szRet[i++] = nByte / 10 + 48;
		szRet[i++] = nByte % 10 + 48;
		szRet[i++] = '.';
	}
	else
	{
		if (nByte / 10)
		{
			szRet[i++] = nByte / 10 + 48;
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
		else
		{
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
	}

	nByte = *((const BYTE *)(&nHostIP) + 2);
	if (nByte / 100)
	{
		szRet[i++] = nByte / 100 + 48;
		nByte %= 100;

		szRet[i++] = nByte / 10 + 48;
		szRet[i++] = nByte % 10 + 48;
		szRet[i++] = '.';
	}
	else
	{
		if (nByte / 10)
		{
			szRet[i++] = nByte / 10 + 48;
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
		else
		{
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
	}

	nByte = *((const BYTE *)(&nHostIP) + 1);
	if (nByte / 100)
	{
		szRet[i++] = nByte / 100 + 48;
		nByte %= 100;

		szRet[i++] = nByte / 10 + 48;
		szRet[i++] = nByte % 10 + 48;
		szRet[i++] = '.';
	}
	else
	{
		if (nByte / 10)
		{
			szRet[i++] = nByte / 10 + 48;
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
		else
		{
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
	}

	nByte = *((const BYTE *)(&nHostIP));
	if (nByte / 100)
	{
		szRet[i++] = nByte / 100 + 48;
		nByte %= 100;

		szRet[i++] = nByte / 10 + 48;
		szRet[i++] = nByte % 10 + 48;
	}
	else
	{
		if (nByte / 10)
		{
			szRet[i++] = nByte / 10 + 48;
			szRet[i++] = nByte % 10 + 48;
		}
		else
		{
			szRet[i++] = nByte % 10 + 48;
		}
	}
	szRet[i++] = 0;
	return szRet;
}

#ifdef WIN32
LPCTSTR FormatNetIPAddress(unsigned int nNetIP, TCHAR szRet[])
{
	//StringCbPrintf(szRet,32,("%d.%d.%d.%d")  
	//, *((const BYTE *)(&nNetIP))
	//, *((const BYTE *)(&nNetIP) + 1)
	//, *((const BYTE *)(&nNetIP) + 2)
	//, *((const BYTE *)(&nNetIP) + 3));	

	unsigned int i = 0;
	BYTE nByte = *((const BYTE *)(&nNetIP));
	if (nByte / 100)
	{
		szRet[i++] = nByte / 100 + 48;
		nByte %= 100;

		szRet[i++] = nByte / 10 + 48;
		szRet[i++] = nByte % 10 + 48;
		szRet[i++] = '.';
	}
	else
	{
		if (nByte / 10)
		{
			szRet[i++] = nByte / 10 + 48;
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
		else
		{
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
	}

	nByte = *((const BYTE *)(&nNetIP) + 1);
	if (nByte / 100)
	{
		szRet[i++] = nByte / 100 + 48;
		nByte %= 100;

		szRet[i++] = nByte / 10 + 48;
		szRet[i++] = nByte % 10 + 48;
		szRet[i++] = '.';
	}
	else
	{
		if (nByte / 10)
		{
			szRet[i++] = nByte / 10 + 48;
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
		else
		{
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
	}

	nByte = *((const BYTE *)(&nNetIP) + 2);
	if (nByte / 100)
	{
		szRet[i++] = nByte / 100 + 48;
		nByte %= 100;

		szRet[i++] = nByte / 10 + 48;
		szRet[i++] = nByte % 10 + 48;
		szRet[i++] = '.';
	}
	else
	{
		if (nByte / 10)
		{
			szRet[i++] = nByte / 10 + 48;
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
		else
		{
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
	}

	nByte = *((const BYTE *)(&nNetIP) + 3);
	if (nByte / 100)
	{
		szRet[i++] = nByte / 100 + 48;
		nByte %= 100;

		szRet[i++] = nByte / 10 + 48;
		szRet[i++] = nByte % 10 + 48;
	}
	else
	{
		if (nByte / 10)
		{
			szRet[i++] = nByte / 10 + 48;
			szRet[i++] = nByte % 10 + 48;
		}
		else
		{
			szRet[i++] = nByte % 10 + 48;
		}
	}
	szRet[i++] = 0;

	return szRet;
}


LPCTSTR FormatHostIPAddress(unsigned int nHostIP, TCHAR szRet[])
{
	//StringCbPrintf(szRet,32,("%d.%d.%d.%d") 
	//	, *((const BYTE *)(&nHostIP) + 3)
	//	, *((const BYTE *)(&nHostIP) + 2)
	//	, *((const BYTE *)(&nHostIP) + 1)
	//	, *((const BYTE *)(&nHostIP)));


	unsigned int i = 0;
	BYTE nByte = *((const BYTE *)(&nHostIP) + 3);
	if (nByte / 100)
	{
		szRet[i++] = nByte / 100 + 48;
		nByte %= 100;

		szRet[i++] = nByte / 10 + 48;
		szRet[i++] = nByte % 10 + 48;
		szRet[i++] = '.';
	}
	else
	{
		if (nByte / 10)
		{
			szRet[i++] = nByte / 10 + 48;
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
		else
		{
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
	}

	nByte = *((const BYTE *)(&nHostIP) + 2);
	if (nByte / 100)
	{
		szRet[i++] = nByte / 100 + 48;
		nByte %= 100;

		szRet[i++] = nByte / 10 + 48;
		szRet[i++] = nByte % 10 + 48;
		szRet[i++] = '.';
	}
	else
	{
		if (nByte / 10)
		{
			szRet[i++] = nByte / 10 + 48;
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
		else
		{
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
	}

	nByte = *((const BYTE *)(&nHostIP) + 1);
	if (nByte / 100)
	{
		szRet[i++] = nByte / 100 + 48;
		nByte %= 100;

		szRet[i++] = nByte / 10 + 48;
		szRet[i++] = nByte % 10 + 48;
		szRet[i++] = '.';
	}
	else
	{
		if (nByte / 10)
		{
			szRet[i++] = nByte / 10 + 48;
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
		else
		{
			szRet[i++] = nByte % 10 + 48;
			szRet[i++] = '.';
		}
	}

	nByte = *((const BYTE *)(&nHostIP));
	if (nByte / 100)
	{
		szRet[i++] = nByte / 100 + 48;
		nByte %= 100;

		szRet[i++] = nByte / 10 + 48;
		szRet[i++] = nByte % 10 + 48;
	}
	else
	{
		if (nByte / 10)
		{
			szRet[i++] = nByte / 10 + 48;
			szRet[i++] = nByte % 10 + 48;
		}
		else
		{
			szRet[i++] = nByte % 10 + 48;
		}
	}
	szRet[i++] = 0;

	return szRet;
}

#endif


#ifdef WIN32
#ifdef ENABLE_MFC
/*-----------------------------------------------------------------
|  函数名称  : IDE_IPv4_to_i
|  描    述  : 转换IP地址到整型
|  参    数  : CString& strIpAddress——
|  返 回 值  : 主机字节序
|  修改记录  : 2007-10-27 16:38:57   -huangdy-   创建
-----------------------------------------------------------------*/
unsigned int IDE_IPv4_to_i_A(CStringA& strIpAddress)
{
	unsigned int LENGTH = strIpAddress.GetLength();
	BYTE chIpAddr[4] = { 0 };
	int nCount = 3;
	unsigned int nCurChar = 0;
	unsigned int i = 0;
	BYTE nSegmentVal = 0;
	while (i < LENGTH)
	{
		nCurChar = strIpAddress.GetAt(i);

		if (46/*'.'*/ != nCurChar)
		{
			nSegmentVal *= 10;
			nSegmentVal += CLYCodeCvt::HexToByte((BYTE)nCurChar);
		}
		else
		{
			chIpAddr[nCount--] = nSegmentVal;
			nSegmentVal = 0;
			if (-1 == nCount)
			{
				break;
			}
		}
		i++;
	}

	chIpAddr[nCount] = nSegmentVal;

	if (i == LENGTH && 0 == nCount)
	{
		//ATLTRACE(("%s\n"),FormatNetIPAddressAnsi(*(unsigned int*)chIpAddr));
		return *(unsigned int*)chIpAddr;
	}

	return 0;
}
#endif
#endif

#ifdef WIN32
#ifdef ENABLE_MFC
/*-----------------------------------------------------------------
|  函数名称  : IDE_IPv4_to_i
|  描    述  : 转换IP地址到整型
|  参    数  : AIM::CString& strIpAddress——
|  返 回 值  : 主机字节序
|  修改记录  : 2007-10-27 16:38:57   -huangdy-   创建
-----------------------------------------------------------------*/
unsigned int IDE_IPv4_to_i(CString& strIpAddress)
{
	unsigned int LENGTH = strIpAddress.GetLength();
	BYTE chIpAddr[4] = { 0 };
	int nCount = 3;
	unsigned int nCurChar = 0;
	unsigned int i = 0;
	BYTE nSegmentVal = 0;
	while (i < LENGTH)
	{
		nCurChar = strIpAddress.GetAt(i);

		if (46/*'.'*/ != nCurChar)
		{
			nSegmentVal *= 10;
			nSegmentVal += CLYCodeCvt::HexToByte((BYTE)nCurChar);
		}
		else
		{
			chIpAddr[nCount--] = nSegmentVal;
			nSegmentVal = 0;
			if (-1 == nCount)
			{
				break;
			}
		}
		i++;
	}

	chIpAddr[nCount] = nSegmentVal;

	if (i == LENGTH && 0 == nCount)
	{
		//ATLTRACE(("%s\n"),FormatHostIPAddress(*(unsigned int*)chIpAddr));
		return *(unsigned int*)chIpAddr;
	}

	return 0;
}

uint64_t IDE_MAC_to_i(CString& strMacAddress)
{
	uint64_t nResult = 0;

	if (strMacAddress.GetLength() == 17)
	{
		strMacAddress.MakeUpper();

		BYTE nTmp = 0;
		for (int i = 0; i < 6; i++)
		{
			nTmp = CLYCodeCvt::HexToByte(strMacAddress.GetAt(i * 3)) * 16;
			nTmp = (BYTE)(nTmp + CLYCodeCvt::HexToByte(strMacAddress.GetAt(i * 3 + 1)));

			*((BYTE*)(&nResult) + i) = nTmp;
		}
	}

	return nResult;
}

bool IDE_IPv6_to_i(CString& strMacAddress, uint16_t pIPAddr[])
{
	memset(pIPAddr, 0, 16);

	bool nCompression = false;
	const unsigned int LEN = strMacAddress.GetLength();
	unsigned int  nIndex = 0;
	char nChar = 0;
	unsigned int  i = 0;
	unsigned int  nTmp = 0;

	for (; i < LEN; i++)
	{
		nChar = strMacAddress.GetAt(i);
		if (nChar != ':')
		{
			nTmp <<= 4;
			nTmp += CLYCodeCvt::HexToByte(nChar);
			if (i + 1 == LEN)
			{
				pIPAddr[nIndex] = ntohs(nTmp);
			}
		}
		else
		{
			pIPAddr[nIndex] = ntohs(nTmp);
			nIndex++;
			nTmp = 0;
			if (i + 1 < LEN)
			{
				if (strMacAddress.GetAt(i + 1) == ':') // 说明是有压缩
				{
					i++;
					nCompression = true;
					break;
				}
			}
		}
	}
	unsigned int nShift = 0;
	if (nCompression)   //如果有压缩就分为两部分转化
	{
		nIndex = 7;
		for (unsigned int j = LEN - 1; j >= i; j--)
		{
			nChar = strMacAddress.GetAt(j);
			if (nChar != ':')
			{
				nTmp += unsigned int(CLYCodeCvt::HexToByte(nChar)) << nShift;
				nShift += 4;

			}
			else
			{
				pIPAddr[nIndex] = ntohs(nTmp);
				nIndex--;
				nShift = 0;
				nTmp = 0;
			}
		}
	}

	return true;
}
#endif
#endif

int GetOUICode(BYTE* pMacAddr)
{
	int nResult = pMacAddr[0];
	nResult <<= 8;
	nResult += pMacAddr[1];
	nResult <<= 8;
	nResult += pMacAddr[2];

	return nResult;
}

char* FormatIPv6AddressAnsi(unsigned int* pIPAddr, char szRet[])
{
	assert(nullptr != pIPAddr);
	if (nullptr != pIPAddr)
	{
		bool bCompression = false;
		unsigned int nTmp = 0;
		int nIndex = 0;

		for (int i = 0; i < 8; i++)
		{
			nTmp = pIPAddr[i];
			if (0 == nTmp && !bCompression)
			{
				int j = i + 1;
				while (j < 8)
				{
					if (0 != pIPAddr[j])
					{
						break;
					}
					j++;
				}

				if (j > i + 1)   //说明有压缩
				{
					if (0 == i)
					{
						szRet[nIndex++] = ':';
					}
					szRet[nIndex++] = ':';
					if (8 == j)
					{
						break;
					}
					i = j;
					nTmp = pIPAddr[i];

					bCompression = true;
				}
			}

			//将每一段的前导零省略，但是每一段都至少应该有一个数字
			nTmp = NTOHL(nTmp);

			if (nTmp >> 12)
			{
				szRet[nIndex++] = HEX_CHAR_TABLE[nTmp >> 12];
			}

			if (nTmp >> 8)
			{
				szRet[nIndex++] = HEX_CHAR_TABLE[(nTmp & 0xF00) >> 8];
			}

			if (nTmp >> 4)
			{
				szRet[nIndex++] = HEX_CHAR_TABLE[(nTmp & 0xF0) >> 4];
			}

			szRet[nIndex++] = HEX_CHAR_TABLE[nTmp & 0x0F];

			if (i < 7)
			{
				szRet[nIndex++] = ':';
			}
		}
		szRet[nIndex++] = 0;
	}

	return szRet;
}


//--------------------------------------------------------------------------------
/// <summary>
/// 替换字符串
/// </summary>
/// <param name="strString">被格式化的字符串</param>
/// <param name="strSrc">被替换的字符串</param>
/// <param name="strDst">替换为的字符串</param>
/// <created>Andy,2020/6/24</created>
/// <changed>Andy,2020/6/24</changed>
//--------------------------------------------------------------------------------
void StrReplace(std::string & strString, const std::string & strSrc, const std::string &strDst)
{
	std::string::size_type nPos = 0;//位置
	const std::string::size_type nSrcLen = strSrc.size();//要替换的字符串大小
	const std::string::size_type nDstLen = strDst.size();//目标字符串大小

	while ((nPos = strString.find(strSrc, nPos)) != std::string::npos)
	{
		strString.replace(nPos, nSrcLen, strDst);
		nPos += nDstLen;
	}

}

#ifndef WIN32
void FormatTimeUMillisecond(int64_t nTime, AIM::CString& strResult)
{
	strResult.Format(("%0.3f ms"), nTime / 1000.0);
}

void FormatTimeUSecond(int64_t nTime, AIM::CString& strResult)
{
	strResult.Format(("%f s"), nTime / 1000000.0);
}

void FormatNumber(int64_t nNumber, AIM::CString& strResult)
{
	strResult.Format(("%I64d"), nNumber);
}

AIM::CString FormatByte(int64_t nBytes, AIM::CString& strResult)
{
	if (nBytes < 1024)
	{
		strResult.Format(("%I64d B"), nBytes);
	}
	else if (nBytes < 1048576)
	{
		strResult.Format(("%.3f KB"), float(nBytes * (1.0 / 1024.0)));
	}
	else if (nBytes < 1073741824)
	{
		strResult.Format(("%.3f MB"), float(nBytes *(1.0 / 1048576.0)));
	}
	else if (nBytes < 1099511627776)
	{
		strResult.Format(("%.3f GB"), float(nBytes *(1.0 / 1073741824.0)));
	}
	else
	{
		strResult.Format(("%.3f TB"), float(nBytes * (1.0 / 1099511627776.0)));
	}

	return strResult;
}

void FormatBit(int64_t nBytes, AIM::CString& strResult)
{
	nBytes *= 8;
	if (nBytes < 1024)
	{
		strResult.Format(("%I64d b"), nBytes);
	}
	else if (nBytes < 1048576)
	{
		strResult.Format(("%.3f Kb"), float(nBytes / 1024.0));
	}
	else if (nBytes < 1073741824)
	{
		strResult.Format(("%.3f Mb"), float(nBytes*(1.0 / 1048576.0)));
	}
	else if (nBytes < 1099511627776)
	{
		strResult.Format(("%.3f Gb"), float(nBytes*(1.0 / 1073741824.0)));
	}
	else
	{
		strResult.Format(("%.3f Tb"), float(nBytes* (1.0 / 1099511627776.0)));
	}

}

void FormatBytePerSecond(int64_t nBytes, AIM::CString& strResult)
{
	if (nBytes < 1024)
	{
		strResult.Format(("%I64d B/s"), nBytes);
	}
	else if (nBytes < 1048576)
	{
		strResult.Format(("%.3f KB/s"), float(nBytes* (1.0 / 1024.0)));
	}
	else if (nBytes < 1073741824)
	{
		strResult.Format(("%.3f MB/s"), float(nBytes*(1.0 / 1048576.0)));
	}
	else if (nBytes < 1099511627776)
	{
		strResult.Format(("%.3f GB/s"), float(nBytes*(1.0 / 1073741824.0)));
	}
	else
	{
		strResult.Format(("%.3f TB/s"), float(nBytes* (1.0 / 1099511627776.0)));
	}

}

void FormatBitPerSecond(int64_t nBytes, AIM::CString& strResult)
{
	nBytes *= 8;

	if (nBytes < 1024)
	{
		strResult.Format(("%I64d b/s"), nBytes);
	}
	else if (nBytes < 1048576)
	{
		strResult.Format(("%.3f Kb/s"), float(nBytes* (1.0 / 1024.0)));
	}
	else if (nBytes < 1073741824)
	{
		strResult.Format(("%.3f Mb/s"), float(nBytes*(1.0 / 1048576.0)));
	}
	else if (nBytes < 1099511627776)
	{
		strResult.Format(("%.3f Gb/s"), float(nBytes*(1.0 / 1073741824.0)));
	}
	else
	{
		strResult.Format(("%.3f Tb/s"), float(nBytes* (1.0 / 1099511627776.0)));
	}
}
#endif


char* Ax3_stristr_ansi(const char *str1, const char *str2)
{
	char *cp = (char *)str1;
	char *s1, *s2;

	if (!*str2)
		return((char *)str1);

	while (*cp)
	{
		s1 = cp;
		s2 = (char *)str2;

		while (*s1 && *s2)
		{
			if (*s1 == *s2 || abs(*s1 - *s2) == 32)
			{
				s1++, s2++;
			}
			else
			{
				break;
			}
		}

		if (!*s2)
			return(cp);

		cp++;
	}

	return(nullptr);
}


/*--------------------------------------------------------------------------
*函数名称：CalcAlign
*描述：内存对齐算法
*参数：unsigned int n->需要计算的数据
*参数：unsigned align->对齐长度
*返回值: 对齐后的值
*原理：

2字节对齐，要求地址位为2,4,6,8...，要求二进制位最后一位为0（2的1次方）
4字节对齐，要求地址位为4,8,12,16...，要求二进制位最后两位为0（2的2次方）
8字节对齐，要求地址位为8,16,24,32...，要求二进制位最后三位为0（2的3次方）
16字节对齐，要求地址位为16,32,48,64...，要求二进制位最后四位为0（2的4次方）
...
由此可见，我们只要对数据补齐对齐所需最少数据，然后将补齐位置0就可以实现对齐计算。

（1）(align-1)，表示对齐所需的对齐位，如：2字节对齐为1，4字节为11，8字节为111，16字节为1111...
（2）(x+(align-1))，表示x补齐对齐所需数据
（3）&~(align-1)，表示去除由于补齐造成的多余数据
（4） (x+(align-1))&~(align-1)，表示对齐后的数据

举个例子：如8字节对齐。起始地始是6
6 + （8 - 1）=0000 0110 + 0000 0111 = 0000 1101
0000 1101 & ~(0000 0111) = 0000 1000  //去除由于补齐造成的多余数据
*修改记录:
*---------------------------------------------------------------------------*/
unsigned int CalcAlign(unsigned int n, unsigned align)
{
	return ((n + align - 1) & (~(align - 1)));
}

//--------------------------------------------------------------------------------
/// <summary>
///  查询指定域名的IP地址
/// </summary>
/// <param name="strDomain">域名</param>
/// <returns>第一个ip</returns>
/// <created>Andy,2020/7/7</created>
/// <changed>Andy,2020/7/7</changed>
//--------------------------------------------------------------------------------
std::string QueryDomainIP(const std::string & strDomain)
{
	std::string strResult;

#ifndef DISENABLE_BOOT
	try
	{
		boost::asio::io_service ios;
		//创建resolver对象 
		boost::asio::ip::tcp::resolver slv(ios);
		//创建query对象 
		boost::asio::ip::tcp::resolver::query qry(strDomain.c_str(), boost::lexical_cast<std::string>(0));//将int型端口转换为字符串 
		//使用resolve迭代端点 
		boost::asio::ip::tcp::resolver::iterator it = slv.resolve(qry);
		boost::asio::ip::tcp::resolver::iterator end;

		if (it != end)
		{
			strResult = (*it).endpoint().address().to_string();
		}
	}
	catch (...)
	{
	}

	//vector<string> ip;

	//for (; it != end; it++)
	//{
	//	ip.push_back((*it).endpoint().address().to_string());
	//}
#endif

	return strResult;
}



//--------------------------------------------------------------------------------
/// <summary>
/// 
/// </summary>
/// <param name="refIPs"></param>
/// <returns></returns>
/// <created>Andy,2020/8/15</created>
/// <changed>Andy,2020/8/15</changed>
//--------------------------------------------------------------------------------
size_t QueryLocalIP(string_list& refIPs)
{
#ifndef DISENABLE_BOOT
	boost::asio::io_service ios;
	//创建resolver对象 
	boost::asio::ip::tcp::resolver slv(ios);
	//创建query对象 
	boost::asio::ip::tcp::resolver::query qry(boost::asio::ip::host_name(), "");
	//使用resolve迭代端点 
	boost::asio::ip::tcp::resolver::iterator it = slv.resolve(qry);
	boost::asio::ip::tcp::resolver::iterator end;

	for (; it != end; it++)
	{
		refIPs.push_back((*it).endpoint().address().to_string());
	}
#endif

	return refIPs.size();
}

uint64_t IDE_MAC_to_i(const char* lpMacAddress)
{
	std::string strMacAddress(lpMacAddress);
	uint64_t nResult = 0;

	std::transform(strMacAddress.begin(), strMacAddress.end(), strMacAddress.begin(), ::toupper); 

	if (strMacAddress.size() == 17)
	{
		BYTE nTmp = 0;

		for (int i = 0; i < 6; i++)
		{
			nTmp = CLYCodeCvt::HexToByte(strMacAddress.at(i * 3)) * 16;
			nTmp = (BYTE)(nTmp + CLYCodeCvt::HexToByte(strMacAddress.at(i * 3 + 1)));

			*((BYTE*)(&nResult) + i) = nTmp;
		}
	}

	return nResult;
}

//--------------------------------------------------------------------------------
/// <summary>
/// 替换字符串
/// </summary>
/// <param name="strSrc">需要处理的字符串</param>
/// <param name="strOld">被替换的字符串</param>
/// <param name="strNew">信息的字符串</param>
/// <returns>处理后的字符串</returns>
/// <created>Andy,2020/8/2</created>
/// <changed>Andy,2020/8/2</changed>
//--------------------------------------------------------------------------------
std::string& ReplaceString(std::string& strSrc, const std::string& strOld, const std::string& strNew)
{
	std::string::size_type nPos(0);

	for (; nPos != std::string::npos; nPos += strNew.size())
	{
		nPos = strSrc.find(strOld, nPos);

		if (nPos != std::string::npos)
		{
			strSrc.replace(nPos, strOld.size(), strNew);
		}
		else
		{
			break;
		}
	}

	return   strSrc;
}

//--------------------------------------------------------------------------------
/// <summary>
/// 查找字符串串，成功返回开始位置
/// </summary>
/// <param name="pSource">源字符串</param>
/// <param name="sPat">被查找的字符串</param>
/// <returns>成功返回开始位置，否则-1</returns>
/// <created>Andy,2020/8/2</created>
/// <changed>Andy,2020/8/2</changed>
//--------------------------------------------------------------------------------
int strupos(const char *pSource, const char *sPat)
{
	int nResult = -1;
	const char* pStart = strstr(pSource, sPat);

	if (nullptr != pStart)
	{
		nResult = (int)(pStart - pSource);
	}

	return nResult;
}

int striupos(const char * pSource, const char * sPat)
{
	int nResult = -1;
	const char* pStart = Ax3_stristr_ansi(pSource, sPat);

	if (nullptr != pStart)
	{
		nResult = (int)(pStart - pSource);
	}

	return nResult;
}

/*-----------------------------------------------------------------
|  函数名称  : ConstructshitTable
|  描    述  : 构造Sunday查询算法的移动表
|  参    数  : int* pShitTable——移动表指针
|              int nTableSize——移动表尺寸
|              char* pPattern——被查询的模式
|              int nPatternLen = 模式长度(缺省-1)
|  返 回 值  :
|  修改记录  : 2007-11-29 20:35:37   -huangdy-   创建
-----------------------------------------------------------------*/
void ConstructshitTable(int* pShitTable, const int nTableSize, const char* pPattern, int nPatternLen /*= -1*/)
{
	if (-1 == nPatternLen)
	{
		nPatternLen = (int)strlen(pPattern);
	}
	for (int i = 0; i < nTableSize; i++)
	{
		pShitTable[i] = nPatternLen + 1;
	}
	for (int i = 0; i < nPatternLen; i++)
	{
		pShitTable[(int)pPattern[i]] = nPatternLen - i;
	}
}

//--------------------------------------------------------------------------------
/// <summary>
/// 
/// </summary>
/// <param name="pValuDescMap"></param>
/// <param name="nType"></param>
/// <returns></returns>
/// <created>Andy,2020/8/7</created>
/// <changed>Andy,2020/8/7</changed>
//--------------------------------------------------------------------------------
const char* GetTypeDesc(
	const ID2DESC* pValuDescMap
	, const unsigned int nType)
{
	if (nullptr != pValuDescMap)
	{
		int i = 0;

		while (pValuDescMap[i].nValue != 0)
		{
			if (pValuDescMap[i].nValue == nType)
			{
				return pValuDescMap[i].lpDesc;
			}

			i++;
		}
	}

	return nullptr;
}

std::string& Trim(std::string &s)
{
	if (s.empty())
	{
		return s;
	}
	
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);

	return s;
}

//--------------------------------------------------------------------------------
/// <summary>
/// 从磁盘读取文件
/// </summary>
/// <param name="lpFile">文件路径</param>
/// <param name="refOut">接收数据的缓冲区</param>
/// <returns>读取数据的长度</returns>
/// <created>Andy,2020/8/17</created>
/// <changed>Andy,2020/8/17</changed>
//--------------------------------------------------------------------------------
int ReadFile(const char* lpFile, COutBuffer& refOut)
{
	int nResult = 0;
	std::ifstream  objFile;

	objFile.open(lpFile);

	if (objFile.is_open())
	{
		objFile.seekg(0, std::ios::end);       //设置文件指针到文件流的尾部

		std::streampos ps = objFile.tellg();   //读取文件指针的位置
		const bool bAllocated = refOut.Allocat((unsigned int)ps);

		if (bAllocated)
		{
			objFile.seekg(0, std::ios::beg);          //设置文件指针到文件流的头部
			objFile.read((char*)refOut, ps);			

			if (!(objFile.rdstate() & std::ifstream::badbit))
			{
				refOut.SetDataLen((unsigned int)ps);
				nResult = (unsigned int)ps;
			}
		}

		objFile.close();
	}

	return nResult;
}

// ********************************************************************************
/// <summary>
/// 统计字符串中asc码字符的个数
/// </summary>
/// <param name="refStr">被统计的字符串</param>
/// <returns>asc码字符的个数</returns>
/// <created>Andy,2021/7/20</created>
/// <changed>Andy,2021/7/20</changed>
// ********************************************************************************
int SumAscCount(const std::wstring& refStr)
{
	int nResult = 0;

	for (size_t i = 0; i < refStr.size(); i++)
	{
		const wchar_t chCur = refStr[i];

		if (48 <= chCur && chCur <= 122)
		{
			nResult++;
		}
	}

	return nResult;
}

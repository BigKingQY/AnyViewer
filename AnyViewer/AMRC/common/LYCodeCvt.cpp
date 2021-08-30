#include "stdafx.h"
#include "LYCodeCvt.h"
#include <locale>
#include <codecvt>
#include <iostream>
#include <stdio.h>
//#include <codecvt>

#ifdef  WIN32
#include <boost\locale\encoding.hpp>
#include <boost\archive\iterators\base64_from_binary.hpp>
#include <boost\archive\iterators\binary_from_base64.hpp>
#include <boost\archive\iterators\transform_width.hpp>
#include <boost\xpressive\xpressive_dynamic.hpp>
#include <boost\lexical_cast.hpp>//使用字符串转换功能 
#else
#include <boost/locale/encoding.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>
#include <boost/lexical_cast.hpp>//使用字符串转换功能 
#endif 
#include "MiscellaneousFunctions.h"

using namespace std;
using namespace boost::archive::iterators;


int preNUm(unsigned char byte)
{
	unsigned char chMask = 0x80;
	int nNum = 0;

	for (int i = 0; i < 8; i++)
	{
		if ((byte & chMask) == chMask)
		{
			chMask = chMask >> 1;
			nNum++;
		}
		else
		{
			break;
		}
	}

	return nNum;
}



CLYCodeCvt::CLYCodeCvt()
{
}


CLYCodeCvt::~CLYCodeCvt()
{
	
}

// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <param name="wstr"></param>
/// <returns></returns>
/// <created>Leiyz,2019/3/7</created>
/// <changed>Leiyz,2019/3/7</changed>
// ********************************************************************************
std::string CLYCodeCvt::UnicodeToUTF8(const std::wstring & wstr)
{
    return boost::locale::conv::from_utf(wstr, "UTF-8");
}

// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
/// <created>Leiyz,2019/3/7</created>
/// <changed>Leiyz,2019/3/7</changed>
// ********************************************************************************
std::wstring CLYCodeCvt::UTF8ToUnicode(const std::string & str)
{
    return boost::locale::conv::utf_to_utf<wchar_t>(str);
}

std::string CLYCodeCvt::UnicodeToANSI(const std::wstring & wstr)
{
    return boost::locale::conv::from_utf(wstr, "GBK");
}

std::wstring CLYCodeCvt::ANSIToUnicode(const std::string & str)
{
    return boost::locale::conv::to_utf<wchar_t>(str, "GBK");
}

std::string CLYCodeCvt::UTF8ToANSI(const std::string & str)
{
    return boost::locale::conv::between(str, "GBK", "UTF-8");
}

std::string CLYCodeCvt::ANSIToUTF8(const std::string & str)
{
    return boost::locale::conv::between(str, "UTF-8", "GBK");
}


/*-----------------------------------------------------------------
|  函数名称  : DeescapeURL
|  描    述  : URL解码
|  参    数  : const CStringA &strURL——
|  返 回 值  :
|  修改记录  : 2007-12-2 17:31:00   -huangdy-   创建
-----------------------------------------------------------------*/
int CLYCodeCvt::DeescapeURL(const std::string &strURL, char* pOutBuffer)
{
	char* pOutBuferHead = pOutBuffer;

	for (size_t i = 0; i < strURL.size(); i++)
	{
		const char chCur = strURL[i];

		if (chCur != '%')
		{
			*pOutBuffer = chCur;
		}
		else
		{
			const char c1 = strURL[++i];
			const char c0 = strURL[++i];
			int nNumb = 0;

			nNumb += HexChar2Dec(c1) * 16 + HexChar2Dec(c0);
			*pOutBuffer = char(nNumb);
		}
		pOutBuffer++;
	}

	return (int)(pOutBuffer - pOutBuferHead);
}


/*-----------------------------------------------------------------
|  函数名称  : EscapeURL
|  描    述  : URL编码
|  参    数  : const CStringA &strURL——
|  返 回 值  :
|  修改记录  : 2007-12-2 17:31:09   -huangdy-   创建
-----------------------------------------------------------------*/
std::string CLYCodeCvt::EscapeURL(const std::string &strURL)
{
	std::string strResult;

	for (size_t i = 0; i < strURL.size(); i++)
	{
		char chCur = strURL[i];

		if (('0' <= chCur && chCur <= '9') ||
			('a' <= chCur && chCur <= 'z') ||
			('A' <= chCur && chCur <= 'Z') ||
			chCur == '/' || chCur == '.')
		{
			strResult += chCur;
		}
		else
		{
			int j = (int)chCur;

			if (j < 0)
			{
				j += 256;
			}

			int i1, i0;

			i1 = j / 16;
			i0 = j - i1 * 16;
			strResult += '%';
			strResult += Dec2HexChar(i1);
			strResult += Dec2HexChar(i0);
		}
	}

	return strResult;
}


//--------------------------------------------------------------------------------
/// <summary>
/// base64编码
/// </summary>
/// <param name="refInput">被编码的字符串</param>
/// <param name="refOuput">输出字符串</param>
/// <returns></returns>
/// <created>Andy,2020/6/11</created>
/// <changed>Andy,2020/6/11</changed>
//--------------------------------------------------------------------------------
bool CLYCodeCvt::Base64Encode(const std::string & refInput, std::string&  refOuput)
{
	typedef base64_from_binary<transform_width<string::const_iterator, 6, 8>> Base64EncodeIterator;
	stringstream ssResult;

	try
	{
		refOuput.clear();
		copy(Base64EncodeIterator(refInput.begin())
			, Base64EncodeIterator(refInput.end())
			, ostream_iterator<char>(ssResult));
	}
	catch (...)
	{
		return false;
	}

	size_t equal_count = (3 - refInput.length() % 3) % 3;

	for (size_t i = 0; i < equal_count; i++)
	{
		ssResult.put('=');
	}

	refOuput = ssResult.str();

	return (refOuput.empty() == false);
}

//--------------------------------------------------------------------------------
/// <summary>
///  base64解码
/// </summary>
/// <param name="refInput">被解码的字符串</param>
/// <param name="refOuput">输出字符串</param>
/// <returns></returns>
/// <created>Andy,2020/6/11</created>
/// <changed>Andy,2020/6/11</changed>
//--------------------------------------------------------------------------------
bool CLYCodeCvt::Base64Decode(const std::string & refInput, std::string&  refOuput)
{
	typedef transform_width<binary_from_base64<string::const_iterator>, 8, 6> Base64DecodeIterator;
	stringstream ssResult;

	try
	{
		refOuput.clear();
		copy(Base64DecodeIterator(refInput.begin())
			, Base64DecodeIterator(refInput.end())
			, ostream_iterator<char>(ssResult));
	}
	catch (...)
	{
		return false;
	}

	refOuput = ssResult.str();

	return (refOuput.empty() == false);
}


/*-----------------------------------------------------------------
|  函数名称  : IDE_ConvertHexToDec
|  描    述  :
|  参    数  : CString& strValue——
|  返 回 值  :
|  修改记录  : 2007-11-6 15:31:53   -huangdy-   创建
-----------------------------------------------------------------*/
int CLYCodeCvt::Hex2Dec(std::string& strValue)
{
	int nResult = 0;
	unsigned int nPowr = 1;
	int  nLen = strValue.size(); //必须是有符号,否则可能出错

	std::transform(strValue.begin(), strValue.end(), strValue.begin(), ::tolower);

	while (nLen--)
	{
		nResult += ConverHexCharToByte(strValue.at(nLen)) * nPowr;
		nPowr <<= 4;
	}

	return nResult;
}


//--------------------------------------------------------------------------------
/// <summary>
/// 判断字符串是否是GBK编码
/// </summary>
/// <param name="pString">指向字符串的指针</param>
/// <param name="nLen">字符串长度</param>
/// <returns>true:是,false:否</returns>
/// <created>Andy,2020/6/6</created>
/// <changed>Andy,2020/6/6</changed>
//--------------------------------------------------------------------------------
bool CLYCodeCvt::IsGBK(const char* pString, const int nLen)
{
	int i = 0;

	while (i < nLen)
	{
		if (pString[i] <= 0x7f)
		{
			//编码小于等于127,只有一个字节的编码，兼容ASCII
			i++;
			continue;
		}
		else
		{
			//大于127的使用双字节编码
			if (pString[i] >= 0x81 &&
				pString[i] <= 0xfe &&
				pString[i + 1] >= 0x40 &&
				pString[i + 1] <= 0xfe &&
				pString[i + 1] != 0xf7)
			{
				i += 2;
				continue;
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}

//--------------------------------------------------------------------------------
/// <summary>
/// 判断字符串是否是UTF8编码
/// </summary>
/// <param name="pString">指向字符串的指针</param>
/// <param name="nLen">字符串长度</param>
/// <returns>true:是,false:否</returns>
/// <created>Andy,2020/6/6</created>
/// <changed>Andy,2020/6/6</changed>
//--------------------------------------------------------------------------------
bool CLYCodeCvt::IsUtf8(const char* pStr, const int nLen)
{
	int nNum = 0;
	int i = 0;

	while (i < nLen)
	{
		if ((pStr[i] & 0x80) == 0x00)
		{
			// 0XXX_XXXX
			i++;
			continue;
		}
		else if ((nNum = preNUm(pStr[i])) > 2)
		{
			// 110X_XXXX 10XX_XXXX
			// 1110_XXXX 10XX_XXXX 10XX_XXXX
			// 1111_0XXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
			// 1111_10XX 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
			// 1111_110X 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
			// preNUm() 返回首个字节8个bits中首�?0bit前面1bit的个数，该数量也是该字符所使用的字节数        
			i++;

			for (int j = 0; j < nNum - 1; j++)
			{
				//判断后面nNum - 1 个字节是不是都是10开
				if ((pStr[i] & 0xc0) != 0x80)
				{
					return false;
				}

				i++;
			}
		}
		else
		{
			//其他情况说明不是utf-8
			return false;
		}
	}

	return true;
}


const unsigned int ASC_TO_HEX_TABLE_LEN = 103;
const unsigned char ASC_TO_HEX_TABLE[ASC_TO_HEX_TABLE_LEN] =
{
	255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,0,1,
	2,3,4,5,6,7,8,9,255,255,
	255,255,255,255,255,10,11,12,13,14,
	15,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,10,11,12,
	13,14,15
};

unsigned char CLYCodeCvt::HexToByte(const unsigned int chHexChar)
{
	//assert(chHexChar < ASC_TO_HEX_TABLE_LEN);

	if (chHexChar < ASC_TO_HEX_TABLE_LEN)
	{
		return ASC_TO_HEX_TABLE[chHexChar];
	}

	return 255;
}

const char arrChars[] = "0123456789ABCDEF";

char CLYCodeCvt::Dec2HexChar(const int n)
{
	if (0 <= n && n <= 15)
	{
		return arrChars[n];
	}
	else
	{
		return char(0);
	}
}

int CLYCodeCvt::HexChar2Dec(char chCur)
{
	if ('0' <= chCur && chCur <= '9')
	{
		return int(chCur - '0');
	}
	else if ('a' <= chCur && chCur <= 'f')
	{
		return (int(chCur - 'a') + 10);
	}
	else if ('A' <= chCur && chCur <= 'F')
	{
		return (int(chCur - 'A') + 10);
	}
	else
	{
		return -1;
	}
}


/*-----------------------------------------------------------------
|  函数名称  : ConvertBinToDec
|  描    述  : 转换二进制字符串为使进制整数
|  参    数  : CString& strValue——
|  返 回 值  :
|  修改记录  : 2007-10-27 16:38:49   -huangdy-   创建
-----------------------------------------------------------------*/
int CLYCodeCvt::ConvertBinToDec(const std::string& refValue)
{
	std::string strValue = refValue;
	int nResult = 0;
	unsigned int nPowr = 1;
	unsigned int nCurChar = 0;

#ifdef WIN32
	int i = min(int(strValue.size()), 32); //必须是有符号，否则可能出错
#else
	int i = std::min(int(strValue.size()), 32); //必须是有符号，否则可能出错
#endif

	std::transform(strValue.begin(), strValue.end(), strValue.begin(), ::tolower);

	while (i--)
	{
		nCurChar = strValue[i];

		switch (nCurChar)
		{
		case 0x31/*'1'*/:
			nResult += nPowr;
			break;

		case 0x62/*'b'*/:
			return nResult;

		case 0x30/*'0'*/:
			break;

		default:
			return -1; //error format
		}

		nPowr <<= 1;
	}

	return -1; //error format
}


/*Quoted-Printable编码 源码！！
Quoted-Printable也是MIME邮件中常用的编码方式之一。同Base64一样，
它也将输入的字符串或数据编码成全是ASCII码的可打印字符串。

Quoted-Printable编码的基本方法是：输入数据在33-60、62-126范围内的，
直接输出；其它的需编码为“=”加两个字节的HEX码(大写)。为保证输出行
不超过规定长度，可在行尾加“=\r\n”序列作为软回车。 */

int CLYCodeCvt::EncodeQuoted(const std::string& refSrc, char* pDst)
{
	char* pDstHead = pDst;
	size_t i = 0;
	const size_t SRC_LEN = refSrc.size();

	while (i < SRC_LEN)
	{
		const char chCur = refSrc[i];

		// ASCII 33-60, 62-126原样输出，其余的需编码
		if ((chCur >= '!') && (chCur <= '~') && (chCur != '='))
		{
			*pDst = chCur;
			pDst++;
		}
		else
		{
			*pDst = '=';
			pDst++;

			*pDst = arrChars[chCur >> 4];
			pDst++;

			*pDst = arrChars[chCur & 0x0F];
			pDst++;			
		}

		i++;
	}

	return (int)(pDst - pDstHead);
}


//--------------------------------------------------------------------------------
/// <summary>
/// Quoted-Printable解码很简单，将编码过程反过来就行了。
/// </summary>
/// <param name="pSrc"></param>
/// <param name="nSrcLen"></param>
/// <param name="refOut"></param>
/// <returns></returns>
/// <created>Andy,2020/8/2</created>
/// <changed>Andy,2020/8/2</changed>
//--------------------------------------------------------------------------------
int CLYCodeCvt::DecodeQuoted(const char* pSrc, const int nSrcLen,std::string& refOut)
{
	int nDstLen = 0;        // 输出的字符计数
	int i = 0;	

	refOut.clear();

	while (i < nSrcLen)
	{
		if (strncmp(pSrc, "=\r\n", 3) == 0)        // 软回车，跳过
		{
			pSrc += 3;
			i += 3;
		}
		else
		{
			if (*pSrc == '=')        // 是编码字节
			{
				pSrc++;

				char chTmp = 0;

				chTmp = (ASC_TO_HEX_TABLE[(int)*pSrc] << 4);
				pSrc++;

				chTmp += (ASC_TO_HEX_TABLE[(int)*pSrc]);
				pSrc++;

				refOut += chTmp;
				i += 3;
			}
			else        // 非编码字节
			{
				refOut += (unsigned char)*pSrc++;
				i++;
			}

			nDstLen++;
		}
	}


	return nDstLen;
}


//--------------------------------------------------------------------------------
/// <summary>
/// 解码Mime邮件头的字段
/// </summary>
/// <param name="pSrc"></param>
/// <param name="nLen"></param>
/// <param name="refRet"></param>
/// <returns></returns>
/// <created>Andy,2020/8/2</created>
/// <changed>Andy,2020/8/2</changed>
//--------------------------------------------------------------------------------
const char* CLYCodeCvt::MimeDecodeMailHeaderField(const char *pSrc,const int /*nLen*/,std::string& refRet)
{
	CHECK_POINTER_EX(pSrc,nullptr);

	char *pOpStr = const_cast<char*>(pSrc);
	std::string strHeader;
	std::string strTail;

	//remove starting spaces...
	while (' ' == *pOpStr)
	{
		pOpStr++;
	}

	const int nBeginPos = striupos(pOpStr, "=?");

	if (nBeginPos > 0)
	{
		strHeader = std::string(pSrc, nBeginPos);
		pOpStr += nBeginPos;
	}

	if (striupos(pOpStr, "=?") == 0)
	{
		//we have to decode
		//if there are text after "?=" we have to move it to another string, so the decoder don't delete it...
		//if isn't objQPDecoder-encoded and the first char is encoded, we have to move past the "?=", to find the next...

		int nPlainPos = striupos(pOpStr, "Q?=");
		
		if (nPlainPos > 0)
		{
			nPlainPos += 3;
			
			char *pTmp = pOpStr + nPlainPos;

			nPlainPos += striupos(pTmp, "?=");
		}
		else
		{
			nPlainPos = striupos(pOpStr, "?=");
		}

		if (nPlainPos > 1)
		{
			char *pMid = pOpStr + nPlainPos + 2;
			
			pOpStr[nPlainPos] = '\0';
			
			size_t nMidLen = strlen(pMid);

			if (nMidLen > 0)
			{
				//we have some text we need to "cut"
				nMidLen++;
				strTail = std::string(pMid, nMidLen);
			}
		}

		//do the decode...
		std::string strTextDecoded;
		int nPos = (int)striupos(pOpStr, "?Q?");

		if (nPos > 0)
		{
			//it'pSrc quoted printable
			//find the right "starting point" in the string...
			
			pOpStr += nPos;
			
			if (strlen(pOpStr) < 4)  //this should never happend, if the sender is RFC compliant...
			{
				return pSrc;
			}

			pOpStr += 3;
			DecodeQuoted(pOpStr,strlen(pOpStr), strTextDecoded);
		}
		else
		{
			nPos = (int)striupos(pOpStr, "?B?");

			if (nPos > 0)
			{
				//it'pSrc base64
				//find the right "starting point" in the string...

				pOpStr += nPos;

				if (strlen(pOpStr) < 4)
				{
					return pSrc;         //this should never happend, if the sender is RFC compliant...
				}

				pOpStr += 3;

				const int STR_LEN = (int)strlen(pOpStr);
				std::string strDecoded(pOpStr, STR_LEN);

				Base64Decode(strDecoded, strTextDecoded);
			}
			else
			{
				return pSrc;
			}
		}

		refRet = strHeader;
		refRet += strTextDecoded;
		refRet += strTail;
	}


	return refRet.c_str();
}

#ifndef  WIN32
std::string CLYCodeCvt::gb2312_to_utf8(std::string const &strGb2312)
{
	std::vector<wchar_t> szBuffer(strGb2312.size());
#ifdef _MSC_VER
	std::locale objlocale("zh-CN");
#else
	std::locale objlocale("zh_CN.GB18030");
#endif
	wchar_t* pwszNext = nullptr;
	const char* pszNext = nullptr;
	mbstate_t objState = {};
	const int nResult = std::use_facet<std::codecvt<wchar_t, char, mbstate_t> >(objlocale).in(
		objState
			,strGb2312.data()
			, strGb2312.data() + strGb2312.size()
			, pszNext
			, szBuffer.data()
			, szBuffer.data() + szBuffer.size()
			, pwszNext);

	if (std::codecvt_base::ok == nResult)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> cutf8;
		return cutf8.to_bytes(std::wstring(szBuffer.data(), pwszNext));
	}

	return "";

}

std::string CLYCodeCvt::utf8_to_gb2312(std::string const &strUtf8)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> cutf8;
	std::wstring wstrTemp = cutf8.from_bytes(strUtf8);
#ifdef _MSC_VER
	std::locale objlocale("zh-CN");
#else
	std::locale objlocale("zh_CN.GB18030");
#endif

	const wchar_t* pwszNext = nullptr;
	char* pszNext = nullptr;
	mbstate_t objState = {};

	std::vector<char> szBuffer(wstrTemp.size() * 2);
	const int nResult = std::use_facet<std::codecvt<wchar_t, char, mbstate_t> >(objlocale).out(
		objState,
		wstrTemp.data()
		, wstrTemp.data() + wstrTemp.size()
		, pwszNext
		, szBuffer.data()
		, szBuffer.data() + szBuffer.size()
		, pszNext);

	if (std::codecvt_base::ok == nResult)
	{
		return std::string(szBuffer.data(), pszNext);
	}
	return "";
}
#endif
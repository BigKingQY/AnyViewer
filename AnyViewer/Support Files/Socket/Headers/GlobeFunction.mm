//
//  GlobeFunction.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/3.
//

#include "GlobeFunction.h"
#include "Typedef.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


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

unsigned char HexToByte(const unsigned int chHexChar)
{
    assert(chHexChar < ASC_TO_HEX_TABLE_LEN);

    if (chHexChar < ASC_TO_HEX_TABLE_LEN)
    {
        return ASC_TO_HEX_TABLE[chHexChar];
    }

    return 255;
}

//将IP地址转换成数字
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
            nSegmentVal += HexToByte(nCurChar);
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


//根据域名获取IP地址，如果是IP地址会直接返回IP地址
std::string getIPWithHostName(std::string hostName)
{
    const char *hostN = hostName.c_str();
    struct hostent* phot;
    @try {
        phot = gethostbyname(hostN);
    } @catch (NSException *exception) {
        return nil;
    }
    struct in_addr ip_addr;
    if (phot == NULL) {
        return nil;
    }
    memcpy(&ip_addr, phot->h_addr_list[0], 4);
    char ip[20] = {0}; inet_ntop(AF_INET, &ip_addr, ip, sizeof(ip));
    
    std::string str_IP(ip);
    return str_IP;
}




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



char* FormatHostIPAddressAnsi(unsigned int nHostIP, char szRet[])
{
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


char* FormatNetIPAddress(unsigned int nNetIP, TCHAR szRet[])
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

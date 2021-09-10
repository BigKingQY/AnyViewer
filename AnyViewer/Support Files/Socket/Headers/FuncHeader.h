//
//  FuncHeader.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/2.
//

#ifndef FuncHeader_h
#define FuncHeader_h

#include "Typedef.h"
//#include <string>
//#import <netinet/in.h>
//#import <arpa/inet.h>
//#include <netdb.h>



inline uint64_t ntohq(uint64_t nData)
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



//
//
//char* FormatHostIPAddress(unsigned int nHostIP, TCHAR szRet[])
//{
//    //StringCbPrintf(szRet,32,("%d.%d.%d.%d")
//    //    , *((const BYTE *)(&nHostIP) + 3)
//    //    , *((const BYTE *)(&nHostIP) + 2)
//    //    , *((const BYTE *)(&nHostIP) + 1)
//    //    , *((const BYTE *)(&nHostIP)));
//
//
//    unsigned int i = 0;
//    BYTE nByte = *((const BYTE *)(&nHostIP) + 3);
//    if (nByte / 100)
//    {
//        szRet[i++] = nByte / 100 + 48;
//        nByte %= 100;
//
//        szRet[i++] = nByte / 10 + 48;
//        szRet[i++] = nByte % 10 + 48;
//        szRet[i++] = '.';
//    }
//    else
//    {
//        if (nByte / 10)
//        {
//            szRet[i++] = nByte / 10 + 48;
//            szRet[i++] = nByte % 10 + 48;
//            szRet[i++] = '.';
//        }
//        else
//        {
//            szRet[i++] = nByte % 10 + 48;
//            szRet[i++] = '.';
//        }
//    }
//
//    nByte = *((const BYTE *)(&nHostIP) + 2);
//    if (nByte / 100)
//    {
//        szRet[i++] = nByte / 100 + 48;
//        nByte %= 100;
//
//        szRet[i++] = nByte / 10 + 48;
//        szRet[i++] = nByte % 10 + 48;
//        szRet[i++] = '.';
//    }
//    else
//    {
//        if (nByte / 10)
//        {
//            szRet[i++] = nByte / 10 + 48;
//            szRet[i++] = nByte % 10 + 48;
//            szRet[i++] = '.';
//        }
//        else
//        {
//            szRet[i++] = nByte % 10 + 48;
//            szRet[i++] = '.';
//        }
//    }
//
//    nByte = *((const BYTE *)(&nHostIP) + 1);
//    if (nByte / 100)
//    {
//        szRet[i++] = nByte / 100 + 48;
//        nByte %= 100;
//
//        szRet[i++] = nByte / 10 + 48;
//        szRet[i++] = nByte % 10 + 48;
//        szRet[i++] = '.';
//    }
//    else
//    {
//        if (nByte / 10)
//        {
//            szRet[i++] = nByte / 10 + 48;
//            szRet[i++] = nByte % 10 + 48;
//            szRet[i++] = '.';
//        }
//        else
//        {
//            szRet[i++] = nByte % 10 + 48;
//            szRet[i++] = '.';
//        }
//    }
//
//    nByte = *((const BYTE *)(&nHostIP));
//    if (nByte / 100)
//    {
//        szRet[i++] = nByte / 100 + 48;
//        nByte %= 100;
//
//        szRet[i++] = nByte / 10 + 48;
//        szRet[i++] = nByte % 10 + 48;
//    }
//    else
//    {
//        if (nByte / 10)
//        {
//            szRet[i++] = nByte / 10 + 48;
//            szRet[i++] = nByte % 10 + 48;
//        }
//        else
//        {
//            szRet[i++] = nByte % 10 + 48;
//        }
//    }
//    szRet[i++] = 0;
//
//    return szRet;
//}


#endif /* FuncHeader_h */

//
//  GlobeFunction.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/3.
//

#ifndef GlobeFunction_h
#define GlobeFunction_h

#include <string>
#include "Typedef.h"

///将IP转换成数字
unsigned int IDE_IPv4_to_i(const std::string & strIpAddress);

///根据域名获取IP地址
std::string getIPWithHostName(std::string hostName);


char* FormatHostIPAddressAnsi(unsigned int nHostIP, char szRet[]);

char* FormatNetIPAddress(unsigned int nNetIP, TCHAR szRet[]);


#endif

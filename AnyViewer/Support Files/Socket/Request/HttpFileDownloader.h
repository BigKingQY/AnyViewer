/////////////////////////////////////////////////////////////////////
/// @file HttpFileDownloader.h
/// @brief 通过HTTP协议下载文件类定义
/// 
/// @author andy
/// @version 1.0
/// @date 2021.3.18
/////////////////////////////////////////////////////////////////////
#pragma once
#include <iostream>
#include <string>
#include "Typedef.h"


// --------------------------------------------------------------------------------
/// <summary>
/// 下载http文件事件
/// </summary>
// --------------------------------------------------------------------------------
enum class HTTPFILE_DOWNLOAD_EVENT
{
	HDE_BEGIN = 1,     ///< 枚举常量,开始下载
	HDE_PROCESS = 2,   ///< 枚举常量,正在下载
	HDE_END = 3,      ///< 枚举常量,下载结束
};

// 下载回调函数
using  DOWNLOAD_CALLBACK = std::function<void(const HTTPFILE_DOWNLOAD_EVENT, const int)>;


class CCallbackPara
{
public:
	CCallbackPara(FILE* pFile, DOWNLOAD_CALLBACK  fnCallback)
		: m_pFile(pFile)
		, m_fnCallback(fnCallback)
	{
	}
public:
	FILE* m_pFile{ nullptr };
	DOWNLOAD_CALLBACK  m_fnCallback{ nullptr };
};


class CHttpFileDownloader 
{
public:
	CHttpFileDownloader(bool bEnableSSL = false);

	~CHttpFileDownloader();

public:
	// 同步下载文件
	bool Downdload(const std::string& refURL,const std::string& refPath);

	// 异步下载
	bool AsynDowndload(const std::string& refURL, const std::string& refPath,DOWNLOAD_CALLBACK fnCallBack= nullptr);

private:

	bool DoDowndload(const std::string& refURL, const std::string& refPath, DOWNLOAD_CALLBACK fnCallBack);

private:	
	///< 是否采用https通信
	DECLARE_MEMBER_AND_METHOD_V11(bool, m_bEnableSSL, EnableSSL, false);

};


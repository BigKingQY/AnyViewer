/////////////////////////////////////////////////////////////////////
/// @file HttpFileDownloader.h
/// @brief ͨ��HTTPЭ�������ļ��ඨ��
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
/// ����http�ļ��¼�
/// </summary>
// --------------------------------------------------------------------------------
enum class HTTPFILE_DOWNLOAD_EVENT
{
	HDE_BEGIN = 1,     ///< ö�ٳ���,��ʼ����
	HDE_PROCESS = 2,   ///< ö�ٳ���,��������
	HDE_END = 3,      ///< ö�ٳ���,���ؽ���
};

// ���ػص�����
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
	// ͬ�������ļ�
	bool Downdload(const std::string& refURL,const std::string& refPath);

	// �첽����
	bool AsynDowndload(const std::string& refURL, const std::string& refPath,DOWNLOAD_CALLBACK fnCallBack= nullptr);

private:

	bool DoDowndload(const std::string& refURL, const std::string& refPath, DOWNLOAD_CALLBACK fnCallBack);

private:	
	///< �Ƿ����httpsͨ��
	DECLARE_MEMBER_AND_METHOD_V11(bool, m_bEnableSSL, EnableSSL, false);

};


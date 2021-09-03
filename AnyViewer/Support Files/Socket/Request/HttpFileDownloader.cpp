/////////////////////////////////////////////////////////////////////
/// @file HttpFileDownloader.cpp
/// @brief ͨ��HTTPЭ�������ļ���ʵ��
/// 
///  https://gitee.com/jackarain/avhttp/blob/master/example/ip138.cpp
/// @author andy
/// @version 1.0
/// @date 2021.3.18
/////////////////////////////////////////////////////////////////////

#include "HttpFileDownloader.h"
#include <boost/format.hpp>
#include <thread>

#include <curly/curly.h>
#include <ZJCurl/ZJCurl.h>
//#include <easy.h>

#pragma comment(lib, "libcurl.lib")  




CHttpFileDownloader::CHttpFileDownloader(bool bEnableSSL)
	: m_bEnableSSL(bEnableSSL)
{
}

CHttpFileDownloader::~CHttpFileDownloader()
{
}




// ********************************************************************************
/// <summary>
/// ͬ�������ļ�
/// </summary>
/// <param name="refURL">�ļ�URL����</param>
/// <param name="refPath">�����ļ���·��</param>
/// <returns>true:�ɹ�,false:ʧ��</returns>
/// <created>Andy,2021/3/18</created>
/// <changed>Andy,2021/3/18</changed>
// ********************************************************************************
bool CHttpFileDownloader::Downdload(const std::string& refURL, const std::string& refPath)
{
	return DoDowndload(refURL, refPath, nullptr);
}

// ********************************************************************************
/// <summary>
/// �첽����
/// </summary>
/// <param name="refURL">�ļ�URL����</param>
/// <param name="refPath">�����ļ���·��</param>
/// <created>Andy,2021/3/19</created>
/// <changed>Andy,2021/3/19</changed>
// ********************************************************************************
bool CHttpFileDownloader::AsynDowndload(
	const std::string& refURL
	, const std::string& refPath
	, DOWNLOAD_CALLBACK fnCallBack)
{
	std::thread objDowndloadThread(
		[this, refURL, refPath, fnCallBack]()
	{
		const bool bSuccess = DoDowndload(refURL, refPath, fnCallBack);

		if (nullptr != fnCallBack)
		{
			fnCallBack(HTTPFILE_DOWNLOAD_EVENT::HDE_END, bSuccess);
		}
	}
	);

	objDowndloadThread.detach();

	return true;
}

// ********************************************************************************
/// <summary>
/// д���ݻص�����
/// </summary>
/// <param name="pBuffer"></param>
/// <param name="nSize"></param>
/// <param name="nItems"></param>
/// <param name="pPara"></param>
/// <returns></returns>
/// <created>Andy,2021/3/19</created>
/// <changed>Andy,2021/3/19</changed>
// ********************************************************************************
size_t WriteData(char* pBuffer, size_t nSize, size_t nItems, void* pPara)
{
	CCallbackPara* pCallbackPara = (CCallbackPara*)pPara;
	const auto nBytesWritten = fwrite(pBuffer, nSize, nItems, pCallbackPara->m_pFile);

	if (nullptr != pCallbackPara->m_fnCallback)
	{
		pCallbackPara->m_fnCallback(HTTPFILE_DOWNLOAD_EVENT::HDE_PROCESS, nBytesWritten);
	}

	return nBytesWritten;
}

// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <param name="b"></param>
/// <param name="size"></param>
/// <param name="nitems"></param>
/// <param name="pPara"></param>
/// <returns></returns>
/// <created>Andy,2021/3/19</created>
/// <changed>Andy,2021/3/19</changed>
// ********************************************************************************
size_t OnDoHTTPHead(char* b, size_t size, size_t nitems, void* pPara)
{
	CCallbackPara* pCallbackPara = (CCallbackPara*)pPara;
	std::string strContent(b, size * nitems);

//	strContent.MakeLower();
//
//	auto nPos1 = strContent.Find(_T("content-length:"));
//
//	if (nPos1 >= 0)
//	{
//		nPos1 += 15; /// "content-length:"�ĳ���
//
//		const auto nPos2 = strContent.Find(_T("\r\n"), nPos1);
//
//		if (nullptr != pCallbackPara->m_fnCallback)
//		{
//			CString strLen = strContent.Mid(nPos1, nPos2 - nPos1);
//
//			pCallbackPara->m_fnCallback(HTTPFILE_DOWNLOAD_EVENT::HDE_BEGIN, _ttoi(strLen));
//		}
//	}

	return size * nitems;
}



// ********************************************************************************
/// <summary>
/// �����ļ�
/// </summary>
/// <param name="refURL">�ļ�URL����</param>
/// <param name="refPath">�����ļ���·��</param>
/// <param name="fnCallBack">ָ��ص�������ָ��</param>
/// <returns>true:�ɹ�,false:ʧ��</returns>
/// <created>Andy,2021/3/19</created>
/// <changed>Andy,2021/3/19</changed>
// ********************************************************************************
bool CHttpFileDownloader::DoDowndload(
	const std::string& refURL
	, const std::string& refPath
	, DOWNLOAD_CALLBACK fnCallBack)
{
	bool bResult = false;

	// ��note.xmlд��myxml.xml�ļ���
	FILE* pFile = fopen(refPath.c_str(), "wb+");

	if (nullptr != pFile)
	{
		CURL* pCurl = curl_easy_init();

		if (nullptr != pCurl)
		{
			CCallbackPara objCallbackPara(pFile, fnCallBack);

			if (GetEnableSSL())
			{
				curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, false);
				curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, false);
			}

			curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)&objCallbackPara);
			curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, WriteData);
			curl_easy_setopt(pCurl, CURLOPT_URL, refURL.c_str());
			curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT, 30);  // �������ӳ�ʱ����λ��
			curl_easy_setopt(pCurl, CURLOPT_HEADERDATA, (void*)&objCallbackPara);
			curl_easy_setopt(pCurl, CURLOPT_HEADERFUNCTION, OnDoHTTPHead);

			const CURLcode nRetCode = curl_easy_perform(pCurl);

			curl_easy_cleanup(pCurl);
			bResult = (CURLE_OK == nRetCode);
		}

		fclose(pFile);
	}

	return bResult;
}

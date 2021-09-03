/////////////////////////////////////////////////////////////////////
/// @file log.cpp
/// @brief 日志类实现
/// 
/// @author andy
/// @version 1.0
/// @date 2020.10.21
/////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "log.h"
#include "AIMTime.h"

#include <algorithm>
#include <iostream>
#include <stdarg.h>

const size_t LEVEL_NAME_LEN_MAX = 8;
const size_t LOG_BUF_LEN_MAX = 4096*2;


// ********************************************************************************
/// <summary>
/// 根据日志类型得到其名称
/// </summary>
/// <param name="nLevel">日志类型</param>
/// <returns>日志名称</returns>
/// <created>Andy,2020/10/21</created>
/// <changed>Andy,2020/10/21</changed>
// ********************************************************************************
const char* GetLevelName(const CLogger::LEVEL_TYPE nLevel)
{
	switch (nLevel)
	{
	case CLogger::LEVEL_FATAL:
		return "[FATAL] ";
	case CLogger::LEVEL_ERROR:
		return "[ERROR] ";
	case CLogger::LEVEL_WARN:
		return "[WARN ] ";
	case CLogger::LEVEL_INFO:
		return "[INFO ] ";
	case CLogger::LEVEL_DEBUG:
		return "[DEBUG] ";
	case CLogger::LEVEL_TRACE:
		return "[TRACE] ";
	}

	return "";
}



CLogger::CLogger() 
{
	m_nRotateSize = 0;
	m_objStats.m_nCurSize = 0;
	m_objStats.m_nTotalSize = 0;
}

CLogger::~CLogger()
{
	this->Close();
}


// ********************************************************************************
/// <summary>
/// 根据名称查询其对应的日志类型
/// </summary>
/// <param name="pLevelname">日志类型名称</param>
/// <returns>日志类型</returns>
/// <created>Andy,2020/10/21</created>
/// <changed>Andy,2020/10/21</changed>
// ********************************************************************************
CLogger::LEVEL_TYPE CLogger::QueryLevel(const char *pLevelname)
{
	if (strcmp("all", pLevelname) == 0)
	{
		return LEVEL_ALL;
	}

	if (strcmp("Trace", pLevelname) == 0)
	{
		return LEVEL_TRACE;
	}
	if (strcmp("Debug", pLevelname) == 0)
	{
		return LEVEL_DEBUG;
	}
	if (strcmp("Info", pLevelname) == 0)
	{
		return LEVEL_INFO;
	}
	if (strcmp("Warning", pLevelname) == 0)
	{
		return LEVEL_WARN;
	}
	if (strcmp("Error", pLevelname) == 0)
	{
		return LEVEL_ERROR;
	}
	if (strcmp("Fatal", pLevelname) == 0)
	{
		return LEVEL_FATAL;
	}


	return LEVEL_DEBUG;
}

// ********************************************************************************
/// <summary>
/// 查询当前设置的日志类型名称
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/10/21</created>
/// <changed>Andy,2020/10/21</changed>
// ********************************************************************************
std::string CLogger::QueryLevelName() const
{
	std::string strResult(GetLevelName(m_nLevelMask));

	return strResult;
}

// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <param name="pFileName"></param>
/// <param name="nLevel"></param>
/// <param name="nRotateSize"></param>
/// <returns></returns>
/// <created>Andy,2020/10/21</created>
/// <changed>Andy,2020/10/21</changed>
// ********************************************************************************
bool CLogger::Open(const char *pFileName, const LEVEL_TYPE nLevel/* = LEVEL_ALL*/, const uint64_t nRotateSize/* = 0*/)
{
	bool bResult = false;
	std::lock_guard<std::recursive_mutex> objLocker(m_csLock);

	m_osFile.open(pFileName, std::ios::binary | std::ios::out| std::ios::trunc);

	if (m_osFile.is_open())
	{
		m_strFileName = pFileName;
		this->m_nLevelMask = nLevel;
		this->m_nRotateSize = nRotateSize;
		m_osFile.seekp(0, std::ios::end);       //设置文件指针到文件流的尾部
		
		std::streampos ps = m_osFile.tellp();   //读取文件指针的位置

		m_objStats.m_nCurSize = ps;
		bResult = true;
	}

	return bResult;
}

// ********************************************************************************
/// <summary>
/// 关闭文件
/// </summary>
/// <created>Andy,2020/10/21</created>
/// <changed>Andy,2020/10/21</changed>
// ********************************************************************************
void CLogger::Close()
{
	std::lock_guard<std::recursive_mutex> objLocker(m_csLock);

	m_osFile.close();
}

// ********************************************************************************
/// <summary>
/// 分割日志文件
/// </summary>
/// <created>Andy,2020/10/21</created>
/// <changed>Andy,2020/10/21</changed>
// ********************************************************************************
bool CLogger::Rotate()
{
	std::lock_guard<std::recursive_mutex> objLocker(m_csLock);

	m_osFile.close();

	std::string strNewFileName = m_strFileName;
	std::string strTime;

	CAIMTime::GetCurTime("%Y-%m-%d %H:%M", strTime);
	strNewFileName += strTime;

	const int bResult = rename(this->m_strFileName.c_str(), strNewFileName.c_str());

	if (bResult == -1)
	{
		return false;
	}

	std::string strFile = m_strFileName;

	Open(strFile.c_str(), m_nLevelMask, m_nRotateSize);
	m_objStats.m_nCurSize = 0;

	return true;
}


// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <param name="nLevel"></param>
/// <param name="fmt"></param>
/// <param name="ap"></param>
/// <returns></returns>
/// <created>Andy,2020/10/21</created>
/// <changed>Andy,2020/10/21</changed>
// ********************************************************************************
size_t CLogger::Logv(const LEVEL_TYPE nLevel, const char *fmt, va_list ap)
{
	std::lock_guard<std::recursive_mutex> objLocker(m_csLock);

	if (!(m_nLevelMask & nLevel))
	{
		return 0;
	}

	std::string strLogMsg;
	
	CAIMTime::GetCurTime("%Y-%m-%d %H:%M", strLogMsg);
	strLogMsg += " ";
	strLogMsg += GetLevelName(nLevel);

	char szBuufer[LOG_BUF_LEN_MAX] = { 0 };

	vsnprintf(szBuufer, LOG_BUF_LEN_MAX, fmt, ap);
	strLogMsg += szBuufer;
	strLogMsg += "\r\n";

	m_osFile << strLogMsg;
	m_osFile.flush();

#ifdef WIN32
#ifdef _DEBUG
	ATLTRACE("%s", strLogMsg.c_str());
#endif
#else
#ifdef DEBUG
	std::cout << strLogMsg << std::endl;
#endif
#endif // DEBUG

	m_objStats.m_nCurSize += strLogMsg.size();
	m_objStats.m_nTotalSize += strLogMsg.size();

	if (m_nRotateSize > 0 && m_objStats.m_nCurSize > m_nRotateSize) 
	{
		this->Rotate();
	}


	return strLogMsg.size();
}

size_t CLogger::Trace(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	size_t nResult = Logv(CLogger::LEVEL_TRACE, fmt, ap);
	va_end(ap);
	return nResult;
}

size_t CLogger::Debug(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	size_t nResult = Logv(CLogger::LEVEL_DEBUG, fmt, ap);
	va_end(ap);
	return nResult;
}

size_t CLogger::Info(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	size_t nResult = Logv(CLogger::LEVEL_INFO, fmt, ap);
	va_end(ap);
	return nResult;
}

size_t CLogger::Warning(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	size_t nResult = Logv(CLogger::LEVEL_WARN, fmt, ap);
	va_end(ap);
	return nResult;
}

size_t CLogger::Error(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	size_t nResult = Logv(CLogger::LEVEL_ERROR, fmt, ap);
	va_end(ap);
	return nResult;
}

size_t CLogger::Fatal(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	size_t nResult = Logv(CLogger::LEVEL_FATAL, fmt, ap);
	va_end(ap);
	return nResult;
}



// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <param name="lpFileName"></param>
/// <param name="nLevel"></param>
/// <param name="nRotateSize"></param>
/// <returns></returns>
/// <created>Andy,2020/10/21</created>
/// <changed>Andy,2020/10/21</changed>
// ********************************************************************************
int CLoggerHelper::Open(
	const char * lpFileName
	, const CLogger::LEVEL_TYPE nLevel/* = CLogger::LEVEL_ALL*/
	, uint64_t nRotateSize /*= 0*/)
{
	return m_objLogger.Open(lpFileName, nLevel, nRotateSize);
}

// --------------------------------------------------------------------------------
/// <summary>
/// 输出运行日志
/// </summary>
/// <param name="nLevel">严重程度</param>
/// <param name="fmt">格式</param>
/// <param name="...">参数</param>
/// <created>黄丽云,2019/1/19</created>
/// <changed>黄丽云,2019/1/19</changed>
// --------------------------------------------------------------------------------
void CLoggerHelper::Log(const CLogger::LEVEL_TYPE nLevel, const char * fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	m_objLogger.Logv(nLevel, fmt, ap);
	va_end(ap);
}
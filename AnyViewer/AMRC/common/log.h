/////////////////////////////////////////////////////////////////////
/// @file log.h
/// @brief 日志类定义
/// 
/// @author andy
/// @version 1.0
/// @date 2020.10.21
/////////////////////////////////////////////////////////////////////
#pragma once
#include "SDPSingleton.h"

#include <mutex>
#include <fstream>

//*******************************************************************
/// <summary>
/// 日志记录类定义
/// </summary>
//*******************************************************************
class CLogger
{
public:	

	/// <summary>日志类型枚举定义</summary>
	enum LEVEL_TYPE
	{
		LEVEL_MIN = 0,
		LEVEL_FATAL = 1,
		LEVEL_ERROR = 2,
		LEVEL_WARN = 4,
		LEVEL_INFO = 8,
		LEVEL_DEBUG = 16,
		LEVEL_TRACE = 32,
		LEVEL_ALL = 63,
		LEVEL_MAX = 64,
	};

	static LEVEL_TYPE QueryLevel(const char *pLevelname);

public:
	CLogger();
	~CLogger();

public:
	LEVEL_TYPE GetLevel() const	{	return m_nLevelMask;}

	void SetLevel(const LEVEL_TYPE nLevel)	{this->m_nLevelMask = nLevel;}

	std::string QueryLevelName() const;

	std::string GetFileName() const {	return m_strFileName;}

	uint64_t GetRotateSize() const {	return m_nRotateSize;}

public:

	// 打开日志文件
	bool Open(const char *pFileName, const LEVEL_TYPE nLevel = LEVEL_ALL, const uint64_t nRotateSize = 0);

	// 关闭文件
	void Close();

	// 输入日志到文件
	size_t Logv(const LEVEL_TYPE nLevel, const char *fmt, va_list ap);

	size_t Trace(const char *fmt, ...);

	size_t Debug(const char *fmt, ...);

	size_t Info(const char *fmt, ...);

	size_t Warning(const char *fmt, ...);

	size_t Error(const char *fmt, ...);

	size_t Fatal(const char *fmt, ...);

private:
	bool Rotate();

private:
	std::ofstream            m_osFile;                   ///< 保存日志的文件流对象 
	std::string              m_strFileName;              ///< 日志文件名称
	LEVEL_TYPE               m_nLevelMask = LEVEL_ALL;   ///< 需要保存的日志掩码
	std::recursive_mutex     m_csLock;	
	uint64_t                 m_nRotateSize = 0;          ///< 分割日志文件的额阈值 

	struct
	{
		uint64_t m_nCurSize;
		uint64_t m_nTotalSize;
	}m_objStats;    ///< 文件统计信息，用于分割日志文件
};


// --------------------------------------------------------------------------------
/// <summary>
/// 
/// </summary>
// --------------------------------------------------------------------------------
class CLoggerHelper
{
private:
	CLoggerHelper()	{}

public:
	static CLoggerHelper * Instance()
	{
		static CLoggerHelper * pT = nullptr;

		if (nullptr == pT)
		{
			pT = new CLoggerHelper;
		}

		return pT;
	}

public:
	// 打开日志文件
	int Open(const char * lpFileName, const CLogger::LEVEL_TYPE nLevel = CLogger::LEVEL_ALL, uint64_t nRotateSize = 0);

	//输出日志信息到文件
	void Log(const CLogger::LEVEL_TYPE nLevel, const char *fmt, ...);

	CLogger::LEVEL_TYPE GetLevel() const { return m_objLogger.GetLevel(); }
	
	void SetLevel(const CLogger::LEVEL_TYPE nLevel) { m_objLogger.SetLevel(nLevel); }

	std::string GetFileName() const { return m_objLogger.GetFileName(); }
private:
	CLogger              m_objLogger;
};



#ifdef WIN32

#ifndef _DEBUG
#define LOG_TRACE(fmt, ...)	\
			CLoggerHelper::Instance()->Log(CLogger::LEVEL_TRACE, "%s(%d): " fmt, __FILE__, __LINE__, __VA_ARGS__)
#else
#define LOG_TRACE(fmt, ...) do{}while(0)
#endif

#ifndef LOG_INFO
#ifdef _DEBUG
#define LOG_INFO(fmt, ...) CLoggerHelper::Instance()->Log(CLogger::LEVEL_INFO, "%s(%d): " fmt, __FILE__, __LINE__,__VA_ARGS__)
#else
#define  LOG_INFO(fmt, ...) ;
#endif // DEBUG
#endif // LOG_INFO

#ifndef LOG_DEBUG
#define LOG_DEBUG(fmt, ...)	\
		CLoggerHelper::Instance()->Log(CLogger::LEVEL_DEBUG, "%s(%d): " fmt, __FILE__, __LINE__, __VA_ARGS__)
#endif

#ifndef LOG_WARN
#define LOG_WARN(fmt, ...)	\
		CLoggerHelper::Instance()->Log(CLogger::LEVEL_WARN,  "%s(%d): " fmt, __FILE__, __LINE__, __VA_ARGS__)
#endif

#ifndef LOG_ERROR
#define LOG_ERROR(fmt, ...)	\
		CLoggerHelper::Instance()->Log(CLogger::LEVEL_ERROR, "%s(%d): " fmt, __FILE__, __LINE__, __VA_ARGS__)
#endif

#ifndef LOG_FATAL
#define LOG_FATAL(fmt, ...)	\
		CLoggerHelper::Instance()->Log(CLogger::LEVEL_FATAL, "%s(%d): " fmt, __FILE__, __LINE__, __VA_ARGS__)
#endif

#else
#ifdef NDEBUG
#define LOG_TRACE(fmt, args...) do{}while(0)
#else
#define LOG_TRACE(fmt, args...)	\
			CLoggerHelper::Instance()->Log(CLogger::LEVEL_TRACE, "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#endif

#ifndef LOG_DEBUG
#define LOG_DEBUG(fmt, args...)	\
		CLoggerHelper::Instance()->Log(CLogger::LEVEL_DEBUG, "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#endif


#ifndef LOG_INFO
#define LOG_INFO(fmt, args...)	\
		CLoggerHelper::Instance()->Log(CLogger::LEVEL_INFO,  "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#endif

#ifndef LOG_WARN
#define LOG_WARN(fmt, args...)	\
		CLoggerHelper::Instance()->Log(CLogger::LEVEL_WARN,  "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#endif

#ifndef LOG_ERROR
#define LOG_ERROR(fmt, args...)	\
		CLoggerHelper::Instance()->Log(CLogger::LEVEL_ERROR, "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#endif

#ifndef LOG_FATAL
#define LOG_FATAL(fmt, args...)	\
		CLoggerHelper::Instance()->Log(CLogger::LEVEL_FATAL, "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#endif

#endif

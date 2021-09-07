//
//  Log.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//

#include "Log.h"
#include "CAIMTime.h"

#include <algorithm>
#include <iostream>
#include <stdarg.h>

const size_t LEVEL_NAME_LEN_MAX = 8;
const size_t LOG_BUF_LEN_MAX = 4096*2;


// ********************************************************************************
/// <summary>
/// ∏˘æ›»’÷æ¿‡–Õµ√µΩ∆‰√˚≥∆
/// </summary>
/// <param name="nLevel">»’÷æ¿‡–Õ</param>
/// <returns>»’÷æ√˚≥∆</returns>
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
/// ∏˘æ›√˚≥∆≤È—Ø∆‰∂‘”¶µƒ»’÷æ¿‡–Õ
/// </summary>
/// <param name="pLevelname">»’÷æ¿‡–Õ√˚≥∆</param>
/// <returns>»’÷æ¿‡–Õ</returns>
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
/// ≤È—Øµ±«∞…Ë÷√µƒ»’÷æ¿‡–Õ√˚≥∆
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
        m_osFile.seekp(0, std::ios::end);       //…Ë÷√Œƒº˛÷∏’ÎµΩŒƒº˛¡˜µƒŒ≤≤ø
        
        std::streampos ps = m_osFile.tellp();   //∂¡»°Œƒº˛÷∏’ÎµƒŒª÷√

        m_objStats.m_nCurSize = ps;
        bResult = true;
    }

    return bResult;
}

// ********************************************************************************
/// <summary>
/// πÿ±’Œƒº˛
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
/// ∑÷∏Ó»’÷æŒƒº˛
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

    CAIMTime::GetCurTime("%Y-%m-%d %H:%M:%S", strLogMsg);
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
    
//    return 0;
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
///  ‰≥ˆ‘À––»’÷æ
/// </summary>
/// <param name="nLevel">—œ÷ÿ≥Ã∂»</param>
/// <param name="fmt">∏Ò Ω</param>
/// <param name="...">≤Œ ˝</param>
/// <created>ª∆¿ˆ‘∆,2019/1/19</created>
/// <changed>ª∆¿ˆ‘∆,2019/1/19</changed>
// --------------------------------------------------------------------------------
void CLoggerHelper::Log(const CLogger::LEVEL_TYPE nLevel, const char * fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    m_objLogger.Logv(nLevel, fmt, ap);
    va_end(ap);
}

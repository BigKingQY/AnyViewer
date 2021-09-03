//
//  Log.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//

#ifndef Log_h
#define Log_h

#include <mutex>
#include <fstream>

//*******************************************************************
/// <summary>
/// »’÷æº«¬º¿‡∂®“Â
/// </summary>
//*******************************************************************
class CLogger
{
public:

    /// <summary>»’÷æ¿‡–Õ√∂æŸ∂®“Â</summary>
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
    LEVEL_TYPE GetLevel() const    {    return m_nLevelMask;}

    void SetLevel(const LEVEL_TYPE nLevel)    {this->m_nLevelMask = nLevel;}

    std::string QueryLevelName() const;

    std::string GetFileName() const {    return m_strFileName;}

    uint64_t GetRotateSize() const {    return m_nRotateSize;}

public:

    // ¥Úø™»’÷æŒƒº˛
    bool Open(const char *pFileName, const LEVEL_TYPE nLevel = LEVEL_ALL, const uint64_t nRotateSize = 0);

    // πÿ±’Œƒº˛
    void Close();

    //  ‰»Î»’÷æµΩŒƒº˛
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
    std::ofstream            m_osFile;                   ///< ±£¥Ê»’÷æµƒŒƒº˛¡˜∂‘œÛ
    std::string              m_strFileName;              ///< »’÷æŒƒº˛√˚≥∆
    LEVEL_TYPE               m_nLevelMask = LEVEL_ALL;   ///< –Ë“™±£¥Êµƒ»’÷æ—⁄¬Î
    std::recursive_mutex     m_csLock;
    uint64_t                 m_nRotateSize = 0;          ///< ∑÷∏Ó»’÷æŒƒº˛µƒ∂Ó„–÷µ

    struct
    {
        uint64_t m_nCurSize;
        uint64_t m_nTotalSize;
    }m_objStats;    ///< Œƒº˛Õ≥º∆–≈œ¢£¨”√”⁄∑÷∏Ó»’÷æŒƒº˛
};


// --------------------------------------------------------------------------------
/// <summary>
///
/// </summary>
// --------------------------------------------------------------------------------
class CLoggerHelper
{
private:
    CLoggerHelper()    {}

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
    // ¥Úø™»’÷æŒƒº˛
    int Open(const char * lpFileName, const CLogger::LEVEL_TYPE nLevel = CLogger::LEVEL_ALL, uint64_t nRotateSize = 0);

    // ‰≥ˆ»’÷æ–≈œ¢µΩŒƒº˛
    void Log(const CLogger::LEVEL_TYPE nLevel, const char *fmt, ...);

    CLogger::LEVEL_TYPE GetLevel() const { return m_objLogger.GetLevel(); }
    
    void SetLevel(const CLogger::LEVEL_TYPE nLevel) { m_objLogger.SetLevel(nLevel); }

    std::string GetFileName() const { return m_objLogger.GetFileName(); }
private:
    CLogger              m_objLogger;
};



#ifdef NDEBUG
#define LOG_TRACE(fmt, args...) do{}while(0)
#else
#define LOG_TRACE(fmt, args...)    \
            CLoggerHelper::Instance()->Log(CLogger::LEVEL_TRACE, "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#endif

#ifndef LOG_DEBUG
#define LOG_DEBUG(fmt, args...)    \
        CLoggerHelper::Instance()->Log(CLogger::LEVEL_DEBUG, "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#endif


#ifndef LOG_INFO
#define LOG_INFO(fmt, args...)    \
        CLoggerHelper::Instance()->Log(CLogger::LEVEL_INFO,  "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#endif

#ifndef LOG_WARN
#define LOG_WARN(fmt, args...)    \
        CLoggerHelper::Instance()->Log(CLogger::LEVEL_WARN,  "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#endif

#ifndef LOG_ERROR
#define LOG_ERROR(fmt, args...)    \
        CLoggerHelper::Instance()->Log(CLogger::LEVEL_ERROR, "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#endif

#ifndef LOG_FATAL
#define LOG_FATAL(fmt, args...)    \
        CLoggerHelper::Instance()->Log(CLogger::LEVEL_FATAL, "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#endif

#endif

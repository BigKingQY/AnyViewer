//
//  CPrjSettings.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/2.
//


#ifndef CPrjSettings_h
#define CPrjSettings_h

//#include "JsonSerializer.h"
#include "EccCode.h"
#include "Log.h"
#include <string>
#include <mutex>


// --------------------------------------------------------------------------------
/// <summary>
/// ¡¨Ω”π˝¿˙ ∑ªÔ∞È’À∫≈
/// </summary>
// --------------------------------------------------------------------------------
class CHistoryPartner : public CSerializeEntry
{
public:
    CHistoryPartner() {}
    CHistoryPartner(const std::string& refDeviceID)
        : m_strDeviceID(refDeviceID)
    {}
    ~CHistoryPartner() {}

    CHistoryPartner& operator=(const CHistoryPartner& refSrc);

    ENABLE_SERIALIZE(CHistoryPartner)
public:
    //–Ú¡–ªØ…Ë÷√–≈œ¢
    virtual void Serialize(CJSONSerializer& refJsonSerializer) const override;

    //∑¥–Ú¡–ªØ…Ë÷√–≈œ¢
    virtual void DeSerialize(CJSONSerializer& refJsonSerializer) override;

private:
    ///< ¥”∑˛ŒÒ∆˜ªÒ»°µƒ…Ë±∏ID£¨≤ª–Ë“™±£¥Ê
    DECLARE_MEMBER_AND_METHOD(std::string, m_strDeviceID, DeviceID);

    ///< ”√ªßµ«¬º π”√µƒø⁄¡Ó
    DECLARE_MEMBER_AND_METHOD(std::string, m_strPwd, Pwd);
};

using CHistoryPartnerPtr = std::shared_ptr<CHistoryPartner>;
using CHistoryPartnerArray = CSharedDynamicJsonVector<CHistoryPartner>;

class CPrjSettings : public CSerializeEntry
{
public:
    CPrjSettings();
    virtual ~CPrjSettings();

    CPrjSettings(const CPrjSettings& refSrc);

    CPrjSettings& operator=(const CPrjSettings& refSrc);


    /// <summary>ÕºœÒ÷ ¡ø¿‡–Õ√∂æŸ∂®“Â</summary>
    enum IMAGE_QUALITY_TYPE
    {
        IQT_AUTOMATIC = 0,               ///< √∂æŸ≥£¡ø,◊‘∂Ø
        IQT_IMAGE_QUALITY_PRIORITY = 1,  ///< √∂æŸ≥£¡ø,ÕºœÒ÷ ¡ø”≈œ»
        IQT_DISPLAY_SPEED_PRIORITY = 2,  ///< √∂æŸ≥£¡ø,ÀŸ∂»”≈œ»
    };

    /// <summary>”Ô—‘¿‡–Õ∂®“Â</summary>
    enum LANGUAGE_TYPE
    {
        LT_UNKNOWN = 0,        ///< √∂æŸ≥£¡ø,Œ¥÷™
        LT_ENGLISH = 1033,    ///< √∂æŸ≥£¡ø,”¢Œƒ
        LT_CHINEASE = 2052,   ///< √∂æŸ≥£¡ø,÷–Œƒ(ºÚÃÂ)
        LT_GERMAN = 1031,   ///< √∂æŸ≥£¡ø,µ¬”Ô
        LT_FRENCH = 1036,   ///< √∂æŸ≥£¡ø,∑®”Ô
        LT_ITALIAN = 1040,   ///< √∂æŸ≥£¡ø,“‚¥Û¿˚”Ô
        LT_SPANISH = 1034,   ///< √∂æŸ≥£¡ø,Œ˜∞‡—¿”Ô
        LT_JAPANESE = 1041,   ///< √∂æŸ≥£¡ø,»’”Ô
        LT_CHINESETW = 1028,   ///< √∂æŸ≥£¡ø,÷–Œƒ(∑±ÃÂ)
    };

    /// <summary>÷˜Ã‚¿‡–Õ</summary>
    enum THEME_TYPE
    {
        TT_FOLLOW_SYSTEM = 0,    ///< √∂æŸ≥£¡ø,
        TT_LIGHT_MODE = 1,       ///< √∂æŸ≥£¡ø,
        TT_DARK_MODE = 2,        ///< √∂æŸ≥£¡ø,
    };

    /// <summary>–£—È√‹¬ÎµƒΩ·π˚¿‡–Õ∂®“Â</summary>
    enum VALID_PWD_RET
    {
        VPR_INVALID = 0,    ///< √∂æŸ≥£¡ø,≤ª∫œ∑®
        VPR_EMPTY = 1,    ///< √∂æŸ≥£¡ø,ø’
        VPR_TOO_SHORT = 2,    ///< √∂æŸ≥£¡ø,Ã´∂Ã
        VPR_TOO_SIMPLE = 4,    ///< √∂æŸ≥£¡ø,Ã´ºÚµ•
        VPR_VALID = 8,    ///< √∂æŸ≥£¡ø,∫œ∑®
    };

    /// <summary>ø⁄¡Óµƒ∞≤»´«ø∂»</summary>
    enum PASSWORD_SECURITY_STRENGTH
    {
        PSS_INVALID = 0,         ///< √∂æŸ≥£¡ø,Œﬁ–ß
        PSS_DANGER = 1,          ///< √∂æŸ≥£¡ø,Œ£œ’
        PSS_LOW_DANGER = 2,      ///< √∂æŸ≥£¡ø,∑Áœ’
        PSS_SECURE = 3,          ///< √∂æŸ≥£¡ø,∞≤»´
        PSS_HIGH_SECURE = 4,     ///< √∂æŸ≥£¡ø,∑«≥£∞≤»´
    };

    /// <summary>∂œø™¡¨Ω”∫Ûµƒ≤Ÿ◊˜¿‡–Õ√∂æŸ</summary>
    enum DISCONNECT_ACTION_TYPE
    {
        DA_DO_NOTHING = 0,           ///< √∂æŸ≥£¡ø, ≤√¥∂º≤ª◊ˆ(ƒ¨»œ)
        DA_LOCK_WORKSTATION = 1,     ///< √∂æŸ≥£¡ø,À¯◊°π§◊˜Ã®
        DA_LOGOUT_WORKSTATION = 2    ///< √∂æŸ≥£¡ø,◊¢œ˙π§◊˜Ã®
    };

    using CUINTArray = CStaticJsonVector<unsigned int>;

public:
    CUINTArray& GetRouteIPs() { std::lock_guard<std::recursive_mutex> objLocker(m_objMutex);  return m_arrRouteIPs; }

    // …Ë÷√÷∏∂®Ã¯ ˝µƒ¬∑”…IP
    void SetRouteIP(const unsigned int nTTL, const unsigned int nIP);

    // µ√µΩ÷∏∂®Ã¯ ˝µƒ¬∑”…IP
    unsigned int GetRouteIP(const unsigned int nTTL);

    // µ√µΩÀ˘”–¬∑”…IP£¨∂‡∏ˆipµÿ÷∑”√∑÷∫≈∑÷∏Ó
    std::string GetRouteIPsAsString() const;

    //  «Â≥˝Œﬁ–ßµƒ¬∑”…IP
    void ClearInvalidRouteIP();

    ///< πÃ∂®ø⁄¡Ó£¨”√ªß…Ë÷√
    std::string GetPwd() const { std::lock_guard<std::recursive_mutex> objLocker(m_objMutex);  return m_strPwd; }

    ///< µ√µΩ±√˚
    std::string GetNickName() const { std::lock_guard<std::recursive_mutex> objLocker(m_objMutex);  return m_strNickName; }

    ///< µ√µΩ∑˛ŒÒ∆˜IP
    std::string GetServerIP() const { std::lock_guard<std::recursive_mutex> objLocker(m_objMutex);  return m_strServerIP; }

    /// µ√µΩµ±«∞”Ô—‘µƒtag
    std::string GetLanguageTag();
public:
    //< ‘ˆº”¿˙ ∑ªÔ∞Èº«¬º
    void AppendHistoryPartner(CHistoryPartnerPtr pHistoryPartner);

    // ≤È’“¿˙ ∑ªÔ∞È
    CHistoryPartnerPtr LookupHistoryPartner(const std::string& refDeviceID) const;

    // µ√µΩ¿˙ ∑ªÔ∞È¡–±Ì
    const CHistoryPartnerArray& GetHistoryPartner() const;

    // «Â≥˝À˘”–µƒ¿˙ ∑ª·ª∞º«¬º
    void ClearAllHistoryPartner();

    // «Â≥˝À˘”–µƒ¿˙ ∑ª·ª∞º«¬ºµƒ∞≤»´¬Î
    void ClearAllHistoryPartnerSecCode();
public:
    // –£—È√‹¬Î «∑Ò∫œ∑®
//    VALID_PWD_RET VerifyPwd(const std::string& refPwd) const;

    // ∑÷Œˆ√‹¬Îµƒ∞≤»´–‘
    PASSWORD_SECURITY_STRENGTH AnalyzePwdSecurity(const std::string& refPwd) const;

public:
    //–Ú¡–ªØ…Ë÷√–≈œ¢
    virtual void Serialize(CJSONSerializer& refJsonSerializer) const override;

    //∑¥–Ú¡–ªØ…Ë÷√–≈œ¢
    virtual void DeSerialize(CJSONSerializer& refJsonSerializer) override;

public:
    //±£¥Êπ§≥Ã…Ë÷√µΩ¥≈≈ÃŒƒº˛
    bool Save(const std::string& refPath) const;

    //¥”¥≈≈ÃŒƒº˛º”‘ÿπ§≥Ã…Ë÷√
    bool Load(const std::string& refPath);

    // ∆Ù”√ƒ¨»œ…Ë÷√
    void DefaultSettings();

    // ªÿ∏¥…Ë÷√
    void Reconver(CPrjSettings& refSrc);
public:
    // …Ë÷√aesº”√‹key
    void SetAESEncipherKey(const UCHAR* pKey);

    // ≈–∂œµ±«∞µƒ√‹¬Î «∑Ò «±»ΩœºÚµ•
//    bool IsSimplePwd(const std::string& refPwd) const;

    // Ω‚√‹ø⁄¡Ó
    std::string DecryptPwd(const std::string& refPwd) const;

    // º”√‹ø⁄¡Ó
    std::string EncryptPwd(const std::string & refPwd) const;

private:
    ///<aesº”√‹∂‘œÛ,”√”⁄º”√‹∑¢ÀÕ ˝æ›
    struct aes_ctx                      m_objAESEncipherKey;

    ///< º”‘ÿªÚ±£¥ÊÀ¯
    mutable std::recursive_mutex        m_objMutex;

    ///< ∆Ù”√∏√—°œÓ∫Û£¨≤≈ø…“‘Ω” ‹∆‰À¸øÿ÷∆’ﬂµƒ«Î«Û
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bEnableCtrl, EnableCtrl, true);

    ///<  «∑Ò‘ –Ì¡Ÿ ±ø⁄¡Ó,∆Ù”√∏√—°œÓ∫Û£¨”¶”√≥Ã–Úª·≤˙…˙“ª∏ˆ¡Ÿ ±ø⁄¡Ó
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bEnableTmpPwd, EnableTmpPwd, true);

    ///< ¥”∑˛ŒÒ∆˜ªÒ»°µƒ…Ë±∏ID£¨≤ª–Ë“™±£¥Ê
    DECLARE_MEMBER_AND_METHOD(std::string, m_strDeviceID, DeviceID);

    ///< ¡Ÿ ±ø⁄¡Ó£¨≤ª–Ë“™±£¥Ê
    DECLARE_MEMBER_AND_METHOD(std::string, m_strTmpPwd, TmpPwd);

    ///<  «∑Ò‘ –Ì¡Ÿ ±ø⁄¡Ó,∆Ù”√∏√—°œÓ∫Û£¨”¶”√≥Ã–Úª·≤˙…˙“ª∏ˆ¡Ÿ ±ø⁄¡Ó
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bEnableFixPwd, EnableFixPwd, false);

    ///< πÃ∂®ø⁄¡Ó£¨”√ªß…Ë÷√
    DECLARE_MEMBER_AND_SET_METHOD(std::string, m_strPwd, Pwd);

    ///< Í«≥∆
    DECLARE_MEMBER_AND_SET_METHOD_V11(std::string, m_strNickName, NickName,"");

    ///< Õº–Œ÷ ¡ø¿‡–Õ…Ë÷√,@see IMAGE_QUALITY_TYPE
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nImageQuality, ImageQuality, IQT_IMAGE_QUALITY_PRIORITY);

    ///<  «∑Ò”∞≤ÿ◊¿√Ê±⁄÷Ω
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bHideWallPage, HideWallPage, false);

    ///< ◊‘∂Ø∆Ù∂Ø,ÀÊwindows∆Ù∂Ø
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bAutoStart, AutoStart, true);

    ///< œµÕ≥ π”√µƒ”Ô—‘¿‡–Õ(»Áπ˚ «LT_UNKNOWN£¨œµÕ≥Ω´◊‘∂ØºÏ≤‚),@see LANGUAGE_TYPE
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nLanguage, Language, LT_ENGLISH);

    ///< ÷˜Ã‚¿‡–Õ,@see THEME_TYPE
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nTheme, Theme, TT_FOLLOW_SYSTEM);

    ///< RCøÿ÷∆∑˛ŒÒ∆˜IPµÿ÷∑
    DECLARE_MEMBER_AND_SET_METHOD_V11(std::string, m_strServerIP, ServerIP, "192.168.3.74");
    
    ///< RCøÿ÷∆∑˛ŒÒ∆˜∂Àø⁄
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nPort, Port, 30192);

    ///< When flag is set server always blocks local input.
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bLockLocalInput, LockLocalInput, false);

    ///< When flag is set server blocks remote input on local input activity.
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bLocalInputPriority, LocalInputPriority, false);

    //
// Local input invactivity timeout during that
// we still blocking remote input(when m_localInputPriority
// is enabled).
//
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nLocalInputPriorityTimeout, LocalInputPriorityTimeout, 3);

    // –Ë“™±£¥Êµƒ»’÷æµ»º∂
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nLogLevel, LogLevel, CLogger::LEVEL_ERROR);

    ///<  Polling configuration
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nPollingInterval, PollingInterval, 50);

    ///<
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bGrabTransparentWindowsFlag, GrabTransparentWindowsFlag, true);

    ///<
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bMirrorDriverAllowed, MirrorDriverAllowed, false);

    ///<
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bD3DAllowed, D3DAllowed, true);

    ///<
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bBlockRemoteInput, BlockRemoteInput, false);

    ///< ∑˛ŒÒ∆˜∂Àø⁄
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nRFBPort, RFBPort, 30193);

    ///< ∂œø™¡¨Ω”≤Ÿ◊˜
    DECLARE_MEMBER_AND_METHOD_V11(DISCONNECT_ACTION_TYPE, m_nDisconnectAction, DisconnectAction, DA_DO_NOTHING);

    ///<  «∑Ò◊‹ «π≤œÌ◊¿√Ê
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bAlwaysShared, AlwaysShared, false);

    ///<  «∑Òæ¯≤ªπ≤œÌ◊¿√Ê
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bNeverShared, NeverShared, true);

    ///<  «∑Ò∂œµ±«∞¥Ê‘⁄µƒ¡¨Ω”
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bDisconnectClients, DisconnectClients, true);
    
    ///  «∑Ò‘ –Ì ‰»ÎCtrl+Alt+Del◊È∫œº¸
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bCtrlAltDelEnabled, CtrlAltDelEnabled,true);

    /// øÕªß∂À «∑Ò‘∏“‚π≤œÌ◊¿√Ê
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bShared, SharedFlag, false);


    ///< ¡¨Ω”≈‰÷√…Ë÷√
//    DECLARE_MEMBER_AND_METHOD(CConnectionConfig, m_objConnectionConfig, ConnectionConfig);

    ///<  «∑Ò∑÷Œˆ¡ÀÕ¯¬Áª∑æ≥
    DECLARE_MEMBER_AND_METHOD_V11(bool,m_bAnalyzedNetEnv, AnalyzedNetEnv,false);

    ///< ¬∑”…IP±Ì
    CUINTArray               m_arrRouteIPs;

    ///< øÿ÷∆∑˛ŒÒ∆˜∂Àø⁄
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nConsolePort, ConsolePort, 30196);

    ///< Ω” ’∆¡ƒª≤…ºØ ˝æ›µƒ∂Àø⁄
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nScreenCapPort, ScreenCapPort, 30197);

    ///< ¿˙ ∑ªÔ∞Èº«¬º¡–±Ì
    CHistoryPartnerArray        m_arrHistoryPartners;

    ///< ≥Ã–Ú…˝º∂–≈œ¢Œƒº˛µƒURLµÿ÷∑
    DECLARE_MEMBER_AND_GET_METHOD_V11(std::string, m_strUpdateFileUrl, UpdateFileUrl, "http://www2.aomeisoftware.com/download/rc/rcupgrade.ini");

    /// < ”√ªß «∑Ò≤Œ”ÎÃÂ—Èº∆ªÆ
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bUserExperience, UserExperience, false);

    /// < µ⁄“ª¥Œ π”√µƒ ±º‰(æ‡¿Î1970-01-01 00:00:00µƒÃÏ ˝)
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nFirstTimeUse, FirstTimeUse, 0);

    /// < ◊ÓΩ¸“ª¥Œ π”√µƒ ±º‰(æ‡¿Î1970-01-01 00:00:00µƒÃÏ ˝)
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nLastTimeUse, LastTimeUse, 0);

    /// < ∫ˆ¬‘≤ª–Ë“™…˝º∂µƒ∞Ê±æ
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nIgnoretheversion, Ignoretheversion, 0);

    /// < ºº ı÷ß≥÷µƒ¡¨Ω”
    DECLARE_MEMBER_AND_METHOD_V11(std::string, m_strTechnicalSupportURL, TechnicalSupportURL, "http://www.anyviewer.com/support.html");
    
    DECLARE_MEMBER_AND_METHOD(std::string, m_strRegion, Region);

    //  «∑Ò“—æ≠‘ƒ∂¡≥ı ºªØ…Ë÷√
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bInitSettingsReaded, InitSettingsReaded,false);

    /// < ∞Ô÷˙÷––ƒURL
    DECLARE_MEMBER_AND_METHOD_V11(std::string, m_strHepeCenterURL, HepeCenterURL, "https://anyviewer.aomeitech.com/support.html");

    /// < —˚«Î≈Û”—URL
    DECLARE_MEMBER_AND_METHOD_V11(std::string, m_strInviteFriendsURL, InviteFriendsURL, "https://anyviewer.aomeitech.com/invite-friends.html");

    ///< œµÕ≥ π”√µƒ”Ô—‘¿‡–Õ(»Áπ˚ «LT_UNKNOWN£¨œµÕ≥Ω´◊‘∂ØºÏ≤‚),@see LANGUAGE_TYPE
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nOldLanguage, OldLanguage, LT_ENGLISH);

    ///<  «∑Ò±£¥Ê±ªøÿ÷∆∂Àµƒ∞≤»´¬Î
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bSaveSecCode, SaveSecCode, true);

    ///< ”¶”√≥Ã–Ú∞Ê±æ–≈œ¢
    DECLARE_MEMBER_AND_METHOD_V11(int, m_nVersion, Version, 0);

};

using CPrjSettingsPtr = std::shared_ptr<CPrjSettings>;

extern CPrjSettings* GetPrjSettings();

#endif

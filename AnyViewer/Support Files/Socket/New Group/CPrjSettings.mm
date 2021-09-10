//
//  CPrjSettings.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/2.
//

#import "CPrjSettings.h"
#include "PacketBase.h"
#include "GlobeFunction.h"
#include "CJsonHelper.h"

const unsigned int PWD_TOO_SHORT_THRD = 5;       ///< √‹¬ÎÃ´∂Ãµƒ„–÷µ
const UCHAR ECC_KEY[] = "8E0BA18EDBE946ED";      ///< ”√”⁄º”√‹µƒkey

const char* PRJSETTINGS_DATA_UUID = "438E1E29-8253-463E-9359-227F13BEB098";  //ºÙÃ˘∞Â ˝æ›Œ®“ª±Í ∂
const unsigned int PRJSETTINGS_DATA_VERSION = 103;  //ºÙÃ˘∞Â ˝æ›∏Ò Ω∞Ê±æ
const char* PRJSETTINGS_DATA_DESC = "RCClient Project Settings";  //π§≥Ã…Ë÷√

const unsigned int LETTER_KIND_THRD = 6;       ///< ∞≤»´√‹¬Î–Ë“™µƒ◊Ó…Ÿ◊÷∑˚÷÷¿‡ ˝„–÷µ
const unsigned int LETTER_COUNT_THRD = 10;     ///< ∑«≥£∞≤»´√‹¬Î–Ë“™µƒ◊Ó…Ÿ◊÷∑˚ ˝„–÷µ
const unsigned int SIMPLE_PWD_LETTER_KIND_THRD = 3;     ///< ºÚµ•√‹¬Î◊÷∑˚÷÷¿‡„–÷µ

REGISTER_CLASS(CHistoryPartner)

void CHistoryPartner::Serialize(CJSONSerializer& refJsonSerializer) const
{
    CSerializeEntry::Serialize(refJsonSerializer);

    CPrjSettings* pPrjSettings = GetPrjSettings();
    std::string&& refPwdEncrypted = pPrjSettings->EncryptPwd(m_strPwd);
    

    refJsonSerializer.Serialize("m1", m_strDeviceID);
    refJsonSerializer.Serialize("m2", refPwdEncrypted);
}

CHistoryPartner& CHistoryPartner::operator=(const CHistoryPartner& refSrc)
{
    if (this == &refSrc)
    {
        return *this;
    }

    m_strDeviceID = refSrc.m_strDeviceID;

    return *this;
}

void CHistoryPartner::DeSerialize(CJSONSerializer& refJsonSerializer)
{
    CSerializeEntry::DeSerialize(refJsonSerializer);

    CPrjSettings* pPrjSettings = GetPrjSettings();
    std::string strPwd;

    refJsonSerializer.DeSerialize("m1", m_strDeviceID);
    refJsonSerializer.DeSerialize("m2", strPwd);

    m_strPwd = pPrjSettings->DecryptPwd(strPwd);
}

/*-----------------------------------CPrjSettings--------------------------------------------------*/
CPrjSettings::CPrjSettings()
{
    SetAESEncipherKey(ECC_KEY);
}

CPrjSettings::~CPrjSettings()
{
}

CPrjSettings::CPrjSettings(const CPrjSettings & refSrc)
: m_bEnableCtrl(refSrc.m_bEnableCtrl)
, m_bEnableTmpPwd(refSrc.m_bEnableTmpPwd)
, m_strTmpPwd(refSrc.m_strTmpPwd)
, m_bEnableFixPwd(refSrc.m_bEnableFixPwd)
, m_strPwd(refSrc.m_strPwd)
, m_strNickName(refSrc.m_strNickName)
, m_nImageQuality(refSrc.m_nImageQuality)
, m_bAutoStart(refSrc.m_bAutoStart)
, m_nLanguage(refSrc.m_nLanguage)
, m_nTheme(refSrc.m_nTheme)
, m_strServerIP(refSrc.m_strServerIP)
, m_nPort(refSrc.m_nPort)
, m_bSaveSecCode(refSrc.m_bSaveSecCode)
{
}

CPrjSettings & CPrjSettings::operator=(const CPrjSettings & refSrc)
{
    if (this == &refSrc)
    {
        return *this;
    }

    m_bEnableCtrl = refSrc.m_bEnableCtrl;
    m_bEnableTmpPwd = refSrc.m_bEnableTmpPwd;
    m_strTmpPwd = refSrc.m_strTmpPwd;
    m_bEnableFixPwd = refSrc.m_bEnableFixPwd;
    m_strPwd = refSrc.m_strPwd;
    m_strNickName = refSrc.m_strNickName;
    m_nImageQuality = refSrc.m_nImageQuality;
    m_bHideWallPage = refSrc.m_bHideWallPage;
    m_bAutoStart = refSrc.m_bAutoStart;
    m_nLanguage = refSrc.m_nLanguage;
    m_nTheme = refSrc.m_nTheme;
    m_strServerIP = refSrc.m_strServerIP;
    m_nPort = refSrc.m_nPort;
    m_bSaveSecCode = refSrc.m_bSaveSecCode;

    return *this;
}


// ********************************************************************************
/// <summary>
/// µ√µΩ√‹¬Î
/// </summary>
/// <param name="refPwd">±ªΩ‚√‹µƒ√‹¬Î</param>
/// <returns>Ω‚√‹∫Ûµƒ√‹¬Î</returns>
/// <created>Andy,2020/10/19</created>
/// <changed>Andy,2020/10/19</changed>
// ********************************************************************************
std::string CPrjSettings::DecryptPwd(const std::string& refPwd) const
{
    std::string strResult;

    if (refPwd.size() > 0)
    {
        CDataStreamBuffer objDataBuffer1(refPwd.size());
        CDataStreamBuffer objDataBuffer2(refPwd.size());

        ConvertHexTextToBin(refPwd, (char *)objDataBuffer1.GetBufferHeadPtr(), refPwd.size());
        aes_aes(&m_objAESEncipherKey, objDataBuffer1.GetBufferHeadPtr(), refPwd.size(), objDataBuffer2.GetBufferHeadPtr(), AES_DECRYPT);

        strResult = std::string((char *)objDataBuffer2.GetBufferHeadPtr());
    }

    return strResult;
}

static const char chHexTable[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
#define TOHEX(a, b) {*b++ = chHexTable[a >> 4]; *b++ = chHexTable[a & 0xf];}
void ConvertBinToHexText(const unsigned char* pInBuffer, const int nLen, std::string& strOut)
{
    strOut.clear();

    if (nullptr != pInBuffer)
    {
        for (int i = 0; i < nLen; i++)
        {
            strOut += chHexTable[*pInBuffer >> 4];
            strOut += chHexTable[*pInBuffer & 0x0f];
            pInBuffer++;
        }
    }
}

// ********************************************************************************
/// <summary>
/// º”√‹ø⁄¡Ó
/// </summary>
/// <param name="refPwd">±ªº”√‹µƒø⁄¡Ó</param>
/// <returns>Ω‚√‹∫Û∂Óø⁄¡Ó</returns>
/// <created>Andy,2020/10/19</created>
/// <changed>Andy,2020/10/19</changed>
// ********************************************************************************
std::string CPrjSettings::EncryptPwd(const std::string & refPwd) const
{
    std::string strResult;

    if (refPwd.size() > 0)
    {
        const unsigned int CIPHERTEXT_LEND = CalcAlign(refPwd.size(), 16);
        CDataStreamBuffer objDataBuffer(CIPHERTEXT_LEND + 4);

        aes_aes(&m_objAESEncipherKey, (unsigned char *)refPwd.c_str(), refPwd.size(), objDataBuffer.GetBufferHeadPtr(), AES_ENCRYPT);
        ConvertBinToHexText((unsigned char *)objDataBuffer.GetBufferHeadPtr(), CIPHERTEXT_LEND, strResult);
    }

    return strResult;
}



// ********************************************************************************
/// <summary>
/// ≈–∂œµ±«∞µƒ√‹¬Î «∑Ò «±»ΩœºÚµ•
/// </summary>
/// <param name="refPwd">±ªºÏ≤‚µƒø⁄¡Ó</param>
/// <returns>true: «,false:∑Ò</returns>
/// <created>Andy,2020/10/19</created>
/// <changed>Andy,2020/10/19</changed>
// ********************************************************************************
//bool CPrjSettings::IsSimplePwd(const std::string& refPwd) const
//{
//    std::map<int, int> mapLetterStat;              ///< ◊÷∑˚Õ≥º∆
//
//    for (size_t i = 0; i < refPwd.size(); i++)
//    {
//        const char chCur = refPwd[i];
//        mapLetterStat[chCur] += 1;
//    }
//
//    if (mapLetterStat.size() <= SIMPLE_PWD_LETTER_KIND_THRD)
//    {
//        return true;
//    }
//
//
//    const static TCHAR* SIIMPLE_PWD[] =
//    {
//        _T("password")
//        ,_T("Password")
//        ,_T("PassWord")
//        ,_T("PASSWORD")
//        ,_T("securitycode")
//        ,_T("security code")
//        ,_T("Securitycode")
//        ,_T("Security code")
//        ,_T("SecurityCode")
//        ,_T("Security Code")
//        ,_T("SECURITYCODE")
//        ,_T("SECURITY CODE")
//        ,_T("123456")
//        ,_T("1234567")
//        ,_T("12345678")
//        ,_T("123456789")
//        ,_T("1234567890")
//        ,_T("qwerty")
//        ,_T("qwertyu")
//        ,_T("qwertyuio")
//        ,_T("qwertyuiop")
//        ,_T("QWERTY")
//        ,_T("QWERTYU")
//        ,_T("QWERTYUI")
//        ,_T("QWERTYUIO")
//        ,_T("QWERTYUIOP")
//        ,_T("qwertyui")
//    };
//
//    bool bResult = false;
//    const CString strPwd(refPwd.c_str());
//
//    for (int i = 0; i < sizeof(SIIMPLE_PWD) / sizeof(TCHAR*); i++)
//    {
//        if (strPwd == SIIMPLE_PWD[i])
//        {
//            bResult = true;
//            break;
//        }
//    }
//
//    return bResult;
//}

// ********************************************************************************
/// <summary>
/// …Ë÷√÷∏∂®Ã¯ ˝µƒ¬∑”…IP
///
/// µ⁄“ªÃ¯µƒ–Ú∫≈ «0
/// </summary>
/// <param name="nTTL">Ã¯ ˝</param>
/// <param name="nIP">¬∑”…∆˜IP</param>
/// <created>Andy,2020/11/25</created>
/// <changed>Andy,2020/11/25</changed>
// ********************************************************************************
void CPrjSettings::SetRouteIP(const unsigned int nTTL, const unsigned int nIP)
{
    std::lock_guard<std::recursive_mutex> objLocker(m_objMutex);

    if (nTTL < m_arrRouteIPs.size())
    {
        m_arrRouteIPs[nTTL] = nIP;
    }
    else
    {
        m_arrRouteIPs.push_back(nIP);
    }
}

// ********************************************************************************
/// <summary>
/// µ√µΩ÷∏∂®Ã¯ ˝µƒ¬∑”…IP
/// </summary>
/// <param name="nTTL">Ã¯ ˝</param>
/// <returns>¬∑”…∆˜IP</returns>
/// <created>Andy,2020/11/25</created>
/// <changed>Andy,2020/11/25</changed>
// ********************************************************************************
unsigned int CPrjSettings::GetRouteIP(const unsigned int nTTL)
{
    std::lock_guard<std::recursive_mutex> objLocker(m_objMutex);
    unsigned int nResult = 0;

    if (nTTL < m_arrRouteIPs.size())
    {
        nResult = m_arrRouteIPs[nTTL];
    }

    return nResult;
}

// ********************************************************************************
/// <summary>
/// µ√µΩÀ˘”–¬∑”…IP£¨∂‡∏ˆipµÿ÷∑”√∑÷∫≈∑÷∏Ó
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/11/25</created>
/// <changed>Andy,2020/11/25</changed>
// ********************************************************************************
std::string CPrjSettings::GetRouteIPsAsString() const
{
    std::lock_guard<std::recursive_mutex> objLocker(m_objMutex);
    std::string strResult;
    char szBuffer[20] = { 0 };

    for (auto nIP: m_arrRouteIPs)
    {
        if (strResult.size() == 0)
        {
            strResult = FormatHostIPAddressAnsi(nIP, szBuffer);
        }
        else
        {
            strResult += ";";
            strResult += FormatHostIPAddressAnsi(nIP, szBuffer);
        }
    }

    return strResult;
}

// ********************************************************************************
/// <summary>
///  «Â≥˝Œﬁ–ßµƒ¬∑”…IP(÷˜“™ «∞—¡–±Ìƒ©Œ≤Œ™0µƒ«Â≥˝)
/// </summary>
/// <created>Andy,2021/2/25</created>
/// <changed>Andy,2021/2/25</changed>
// ********************************************************************************
void CPrjSettings::ClearInvalidRouteIP()
{
    std::lock_guard<std::recursive_mutex> objLocker(m_objMutex);

    while (m_arrRouteIPs.size() > 0)
    {
        if (*(m_arrRouteIPs.rbegin()) == 0)
        {
            m_arrRouteIPs.pop_back();
        }
        else
        {
            break;
        }
    }
}

// ********************************************************************************
/// <summary>
/// µ√µΩµ±«∞”Ô—‘µƒtag
/// </summary>
/// <returns>”Ô—‘µƒ◊÷∑˚¥Æ±Í ∂</returns>
/// <created>Andy,2021/5/25</created>
/// <changed>Andy,2021/5/25</changed>
// ********************************************************************************
std::string CPrjSettings::GetLanguageTag()
{
    std::string strResult("en");

    switch (GetLanguage())
    {
    case CPrjSettings::LT_CHINEASE:   ///< √∂æŸ≥£¡ø,÷–Œƒ
        strResult = ("cn");
        break;

    case CPrjSettings::LT_GERMAN:   ///< √∂æŸ≥£¡ø,µ¬Œƒ
        strResult = ("de");
        break;

    case CPrjSettings::LT_FRENCH:   ///< √∂æŸ≥£¡ø,÷–Œƒ
        strResult = ("fr");
        break;

    case CPrjSettings::LT_ITALIAN:   ///< √∂æŸ≥£¡ø,÷–Œƒ
        strResult = ("it");
        break;

    case CPrjSettings::LT_SPANISH:   ///< √∂æŸ≥£¡ø,÷–Œƒ
        strResult = ("es");
        break;

    case CPrjSettings::LT_JAPANESE:   ///< √∂æŸ≥£¡ø,÷–Œƒ
        strResult = ("jp");
        break;

    case CPrjSettings::LT_CHINESETW:   ///< √∂æŸ≥£¡ø,÷–Œƒ
        strResult = ("tw");
        break;

    default:
        strResult = ("en");
    }

    return strResult;
}

// ********************************************************************************
/// <summary>
/// ‘ˆº”¿˙ ∑ªÔ∞Èº«¬º
/// </summary>
/// <param name="pHistoryPartner"></param>
/// <created>Andy,2021/2/3</created>
/// <changed>Andy,2021/2/3</changed>
// ********************************************************************************
void CPrjSettings::AppendHistoryPartner(CHistoryPartnerPtr pHistoryPartner)
{
    CHECK_POINTER(pHistoryPartner);

    {
        std::lock_guard<std::recursive_mutex> objLocker(m_objMutex);

        m_arrHistoryPartners.push_back(pHistoryPartner);
    }
}

// ********************************************************************************
/// <summary>
/// ≤È’“¿˙ ∑ªÔ∞È
/// </summary>
/// <param name="refDeviceID"></param>
/// <returns></returns>
/// <created>Andy,2021/2/3</created>
/// <changed>Andy,2021/2/3</changed>
// ********************************************************************************
CHistoryPartnerPtr CPrjSettings::LookupHistoryPartner(const std::string& refDeviceID) const
{
    std::lock_guard<std::recursive_mutex> objLocker(m_objMutex);
    CHistoryPartnerPtr pResult = nullptr;

    for (auto pPartner : m_arrHistoryPartners)
    {
        if (pPartner->GetDeviceID() == refDeviceID)
        {
            pResult = pPartner;
            break;
        }
    }

    return pResult;
}

// ********************************************************************************
/// <summary>
/// µ√µΩ¿˙ ∑ªÔ∞È¡–±Ì
/// </summary>
/// <returns></returns>
/// <created>Andy,2021/2/3</created>
/// <changed>Andy,2021/2/3</changed>
// ********************************************************************************
const CHistoryPartnerArray& CPrjSettings::GetHistoryPartner() const
{
    std::lock_guard<std::recursive_mutex> objLocker(m_objMutex);

    return m_arrHistoryPartners;
}

// ********************************************************************************
/// <summary>
/// «Â≥˝À˘”–µƒ¿˙ ∑ª·ª∞º«¬º
/// </summary>
/// <created>Andy,2021/2/3</created>
/// <changed>Andy,2021/2/3</changed>
// ********************************************************************************
void CPrjSettings::ClearAllHistoryPartner()
{
    std::lock_guard<std::recursive_mutex> objLocker(m_objMutex);

    m_arrHistoryPartners.clear();
}

// ********************************************************************************
/// <summary>
/// «Â≥˝À˘”–µƒ¿˙ ∑ª·ª∞º«¬ºµƒ∞≤»´¬Î
/// </summary>
/// <created>Andy,2021/7/21</created>
/// <changed>Andy,2021/7/21</changed>
// ********************************************************************************
void CPrjSettings::ClearAllHistoryPartnerSecCode()
{
    std::lock_guard<std::recursive_mutex> objLocker(m_objMutex);

    for (auto pPartner : m_arrHistoryPartners)
    {
        pPartner->SetPwd("");
    }
}

// ********************************************************************************
/// <summary>
/// –£—È√‹¬Î «∑Ò∫œ∑®
/// </summary>
/// <param name="refPwd">±ª–£—Èµƒ√‹¬Î</param>
/// <returns>∑µªÿVALID_PWD_RET¿‡µƒ–£—ÈΩ·π˚</returns>
/// <created>Andy,2020/10/16</created>
/// <changed>Andy,2020/10/19</changed>
// ********************************************************************************
//CPrjSettings::VALID_PWD_RET CPrjSettings::VerifyPwd(const std::string& refPwd) const
//{
//    VALID_PWD_RET nResult = VPR_VALID;
//
//    if (refPwd.size() > 0)
//    {
//        if (GetNickName() == refPwd)
//        {
//            nResult = VPR_TOO_SIMPLE;
//        }
//        else
//        {
//            if (refPwd.size() > PWD_TOO_SHORT_THRD)
//            {
//                if (IsSimplePwd(refPwd))
//                {
//                    nResult = VPR_TOO_SIMPLE;
//                }
//            }
//            else
//            {
//                nResult = VPR_TOO_SHORT;
//            }
//        }
//    }
//    else
//    {
//        nResult = VPR_EMPTY;
//    }
//
//    return nResult;
//}

// ********************************************************************************
/// <summary>
/// ∑÷Œˆ√‹¬Îµƒ∞≤»´–‘
/// </summary>
/// <param name="refPwd">±ª∑÷Œˆµƒ√‹¬Î</param>
/// <returns>@see PASSWORD_SECURITY_STRENGTH</returns>
/// <created>Andy,2020/10/19</created>
/// <changed>Andy,2020/10/19</changed>
// ********************************************************************************
CPrjSettings::PASSWORD_SECURITY_STRENGTH CPrjSettings::AnalyzePwdSecurity(const std::string& refPwd) const
{
    if (refPwd.size() == 0)
    {
        // ø’√‹¬Î
        return PSS_INVALID;
    }

//    const VALID_PWD_RET nValidRet = VerifyPwd(refPwd);
//
//    if (VPR_TOO_SHORT == nValidRet || VPR_TOO_SIMPLE == nValidRet)
//    {
//        return PSS_DANGER;
//    }

    /// <summary>◊÷ƒ∏÷÷¿‡√∂æŸ¿‡–Õ</summary>
    enum LETTER_KIND
    {
        LK_UPPERCASE = 1,      ///< √∂æŸ≥£¡ø£¨¥Û–¥
        LK_LOWERCASECASE = 2,  ///< √∂æŸ≥£¡ø£¨–°–¥
        LK_NUMERAL = 4,        ///< √∂æŸ≥£¡ø£¨ ˝◊÷
        LK_SPECIAL = 8,        ///< √∂æŸ≥£¡ø£¨Ãÿ ‚
    };

    PASSWORD_SECURITY_STRENGTH nResult = PSS_INVALID;
    unsigned int nLetterKindFlag = 0;
    std::map<int, int> mapLetterStat;              ///< ◊÷∑˚Õ≥º∆

    for (size_t i = 0; i < refPwd.size(); i++)
    {
        const char chCur = refPwd[i];

        mapLetterStat[chCur] += 1;

        if (48 <= chCur && chCur <= 57)
        {
            //  ˝◊÷◊÷∑˚

            nLetterKindFlag |= LK_NUMERAL;
        }
        else if (65 <= chCur && chCur <= 90)
        {
            // ¥Û–¥◊÷∑˚

            nLetterKindFlag |= LK_UPPERCASE;
        }
        else if (97 <= chCur && chCur <= 122)
        {
            // –°–¥◊÷∑˚

            nLetterKindFlag |= LK_LOWERCASECASE;
        }
        else
        {
            // Ãÿ ‚◊÷∑˚

            nLetterKindFlag |= LK_SPECIAL;
        }
    }

    if (mapLetterStat.size() >= LETTER_KIND_THRD /*¥Û”⁄6÷÷◊÷∑˚*/
        && nLetterKindFlag& LK_NUMERAL /*¥Ê‘⁄ ˝◊÷*/
        && nLetterKindFlag& (LK_UPPERCASE | LK_LOWERCASECASE) /*¥Ê‘⁄◊÷∑˚*/)
    {
        if (nLetterKindFlag& LK_SPECIAL
            && nLetterKindFlag& LK_UPPERCASE
            && nLetterKindFlag& LK_LOWERCASECASE
            && refPwd.size() > LETTER_COUNT_THRD)
        {
            // ±‹√‚¡À≈–∂œŒ™°∞Œ£œ’°±µƒÃıº˛£¨∞¸∫¨¡À¥Û–¥◊÷ƒ∏+–°–¥◊÷ƒ∏+ ˝◊÷+Ãÿ ‚∑˚∫≈£¨◊È≥…√‹¬Îµƒ◊÷∑˚◊‹π≤¥Û”⁄µ»”⁄6÷÷£¨√‹¬Î◊‹≥§∂»≥¨π˝10Œª°£
            nResult = PSS_HIGH_SECURE;
        }
        else
        {
            // ±‹√‚¡À≈–∂œŒ™°∞Œ£œ’°±µƒÃıº˛£¨∞¸∫¨¡À◊÷ƒ∏£®≤ª∑÷¥Û–°–¥£©+ ˝◊÷£¨◊È≥…√‹¬Îµƒ◊÷∑˚◊‹π≤¥Û”⁄µ»”⁄6÷÷£¨µ´√ª”–¬˙◊„¥ÔµΩ°∞∑«≥£∞≤»´°±µƒÃıº˛°£
            nResult = PSS_SECURE;
        }
    }
    else
    {
        nResult = PSS_LOW_DANGER;
    }

    return nResult;
}


void CPrjSettings::Serialize(CJSONSerializer& refJsonSerializer) const
{
    CSerializeEntry::Serialize(refJsonSerializer);

    std::string&& refPwdEncrypted =  EncryptPwd(m_strPwd);

    refJsonSerializer.Serialize("m1", m_bEnableCtrl);
    refJsonSerializer.Serialize("m2", m_bEnableTmpPwd);
    refJsonSerializer.Serialize("m3", m_strTmpPwd);
    refJsonSerializer.Serialize("m4", m_bEnableFixPwd);
    refJsonSerializer.Serialize("m5", refPwdEncrypted);
    refJsonSerializer.Serialize("m6", m_strNickName);
    refJsonSerializer.Serialize("m7", m_nImageQuality);
    refJsonSerializer.Serialize("m9", m_bAutoStart);
    refJsonSerializer.Serialize("m10", m_nLanguage);
    refJsonSerializer.Serialize("m11", m_nTheme);
    refJsonSerializer.Serialize("m12", m_strServerIP);
    refJsonSerializer.Serialize("m13", m_nPort);
    refJsonSerializer.Serialize("m14", m_bAnalyzedNetEnv);
    refJsonSerializer.Serialize("m15", m_arrRouteIPs);
    refJsonSerializer.Serialize("m16", m_nConsolePort);
    refJsonSerializer.Serialize("m17", m_nScreenCapPort);
    refJsonSerializer.Serialize("m18", m_arrHistoryPartners);


    refJsonSerializer.Serialize("m23", m_nPollingInterval);
    refJsonSerializer.Serialize("m24", m_strUpdateFileUrl);
    refJsonSerializer.Serialize("m25", m_nFirstTimeUse);
    refJsonSerializer.Serialize("m26", m_nLastTimeUse);
    refJsonSerializer.Serialize("m27", m_nIgnoretheversion);
    refJsonSerializer.Serialize("m28", m_strTechnicalSupportURL);
    refJsonSerializer.Serialize("m29", m_bHideWallPage);
    refJsonSerializer.Serialize("m30", m_strRegion);
    refJsonSerializer.Serialize("m31", m_bInitSettingsReaded);
    
    refJsonSerializer.Serialize("m32", m_strHepeCenterURL);
    refJsonSerializer.Serialize("m33", m_strInviteFriendsURL);
    refJsonSerializer.Serialize("m34", m_bUserExperience);
    refJsonSerializer.Serialize("m35", m_nLogLevel);
    refJsonSerializer.Serialize("m36", m_bD3DAllowed);
    refJsonSerializer.Serialize("m37", m_bSaveSecCode);
    refJsonSerializer.Serialize("m38", m_nVersion);
}

void CPrjSettings::DeSerialize(CJSONSerializer & refJsonSerializer)
{
    CSerializeEntry::DeSerialize(refJsonSerializer);

    std::string strPwd;

    refJsonSerializer.DeSerialize("m1", m_bEnableCtrl);
    refJsonSerializer.DeSerialize("m2", m_bEnableTmpPwd);
    refJsonSerializer.DeSerialize("m3", m_strTmpPwd);
    refJsonSerializer.DeSerialize("m4", m_bEnableFixPwd);
    refJsonSerializer.DeSerialize("m5", strPwd);
    refJsonSerializer.DeSerialize("m6", m_strNickName);
    refJsonSerializer.DeSerialize("m7", m_nImageQuality);
    refJsonSerializer.DeSerialize("m9", m_bAutoStart);
    refJsonSerializer.DeSerialize("m10", m_nLanguage);
    refJsonSerializer.DeSerialize("m11", m_nTheme);


    refJsonSerializer.DeSerialize("m12", m_strServerIP);
    refJsonSerializer.DeSerialize("m13", m_nPort);
    refJsonSerializer.DeSerialize("m16", m_nConsolePort);
    refJsonSerializer.DeSerialize("m17", m_nScreenCapPort);

    refJsonSerializer.DeSerialize("m14", m_bAnalyzedNetEnv);
    refJsonSerializer.DeSerialize("m15", m_arrRouteIPs);
    refJsonSerializer.DeSerialize("m18", m_arrHistoryPartners);

    refJsonSerializer.DeSerialize("m23", m_nPollingInterval);
    refJsonSerializer.DeSerialize("m24", m_strUpdateFileUrl);
    refJsonSerializer.DeSerialize("m25", m_nFirstTimeUse);
    refJsonSerializer.DeSerialize("m26", m_nLastTimeUse);
    refJsonSerializer.DeSerialize("m27", m_nIgnoretheversion);
    refJsonSerializer.DeSerialize("m28", m_strTechnicalSupportURL);
    refJsonSerializer.DeSerialize("m29", m_bHideWallPage);
    refJsonSerializer.DeSerialize("m30", m_strRegion);
    refJsonSerializer.DeSerialize("m31", m_bInitSettingsReaded);

    refJsonSerializer.DeSerialize("m32", m_strHepeCenterURL);
    refJsonSerializer.DeSerialize("m33", m_strInviteFriendsURL);
    refJsonSerializer.DeSerialize("m34", m_bUserExperience);
    refJsonSerializer.DeSerialize("m35", m_nLogLevel);
    refJsonSerializer.DeSerialize("m36", m_bD3DAllowed);
    refJsonSerializer.DeSerialize("m37", m_bSaveSecCode);
    refJsonSerializer.DeSerialize("m38", m_nVersion);

    m_nOldLanguage = m_nLanguage;
    m_strPwd = DecryptPwd(strPwd);
    

//#ifdef _DEBUG
//    m_strServerIP = "192.168.3.74";
//#endif
}

//--------------------------------------------------------------------------------
/// <summary>
/// ±£¥Êπ§≥Ã…Ë÷√µΩ¥≈≈ÃŒƒº˛
/// </summary>
/// <param name="refPath">Œƒº˛¬∑æ∂</param>
/// <created>Andy,2020/7/2</created>
/// <changed>Andy,2020/7/2</changed>
//--------------------------------------------------------------------------------
bool CPrjSettings::Save(const std::string & refPath) const
{
    std::lock_guard<std::recursive_mutex> objLocker(m_objMutex);
    CJsonHelper objJsonHelper;
    const bool bResult = objJsonHelper.Save<CPrjSettings>(
        *this
        , refPath
        , PRJSETTINGS_DATA_UUID
        , PRJSETTINGS_DATA_VERSION
        , PRJSETTINGS_DATA_DESC);

    return bResult;


}

//--------------------------------------------------------------------------------
/// <summary>
/// ¥”¥≈≈ÃŒƒº˛º”‘ÿπ§≥Ã…Ë÷√
/// </summary>
/// <param name="refPath">Œƒº˛¬∑æ∂</param>
/// <created>Andy,2020/7/2</created>
/// <changed>Andy,2020/7/2</changed>
//--------------------------------------------------------------------------------
bool CPrjSettings::Load(const std::string & refPath)
{
    std::lock_guard<std::recursive_mutex> objLocker(m_objMutex);
    CJsonHelper objJsonHelper;
    const bool bResult = objJsonHelper.Load<CPrjSettings>(
        refPath
        , PRJSETTINGS_DATA_UUID
        , *this);

    return bResult;
}

// ********************************************************************************
/// <summary>
/// ∆Ù”√ƒ¨»œ…Ë÷√
/// </summary>
/// <created>Andy,2020/10/15</created>
/// <changed>Andy,2020/10/15</changed>
// ********************************************************************************
void CPrjSettings::DefaultSettings()
{
    m_bEnableCtrl = true;
    m_bEnableTmpPwd = true;
    m_strTmpPwd.clear();
    //m_bEnableFixPwd = false;
    m_strPwd.clear();
    m_strNickName = "";
    m_nImageQuality = IQT_IMAGE_QUALITY_PRIORITY;
    m_bAutoStart = true;
    //m_nLanguage = LT_ENGLISH;    ///<
    //m_nTheme = TT_FOLLOW_SYSTEM;

#ifdef _DEBUG
//    m_strServerIP = "controlserver.anyviewer.com";
    m_strServerIP = "192.168.3.74";
#endif
    m_nPort = 30192;
    m_nPollingInterval = 50;
    m_bHideWallPage = false;
    m_bSaveSecCode = true;
}

// ********************************************************************************
/// <summary>
/// ªÿ∏¥…Ë÷√
/// </summary>
/// <param name="refSrc">‘¥</param>
/// <created>Andy,2021/4/21</created>
/// <changed>Andy,2021/4/21</changed>
// ********************************************************************************
void CPrjSettings::Reconver(CPrjSettings& refSrc)
{
    m_bEnableCtrl = refSrc.m_bEnableCtrl;
    m_bEnableTmpPwd = refSrc.m_bEnableTmpPwd;
    m_strTmpPwd = refSrc.m_strTmpPwd;
    m_bEnableFixPwd = refSrc.m_bEnableFixPwd;
    m_strPwd = refSrc.m_strPwd;
    m_strNickName = refSrc.m_strNickName;
    m_nImageQuality = refSrc.m_nImageQuality;
    m_bHideWallPage = refSrc.m_bHideWallPage;
    m_bAutoStart = refSrc.m_bAutoStart; //œ¬“ª∏ˆ∞Ê±æ–Ë“™ª÷∏¥
    m_nLanguage = refSrc.m_nLanguage;
    m_nTheme = refSrc.m_nTheme;
    m_bLockLocalInput = refSrc.m_bLockLocalInput;
    m_bLocalInputPriority = refSrc.m_bLocalInputPriority;
    m_nLocalInputPriorityTimeout = refSrc.m_nLocalInputPriorityTimeout;
    m_nLogLevel = refSrc.m_nLogLevel; 
    m_nPollingInterval = refSrc.m_nPollingInterval;
    m_bGrabTransparentWindowsFlag = refSrc.m_bGrabTransparentWindowsFlag;
    m_bMirrorDriverAllowed = refSrc.m_bMirrorDriverAllowed;
    m_bD3DAllowed = refSrc.m_bD3DAllowed;
    m_bBlockRemoteInput = refSrc.m_bBlockRemoteInput;
    m_nDisconnectAction = refSrc.m_nDisconnectAction;
    m_bAlwaysShared = refSrc.m_bAlwaysShared;
    m_bNeverShared = refSrc.m_bNeverShared;
    m_bDisconnectClients = refSrc.m_bDisconnectClients;
    m_bCtrlAltDelEnabled = refSrc.m_bCtrlAltDelEnabled;

    refSrc.m_arrHistoryPartners.MoveTo(m_arrHistoryPartners);

    m_bShared = refSrc.m_bShared;
    m_nFirstTimeUse = refSrc.m_nFirstTimeUse;
    m_nLastTimeUse = refSrc.m_nLastTimeUse;
    m_bSaveSecCode = refSrc.m_bSaveSecCode;

    if (IQT_AUTOMATIC == m_nImageQuality)
    {
        // œ¬√Êµƒ¥˙¬Î «Œ™¡ÀºÊ»›¿œ∞Ê±æ
        m_nImageQuality = IQT_IMAGE_QUALITY_PRIORITY;
    }

}

// ********************************************************************************
/// <summary>
/// …Ë÷√º”√‹key
/// </summary>
/// <param name="pKey"></param>
/// <created>Andy,2020/10/19</created>
/// <changed>Andy,2020/10/19</changed>
// ********************************************************************************
void CPrjSettings::SetAESEncipherKey(const UCHAR * pKey)
{
    memset(&m_objAESEncipherKey, 0, sizeof(m_objAESEncipherKey));
    aes_set_key(&m_objAESEncipherKey, pKey, AES_KEY_LEN);
}

CPrjSettings* GetPrjSettings()
{
    return CGlobleSingleton<CPrjSettings>::Instance();
}



//
//  CJsonHelper.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/2.
//

#import "CJsonHelper.h"
#include "rapidjson/filereadstream.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*---------------------------CKeyValueList-------------------*/

//--------------------------------------------------------------------------------
/// <summary>
/// // ¥¥Ω®“ª∏ˆ◊÷∂Œ”≥…‰∂®“Â≤¢…Ë÷√∆‰"type" Ù–‘£®◊®”√∫Ø ˝£©
/// </summary>
/// <param name="lpName">◊÷∂Œ√˚≥∆</param>
/// <param name="lpType">¿‡–Õ</param>
/// <created>Andy,2020/4/5</created>
/// <changed>Andy,2020/4/5</changed>
//--------------------------------------------------------------------------------
std::shared_ptr<CKeyValueList> CKeyValueList::CreateFieldProp(
    const char* lpName
    , const char* lpType)
{
    std::shared_ptr<CKeyValueList> pResult = nullptr;
    std::shared_ptr<CKeyValue<CKeyValueList>> pFieldDef = std::make_shared<CKeyValue<CKeyValueList>>(
        lpName, std::make_shared<CKeyValueList>());

    if (nullptr != pFieldDef)
    {
        m_lstKeyValue.push_back(pFieldDef);
        pResult = pFieldDef->GetValue();

        if (nullptr != pResult)
        {
            pResult->CreateKV<std::string>("type", std::make_shared<std::string>(lpType));
        }
    }

    return pResult;
}

void CKeyValueList::Serialize(CJSONSerializer & refJsonSerializer) const
{
    CSerializeEntry::Serialize(refJsonSerializer);
    
    auto itr = m_lstKeyValue.begin();

    for (; m_lstKeyValue.end() != itr;itr++)
    {
        (*itr)->Serialize(refJsonSerializer);
    }
}

void CKeyValueList::DeSerialize(CJSONSerializer & refJsonSerializer)
{
    CSerializeEntry::DeSerialize(refJsonSerializer);
}

/*--------------------CJsonHelper------------------------------*/
CJsonHelper::CJsonHelper()
{
}


CJsonHelper::~CJsonHelper()
{

}
#ifdef WIN32

const NUM_TO_STR JSON_ERROR_MSG804[] =
{
    {rapidjson::kParseErrorDocumentEmpty,              _T("ø’Œƒµµ")},
{rapidjson::kParseErrorDocumentRootNotSingular,       _T("Œƒµµ∏˘ƒø¬º≤ªƒ‹∫Û∏˙∆‰À˚÷µ")},

{rapidjson::kParseErrorValueInvalid,                   _T("Œﬁ–ß÷µ")},

{rapidjson::kParseErrorObjectMissName,                _T("»±…Ÿ∂‘œÛ≥…‘±µƒ√˚≥∆")},
{rapidjson::kParseErrorObjectMissColon,               _T("∂‘œÛ≥…‘±√˚≥∆∫Û»±…Ÿ√∞∫≈")},
{rapidjson::kParseErrorObjectMissCommaOrCurlyBracket, _T("∂‘œÛ≥…‘±∫Û»±…Ÿ∂∫∫≈ªÚ°∞}°±")},

{rapidjson::kParseErrorArrayMissCommaOrSquareBracket,  _T(" ˝◊È‘™Àÿ∫Û»±…Ÿ∂∫∫≈ªÚ°∞]°±")},

{rapidjson::kParseErrorStringUnicodeEscapeInvalidHex,  _T("◊÷∑˚¥Æ◊™“Â∫Ûµƒ'\\u' Æ¡˘Ω¯÷∆ ˝◊÷≤ª’˝»∑")},
{rapidjson::kParseErrorStringUnicodeSurrogateInvalid,  _T("◊÷∑˚¥Æ÷–µƒ¥˙¿ÌœÓ∂‘Œﬁ–ß")},
{rapidjson::kParseErrorStringEscapeInvalid,            _T("◊÷∑˚¥Æ÷–µƒ◊™“Â◊÷∑˚Œﬁ–ß")},
{rapidjson::kParseErrorStringMissQuotationMark,        _T("◊÷∑˚¥Æ÷–»±…Ÿ”““˝∫≈")},
{rapidjson::kParseErrorStringInvalidEncoding,          _T("◊÷∑˚¥Æ÷–µƒ±‡¬ÎŒﬁ–ß")},

{rapidjson::kParseErrorNumberTooBig,                   _T(" ˝◊÷Ã´¥Û£¨≤ªƒ‹¥Ê¥¢‘⁄À´æ´∂»÷–")},
{rapidjson::kParseErrorNumberMissFraction,             _T("»±…Ÿ∑÷ ˝≤ø∑÷")},
{rapidjson::kParseErrorNumberMissExponent,             _T("»±…Ÿ÷∏ ˝")},

{rapidjson::kParseErrorTermination,                    _T("∑÷Œˆ“—÷’÷π")},
{rapidjson::kParseErrorUnspecificSyntaxError,         _T("∑«Ãÿ∂®”Ô∑®¥ÌŒÛ")}
};


const NUM_TO_STR   JSON_ERROR_MSG409[] =
{
    {rapidjson::kParseErrorDocumentEmpty,              _T("The document is empty")},
{rapidjson::kParseErrorDocumentRootNotSingular,        _T("The document root must not follow by other values")},

{rapidjson::kParseErrorValueInvalid,                   _T("Invalid value")},

{rapidjson::kParseErrorObjectMissName,                _T("Missing a name for object member")},
{rapidjson::kParseErrorObjectMissColon,                _T("Missing a colon after a name of object member")},
{rapidjson::kParseErrorObjectMissCommaOrCurlyBracket, _T("Missing a comma or '}' after an object member")},

{rapidjson::kParseErrorArrayMissCommaOrSquareBracket,  _T("Missing a comma or ']' after an array element")},

{rapidjson::kParseErrorStringUnicodeEscapeInvalidHex,  _T("Incorrect hex digit after \\u escape in string")},
{rapidjson::kParseErrorStringUnicodeSurrogateInvalid,  _T("The surrogate pair in string is invalid")},
{rapidjson::kParseErrorStringEscapeInvalid,           _T("Invalid escape character in string")},
{rapidjson::kParseErrorStringMissQuotationMark,        _T("Missing a closing quotation mark in string")},
{rapidjson::kParseErrorStringInvalidEncoding,         _T("Invalid encoding in string")},

{rapidjson::kParseErrorNumberTooBig,                   _T("Number too big to be stored in double")},
{rapidjson::kParseErrorNumberMissFraction,             _T("Miss fraction part in number")},
{rapidjson::kParseErrorNumberMissExponent,             _T("Miss exponent in number")},

{rapidjson::kParseErrorTermination,                    _T("Parsing was terminated")},
{rapidjson::kParseErrorUnspecificSyntaxError,           _T("Unspecific syntax error")}
};

//--------------------------------------------------------------------------------
/// <summary>
/// µ√µΩ¥ÌŒÛ–≈œ¢
/// </summary>
/// <param name="nLanguage">”Ô—‘</param>
/// <param name="nErrCode">¥ÌŒÛ¥˙¬Î</param>
/// <returns>¥ÌŒÛÃ· æ–≈œ¢</returns>
/// <created>Andy,2020/4/15</created>
/// <changed>Andy,2020/4/15</changed>
//--------------------------------------------------------------------------------
CString CJsonHelper::GetErrMsg(
    const unsigned int nLanguage
    , const rapidjson::ParseErrorCode nErrCode)
{
    CString strResult;

    if (nLanguage == 0x804)
    {
        strResult = GetDescStr(JSON_ERROR_MSG804, nErrCode);
    }
    else if (nLanguage == 0x409)
    {
        strResult = GetDescStr(JSON_ERROR_MSG409, nErrCode);
    }

    return strResult;
}

//-----------------------------------------------------------------------------
///@briefΩ‚ŒˆjsonµΩdom÷–
///@param [inout]refDom-dom∂‘œÛ
///@param [in]lpFile-¥˝Ω‚ŒˆµƒŒƒº˛
///@author leiyz
///@date 20180712
//-----------------------------------------------------------------------------
bool CJsonHelper::Decode(rapidjson::Document & refDom, LPCTSTR lpFile)
{
    FILE* pFile = nullptr;
    bool bResult = false;

    if (0 == _tfopen_s(&pFile, lpFile, _T("rb")))
    {
        char readBuffer[4096] = { 0 };
        rapidjson::FileReadStream objFileReadStream(pFile, readBuffer, sizeof(readBuffer));
        refDom.ParseStream(objFileReadStream);
        if (pFile)
        {
            fclose(pFile);
        }

        if (!refDom.HasParseError())
        {
            bResult = true;
        }
        else
        {
           // TRACE("json parse error offset: %d", refDom.GetErrorOffset());
        }
    }

    return bResult;
}
#endif

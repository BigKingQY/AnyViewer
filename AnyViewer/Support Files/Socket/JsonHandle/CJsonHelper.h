//
//  CJsonHelper.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/2.
//

#ifndef CJsonHelper_h
#define CJsonHelper_h

#include "JsonSerializer.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/error.h"
#include <fstream>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
///  Ù–‘∫Õ÷µ”≥…‰ƒ£∞Â¿‡
/// </summary>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
class CKeyValue : public CSerializeEntry
{
public:
    CKeyValue() {}
    CKeyValue(const char* lpName, std::shared_ptr<T> objValue)
        : m_strName(lpName)
        , m_objValue(objValue)
    {}
    ~CKeyValue() {}

public:
    virtual void Serialize(CJSONSerializer& refJsonSerializer) const override
    {
        CSerializeEntry::Serialize(refJsonSerializer);

        if (nullptr != m_objValue)
        {
            refJsonSerializer.Serialize(m_strName.c_str(), *m_objValue);
        }
    }

    virtual void DeSerialize(CJSONSerializer& refJsonSerializer) override
    {
        CSerializeEntry::DeSerialize(refJsonSerializer);

        if (nullptr != m_objValue)
        {
            refJsonSerializer.DeSerialize(m_strName.c_str(), *m_objValue);
        }
    }
private:
    DECLARE_MEMBER_AND_METHOD(std::string, m_strName, Name);
    DECLARE_MEMBER_AND_METHOD(std::shared_ptr<T>, m_objValue, Value);
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
///
/// </summary>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CKeyValueList : public CSerializeEntry
{
public:
    CKeyValueList() {}
    ~CKeyValueList() {}

public:
    template<class T>
    std::shared_ptr<CKeyValue<T>> CreateKV(const char* lpName, std::shared_ptr<T> objValue)
    {
        std::shared_ptr<CKeyValue<T>> pResult = std::make_shared<CKeyValue<T>>(lpName, objValue);

        m_lstKeyValue.push_back(pResult);
        return pResult;
    }


    // ¥¥Ω®“ª∏ˆ◊÷∂Œ”≥…‰∂®“Â≤¢…Ë÷√∆‰"type" Ù–‘£®◊®”√∫Ø ˝£©
    std::shared_ptr<CKeyValueList> CreateFieldProp(const char* lpName, const char* lpType);

public:
    virtual void Serialize(CJSONSerializer& refJsonSerializer) const override;

    virtual void DeSerialize(CJSONSerializer& refJsonSerializer) override;
public:
    std::vector<std::shared_ptr<CSerializeEntry>>   m_lstKeyValue;
};

class CJsonHelper
{
public:
    CJsonHelper();
    ~CJsonHelper();

public:


public:

#ifdef WIN32
    // µ√µΩ¥ÌŒÛ–≈œ¢
    static CString GetErrMsg(const unsigned int nLanguage, const rapidjson::ParseErrorCode nErrCode);

    bool Decode(rapidjson::Document& refDom, LPCTSTR lpFile);
#endif // WIN32

public:

    //--------------------------------------------------------------------------------
    /// <summary>
    ///  ±£¥Ê∂‘œÛŒ™json∏Ò Ω
    /// </summary>
    /// <param name="refObject">±ª±£¥Êµƒ∂‘œÛ</param>
    /// <param name="refBuffer">Ω” ’ ˝æ›µƒª∫≥Â«¯</param>
    /// <returns>true:≥…π¶,false: ß∞‹</returns>
    /// <created>Andy,2019/9/24</created>
    /// <changed>Andy,2019/9/24</changed>
    //--------------------------------------------------------------------------------
    template<class T>
    bool WriteToJson(const T& refObject,
        rapidjson::StringBuffer& refBuffer) const
    {
        bool bResult = false;

        rapidjson::Document objJsonDoc(rapidjson::kObjectType);    //…˙≥…“ª∏ˆdom‘™ÀÿDocument
        CJSONSerializer objJsonSerializerHelper(objJsonDoc.GetAllocator());

        refObject.Serialize(objJsonSerializerHelper);
        objJsonDoc.AddMember("data", objJsonSerializerHelper.GetJsonValue(), objJsonDoc.GetAllocator());

        rapidjson::Writer<rapidjson::StringBuffer> objWriter(refBuffer);
        bResult = objJsonDoc.Accept(objWriter);

        return bResult;
    }

    //----------------------------------------------------------------
    // @routine WriteToJson
    /// @brief ±£¥Ê∂‘œÛŒ™json∏Ò Ω
    ///
    /// @param °æ_in°ørefObject-±ª±£¥Êµƒ∂‘œÛ
    /// @param °æ_in°øpUUID- ˝æ› ∂±¬Î
    /// @param °æ_in°ønVersion-∞Ê±æ
    /// @param °æ_in°øpDesc-±Ì ˆ
    /// @param °æ_inout°ørefBuffer-Ω” ’ ˝æ›µƒª∫≥Â«¯
    /// @return true:≥…π¶,false: ß∞‹
    /// @author ª∆¿ˆ‘∆
    /// @date 2018-9-21
    //---------------------------------------------------------------
    template<class T>
    bool WriteToJson(
        const T& refObject,
        const char* pUUID,
        const unsigned int nVersion,
        const char* pDesc,
        rapidjson::StringBuffer& refBuffer) const
    {
        bool bResult = false;

        rapidjson::Document objJsonDoc(rapidjson::kObjectType);     //…˙≥…“ª∏ˆdom‘™ÀÿDocument
        rapidjson::Document::AllocatorType &objAllocator = objJsonDoc.GetAllocator();  //ªÒ»°∑÷≈‰∆˜

        objJsonDoc.AddMember("uuid", rapidjson::Value().SetString(pUUID, objAllocator).Move(), objAllocator);
        objJsonDoc.AddMember("version", rapidjson::Value(nVersion), objAllocator);
        objJsonDoc.AddMember("description", rapidjson::Value().SetString(pDesc, objAllocator).Move(), objAllocator);

        CJSONSerializer objJsonSerializerHelper(objJsonDoc.GetAllocator());

        refObject.Serialize(objJsonSerializerHelper);
        objJsonDoc.AddMember("data", objJsonSerializerHelper.GetJsonValue(), objJsonDoc.GetAllocator());

        rapidjson::Writer<rapidjson::StringBuffer> objWriter(refBuffer);
        bResult = objJsonDoc.Accept(objWriter);

        return bResult;
    }
    //----------------------------------------------------------------
    // @routine WriteToJson
    /// @brief ±£¥Ê∂‘œÛŒ™json∏Ò Ω
    ///
    /// @param °æ_in°ørefObject-±ª±£¥Êµƒ∂‘œÛ
    /// @param °æ_in°øpUUID- ˝æ› ∂±¬Î
    /// @param °æ_in°ønVersion-∞Ê±æ
    /// @param °æ_in°øpDesc-±Ì ˆ
    /// @param °æ_inout°ørefBuffer-Ω” ’ ˝æ›µƒª∫≥Â«¯
    /// @return true:≥…π¶,false: ß∞‹
    /// @author ª∆¿ˆ‘∆
    /// @date 2018-9-21
    //---------------------------------------------------------------
    template<class T>
    bool WriteToPrettyJson(
        T& refObject
        , const char* pUUID
        , const unsigned int nVersion
        , const char* pDesc
        , rapidjson::StringBuffer& refBuffer) const
    {
        bool bResult = false;

        rapidjson::Document objJsonDoc(rapidjson::kObjectType);    //…˙≥…“ª∏ˆdom‘™ÀÿDocument
        rapidjson::Document::AllocatorType &objAllocator = objJsonDoc.GetAllocator(); //ªÒ»°∑÷≈‰∆˜

        objJsonDoc.AddMember("uuid", rapidjson::Value().SetString(pUUID, objAllocator).Move(), objAllocator);
        objJsonDoc.AddMember("version", rapidjson::Value(nVersion), objAllocator);
        objJsonDoc.AddMember("description", rapidjson::Value().SetString(pDesc, objAllocator).Move(), objAllocator);

        CJSONSerializer objJsonSerializerHelper(objJsonDoc.GetAllocator());

        refObject.Serialize(objJsonSerializerHelper);
        objJsonDoc.AddMember("data", objJsonSerializerHelper.GetJsonValue(), objJsonDoc.GetAllocator());

        rapidjson::PrettyWriter<rapidjson::StringBuffer> objWriter(refBuffer);
        bResult = objJsonDoc.Accept(objWriter);

        return bResult;
    }

    //----------------------------------------------------------------
// @routine ReadFromJson
/// @brief ¥”json ˝æ›∂¡»° ˝◊Èº«¬º
///
/// @param °æ_in°øpJson- ˝æ›
/// @param °æ_in°ønLen- ˝æ›≥§∂»
/// @param °æ_in°øpUUID- ˝æ› ∂±¬Î
/// @param °æ_inout°ørefObject-∂‘œÛ≤Œøº
/// @return true:≥…π¶,false: ß∞‹
/// @author ª∆¿ˆ‘∆
/// @date 2018-9-21
//---------------------------------------------------------------
    template<class T>
    bool ReadFromJson(const std::string& refJsonData, T& refObject) const
    {
        bool bResult = false;
        rapidjson::Document objJsonDoc;

        objJsonDoc.Parse(refJsonData.c_str(), refJsonData.size());

        if (!objJsonDoc.IsNull())
        {
            auto itr = objJsonDoc.FindMember("data");

            if (objJsonDoc.MemberEnd() != itr)
            {
                CJSONSerializer objJsonSerializerHelper(objJsonDoc.GetAllocator());

                objJsonSerializerHelper.SetJsonValue(const_cast<rapidjson::Value&>(itr->value));
                refObject.DeSerialize(objJsonSerializerHelper);

                bResult = true;
            }
        }

        return bResult;
    }

    //----------------------------------------------------------------
    // @routine ReadFromJson
    /// @brief ¥”json ˝æ›∂¡»° ˝◊Èº«¬º
    ///
    /// @param °æ_in°øpJson- ˝æ›
    /// @param °æ_in°ønLen- ˝æ›≥§∂»
    /// @param °æ_in°øpUUID- ˝æ› ∂±¬Î
    /// @param °æ_inout°ørefObject-∂‘œÛ≤Œøº
    /// @return true:≥…π¶,false: ß∞‹
    /// @author ª∆¿ˆ‘∆
    /// @date 2018-9-21
    //---------------------------------------------------------------
    template<class T>
    bool ReadFromJson(const char* pJson, const size_t nLen, const char* pUUID, T& refObject) const
    {
        bool bResult = false;
        rapidjson::Document objJsonDoc;

        objJsonDoc.Parse(pJson, nLen);

        if (!objJsonDoc.IsNull())
        {
            std::string strClipboardUUID(pUUID);

            assert(strClipboardUUID == objJsonDoc["uuid"].GetString());

            if (strClipboardUUID == objJsonDoc["uuid"].GetString())
            {
                auto itr = objJsonDoc.FindMember("data");

                if (objJsonDoc.MemberEnd() != itr)
                {
                    CJSONSerializer objJsonSerializerHelper(objJsonDoc.GetAllocator());

                    objJsonSerializerHelper.SetJsonValue(const_cast<rapidjson::Value&>(itr->value));
                    refObject.DeSerialize(objJsonSerializerHelper);

                    bResult = true;
                }
            }
        }

        return bResult;
    }

    //----------------------------------------------------------------
    // @routine Export
    /// @brief µº≥ˆµ±«∞±Ì÷–µƒº«¬º
    ///
    /// @param strName-Œƒº˛√˚≥∆
    /// @param bAll-À˘”–º«¬º
    /// @return true:≥…π¶,false: ß∞‹
    /// @author ª∆¿ˆ‘∆
    /// @date 2018-9-21
    //---------------------------------------------------------------
    template<class T>
    bool Save(
        const T& refObject
        , const std::string& strName
        , const char* pUUID
        , const unsigned int nVersion
        , const char* pDesc) const
    {
        bool bResult = false;
        std::ofstream objFileWrited(strName);

        if (objFileWrited.is_open())
        {
            rapidjson::StringBuffer objBuffer;

            bResult = WriteToJson<T>(refObject, pUUID, nVersion, pDesc, objBuffer);
            assert(bResult);
#ifndef _DEBUG
            if (bResult)
#endif
            {
                auto out = objBuffer.GetString();

                objFileWrited.write(out, objBuffer.GetSize());
            }

            objFileWrited.close();
        }

        return bResult;
    }

    //----------------------------------------------------------------
    // @routine Export
    /// @brief ¥”Œƒº˛÷–µº»Îº«¬º
    ///
    /// @param strName-Œƒº˛√˚≥∆
    /// @param refItems-Ω” ‹º«¬º
    /// @return true:≥…π¶,false: ß∞‹
    /// @author ª∆¿ˆ‘∆
    /// @date 2018-9-21
    //---------------------------------------------------------------
    template<class T>
    bool Load(const std::string& strName, const char* pUUID, T& refObject)
    {
        bool bResult = false;
        std::ifstream objFileRead(strName, std::ios::in | std::ios::out | std::ios::binary);

        try
        {
            if (objFileRead.is_open())
            {
                objFileRead.seekg(0, std::ios::end);       //…Ë÷√Œƒº˛÷∏’ÎµΩŒƒº˛¡˜µƒŒ≤≤ø
                std::streampos ps = objFileRead.tellg();   //∂¡»°Œƒº˛÷∏’ÎµƒŒª÷√

                char* lpBuffer = new char[(int)ps + 1];

                if (nullptr != lpBuffer)
                {
                    objFileRead.seekg(0, std::ios::beg);          //…Ë÷√Œƒº˛÷∏’ÎµΩŒƒº˛¡˜µƒÕ∑≤ø

                    objFileRead.read(lpBuffer, ps);
                    bResult = ReadFromJson(lpBuffer, (size_t)ps, pUUID, refObject);

                    delete[]lpBuffer;
                }

                objFileRead.close();
            }
        }
        catch (...)
        {
            //TRACE(_T("Fail to load the file (%s)\r\n"), static_cast<LPCSTR>(strName));
        }

        return bResult;
    }

#ifdef WIN32
    //----------------------------------------------------------------
// @routine SaveDataToClipboard
/// @brief ±£¥Ê ˝æ›µΩÀΩ”–ºÙÃ˘∞Ê
///
/// @param refItems-±ª±£¥Êµƒ ˝◊È
/// @param lpName-√˚≥∆
/// @return true:≥…π¶£¨false: ß∞‹
/// @note ”–√˚≥∆±Ì æ «–° ˝æ›
/// @author ª∆¿ˆ‘∆
/// @date 2018-9-21
//---------------------------------------------------------------
    template<class T>
    bool SaveDataToClipboard(
        T& refObject
        , const char* pUUID
        , const unsigned int nVersion
        , const char* pDesc)
    {
        rapidjson::StringBuffer objBuffer;
        bool bResult = WriteToJson<T>(refObject, pUUID, nVersion, pDesc, objBuffer);

        if (bResult)
        {
            auto out = objBuffer.GetString();

            if (OpenClipboard(nullptr))
            {
                EmptyClipboard();

                HGLOBAL pClipBuffer = GlobalAlloc(GMEM_DDESHARE, objBuffer.GetLength() + 1);
                char * pTextBuffer = (char*)GlobalLock(pClipBuffer);

                strcpy(pTextBuffer, out);

                GlobalUnlock(pClipBuffer);
                SetClipboardData(CF_TEXT, pClipBuffer);
                CloseClipboard();
            }
        }

#ifdef _DEBUG
        //const char szFilters[] = _T("json file(*.json)|*.json|All Files (*.*)|*.*||");

        //CFileDialog dlgImport(false, _T("json"), _T("*.json"),
        //    OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);

        //if (dlgImport.DoModal() == IDOK)
        //{
        //    CString strFileName = dlgImport.GetPathName();
        //    CFile objFileWrited;
        //    if (objFileWrited.Open(strFileName, CFile::modeCreate | CFile::modeWrite))
        //    {
        //        objFileWrited.Write(out, buffer.GetLength());
        //        objFileWrited.Close();
        //    }
        //}
#endif

        return bResult;
    }

    //----------------------------------------------------------------
    // @routine CopyDataFromClipboard
    /// @brief ¥”ÀΩ”–ºÙÃ˘∞ÊøΩ±¥ ˝æ›
    ///
    /// @param lpUUID- ˝æ› ∂±ID
    /// @param refItems-±ª±£¥Êµƒ ˝◊È
    /// @param nIndex-ºÙÃ˘∞Â ˝æ›
    /// @return true:≥…π¶£¨false: ß∞‹
    /// @author ª∆¿ˆ‘∆
    /// @date 2018-9-21
    //---------------------------------------------------------------
    template<class T>
    bool CopyDataFromClipboard(const char* pUUID, T& refObject)
    {
        std::string  strClipboardData;

        //¥Úø™ºÙÃ˘∞Â

        if (OpenClipboard(nullptr))
        {
            HANDLE hData = GetClipboardData(CF_TEXT);

            strClipboardData = (char*)GlobalLock(hData);
            GlobalUnlock(hData);
            CloseClipboard();
        }

        bool bResult = ReadFromJson(strClipboardData.c_str(), strClipboardData.length(), pUUID, refObject);

        return bResult;
    }


#endif

    template <class T>
    bool ExtractESJsonObjT(const std::string& refJsonData, T& refJsonObj) const
    {
        bool bResult = false;
        rapidjson::Document objJsonDoc;
        objJsonDoc.Parse(refJsonData.c_str(), refJsonData.size());

        if (!objJsonDoc.IsNull())
        {
            CJSONSerializer objJsonSerializerHelper(objJsonDoc.GetAllocator());

            objJsonSerializerHelper.SetJsonValue(*((rapidjson::Value*)&objJsonDoc));
            refJsonObj.DeSerialize(objJsonSerializerHelper);

            bResult = true;
        }

        return bResult;
    }

private:
    CJsonHelper(const CJsonHelper&);
    CJsonHelper& operator=(const CJsonHelper&);

};

#endif

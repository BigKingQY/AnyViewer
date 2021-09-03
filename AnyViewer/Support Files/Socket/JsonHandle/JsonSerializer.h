//
//  JsonSerializer.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//

#ifndef JsonSerializer_h
#define JsonSerializer_h

#pragma once
#include "SDPFactory.h"
#include "Typedef.h"
#include "LYCodeCvt.h"
#include <assert.h>
#include <vector>
#include <string>
#include <map>
#include <rapidjson/document.h>
#include <memory>
#include <math.h>

#define NVP(name) #name, name
#define SerializeNVP(name) Serialize(NVP(name))
#define DeSerializeNVP(name) DeSerialize(NVP(name))
#define ENABLE_SERIALIZE(T) \
virtual std::string GetClassName() const override { return #T;}


#ifdef _ENABLE_MFC_JSON_
extern void EscapeString(CString& strSource, LPCTSTR lpEscape, bool bAdd);  ///◊™“Â◊÷∑˚¥Æ
#endif


class CJSONSerializer;

//*******************************************************************
/// <summary>
/// ÷ß≥÷json–Ú¡–ªØ¥Ê¥¢π¶ƒ‹µƒª˘¿‡
/// </summary>
//*******************************************************************
class CSerializeEntry
{
public:
    CSerializeEntry() {}
    virtual ~CSerializeEntry() {}

public:
    virtual std::string GetClassName() const { assert(false);  return nullptr; }

public:
    virtual void Serialize(CJSONSerializer& /*refJsonSerializer*/) const {}
    virtual void DeSerialize(CJSONSerializer& /*refJsonSerializer*/) {}

};


class CJSONSerializer
{
public:
    CJSONSerializer(rapidjson::Document::AllocatorType & refAllocator
        , rapidjson::Type nType = rapidjson::kObjectType)
        : m_objAllocator(refAllocator)
        , m_objJsonValue(nType)
    { }

    CJSONSerializer(CJSONSerializer& refSrc)
        : m_objAllocator(refSrc.m_objAllocator)
        , m_objJsonValue(refSrc.m_objJsonValue.GetType())
    { }

public:
    rapidjson::Document::AllocatorType & GetAllocator() { return m_objAllocator; }
    rapidjson::Value& GetJsonValue() { return m_objJsonValue; }
    void SetJsonValue(rapidjson::Value& refValue) { m_objJsonValue = refValue; }

public:
    template<typename TKey>
    void Serialize(TKey key, const I16& value) { Write(key, value); }

    template<typename TKey>
    void Serialize(TKey key, const U16& value) { Write(key, value); }

    template<typename TKey>
    void Serialize(TKey key, const I32& value) { Write(key, value); }
    
    template<typename TKey>
    void Serialize(TKey key, const U32& value) { Write(key, value); }

    template<typename TKey>
    void Serialize(TKey key, const I64& value) { Write(key, value); }

    template<typename TKey>
    void Serialize(TKey key, const U64& value) { Write(key, value); }

    template<typename TKey>
    void Serialize(TKey key, const float& value)
    {
#if(_MSC_VER >= 1700 || __GNUC__ > 3)
        assert(isfinite(value));
#endif
        Write(key, value);
    }

    template<typename TKey>
    void Serialize(TKey key, const double& value)
    {
#if(_MSC_VER >= 1700 || __GNUC__ > 3)
        assert(isfinite(value));
#endif
        Write(key, value);
    }

    template<typename TKey>
    void Serialize(TKey key, const bool& value) { Write(key, value); }

   
    //Serialize a string value
    template<typename TKey>
    void Serialize(TKey key, const std::string& value)
    {
        std::string strCriterion;
        FormatString(value, strCriterion);
#ifdef WIN32
        strCriterion = CLYCodeCvt::ANSIToUTF8(strCriterion);
#endif // WIN32

        m_objJsonValue.AddMember(
            rapidjson::Value().SetString(key, m_objAllocator).Move()
            , rapidjson::Value().SetString(strCriterion.data(), strCriterion.size(), m_objAllocator).Move()
            , m_objAllocator);

        //Write(key, value);
    }

    //Serialize a string value
    template<typename TKey>
    void Serialize(TKey key, const std::wstring& value)
    {
        std::string strCriterion;
        std::wstring strTmp;
        FormatString(value, strTmp);
#ifdef WIN32
        strCriterion = CLYCodeCvt::UnicodeToUTF8(strTmp);
#endif // WIN32

        m_objJsonValue.AddMember(
            rapidjson::Value().SetString(key, m_objAllocator).Move()
            , rapidjson::Value().SetString(strCriterion.data(), strCriterion.size(), m_objAllocator).Move()
            , m_objAllocator);

        //Write(key, value);
    }

    template<typename TKey, typename TValue>
    void Serialize(TKey key, const TValue& value)
    {
        // class to json
        CJSONSerializer objChildObjValue(m_objAllocator);

        value.Serialize(objChildObjValue);

        m_objJsonValue.AddMember(
            rapidjson::Value().SetString(key, m_objAllocator).Move()
            , objChildObjValue.m_objJsonValue
            , m_objAllocator);
    }

#ifdef _ENABLE_MFC_JSON_
    template<typename TKey>
    void Serialize(TKey key, const CString& value)
    {
        CString strTmp = value;
        EscapeString(strTmp, _T("\""), true);
        std::string strValue = CLYCodeCvt::UnicodeToUTF8((LPCWSTR)CT2W(strTmp));
        m_objJsonValue.AddMember(
            rapidjson::Value().SetString(key, m_objAllocator).Move()
            , rapidjson::Value().SetString(strValue.data(), strValue.size(), m_objAllocator).Move()
            , m_objAllocator);
    }
    
    template<typename TKey>
    void Serialize(TKey key, const COLORREF& value)
    {
        U32 nTmpValue(value);
        Serialize(key, nTmpValue);
    }
#endif

public:
    template<typename TKey>
    void DeSerialize(TKey key, I16& value)
    {
        const rapidjson::Value& objJonsValue = m_objJsonValue[key];

        if (objJonsValue.IsNumber())
        {
            value = objJonsValue.GetInt();
        }
        else if (objJonsValue.IsString())
        {
            value = atoi(objJonsValue.GetString());
        }
        else
        {
            value = 0;
        }
    }

    template<typename TKey>
    void DeSerialize(TKey key, U16& value)
    {
        const rapidjson::Value& objJonsValue = m_objJsonValue[key];

        if (objJonsValue.IsNumber())
        {
            value = (U16)objJonsValue.GetInt();
        }
        else if (objJonsValue.IsString())
        {
            value = (U16)atoi(objJonsValue.GetString());
        }
        else
        {
            value = 0;
        }
    }

    template<typename TKey>
    void DeSerialize(TKey key, I32& value)
    {
        const rapidjson::Value& objJonsValue = m_objJsonValue[key];

        if (objJonsValue.IsNumber())
        {
            value = objJonsValue.GetInt();
        }
        else if (objJonsValue.IsString())
        {
            value = atoi(objJonsValue.GetString());
        }
        else
        {
            value = 0;
        }
    }

    template<typename TKey>
    void DeSerialize(TKey key, U32& value)
    {
        const rapidjson::Value& objJonsValue = m_objJsonValue[key];

        if (objJonsValue.IsNumber())
        {
            value = objJonsValue.GetUint();
        }
        else if (objJonsValue.IsString())
        {
            value = atoi(objJonsValue.GetString());
        }
        else
        {
            value = 0;
        }
    }

    template<typename TKey>
    void DeSerialize(TKey key, I64& value)
    {
        const rapidjson::Value& objJonsValue = m_objJsonValue[key];

        if (objJonsValue.IsNumber())
        {
            value = objJonsValue.GetInt64();
        }
        else if (objJonsValue.IsString())
        {
            value = atoi(objJonsValue.GetString());
        }
        else
        {
            value = 0;
        }
    }

    template<typename TKey>
    void DeSerialize(TKey key, U64& value)
    {
        const rapidjson::Value& objJonsValue = m_objJsonValue[key];

        if (objJonsValue.IsNumber())
        {
            value = objJonsValue.GetUint64();
        }
        else if (objJonsValue.IsString())
        {
            value = atoi(objJonsValue.GetString());
        }
        else
        {
            value = 0;
        }
    }

    template<typename TKey>
    void DeSerialize(TKey key, float& value)
    {
        const rapidjson::Value& objJonsValue = m_objJsonValue[key];

        if (objJonsValue.IsNumber())
        {
            value = objJonsValue.GetFloat();
        }
        else if (objJonsValue.IsString())
        {
            value = atof(objJonsValue.GetString());
        }
        else
        {
            value = 0.0f;
        }
    }

    template<typename TKey>
    void DeSerialize(TKey key, double& value)
    {
        const rapidjson::Value& objJonsValue = m_objJsonValue[key];

        if (objJonsValue.IsNumber())
        {
            value = objJonsValue.GetDouble();
        }
        else if (objJonsValue.IsString())
        {
            value = atof(objJonsValue.GetString());
        }
        else
        {
            value = 0.0f;
        }
    }

    template<typename TKey>
    void DeSerialize(TKey key, bool& value)
    {
        value = m_objJsonValue[key].GetBool();
    }
    
    //DeSerialize a string value
    template<typename TKey>
    void DeSerialize(TKey key, std::string& value)
    {
        std::string sval = "";
        const rapidjson::Value& objJonsValue = m_objJsonValue[key];

        if (objJonsValue.IsString())
        {
            std::string strUtf8(objJonsValue.GetString(), objJonsValue.GetStringLength());
#ifdef WIN32
            FormatString(CLYCodeCvt::UTF8ToANSI(strUtf8), sval, false);
#else
            FormatString(strUtf8, sval, false);
#endif // WIN32
        }
        else
        {
            assert(false);
        }
        value = sval;
    }

    //DeSerialize a string value
    template<typename TKey>
    void DeSerialize(TKey key, std::wstring& value)
    {
        std::wstring sval = L"";
        const rapidjson::Value& objJonsValue = m_objJsonValue[key];

        if (objJonsValue.IsString())
        {
            std::string strUtf8(objJonsValue.GetString(), objJonsValue.GetStringLength());
#ifdef WIN32
            FormatString(CLYCodeCvt::UTF8ToUnicode(strUtf8), sval, false);
#else
            FormatString(CLYCodeCvt::UTF8ToUnicode(strUtf8), sval, false);
#endif // WIN32
        }
        else
        {
            assert(false);
        }
        value = sval;
    }

    template<typename TKey, typename TValue>
    void DeSerialize(TKey key, TValue& value)
    {
        CJSONSerializer subVal(m_objAllocator);

        subVal.m_objJsonValue = m_objJsonValue[key];
        value.DeSerialize(subVal);
    }

    template<typename TKey, typename TValue>
    void DeSerialize(TKey key, TValue* value)
    {
        CJSONSerializer subVal(m_objAllocator);

        subVal.m_objJsonValue = m_objJsonValue[key];
        value->DeSerialize(subVal);
    }

#ifdef _ENABLE_MFC_JSON_
    template<typename TKey>
    void DeSerialize(TKey key, CString& value)
    {
        const rapidjson::Value& objJonsValue = m_objJsonValue[key];

        if (objJonsValue.IsString())
        {
            std::string strUtf8(objJonsValue.GetString(), objJonsValue.GetStringLength());
            value = CLYCodeCvt::UTF8ToUnicode(strUtf8).c_str();;
            EscapeString(value, _T("\""), false);
        }
        else
        {
            value.Empty();
        }
    }
    template<typename TKey>
    void DeSerialize(TKey key, COLORREF& value)
    {
        U32 nTmpValue = 0;
        DeSerialize(key, nTmpValue);
        value = (COLORREF)nTmpValue;
    }
#endif

private:
    template<typename TKey, typename TValue>
    void Write(TKey key, const TValue& value)
    {
        m_objJsonValue.AddMember(
            rapidjson::Value().SetString(key, m_objAllocator).Move()
            , value
            , m_objAllocator);
    }


    // --------------------------------------------------------------------------------
    /// <summary>
    /// ∏Ò ΩªØ±£¥Êµƒ◊÷∑˚¥Æ£¨÷˜“™ «◊™“Â'\'∫Õ'"'
    /// </summary>
    /// <param name="strSrc">±ª∏Ò ΩªØµƒ◊÷∑˚¥Æ</param>
    /// <param name="strDst">Ω·π˚◊÷∑˚¥Æ</param>
    /// <param name="bSerialize">±£¥Ê±Í÷æ£¨true:±£¥Ê,false:º”‘ÿ</param>
    /// <created>ª∆¿ˆ‘∆,2019/1/16</created>
    /// <changed>ª∆¿ˆ‘∆,2019/1/16</changed>
    // --------------------------------------------------------------------------------
    void FormatString(const std::string& strSrc, std::string& strDst,const bool bSerialize = true) const
    {
        strDst.clear();

        if (bSerialize)
        {
            auto itr = strSrc.begin();

            for (; strSrc.end() != itr; itr++)
            {
                switch (*itr)
                {
                case '\\':
                case '"':
                    strDst += '\\';
                    break;
                }

                strDst += (*itr);
            }
        }
        else
        {
            auto itr = strSrc.begin();

            for (; strSrc.end() != itr; itr++)
            {
                switch (*itr)
                {
                case '\\':
                    itr++;
                    break;
                }

                strDst += (*itr);
            }
        }
    }

    // --------------------------------------------------------------------------------
    /// <summary>
    /// ∏Ò ΩªØ±£¥Êµƒ◊÷∑˚¥Æ£¨÷˜“™ «◊™“Â'\'∫Õ'"'
    /// </summary>
    /// <param name="strSrc">±ª∏Ò ΩªØµƒ◊÷∑˚¥Æ</param>
    /// <param name="strDst">Ω·π˚◊÷∑˚¥Æ</param>
    /// <param name="bSerialize">±£¥Ê±Í÷æ£¨true:±£¥Ê,false:º”‘ÿ</param>
    /// <created>ª∆¿ˆ‘∆,2019/1/16</created>
    /// <changed>ª∆¿ˆ‘∆,2019/1/16</changed>
    // --------------------------------------------------------------------------------
    void FormatString(const std::wstring& strSrc, std::wstring& strDst, const bool bSerialize = true) const
    {
        strDst.clear();

        if (bSerialize)
        {
            auto itr = strSrc.begin();

            for (; strSrc.end() != itr; itr++)
            {
                switch (*itr)
                {
                case '\\':
                case '"':
                    strDst += '\\';
                    break;
                }

                strDst += (*itr);
            }
        }
        else
        {
            auto itr = strSrc.begin();

            for (; strSrc.end() != itr; itr++)
            {
                switch (*itr)
                {
                case '\\':
                    itr++;
                    break;
                }

                strDst += (*itr);
            }
        }
    }


private:
    rapidjson::Document::AllocatorType & m_objAllocator;
    rapidjson::Value         m_objJsonValue;
};



template<typename Tkey, typename Tvalue>
class CDynamicJsonMap : public std::map<Tkey, Tvalue*>
{
public:
    CDynamicJsonMap(bool bAutoRelease = true)
        :m_bAutoRelease(bAutoRelease)
    {
    }

    ~CDynamicJsonMap()
    {
        if (m_bAutoRelease)
        {
            DestroyAllItems();
        }
    }
public:
    void Serialize(CJSONSerializer & refJsonSerializer) const
    {
        refJsonSerializer.GetJsonValue().SetArray();
        auto itr = this->begin();

        for (; itr != this->end(); itr++)
        {
            CJSONSerializer objElementJsonSerializer(refJsonSerializer.GetAllocator());

            objElementJsonSerializer.Serialize("key", (*itr).first);
            objElementJsonSerializer.Serialize("value", *(*itr).second);

            refJsonSerializer.GetJsonValue().PushBack(
                objElementJsonSerializer.GetJsonValue()
                , refJsonSerializer.GetAllocator());
        }
    }

    void DeSerialize(CJSONSerializer & refJsonSerializer)
    {
        rapidjson::Value& arrChildItems = refJsonSerializer.GetJsonValue();

        for (size_t i = 0; i < arrChildItems.Size(); i++)
        {
            CJSONSerializer objElementJsonSerializer(refJsonSerializer.GetAllocator());

            objElementJsonSerializer.SetJsonValue(arrChildItems[i]);

            Tkey key;
            Tvalue *value = new Tvalue;

            objElementJsonSerializer.DeSerialize("key", key);
            objElementJsonSerializer.DeSerialize("value", *value);

            this->insert(std::pair<Tkey, Tvalue*>(key, value));
        }
    }

public:
    //----------------------------------------------------------------
    // @routine * DestroyAllItems
    /// @brief  Õ∑≈’˚∏ˆ ˝◊È∏˜∏ˆ‘™Àÿº∞∆‰ƒ⁄¥Êø’º‰
    ///
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------
    void DestroyAllItems()
    {
        for (auto itr = this->begin(); itr != this->end(); itr++)
        {
            delete itr->second;
        }

        this->clear();
    }

private:
    bool m_bAutoRelease;
};


template<typename Tkey, typename Tvalue>
class CStaticJsonMap : public std::map<Tkey, Tvalue>
{
public:
    CStaticJsonMap()
    {}

    ~CStaticJsonMap()
    {
    }
public:
    void Serialize(CJSONSerializer & refJsonSerializer) const
    {
        refJsonSerializer.GetJsonValue().SetArray();
        auto itr = this->begin();

        for (; itr != this->end(); itr++)
        {
            CJSONSerializer objElementJsonSerializer(refJsonSerializer.GetAllocator());

            objElementJsonSerializer.Serialize("key", (*itr).first);
            objElementJsonSerializer.Serialize("value", (*itr).second);

            refJsonSerializer.GetJsonValue().PushBack(
                objElementJsonSerializer.GetJsonValue()
                , refJsonSerializer.GetAllocator());
        }
    }

    void DeSerialize(CJSONSerializer & refJsonSerializer)
    {
        rapidjson::Value& arrChildItems = refJsonSerializer.GetJsonValue();

        for (size_t i = 0; i < arrChildItems.Size(); i++)
        {
            CJSONSerializer objElementJsonSerializer(refJsonSerializer.GetAllocator());

            objElementJsonSerializer.SetJsonValue(arrChildItems[i]);

            Tkey key;
            Tvalue value;

            objElementJsonSerializer.DeSerialize("key", key);
            objElementJsonSerializer.DeSerialize("value", value);
            this->insert(std::pair<Tkey, Tvalue>(key, value));
        }
    }
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// ≥…‘± «“ª∏ˆπ≤œÌ÷∏’Î£¨∑¥–Ú¡–ªØµƒ ±∫Ú£¨–Ë“™∂ØÃ¨¥¥Ω®
/// </summary>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename Tkey, typename Tvalue>
class CSharedDynamicJsonMap : public std::map<Tkey, std::shared_ptr<Tvalue>>
{
public:
    CSharedDynamicJsonMap()
    {}

    ~CSharedDynamicJsonMap()
    {
    }
public:
    void Serialize(CJSONSerializer & refJsonSerializer) const
    {
        refJsonSerializer.GetJsonValue().SetArray();
        auto itr = this->begin();

        for (; itr != this->end(); itr++)
        {
            CJSONSerializer objElementJsonSerializer(refJsonSerializer.GetAllocator());

            objElementJsonSerializer.Serialize("key", (*itr).first);
            objElementJsonSerializer.Serialize("value", *(*itr).second);

            refJsonSerializer.GetJsonValue().PushBack(
                objElementJsonSerializer.GetJsonValue()
                , refJsonSerializer.GetAllocator());
        }
    }

    void DeSerialize(CJSONSerializer & refJsonSerializer)
    {
        rapidjson::Value& arrChildItems = refJsonSerializer.GetJsonValue();

        for (size_t i = 0; i < arrChildItems.Size(); i++)
        {
            CJSONSerializer objElementJsonSerializer(refJsonSerializer.GetAllocator());

            objElementJsonSerializer.SetJsonValue(arrChildItems[i]);

            Tkey key;
            std::shared_ptr<Tvalue> value = std::make_shared<Tvalue>();

            objElementJsonSerializer.DeSerialize("key", key);
            objElementJsonSerializer.DeSerialize("value", *value);
            this->insert(std::pair<Tkey, std::shared_ptr<Tvalue>>(key, value));
        }
    }
};




template<typename T>
class CDynamicJsonVector : public std::vector<T*>
{
public:
    CDynamicJsonVector(bool bAutoRelease = false)
        : m_bAutoRelease(bAutoRelease)
        , m_bEnableClassName(true)
    {
    }

    virtual ~CDynamicJsonVector()
    {
        if (m_bAutoRelease)
        {
            DestroyAllItems();
        }
    }

    CDynamicJsonVector(const CDynamicJsonVector &refSource)
    {
        *this = refSource;
    }

    CDynamicJsonVector& operator=(const CDynamicJsonVector &refSource)
    {
        if (this == &refSource)
        {
            return *this;
        }

        DestroyAllItems();

        auto itr = refSource.begin();

        for (; refSource.end() != itr; itr++)
        {
            T * pNewItem = CreateItem();

            *pNewItem = *(*itr);
            //CreateItem()µƒ ±∫Ú“—æ≠ÃÌº”Ω¯»•
            //AppendItem(pNewItem);
        }
        return *this;
    }


    CDynamicJsonVector&  MoveTo(CDynamicJsonVector &refSource)
    {
        auto itr = this->begin();

        for (; this->end() != itr; itr++)
        {
            refSource.push_back(*itr);
        }

        this->clear();

        return *this;
    }

public:
    void SetAutoRelease(const bool bAutoRelease) { m_bAutoRelease = bAutoRelease; }
    bool GetAutoRelease() const { return  m_bAutoRelease; }

    void SetEnableClassName(const bool bEnableClassName) { m_bEnableClassName = bEnableClassName; }
public:
    T * GetAt(size_t nIndex) const
    {
        T* pResult = nullptr;

        if (nIndex < this->size())
        {
            pResult = this->at(nIndex);
        }

        return pResult;
    }

public:
    //----------------------------------------------------------------
// @routine * CreateElement
/// @brief ¥¥Ω®“ª∏ˆ÷∏∂®µƒº«¬º
///
/// @return “ª∏ˆ–¬Ω®º«¬ºµƒ÷∏’Î
/// @author ª∆¿ˆ‘∆
//---------------------------------------------------------------

    T * CreateItem()
    {
        T* pNewElement = new T;

        this->push_back(pNewElement);

        return pNewElement;
    }

    virtual T * CreateItem(CJSONSerializer & /*refJsonSerializer*/) { return CreateItem(); }

    //----------------------------------------------------------------
    // @routine * AppendItem
    /// @brief ÃÌº”“ª∏ˆ÷∏∂®µƒº«¬º
    ///
    /// @param °æ_in°øpNewElement –¬ÃÌº”µƒº«¬º
    /// @param °æ_in°øbExistentialChecks ºÏ≤Èº«¬º «∑Ò“—æ≠¥Ê‘⁄
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------
    bool AppendItem(T* pNewElement, bool bExistentialChecks = true)
    {
        bool bResult = false;
        assert(nullptr != pNewElement);

        if (!bExistentialChecks || Find(pNewElement) < 0)
        {
            this->push_back(pNewElement);
            bResult = true;
        }

        return bResult;
    }

    //----------------------------------------------------------------
    // @routine * InsertAt
    /// @brief ≤Â»Î“ª∏ˆº«¬º‘⁄÷∏∂®µƒŒª÷√
    ///
    /// @param °æ_in°ønIndex ≤Â»ÎŒª÷√
    /// @param °æ_in°øpNewElement –¬ÃÌº”µƒ‘™Àÿ
    /// @param °æ_in°øbExistentialChecks ÷ÿ∏¥≤Â»ÎºÏ≤È
    /// @return true:≥…π¶,false: ß∞‹
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------

    bool InsertAt(size_t nIndex, T* pNewElement, bool bExistentialChecks = true)
    {
        assert(nullptr != pNewElement);

        bool bResult = false;

        if (nullptr != pNewElement)
        {
            if (!bExistentialChecks || Find(pNewElement) < 0)
            {
                if (nIndex < this->size())
                {
                    this->insert(this->begin() + nIndex, pNewElement);
                }
                else
                {
                    this->push_back(pNewElement);
                }

                bResult = true;
            }
        }

        return bResult;
    }

    //----------------------------------------------------------------
    // @routine * InsertItem
    /// @brief ≤Â»Î“ª∏ˆº«¬º‘⁄÷∏∂®µƒº«¬º«∞√Ê
    ///
    /// @param °æ_in°øpPosRecord –¬ÃÌº”µƒº«¬º
    /// @param °æ_in°øpNewElement –¬ÃÌº”µƒº«¬º
    /// @param °æ_in°øbExistentialChecks ÷ÿ∏¥≤Â»ÎºÏ≤È
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------
    bool InsertItem(T* pNewElement, const T* pPosRecord, bool bExistentialChecks = true)
    {
        assert(nullptr != pNewElement);

        bool bResult = false;

        if (nullptr != pNewElement)
        {
            bResult = (bExistentialChecks || Find(pNewElement) < 0);

            if (bResult)
            {
                if (nullptr != pPosRecord)
                {
                    const int nIndex = Find(pPosRecord);
                    if (nIndex >= 0)
                    {
                        InsertAt(nIndex, pNewElement);
                    }
                    else
                    {
                        this->push_back(pNewElement);
                    }
                }
                else
                {
                    this->push_back(pNewElement);
                }
            }
        }

        return bResult;
    }

    //----------------------------------------------------------------
    // @routine * RemoveAt
    /// @brief …æ≥˝“ª∏ˆ÷∏∂®µƒº«¬º
    ///
    /// @param °æ_in°ønIndex ±ª…æ≥˝µƒ‘™ÀÿÀ˜“˝
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------
    void RemoveAt(size_t nIndex)
    {
        if (nIndex < this->size())
        {
            erase(this->begin() + nIndex);
        }
    }

    //----------------------------------------------------------------
    // @routine * RemoveItem
    /// @brief …æ≥˝“ª∏ˆ÷∏∂®µƒº«¬º
    ///
    /// @param °æ_in°øpItemRemoved ±ª…æ≥˝µƒ‘™Àÿ
    /// @return true:≥…π¶,false: ß∞‹
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------

    bool RemoveItem(const T* pItemRemoved)
    {
        bool bResult = false;
        auto itr = this->begin();

        for (; itr != this->end(); itr++)
        {
            if ((*itr) == pItemRemoved)
            {
                this->erase(itr);
                bResult = true;
                break;
            }
        }

        return bResult;
    }

    //----------------------------------------------------------------
    // @routine * DestroyAt
    /// @brief  Õ∑≈÷∏∂®µƒ‘™Àÿº∞∆‰ƒ⁄¥Êø’º‰
    ///
    /// @param °æ_in°ønIndex ±ª Õ∑≈µƒ‘™ÀÿÀ˜“˝
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------
    void DestroyAt(size_t nIndex)
    {
        if (nIndex < this->size())
        {
            T* pElement = this->at(nIndex);

            delete pElement;
            erase(this->begin() + nIndex);
        }
    }

    //----------------------------------------------------------------
    // @routine * DestroyItem
    /// @brief  Õ∑≈÷∏∂®µƒ‘™Àÿº∞∆‰ƒ⁄¥Êø’º‰
    ///
    /// @param °æ_in°øpElementRemoved ±ª Õ∑≈µƒ‘™Àÿ
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------
    bool DestroyItem(T* pElementRemoved)
    {
        bool bResult = false;
        auto itr = this->begin();

        for (; itr != this->end(); itr++)
        {
            if (pElementRemoved == (*itr))
            {
                delete pElementRemoved;
                this->erase(itr);
                bResult = true;
                break;
            }
        }

        return bResult;
    }

    //----------------------------------------------------------------
    // @routine * DestroyAllItems
    /// @brief  Õ∑≈’˚∏ˆ ˝◊È∏˜∏ˆ‘™Àÿº∞∆‰ƒ⁄¥Êø’º‰
    ///
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------

    void DestroyAllItems()
    {
        auto itr = this->begin();

        for (; itr != this->end(); itr++)
        {
            delete (*itr);
        }
        
        this->clear();
    }


public:
    //----------------------------------------------------------------
    // @routine * Find
    /// @brief ≤È’“÷∆∂®µƒ‘™Àÿ
    ///
    /// @param °æ_in°øpNewElement ±ª≤È’“µƒ‘™Àÿ∂‘œÛ÷∏’Î
    /// @return ≥…π¶‘™ÀÿµƒÀ˜“˝£¨∑Ò‘Ú-1
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------
    int Find(const T* pNewElement) const
    {
        int nResult = -1;
        int nIndex = 0;
        auto itr = this->begin();

        for (; itr != this->end(); itr++)
        {
            if ((*itr) == pNewElement)
            {
                nResult = nIndex;
                break;
            }

            nIndex++;
        }

        return nResult;
    }

public:

    //----------------------------------------------------------------
    // @routine  MoveUpAble
    /// @brief ≈–∂œµ±«∞µƒΩ⁄µ„ «∑Òø…“‘œÚ…œ“∆∂Ø
    ///
    /// @param °æ_in°øpItem ±ªºÏ≤ÈµƒΩ⁄µ„
    /// @return true:ø…“‘“∆∂Ø,∑Ò‘Ú≤ªø…“‘“∆∂Ø
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------

    bool MoveUpAble(const T* pItem) const
    {
        bool bResult = false;

        if (this->size() >= 2)
        {
            bResult = (this->at(0) != pItem);
        }

        return bResult;
    }
    //----------------------------------------------------------------
    // @routine * MoveUp
    /// @brief œÚ…œ“∆∂ØΩ⁄µ„
    ///
    /// ∞—÷∏∂®º«¬ºœÚ…œ“∆∂Ø“ª––,≤¢∑µªÿ±ªΩªªªµƒΩ⁄µ„
    /// @param °æ_in°øpItem ±ª“∆∂ØµƒΩ⁄µ„
    /// @return ±ªΩªªªµƒΩ⁄µ„
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------

    T* MoveUp(T* pItem)
    {
        T* pItemSwitched = nullptr;

        // ÷ª”–Ω⁄µ„¥Û”⁄1,≤≈ƒ‹“∆∂Ø
        if (this->size() >= 2)
        {
            for (size_t i = 0; i < this->size(); i++)
            {
                if (this->at(i) == pItem)
                {
                    if (i >= 1)
                    {
                        pItemSwitched = this->at(i - 1);
                        *(this->begin() + (i - 1)) = pItem;
                        *(this->begin() + i) = pItemSwitched;

                    }

                    break;
                }
            }
        }

        return     pItemSwitched;
    }
    //----------------------------------------------------------------
    // @routine  MoveDownAble
    /// @brief ≈–∂œµ±«∞µƒΩ⁄µ„ «∑Òø…“‘œÚœ¬“∆∂Ø
    ///
    /// @param °æ_in°øpItem ±ªºÏ≤ÈµƒΩ⁄µ„
    /// @return true:ø…“‘“∆∂Ø,∑Ò‘Ú≤ªø…“‘“∆∂Ø
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------

    bool MoveDownAble(const T* pItem) const
    {
        bool bResult = false;

        if (this->size() >= 2)
        {
            bResult = (this->at(this->size() - 1) != pItem);
        }

        return bResult;
    }
    //----------------------------------------------------------------
    // @routine * MoveDown
    /// @brief œÚœ¬“∆∂ØΩ⁄µ„
    ///
    /// ∞—÷∏∂®º«¬ºœÚœ¬“∆∂Ø“ª––,≤¢∑µªÿ±ªΩªªªµƒΩ⁄µ„
    /// @param °æ_in°øpItem ±ª“∆∂ØµƒΩ⁄µ„
    /// @return ±ªΩªªªµƒΩ⁄µ„
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------

    T* MoveDown(T* pItem)
    {
        T* pItemSwitched = nullptr;

        // ÷ª”–Ω⁄µ„¥Û”⁄1,≤≈ƒ‹“∆∂Ø
        if (this->size() >= 2)
        {
            for (size_t i = 0; i < this->size(); i++)
            {
                if (this->at(i) == pItem)
                {
                    if (i + 1 < this->size())
                    {
                        pItemSwitched = this->at(i + 1);
                        *(this->begin() + i + 1) = pItem;
                        *(this->begin() + i) = pItemSwitched;

                    }

                    break;
                }
            }
        }

        return     pItemSwitched;
    }
    //----------------------------------------------------------------
    // @routine MoveTo
    /// @brief “∆∂Ø◊”Ω⁄µ„
    ///
    /// ∞—“ª∏ˆΩ⁄µ„“∆∂ØµΩ∆‰–÷µ‹Ω⁄µ„µƒ«∞√Ê
    /// @param °æ_in°øpNodeMoved ±ª“∆∂Øµƒµ•œÓπ§≥ÃΩ⁄µ„
    /// @param °æ_in°øpDstNode ƒø±Íπ§≥ÃΩ⁄µ„
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------
    void MoveTo(T* pNodeMoved, T* pDstNode)
    {
        assert(nullptr != pNodeMoved);
        assert(nullptr != pDstNode);

        for (size_t i = 0; i < this->size(); i++)
        {
            if (this->at(i) == pNodeMoved)
            {
                erase(this->begin() + i);

                for (size_t j = 0; j < this->size(); j++)
                {
                    if (this->at(j) == pDstNode)
                    {
                        this->insert(this->begin() + j, pNodeMoved);
                        break;
                    }
                }

                break;
            }
        }
    }

    //----------------------------------------------------------------
    // @routine * Replace
    /// @brief ÃÊªª÷∏∂®µƒΩ⁄µ„
    ///
    /// @param °æ_in°øpNode1 ±ª≤È—ØµƒΩ⁄µ„
    /// @param °æ_in°øpNode2 ±ª≤È—ØµƒΩ⁄µ„
    /// @param °æ_in°øbDestory  «∑Ò Õ∑≈±ªÃÊªªµƒΩ⁄µ„
    /// @return ≥…π¶∑µªÿtrue,∑◊„∂Ó∑µªÿfalse
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------

    bool Replace(T* pNode1, T* pNode2, bool bDestory = true)
    {
        bool bResult = false;

        for (size_t i = 0; i < this->size(); i++)
        {
            if (this->at(i) == pNode1)
            {
                if (bDestory)
                {
                    delete pNode1;
                }
                auto itr = this->begin() + i;
                (*itr) = pNode2;
                bResult = true;

                break;
            }
        }

        /*    if (!bResult)
            {
                this->push_back(pNode2);
                bResult = true;
            }*/

        return bResult;
    }

public:
    virtual void Serialize(CJSONSerializer & refJsonSerializer) const
    {
        refJsonSerializer.GetJsonValue().SetArray();
        auto itr = this->begin();

        for (; itr != this->end(); itr++)
        {
            CJSONSerializer objElementJsonSerializer(refJsonSerializer.GetAllocator());
            const std::string strName = (*itr)->GetClassName();

            objElementJsonSerializer.Serialize("class_name", strName);
            (*itr)->Serialize(objElementJsonSerializer);
            refJsonSerializer.GetJsonValue().PushBack(
                objElementJsonSerializer.GetJsonValue()
                , refJsonSerializer.GetAllocator());
        }
    }

    virtual void DeSerialize(CJSONSerializer & refJsonSerializer)
    {
        DestroyAllItems();

        rapidjson::Value& arrChildItems = refJsonSerializer.GetJsonValue();

        for (size_t i = 0; i < arrChildItems.Size(); i++)
        {
            T * pNewElement = nullptr;
            std::string strClassName;
            CJSONSerializer objElementJsonSerializer(refJsonSerializer.GetAllocator());
            rapidjson::Value& refNode = arrChildItems[i];

            objElementJsonSerializer.SetJsonValue(refNode);

            if (m_bEnableClassName)
            {
                objElementJsonSerializer.DeSerialize("class_name", strClassName);

                if (strClassName.size() > 0)
                {
                    pNewElement = CreateObject<T>(strClassName);

                    if (nullptr != pNewElement)
                    {
                        this->push_back(pNewElement);
                    }
                }
            }
            
            if(nullptr == pNewElement)
            {
                // ∏√¥˙¬Î±ÿ–Î¥Ê‘⁄,“ÚŒ™¥¥Ω®∂‘œÛµƒ ±∫Ú,∂‘œÛ”–π§≥ßø…ƒ‹ªπ√ª”–±ª◊¢≤·
                pNewElement = CreateItem(objElementJsonSerializer);
            }

            assert(nullptr != pNewElement);

            if (nullptr != pNewElement)
            {
                pNewElement->DeSerialize(objElementJsonSerializer);
                
            }
        }
    }

private:
    bool                    m_bAutoRelease;   ///< ◊‘∂Ø Õ∑≈º«¬º∂‘œÛ
    bool                    m_bEnableClassName;   ///< ∆Ù”√∏˘æ›¿‡√˚≥∆¥¥Ω®≥…‘±
};


template<typename T>
class CSharedDynamicJsonVector : public std::vector<std::shared_ptr<T>>
{
public:
    CSharedDynamicJsonVector()
        : m_bEnableClassName(true)
    {
    }

    virtual ~CSharedDynamicJsonVector()
    {
    }

    CSharedDynamicJsonVector(const CSharedDynamicJsonVector &refSource)
    {
        *this = refSource;
    }

    CSharedDynamicJsonVector& operator=(const CSharedDynamicJsonVector &refSource)
    {
        if (this == &refSource)
        {
            return *this;
        }

        this->assign(refSource.begin(), refSource.end());

        return *this;
    }


    CSharedDynamicJsonVector&  MoveTo(CSharedDynamicJsonVector &refSource)
    {
        refSource = *this;
        this->clear();

        return *this;
    }

public:

    void SetEnableClassName(const bool bEnableClassName) { m_bEnableClassName = bEnableClassName; }


public:
    //----------------------------------------------------------------
// @routine * CreateElement
/// @brief ¥¥Ω®“ª∏ˆ÷∏∂®µƒº«¬º
///
/// @return “ª∏ˆ–¬Ω®º«¬ºµƒ÷∏’Î
/// @author ª∆¿ˆ‘∆
//---------------------------------------------------------------

    std::shared_ptr<T> CreateItem()
    {
        std::shared_ptr<T> pNewElement = std::make_shared<T>();

        this->push_back(pNewElement);

        return pNewElement;
    }

    virtual std::shared_ptr<T> CreateItem(CJSONSerializer & /*refJsonSerializer*/) { return CreateItem(); }

    //----------------------------------------------------------------
    // @routine * AppendItem
    /// @brief ÃÌº”“ª∏ˆ÷∏∂®µƒº«¬º
    ///
    /// @param °æ_in°øpNewElement –¬ÃÌº”µƒº«¬º
    /// @param °æ_in°øbExistentialChecks ºÏ≤Èº«¬º «∑Ò“—æ≠¥Ê‘⁄
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------
    bool AppendItem(std::shared_ptr<T> pNewElement, bool bExistentialChecks = true)
    {
        bool bResult = false;
        assert(nullptr != pNewElement);

        if (!bExistentialChecks || Find(pNewElement) < 0)
        {
            this->push_back(pNewElement);
            bResult = true;
        }

        return bResult;
    }

    //----------------------------------------------------------------
    // @routine * InsertAt
    /// @brief ≤Â»Î“ª∏ˆº«¬º‘⁄÷∏∂®µƒŒª÷√
    ///
    /// @param °æ_in°ønIndex ≤Â»ÎŒª÷√
    /// @param °æ_in°øpNewElement –¬ÃÌº”µƒ‘™Àÿ
    /// @param °æ_in°øbExistentialChecks ÷ÿ∏¥≤Â»ÎºÏ≤È
    /// @return true:≥…π¶,false: ß∞‹
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------

    bool InsertAt(size_t nIndex, std::shared_ptr<T> pNewElement, bool bExistentialChecks = true)
    {
        assert(nullptr != pNewElement);

        bool bResult = false;

        if (nullptr != pNewElement)
        {
            if (!bExistentialChecks || Find(pNewElement) < 0)
            {
                if (nIndex < this->size())
                {
                    this->insert(this->begin() + nIndex, pNewElement);
                }
                else
                {
                    this->push_back(pNewElement);
                }

                bResult = true;
            }
        }

        return bResult;
    }

    //----------------------------------------------------------------
    // @routine * InsertItem
    /// @brief ≤Â»Î“ª∏ˆº«¬º‘⁄÷∏∂®µƒº«¬º«∞√Ê
    ///
    /// @param °æ_in°øpPosRecord –¬ÃÌº”µƒº«¬º
    /// @param °æ_in°øpNewElement –¬ÃÌº”µƒº«¬º
    /// @param °æ_in°øbExistentialChecks ÷ÿ∏¥≤Â»ÎºÏ≤È
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------
    bool InsertItem(
        std::shared_ptr<T> pNewElement
        , const std::shared_ptr<T> pPosRecord
        , bool bExistentialChecks = true)
    {
        assert(nullptr != pNewElement);

        bool bResult = false;

        if (nullptr != pNewElement)
        {
            bResult = (bExistentialChecks || Find(pNewElement) < 0);

            if (bResult)
            {
                if (nullptr != pPosRecord)
                {
                    const int nIndex = Find(pPosRecord);
                    if (nIndex >= 0)
                    {
                        InsertAt(nIndex, pNewElement);
                    }
                    else
                    {
                        this->push_back(pNewElement);
                    }
                }
                else
                {
                    this->push_back(pNewElement);
                }
            }
        }

        return bResult;
    }

    //----------------------------------------------------------------
    // @routine * RemoveAt
    /// @brief …æ≥˝“ª∏ˆ÷∏∂®µƒº«¬º
    ///
    /// @param °æ_in°ønIndex ±ª…æ≥˝µƒ‘™ÀÿÀ˜“˝
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------
    void RemoveAt(size_t nIndex)
    {
        if (nIndex < this->size())
        {
            erase(this->begin() + nIndex);
        }
    }

    //----------------------------------------------------------------
    // @routine * RemoveItem
    /// @brief …æ≥˝“ª∏ˆ÷∏∂®µƒº«¬º
    ///
    /// @param °æ_in°øpItemRemoved ±ª…æ≥˝µƒ‘™Àÿ
    /// @return true:≥…π¶,false: ß∞‹
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------

    bool RemoveItem(const std::shared_ptr<T> pItemRemoved)
    {
        bool bResult = false;
        auto itr = this->begin();

        for (; itr != this->end(); itr++)
        {
            if ((*itr) == pItemRemoved)
            {
                this->erase(itr);
                bResult = true;
                break;
            }
        }

        return bResult;
    }


public:
    //----------------------------------------------------------------
    // @routine * Find
    /// @brief ≤È’“÷∆∂®µƒ‘™Àÿ
    ///
    /// @param °æ_in°øpNewElement ±ª≤È’“µƒ‘™Àÿ∂‘œÛ÷∏’Î
    /// @return ≥…π¶‘™ÀÿµƒÀ˜“˝£¨∑Ò‘Ú-1
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------
    int Find(const std::shared_ptr<T> pNewElement) const
    {
        int nResult = -1;
        int nIndex = 0;
        auto itr = this->begin();

        for (; itr != this->end(); itr++)
        {
            if ((*itr) == pNewElement)
            {
                nResult = nIndex;
                break;
            }

            nIndex++;
        }

        return nResult;
    }

public:

    //----------------------------------------------------------------
    // @routine  MoveUpAble
    /// @brief ≈–∂œµ±«∞µƒΩ⁄µ„ «∑Òø…“‘œÚ…œ“∆∂Ø
    ///
    /// @param °æ_in°øpItem ±ªºÏ≤ÈµƒΩ⁄µ„
    /// @return true:ø…“‘“∆∂Ø,∑Ò‘Ú≤ªø…“‘“∆∂Ø
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------

    bool MoveUpAble(const std::shared_ptr<T> pItem) const
    {
        bool bResult = false;

        if (this->size() >= 2)
        {
            bResult = (this->at(0) != pItem);
        }

        return bResult;
    }
    //----------------------------------------------------------------
    // @routine * MoveUp
    /// @brief œÚ…œ“∆∂ØΩ⁄µ„
    ///
    /// ∞—÷∏∂®º«¬ºœÚ…œ“∆∂Ø“ª––,≤¢∑µªÿ±ªΩªªªµƒΩ⁄µ„
    /// @param °æ_in°øpItem ±ª“∆∂ØµƒΩ⁄µ„
    /// @return ±ªΩªªªµƒΩ⁄µ„
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------

    std::shared_ptr<T> MoveUp(std::shared_ptr<T> pItem)
    {
        std::shared_ptr<T> pItemSwitched = nullptr;

        // ÷ª”–Ω⁄µ„¥Û”⁄1,≤≈ƒ‹“∆∂Ø
        if (this->size() >= 2)
        {
            for (size_t i = 0; i < this->size(); i++)
            {
                if (this->at(i) == pItem)
                {
                    if (i >= 1)
                    {
                        pItemSwitched = this->at(i - 1);
                        *(this->begin() + (i - 1)) = pItem;
                        *(this->begin() + i) = pItemSwitched;

                    }

                    break;
                }
            }
        }

        return     pItemSwitched;
    }
    //----------------------------------------------------------------
    // @routine  MoveDownAble
    /// @brief ≈–∂œµ±«∞µƒΩ⁄µ„ «∑Òø…“‘œÚœ¬“∆∂Ø
    ///
    /// @param °æ_in°øpItem ±ªºÏ≤ÈµƒΩ⁄µ„
    /// @return true:ø…“‘“∆∂Ø,∑Ò‘Ú≤ªø…“‘“∆∂Ø
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------

    bool MoveDownAble(const std::shared_ptr<T> pItem) const
    {
        bool bResult = false;

        if (this->size() >= 2)
        {
            bResult = (this->at(this->size() - 1) != pItem);
        }

        return bResult;
    }
    //----------------------------------------------------------------
    // @routine * MoveDown
    /// @brief œÚœ¬“∆∂ØΩ⁄µ„
    ///
    /// ∞—÷∏∂®º«¬ºœÚœ¬“∆∂Ø“ª––,≤¢∑µªÿ±ªΩªªªµƒΩ⁄µ„
    /// @param °æ_in°øpItem ±ª“∆∂ØµƒΩ⁄µ„
    /// @return ±ªΩªªªµƒΩ⁄µ„
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------

    std::shared_ptr<T> MoveDown(std::shared_ptr<T> pItem)
    {
        std::shared_ptr<T> pItemSwitched = nullptr;

        // ÷ª”–Ω⁄µ„¥Û”⁄1,≤≈ƒ‹“∆∂Ø
        if (this->size() >= 2)
        {
            for (size_t i = 0; i < this->size(); i++)
            {
                if (this->at(i) == pItem)
                {
                    if (i + 1 < this->size())
                    {
                        pItemSwitched = this->at(i + 1);
                        *(this->begin() + i + 1) = pItem;
                        *(this->begin() + i) = pItemSwitched;

                    }

                    break;
                }
            }
        }

        return     pItemSwitched;
    }
    //----------------------------------------------------------------
    // @routine MoveTo
    /// @brief “∆∂Ø◊”Ω⁄µ„
    ///
    /// ∞—“ª∏ˆΩ⁄µ„“∆∂ØµΩ∆‰–÷µ‹Ω⁄µ„µƒ«∞√Ê
    /// @param °æ_in°øpNodeMoved ±ª“∆∂Øµƒµ•œÓπ§≥ÃΩ⁄µ„
    /// @param °æ_in°øpDstNode ƒø±Íπ§≥ÃΩ⁄µ„
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------
    void MoveTo(std::shared_ptr<T> pNodeMoved, std::shared_ptr<T> pDstNode)
    {
        assert(nullptr != pNodeMoved);
        assert(nullptr != pDstNode);

        for (size_t i = 0; i < this->size(); i++)
        {
            if (this->at(i) == pNodeMoved)
            {
                erase(this->begin() + i);

                for (size_t j = 0; j < this->size(); j++)
                {
                    if (this->at(j) == pDstNode)
                    {
                        this->insert(this->begin() + j, pNodeMoved);
                        break;
                    }
                }

                break;
            }
        }
    }

    //----------------------------------------------------------------
    // @routine * Replace
    /// @brief ÃÊªª÷∏∂®µƒΩ⁄µ„
    ///
    /// @param °æ_in°øpNode1 ±ª≤È—ØµƒΩ⁄µ„
    /// @param °æ_in°øpNode2 ±ª≤È—ØµƒΩ⁄µ„
    /// @param °æ_in°øbDestory  «∑Ò Õ∑≈±ªÃÊªªµƒΩ⁄µ„
    /// @return ≥…π¶∑µªÿtrue,∑◊„∂Ó∑µªÿfalse
    /// @author ª∆¿ˆ‘∆
    //---------------------------------------------------------------

    bool Replace(std::shared_ptr<T> pNode1, std::shared_ptr<T> pNode2)
    {
        bool bResult = false;

        for (size_t i = 0; i < this->size(); i++)
        {
            if (this->at(i) == pNode1)
            {
                auto itr = this->begin() + i;
            
                (*itr) = pNode2;
                bResult = true;

                break;
            }
        }

        return bResult;
    }

public:
    virtual void Serialize(CJSONSerializer & refJsonSerializer) const
    {
        refJsonSerializer.GetJsonValue().SetArray();
        auto itr = this->begin();

        for (; itr != this->end(); itr++)
        {
            CJSONSerializer objElementJsonSerializer(refJsonSerializer.GetAllocator());
            const std::string strName = (*itr)->GetClassName();

            objElementJsonSerializer.Serialize("class_name", strName);
            (*itr)->Serialize(objElementJsonSerializer);
            refJsonSerializer.GetJsonValue().PushBack(
                objElementJsonSerializer.GetJsonValue()
                , refJsonSerializer.GetAllocator());
        }
    }

    virtual void DeSerialize(CJSONSerializer & refJsonSerializer)
    {
        this->clear();

        rapidjson::Value& arrChildItems = refJsonSerializer.GetJsonValue();

        for (size_t i = 0; i < arrChildItems.Size(); i++)
        {
            std::shared_ptr<T> pNewElement = nullptr;
            std::string strClassName;
            CJSONSerializer objElementJsonSerializer(refJsonSerializer.GetAllocator());
            rapidjson::Value& refNode = arrChildItems[i];

            objElementJsonSerializer.SetJsonValue(refNode);

            if (m_bEnableClassName)
            {
                objElementJsonSerializer.DeSerialize("class_name", strClassName);

                if (strClassName.size() > 0)
                {
                    pNewElement = CreateSharedObject<T>(strClassName);

                    if (nullptr != pNewElement)
                    {
                        this->push_back(pNewElement);
                    }
                }
            }

            if (nullptr == pNewElement)
            {
                // ∏√¥˙¬Î±ÿ–Î¥Ê‘⁄,“ÚŒ™¥¥Ω®∂‘œÛµƒ ±∫Ú,∂‘œÛ”–π§≥ßø…ƒ‹ªπ√ª”–±ª◊¢≤·
                pNewElement = CreateItem(objElementJsonSerializer);
            }

            assert(nullptr != pNewElement);

            if (nullptr != pNewElement)
            {
                pNewElement->DeSerialize(objElementJsonSerializer);

            }
        }
    }

private:
    bool                    m_bEnableClassName;   ///< ∆Ù”√∏˘æ›¿‡√˚≥∆¥¥Ω®≥…‘±
};


template<typename T>
class CStaticJsonVector : public std::vector<T>
{
public:
    CStaticJsonVector(){}

public:
    virtual void Serialize(CJSONSerializer & refJsonSerializer) const
    {
        refJsonSerializer.GetJsonValue().SetArray();
        auto itr = this->begin();

        for (; itr != this->end(); itr++)
        {
            CJSONSerializer objElementJsonSerializer(refJsonSerializer.GetAllocator());

            objElementJsonSerializer.Serialize("value", (*itr));

            refJsonSerializer.GetJsonValue().PushBack(
                objElementJsonSerializer.GetJsonValue()
                , refJsonSerializer.GetAllocator());
        }
    }

    virtual void DeSerialize(CJSONSerializer & refJsonSerializer)
    {
        rapidjson::Value& arrChildItems = refJsonSerializer.GetJsonValue();

        for (size_t i = 0; i < arrChildItems.Size(); i++)
        {
            T value;
            CJSONSerializer objElementJsonSerializer(refJsonSerializer.GetAllocator());

            objElementJsonSerializer.SetJsonValue(arrChildItems[i]);
            objElementJsonSerializer.DeSerialize("value", value);
            this->push_back(value);
        }
    }
};

#endif

//
//  CVNCPacket.hpp
//  AnyViewer
//
//  Created by Aomei on 2021/8/31.
//

#pragma once
#include "TypeDefine.h"
#include "PacketBase.hpp"
//#include "VNCProtocol.h"
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

#include <iostream>


//=============================================
/// @brief Õ®”√µƒ ˝æ›∞¸ª˘¿‡
///
/// Õ®”√µƒ ˝æ›∞¸ª˘¿‡,”µ”–“ª∏ˆ¡˜ª∫≥Â«¯≥…‘±£¨”√”⁄¥Ê¥¢ ˝æ›∞¸ ˝æ›
//=============================================
class CVNCPacket : public CNetPacket
{
public:

    /// @brief ππ‘Ï∫Ø ˝
    CVNCPacket();

    /// @brief ππ‘Ï∫Ø ˝
    CVNCPacket(int nSize);

    /// @brief ππ‘Ï∫Ø ˝
    CVNCPacket(const U8* pBuffer, int nLen);

    /// @brief Œˆππ∫Ø ˝
    virtual ~CVNCPacket();

    /// @brief øΩ±¥ππ‘Ï∫Ø ˝
    CVNCPacket(const CVNCPacket& refDataPacket);

    /// @brief ∏≥÷µ∫Ø ˝
    CVNCPacket& operator=(const CVNCPacket& refDataPacket);

    enum
    {
        PACKET_LEN_MAX = 100 * 1024,  ///< ◊Ó¥Ûµƒ ˝æ›∞¸≥§∂»
    };
public:

    /// @brief µ√µΩª∫≥Â«¯∂‘œÛ
    ///
    /// @return  ∑µªÿ“ª∏ˆ÷∏œÚª∫≥Â«¯∂‘œÛµƒ≤Œøº÷∏’Î
    CDataStreamBuffer& GetPacketBuffer() { return m_objDataBuffer; }

public:
    /// @brief πÿ¡™µ±«∞µƒ ˝æ›∞¸µΩ÷∏∂®µƒ ˝æ›
    ///
    /// ∏√≤Ÿ◊˜≤ªª·“˝∑¢ ˝æ›øΩ±¥≤Ÿ◊˜,“ªµ©πÿ¡™≥…π¶£¨Œ“√«Ω´≤ªƒ‹–ﬁ∏ƒ ˝æ›∞¸ ˝æ›¡À
    /// @param pData ∑µ“ª∏ˆ÷∏œÚƒø±Í ˝æ›µƒ÷∏’Î
    /// @param  nLen ƒø±Í ˝æ›µƒ≥§∂»
    void Attach(U8* pData, int nLen) { m_objDataBuffer.Attach(pData, nLen); m_objDataBuffer.IncreaseDataLen(nLen); }

    /// @brief »°œ˚πÿ¡™
    void Dettach() { m_objDataBuffer.Dettach(); }

    void SetLength(const int nValidPacketLen);

    // º∆À„‘ÿ∫… ˝æ›µƒ≥§∂»
    void CalcPayloadLen();

    // …Ë÷√ª∫≥Â«¯÷–µƒ∂¡∆´“∆
    void SetReadingOffset(const unsigned int nOffset);

public:
    /// –¥»Î ˝æ›µΩ
    void Write(const U8* pData, const int nLen);

    void WriteUInt8(const U8 x);

    void WriteUInt16(const U16 x);

    void WriteUInt32(const U32 x);

    void WriteUInt64(const U64 x);

    /// ∂¡≥ˆ ˝æ›µΩ
    int Read(U8* pData, const int nLen);

    U8 ReadUInt8();

    U16 ReadUInt16();

    U32 ReadUInt32();

    U64 ReadUInt64();

public:
    //¥” ˝æ›¡˜÷–Ã·»° ˝æ›∞¸
    virtual NET_EXTRACT_DATA_PACKET_RESULT ExtractData(CDataStreamBuffer&) override;

    //«Âø’ ˝æ›∞¸
    virtual void ResetContent() override;

    //µ√µΩ ˝æ›Õ∑÷∏’Î
    virtual U8* GetHeaderPtr() override;
    
    //µ√µΩ’˚∏ˆ ˝æ›∞¸µƒ≥§∂»
    virtual int GetTotalLength() override;

    virtual int GetPacketId() override { return -1; } // ªÒ»°±®ŒƒID°¢±æª˙◊÷Ω⁄–Ú


public:
    // µ√µΩœ˚œ¢¿‡–Õ
    U8 GetType();

    // µ√µΩª·ª∞ID
    U32 GetSessionID();

    // µ√µΩ‘ÿ∫… ˝æ›µƒ≥§∂»
    unsigned int GetPayloadLen() const;

    // µ√µΩ±Í÷æ
    U8 GetFlag();

    // µ√µΩ‘ÿ∫… ˝æ›Õ∑÷∏’Î
    template<class MSG_HEADER>
    U8* GetPayloadPtr();

    INT Copy(const U8* pBuffer, INT nLen);

    // ≥ı ºªØ ˝æ›∞¸
    bool Initial(const U8 nType, const U32 nSessionID, const U8 nFlag /*= 0*/, const UINT nVarPayloadLen/* = 0*/);

    //≥ı ºªØ ˝æ›∞¸µƒƒ£∞Â∫Ø ˝£®Õ∆ºˆ π”√∏√∫Ø ˝£©
    template <class HeadT,class BodyT>
    bool InitialT(const U8 nType, const U32 nSessionID, const U8 nFlag /*= 0*/, const UINT nVarPayloadLen/* = 0*/);

    template <class PObject = CSerializeEntry * >
    bool InitialJsonT(const U8 nType, const U32 nSessionID, const U8 nFlag, const PObject pJsonObj);

    template <class T>
    bool ExtractJsonObjT(const unsigned int nStartPos, const unsigned int nLen, T& refJsonObj);

    template <class TPacket, class TObject>
    bool ExtractJsonObjT(TObject& refJsonObj);
protected:
    CDataStreamBuffer        m_objDataBuffer;                  ///< ¥Ê∑≈ ˝æ›µƒª∫≥Â«¯∂‘œÛ
};


template<class MSG_HEADER>
inline U8 * CVNCPacket::GetPayloadPtr()
{
    U8 * pResult = m_objDataBuffer.GetBufferHeadPtr();

    return pResult + sizeof(MSG_HEADER);
}

/*--------------------------------------------------------------------------
*∫Ø ˝√˚≥∆£∫InitialT
*√Ë ˆ£∫ ≥ı ºªØ ˝æ›∞¸µƒƒ£∞Â∫Ø ˝£®Õ∆ºˆ π”√∏√∫Ø ˝£©
*≤Œ ˝ :  UINT nID-> ˝æ›∞¸µƒID
*≤Œ ˝ : WORD nType-> ˝æ›∞¸µƒ¿‡–Õ
*≤Œ ˝ : WORD nFlags-> ˝æ›∞¸µƒ±Í÷æ
*≤Œ ˝ : UINT nVarPayloadLen-> ˝æ›∞¸–Ø¥¯µƒø…—° ˝æ›
*∑µªÿ÷µ: true->≥…π¶,false-> ß∞‹ (÷˜“™ «∑÷≈‰ƒ⁄¥Ê ß∞‹)
*–ﬁ∏ƒº«¬º:
*---------------------------------------------------------------------------*/
template<class HeadT, class BodyT>
inline bool CVNCPacket::InitialT(const U8 nType, const U32 nSessionID, const U8 nFlag, const UINT nVarPayloadLen)
{
    const U32 nPayloadLen = (sizeof(HeadT) - sizeof(VNCP::VNC_MESSAGE_HEADER) + sizeof(BodyT) + nVarPayloadLen);

    assert(nPayloadLen <= VNCP_PAYLOAD_LEN_MAX);

    bool bResult = m_objDataBuffer.Reallocate(sizeof(HeadT) + sizeof(BodyT) + nVarPayloadLen);

    if (bResult)
    {
        VNCP::VNC_MESSAGE_HEADER* pMsgHeader = (VNCP::VNC_MESSAGE_HEADER*)m_objDataBuffer.GetBufferHeadPtr();

        pMsgHeader->nMagic = HTONS((U16)VNCP::HEADER_MAGIC);
        pMsgHeader->nType = nType;
        pMsgHeader->nSessionID = HTONL(nSessionID);
        pMsgHeader->nFlag = nFlag;
        pMsgHeader->nPayloadLen = HTON3B(nPayloadLen);

        m_objDataBuffer.IncreaseDataLen(sizeof(HeadT) + sizeof(BodyT) + nVarPayloadLen);   //∏√¥˙¬Î≤ªƒ‹…Ÿ
    }

    return bResult;
}

// --------------------------------------------------------------------------------
/// <summary>
/// ≥ı ºªØ ˝æ›∞¸µƒƒ£∞Â∫Ø ˝£®Õ∆ºˆ π”√∏√∫Ø ˝£©
/// </summary>
/// <param name="nType"> ˝æ›∞¸µƒ¿‡–Õ</param>
/// <param name="nSessionID"> ˝æ›∞¸µƒ±Í÷æ</param>
/// <param name="pJsonObj">÷∏œÚ“ª∏ˆ”––Ú¡–ªØjson ˝æ›µƒ∂‘œÛ£¨–Ú¡–ªØ∫Û ˝æ›Ω´ÃÌº”µΩ ˝æ›∞¸∫Û√Ê</param>
/// <returns></returns>
/// <created>ª∆¿ˆ‘∆,2019/2/27</created>
/// <changed>ª∆¿ˆ‘∆,2019/2/27</changed>
template<class PObject>
inline bool CVNCPacket::InitialJsonT(const U8 nType, const U32 nSessionID, const U8 nFlag, const PObject pJsonObj)
{
    bool bResult = false;
    rapidjson::StringBuffer objBuffer;
    rapidjson::Document objJsonDoc(rapidjson::kObjectType);    //…˙≥…“ª∏ˆdom‘™ÀÿDocument
    CJSONSerializer objJsonSerializerHelper(objJsonDoc.GetAllocator());

    pJsonObj->Serialize(objJsonSerializerHelper);
    objJsonDoc.AddMember("data", objJsonSerializerHelper.GetJsonValue(), objJsonDoc.GetAllocator());
    rapidjson::Writer<rapidjson::StringBuffer> objWriter(objBuffer);

    bResult = objJsonDoc.Accept(objWriter);
    assert(bResult);

    if (bResult)
    {
        auto out = objBuffer.GetString();
        const int nVarPayloadLen = objBuffer.GetLength();

        bResult = Initial(nType, nSessionID, nFlag,nVarPayloadLen);

        if (bResult)
        {
            if (nVarPayloadLen > 0)
            {
                char* pDataptr = (char*)m_objDataBuffer.GetBufferHeadPtr() + sizeof(VNCP::VNC_MESSAGE_HEADER);

                memcpy(pDataptr, out, nVarPayloadLen);
            }
        }
    }

    return bResult;
}


// --------------------------------------------------------------------------------
/// <summary>
/// ¥” ˝æ›∞¸µƒ‘ÿ∫… ˝æ›÷–Ã·»°json∂‘œÛ
/// </summary>
/// <param name="refJsonObj">json∂‘œÛ</param>
/// <returns></returns>
/// <created>ª∆¿ˆ‘∆,2019/2/27</created>
/// <changed>ª∆¿ˆ‘∆,2019/2/27</changed>
// --------------------------------------------------------------------------------
template<class T>
inline bool CVNCPacket::ExtractJsonObjT(const unsigned int nStartPos, const unsigned int nLen, T & refJsonObj)
{
    bool bResult = false;

    if (nLen > 0)
    {
        rapidjson::Document objJsonDoc;
        objJsonDoc.Parse(
            (char*)(m_objDataBuffer.GetBufferHeadPtr()) + nStartPos
            , nLen);

        if (!objJsonDoc.IsNull())
        {
            auto itr = objJsonDoc.FindMember("data");

            if (objJsonDoc.MemberEnd() != itr)
            {
                CJSONSerializer objJsonSerializerHelper(objJsonDoc.GetAllocator());

                objJsonSerializerHelper.SetJsonValue(const_cast<rapidjson::Value&>(itr->value));
                refJsonObj.DeSerialize(objJsonSerializerHelper);

                bResult = true;
            }
        }
    }

    return bResult;
}

// --------------------------------------------------------------------------------
/// <summary>
/// ¥” ˝æ›∞¸µƒ‘ÿ∫… ˝æ›÷–Ã·»°json∂‘œÛ
/// </summary>
/// <param name="refJsonObj">json∂‘œÛ</param>
/// <returns></returns>
/// <created>ª∆¿ˆ‘∆,2019/2/27</created>
/// <changed>ª∆¿ˆ‘∆,2019/2/27</changed>
// --------------------------------------------------------------------------------
template<class TPacket, class TObject>
inline bool CVNCPacket::ExtractJsonObjT(TObject & refJsonObj)
{
    VNCP::VNC_MESSAGE_HEADER* pMsgHeader = (VNCP::VNC_MESSAGE_HEADER*)m_objDataBuffer.GetBufferHeadPtr();

    const bool bResult = ExtractJsonObjT<TObject>(sizeof(TPacket)
        , NTOH3B(pMsgHeader->nPayloadLen)
        , refJsonObj);

    return bResult;
}

using CVNCPacketPtr = std::shared_ptr<CVNCPacket>;

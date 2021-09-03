//
//  CDataPacket.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//

#ifndef CDataPacket_h
#define CDataPacket_h

#pragma once
#include "PacketBase.h"
#include "MessageHeadDef.h"
#include "JsonSerializer.h"
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>
#include <iostream>

#define FLAG_OPERATOR 0X0003
#define FLAG_VERSION 0X78
#define MSG_HEAD_FLAG_MAX 3


//extern char const CDATAPACKET_ID[];


class CDataPacket  : public CNetPacketBase
{
public:
    CDataPacket();
    CDataPacket(BYTE* pBuffer, int nLen);
    ~CDataPacket();
    CDataPacket(CDataPacket& rDataPacket);
    CDataPacket& operator=(CDataPacket& rDataPacket);

    //USE_CLASS_MEMORY_POOL(CDataPacket,128,16);

    using CNetPacketBase::CNetPacketBase;

    enum
    {
        PACKET_LEN_MAX = 100 * 1024,  ///< ◊Ó¥Ûµƒ ˝æ›∞¸≥§∂»
    };

public:
    void SetEnableEncipher(bool bEnableEncipher) {  m_bEnableEncipher = bEnableEncipher; }
    bool GetEnableEncipher()                           {  return m_bEnableEncipher; }

public: //œ¬√Êµƒ∫Ø ˝ «”ÎCRC32–£—Èœ‡πÿµƒ
    unsigned int CalculateCRC32();
    bool VerifyCRC32();

public:
    void Encipher(struct aes_ctx& refCAES);
    bool Decipherer(struct aes_ctx& refCAES);

public:
    bool IsValid();

public:
    bool Initial(UINT nID, WORD nType, BYTE nFlags, UINT nDataLen);

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
    template <class T>
    bool InitialT(UINT nID, WORD nType, BYTE nFlags, UINT nVarPayloadLen = 0)
    {
        bool bResult = m_objDataBuffer.Reallocate(nVarPayloadLen + sizeof(T));
        if (bResult)
        {
            m_objDataBuffer.Empty();

            if (!(nFlags & OT_RESPONSE))
            {
                nFlags |= OR_SUCCESS;
            }

            MESSAGE_HEADER* pMsgHeader = (MESSAGE_HEADER*)m_objDataBuffer.GetBufferHeadPtr();

            pMsgHeader->nMagic        = htons((U16)HEADER_MAGIC);
            pMsgHeader->nID            = htonl(nID);
            pMsgHeader->nType        = htons(nType);
            pMsgHeader->nFlags        = (nFlags | BYTE(VERSION_ONE));
            pMsgHeader->nPayloadLen    = htonl(MESSAGE_FIXED_PAYLOAD_LEN(T) + nVarPayloadLen); // ˝æ›∞¸–Ø¥¯ ˝æ›≥§∂»= ˝æ›Õ∑≥§∂»+πÃ∂® ˝æ›≥§∂»+ø…±‰ ˝æ›µƒ≥§∂»(ø…—°)
            pMsgHeader->nPadding = 0;
            pMsgHeader->nChecksum = 0;

            m_objDataBuffer.IncreaseDataLen(nVarPayloadLen + sizeof(T));   //∏√¥˙¬Î≤ªƒ‹…Ÿ
        }

        return bResult;
    }

    // --------------------------------------------------------------------------------
/// <summary>
/// ≥ı ºªØ ˝æ›∞¸µƒƒ£∞Â∫Ø ˝£®Õ∆ºˆ π”√∏√∫Ø ˝£©
/// </summary>
/// <param name="nID"> ˝æ›∞¸µƒID</param>
/// <param name="nType"> ˝æ›∞¸µƒ¿‡–Õ</param>
/// <param name="nFlags"> ˝æ›∞¸µƒ±Í÷æ</param>
/// <param name="pJsonObj">÷∏œÚ“ª∏ˆ”––Ú¡–ªØjson ˝æ›µƒ∂‘œÛ£¨–Ú¡–ªØ∫Û ˝æ›Ω´ÃÌº”µΩ ˝æ›∞¸∫Û√Ê</param>
/// <returns></returns>
/// <created>ª∆¿ˆ‘∆,2019/2/27</created>
/// <changed>ª∆¿ˆ‘∆,2019/2/27</changed>
    template <class TPacket, class PObject = CSerializeEntry*>
    bool InitialJsonT(UINT nID, WORD nType, BYTE nFlags, const PObject pJsonObj)
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

            const int nVarPayloadLen = (int)objBuffer.GetSize(); //objBuffer.GetLength();

            bResult = InitialT<TPacket>(nID, nType, nFlags, nVarPayloadLen);

            if (bResult)
            {
                if (nVarPayloadLen > 0)
                {
                    char* pDataptr = (char*)m_objDataBuffer.GetBufferHeadPtr() + sizeof(TPacket);

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
    template <class T>
    bool ExtractJsonObjT(const unsigned int nStartPos,const unsigned int nLen,T& refJsonObj)
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
    template <class TPacket,class TObject>
    bool ExtractJsonObjT(TObject& refJsonObj)
    {
        MESSAGE_HEADER* pMsgHeader = (MESSAGE_HEADER*)m_objDataBuffer.GetBufferHeadPtr();

        bool bResult = ExtractJsonObjT<TObject>(sizeof(TPacket)
            , htonl(pMsgHeader->nPayloadLen)
            , refJsonObj);

        return bResult;
    }

    template <class TPacket>
    std::string GetPayload()
    {
        MESSAGE_HEADER* pMsgHeader = (MESSAGE_HEADER*)m_objDataBuffer.GetBufferHeadPtr();
        std::string strResult((char*)(m_objDataBuffer.GetBufferHeadPtr()) + sizeof(TPacket)
            , htonl(pMsgHeader->nPayloadLen));

        return strResult;
    }

public:

    NET_EXTRACT_DATA_PACKET_RESULT ExtractData(CDataStreamBuffer& refDataBuffer);   //∑÷ŒˆÀ˘Ω” ’ ˝æ›
    int GetPacketLen();// ªÒ»°’˚∏ˆ±®µƒ≥§∂»°¢±æª˙◊÷Ω⁄–Ú
    int GetPacketType();// ªÒ»°±®Œƒ¿‡–Õ°¢±æª˙◊÷Ω⁄–Ú
    int GetPacketId();// ªÒ»°±®ŒƒID°¢±æª˙◊÷Ω⁄–Ú
    int GetPacketFlag();// ªÒ»°±®Œƒ≤Ÿ◊˜¿‡–Õ∫ÕΩ·π˚°¢±æª˙◊÷Ω⁄–Ú
    int GetPacketVer();                     //  ªÒ»°±®Œƒ∞Ê±æ°¢±æª˙◊÷Ω⁄–Ú
    BYTE* GetBuffer();
    BYTE* GetBufferHeadPtr() { return m_objDataBuffer.GetBufferHeadPtr(); }
private:
    bool                           m_bEnableEncipher;            // «∑Ò–Ë“™º”√‹
};

#endif

//
//  PacketBase.hpp
//  AnyViewer
//
//  Created by Aomei on 2021/8/31.
//

#pragma once

#include "TypeDefine.h"
#include "BufferImpl.h"

#define CRC32_TABLE_SIZE_MAX 256

///     ˝æ›∞¸Ã·»°Ω·π˚√∂æŸ∂®“Â
///
///    ∏√√∂æŸ∂®“Â¡ÀÃ·»°ª∫≥Â«¯ ˝æ›∞¸µƒΩ·π˚¿‡–Õ\n
enum NET_EXTRACT_DATA_PACKET_RESULT
{
    NEDPR_INVALID = 0,     ///< √∂æŸ£¨Œﬁ–ßµƒ ˝æ›∞¸
    NEDPR_JUST_A_PACKET,           ///< √∂æŸ£¨∏’∫√“ª∏ˆ ˝æ›∞¸
    NEDPR_INCOMPLETE_PACKET,   ///< √∂æŸ£¨≤ªÕÍ’˚µƒ“ª∏ˆ ˝æ›∞¸
    NEDPR_MORE_PACKETS            ///< √∂æŸ£¨∏¸∂‡µƒ ˝æ›∞¸
};

typedef CBufferImplT<BYTE>    CDataStreamBuffer;

//=============================================
/// @brief ≥ÈœÛ ˝æ›∞¸Ω”ø⁄∂®“Â
///
/// ’˚∏ˆÕ®–≈øÚº‹÷–À˘”–µƒ ˝æ›∞¸∂‘œÛµƒª˘¿‡
//=============================================
class CNetPacket
{
public:
    CNetPacket() {}

    /// @brief ¥” ˝æ›¡˜÷–Ã·»° ˝æ›∞¸
    ///
    /// @param CDataStreamBuffer ¥Ê∑≈ ˝æ›∞¸µƒª∫≥Â«¯∂‘œÛ
    /// @return  ∑µªÿÃ·»° ˝æ›∞¸∫ÛµƒΩ·π˚,≤Œøº¿‡–ÕNET_EXTRACT_DATA_PACKET_RESULT
    virtual NET_EXTRACT_DATA_PACKET_RESULT ExtractData(CDataStreamBuffer&) = 0;

    /// @brief «Âø’ ˝æ›∞¸
    virtual void ResetContent() = 0;

    /// @brief µ√µΩ ˝æ›Õ∑÷∏’Î
    ///
    /// @return  ∑µªÿ“ª∏ˆ÷∏œÚ ˝æ›∞¸Õ∑µƒ÷∏’Î
    virtual BYTE* GetHeaderPtr() = 0;

    /// @brief µ√µΩ’˚∏ˆ ˝æ›∞¸µƒ≥§∂»
    ///
    /// @return  ∑µªÿ ˝æ›∞¸”––ß≥§∂»
    virtual int GetTotalLength() = 0;
    virtual ~CNetPacket() {}

    virtual int GetPacketId() = 0; // ªÒ»°±®ŒƒID°¢±æª˙◊÷Ω⁄–Ú
public:
    /// @brief ÷ÿ‘ÿ∫Ø ˝BYTE*( )
    ///
    /// @return  ∑µªÿ“ª∏ˆ÷∏œÚ ˝æ›∞¸Õ∑µƒ÷∏’Î
    operator BYTE*() { return GetHeaderPtr(); }
};

//=============================================
/// @brief Õ®”√µƒ ˝æ›∞¸ª˘¿‡
///
/// Õ®”√µƒ ˝æ›∞¸ª˘¿‡,”µ”–“ª∏ˆ¡˜ª∫≥Â«¯≥…‘±£¨”√”⁄¥Ê¥¢ ˝æ›∞¸ ˝æ›
//=============================================
class CNetPacketBase : public CNetPacket
{
public:

    /// @brief ππ‘Ï∫Ø ˝
    CNetPacketBase();

    /// @brief ππ‘Ï∫Ø ˝
    CNetPacketBase(int nSize);

    /// @brief ππ‘Ï∫Ø ˝
    CNetPacketBase(const BYTE* pBuffer, int nLen);

    /// @brief Œˆππ∫Ø ˝
    virtual ~CNetPacketBase();

    /// @brief øΩ±¥ππ‘Ï∫Ø ˝
    CNetPacketBase(const CNetPacketBase& refDataPacket);

    /// @brief ∏≥÷µ∫Ø ˝
    CNetPacketBase& operator=(const CNetPacketBase& refDataPacket);
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
    void Attach(BYTE* pData, int nLen) { m_objDataBuffer.Attach(pData, nLen); m_objDataBuffer.IncreaseDataLen(nLen); }

    /// @brief »°œ˚πÿ¡™
    void Dettach() { m_objDataBuffer.Dettach(); }
public:
    virtual NET_EXTRACT_DATA_PACKET_RESULT ExtractData(CDataStreamBuffer&) { return NEDPR_INVALID; }        //¥” ˝æ›¡˜÷–Ã·»° ˝æ›∞¸
    virtual void ResetContent();                                                                       //«Âø’ ˝æ›∞¸
    virtual BYTE* GetHeaderPtr();                                                                    //µ√µΩ ˝æ›Õ∑÷∏’Î
    virtual int GetTotalLength();                                                                        //µ√µΩ’˚∏ˆ ˝æ›∞¸µƒ≥§∂»
    virtual int Copy(const BYTE* pBuffer, int nLen);                                             //¥”÷∏∂®µƒ‘¥øΩ±¥ ˝æ›

public: //œ¬√Êµƒ∫Ø ˝ «”ÎCRC32–£—Èœ‡πÿµƒ

    /// @brief ≥ı ºªØCRC32º∆À„±Ì
    static void  InitCRC32Table(void);

    /// @brief º∆À„÷∏∂® ˝æ›µƒCRC32÷µ
    static UINT CalculateCRC32(const BYTE* pData, UINT nLength);

protected:
    CDataStreamBuffer        m_objDataBuffer;                  ///< ¥Ê∑≈ ˝æ›µƒª∫≥Â«¯∂‘œÛ

private:
    static bool                   s_bCRC32TableInitialized;        ///<  «∑Ò“—æ≠≥ı ºªØCRC32±Ì
    static UINT                    s_szCRC32Table[CRC32_TABLE_SIZE_MAX];           ///< ”√”⁄CRC32–£—Èµƒ±Ì
};

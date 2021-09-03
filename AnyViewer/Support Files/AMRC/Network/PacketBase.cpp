//
//  PacketBase.cpp
//  AnyViewer
//
//  Created by Aomei on 2021/8/31.
//

#include "PacketBase.hpp"
#include "TypeDefine.h"

UINT          CNetPacketBase::s_szCRC32Table[CRC32_TABLE_SIZE_MAX] = { 0 };
bool         CNetPacketBase::s_bCRC32TableInitialized = false;


/*--------------------------------------------CNetPacketBase---------------------------------------------------*/
//------------------------------------------------------------------------------------
// CNetPacketBase::CNetPacketBase
/// @brief ππ‘Ï∫Ø ˝
///
/// ∏√∞Êππ‘Ï∫Ø ˝ΩˆΩˆ≥ı ºªØCRC32–£—È∫Õº∆À„±Ì
///
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
CNetPacketBase::CNetPacketBase()
{
    if (!s_bCRC32TableInitialized)
    {
        InitCRC32Table();
        s_bCRC32TableInitialized = true;
    }
}
//------------------------------------------------------------------------------------
// CNetPacketBase::CNetPacketBase
/// @brief ππ‘Ï∫Ø ˝
///
/// ∏√∞Êππ‘Ï∫Ø ˝ø…“‘≥ı º ˝æ›ª∫≥Â«¯µƒ¥Û–°,«“≥ı ºªØCRC32–£—È∫Õº∆À„±Ì
/// @param nSize  ˝æ›ª∫≥Â«¯µƒ≥ı º÷µ
///
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
CNetPacketBase::CNetPacketBase(INT nSize)
    : m_objDataBuffer(nSize)
{
    if (!s_bCRC32TableInitialized)
    {
        InitCRC32Table();
        s_bCRC32TableInitialized = true;
    }
}

//------------------------------------------------------------------------------------
// CNetPacketBase::CNetPacketBase
/// @brief ππ‘Ï∫Ø ˝
///
/// ∏√∞Êππ‘Ï∫Ø ˝”√÷∏∂®µƒ ˝æ›ππ‘Ïµ±«∞µƒ ˝æ›∞¸,«“≥ı ºªØCRC32–£—È∫Õº∆À„±Ì
/// @param pBuffer “ª∏ˆ÷∏œÚ‘¥ ˝æ›µƒƒ⁄¥Ê÷∏’Î
/// @param nLen ‘¥ ˝æ›µƒ≥§∂»
///
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
CNetPacketBase::CNetPacketBase(const U8* pBuffer, INT nLen)
    : m_objDataBuffer(pBuffer, nLen)
{
    if (!s_bCRC32TableInitialized)
    {
        InitCRC32Table();
        s_bCRC32TableInitialized = true;
    }
}

//------------------------------------------------------------------------------------
// CNetPacketBase::CNetPacketBase
/// @brief øΩ±¥ππ‘Ï∫Ø ˝
///
/// ∏√∞Êππ‘Ï∫Ø ˝»√”√ªßø…“‘”√∆‰À˚µƒ ˝æ›∞¸∂‘œÛ¿¥ππ‘Ïµ±«∞µƒ ˝æ›∞¸
/// @param refDataPacket “ª∏ˆ÷∏œÚ‘¥ ˝æ›∞¸∂‘œÛµƒ≤Œøº
///
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
CNetPacketBase::CNetPacketBase(const CNetPacketBase& refDataPacket)
{
    m_objDataBuffer = refDataPacket.m_objDataBuffer;
}

//------------------------------------------------------------------------------------
// CNetPacketBase::operator=
/// @brief ∏≥÷µ÷ÿ‘ÿ∫Ø ˝
///
/// Õ®π˝∏√∫Ø ˝Œ“√«ø…“‘∞—“ª∏ˆ ˝æ›∞¸∂‘œÛ∏≥÷µ∏¯¡ÌÕ‚“ª∏ˆ ˝æ›∞¸∂‘œÛ
/// @param refDataPacket “ª∏ˆ÷∏œÚ‘¥ ˝æ›∞¸∂‘œÛµƒ≤Œøº
///
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------

CNetPacketBase& CNetPacketBase::operator=(const CNetPacketBase& refDataPacket)
{
    if (this != &refDataPacket)
    {
        m_objDataBuffer = refDataPacket.m_objDataBuffer;
    }
    return *this;
}

//------------------------------------------------------------------------------------
// CNetPacketBase::Copy
/// @brief øΩ±¥ ˝æ›
///
/// Õ®π˝∏√∫Ø ˝Œ“√«ø…“‘∞—“ª∏ˆ ˝æ›∞¸∂‘œÛ∏≥÷µ∏¯¡ÌÕ‚“ª∏ˆ ˝æ›∞¸∂‘œÛ
/// @param refDataPacket “ª∏ˆ÷∏œÚ‘¥ ˝æ›∞¸∂‘œÛµƒ≤Œøº
///
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
INT CNetPacketBase::Copy(const U8* pBuffer, INT nLen)
{
    m_objDataBuffer.Empty();

    INT nResult = 0;
    assert(pBuffer);
    if (pBuffer)
    {
        nResult = m_objDataBuffer.Write(pBuffer, nLen);
    }

    return nResult;
}

CNetPacketBase::~CNetPacketBase()
{
}

//------------------------------------------------------------------------------------
// CNetPacketBase::ResetContent
/// @brief «Âø’µ±«∞ ˝æ›∞¸∂‘œÛµƒ ˝æ›–≈œ¢
///
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
void CNetPacketBase::ResetContent()
{
    m_objDataBuffer.Empty();
}

//------------------------------------------------------------------------------------
// CNetPacketBase::GetHeaderPtr
/// @brief µ√µΩ ˝æ›∞¸µƒƒ⁄¥Ê ˝æ›÷∏’Î
///
/// @return  ∑µªÿ“ª∏ˆ÷∏œÚª∫≥Â«¯∂‘œÛµƒ≤Œøº÷∏’Î
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
U8* CNetPacketBase::GetHeaderPtr()
{
    return m_objDataBuffer.GetBufferHeadPtr();
}

//------------------------------------------------------------------------------------
// CNetPacketBase::GetTotalLength
/// @brief µ√µΩ ˝æ›∞¸µƒ”––ß ˝æ›≥§∂»
///
/// @return  ∑µªÿµ±«∞ ˝æ›∞¸µƒ”––ß ˝æ›≥§∂»
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
INT CNetPacketBase::GetTotalLength()
{
    return m_objDataBuffer.GetDataLen();
}

//------------------------------------------------------------------------------------
// CNetPacketBase::InitCRC32Table
/// @brief ≥ı ºªØcrc±Ì,…˙≥…32Œª¥Û–°µƒcrc±Ì
///
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
void CNetPacketBase::InitCRC32Table(void)
{
    UINT c;
    UINT i, j;

    for (i = 0; i < 256; i++)
    {
        c = (UINT)i;
        for (j = 0; j < 8; j++)
        {
            if (c & 1)
            {
                c = 0xedb88320L ^ (c >> 1);
            }
            else
            {
                c = c >> 1;
            }
        }
        s_szCRC32Table[i] = c;
    }
}

//------------------------------------------------------------------------------------
// CNetPacketBase::CalculateCRC32
/// @brief º∆À„ ˝æ›∞¸µƒCRC32–£—È÷µ(◊¢“‚£∫À˘”–µƒ ˝æ›±ÿ–Î «Õ¯¬Á◊÷Ω⁄–Ú)
///
/// @param pData “ª∏ˆ÷∏œÚ±ªº”√‹ ˝æ›µƒ÷∏’Î
/// @param nLength ±ªº”√‹ ˝æ›µƒ≥§∂»
/// @return ∑µªÿ ˝æ›µƒCRC32–£—È÷µ
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
UINT   CNetPacketBase::CalculateCRC32(const U8* pData, UINT nLength)
{
    UINT nCRC = 0;
    UINT i = 0;

    if (pData && nLength > 0)
    {
        for (i = 0; i < nLength; i++)
        {
            nCRC = s_szCRC32Table[(nCRC ^ pData[i]) & 0xff] ^ (nCRC >> 8);
        }
    }

    return nCRC;
}


//
//  VNCPacket.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/9.
//


#include "VNCPacket.h"
#include "log.h"

#include <exception>


/*--------------------------------------------CVNCPacket---------------------------------------------------*/
//------------------------------------------------------------------------------------
// CVNCPacket::CVNCPacket
/// @brief ππ‘Ï∫Ø ˝
///
/// ∏√∞Êππ‘Ï∫Ø ˝ΩˆΩˆ≥ı ºªØCRC32–£—È∫Õº∆À„±Ì
///
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
CVNCPacket::CVNCPacket()
{

}
//------------------------------------------------------------------------------------
// CVNCPacket::CVNCPacket
/// @brief ππ‘Ï∫Ø ˝
///
/// ∏√∞Êππ‘Ï∫Ø ˝ø…“‘≥ı º ˝æ›ª∫≥Â«¯µƒ¥Û–°,«“≥ı ºªØCRC32–£—È∫Õº∆À„±Ì
/// @param nSize  ˝æ›ª∫≥Â«¯µƒ≥ı º÷µ
///
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
CVNCPacket::CVNCPacket(INT nSize)
    : m_objDataBuffer(nSize)
{
    
}

//------------------------------------------------------------------------------------
// CVNCPacket::CVNCPacket
/// @brief ππ‘Ï∫Ø ˝
///
/// ∏√∞Êππ‘Ï∫Ø ˝”√÷∏∂®µƒ ˝æ›ππ‘Ïµ±«∞µƒ ˝æ›∞¸,«“≥ı ºªØCRC32–£—È∫Õº∆À„±Ì
/// @param pBuffer “ª∏ˆ÷∏œÚ‘¥ ˝æ›µƒƒ⁄¥Ê÷∏’Î
/// @param nLen ‘¥ ˝æ›µƒ≥§∂»
///
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
CVNCPacket::CVNCPacket(const U8* pBuffer, INT nLen)
    : m_objDataBuffer(pBuffer, nLen)
{
}

//------------------------------------------------------------------------------------
// CVNCPacket::CVNCPacket
/// @brief øΩ±¥ππ‘Ï∫Ø ˝
///
/// ∏√∞Êππ‘Ï∫Ø ˝»√”√ªßø…“‘”√∆‰À˚µƒ ˝æ›∞¸∂‘œÛ¿¥ππ‘Ïµ±«∞µƒ ˝æ›∞¸
/// @param refDataPacket “ª∏ˆ÷∏œÚ‘¥ ˝æ›∞¸∂‘œÛµƒ≤Œøº
///
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
CVNCPacket::CVNCPacket(const CVNCPacket& refDataPacket)
{
    m_objDataBuffer = refDataPacket.m_objDataBuffer;
}

//------------------------------------------------------------------------------------
// CVNCPacket::operator=
/// @brief ∏≥÷µ÷ÿ‘ÿ∫Ø ˝
///
/// Õ®π˝∏√∫Ø ˝Œ“√«ø…“‘∞—“ª∏ˆ ˝æ›∞¸∂‘œÛ∏≥÷µ∏¯¡ÌÕ‚“ª∏ˆ ˝æ›∞¸∂‘œÛ
/// @param refDataPacket “ª∏ˆ÷∏œÚ‘¥ ˝æ›∞¸∂‘œÛµƒ≤Œøº
///
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------

CVNCPacket& CVNCPacket::operator=(const CVNCPacket& refDataPacket)
{
    if (this != &refDataPacket)
    {
        m_objDataBuffer = refDataPacket.m_objDataBuffer;
    }
    return *this;
}



CVNCPacket::~CVNCPacket()
{
}


// ********************************************************************************
/// <summary>
/// –¥»Î ˝æ›µΩ
/// </summary>
/// <param name="pData">÷∏œÚ ˝æ›µƒ÷∏’Î</param>
/// <param name="nLen">±ª–¥ ˝æ›∞¸µƒ≥§∂»</param>
/// <created>Andy,2020/12/1</created>
/// <changed>Andy,2020/12/1</changed>
// ********************************************************************************
void CVNCPacket::Write(const U8* pData, const int nLen)
{
    m_objDataBuffer.Write(pData, nLen);
}

void CVNCPacket::WriteUInt8(const U8 x)
{
    Write((unsigned char *)&x, 1);
};

void CVNCPacket::WriteUInt16(const U16 data)
{
    //U8 szBuffer[2];

    //szBuffer[0] = GETBYTE(data, 1);
    //szBuffer[1] = GETBYTE(data, 0);

    Write((unsigned char *)&data, sizeof(data));
}

void CVNCPacket::WriteUInt32(const U32 data)
{
    //U8 szBuffer[4];

    //szBuffer[0] = GETBYTE(data, 3);
    //szBuffer[1] = GETBYTE(data, 2);
    //szBuffer[2] = GETBYTE(data, 1);
    //szBuffer[3] = GETBYTE(data, 0);

    Write((unsigned char *)&data, sizeof(data));
}

void CVNCPacket::WriteUInt64(const U64 data)
{
    //U8 szBuffer[8];

    //szBuffer[0] = GETBYTE(data, 7);
    //szBuffer[1] = GETBYTE(data, 6);
    //szBuffer[2] = GETBYTE(data, 5);
    //szBuffer[3] = GETBYTE(data, 4);
    //szBuffer[4] = GETBYTE(data, 3);
    //szBuffer[5] = GETBYTE(data, 2);
    //szBuffer[6] = GETBYTE(data, 1);
    //szBuffer[7] = GETBYTE(data, 0);

    Write((unsigned char *)&data, sizeof(data));
}

int CVNCPacket::Read(U8 * pData, const int nLen)
{
    return m_objDataBuffer.Read(pData, nLen);
}

U8 CVNCPacket::ReadUInt8()
{
    U8 nResult = 0;

    Read((U8*)&nResult,sizeof(nResult));

    return nResult;
}

U16 CVNCPacket::ReadUInt16()
{
    U16 nResult = 0;

    Read((U8*)&nResult, sizeof(nResult));

    return nResult;
}

U32 CVNCPacket::ReadUInt32()
{
    U32 nResult = 0;

    Read((U8*)&nResult, sizeof(nResult));

    return nResult;
}

U64 CVNCPacket::ReadUInt64()
{
    U64 nResult = 0;

    Read((U8*)&nResult, sizeof(nResult));

    return nResult;
}


// ********************************************************************************
/// <summary>
/// …Ë÷√ ˝æ›∞¸µƒ≥§∂»£¨Õ¨ ±º∆À„‘ÿ∫… ˝æ›µƒ≥§∂»
/// </summary>
/// <created>Andy,2020/12/1</created>
/// <changed>Andy,2020/12/1</changed>
// ********************************************************************************
void CVNCPacket::SetLength(const int nValidPacketLen)
{
    assert(nValidPacketLen >= sizeof(VNCP::VNC_MESSAGE_HEADER));

    VNCP::VNC_MESSAGE_HEADER* pMsgHeader = (VNCP::VNC_MESSAGE_HEADER*)m_objDataBuffer.GetDataHeadPtr();
    const unsigned int nPayloadLen = (nValidPacketLen - sizeof(VNCP::VNC_MESSAGE_HEADER));

    assert(nPayloadLen <= VNCP_PAYLOAD_LEN_MAX);
    pMsgHeader->nPayloadLen = HTON3B(nPayloadLen);
    m_objDataBuffer.SetCurWritingPos(nValidPacketLen);
}

// ********************************************************************************
/// <summary>
/// º∆À„‘ÿ∫… ˝æ›µƒ≥§∂»
/// </summary>
/// <created>Andy,2020/12/1</created>
/// <changed>Andy,2020/12/1</changed>
// ********************************************************************************
void CVNCPacket::CalcPayloadLen()
{
    VNCP::VNC_MESSAGE_HEADER* pMsgHeader = (VNCP::VNC_MESSAGE_HEADER*)m_objDataBuffer.GetDataHeadPtr();
    const unsigned int nPayloadLen = (m_objDataBuffer.GetDataLen() - sizeof(VNCP::VNC_MESSAGE_HEADER));

    assert(nPayloadLen <= VNCP_PAYLOAD_LEN_MAX);
    pMsgHeader->nPayloadLen = HTON3B(nPayloadLen);
}

// ********************************************************************************
/// <summary>
/// …Ë÷√ª∫≥Â«¯÷–µƒ∂¡∆´“∆
/// </summary>
/// <param name="nOffset"></param>
/// <created>Andy,2020/12/2</created>
/// <changed>Andy,2020/12/2</changed>
// ********************************************************************************
void CVNCPacket::SetReadingOffset(const unsigned int nOffset)
{
    m_objDataBuffer.SetCurReadingPos(nOffset);
}

// ********************************************************************************
/// <summary>
/// ¥” ˝æ›ª∫≥Â«¯÷–∑÷¿Î“ª∏ˆ ˝æ›∞¸
/// </summary>
/// <param name="refDataBuffer"> ˝æ›ª∫≥Â«¯</param>
/// <returns>∑µªÿÃ·»°µƒΩ·π˚</returns>
/// <created>Andy,2020/11/27</created>
/// <changed>Andy,2020/11/27</changed>
// ********************************************************************************
NET_EXTRACT_DATA_PACKET_RESULT CVNCPacket::ExtractData(CDataStreamBuffer &refDataBuffer)
{
    NET_EXTRACT_DATA_PACKET_RESULT nResult = NEDPR_INVALID;
    const BYTE* pData = (BYTE*)refDataBuffer.GetDataHeadPtr();
    const UINT   DATA_LENGTH = refDataBuffer.GetDataLen();

    for (UINT i = 0; i < DATA_LENGTH; i++)
    {
        // £”‡ ˝æ›≥§∂»
        const int nRemainDateLen = int(DATA_LENGTH - i);

        if (nRemainDateLen < sizeof(VNCP::VNC_MESSAGE_HEADER))
        {
            nResult = NEDPR_INCOMPLETE_PACKET;
            break;
        }

        const VNCP::VNC_MESSAGE_HEADER* pMsgHeader = reinterpret_cast<const VNCP::VNC_MESSAGE_HEADER*>(&pData[i]);

        //’“µΩ±®Õ∑Ãÿ’˜¬Î
        if (VNCP::HEADER_MAGIC == ntohs(pMsgHeader->nMagic))
        {
            //∏√ ˝æ›±®≥§∂»
            const int PACKET_LENGTH = NTOH3B(pMsgHeader->nPayloadLen) + sizeof(VNCP::VNC_MESSAGE_HEADER);

            if (PACKET_LENGTH == nRemainDateLen)
            {
                m_objDataBuffer.Empty();

                if (m_objDataBuffer.GetSize() < PACKET_LENGTH)
                {
                    m_objDataBuffer.Reallocate(PACKET_LENGTH);
                }

                refDataBuffer.Read(m_objDataBuffer, PACKET_LENGTH);
                nResult = NEDPR_JUST_A_PACKET;
            }
            else if (PACKET_LENGTH < nRemainDateLen)
            {
                m_objDataBuffer.Empty();

                if (m_objDataBuffer.GetSize() < PACKET_LENGTH)
                {
                    m_objDataBuffer.Reallocate(PACKET_LENGTH);
                }

                refDataBuffer.Read(m_objDataBuffer, PACKET_LENGTH);
                nResult = NEDPR_MORE_PACKETS;
            }
            else
            {
                nResult = NEDPR_INCOMPLETE_PACKET;
            }

            // break∫‹÷ÿ“™£¨“ª¥Œ÷ªÃ·»°“ª∏ˆ ˝æ›±®
            break;
        }
        else
        {
            nResult = NEDPR_INVALID;
            LOG_ERROR("Invalid vnc data!");
            assert(false);
        }


        refDataBuffer.MoveReadingPos(1);
    }

    return nResult;
}

//------------------------------------------------------------------------------------
// CVNCPacket::ResetContent
/// @brief «Âø’µ±«∞ ˝æ›∞¸∂‘œÛµƒ ˝æ›–≈œ¢
///
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
void CVNCPacket::ResetContent()
{
    m_objDataBuffer.Empty();
}

//------------------------------------------------------------------------------------
// CVNCPacket::GetHeaderPtr
/// @brief µ√µΩ ˝æ›∞¸µƒƒ⁄¥Ê ˝æ›÷∏’Î
///
/// @return  ∑µªÿ“ª∏ˆ÷∏œÚª∫≥Â«¯∂‘œÛµƒ≤Œøº÷∏’Î
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
U8* CVNCPacket::GetHeaderPtr()
{
    return m_objDataBuffer.GetBufferHeadPtr();
}

//------------------------------------------------------------------------------------
// CVNCPacket::GetTotalLength
/// @brief µ√µΩ ˝æ›∞¸µƒ”––ß ˝æ›≥§∂»
///
/// @return  ∑µªÿµ±«∞ ˝æ›∞¸µƒ”––ß ˝æ›≥§∂»
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
INT CVNCPacket::GetTotalLength()
{
    return m_objDataBuffer.GetDataLen();
}

// ********************************************************************************
/// <summary>
/// µ√µΩœ˚œ¢¿‡–Õ
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/11/27</created>
/// <changed>Andy,2020/11/27</changed>
// ********************************************************************************
U8 CVNCPacket::GetType()
{
    U8 nResult = 0;

    if (m_objDataBuffer.GetDataLen() >= sizeof(VNCP::VNC_MESSAGE_HEADER))
    {
        VNCP::VNC_MESSAGE_HEADER* pMsgHeader = (VNCP::VNC_MESSAGE_HEADER*)m_objDataBuffer.GetBufferHeadPtr();

        nResult = pMsgHeader->nType;
    }

    return nResult;
}

// ********************************************************************************
/// <summary>
/// µ√µΩª·ª∞ID
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/11/28</created>
/// <changed>Andy,2020/11/28</changed>
// ********************************************************************************
U32 CVNCPacket::GetSessionID()
{
    U32 nResult = 0;

    if (m_objDataBuffer.GetDataLen() >= sizeof(VNCP::VNC_MESSAGE_HEADER))
    {
        VNCP::VNC_MESSAGE_HEADER* pMsgHeader = (VNCP::VNC_MESSAGE_HEADER*)m_objDataBuffer.GetBufferHeadPtr();

        nResult = NTOHL(pMsgHeader->nSessionID);
    }

    return nResult;
}

// ********************************************************************************
/// <summary>
/// µ√µΩ‘ÿ∫… ˝æ›µƒ≥§∂»
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/11/27</created>
/// <changed>Andy,2020/11/27</changed>
// ********************************************************************************
unsigned int CVNCPacket::GetPayloadLen() const
{
    unsigned int nResult = 0;

    if (m_objDataBuffer.GetDataLen() >= sizeof(VNCP::VNC_MESSAGE_HEADER))
    {
        VNCP::VNC_MESSAGE_HEADER* pMsgHeader = (VNCP::VNC_MESSAGE_HEADER*)m_objDataBuffer.GetBufferHeadPtr();

        nResult = NTOH3B(pMsgHeader->nPayloadLen);
    }

    return nResult;
}

//------------------------------------------------------------------------------------
// CVNCPacket::Copy
/// @brief øΩ±¥ ˝æ›
///
/// Õ®π˝∏√∫Ø ˝Œ“√«ø…“‘∞—“ª∏ˆ ˝æ›∞¸∂‘œÛ∏≥÷µ∏¯¡ÌÕ‚“ª∏ˆ ˝æ›∞¸∂‘œÛ
/// @param refDataPacket “ª∏ˆ÷∏œÚ‘¥ ˝æ›∞¸∂‘œÛµƒ≤Œøº
///
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
INT CVNCPacket::Copy(const U8* pBuffer, INT nLen)
{
    INT nResult = 0;

    m_objDataBuffer.Empty();
    assert(pBuffer);

    if (pBuffer)
    {
        nResult = m_objDataBuffer.Write(pBuffer, nLen);
    }

    return nResult;
}

// ********************************************************************************
/// <summary>
/// ≥ı ºªØ ˝æ›∞¸
/// </summary>
/// <param name="nType">¿‡–Õ</param>
/// <param name="nSessionID">ª·ª∞</param>
/// <param name="nFlag"></param>
/// <param name="nVarPayloadLen">ø…±‰ ˝æ›≥§∂»</param>
/// <returns>true:≥…π¶,false: ß∞‹</returns>
/// <created>Andy,2020/11/30</created>
/// <changed>Andy,2020/12/10</changed>
// ********************************************************************************
bool CVNCPacket::Initial(const U8 nType, const U32 nSessionID, const U8 nFlag, const UINT nVarPayloadLen)
{
    bool bResult = m_objDataBuffer.Reallocate(sizeof(VNCP::VNC_MESSAGE_HEADER) + nVarPayloadLen);

    if (bResult)
    {
        assert(nVarPayloadLen <= VNCP_PAYLOAD_LEN_MAX);

        VNCP::VNC_MESSAGE_HEADER* pMsgHeader = (VNCP::VNC_MESSAGE_HEADER*)m_objDataBuffer.GetBufferHeadPtr();

        pMsgHeader->nMagic = htons((U16)VNCP::HEADER_MAGIC);
        pMsgHeader->nType = nType;
        pMsgHeader->nSessionID = htonl(nSessionID);
        pMsgHeader->nFlag = nFlag;
        pMsgHeader->nPayloadLen = HTON3B(nVarPayloadLen);

        m_objDataBuffer.IncreaseDataLen(sizeof(VNCP::VNC_MESSAGE_HEADER) + nVarPayloadLen);   //∏√¥˙¬Î≤ªƒ‹…Ÿ
    }

    return bResult;
}

// ********************************************************************************
/// <summary>
/// µ√µΩ±Í÷æ
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/12/14</created>
/// <changed>Andy,2020/12/14</changed>
// ********************************************************************************
U8 CVNCPacket::GetFlag()
{
    VNCP::VNC_MESSAGE_HEADER* pMsgHeader = (VNCP::VNC_MESSAGE_HEADER*)m_objDataBuffer.GetBufferHeadPtr();

    return pMsgHeader->nFlag;
}


//
//  CDataPacket.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//

#import "CDataPacket.h"

#include <exception>
#include "CDataPacket.h"
#include "EccCode.h"

char const CDATAPACKET_ID[] = "CDataPacket";
const unsigned int  ENCRYPTED_DATA_BLOCK_LENGTH = 16; ///< º”√‹ ˝æ›øÏµƒ≥§∂»

CDataPacket::CDataPacket()
    : m_bEnableEncipher(true)
{
}

CDataPacket::CDataPacket(BYTE* pBuffer, int nLen)
    : CNetPacketBase(pBuffer,nLen)
    , m_bEnableEncipher(true)
{

}

CDataPacket::CDataPacket(CDataPacket& rDataPacket)
    : CNetPacketBase(rDataPacket)
    , m_bEnableEncipher(rDataPacket.GetEnableEncipher())
{
}

CDataPacket& CDataPacket::operator=( CDataPacket& rDataPacket)
{
    if (this != &rDataPacket)
    {
        *((CNetPacketBase*)this) = rDataPacket;
        m_bEnableEncipher         = rDataPacket.GetEnableEncipher();
    }

    return *this;
}


CDataPacket::~CDataPacket()
{
}

//------------------------------------------------------------------------------------
// ∫Ø ˝√˚≥∆£∫Initial
/// @brief ≥ı ºªØ ˝æ›∞¸
///
/// @param _in UINT nID-> ˝æ›∞¸µƒID
/// @param _in WORD nType-> ˝æ›∞¸µƒ¿‡–Õ
/// @param _in WORD nFlags-> ˝æ›∞¸µƒ±Í÷æ
/// @param _in UINT nDataLen->–Ø¥¯µƒ‘ÿ∫… ˝æ›µƒ≥§∂» (ƒ¨»œ «0)
/// @return true->≥…π¶,false-> ß∞‹ (÷˜“™ «∑÷≈‰ƒ⁄¥Ê ß∞‹)
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
bool CDataPacket::Initial(UINT nID, WORD nType, BYTE nFlags, UINT nDataLen)
{
    bool bResult = m_objDataBuffer.Reallocate(nDataLen + sizeof(MESSAGE_HEADER));
    if (bResult)
    {
        m_objDataBuffer.Empty();

        MESSAGE_HEADER* pMsgHeader = (MESSAGE_HEADER*)m_objDataBuffer.GetDataHeadPtr();

        pMsgHeader->nMagic        = htons((U32)HEADER_MAGIC);
        pMsgHeader->nID            = htonl(nID);
        pMsgHeader->nType        = htons(nType);
        pMsgHeader->nFlags        = (nFlags | BYTE(VERSION_ONE));
        pMsgHeader->nPayloadLen    = htonl(nDataLen);
        pMsgHeader->nChecksum = 0;

        m_objDataBuffer.IncreaseDataLen(nDataLen + sizeof(MESSAGE_HEADER));  //∏√¥˙¬Î≤ªƒ‹…Ÿ
    }

    return bResult;
}

//------------------------------------------------------------------------------------
// ∫Ø ˝√˚≥∆£∫CalculateCRC32
/// @briefº∆À„ ˝æ›∞¸µƒCRC32–£—È÷µ(◊¢“‚£∫À˘”–µƒ ˝æ›±ÿ–Î «Õ¯¬Á◊÷Ω⁄–Ú)
///
/// @param _in const BYTE* pBuffer->“ª∏ˆ÷∏œÚª∫≥Â«¯µƒ÷∏’Î
/// @param _in  int nLen-> ˝æ›≥§∂»
/// @return ∑µªÿ ˝æ›µƒCRC32–£—È÷µ
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
unsigned int CDataPacket::CalculateCRC32()
{
    const BYTE* pBuffer  = m_objDataBuffer.GetDataHeadPtr();
    const UINT nDataLen = m_objDataBuffer.GetDataLen();
    const MESSAGE_HEADER* pMsgHeader = (MESSAGE_HEADER*)pBuffer;


    // ¥À¥¶µƒnPayloadLen“—æ≠ «æ≠π˝ÃÓ≥‰∂‘∆Îº”√‹µƒ£¨“Ú¥À≤ª“™º”ÃÓ≥‰ ˝æ›
    assert(nDataLen == sizeof(MESSAGE_HEADER) + htonl(pMsgHeader->nPayloadLen));
    ((MESSAGE_HEADER*)pBuffer)->nChecksum = 0;
    
    const unsigned int nCRC = CNetPacketBase::CalculateCRC32(pBuffer, nDataLen);

    ((MESSAGE_HEADER*)pBuffer)->nChecksum = htonl(nCRC);  //◊™ªØŒ™Õ¯¬Á◊÷Ω⁄–Ú

    //ATLTRACE(_T("ID = %u,Len = %u,CheckSum = %u\r\n")
    //    ,GetPacketId()
    //    ,nDataLen
    //    ,((MESSAGE_HEADER*)pBuffer)->nChecksum);
    return nCRC ;
}

//------------------------------------------------------------------------------------
// ∫Ø ˝√˚≥∆£∫VerifyCRC32
/// @brief —È÷§ ˝æ›∞¸µƒCRC32÷µ «∑Ò’˝»∑,◊¢“‚À˘”–µƒ ˝æ› «Õ¯¬Á◊÷Ω⁄–Ú
///
/// @return true->’˝»∑,false->¥ÌŒÛ
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
bool CDataPacket::VerifyCRC32()
{
    const BYTE* pBuffer = (BYTE*)m_objDataBuffer.GetDataHeadPtr();
    const UINT nDataLen = m_objDataBuffer.GetDataLen();

    if (nDataLen > PACKET_LEN_MAX)
    {
        return false;
    }

    const UINT nOldCRC = ((MESSAGE_HEADER*)pBuffer)->nChecksum;
    ((MESSAGE_HEADER*)pBuffer)->nChecksum = 0;
    unsigned int nCRC = htonl(CNetPacketBase::CalculateCRC32(pBuffer, nDataLen));
    ((MESSAGE_HEADER*)pBuffer)->nChecksum = nOldCRC;

    assert(nOldCRC == nCRC);
   /* if (nOldCRC != nCRC)
    {
        LOG_ERROR("CRC32–£—È ß∞‹old:%0X, clac:%0X", nOldCRC, nCRC);
    }*/
    return (nOldCRC == nCRC);
}

//------------------------------------------------------------------------------------
// ∫Ø ˝√˚≥∆£∫Encipher
/// @brief º”√‹ ˝æ›∞¸
///
/// ”√÷∏∂®µƒº”√‹∂‘œÛº”√‹µ±«∞ª∫≥Â«¯÷–µƒ ˝æ›£¨º”√‹∫Ûµƒ ˝æ›\n
/// ”–ø…ƒ‹¥Û”⁄ µº µƒ ˝æ›£¨“ÚŒ™Œ“√« «∞¥øÈº”√‹µƒ£¨À˘“‘ª˙√‹∫Û\n
/// –Ë“™µ˜’˚∞¸Õ∑µƒ≥§∂»∫Õª∫≥Â«¯÷–µƒ≥§∂»
/// @param _in struct aes_ctx& refCAES->AESº”√‹∂‘œÛ
/// @note ∏√∫Ø ˝‘⁄∑¢ÀÕ÷Æ«∞µ˜”√£¨ƒ« ±À˘”–µƒ ˝æ›∂º◊™ªªŒ™¡ÀÕ¯¬Á◊÷Ω⁄–Ú
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
void CDataPacket::Encipher(struct aes_ctx& refCAES)
{
    BYTE* pBuffer               = (BYTE*)m_objDataBuffer.GetDataHeadPtr();
    MESSAGE_HEADER* pMsgHeader  = (MESSAGE_HEADER*)pBuffer;
    const UINT nDataLen         = htonl(pMsgHeader->nPayloadLen);
    const WORD nEncipherDataLen = WORD(CalcAlign(nDataLen, ENCRYPTED_DATA_BLOCK_LENGTH));    ///< º∆À„º”√‹∫Û ˝æ›µƒ≥§∂»

    pMsgHeader->nPayloadLen = htonl(nEncipherDataLen);
    pMsgHeader->nPadding = (BYTE)(nEncipherDataLen - nDataLen);
    m_objDataBuffer.FillPadding(pMsgHeader->nPadding, 0);


    //œ¬√Êµƒ¥˙¬Î≤ªƒ‹»•≥˝£¨“ÚŒ™FillPadding”–ø…ƒ‹÷ÿ–¬∑÷≈‰ª∫≥Â«¯
    pBuffer                     = (BYTE*)m_objDataBuffer.GetDataHeadPtr();

    aes_aes(&refCAES, &pBuffer[sizeof(MESSAGE_HEADER)],nEncipherDataLen , &pBuffer[sizeof(MESSAGE_HEADER)], AES_ENCRYPT);
    //refCAES.Encipher(&pBuffer[sizeof(MESSAGE_HEADER) - sizeof(DATA_HEADER)],nEncipherDataLen);
}

//------------------------------------------------------------------------------------
// ∫Ø ˝√˚≥∆£∫Decipherer
/// @brief ”√÷∏∂®µƒΩ‚√‹∂‘œÛΩ‚√‹µ±«∞ª∫≥Â«¯µƒ ˝æ›£¨≤¢«“µ˜’˚∞¸Õ∑µƒ ˝æ›≥§∂»
///
/// @param _in struct aes_ctx& refCAES>AESº”√‹∂‘œÛ
/// @note ∏√∫Ø ˝‘⁄Ω” ‹ ˝æ›±®∫ÛæÕ¡¢º¥µ˜”√
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
bool CDataPacket::Decipherer(struct aes_ctx& refCAES)
{
    bool bResult = false;

    if (refCAES.key_length > 0)
    {
        BYTE* pBuffer  = (BYTE*)m_objDataBuffer.GetDataHeadPtr();
        MESSAGE_HEADER* pMsgHeader  = (MESSAGE_HEADER*)pBuffer;
        const UINT nDataLen = htonl(pMsgHeader->nPayloadLen);

        //refCAES.Decipherer(&pBuffer[sizeof(MESSAGE_HEADER) - sizeof(DATA_HEADER)],nDataLen);
        aes_aes(&refCAES, &pBuffer[sizeof(MESSAGE_HEADER)], nDataLen, &pBuffer[sizeof(MESSAGE_HEADER)], AES_DECRYPT);

        //œ¬√Êµƒ¡Ω––¥˙¬Î «∏ƒ±‰”––ß ˝æ›µƒ≥§∂»,“ÚŒ™Ω‚√‹ª·∏ƒ±‰’˚∏ˆ”––ß ˝æ›µƒ≥§∂»
        const DWORD nRawDataLen = htonl(pMsgHeader->nPayloadLen) - pMsgHeader->nPadding;

        m_objDataBuffer.SetDataLen(nRawDataLen + sizeof(MESSAGE_HEADER));
        pMsgHeader->nPayloadLen = htonl(nRawDataLen);
        pMsgHeader->nPadding = 0;      ///< Ω‚√‹∫Ûª·»•µÙÃÓ≥‰ ˝æ›£¨“Ú¥ÀÃÓ≥‰ ˝æ›”¶∏√…ËŒ™0
        bResult = true;
      
    }

    return bResult;
}

//------------------------------------------------------------------------------------
// ∫Ø ˝√˚≥∆£∫IsValid
/// @brief ≈–∂œµ±«∞µƒ ˝æ›∞¸ «∑Ò”––ß(∏√∫Ø ˝±ÿ–Î‘⁄Ω‚√‹∫Ûµ˜”√)
///
/// @return true->”––ß,false->Œﬁ–ß
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
bool CDataPacket::IsValid()
{
    //BYTE* pBuffer  = (BYTE*)m_objDataBuffer.GetDataHeadPtr();
    //MESSAGE_HEADER* pMsgHeader  = (MESSAGE_HEADER*)pBuffer;

    return true;
}

//------------------------------------------------------------------------------------
// ∫Ø ˝√˚≥∆£∫ExtractData
/// @brief ¥”ª∫≥Â«¯÷–Ã·»° ˝æ›∞¸(±ªÃ·»°µƒ ˝æ›“ª∂® «¥Û”⁄∞¸Õ∑µƒ≥§∂»µƒ£¨∑Ò‘Ú≥ˆ¥Ì)
///
/// @param _in CDataStreamBuffer& refDataBuffer->“ª∏ˆ÷∏œÚª∫≥Â«¯µƒ÷∏’Î
/// @return ≥…π¶∑µªÿÃ·»° ˝æ›∞¸µƒ∏ˆ ˝
/// @author ª∆¿ˆ‘∆
//------------------------------------------------------------------------------------
NET_EXTRACT_DATA_PACKET_RESULT CDataPacket::ExtractData(CDataStreamBuffer& refDataBuffer)
{
    NET_EXTRACT_DATA_PACKET_RESULT nResult = NEDPR_INVALID;
    const BYTE* pData         = (BYTE*)refDataBuffer.GetDataHeadPtr();
    const UINT   DATA_LENGTH   = refDataBuffer.GetDataLen();

    for (UINT i = 0; i < DATA_LENGTH; i++)
    {
        // £”‡ ˝æ›≥§∂»
        const int nRemainDateLen = int(DATA_LENGTH - i);

        if (nRemainDateLen < sizeof(MESSAGE_HEADER))
        {
            nResult = NEDPR_INCOMPLETE_PACKET;
            break;
        }

        const MESSAGE_HEADER* pMsgHeader = reinterpret_cast<const MESSAGE_HEADER*>(&pData[i]);
       
        //’“µΩ±®Õ∑Ãÿ’˜¬Î
        assert(HEADER_MAGIC == ntohs(pMsgHeader->nMagic));

        if (HEADER_MAGIC == ntohs(pMsgHeader->nMagic))
        {
            assert(((pMsgHeader->nFlags) & FLAG_VERSION) == BYTE(VERSION_ONE));

            if (((pMsgHeader->nFlags) & FLAG_VERSION) == BYTE(VERSION_ONE))
            {
                //∏√ ˝æ›±®≥§∂»
                const int PACKET_LENGTH = htonl(pMsgHeader->nPayloadLen) + MESSAGE_HEADER_LEN;

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
            }
        }

        refDataBuffer.MoveReadingPos(1);
    }

    return nResult;
}

int CDataPacket::GetPacketLen()
{
    if (!m_objDataBuffer.IsValidity())
    {
        return 0;
    }
    
    MESSAGE_HEADER* pMsgHead = (MESSAGE_HEADER*)m_objDataBuffer.GetDataHeadPtr();

    return htonl(pMsgHead->nPayloadLen) + MESSAGE_HEADER_LEN;
}

int CDataPacket::GetPacketType()
{
    if (!m_objDataBuffer.IsValidity())
    {
        return -1;
    }
    MESSAGE_HEADER* pMsgHead = (MESSAGE_HEADER*)m_objDataBuffer.GetDataHeadPtr();
    return ntohs(pMsgHead->nType);
}

int CDataPacket::GetPacketId()
{
    if (!m_objDataBuffer.IsValidity())
    {
        return -1;
    }
    MESSAGE_HEADER* pMsgHead = (MESSAGE_HEADER*)m_objDataBuffer.GetDataHeadPtr();
    return NTOHL(pMsgHead->nID);
}

int CDataPacket::GetPacketFlag()
{
    if (!m_objDataBuffer.IsValidity())
    {
        return -1;
    }
    MESSAGE_HEADER* pMsgHead = (MESSAGE_HEADER*)m_objDataBuffer.GetDataHeadPtr();
    return pMsgHead->nFlags;
}

int CDataPacket::GetPacketVer()
{
    if (!m_objDataBuffer.IsValidity())
    {
        return -1;
    }
    MESSAGE_HEADER* pMsgHead = (MESSAGE_HEADER*)m_objDataBuffer.GetDataHeadPtr();
    return pMsgHead->nFlags&FLAG_VERSION;
}

BYTE* CDataPacket::GetBuffer()
{
    return (BYTE*)m_objDataBuffer.GetDataHeadPtr();
}

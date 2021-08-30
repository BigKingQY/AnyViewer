///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  DataPacket.cpp
/// @brief ��ˮǽ̽ͷ����ʹ�õ����ݰ�
/// 
///
/// @author ������
/// @version 1.0
/// @date        2016-9-1
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include <exception>
#include "VNCPacket.h"
#include "log.h"



/*--------------------------------------------CVNCPacket---------------------------------------------------*/
//------------------------------------------------------------------------------------
// CVNCPacket::CVNCPacket
/// @brief ���캯��
///
/// �ð湹�캯��������ʼ��CRC32У��ͼ����
///
/// @author ������
//------------------------------------------------------------------------------------
CVNCPacket::CVNCPacket()
{

}
//------------------------------------------------------------------------------------
// CVNCPacket::CVNCPacket
/// @brief ���캯��
///
/// �ð湹�캯�����Գ�ʼ���ݻ������Ĵ�С,�ҳ�ʼ��CRC32У��ͼ����
/// @param nSize ���ݻ������ĳ�ʼֵ
///
/// @author ������
//------------------------------------------------------------------------------------
CVNCPacket::CVNCPacket(INT nSize)
	: m_objDataBuffer(nSize)
{
	
}

//------------------------------------------------------------------------------------
// CVNCPacket::CVNCPacket
/// @brief ���캯��
///
/// �ð湹�캯����ָ�������ݹ��쵱ǰ�����ݰ�,�ҳ�ʼ��CRC32У��ͼ����
/// @param pBuffer һ��ָ��Դ���ݵ��ڴ�ָ��
/// @param nLen Դ���ݵĳ���
///
/// @author ������
//------------------------------------------------------------------------------------
CVNCPacket::CVNCPacket(const U8* pBuffer, INT nLen)
	: m_objDataBuffer(pBuffer, nLen)
{
}

//------------------------------------------------------------------------------------
// CVNCPacket::CVNCPacket
/// @brief �������캯��
///
/// �ð湹�캯�����û����������������ݰ����������쵱ǰ�����ݰ�
/// @param refDataPacket һ��ָ��Դ���ݰ�����Ĳο�
///
/// @author ������
//------------------------------------------------------------------------------------
CVNCPacket::CVNCPacket(const CVNCPacket& refDataPacket)
{
	m_objDataBuffer = refDataPacket.m_objDataBuffer;
}

//------------------------------------------------------------------------------------
// CVNCPacket::operator=
/// @brief ��ֵ���غ���
///
/// ͨ���ú������ǿ��԰�һ�����ݰ�����ֵ������һ�����ݰ�����
/// @param refDataPacket һ��ָ��Դ���ݰ�����Ĳο�
///
/// @author ������
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
/// д�����ݵ�
/// </summary>
/// <param name="pData">ָ�����ݵ�ָ��</param>
/// <param name="nLen">��д���ݰ��ĳ���</param>
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
/// �������ݰ��ĳ��ȣ�ͬʱ�����غ����ݵĳ���
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
/// �����غ����ݵĳ���
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
/// ���û������еĶ�ƫ��
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
/// �����ݻ������з���һ�����ݰ�
/// </summary>
/// <param name="refDataBuffer">���ݻ�����</param>
/// <returns>������ȡ�Ľ��</returns>
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
		//ʣ�����ݳ���
		const int nRemainDateLen = int(DATA_LENGTH - i);

		if (nRemainDateLen < sizeof(VNCP::VNC_MESSAGE_HEADER))
		{
			nResult = NEDPR_INCOMPLETE_PACKET;
			break;
		}

		const VNCP::VNC_MESSAGE_HEADER* pMsgHeader = reinterpret_cast<const VNCP::VNC_MESSAGE_HEADER*>(&pData[i]);

		//�ҵ���ͷ������
        if (VNCP::HEADER_MAGIC == ntohs(pMsgHeader->nMagic))
		{
			//�����ݱ�����
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

			// break����Ҫ��һ��ֻ��ȡһ�����ݱ�
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
/// @brief ��յ�ǰ���ݰ������������Ϣ
///
/// @author ������
//------------------------------------------------------------------------------------
void CVNCPacket::ResetContent()
{
	m_objDataBuffer.Empty();
}

//------------------------------------------------------------------------------------
// CVNCPacket::GetHeaderPtr
/// @brief �õ����ݰ����ڴ�����ָ��
///
/// @return  ����һ��ָ�򻺳�������Ĳο�ָ��
/// @author ������
//------------------------------------------------------------------------------------
U8* CVNCPacket::GetHeaderPtr()
{
	return m_objDataBuffer.GetBufferHeadPtr();
}

//------------------------------------------------------------------------------------
// CVNCPacket::GetTotalLength
/// @brief �õ����ݰ�����Ч���ݳ���
///
/// @return  ���ص�ǰ���ݰ�����Ч���ݳ���
/// @author ������
//------------------------------------------------------------------------------------
INT CVNCPacket::GetTotalLength()
{
	return m_objDataBuffer.GetDataLen();
}

// ********************************************************************************
/// <summary>
/// �õ���Ϣ����
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
/// �õ��ỰID
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
/// �õ��غ����ݵĳ���
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
/// @brief ��������
///
/// ͨ���ú������ǿ��԰�һ�����ݰ�����ֵ������һ�����ݰ�����
/// @param refDataPacket һ��ָ��Դ���ݰ�����Ĳο�
///
/// @author ������
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
/// ��ʼ�����ݰ�
/// </summary>
/// <param name="nType">����</param>
/// <param name="nSessionID">�Ự</param>
/// <param name="nFlag"></param>
/// <param name="nVarPayloadLen">�ɱ����ݳ���</param>
/// <returns>true:�ɹ�,false:ʧ��</returns>
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

//		pMsgHeader->nMagic = HTONS((U16)VNCP::HEADER_MAGIC);
//		pMsgHeader->nType = nType;
//		pMsgHeader->nSessionID = HTONL(nSessionID);
		pMsgHeader->nFlag = nFlag;
		pMsgHeader->nPayloadLen = HTON3B(nVarPayloadLen);

		m_objDataBuffer.IncreaseDataLen(sizeof(VNCP::VNC_MESSAGE_HEADER) + nVarPayloadLen);   //�ô��벻����
	}

	return bResult;
}

// ********************************************************************************
/// <summary>
/// �õ���־
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

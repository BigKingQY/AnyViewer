///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  DataPacket.cpp
/// @brief 防水墙探头服务使用的数据包
/// 
///
/// @author 黄丽云
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
/// @brief 构造函数
///
/// 该版构造函数仅仅初始化CRC32校验和计算表
///
/// @author 黄丽云
//------------------------------------------------------------------------------------
CVNCPacket::CVNCPacket()
{

}
//------------------------------------------------------------------------------------
// CVNCPacket::CVNCPacket
/// @brief 构造函数
///
/// 该版构造函数可以初始数据缓冲区的大小,且初始化CRC32校验和计算表
/// @param nSize 数据缓冲区的初始值
///
/// @author 黄丽云
//------------------------------------------------------------------------------------
CVNCPacket::CVNCPacket(INT nSize)
	: m_objDataBuffer(nSize)
{
	
}

//------------------------------------------------------------------------------------
// CVNCPacket::CVNCPacket
/// @brief 构造函数
///
/// 该版构造函数用指定的数据构造当前的数据包,且初始化CRC32校验和计算表
/// @param pBuffer 一个指向源数据的内存指针
/// @param nLen 源数据的长度
///
/// @author 黄丽云
//------------------------------------------------------------------------------------
CVNCPacket::CVNCPacket(const U8* pBuffer, INT nLen)
	: m_objDataBuffer(pBuffer, nLen)
{
}

//------------------------------------------------------------------------------------
// CVNCPacket::CVNCPacket
/// @brief 拷贝构造函数
///
/// 该版构造函数让用户可以用其他的数据包对象来构造当前的数据包
/// @param refDataPacket 一个指向源数据包对象的参考
///
/// @author 黄丽云
//------------------------------------------------------------------------------------
CVNCPacket::CVNCPacket(const CVNCPacket& refDataPacket)
{
	m_objDataBuffer = refDataPacket.m_objDataBuffer;
}

//------------------------------------------------------------------------------------
// CVNCPacket::operator=
/// @brief 赋值重载函数
///
/// 通过该函数我们可以把一个数据包对象赋值给另外一个数据包对象
/// @param refDataPacket 一个指向源数据包对象的参考
///
/// @author 黄丽云
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
/// 写入数据到
/// </summary>
/// <param name="pData">指向数据的指针</param>
/// <param name="nLen">被写数据包的长度</param>
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
/// 设置数据包的长度，同时计算载荷数据的长度
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
/// 计算载荷数据的长度
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
/// 设置缓冲区中的读偏移
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
/// 从数据缓冲区中分离一个数据包
/// </summary>
/// <param name="refDataBuffer">数据缓冲区</param>
/// <returns>返回提取的结果</returns>
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
		//剩余数据长度
		const int nRemainDateLen = int(DATA_LENGTH - i);

		if (nRemainDateLen < sizeof(VNCP::VNC_MESSAGE_HEADER))
		{
			nResult = NEDPR_INCOMPLETE_PACKET;
			break;
		}

		const VNCP::VNC_MESSAGE_HEADER* pMsgHeader = reinterpret_cast<const VNCP::VNC_MESSAGE_HEADER*>(&pData[i]);

		//找到报头特征码
        if (VNCP::HEADER_MAGIC == ntohs(pMsgHeader->nMagic))
		{
			//该数据报长度
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

			// break很重要，一次只提取一个数据报
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
/// @brief 清空当前数据包对象的数据信息
///
/// @author 黄丽云
//------------------------------------------------------------------------------------
void CVNCPacket::ResetContent()
{
	m_objDataBuffer.Empty();
}

//------------------------------------------------------------------------------------
// CVNCPacket::GetHeaderPtr
/// @brief 得到数据包的内存数据指针
///
/// @return  返回一个指向缓冲区对象的参考指针
/// @author 黄丽云
//------------------------------------------------------------------------------------
U8* CVNCPacket::GetHeaderPtr()
{
	return m_objDataBuffer.GetBufferHeadPtr();
}

//------------------------------------------------------------------------------------
// CVNCPacket::GetTotalLength
/// @brief 得到数据包的有效数据长度
///
/// @return  返回当前数据包的有效数据长度
/// @author 黄丽云
//------------------------------------------------------------------------------------
INT CVNCPacket::GetTotalLength()
{
	return m_objDataBuffer.GetDataLen();
}

// ********************************************************************************
/// <summary>
/// 得到消息类型
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
/// 得到会话ID
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
/// 得到载荷数据的长度
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
/// @brief 拷贝数据
///
/// 通过该函数我们可以把一个数据包对象赋值给另外一个数据包对象
/// @param refDataPacket 一个指向源数据包对象的参考
///
/// @author 黄丽云
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
/// 初始化数据包
/// </summary>
/// <param name="nType">类型</param>
/// <param name="nSessionID">会话</param>
/// <param name="nFlag"></param>
/// <param name="nVarPayloadLen">可变数据长度</param>
/// <returns>true:成功,false:失败</returns>
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

		m_objDataBuffer.IncreaseDataLen(sizeof(VNCP::VNC_MESSAGE_HEADER) + nVarPayloadLen);   //该代码不能少
	}

	return bResult;
}

// ********************************************************************************
/// <summary>
/// 得到标志
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

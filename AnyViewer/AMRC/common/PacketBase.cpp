/////////////////////////////////////////////////////////////////////
/// @file CHTTPPacket.h
/// @brief http数据包定义
/// 
/// @author andy
/// @version 1.0
/// @date 2020.4.1
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PacketBase.h"

UINT          CNetPacketBase::s_szCRC32Table[CRC32_TABLE_SIZE_MAX] = { 0 };
bool         CNetPacketBase::s_bCRC32TableInitialized = false;


/*--------------------------------------------CNetPacketBase---------------------------------------------------*/
//------------------------------------------------------------------------------------
// CNetPacketBase::CNetPacketBase
/// @brief 构造函数
///
/// 该版构造函数仅仅初始化CRC32校验和计算表
///
/// @author 黄丽云
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
/// @brief 构造函数
///
/// 该版构造函数可以初始数据缓冲区的大小,且初始化CRC32校验和计算表
/// @param nSize 数据缓冲区的初始值
///
/// @author 黄丽云
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
/// @brief 构造函数
///
/// 该版构造函数用指定的数据构造当前的数据包,且初始化CRC32校验和计算表
/// @param pBuffer 一个指向源数据的内存指针
/// @param nLen 源数据的长度
///
/// @author 黄丽云
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
/// @brief 拷贝构造函数
///
/// 该版构造函数让用户可以用其他的数据包对象来构造当前的数据包
/// @param refDataPacket 一个指向源数据包对象的参考
///
/// @author 黄丽云
//------------------------------------------------------------------------------------
CNetPacketBase::CNetPacketBase(const CNetPacketBase& refDataPacket)
{
	m_objDataBuffer = refDataPacket.m_objDataBuffer;
}

//------------------------------------------------------------------------------------
// CNetPacketBase::operator=
/// @brief 赋值重载函数
///
/// 通过该函数我们可以把一个数据包对象赋值给另外一个数据包对象
/// @param refDataPacket 一个指向源数据包对象的参考
///
/// @author 黄丽云
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
/// @brief 拷贝数据
///
/// 通过该函数我们可以把一个数据包对象赋值给另外一个数据包对象
/// @param refDataPacket 一个指向源数据包对象的参考
///
/// @author 黄丽云
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
/// @brief 清空当前数据包对象的数据信息
///
/// @author 黄丽云
//------------------------------------------------------------------------------------
void CNetPacketBase::ResetContent()
{
	m_objDataBuffer.Empty();
}

//------------------------------------------------------------------------------------
// CNetPacketBase::GetHeaderPtr
/// @brief 得到数据包的内存数据指针
///
/// @return  返回一个指向缓冲区对象的参考指针
/// @author 黄丽云
//------------------------------------------------------------------------------------
U8* CNetPacketBase::GetHeaderPtr()
{
	return m_objDataBuffer.GetBufferHeadPtr();
}

//------------------------------------------------------------------------------------
// CNetPacketBase::GetTotalLength
/// @brief 得到数据包的有效数据长度
///
/// @return  返回当前数据包的有效数据长度
/// @author 黄丽云
//------------------------------------------------------------------------------------
INT CNetPacketBase::GetTotalLength()
{
	return m_objDataBuffer.GetDataLen();
}

//------------------------------------------------------------------------------------
// CNetPacketBase::InitCRC32Table
/// @brief 初始化crc表,生成32位大小的crc表
///
/// @author 黄丽云
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
/// @brief 计算数据包的CRC32校验值(注意：所有的数据必须是网络字节序)
///
/// @param pData 一个指向被加密数据的指针
/// @param nLength 被加密数据的长度
/// @return 返回数据的CRC32校验值
/// @author 黄丽云
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


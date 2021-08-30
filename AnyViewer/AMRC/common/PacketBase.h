/////////////////////////////////////////////////////////////////////
/// @file CHTTPPacket.h
/// @brief http数据包定义
/// 
/// @author andy
/// @version 1.0
/// @date 2020.4.1
/////////////////////////////////////////////////////////////////////

#pragma once

#include "BufferImpl.h"

#define CRC32_TABLE_SIZE_MAX 256

///    数据包提取结果枚举定义  
///   
///    该枚举定义了提取缓冲区数据包的结果类型\n  
enum NET_EXTRACT_DATA_PACKET_RESULT
{
	NEDPR_INVALID = 0,     ///< 枚举，无效的数据包
	NEDPR_JUST_A_PACKET,           ///< 枚举，刚好一个数据包
	NEDPR_INCOMPLETE_PACKET,   ///< 枚举，不完整的一个数据包
	NEDPR_MORE_PACKETS            ///< 枚举，更多的数据包
};

typedef CBufferImplT<BYTE>    CDataStreamBuffer;

//=============================================
/// @brief 抽象数据包接口定义
///
/// 整个通信框架中所有的数据包对象的基类 
//=============================================
class CNetPacket
{
public:
	CNetPacket() {}

	/// @brief 从数据流中提取数据包
	///
	/// @param CDataStreamBuffer 存放数据包的缓冲区对象
	/// @return  返回提取数据包后的结果,参考类型NET_EXTRACT_DATA_PACKET_RESULT
	virtual NET_EXTRACT_DATA_PACKET_RESULT ExtractData(CDataStreamBuffer&) = 0;

	/// @brief 清空数据包
	virtual void ResetContent() = 0;

	/// @brief 得到数据头指针
	///
	/// @return  返回一个指向数据包头的指针
	virtual BYTE* GetHeaderPtr() = 0;

	/// @brief 得到整个数据包的长度
	///
	/// @return  返回数据包有效长度
	virtual int GetTotalLength() = 0;
	virtual ~CNetPacket() {}

	virtual int GetPacketId() = 0; // 获取报文ID、本机字节序
public:
	/// @brief 重载函数BYTE*( )
	///
	/// @return  返回一个指向数据包头的指针
	operator BYTE*() { return GetHeaderPtr(); }
};

//=============================================
/// @brief 通用的数据包基类
///
/// 通用的数据包基类,拥有一个流缓冲区成员，用于存储数据包数据
//=============================================
class CNetPacketBase : public CNetPacket
{
public:

	/// @brief 构造函数
	CNetPacketBase();

	/// @brief 构造函数
	CNetPacketBase(int nSize);

	/// @brief 构造函数
	CNetPacketBase(const BYTE* pBuffer, int nLen);

	/// @brief 析构函数
	virtual ~CNetPacketBase();

	/// @brief 拷贝构造函数
	CNetPacketBase(const CNetPacketBase& refDataPacket);

	/// @brief 赋值函数
	CNetPacketBase& operator=(const CNetPacketBase& refDataPacket);
public:

	/// @brief 得到缓冲区对象
	///
	/// @return  返回一个指向缓冲区对象的参考指针
	CDataStreamBuffer& GetPacketBuffer() { return m_objDataBuffer; }

public:
	/// @brief 关联当前的数据包到指定的数据
	///
	/// 该操作不会引发数据拷贝操作,一旦关联成功，我们将不能修改数据包数据了
	/// @param pData 返一个指向目标数据的指针
	/// @param  nLen 目标数据的长度
	void Attach(BYTE* pData, int nLen) { m_objDataBuffer.Attach(pData, nLen); m_objDataBuffer.IncreaseDataLen(nLen); }

	/// @brief 取消关联
	void Dettach() { m_objDataBuffer.Dettach(); }
public:
	virtual NET_EXTRACT_DATA_PACKET_RESULT ExtractData(CDataStreamBuffer&) { return NEDPR_INVALID; }        //从数据流中提取数据包
	virtual void ResetContent();                                                                       //清空数据包
	virtual BYTE* GetHeaderPtr();                                                                    //得到数据头指针
	virtual int GetTotalLength();                                                                        //得到整个数据包的长度
	virtual int Copy(const BYTE* pBuffer, int nLen);                                             //从指定的源拷贝数据

public: //下面的函数是与CRC32校验相关的

	/// @brief 初始化CRC32计算表
	static void  InitCRC32Table(void);

	/// @brief 计算指定数据的CRC32值
	static UINT CalculateCRC32(const BYTE* pData, UINT nLength);

protected:
	CDataStreamBuffer        m_objDataBuffer;                  ///< 存放数据的缓冲区对象

private:
	static bool                   s_bCRC32TableInitialized;        ///< 是否已经初始化CRC32表
	static UINT                    s_szCRC32Table[CRC32_TABLE_SIZE_MAX];           ///< 用于CRC32校验的表
};

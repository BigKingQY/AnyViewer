#pragma once
#include "Typedef.h"
#include "PacketBase.h"
//#include "ObjectPoolImpl.h"
#include "VNCProtocol.h"
#include "document.h"
#include "prettywriter.h"
#include "stringbuffer.h"
#include "filewritestream.h"

#include <iostream>


//=============================================
/// @brief 通用的数据包基类
///
/// 通用的数据包基类,拥有一个流缓冲区成员，用于存储数据包数据
//=============================================
class CVNCPacket : public CNetPacket
{
public:

	/// @brief 构造函数
	CVNCPacket();

	/// @brief 构造函数
	CVNCPacket(int nSize);

	/// @brief 构造函数
	CVNCPacket(const U8* pBuffer, int nLen);

	/// @brief 析构函数
	virtual ~CVNCPacket();

	/// @brief 拷贝构造函数
	CVNCPacket(const CVNCPacket& refDataPacket);

	/// @brief 赋值函数
	CVNCPacket& operator=(const CVNCPacket& refDataPacket);

	enum
	{
		PACKET_LEN_MAX = 100 * 1024,  ///< 最大的数据包长度
	};
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
	void Attach(U8* pData, int nLen) { m_objDataBuffer.Attach(pData, nLen); m_objDataBuffer.IncreaseDataLen(nLen); }

	/// @brief 取消关联
	void Dettach() { m_objDataBuffer.Dettach(); }

	void SetLength(const int nValidPacketLen);

	// 计算载荷数据的长度
	void CalcPayloadLen();

	// 设置缓冲区中的读偏移
	void SetReadingOffset(const unsigned int nOffset);

public:
	/// 写入数据到
	void Write(const U8* pData, const int nLen);

	void WriteUInt8(const U8 x);

	void WriteUInt16(const U16 x);

	void WriteUInt32(const U32 x);

	void WriteUInt64(const U64 x);

	/// 读出数据到
	int Read(U8* pData, const int nLen);

	U8 ReadUInt8();

	U16 ReadUInt16();

	U32 ReadUInt32();

	U64 ReadUInt64();

public:
	//从数据流中提取数据包
	virtual NET_EXTRACT_DATA_PACKET_RESULT ExtractData(CDataStreamBuffer&) override;

	//清空数据包
	virtual void ResetContent() override;

	//得到数据头指针
	virtual U8* GetHeaderPtr() override;
	
	//得到整个数据包的长度
	virtual int GetTotalLength() override;

	virtual int GetPacketId() override { return -1; } // 获取报文ID、本机字节序


public:
	// 得到消息类型
	U8 GetType();

	// 得到会话ID
	U32 GetSessionID();

	// 得到载荷数据的长度
	unsigned int GetPayloadLen() const;

	// 得到标志
	U8 GetFlag();

	// 得到载荷数据头指针
	template<class MSG_HEADER>
	U8* GetPayloadPtr();

	INT Copy(const U8* pBuffer, INT nLen);

	// 初始化数据包
	bool Initial(const U8 nType, const U32 nSessionID, const U8 nFlag /*= 0*/, const UINT nVarPayloadLen/* = 0*/);

	//初始化数据包的模板函数（推荐使用该函数）
	template <class HeadT,class BodyT>
	bool InitialT(const U8 nType, const U32 nSessionID, const U8 nFlag /*= 0*/, const UINT nVarPayloadLen/* = 0*/);

	template <class PObject = CSerializeEntry * >
	bool InitialJsonT(const U8 nType, const U32 nSessionID, const U8 nFlag, const PObject pJsonObj);

	template <class T>
	bool ExtractJsonObjT(const unsigned int nStartPos, const unsigned int nLen, T& refJsonObj);

	template <class TPacket, class TObject>
	bool ExtractJsonObjT(TObject& refJsonObj);
protected:
	CDataStreamBuffer        m_objDataBuffer;                  ///< 存放数据的缓冲区对象
};


template<class MSG_HEADER>
inline U8 * CVNCPacket::GetPayloadPtr()
{
	U8 * pResult = m_objDataBuffer.GetBufferHeadPtr();

	return pResult + sizeof(MSG_HEADER);
}

/*--------------------------------------------------------------------------
*函数名称：InitialT
*描述： 初始化数据包的模板函数（推荐使用该函数）
*参数 :  UINT nID->数据包的ID
*参数 : WORD nType->数据包的类型
*参数 : WORD nFlags->数据包的标志
*参数 : UINT nVarPayloadLen->数据包携带的可选数据
*返回值: true->成功,false->失败 (主要是分配内存失败)
*修改记录:
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

		pMsgHeader->nMagic = htons((U16)(VNCP::HEADER_MAGIC));
		pMsgHeader->nType = nType;
        pMsgHeader->nSessionID = htonl(nSessionID);
		pMsgHeader->nFlag = nFlag;
		pMsgHeader->nPayloadLen = HTON3B(nPayloadLen);

		m_objDataBuffer.IncreaseDataLen(sizeof(HeadT) + sizeof(BodyT) + nVarPayloadLen);   //该代码不能少
	}

	return bResult;
}

// --------------------------------------------------------------------------------
/// <summary>
/// 初始化数据包的模板函数（推荐使用该函数）
/// </summary>
/// <param name="nType">数据包的类型</param>
/// <param name="nSessionID">数据包的标志</param>
/// <param name="pJsonObj">指向一个有序列化json数据的对象，序列化后数据将添加到数据包后面</param>
/// <returns></returns>
/// <created>黄丽云,2019/2/27</created>
/// <changed>黄丽云,2019/2/27</changed>
template<class PObject>
inline bool CVNCPacket::InitialJsonT(const U8 nType, const U32 nSessionID, const U8 nFlag, const PObject pJsonObj)
{
	bool bResult = false;
	rapidjson::StringBuffer objBuffer;
	rapidjson::Document objJsonDoc(rapidjson::kObjectType);    //生成一个dom元素Document
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
/// 从数据包的载荷数据中提取json对象
/// </summary>
/// <param name="refJsonObj">json对象</param>
/// <returns></returns>
/// <created>黄丽云,2019/2/27</created>
/// <changed>黄丽云,2019/2/27</changed>
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
/// 从数据包的载荷数据中提取json对象
/// </summary>
/// <param name="refJsonObj">json对象</param>
/// <returns></returns>
/// <created>黄丽云,2019/2/27</created>
/// <changed>黄丽云,2019/2/27</changed>
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

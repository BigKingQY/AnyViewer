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
/// @brief ͨ�õ����ݰ�����
///
/// ͨ�õ����ݰ�����,ӵ��һ������������Ա�����ڴ洢���ݰ�����
//=============================================
class CVNCPacket : public CNetPacket
{
public:

	/// @brief ���캯��
	CVNCPacket();

	/// @brief ���캯��
	CVNCPacket(int nSize);

	/// @brief ���캯��
	CVNCPacket(const U8* pBuffer, int nLen);

	/// @brief ��������
	virtual ~CVNCPacket();

	/// @brief �������캯��
	CVNCPacket(const CVNCPacket& refDataPacket);

	/// @brief ��ֵ����
	CVNCPacket& operator=(const CVNCPacket& refDataPacket);

	enum
	{
		PACKET_LEN_MAX = 100 * 1024,  ///< �������ݰ�����
	};
public:

	/// @brief �õ�����������
	///
	/// @return  ����һ��ָ�򻺳�������Ĳο�ָ��
	CDataStreamBuffer& GetPacketBuffer() { return m_objDataBuffer; }

public:
	/// @brief ������ǰ�����ݰ���ָ��������
	///
	/// �ò��������������ݿ�������,һ�������ɹ������ǽ������޸����ݰ�������
	/// @param pData ��һ��ָ��Ŀ�����ݵ�ָ��
	/// @param  nLen Ŀ�����ݵĳ���
	void Attach(U8* pData, int nLen) { m_objDataBuffer.Attach(pData, nLen); m_objDataBuffer.IncreaseDataLen(nLen); }

	/// @brief ȡ������
	void Dettach() { m_objDataBuffer.Dettach(); }

	void SetLength(const int nValidPacketLen);

	// �����غ����ݵĳ���
	void CalcPayloadLen();

	// ���û������еĶ�ƫ��
	void SetReadingOffset(const unsigned int nOffset);

public:
	/// д�����ݵ�
	void Write(const U8* pData, const int nLen);

	void WriteUInt8(const U8 x);

	void WriteUInt16(const U16 x);

	void WriteUInt32(const U32 x);

	void WriteUInt64(const U64 x);

	/// �������ݵ�
	int Read(U8* pData, const int nLen);

	U8 ReadUInt8();

	U16 ReadUInt16();

	U32 ReadUInt32();

	U64 ReadUInt64();

public:
	//������������ȡ���ݰ�
	virtual NET_EXTRACT_DATA_PACKET_RESULT ExtractData(CDataStreamBuffer&) override;

	//������ݰ�
	virtual void ResetContent() override;

	//�õ�����ͷָ��
	virtual U8* GetHeaderPtr() override;
	
	//�õ��������ݰ��ĳ���
	virtual int GetTotalLength() override;

	virtual int GetPacketId() override { return -1; } // ��ȡ����ID�������ֽ���


public:
	// �õ���Ϣ����
	U8 GetType();

	// �õ��ỰID
	U32 GetSessionID();

	// �õ��غ����ݵĳ���
	unsigned int GetPayloadLen() const;

	// �õ���־
	U8 GetFlag();

	// �õ��غ�����ͷָ��
	template<class MSG_HEADER>
	U8* GetPayloadPtr();

	INT Copy(const U8* pBuffer, INT nLen);

	// ��ʼ�����ݰ�
	bool Initial(const U8 nType, const U32 nSessionID, const U8 nFlag /*= 0*/, const UINT nVarPayloadLen/* = 0*/);

	//��ʼ�����ݰ���ģ�庯�����Ƽ�ʹ�øú�����
	template <class HeadT,class BodyT>
	bool InitialT(const U8 nType, const U32 nSessionID, const U8 nFlag /*= 0*/, const UINT nVarPayloadLen/* = 0*/);

	template <class PObject = CSerializeEntry * >
	bool InitialJsonT(const U8 nType, const U32 nSessionID, const U8 nFlag, const PObject pJsonObj);

	template <class T>
	bool ExtractJsonObjT(const unsigned int nStartPos, const unsigned int nLen, T& refJsonObj);

	template <class TPacket, class TObject>
	bool ExtractJsonObjT(TObject& refJsonObj);
protected:
	CDataStreamBuffer        m_objDataBuffer;                  ///< ������ݵĻ���������
};


template<class MSG_HEADER>
inline U8 * CVNCPacket::GetPayloadPtr()
{
	U8 * pResult = m_objDataBuffer.GetBufferHeadPtr();

	return pResult + sizeof(MSG_HEADER);
}

/*--------------------------------------------------------------------------
*�������ƣ�InitialT
*������ ��ʼ�����ݰ���ģ�庯�����Ƽ�ʹ�øú�����
*���� :  UINT nID->���ݰ���ID
*���� : WORD nType->���ݰ�������
*���� : WORD nFlags->���ݰ��ı�־
*���� : UINT nVarPayloadLen->���ݰ�Я���Ŀ�ѡ����
*����ֵ: true->�ɹ�,false->ʧ�� (��Ҫ�Ƿ����ڴ�ʧ��)
*�޸ļ�¼:
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

		m_objDataBuffer.IncreaseDataLen(sizeof(HeadT) + sizeof(BodyT) + nVarPayloadLen);   //�ô��벻����
	}

	return bResult;
}

// --------------------------------------------------------------------------------
/// <summary>
/// ��ʼ�����ݰ���ģ�庯�����Ƽ�ʹ�øú�����
/// </summary>
/// <param name="nType">���ݰ�������</param>
/// <param name="nSessionID">���ݰ��ı�־</param>
/// <param name="pJsonObj">ָ��һ�������л�json���ݵĶ������л������ݽ���ӵ����ݰ�����</param>
/// <returns></returns>
/// <created>������,2019/2/27</created>
/// <changed>������,2019/2/27</changed>
template<class PObject>
inline bool CVNCPacket::InitialJsonT(const U8 nType, const U32 nSessionID, const U8 nFlag, const PObject pJsonObj)
{
	bool bResult = false;
	rapidjson::StringBuffer objBuffer;
	rapidjson::Document objJsonDoc(rapidjson::kObjectType);    //����һ��domԪ��Document
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
/// �����ݰ����غ���������ȡjson����
/// </summary>
/// <param name="refJsonObj">json����</param>
/// <returns></returns>
/// <created>������,2019/2/27</created>
/// <changed>������,2019/2/27</changed>
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
/// �����ݰ����غ���������ȡjson����
/// </summary>
/// <param name="refJsonObj">json����</param>
/// <returns></returns>
/// <created>������,2019/2/27</created>
/// <changed>������,2019/2/27</changed>
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

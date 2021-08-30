///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  
/// @brief ��ˮǽ̽ͷ����
/// 
/// ��ˮǽ̽ͷ�������֮��ͨ������Ҫ����Ϣ���Ͷ���.���е���Ϣ������Ϣͷ����Ϣ\n
/// ������������ɣ���Ϣͷ�Ľṹ������ͬ�ģ�������Ϣ���ݸ�����Ϣ���͵Ĳ�ͬ��\n
/// ��ͬ.����Э������&Ӧ���ģʽ��
///
/// @author ������
/// @version 1.0
/// @date        2016-9-1
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifdef WIN32
#pragma warning(disable : 4200) 
#endif

#include "TypeDefine.h"

#define ACCOUNT_LENGTH_MAX 32
#define PASSWORD_LENGTH_MAX 32
#define MD5_LEN_MAX 32

/// ��������ö��
enum OPERATOR_TYPE
{
	OT_REQUEST = 0,            ///< ö�٣��������
	OT_RESPONSE = 1,	        ///< ö�٣�Ӧ�����
};

/// �������ö��
enum OPERATOR_RESULT
{
	OR_FAILURE = 0, ///< ö�٣���ǰ����ʧ��
	OR_SUCCESS = 2  ///< ö�٣���ǰ�����ɹ�
};

/// �㲥����ö��
enum BROADCAST_TYPE
{
	BT_SINGLECAST = 0,    ///< ö�٣�����
	BT_BROADCAST = 4      ///< ö�٣��㲥
};


/// ͨ��Э��ö��
/// 
/// �������ͨ�ŵ�Э���б�
enum AX3SOFT_PROTOCOL_VERSION
{
	VERSION_ONE = 0X10,
	VERSION_MAX,
};

/// <summary></summary>
enum STREAM_MSG
{
	MT_TRANSFER_STREAM = 4096,	                ///< ö�٣�����������
	MT_TRANSFER_STREAM_DATA,	        ///< ö�٣���������������
	MT_SYN_STREAM,	                    ///< ö�٣�ͬ��������
	MT_ABORT_TRANSFER_STREAM,          ///< ö�٣���ֹ���������ݴ���
	MT_MOVE_STREAM_OFFSERT,            ///< ö�٣��ƶ���������������ƫ��
};


/// ��������ö��
/// 
/// �����ǰ�Ĵ���λ���ش������ӽ��Ͽ�
enum STREAM_EXCEPTION_STATUS
{
	ES_NOT_FOUND_TRANS = 4096,      ///< ö�٣�û���ҵ�ָ��������
	ES_RETRANSFER,      ///< ö�٣���������ʧ��
	ES_ALLOCATE_TRANS_UNSUCCESSFULLY,      ///< ö�٣���������������ʧ��
};

/// ͨ��Э���ʶö��
/// 
enum MAGIC_FLAG
{
	HEADER_MAGIC = 0XFFFE, ///< ö�٣� ����ʶ��ͨ�����ݰ�
};

// ���е���Ϣ������ͬ����Ϣͷ,��������,������Ϣͷ����4��
// 1: ��ͷʶ����
// 2: ��ϢΨһ��ID
// 3: ��Ϣ����
// 4: ��־����һλ�ǲ������Ͷ���(0:����1����Ӧ),�ڶ�λ�����������(0:ʧ�ܣ�1���ɹ�),������λ����
// 5: ��Ϣ��Я�������ݰ�����
// 6: У���.
// 7: �غ�����ͷ����Ҫ����У�飩
// ----------------------------
// |                          |
// |                          |
// | ��Ϣͷ                |
// -----------------------------
// |                          |
// | (��ѡ����)           |
// ----------------------------


// ------------------------��Ϣͷ���岿��---------------------------------------------

#pragma pack(1)

// --------------------------------------------------------------------------------
/// <summary>
/// @brief ��Ϣͷ
/// ��ͨ����ÿ�����ݰ�����Я��һ����Ϣͷ
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _message_header
{
	U16                  nMagic;                  ///< ��ͷʶ����
	U32                  nID;                     ///< ��ϢID
	U16                  nType;                   ///< ��Ϣ����MESSAGE_TYPE
	U8                   nFlags;                  ///< ��15 -12λ��ʾ�汾����һλ��ʾ�������͡��ڶ�λ��ʾ�������������λ��ʾ�Ƿ��ǹ㲥��
	U32                  nPayloadLen;             ///< Я�������ݳ���
	U8                   nPadding;                ///< �����ֽ���
	U32                  nChecksum;               ///< У��ͣ�������TCP��ͬ���㷽������Ҫ��������ݰ�����ͷ���غ�����
}MESSAGE_HEADER;


/// @brief ��ѯ��¼״̬����
typedef struct _common_request
{
	MESSAGE_HEADER objMsgHeader;
	U64                     nStatusCode;   ///< ������Ӧ���״̬��
}COMMON_REQUEST;

/// @brief ��¼��Ӧ
typedef struct  _common_response
{
	MESSAGE_HEADER objMsgHeader;
	U64                     nStatusCode;   ///< ������Ӧ���״̬��
}COMMON_RESPONSE;


/// @brief ������
typedef struct _common_heartbeat
{
	MESSAGE_HEADER objMsgHeader;
	U64                     nStatusCode;   ///< ������Ӧ���״̬��
}COMMON_HEARTBEAT;

// --------------------------------------------------------------------------------
/// <summary>
/// ��������
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _transfer_stream_request
{
	MESSAGE_HEADER objMsgHeader;
	U16 nType;                   ///< �������� @see STREAM_DATA_TYPE
	U64 nId;                   ///< ����id
	U32 nSrcAccount;           ///<Դ�˺�
	U32 nDstAccount;           ///<Ŀ���˺�
	I8 szMd5[MD5_LEN_MAX + 1];              ///< md5
	U64 nSize;                 ///< ��С
	I8 bQuickMode;
}TRANSFER_STREAM_REQUEST;


// --------------------------------------------------------------------------------
/// <summary>
/// ͬ������������
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _syn_stream_request
{
	MESSAGE_HEADER objMsgHeader;
	U16 nType;                   ///< �������� @see STREAM_DATA_TYPE
	U64 nId;                   ///< Դ����id
	U32 nSrcAccount;            ///<Դ�˺�
	U32 nDstAccount;            ///<Ŀ���˺�
	I8 szMd5[MD5_LEN_MAX + 1];              ///< md5
	U64 nSize;                 ///< ��С
}SYN_STREAM_REQUEST;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// ��Ӧ�����ݰ�
/// </summary>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct  _transfer_stream_response
{
	MESSAGE_HEADER objMsgHeader;
	U64 nId;       ///< ��ԴID
	U64 nStatusCode;     ///< ������Ӧ���״̬��
}TRANSFER_STREAM_RESPONSE;

// --------------------------------------------------------------------------------
/// <summary>
/// ��ֹ��������������
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _abort_transfer_stream_request
{
	MESSAGE_HEADER objMsgHeader;
	U64 nId;                   ///< ����id
}ABORT_TRANSFER_STREAM_REQUEST;

// --------------------------------------------------------------------------------
/// <summary>
/// ����ʵ��
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _transfer_stream_data_request
{
	MESSAGE_HEADER objMsgHeader;
	U64 nId;                   ///< ����id
	U32 nSize;                 ///< ��С
	U64 nOffset;               ///< ƫ��
}TRANSFER_STREAM_DATA_REQUEST;



/// @brief ����������Ӧ�����ݰ�
typedef struct  _transfer_stream_data_response
{
	MESSAGE_HEADER objMsgHeader;
	U64 nId;             ///< ����id
	U64 nOffset;         ///< ��һ��ƫ��
	U32 nStatusCode;     ///< ������Ӧ���״̬��
}TRANSFER_STREAM_DATA_RESPONSE;


// --------------------------------------------------------------------------------
/// <summary>
/// �ƶ���������ƫ��,���ڶϵ�����
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _move_stream_offsert_request
{
	MESSAGE_HEADER objMsgHeader;
	U64 nId;             ///< ����id
	U64 nOffset;               ///< ƫ��
}MOVE_STREAM_OFFSET_REQUEST;


// --------------------------------------------------------------------------------
/// <summary>
/// �ƶ���������ƫ��,���ڶϵ�����
/// </summary>
// --------------------------------------------------------------------------------
typedef struct  _move_stream_offsert_reponse
{
	MESSAGE_HEADER objMsgHeader;
	U64 nId;             ///< ����id
	U64 nOffset;      ///< ƫ��
	U32 nStatusCode;  ///< ������Ӧ���״̬��
}MOVE_STREAM_OFFSET_REPONSE;

#pragma pack()


//************************************************************************************************//
//������һЩ��������

#define MESSAGE_HEADER_LEN     sizeof(MESSAGE_HEADER)
#define MESSAGE_FIXED_PAYLOAD_LEN(a)    (sizeof(a)  -  sizeof(MESSAGE_HEADER)) ///< �̶����ݵĳ��ȣ�һ�����ݰ�Я���������ɹ̶�����+�ɱ����ݣ���ѡ��
#define REQUEST_SUCCESSS(a)    ((OR_SUCCESS & a) == OR_SUCCESS)
#define MESSAGE_DATA_PTR(a)    (((unsigned char*)a) + MESSAGE_HEADER_LEN)     ///<  ��������ͷָ��



// --------------------------------------------------------------------------------
/// <summary>
/// ����ECC���ĳ���
/// </summary>
/// <param name="nPlaintextLen">���ĳ���</param>
/// <returns>���ĳ���</returns>
/// <created>������,2019/2/19</created>
/// <changed>������,2019/2/19</changed>
// --------------------------------------------------------------------------------
inline U32 CalcECCCiphertextLength(U32 nPlaintextLen)
{
	return (nPlaintextLen + 45) / 46 * 48 + 1;
}

inline U32 GetMsgId()
{
	static U32 uId = 1;
	return uId++;
}

template<class T>
inline unsigned char* GetVarDataPtr(unsigned char * pPacket)
{
	return pPacket + sizeof(T);
}

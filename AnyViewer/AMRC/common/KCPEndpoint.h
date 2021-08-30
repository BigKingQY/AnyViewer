////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  UDTEndpoint.h
/// @brief 采用UDT协议进行传输的端点模板类定义
/// 
/// 报文格式:http://www.wisestudy.cn/opentech/udt-packet.html
/// UDT源码分析:https://www.cnblogs.com/ukernel/category/1208916.html
/// 注意:受到KCP引擎的限制，消息模式下单个包的最大长度不能查过255 * MTU(默认是1400)
/// @author 黄丽云
/// @version 1.0
/// @date    2020-12-25
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "ServiceImpl.h"
#include "ikcp.h"
#include "AIMTime.h"

#include <atomic>

#import <sys/socket.h>
#import <netinet/in.h>
#import <arpa/inet.h>

	//**********************************************************************
	/// @brief 可靠的通信端点类
	///
	/// 下面的类是一个可靠会话模板类，有心跳功能，并且能保证数据包百分之百正确的到达目标主机\n
	/// 适应于服务器端和客户端
	//**********************************************************************
template <typename PacketT, class TransferT = CNetUDPDataTransfer,const U32 nIntputBufferSize = 10 * 1024>
class CKCPEndpoint 
	: public CNetEndpoint<PacketT, TransferT, nIntputBufferSize>
	,public CKCP
{
public:
	CKCPEndpoint(CNetEndpointEventSink* pNetConversationEventSink = NULL)
		: CNetEndpoint<PacketT, TransferT, nIntputBufferSize>(pNetConversationEventSink)
		, m_nNextID(0)
		, m_objKCPIntputBuffer(1024*1024)
	{
	}

	virtual ~CKCPEndpoint()
	{
		if (!m_bStopReceving)
		{
			m_bStopReceving = true;

			if (m_objRecevingThread.joinable())
			{
				m_objRecevingThread.join();
			}
		}
	}

public:
	virtual bool Initial(socket pSocket = NULL) override;

	/// @brief 连接到指定的服务器
	virtual bool Connect(UINT nIP, USHORT nPort, int nTimeout = 30) override;

	virtual bool Send(CNetPacket* pPacket) override;

	// 发送数据
	virtual bool Send(const BYTE* pData, const unsigned int nLen) override;


	virtual bool ExtractPacketfromBuffer(CDataStreamBuffer& objDataBuffer);

	virtual void Close()  override;

	// 发送简单的消息
	bool SendCommonRequest(const unsigned int nType);
protected:

	// 输出数据
	virtual int KCPOutput(const char *pBuffer, const int nLen) override;

	virtual void Writelog(const char *lpLog) override;
protected:
	
	void KCPRecevingThread();

protected:
	//--------------------------------------------------------------------------------------------------------------------
	//函数名称： CKCPEndpoint::Clear
	/// @brief  清除三个队列并删除队列中的节点
	///
	/// @author 黄丽云
	//--------------------------------------------------------------------------------------------------------------------
	void Clear()
	{
		//std::lock_guard<std::recursive_mutex> objAutoLocker(m_objCritSec);


	}

	UINT CreateID() { return ++m_nNextID; }



private:

	mutable std::recursive_mutex     m_objCritSec;                 ///< 会话链表临界区	

	DECLARE_MEMBER_AND_GET_METHOD_V11(UINT, m_nNextID, NextID, 0);     /// < 下一个数据包ID编号
	std::thread              m_objRecevingThread;
	std::atomic_bool         m_bStopReceving{false};
	CDataStreamBuffer        m_objKCPIntputBuffer;                    ///< 存放从KCP接收数据的缓冲区
};


template<typename PacketT, class TransferT, const U32 nIntputBufferSize>
inline bool CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::Initial(SOCKET pSocket)
{
	bool bResult = CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::Initial(pSocket);

	if (bResult)
	{
		CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::m_objInputBuffer.Allocate(2 * 1024);
		bResult = KCPInit(0);

		if (bResult)
		{
			KCPSetNoDelay(1, 10, 2, 1);   ///< 极速模式			
			SetStreamMod(CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::GetClient()? false:true);	/// 注意客户端和服务端采用的模式不同，		
			m_bStopReceving = false;

			std::thread objRecevingThread(std::bind([this]() { KCPRecevingThread(); }));

			m_objRecevingThread = std::move(objRecevingThread);
		}
	}

	return bResult;
}

//--------------------------------------------------------------------------------------------------------------------
//函数名称： CNetClientImpl::Connect
/// @brief 连接到指定的服务器
///
/// @param _in UINT nIP->服务器的IP地址
/// @param _in USHORT nPort->服务器的端口
/// @param _in int nTimeout->连接超时时间(缺省是30秒)
/// @return  bool true: 成功, false: 失败
/// @author 黄丽云
//--------------------------------------------------------------------------------------------------------------------
template<typename PacketT, class TransferT, const U32 nIntputBufferSize>
inline bool CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::Connect(UINT nIP, USHORT nPort, int nTimeout)
{
	std::lock_guard<std::recursive_mutex> objAutoLocker(m_objCritSec); //必须如此，该函数可能并发调用	
	bool bResult = CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::Connect(nIP, nPort, nTimeout);

	return bResult;
}


// ********************************************************************************
/// <summary>
/// 把数据发送到KCP引擎中去
/// </summary>
/// <param name="pData">指向数据的指针</param>
/// <param name="nLen">被发送数据的长度</param>
/// <returns></returns>
/// <created>Andy,2020/12/28</created>
/// <changed>Andy,2020/12/28</changed>
// ********************************************************************************
template<typename PacketT, class TransferT, const U32 nIntputBufferSize>
inline bool CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::Send(CNetPacket* pPacket)
{
	return CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::Send(pPacket);
}


// ********************************************************************************
/// <summary>
/// 把数据发送到KCP引擎中去
/// </summary>
/// <param name="pData">指向数据的指针</param>
/// <param name="nLen">被发送数据的长度</param>
/// <returns></returns>
/// <created>Andy,2020/12/28</created>
/// <changed>Andy,2020/12/28</changed>
// ********************************************************************************
template<typename PacketT, class TransferT, const U32 nIntputBufferSize>
inline bool CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::Send(const BYTE * pData, const unsigned int nLen)
{
	const int nBytesSent = KCPSend((const char*)pData, nLen);

	return (nBytesSent >= 0);
}


// ********************************************************************************
/// <summary>
/// 从KCP引擎中读取数据，并跟新KCP的状态
/// </summary>
/// <created>Andy,2020/12/28</created>
/// <changed>Andy,2020/12/28</changed>
// ********************************************************************************
template<typename PacketT, class TransferT, const U32 nIntputBufferSize>
inline void CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::KCPRecevingThread()
{
	while (!m_bStopReceving)
	{
		KCPUpdate((U32)(CAIMTime::GetCurMilliseconds()&0xfffffffful));

		const int nBytesReceived  = KCPRecv((char *)m_objKCPIntputBuffer.GetBufferFreeHeadPtr(), m_objKCPIntputBuffer.GetFreeSize());

		if (nBytesReceived > 0)
		{
			m_objKCPIntputBuffer.IncreaseDataLen(nBytesReceived);
			CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::ExtractPacketfromBuffer(m_objKCPIntputBuffer);
		}

		// 如果接收缓冲区和发送缓冲中都有数据，那不用等待
		if(GetRecevingQueueSize() == 0 && GetSendingQueueSize() == 0)
		{
			std::chrono::milliseconds objSleepDyratuib(10);

			std::this_thread::sleep_for(objSleepDyratuib);
		}
	}
}

// ********************************************************************************
/// <summary>
/// 该函数不在实际分析数据包，而是把收到的数据直接写入KCP引擎
/// </summary>
/// <param name="objDataBuffer">数据缓冲对象的参考</param>
/// <returns></returns>
/// <created>Andy,2020/12/28</created>
/// <changed>Andy,2020/12/28</changed>
// ********************************************************************************
template<typename PacketT, class TransferT, const U32 nIntputBufferSize>
inline bool CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::ExtractPacketfromBuffer(CDataStreamBuffer& objDataBuffer)
{
	const int nBytesInputed = KCPInput((const char*)objDataBuffer.GetBufferHeadPtr(), objDataBuffer.GetDataLen());

	objDataBuffer.Empty();

	return (nBytesInputed > 0);
}

template<typename PacketT, class TransferT, const U32 nIntputBufferSize>
inline void CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::Close()
{
	CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::Close();	
}

template<typename PacketT, class TransferT, U32 nIntputBufferSize>
inline bool CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::SendCommonRequest(const unsigned int nType)
{
	PacketT objRequestPacket;

	objRequestPacket.Initial(nType, 0, VNCP::OT_REQUEST, 0);

	return CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::Send(&objRequestPacket);
}


// ********************************************************************************
/// <summary>
/// 被KCP引擎调用发送数据到底层网络
/// </summary>
/// <param name="pBuffer">指向被发送数据的指针</param>
/// <param name="nLen">被发送数据的长度</param>
/// <returns></returns>
/// <created>Andy,2020/12/28</created>
/// <changed>Andy,2020/12/28</changed>
// ********************************************************************************
template<typename PacketT, class TransferT, const U32 nIntputBufferSize>
inline int CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::KCPOutput(const char * pBuffer, const int nLen)
{
	bool bResult = false;

	assert(NULL != pBuffer);

	if (NULL != pBuffer && nLen > 0)
	{
		const int nBytesSent = CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::m_pDataTransfer->Send((const BYTE *)pBuffer, nLen);
		bResult = (nBytesSent == int(nLen));

		if (SOCKET_ERROR == nBytesSent)
		{
			if (std::is_same<TransferT, CNetTCPDataTransfer>::value)
			{
				CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::Connect(
					CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::m_pDataTransfer->GetIP()
					, CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::m_pDataTransfer->GetPort(), 5);
				CNetEndpoint<PacketT, TransferT, nIntputBufferSize>::m_pNetEndpointEventSink->OnReconnectEventHandle(
					CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::shared_from_this());
			}
		}
	}

	return bResult;
}

template<typename PacketT, class TransferT, const U32 nIntputBufferSize>
inline void CKCPEndpoint<PacketT, TransferT, nIntputBufferSize>::Writelog(const char * lpLog)
{
	LOG_DEBUG("%s",lpLog);
}

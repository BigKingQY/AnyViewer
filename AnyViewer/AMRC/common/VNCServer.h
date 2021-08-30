///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  
/// @brief VNC通信服务器类定义
/// 
/// 该类可以收发控制端和被控制端数据
/// @author 黄丽云
/// @version 1.0
/// @date        2020-11-27
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ServiceImpl.h"
#include "VNCEndpoint.h"
#include "LocalBusMessageCode.h"
#include "SDPMessageBus.h"

#include <boost/timer.hpp>

const int HEART_TIMEOUT = 90; //心跳超时

extern CMessageBus& GetVNCMessageBus();

template<class base>
class CVNCDataServerT : public base
{
public:
	CVNCDataServerT(
		CNetEndpointEventSink* pEventSink
		, UINT nIP
		, USHORT nPort
		, UINT nWorkerThreadCount = 2)
		: m_pEventSink(pEventSink)
	{
		base::SetIP(nIP);
		base::SetListenPort(nPort);
		base::SetWorkerThreadCount(nWorkerThreadCount);
	}

	~CVNCDataServerT()
	{
	}

public:  // 通信结构事件	


	// 处理收到数据包事件
	virtual bool OnReceivedPacketEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint, CNetPacket* pPacket) override
	{
		bool bResult = base::OnProReceivedPacketEventHandle(pEndpoint, pPacket);

		if (!bResult)
		{
			return bResult;
		}

		CVNCPacket* pDataPacket = dynamic_cast<CVNCPacket*>(pPacket);   //°²È«µÄÏòÏÂÀàÐÍ×ª»¯

		CHECK_POINTER_EX(pDataPacket, false);

		if (std::is_same<base, CNetUDPServiceImpl< CVNCUDPEndpoint>>::value)
		{
			if (pDataPacket->GetType() == VNCP::MT_SYN)
			{
				CVNCPacket objSynPacket;

				objSynPacket.Initial(VNCP::MT_SYN, 0, VNCP::OT_RESPONSE, 0);
				bResult = pEndpoint->Send(&objSynPacket);
				std::dynamic_pointer_cast<CVNCUDPEndpoint>(pEndpoint)->ChanageStatus(RS_ESTABLISH);

				return bResult;
			}
		}


		if (GetSenttoMsgBus())
		{
			CMessageBus& refMessageBus = GetVNCMessageBus();

			refMessageBus.SendReq<void, PTR_NET_ENDPOINT_INTERFACE, CVNCPacket*>(
				LMBS_RECEIVED_VNC_PACKET
				, std::forward <PTR_NET_ENDPOINT_INTERFACE>(pEndpoint)
				, std::forward <CVNCPacket*>(pDataPacket));
		}

		m_pEventSink->OnReceivedPacketEventHandle(pEndpoint, pPacket);

		return bResult;
	}

	/// @brief 会话对象关闭事件 
	virtual bool OnCloseEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint) override
	{
		if (GetSenttoMsgBus())
		{
			// 必须再线程中释放
			CMessageBus& refMessageBus = GetVNCMessageBus();

			refMessageBus.SendReq<void, PTR_NET_ENDPOINT_INTERFACE>(
				LMBS_CLOSE_VNC_CONNECTION
				, std::forward <PTR_NET_ENDPOINT_INTERFACE>(pEndpoint));

			//std::thread objUnRegist
			//(
			//	[&refMessageBus, &pEndpoint]()
			//	{
			//		refMessageBus.SendReq<void, PTR_NET_ENDPOINT_INTERFACE>(
			//			LMBS_CLOSE_VNC_CONNECTION
			//			, std::forward <PTR_NET_ENDPOINT_INTERFACE>(pEndpoint));
			//	}
			//);

			//objUnRegist.detach();
			
		}

		return true;
	}

	virtual void OnNewConversationEventHandle(PTR_NET_ENDPOINT_INTERFACE pEndpoint)
	{
		m_pEventSink->OnNewConversationEventHandle(pEndpoint);
	}


private:
	CNetEndpointEventSink*   m_pEventSink = nullptr;
	DECLARE_MEMBER_AND_METHOD_V11(bool, m_bSenttoMsgBus, SenttoMsgBus, true);     /// < 是否把消息发往消息总线
};

using CVNCDataTCPServer = CVNCDataServerT<CNetTCPServiceImpl< CVNCTCPEndpoint>>;
using CVNCDataTCPServerPtr = std::shared_ptr<CVNCDataTCPServer>;


using CVNCDataUDPServer = CVNCDataServerT<CNetUDPServiceImpl< CVNCUDPEndpoint>>;
using CVNCDataUDPServerPtr = std::shared_ptr<CVNCDataUDPServer>;

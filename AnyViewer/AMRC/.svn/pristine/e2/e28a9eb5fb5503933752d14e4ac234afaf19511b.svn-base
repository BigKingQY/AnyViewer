#include "WebsocketSvr.h"
#include <WebSocketParse.h>
#include <iostream>
#include <string>
#include <fstream>

CWebsocketSvr::CWebsocketSvr(U16 nPort)
    :CNetServiceImpl(0, nPort)
{
}


CWebsocketSvr::~CWebsocketSvr()
{
}

BOOL CWebsocketSvr::OnReceivedPacketEventHandle(CNetEndpointInterface* p_net_endpoint, CNetPacket* p_net_packet)
{
    CWebSocketPacket* p_websocket_packet = dynamic_cast<CWebSocketPacket*>(p_net_packet);
    if (nullptr != p_websocket_packet)
    {
        if (CWebSocketParse::wdt_disconnect == p_websocket_packet->GetDataType())
        {
            p_net_endpoint->Close();
            printf("%p: close\n", p_net_endpoint);
        }
        else
        {
            on_received_really_data(p_net_endpoint, p_websocket_packet);
        }
    }
    else
    {
        CWsktHandshakePacket* p_handshark_packet = dynamic_cast<CWsktHandshakePacket*>(p_net_packet);

        on_handshark(p_net_endpoint, p_handshark_packet);

        NET_ENDPOINT* p_endpoint = (NET_ENDPOINT*)p_net_endpoint;

        p_endpoint->ChangePacketType<CWebSocketPacket>();
    }

    return TRUE;
}

bool CWebsocketSvr::on_received_really_data(CNetEndpointInterface*, CWebSocketPacket*)
{
    return true;
}

//------------------------------------------------------------------------------------
/// @method on_handshark
/// @brief 处理websocket握手消息 
/// 
/// @param _in p_net_endpoint->通信端点
/// @param _in p_handshark_packet 握手数据包
/// @return 成功返回NET_EXTRACT_DATA_PACKET_RESULT枚举
/// @author Leiyz
/// @date 20170829
//------------------------------------------------------------------------------------
void CWebsocketSvr::on_handshark(CNetEndpointInterface* p_net_endpoint, CWsktHandshakePacket* p_handshark_packet)
{
    std::string str_websocket_key;

    str_websocket_key = p_handshark_packet->Getwebsocket_key();

    CWsktHandshakePacket objDatapacket;

    objDatapacket.InitHandsharkResponse(str_websocket_key);
    p_net_endpoint->Send(&objDatapacket);
}
#pragma once

#include "WebSocketPacket.h"
#include "WsktHandshakePacket.h"
#include <ServiceImpl.h>

class CWebsocketSvr : public CNetServiceImpl<CNetEndpoint<CWsktHandshakePacket, false> >
{
public:
    CWebsocketSvr(U16 nPort);
    ~CWebsocketSvr();

    using NET_ENDPOINT = CNetEndpoint<CWsktHandshakePacket, false>;

public:
    /// @brief 接收数据包事件
    virtual BOOL OnReceivedPacketEventHandle(CNetEndpointInterface*, CNetPacket*) override;

    //收到websocket真实数据
    virtual bool on_received_really_data(CNetEndpointInterface*, CWebSocketPacket*);

private:
    void on_handshark(CNetEndpointInterface*, CWsktHandshakePacket*);
};


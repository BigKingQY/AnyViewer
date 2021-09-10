//
//  VNCEndpointT.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/9.
//

#ifndef VNCEndpointT_h
#define VNCEndpointT_h

#pragma once
#include "CNetEndpoint.h"
#include "VNCPacket.h"
#include "KCPEndpoint.h"

template <class TransferT = CNetTCPDataTransfer>
class CVNCEndpointT : public CNetEndpoint<CVNCPacket, TransferT, CVNCPacket::PACKET_LEN_MAX>
{
public:
    CVNCEndpointT(CNetEndpointEventSink* pNetEndpointEventSink)
    : CNetEndpoint<CVNCPacket, TransferT, CVNCPacket::PACKET_LEN_MAX>::CNetEndpoint(pNetEndpointEventSink)
    {
    }
    ~CVNCEndpointT(){}

public:
    bool Send(CNetPacket * pPacket) override
    {
        if (!CNetEndpoint<CVNCPacket, TransferT, CVNCPacket::PACKET_LEN_MAX>::GetNetEndpointEventSink()->OnPreSendPacketEventHandle(
            CNetEndpointInterface::shared_from_this()
            , pPacket))
        {
            return false;
        }

        return CNetEndpoint<CVNCPacket, TransferT, CVNCPacket::PACKET_LEN_MAX>::Send(pPacket);
    }

    // ********************************************************************************
/// <summary>
/// ∑¢ÀÕºÚµ•µƒœ˚œ¢
/// </summary>
/// <param name="nType">œ˚œ¢¿‡–Õ</param>
/// <returns></returns>
/// <created>Andy,2020/11/30</created>
/// <changed>Andy,2020/11/30</changed>
// ********************************************************************************
    bool SendCommonRequest(const unsigned int nType)
    {
        CVNCPacket objRequestPacket;

        objRequestPacket.Initial(nType, 0, VNCP::OT_REQUEST, 0);
        return CNetEndpoint<CVNCPacket, TransferT, CVNCPacket::PACKET_LEN_MAX>::Send(&objRequestPacket);
    }

};


using CVNCTCPEndpoint = CVNCEndpointT<CNetTCPDataTransfer>;
using CVNCUDPEndpoint = CKCPEndpoint<CVNCPacket,CNetUDPDataTransfer, CVNCPacket::PACKET_LEN_MAX>;
using CVNCTCPEndpointPtr = std::shared_ptr<CVNCTCPEndpoint>;
using CVNCUDPEndpointPtr = std::shared_ptr<CVNCUDPEndpoint>;

//using CVNCTCPEndpoint = CVNCEndpointT<CNetTCPDataTransfer>;
//using CVNCUDPEndpoint = CKCPEndpoint<CVNCPacket,CNetUDPDataTransfer, CVNCPacket::PACKET_LEN_MAX>;
//using CVNCTCPEndpointPtr = std::shared_ptr<CVNCTCPEndpoint>;
//using CVNCUDPEndpointPtr = std::shared_ptr<CVNCUDPEndpoint>;


#endif /* VNCEndpointT_h */

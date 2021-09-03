#pragma once

#include <ServiceImpl.h>
#include <WebSocketParse.h>


class CWebSocketPacket : public CNetPacketBase
{
public:
    CWebSocketPacket();
    CWebSocketPacket(const BYTE* pBuffer, INT nLen);
    ~CWebSocketPacket();

public:
    virtual NET_EXTRACT_DATA_PACKET_RESULT ExtractData(CDataStreamBuffer&) override;

    int GetPacketLen();// 获取整个报的长度、本机字节序

    const BYTE* GetBuffer(){ return m_objDataBuffer.GetDataHeadPtr(); }

private:
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_b_fin, FinFlag, false)                                    ///<FIN标志
    DECLARE_MEMBER_AND_METHOD_V11(CWebSocketParse::WEB_DATA_TYPE, m_e_data_type, DataType, CWebSocketParse::wdt_text)                 ///<数据类型

};


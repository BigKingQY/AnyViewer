#include "WebSocketPacket.h"

CWebSocketPacket::CWebSocketPacket()
    :CNetPacketBase(1024)
{
}

CWebSocketPacket::CWebSocketPacket(const BYTE* pBuffer, INT nLen)
    :CNetPacketBase(pBuffer, nLen)
{
    ;
}

CWebSocketPacket::~CWebSocketPacket()
{
}

//------------------------------------------------------------------------------------
/// @method ExtractData
/// @brief 从缓冲区中提取数据包(被提取的数据一定是大于包头的长度的，否则出错)
/// 
/// 数据格式处理条件：
/// 1.客户端向服务器传输的数据帧必须进行掩码处理：服务器若接收到未经过掩码处理的数据帧，则必须主动关闭连接。
/// 2.服务器向客户端传输的数据帧一定不能进行掩码处理。客户端若接收到经过掩码处理的数据帧，则必须主动关闭连接。
/// 发现错误的一方可向对方发送close帧（状态码是1002，表示协议错误），以关闭连接。
/// @param _in CDataStreamBuffer& refDataBuffer->一个指向缓冲区的引用
/// @return 成功返回NET_EXTRACT_DATA_PACKET_RESULT枚举
/// @author Leiyz
/// @date 20170829
//------------------------------------------------------------------------------------
NET_EXTRACT_DATA_PACKET_RESULT CWebSocketPacket::ExtractData(CDataStreamBuffer& refDataBuffer)
{
    NET_EXTRACT_DATA_PACKET_RESULT nResult = NEDPR_INVALID;

    CWebSocketParse obj_web_socket_parse;
    NET_EXTRACT_DATA_PACKET_RESULT n_result = obj_web_socket_parse.parse_interaction_packet(refDataBuffer, m_objDataBuffer);

    if (NEDPR_JUST_A_PACKET == n_result || NEDPR_MORE_PACKETS == n_result)
    {
        m_e_data_type = obj_web_socket_parse.GetDataType();
    }

    return n_result;
}

int CWebSocketPacket::GetPacketLen()
{
    return m_objDataBuffer.GetDataLen();
}
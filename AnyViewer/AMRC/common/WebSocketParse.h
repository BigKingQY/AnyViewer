/////////////////////////////////////////////////////////////////////////////////////////
/// @file  CWebSocketParse.h
/// @brief websocket数据包解析类
/// 
/// @author Leiyz
/// @date   20170831
/////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ServiceImpl.h>
#include <string>
#include <unordered_map>

class CWebSocketParse
{
public:
    CWebSocketParse();
    ~CWebSocketParse();

    enum  SCAN_GET_RET
    {
        SGR_FAILED,                    ///<查找失败
        SGR_INCOMPLETE,                ///<字段不完整
        SGR_SUCCESS,                   ///<查找成功
    };

    enum WEB_DATA_TYPE
    {
        wdt_middle_packet = 0,                                 ///<中间数据包
        wdt_text,                                              ///<text文本
        wdt_binary,                                            ///<二进制
        wdt_disconnect = 8,                                    ///<断开连接类型数据包
        wdt_ping,                                              ///<ping类型数据包
        wdt_pong,                                              ///<pong类型数据包
    };

public:
    //解析握手数据包 
    NET_EXTRACT_DATA_PACKET_RESULT parse_handshark_packet(CDataStreamBuffer& obj_buffer_in, CDataStreamBuffer& obj_buffer_out);

    //获取Sec-WebSocket-Key
    void get_websocket_key(std::string& strWebsocketKey);

    //创建握手响应包
    bool create_handshark_response(const std::string& str_websocket_key, CDataStreamBuffer& obj_buffer);

    //解析交互数据包
    NET_EXTRACT_DATA_PACKET_RESULT parse_interaction_packet(CDataStreamBuffer& obj_buffer_in, CDataStreamBuffer& obj_buffer_out);

private:
    //扫描握手头
    SCAN_GET_RET scan_handshark_header(const U8* pBuffer, const UINT nBuflen, UINT& nOffset);

    //校验握手数据报key
    bool verify_hanshark_key();

private:
    //提取fin标记
    bool extract_fin(U8 cFin);

    //提取操作码
    void extract_opcode(U8 n_opcode);

    //提取载荷长度
    bool extract_payload_len(U8* pBuffer, const UINT nLength, UINT& nPos);

    //提取数据
    void extract_payload_data(U8* pBuffer, const UINT nLength, CDataStreamBuffer& obj_buffer_out);

private:
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_b_fin, FinFlag, false)                                    ///<FIN标志
    DECLARE_MEMBER_AND_METHOD_V11(WEB_DATA_TYPE, m_e_data_type, DataType, wdt_text)                 ///<数据类型
    DECLARE_MEMBER_AND_METHOD_V11(U64, m_n_payload_len, PayloadLen, 0)                              ///<载荷数据长度

private:
    std::string m_str_get;                                                     //握手GET行
    static const char* m_psz_http_ver;                                         
    static const UINT HANDSHARK_LEN_MAX = 4096;                                //握手数据报阈值
    static const char* m_psz_web_socket_key;
    static const char MAGIC_KEY[];                                             //websocket握手响应包魔数
    UINT m_n_data_len{ 0 };
    std::unordered_map<std::string, std::string> m_map_handshark_request;      //握手请求字段k,v 
};


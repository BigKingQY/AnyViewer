#include "WebSocketParse.h"
#include <LyUtilities.h>
#include <base64.h>
#include <sha1.h>

const char* CWebSocketParse::m_psz_http_ver = "HTTP/1.1";
const char* CWebSocketParse::m_psz_web_socket_key = "sec-websocket-key";
const char CWebSocketParse::MAGIC_KEY[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

CWebSocketParse::CWebSocketParse()
{
}


CWebSocketParse::~CWebSocketParse()
{
}

//------------------------------------------------------------------------------------
/// @method parse_handshark_packet
/// @brief 解析握手数据报  
/// 
/// @param _in obj_buffer_in->输入数据流
/// @param _out obj_buffer_out 输出数据流
/// @return 成功返回NET_EXTRACT_DATA_PACKET_RESULT枚举
/// @author Leiyz
/// @date 20170831
//------------------------------------------------------------------------------------
NET_EXTRACT_DATA_PACKET_RESULT CWebSocketParse::parse_handshark_packet(CDataStreamBuffer& obj_buffer_in, CDataStreamBuffer& obj_buffer_out)
{
    NET_EXTRACT_DATA_PACKET_RESULT e_result = NEDPR_INVALID;
  
    UINT nOffset = 0;
    SCAN_GET_RET e_scan_get_ret = scan_handshark_header(obj_buffer_in.GetDataHeadPtr(), obj_buffer_in.GetDataLen(), nOffset);
    obj_buffer_in.MoveReadingPos(nOffset);

    if (SGR_SUCCESS != e_scan_get_ret)
    {
        if (SGR_INCOMPLETE == e_scan_get_ret)
        {
            e_result = NEDPR_INCOMPLETE_PACKET;
        }
        return e_result;
    }

    const U8* p_valid_data = obj_buffer_in.GetDataHeadPtr();
    const UINT n_remain_len = obj_buffer_in.GetDataLen();
    const char szFinishFlag[] = "\r\n\r\n";
    std::string strRequest((char*)p_valid_data, n_remain_len);
    int n_pos = strRequest.rfind(szFinishFlag);

    if (n_pos == std::string::npos)
    {
        if (n_remain_len > HANDSHARK_LEN_MAX)
        {
            return NEDPR_INVALID;
        }
        return NEDPR_INCOMPLETE_PACKET;
    }

    const UINT n_packet_len =  n_pos + strlen(szFinishFlag);

    if (n_packet_len == n_remain_len)
    {
        e_result = NEDPR_JUST_A_PACKET;
    }
    else
    {
        e_result = NEDPR_MORE_PACKETS;
    }      

    //分离字段
    std::istringstream iss(strRequest.substr(m_str_get.size(), n_packet_len));
    std::string strHeader;
    bool bSuccess = true;

    while (std::getline(iss, strHeader) && "\r" != strHeader)
    {
        if ('\r' != *(strHeader.rbegin()))
        {
            assert(false && "websocket header failed!");
            bSuccess = false;
            break;
        }
        else
        {
            strHeader.pop_back();
        }

        int nPosColon = strHeader.find(": ");
        if (std::string::npos == nPosColon)
        {
            bSuccess = false;
            break;
        }

        std::string str_key = strHeader.substr(0, nPosColon);
        m_map_handshark_request[CLyUtilities::make_lower(str_key)] = strHeader.substr(nPosColon + 2);
    }

    if (!bSuccess)
    {
        e_result = NEDPR_INVALID;
        m_map_handshark_request.clear();
        return e_result;
    }

    for (auto it = m_map_handshark_request.begin(); it != m_map_handshark_request.end(); ++it)
    {
        printf("%s, %s\n", it->first.c_str(), it->second.c_str());
    }

    if (!verify_hanshark_key())
    {
        e_result = NEDPR_INVALID;
        m_map_handshark_request.clear();
        return e_result;
    }

    obj_buffer_in.Read(obj_buffer_out, n_packet_len);

    return e_result;
}

//------------------------------------------------------------------------------------
/// @method scan_handshark_header
/// @brief 扫描握手头
/// 
/// 握手头由GET开始\r\n结束，且HTTP版本为1.1
/// @param _in pBuffer->待扫描缓冲区
/// @param _in nBuflen 缓冲区大小
/// @param _in nOffset 指向扫描结束后无效字符偏移
/// @return 成功返回NET_EXTRACT_DATA_PACKET_RESULT枚举
/// @author Leiyz
/// @date 20170831
//------------------------------------------------------------------------------------
CWebSocketParse::SCAN_GET_RET CWebSocketParse::scan_handshark_header(const U8* pBuffer, const UINT nBuflen, UINT& nOffset)
{
    SCAN_GET_RET nResult = SGR_FAILED;

    enum SCAN_GET
    {
        SCAN_G,
        SCAN_E,
        SCAN_T,
        SCAN_RET,
    };

    SCAN_GET e_scan_get = SCAN_G;
    bool bComplete = false;
    m_str_get.clear();

    //查找GET,websocke握手开头必须是GET
    for (nOffset = 0; nOffset < nBuflen; ++nOffset)
    {
        switch (e_scan_get)
        {
        case SCAN_G:
        {
            if ('g' == pBuffer[nOffset] || 'G' == pBuffer[nOffset])
            {
                e_scan_get = SCAN_E;
                m_str_get.push_back(pBuffer[nOffset]);
            }
        }
        break;
        case SCAN_E:
        {
            if ('e' == pBuffer[nOffset] || 'E' == pBuffer[nOffset])
            {
                e_scan_get = SCAN_T;
                m_str_get.push_back(pBuffer[nOffset]);
            }
            else
            {
                e_scan_get = SCAN_G;
                m_str_get.empty();
            }
        }
        break;
        case SCAN_T:
        {
            if ('t' == pBuffer[nOffset] || 'T' == pBuffer[nOffset])
            {
                e_scan_get = SCAN_RET;
                m_str_get.push_back(pBuffer[nOffset]);
            }
            else
            {
                e_scan_get = SCAN_G;
                m_str_get.empty();
            }
        }
        break;
        case SCAN_RET:
        {
            m_str_get.push_back(pBuffer[nOffset]);
            if ('\n' == pBuffer[nOffset])
            {
                bComplete = true;
            }
        }
        break;
        default:
            break;
        }

        if (bComplete)
        {
            nResult = SGR_SUCCESS;
            break;
        }
    }

    //GET行大小阈值
    const size_t GET_LEN_MAX = 1024;
    
    if (nResult == SGR_FAILED && !m_str_get.empty() && m_str_get.length() < GET_LEN_MAX)
    {
        nResult = SGR_INCOMPLETE;
    }

    //修正偏移位置
    if (SGR_FAILED != nResult)
    {
        nOffset -= m_str_get.length();
    }

    //查找HTTP/1.1
    if (std::string::npos == m_str_get.rfind(m_psz_http_ver))
    {
        nResult = SGR_FAILED;
        m_str_get.clear();
    }

    return nResult;
}

//------------------------------------------------------------------------------------
/// @method verify_hanshark_key
/// @brief 校验握手数据包头指定字段
/// 
/// @return 成功true， 失败false
/// @author Leiyz
/// @date 20170831
//------------------------------------------------------------------------------------
bool CWebSocketParse::verify_hanshark_key()
{
    const char* arr_handshark_kye[] =
    {
        "upgrade"
        , "sec-websocket-version"
        , "sec-websocket-key"
    };

    for (auto value : arr_handshark_kye)
    {
        if (m_map_handshark_request.end() == m_map_handshark_request.find(value))
        {
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------------
/// @method get_websocket_key
/// @brief 获取握手请求websocketkey
/// 
/// @param -out strWebsocketKey-所获取的key
/// @author Leiyz
/// @date 20170831
//------------------------------------------------------------------------------------
void CWebSocketParse::get_websocket_key(std::string& strWebsocketKey)
{
    strWebsocketKey.clear();
    auto it = m_map_handshark_request.find(m_psz_web_socket_key);
    if (m_map_handshark_request.end() != it)
    {
        strWebsocketKey = it->second;
    }
}

//创建握手响应包
//------------------------------------------------------------------------------------
/// @method create_handshark_response
/// @brief 创建websocketkey握手响应包
/// 
/// @param -out str_websocket_key- Sec-WebSocket-Key
/// @param obj_buffer-创建响应包的缓冲区对象
/// @author Leiyz
/// @date 20170904
//------------------------------------------------------------------------------------
bool CWebSocketParse::create_handshark_response(const std::string& str_websocket_key, CDataStreamBuffer& obj_buffer)
{
    obj_buffer.Empty();
    bool bRet = obj_buffer.Reallocate(1024);

    if (!bRet)
    {
        printf("Reallocate failed!\n");
        return false;
    }

    char* pBuffer = (char*)obj_buffer.GetBufferHeadPtr();

    obj_buffer.IncreaseDataLen(snprintf((char*)obj_buffer.GetBufferFreeHeadPtr(), obj_buffer.GetFreeSize(), "HTTP/1.1 101 Switching Protocols\r\n"));
    obj_buffer.IncreaseDataLen(snprintf((char*)obj_buffer.GetBufferFreeHeadPtr(), obj_buffer.GetFreeSize(), "Connection: Upgrade\r\n"));
    obj_buffer.IncreaseDataLen(snprintf((char*)obj_buffer.GetBufferFreeHeadPtr(), obj_buffer.GetFreeSize(), "Upgrade: websocket\r\n"));
    obj_buffer.IncreaseDataLen(snprintf((char*)obj_buffer.GetBufferFreeHeadPtr(), obj_buffer.GetFreeSize(), "Sec-WebSocket-Accept: "));

    std::string str_server_key = str_websocket_key + MAGIC_KEY;

    SHA1 sha;
    unsigned int message_digest[5];
    sha.Reset();
    sha << str_server_key.c_str();

    sha.Result(message_digest);
    for (int i = 0; i < 5; i++)
    {
        message_digest[i] = htonl(message_digest[i]);
    }

    str_server_key = base64_encode(reinterpret_cast<const unsigned char*>(message_digest), 20);
    str_server_key += "\r\n";
    obj_buffer.IncreaseDataLen(snprintf((char*)obj_buffer.GetBufferFreeHeadPtr(), obj_buffer.GetFreeSize(), "%s\r\n", str_server_key.c_str()));

    return true;
}

//------------------------------------------------------------------------------------
/// @method parse_interaction_packet
/// @brief 解析交互数据包
/// 
/// 数据格式处理条件：
/// 1.客户端向服务器传输的数据帧必须进行掩码处理：服务器若接收到未经过掩码处理的数据帧，则必须主动关闭连接。
/// 2.服务器向客户端传输的数据帧一定不能进行掩码处理。客户端若接收到经过掩码处理的数据帧，则必须主动关闭连接。
/// 发现错误的一方可向对方发送close帧（状态码是1002，表示协议错误），以关闭连接。
/// @param _in obj_buffer_in 一个指向输入缓冲区的引用
/// @param _in obj_buffer_out 一个指向输出缓冲区的引用
/// @return 成功返回NET_EXTRACT_DATA_PACKET_RESULT枚举
/// @author Leiyz
/// @date 20170904
//------------------------------------------------------------------------------------
NET_EXTRACT_DATA_PACKET_RESULT CWebSocketParse::parse_interaction_packet(CDataStreamBuffer& obj_buffer_in, CDataStreamBuffer& obj_buffer_out)
{
    NET_EXTRACT_DATA_PACKET_RESULT nResult = NEDPR_INVALID;

    const BYTE* pData = obj_buffer_in.GetDataHeadPtr();
    const UINT  DATA_LENGTH = obj_buffer_in.GetDataLen();

    if (DATA_LENGTH < 2)
    {
        return NEDPR_INCOMPLETE_PACKET;
    }

    UINT nPos = 0;

    extract_fin(pData[nPos]);
    extract_opcode(pData[nPos]);
    ++nPos;

    //客户端请求必须进行掩码MASK处理
    if (pData[nPos] >> 7 < 1)
    {
        return NEDPR_INVALID;
    }

    UINT n_payload_pos = 0;
    //payload len
    if (!extract_payload_len((U8*)pData + nPos, DATA_LENGTH - nPos, n_payload_pos))
    {
        return NEDPR_INCOMPLETE_PACKET;
    }
    nPos += n_payload_pos;

    const UINT n_mask_len = 4;

    //获取数据
    if (DATA_LENGTH - nPos < m_n_payload_len + n_mask_len)
    {
        return NEDPR_INCOMPLETE_PACKET;
    }

    //提取数据
    extract_payload_data((U8*)pData + nPos, DATA_LENGTH - nPos, obj_buffer_out);
    if (DATA_LENGTH - nPos == m_n_payload_len + n_mask_len)
    {
        nResult = NEDPR_JUST_A_PACKET;
    }
    else
    {
        nResult = NEDPR_MORE_PACKETS;
    }

    nPos += m_n_payload_len + n_mask_len;
    obj_buffer_in.MoveReadingPos(nPos);


    return nResult;
}

//------------------------------------------------------------------------------------
/// @method extract_fin
/// @brief 提取FIN标志
///
/// @param _in cFin 待判断的数据
/// @return true-消息尾部， false-还有后续数据报
/// @author Leiyz
/// @date 20170830
//------------------------------------------------------------------------------------
bool CWebSocketParse::extract_fin(U8 cFin)
{
    return m_b_fin = (cFin >> 7) > 0;
}

void CWebSocketParse::extract_opcode(U8 n_opcode)
{
    m_e_data_type = (WEB_DATA_TYPE)(n_opcode & 0x0f);
}

//------------------------------------------------------------------------------------
/// @method extract_payload_len
/// @brief 提取载荷长度
///
/// @param _in pBuffer 待提取数据
/// @param _in nLength 数据长度
/// @param _out nPos 提取数据后的偏移
/// @return true-成功 ， false-数据长度不够
/// @author Leiyz
/// @date 20170830
//------------------------------------------------------------------------------------
bool CWebSocketParse::extract_payload_len(U8* pBuffer, const UINT nLength, UINT& nPos)
{
    if (0 == nLength || nullptr == pBuffer)
    {
        return false;
    }

    nPos = 0;
    U64 n_payload_len = pBuffer[nPos++] & 0x7f;

    if (126 == n_payload_len)
    {
        if (nLength - nPos < 2)
        {
            return false;
        }
        U16 nlength = *(U16*)(pBuffer + nPos);
        n_payload_len = NTOHS(nlength);
        nPos += 2;
    }
    else if (127 == n_payload_len)
    {
        if (nLength - nPos < 8)
        {
            return false;
        }
        U64 nlength = *(U64*)(pBuffer + nPos);
        n_payload_len = NTOHQ(nlength);
        nPos += 8;
    }

    m_n_payload_len = n_payload_len;
    return true;
}

//------------------------------------------------------------------------------------
/// @method extract_payload_data
/// @brief 提取数据
///
/// @param _in pBuffer 待提取数据
/// @param _in nLength 数据长度
/// @author Leiyz
/// @date 20170830
//------------------------------------------------------------------------------------
void CWebSocketParse::extract_payload_data(U8* pBuffer, const UINT nLength, CDataStreamBuffer& obj_buffer_out)
{
    U8 sz_mask[4] = { 0 };
    memcpy(sz_mask, pBuffer, sizeof(sz_mask));

    U8* p_payload_data = new(std::nothrow) U8[m_n_payload_len];
    if (nullptr == p_payload_data)
    {
        return;
    }

    for (int i = 0; i < m_n_payload_len; ++i)
    {
        p_payload_data[i] = pBuffer[i + sizeof(sz_mask)] ^ sz_mask[i%sizeof(sz_mask)];
    }

    obj_buffer_out.Write(p_payload_data, m_n_payload_len);
    delete[] p_payload_data;
}
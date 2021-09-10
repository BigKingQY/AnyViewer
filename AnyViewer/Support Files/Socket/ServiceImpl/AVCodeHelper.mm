//
//  AVCodeHelper.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/10.
//

/////////////////////////////////////////////////////////////////////
/// @file AVCodeHelper.cpp
/// @brief ±‡¬Î∏®÷˙¿‡ µœ÷
///
/// @author andy
/// @version 1.0
/// @date 2021.4.6
/////////////////////////////////////////////////////////////////////

#include "AVCodeHelper.h"
#include "FrameBuffer.h"


void FFmpegLogFunc(void* /*ptr*/, int /*level*/, const char* /*fmt*/, va_list /*vl*/)
{
    //char* szBuffer = new char[1024 * 100];

    //if (nullptr != szBuffer)
    //{
    //    sprintf_s(szBuffer, 1024 * 100, fmt, vl);

    //    LOG_ERROR("level =%d,%s", level, szBuffer);
    //    delete[]szBuffer;
    //}
}

CAVCodeHelper::CAVCodeHelper(EncodeCallback fnEncodeCallback)
    :m_fnEncodeCallback(fnEncodeCallback)
{
}

CAVCodeHelper::CAVCodeHelper(DecodeCallback fnDecodeCallback)
    : m_fnDecodeCallback(fnDecodeCallback)
{
}

CAVCodeHelper::~CAVCodeHelper()
{
    Destroy();
}



// ********************************************************************************
/// <summary>
/// µ√µΩ ‰≥ˆÕºœÒµƒX∑≈¥ÛªÚÀı–°œµ ˝
/// </summary>
/// <returns></returns>
/// <created>Andy,2021/8/20</created>
/// <changed>Andy,2021/8/20</changed>
// ********************************************************************************
double CAVCodeHelper::GetXScale()
{
    double nResult = 1.0;

    if (GetInWidth() > 0)
    {
        nResult = double(GetOutWidth()) / GetInWidth();
    }

    return nResult;
}

// ********************************************************************************
/// <summary>
/// µ√µΩ ‰≥ˆÕºœÒµƒY∑≈¥ÛªÚÀı–°œµ ˝
/// </summary>
/// <returns></returns>
/// <created>Andy,2021/8/20</created>
/// <changed>Andy,2021/8/20</changed>
// ********************************************************************************
double CAVCodeHelper::GetYScale()
{
    double nResult = 1.0;

    if (GetInHeight() > 0)
    {
        nResult = double(GetOutHeight()) / GetInHeight();
    }

    return nResult;
}

// ********************************************************************************
/// <summary>
/// ≥ı ºªØ
/// </summary>
/// <param name="nMode">π§◊˜ƒ£ Ω</param>
/// <param name="nInWidth">÷°µƒøÌ∂»</param>
/// <param name="nInHeight">÷°µƒ∏ﬂ∂»</param>
/// <param name="AV_PIX_FMT_YUV420P"></param>
/// <nReturns></nReturns>
/// <created>Andy,2021/4/6</created>
/// <changed>Andy,2021/4/6</changed>
// ********************************************************************************
bool CAVCodeHelper::Construct(
    const WORKING_MODE nMode
    , const unsigned int nInWidth
    , const unsigned int nInHeight
    , const AVPixelFormat nPixelFormat
    , const unsigned int nOutWidth
    , const unsigned int nOutHeight)
{
    std::unique_lock<std::mutex> objAutoLocker(m_objMutex);
    bool bResult = false;

    SetConstructed(false);
    SetMode(nMode);
    SetInWidth(nInWidth);
    SetInHeight(nInHeight);

    {
        //»Áπ˚ ‰≥ˆ≥ﬂ¥Á «0£¨ƒ«√¥≤…”√ ‰»Î≥ﬂ¥Á£¨»Áπ˚ «∆Ê ˝◊™ªªŒ™≈º ˝
        SetOutWidth(nOutWidth ? nOutWidth : nInWidth);
        SetOutHeight(nOutHeight ? nOutHeight : nInHeight);
        SetOutWidth(GetOutWidth() - GetOutWidth() % 2);
        SetOutHeight(GetOutHeight() - GetOutHeight() % 2);
    }
    
    LOG_ERROR("CAVCodeHelper::Construct(%d * %d)",    GetInWidth(), GetInHeight());

    av_log_set_callback(FFmpegLogFunc);
    avdevice_register_all();

    if (GetMode() == WM_ENCODE)
    {
        m_pAVCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    }
    else
    {
        m_pAVCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    }

    CHECK_POINTER_EX(m_pAVCodec, false);

    m_pAVCodecContext = avcodec_alloc_context3(m_pAVCodec);
    CHECK_POINTER_EX(m_pAVCodec, false);

    if (GetMode() == WM_ENCODE)
    {
        m_pAVCodecContext->flags |= AV_CODEC_FLAG_QSCALE;

        switch (GetImageQuality())
        {
        case CPrjSettings::IQT_DISPLAY_SPEED_PRIORITY:
            m_pAVCodecContext->bit_rate = 1000000;   // ±»Ãÿ¬ ‘Ω∏ﬂ£¨¥´ÀÕµƒ ˝æ›‘Ω¥Û,‘Ω«ÂŒ˙265ø…“‘…Ë400k£¨264–Ë“™800k,Õ¯¬Á¥´ ‰±‰µ√≤ª«ÂŒ˙¡À£¨–Ë“™º”¥ÛµΩ1500K
            m_pAVCodecContext->rc_min_rate = 500000;
            m_pAVCodecContext->rc_max_rate = 1000000;
            m_pAVCodecContext->bit_rate_tolerance = 1000000;
            break;

        default:
            m_pAVCodecContext->bit_rate = 3000000;   // ±»Ãÿ¬ ‘Ω∏ﬂ£¨¥´ÀÕµƒ ˝æ›‘Ω¥Û,‘Ω«ÂŒ˙265ø…“‘…Ë400k£¨264–Ë“™800k,Õ¯¬Á¥´ ‰±‰µ√≤ª«ÂŒ˙¡À£¨–Ë“™º”¥ÛµΩ1500K
            m_pAVCodecContext->rc_min_rate = 800000;
            m_pAVCodecContext->rc_max_rate = 3000000;
            m_pAVCodecContext->bit_rate_tolerance = 3000000;
        }



        //
        m_pAVCodecContext->time_base = { 1, 30 };
        m_pAVCodecContext->width = GetInWidth();
        m_pAVCodecContext->height = GetInHeight();

        ////√ø100÷°≤Â»Î1∏ˆI÷°£¨I÷°‘Ω…Ÿ£¨ ”∆µ‘Ω–°
        m_pAVCodecContext->gop_size = 100;

        //£®∫Ø ˝ ‰≥ˆµƒ—” ±ΩˆΩˆ∏˙max_b_framesµƒ…Ë÷√”–πÿ£¨œÎΩ¯–– µ ±±‡¬Î£¨Ω´max_b_frames…Ë÷√Œ™0±„√ª”–±‡¬Î—” ±¡À£©
        // …Ë÷√ B ÷°◊Ó¥Ûµƒ ˝¡ø£¨B÷°Œ™ ”∆µÕº∆¨ø’º‰µƒ«∞∫Û‘§≤‚÷°£¨ B ÷°œ‡∂‘”⁄ I°¢P ÷°¿¥Àµ£¨—πÀı¬ ±»Ωœ¥Û£¨≤…”√∂‡±‡¬Î B ÷°Ã·∏ﬂ«ÂŒ˙∂»
        m_pAVCodecContext->max_b_frames = 0;

        // ≤ªƒ‹≤…”√∂‡œﬂ≥Ã£¨ª·µº÷¬ª≠√Ê—”≥Ÿ
        //m_pAVCodecContext->thread_count = std::thread::hardware_concurrency();
        m_pAVCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
        m_pAVCodecContext->codec_id = m_pAVCodec->id;
        m_pAVCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;

        //av_opt_set(m_pAVCodecContext->priv_data, "x265-params", "qp=20", 0);
        av_opt_set(m_pAVCodecContext->priv_data, "preset", "superfast", 0);
        av_opt_set(m_pAVCodecContext->priv_data, "tune", "zerolatency", 0);

        int nResult = avcodec_open2(m_pAVCodecContext, m_pAVCodec, NULL);

        if (nResult >= 0)
        {
            m_pAVFrame = av_frame_alloc();
            CHECK_POINTER_EX(m_pAVFrame, false);
            m_pAVFrame->format = AV_PIX_FMT_YUV420P;
            m_pAVFrame->width = GetInWidth();
            m_pAVFrame->height = GetInHeight();

            const int nFrameSize = av_image_get_buffer_size((AVPixelFormat)m_pAVFrame->format, GetInWidth(), GetInHeight(), 32);

            if (m_objOutBuffer.GetSize() < nFrameSize)
            {
                m_objOutBuffer.Allocate(nFrameSize);
            }
            else
            {
                memset(m_objOutBuffer.GetBufferFreeHeadPtr(), 0, m_objOutBuffer.GetSize());
            }

            nResult = av_image_fill_arrays(
                m_pAVFrame->data
                , m_pAVFrame->linesize
                , m_objOutBuffer.GetBufferHeadPtr()
                , (AVPixelFormat)m_pAVFrame->format
                , GetInWidth()
                , GetInHeight()
                , 1);

            if (nResult > 0)
            {
                m_pAVPacket = av_packet_alloc();
                CHECK_POINTER_EX(m_pAVPacket, false);
                av_init_packet(m_pAVPacket);

                m_pImgConvertContext = sws_getContext(
                    m_pAVCodecContext->width
                    , m_pAVCodecContext->height
                    , nPixelFormat
                    , m_pAVCodecContext->width
                    , m_pAVCodecContext->height
                    , (AVPixelFormat)m_pAVFrame->format
                    , SWS_BICUBIC
                    , NULL
                    , NULL
                    , NULL);
                
                bResult = true;
            }
        }
        else
        {
            LOG_ERROR("Fail to run avcodec_open2 (error = %d)", nResult);
        }
    }
    else
    {
        m_pAVCodecContext->gop_size = 2;
        m_pAVCodecContext->max_b_frames = 0;
        //m_pAVCodecContext->thread_count = std::thread::hardware_concurrency();
        //av_opt_set(m_pAVCodecContext->priv_data, "x265-params", "qp=20", 0);
        av_opt_set(m_pAVCodecContext->priv_data, "preset", "superfast", 0);
        av_opt_set(m_pAVCodecContext->priv_data, "tune", "zerolatency", 0);

        int nResult = avcodec_open2(m_pAVCodecContext, m_pAVCodec, NULL);

        if (nResult >= 0)
        {
            m_pCodecParserContext = av_parser_init(m_pAVCodec->id);

            if (nullptr != m_pCodecParserContext)
            {
                const int nFrameSize = av_image_get_buffer_size(nPixelFormat, GetOutWidth(), GetOutHeight(), 32);

                if (m_objOutBuffer.GetSize() < nFrameSize)
                {
                    m_objOutBuffer.Allocate(nFrameSize);
                }
                else
                {
                    memset(m_objOutBuffer.GetBufferFreeHeadPtr(), 0, m_objOutBuffer.GetSize());
                }

                m_pAVFrame = av_frame_alloc();
                CHECK_POINTER_EX(m_pAVFrame, false);

                m_pAVPacket = av_packet_alloc();
                CHECK_POINTER_EX(m_pAVPacket, false);
                av_init_packet(m_pAVPacket);

                m_pImgConvertContext = sws_getContext(
                     GetInWidth()
                    , GetInHeight()
                    , AV_PIX_FMT_YUV420P
                    , GetOutWidth()
                    , GetOutHeight()
                    , nPixelFormat
                    , SWS_BICUBIC
                    , NULL
                    , NULL
                    , NULL);


                bResult = true;
            }
            else
            {
                LOG_ERROR("Fail to run av_parser_init");
            }
        }
        else
        {
            LOG_ERROR("Fail to run avcodec_open2 (error = %d)", nResult);
        }
    }

    SetConstructed(bResult);

    return bResult;
}

// ********************************************************************************
/// <summary>
///  Õ∑≈
/// </summary>
/// <created>Andy,2021/4/10</created>
/// <changed>Andy,2021/4/10</changed>
// ********************************************************************************
void CAVCodeHelper::Destroy()
{
    std::unique_lock<std::mutex> objAutoLocker(m_objMutex);

    SetConstructed(false);

    if (nullptr != m_pAVCodecContext)
    {
        avcodec_close(m_pAVCodecContext);
        avcodec_free_context(&m_pAVCodecContext);
        m_pAVCodecContext = nullptr;
    }

    if (nullptr != m_pAVPacket)
    {
        av_packet_free(&m_pAVPacket);
        m_pAVPacket = nullptr;
    }


    if (nullptr != m_pAVFrame)
    {
        av_frame_free(&m_pAVFrame);
        m_pAVFrame = nullptr;
    }

    if (nullptr != m_pCodecParserContext)
    {
        av_parser_close(m_pCodecParserContext);
        m_pCodecParserContext = nullptr;
    }

    if (nullptr != m_pImgConvertContext)
    {
        sws_freeContext(m_pImgConvertContext);
        m_pImgConvertContext = nullptr;
    }
}


// ********************************************************************************
/// <summary>
/// ≤È—Ø¥ÌŒÛœ˚œ¢
/// </summary>
/// <param name="nError"></param>
/// <returns></returns>
/// <created>Andy,2021/4/7</created>
/// <changed>Andy,2021/4/7</changed>
// ********************************************************************************
const TCHAR* CAVCodeHelper::QueryErrorMsg(const int nError)
{
    switch (AVUNERROR(nError))
    {
    case EAGAIN:
        return "EAGAIN";
        break;

    case AVERROR_EOF:
        return "AVERROR_EOF";
        break;

    case EINVAL:
        return "EINVAL";
        break;

    case ENOMEM:
        return "ENOMEM";
        break;
    }

    return nullptr;
}


// ********************************************************************************
/// <summary>
/// ±‡¬Î
/// </summary>
/// <param name="pFrameBuffer">÷∏œÚ–Ë“™±‡¬ÎÕºœÒ ˝æ›</param>
/// <nReturns></nReturns>
/// <created>Andy,2021/4/6</created>
/// <changed>Andy,2021/4/6</changed>
// ********************************************************************************
bool CAVCodeHelper::Encode(const CFrameBuffer* pFrameBuffer)
{
    std::unique_lock<std::mutex> objAutoLocker(m_objMutex);

    assert(GetMode() == WM_ENCODE);
    assert(nullptr != m_pAVCodecContext);
    assert(nullptr != m_pAVPacket);
    assert(nullptr != m_pAVPacket);
    assert(nullptr != m_pAVFrame);

    CHECK_POINTER_EX(pFrameBuffer, false);

    if (!GetConstructed())
    {
        return false;
    }

    if ((pFrameBuffer->m_objDimension.width != int(GetInWidth()))
        || (pFrameBuffer->m_objDimension.height != int(GetInHeight())))
    {
        LOG_ERROR("Size mismatch(%d * %d->%d * %d)",
            GetInWidth()
            , GetInHeight()
            , pFrameBuffer->m_objDimension.width
            , pFrameBuffer->m_objDimension.height);
        return false;
    }
    
    int arrLinesize[3] = { int(GetInWidth() * pFrameBuffer->GetBytesPerPixel()),0,0 };
    uint8_t* arrBGRBuffer[3] = { (uint8_t*)pFrameBuffer->GetBuffer(),nullptr,nullptr };

    const int nSliceHeight = sws_scale(
        m_pImgConvertContext
        , arrBGRBuffer
        , arrLinesize
        , 0
        , GetInHeight()
        , m_pAVFrame->data
        , m_pAVFrame->linesize);

    m_pAVFrame->pts = m_pAVFrame->pkt_dts = 0;
    /*m_pAVFrame->pts = 0;av_rescale_q_rnd(
        frameNumber
        , m_pAVCodecContext->time_base
        , avStream->time_base
        , (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX))*/
    m_pAVFrame->pkt_duration = 0;
    m_pAVFrame->pkt_pos = -1;

    bool bResult = false;
    int nResult = avcodec_send_frame(m_pAVCodecContext, m_pAVFrame);

    if (nResult >= 0)
    {
        while (nResult >= 0)
        {
            nResult = avcodec_receive_packet(m_pAVCodecContext, m_pAVPacket);
            
            if (nResult == AVERROR(EAGAIN) || nResult == AVERROR_EOF)
            {
                //LOG_ERROR("Error during encoding (err = %d)", nResult);
                break;
            }
            else if (nResult < 0)
            {
                LOG_ERROR("Error during encoding (err = %d)", nResult);
                break;
            }

            m_fnEncodeCallback(m_pAVPacket);
            av_packet_unref(m_pAVPacket);
            bResult = true;
        }
    }
    else
    {
        LOG_ERROR("Fail to send a frame for encoding (err = %d)", nResult);
    }


    return bResult;
}

// ********************************************************************************
/// <summary>
/// Ω‚¬Î
/// </summary>
/// <param name="pBuffer">÷∏œÚ±ªΩ‚¬Îµƒ ˝æ›</param>
/// <param name="nLen">±ªΩ‚¬Îµƒ ˝æ›≥£¡ø</param>
/// <nReturns></nReturns>
/// <created>Andy,2021/4/6</created>
/// <changed>Andy,2021/4/6</changed>
// ********************************************************************************
bool CAVCodeHelper::Decode(const unsigned char* pBuffer, const unsigned int nLen)
{
    std::unique_lock<std::mutex> objAutoLocker(m_objMutex);

    assert(GetMode() == WM_DECODE);
    assert(nullptr != m_pAVCodecContext);
    assert(nullptr != m_pCodecParserContext);
    assert(nullptr != m_pAVPacket);
    assert(nullptr != m_pAVFrame);
    assert(nullptr != m_pImgConvertContext);

    if (!GetConstructed())
    {
        return false;
    }

    bool bResult = false;
    //int nBytesInputed = 0;

    // “ÚŒ™ ’µΩµƒ∏’∫√ ±“ª∏ˆ ˝æ›∞¸£¨À˘“‘¥À¥¶≤ª–Ë“™∑÷¿Î ˝æ›∞¸
    m_pAVPacket->data = (uint8_t*)pBuffer;
    m_pAVPacket->size = nLen;

    int nRet = avcodec_send_packet(m_pAVCodecContext, m_pAVPacket);

    if (nRet >= 0)
    {
        do
        {
            nRet = avcodec_receive_frame(m_pAVCodecContext, m_pAVFrame);

            if (nRet == AVERROR(EAGAIN) || nRet == AVERROR_EOF)
            {
                break;
            }
            else if (nRet < 0)
            {
                LOG_ERROR("Error during decoding(error = %d)", nRet);
                break;
            }

            if (m_objOutBuffer.GetBufferHeadPtr() != nullptr)
            {
                int arrLinesize[3] = { int(GetOutWidth() * 4),0,0 };
                uint8_t* arrBGRBuffer[3] = { m_objOutBuffer.GetBufferHeadPtr(),nullptr,nullptr };

                sws_scale(m_pImgConvertContext, m_pAVFrame->data, m_pAVFrame->linesize, 0, m_pAVFrame->height, arrBGRBuffer, arrLinesize);
                m_fnDecodeCallback(m_objOutBuffer.GetBufferHeadPtr(), GetOutWidth(), GetOutHeight());
                bResult = true;
            }
            else
            {
                LOG_ERROR("Invalid buffer!");
            }

        } while (nRet >= 0);
    }
    else
    {
        LOG_ERROR("Error sending a packet for decoding (error = %s)", QueryErrorMsg(AVUNERROR(nRet)));
    }


    return bResult;
}

// ********************************************************************************
/// <summary>
/// ∏ƒ±‰ ‰≥ˆÕºœÒ≥ﬂ¥Á∫Õ∏Ò Ω
/// </summary>
/// <param name="nOutWidth"></param>
/// <param name="nOutHeight"></param>
/// <param name="nPixelFormat"></param>
/// <returns></returns>
/// <created>Andy,2021/8/20</created>
/// <changed>Andy,2021/8/20</changed>
// ********************************************************************************
bool CAVCodeHelper::ChangeOutImgProperty(
    const unsigned int nOutWidth
    , const unsigned int nOutHeight
    , const AVPixelFormat nPixelFormat/* = AV_PIX_FMT_RGB32*/)
{
    std::unique_lock<std::mutex> objAutoLocker(m_objMutex);

    SetOutWidth(nOutWidth);
    SetOutHeight(nOutHeight);

    if (nullptr != m_pImgConvertContext)
    {
        sws_freeContext(m_pImgConvertContext);
        m_pImgConvertContext = nullptr;
    }



    const int nFrameSize = av_image_get_buffer_size(nPixelFormat, GetOutWidth(), GetOutHeight(), 16);

    if (m_objOutBuffer.GetSize() < nFrameSize)
    {
        m_objOutBuffer.Allocate(nFrameSize);
    }
    else
    {
        memset(m_objOutBuffer.GetBufferFreeHeadPtr(), 0, m_objOutBuffer.GetSize());
    }

    m_pImgConvertContext = sws_getContext(
        GetInWidth()
        , GetInHeight()
        , AV_PIX_FMT_YUV420P
        , GetOutWidth()
        , GetOutHeight()
        , nPixelFormat
        , SWS_BICUBIC
        , NULL
        , NULL
        , NULL);

    return (nullptr != m_pImgConvertContext);
}

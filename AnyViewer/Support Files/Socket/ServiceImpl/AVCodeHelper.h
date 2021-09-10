//
//  AVCodeHelper.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/10.
//

#ifndef AVCodeHelper_h
#define AVCodeHelper_h

/////////////////////////////////////////////////////////////////////
/// @file AVCodeHelper.h
/// @brief ±‡¬Î∏®÷˙¿‡∂®“Â
///
/// @author andy
/// @version 1.0
/// @date 2021.4.6
/////////////////////////////////////////////////////////////////////

#pragma once
#include "BufferImpl.h"
#include "CPrjSettings.h"

extern "C"
{
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/time.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
};

class CFrameBuffer;

using EncodeCallback = std::function<bool(AVPacket*)>;
using DecodeCallback = std::function<bool(const unsigned char*, const unsigned int, const unsigned int)>;

class CAVCodeHelper
{
public:
    CAVCodeHelper(EncodeCallback fnEncodeCallback);
    CAVCodeHelper(DecodeCallback fnDecodeCallback);
    ~CAVCodeHelper();

    /// <summary>π§◊˜ƒ£ Ω</summary>
    enum WORKING_MODE
    {
        WM_ENCODE = 1,    ///< √∂æŸ≥£¡ø£¨±‡¬Îƒ£ Ω
        WM_DECODE = 2,    ///< √∂æŸ≥£¡ø£¨Ω‚¬Îƒ£ Ω
    };

public:
    // µ√µΩ ‰≥ˆÕºœÒµƒX∑≈¥ÛªÚÀı–°œµ ˝
    double GetXScale();

    // µ√µΩ ‰≥ˆÕºœÒµƒY∑≈¥ÛªÚÀı–°œµ ˝
    double GetYScale();

public:
    // ≥ı ºªØ
    bool Construct(const WORKING_MODE nMode,const unsigned int nInWidth,const unsigned int nInHeight, const AVPixelFormat nPixelFormat = AV_PIX_FMT_RGB32, const unsigned int nOutWidth = 0, const unsigned int nOutHeight = 0);

    //  Õ∑≈
    void Destroy();

    // ±‡¬Î
    bool Encode(const CFrameBuffer* pFrameBuffer);

    // Ω‚¬Î
    bool Decode(const unsigned char* pBuffer,const unsigned int nLen);

    // ∏ƒ±‰ ‰≥ˆÕºœÒ≥ﬂ¥Á∫Õ∏Ò Ω
    bool ChangeOutImgProperty(const unsigned int nOutWidth, const unsigned int nOutHeight,const AVPixelFormat nPixelFormat = AV_PIX_FMT_RGB32);
private:

    //  ≤È—Ø¥ÌŒÛœ˚œ¢
    const TCHAR* QueryErrorMsg(const int nError);

private:
    /// < π§◊˜ƒ£ Ω
    DECLARE_MEMBER_AND_METHOD_V11(WORKING_MODE, m_nMode, Mode, WM_ENCODE);

    /// <  ‰»Î÷°µƒøÌ∂»
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int,m_nInWidth, InWidth, 0);
    
    /// <  ‰»Î÷°µƒ∏ﬂ∂»
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int,m_nInHeight, InHeight, 0);

    /// <  ‰≥ˆ÷°µƒøÌ∂»
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nOutWidth, OutWidth, 0);

    /// <  ‰≥ˆ÷°µƒ∏ﬂ∂»
    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nOutHeight, OutHeight, 0);

    DECLARE_MEMBER_AND_GET_METHOD_V11(AVCodec* ,m_pAVCodec, AVCodecPtr, nullptr);

    DECLARE_MEMBER_AND_GET_METHOD_V11(AVCodecContext*, m_pAVCodecContext, AVCodecContextPtr,nullptr);

    DECLARE_MEMBER_AND_GET_METHOD_V11(AVFrame* ,m_pAVFrame, AVFramePtr,nullptr);

    DECLARE_MEMBER_AND_GET_METHOD_V11(AVPacket*, m_pAVPacket, AVPacketPtr,nullptr);

    AVCodecParserContext*    m_pCodecParserContext{ nullptr };

    CBufferImplT<BYTE>       m_objOutBuffer;

    // ±‡¬Îªÿµ˜∫Ø ˝
    EncodeCallback           m_fnEncodeCallback{ nullptr };

    // Ω‚¬Îªÿµ˜∫Ø ˝
    DecodeCallback           m_fnDecodeCallback{ nullptr };

    struct SwsContext*       m_pImgConvertContext{ nullptr };

    std::mutex               m_objMutex;

    // ≥…π¶ππ‘Ï
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bConstructed, Constructed, false);

    DECLARE_MEMBER_AND_METHOD_V11(unsigned int, m_nImageQuality, ImageQuality, CPrjSettings::IQT_IMAGE_QUALITY_PRIORITY);
};


using CAVCodeHelperPtr = std::shared_ptr<CAVCodeHelper>;


#endif /* AVCodeHelper_h */

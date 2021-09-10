//
//  FrameBuffer.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/10.
//

#ifndef FrameBuffer_h
#define FrameBuffer_h

#pragma once
#include "Typedef.h"
#include "Dimension.h"
#include "PixelFormat.h"

#include <mutex>
// FIXME: Move implementation to the .cpp file.

// --------------------------------------------------------------------------------
/// <summary>
/// ÷°ª∫≥Â«¯¿‡∂®“Â
/// </summary>
// --------------------------------------------------------------------------------
class CFrameBuffer
{
public:
    CFrameBuffer(void);
    virtual ~CFrameBuffer(void);

public:
    std::recursive_mutex& GetLock() { return m_objLock; }


public:
    virtual bool AssignProperties(const CFrameBuffer* pSrcFrameBuffer);
    virtual bool Clone(const CFrameBuffer* pSrcFrameBuffer);
    virtual void SetColor(const U8 nRed, const U8 nGreen, const U8 nBlue);
    virtual void FillRect(const CRect* pDstRect, const U32 nColor);

    // Return value: true - if equal
    //               false - if PixelFormats or size differs
    virtual bool IsEqualTo(const CFrameBuffer* pFrameBuffer);

    // Copy to self by specified destination rectangle from the specified
    // coordinates of pSrcFrameBuffer
    virtual bool CopyFrom(const CRect* pDstRect, const CFrameBuffer* pSrcFrameBuffer, const int nSrcX, const int nSrcY);

    // The same as above but destination rect is m_objDimension
    virtual bool CopyFrom(const CFrameBuffer* pSrcFrameBuffer,
        int nSrcX, int nSrcY);

    // Copy to self by specified destination rectangle from the specified
    // coordinates of pSrcFrameBuffer. When source farmebuffer and source coordinates are
    // rotated with 90 degree.
    virtual bool CopyFromRotated90(const CRect* pDstRect, const CFrameBuffer* pSrcFrameBuffer,
        const int nSrcX, const int nSrcY);

    // Copy to self by specified destination rectangle from the specified
    // coordinates of pSrcFrameBuffer. When source farmebuffer and source coordinates are
    // rotated with 180 degree.
    virtual bool CopyFromRotated180(const CRect* pDstRect, const CFrameBuffer* pSrcFrameBuffer,
        const int nSrcX, const int nSrcY);

    // Copy to self by specified destination rectangle from the specified
    // coordinates of pSrcFrameBuffer. When source farmebuffer and source coordinates are
    // rotated with 270 degree.
    virtual bool CopyFromRotated270(const CRect* pDstRect, const CFrameBuffer* pSrcFrameBuffer,const int nSrcX, const int nSrcY);

    // Overlays the source image to this with by the AND mask
    virtual bool Overlay(const CRect* pDstRect
        , const CFrameBuffer* pSrcFrameBuffer
        , const int nSrcX
        , const int nSrcY
        , const char* pAndMask);
    virtual void Move(const CRect* pDstRect, const int nSrcX, const int nSrcY);
    // Return value: true - if equal
    //               false - if PixelFormats or data differs
    virtual bool CmpFrom(const CRect* pDstRect, const CFrameBuffer* pSrcFrameBuffer,
        const int nSrcX, const int nSrcY);

    virtual bool SetDimension(const CSize* pNewDim);
    virtual bool SetDimension(const CRect* rect)
    {
        CSize dim(rect);
        return SetDimension(&dim);
    }

    // Sets dimension to the frame buffer without buffer resizing
    virtual void SetEmptyDimension(const CRect* pDimByRect);

    // Sets pixel format to the frame buffer without buffer resizing
    virtual void SetEmptyPixelFmt(const CPixelFormat* pPixelFormat);

    virtual void SetPropertiesWithoutResize(const CSize* pNewDim, const CPixelFormat* pPixelFormat);

    virtual inline CSize GetDimension() const { return m_objDimension; }

    virtual bool SetPixelFormat(const CPixelFormat* pPixelFormat);
    virtual inline CPixelFormat GetPixelFormat() const { return m_objPixelFormat; }

    // This function set both CPixelFormat and CSize
    virtual bool SetProperties(const CSize& refFBDimension, const CPixelFormat& refPixelFormat);
    virtual bool SetProperties(const CRect* pDimByRect, const CPixelFormat* pPixelFormat);

    // Return the number of bits occupied by one pixel (can be 8, 16 or 32).
    virtual U8 GetBitsPerPixel() const;

    // Return the number of bytes occupied by one pixel (can be 1, 2 or 4).
    virtual U8 GetBytesPerPixel() const;

    virtual void SetBuffer(void* newBuffer) { m_pBuffer = newBuffer; }
    virtual inline void* GetBuffer() const { return m_pBuffer; }

    // Return a pointer to the pixel data specified by the coordinates of that
    // pixel. GetBufferPtr(0, 0) should be equivalent to GetBuffer(). This
    // function does not check if the coordinates are within the frame buffer
    // boundaries.
    virtual void* GetBufferPtr(const int x, const int y) const;

    virtual inline int GetBufferSize() const;

    virtual inline int GetBytesPerRow() const
    {
        return m_objDimension.width *
            m_objPixelFormat.nBitsPerPixel / 8;
    }

public:
    //±£¥Êµ±«∞µƒ÷° ˝æ›µΩ¥≈≈ÃŒƒº˛
    bool SaveBitmap(const TCHAR* lpPath);

protected:
    bool ResizeBuffer();

    void ClipRect(const CRect* pDstRect, const CFrameBuffer* pSrcFrameBuffer,
        const int nSrcX, const int nSrcY,
                  CRect* pDstClippedRect, CRect* pSrcClippedRect);

    void ClipRect(const CRect* pDstRect, const CRect* srcBufferRect,
        const int nSrcX, const int nSrcY,
                  CRect* pDstClippedRect, CRect* pSrcClippedRect);

    template<class PIXEL_T> bool OverlayT(
        const CRect* pDstRect
        , const CFrameBuffer* pSrcFrameBuffer
        , const int nSrcX
        , const int nSrcY
        , const char* pAndMask);

public:
    CSize                    m_objDimension;
    CPixelFormat             m_objPixelFormat;

private:
    void*                    m_pBuffer{ nullptr };
    mutable std::recursive_mutex     m_objLock;
};



#endif /* FrameBuffer_h */

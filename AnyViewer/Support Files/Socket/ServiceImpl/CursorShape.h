//
//  CursorShape.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/10.
//

#ifndef CursorShape_h
#define CursorShape_h

#pragma once

#include "FrameBuffer.h"
#include "Point.h"

// --------------------------------------------------------------------------------
/// <summary>
/// π‚±Í–Œ◊¥–≈œ¢¿‡∂®“Â
/// </summary>
// --------------------------------------------------------------------------------
class CCursorShape
{
public:
    CCursorShape() : m_arrMask(0) {}
    ~CCursorShape();

    bool Clone(const CCursorShape *pSrcCursorShape);

    bool SetDimension(const CSize *pNewDim);
    CSize GetDimension() const { return m_objPixels.GetDimension(); }

    bool SetPixelFormat(const CPixelFormat *pPixelFormat);
    CPixelFormat GetPixelFormat() const { return m_objPixels.GetPixelFormat(); }

    // This function set both CPixelFormat and CSize
    bool SetProperties(const CSize *pNewDim, const CPixelFormat *pPixelFormat);

    const CFrameBuffer *getPixels() const { return &m_objPixels; }
    int getPixelsSize() const { return m_objPixels.GetBufferSize(); }
    const char *GetMask() const { return m_arrMask.empty() ? 0 : &m_arrMask.front(); }
    void assignMaskFromRfb(const char *pSrcMask);
    void assignMaskFromWindows(const char *pSrcMask);
    int GetMaskSize() const;
    int getMaskWidthInBytes() const;

    void setHotSpot(int x, int y) { m_ptHotSpot.x = x; m_ptHotSpot.y = y; }
    CPoint GetHotSpot() const { return m_ptHotSpot; }

    // Resets the cursor shape to empty state (zero dimension and hot spot
    // and empty masks). But the pixel format saves the same.
    void resetToEmpty();

private:
    bool ResizeBuffer();

private:
    CFrameBuffer               m_objPixels;
    std::vector<char>          m_arrMask;
    CPoint                      m_ptHotSpot;
};



#endif /* CursorShape_h */

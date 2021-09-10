//
//  CursorShape.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/10.
//

#include "CursorShape.h"

CCursorShape::~CCursorShape()
{
}

bool CCursorShape::Clone(const CCursorShape *pSrcCursorShape)
{
    CHECK_POINTER_EX(pSrcCursorShape,false);

    m_ptHotSpot = pSrcCursorShape->GetHotSpot();

    if (m_objPixels.Clone(pSrcCursorShape->getPixels()))
    {
        m_arrMask = pSrcCursorShape->m_arrMask;
        return true;
    }
    else
    {
        return false;
    }
}

void CCursorShape::assignMaskFromRfb(const char *pSrcMask)
{
    CHECK_POINTER(pSrcMask);

    if (!m_arrMask.empty())
    {
        const int nHeight = m_objPixels.GetDimension().height;
        const size_t nRFBWidthInBytes = getMaskWidthInBytes();
        const size_t nMaskLen = nHeight * nRFBWidthInBytes;

        memcpy(&m_arrMask.front(), pSrcMask, nMaskLen);
    }
}

void CCursorShape::assignMaskFromWindows(const char *pSrcMask)
{
    CHECK_POINTER(pSrcMask);

    if (!m_arrMask.empty())
    {
        const int nHeight = m_objPixels.GetDimension().height;
        const int nWinWidthInBytes = ((m_objPixels.GetDimension().width + 15) / 16) * 2;
        const int nRFBWidthInBytes = getMaskWidthInBytes();
        
        for (int i = 0; i < nHeight; i++)
        {
            memcpy(&m_arrMask[i * nRFBWidthInBytes],
                &pSrcMask[i * nWinWidthInBytes],
                nRFBWidthInBytes);
        }
    }
}

bool CCursorShape::SetDimension(const CSize *pNewDim)
{
    CHECK_POINTER_EX(pNewDim,false);

    const bool bResult = m_objPixels.SetDimension(pNewDim);

    return bResult && ResizeBuffer();
}

bool CCursorShape::SetPixelFormat(const CPixelFormat *pixFormat)
{
    const bool bResult = m_objPixels.SetPixelFormat(pixFormat);

    return bResult && ResizeBuffer();
}

bool CCursorShape::SetProperties(
    const CSize *pNewDim,
    const CPixelFormat *pPixelFormat)
{
    CHECK_POINTER_EX(pNewDim, false);
    CHECK_POINTER_EX(pPixelFormat, false);

    const bool bResult = m_objPixels.SetDimension(pNewDim) &&
        m_objPixels.SetPixelFormat(pPixelFormat);

    return bResult && ResizeBuffer();
}

void CCursorShape::resetToEmpty()
{
    CSize objSize(0, 0);
    SetDimension(&objSize);
    setHotSpot(0, 0);
}

bool CCursorShape::ResizeBuffer()
{
    m_arrMask.resize(GetMaskSize());

    return true;
}

int CCursorShape::GetMaskSize() const
{
    return getMaskWidthInBytes() * m_objPixels.GetDimension().height;
}

int CCursorShape::getMaskWidthInBytes() const
{
    return (m_objPixels.GetDimension().width + 7) / 8;
}

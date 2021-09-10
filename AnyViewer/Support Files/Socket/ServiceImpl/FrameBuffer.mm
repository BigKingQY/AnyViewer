//
//  FrameBuffer.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/10.
//

#include "FrameBuffer.h"
#include <string.h>

CFrameBuffer::CFrameBuffer(void)
{
    memset(&m_objPixelFormat, 0, sizeof(m_objPixelFormat));
}

CFrameBuffer::~CFrameBuffer(void)
{
    if (nullptr != m_pBuffer)
    {
        delete[]m_pBuffer;
    }
}

// ********************************************************************************
/// <summary>
///
/// </summary>
/// <param name="pSrcFrameBuffer"></param>
/// <returns></returns>
/// <created>Andy,2020/9/28</created>
/// <changed>Andy,2020/9/28</changed>
// ********************************************************************************
bool CFrameBuffer::AssignProperties(const CFrameBuffer* pSrcFrameBuffer)
{
    return SetProperties(pSrcFrameBuffer->GetDimension(), pSrcFrameBuffer->GetPixelFormat());
}

bool CFrameBuffer::Clone(const CFrameBuffer* pSrcFrameBuffer)
{
    if (!AssignProperties(pSrcFrameBuffer))
    {
        return false;
    }
    
    CRect objDimensionRect = m_objDimension.GetRect();
    
    const CRect objRect = &objDimensionRect;

    CopyFrom(&objRect, pSrcFrameBuffer, objRect.left, objRect.top);

    return true;
}

void CFrameBuffer::SetColor(const U8 nRed, const U8 nGreen, const U8 nBlue)
{
    std::unique_lock<std::recursive_mutex> objAutoLocker(m_objLock);

    const size_t nSizeInPixels = m_objDimension.Area();
    const int nPixelSize = m_objPixelFormat.nBitsPerPixel / 8;
    const U32 nRedPix = (nRed * m_objPixelFormat.redMax / 255) << m_objPixelFormat.redShift;
    const U32 nGreenPix = (nGreen * m_objPixelFormat.greenMax / 255) << m_objPixelFormat.greenShift;
    const U32 nBluePix = (nBlue * m_objPixelFormat.blueMax / 255) << m_objPixelFormat.blueShift;
    const U32 nColor = nRedPix | nGreenPix | nBluePix;
    const U8* pEndPixPtr = (U8*)m_pBuffer + GetBufferSize();
    U8* pPixPtr = (U8*)m_pBuffer;

    assert(nPixelSize > 0);

    if (nPixelSize > 0)
    {
        for (; pPixPtr <= pEndPixPtr - nPixelSize; pPixPtr += nPixelSize)
        {
            memcpy(pPixPtr, &nColor, nPixelSize);
        }
    }
}

// ********************************************************************************
/// <summary>
///
/// </summary>
/// <param name="pDstRect"></param>
/// <param name="nColor"></param>
/// <created>Andy,2020/12/8</created>
/// <changed>Andy,2020/12/8</changed>
// ********************************************************************************
void CFrameBuffer::FillRect(const CRect* pDstRect, const U32 nColor)
{
    const CRect objClipRect = m_objDimension.GetRect().Intersection(pDstRect);
    const int nPixelSize = GetBytesPerPixel();
    const size_t nSizeLineFb = GetBytesPerRow();
    const size_t nSizeLineRect = objClipRect.getWidth() * nPixelSize;

    // it's pointer to first line of rect
    U8* pSrcLinePtr = reinterpret_cast<U8*>(GetBufferPtr(objClipRect.left, objClipRect.top));

    // it's pointer to pixel in first line
    U8* pPixPtr = pSrcLinePtr;

    for (int x = objClipRect.left; x < objClipRect.right; x++, pPixPtr += nPixelSize)
    {
        memcpy(pPixPtr, &nColor, nPixelSize);
    }

    // it's pointer to next line of rect
    U8* pDstLinePtr = pSrcLinePtr + nSizeLineFb;

    for (int y = objClipRect.top + 1; y < objClipRect.bottom; y++, pDstLinePtr += nSizeLineFb)
    {
        memcpy(pDstLinePtr, pSrcLinePtr, nSizeLineRect);
    }
}

bool CFrameBuffer::IsEqualTo(const CFrameBuffer* pFrameBuffer)
{
    CSize objSize           = pFrameBuffer->GetDimension();
    CPixelFormat objFormat  = pFrameBuffer->GetPixelFormat();
    
    return m_objDimension.cmpDim(&objSize) &&
        m_objPixelFormat.IsEqualTo(&objFormat);
}

// ********************************************************************************
/// <summary>
///
/// </summary>
/// <param name="pDstRect"></param>
/// <param name="pSrcFrameBuffer"></param>
/// <param name="nSrcX"></param>
/// <param name="nSrcY"></param>
/// <param name="objDstClippedRect"></param>
/// <param name="objSrcClippedRect"></param>
/// <created>Andy,2020/9/28</created>
/// <changed>Andy,2020/9/28</changed>
// ********************************************************************************
void CFrameBuffer::ClipRect(
    const CRect* pDstRect
    , const CFrameBuffer* pSrcFrameBuffer
    , const int nSrcX
    , const int nSrcY
    , CRect* objDstClippedRect
    , CRect* objSrcClippedRect)
{
    CRect srcBufferRect = pSrcFrameBuffer->GetDimension().GetRect();

    ClipRect(pDstRect, &srcBufferRect, nSrcX, nSrcY, objDstClippedRect, objSrcClippedRect);
}

// ********************************************************************************
/// <summary>
///
/// </summary>
/// <param name="pDstRect"></param>
/// <param name="srcBufferRect"></param>
/// <param name="nSrcX"></param>
/// <param name="nSrcY"></param>
/// <param name="objDstClippedRect"></param>
/// <param name="objSrcClippedRect"></param>
/// <created>Andy,2020/9/28</created>
/// <changed>Andy,2020/9/28</changed>
// ********************************************************************************
void CFrameBuffer::ClipRect(
    const CRect* pDstRect
    , const CRect* srcBufferRect
    , const int nSrcX
    , const int nSrcY
    , CRect* objDstClippedRect
    , CRect* objSrcClippedRect)
{

    CRect objDstBufferRect = m_objDimension.GetRect();

    // Building objSrcRect
    CRect objSrcRect(nSrcX, nSrcY, nSrcX + pDstRect->getWidth(), nSrcY + pDstRect->getHeight());

    // Finding common area between the pDstRect, objSrcRect and the FrameBuffers
    CRect objDstRectFB = objDstBufferRect.Intersection(pDstRect);
    CRect objSrcRectFB = srcBufferRect->Intersection(&objSrcRect);

    // Finding common area between the objDstRectFB and the objSrcRectFB
    CRect objDstCommonArea(&objDstRectFB);
    CRect objSrcCommonArea(&objSrcRectFB);

    // Move to common place (left = 0, top = 0)
    objDstCommonArea.move(-pDstRect->left, -pDstRect->top);
    objSrcCommonArea.move(-objSrcRect.left, -objSrcRect.top);
    
    CRect objAreaRect(&objSrcCommonArea);

    CRect objDstAreaRect = objDstCommonArea.Intersection(&objAreaRect);
    
    CRect objCommonRect = &objDstAreaRect;

    // Moving objCommonRect to destination coordinates and pSrc
    objDstClippedRect->setCRect(&objCommonRect);
    objDstClippedRect->move(pDstRect->left, pDstRect->top);

    objSrcClippedRect->setCRect(&objCommonRect);
    objSrcClippedRect->move(objSrcRect.left, objSrcRect.top);
}

bool CFrameBuffer::Overlay(
    const CRect* pDstRect
    , const CFrameBuffer* pSrcFrameBuffer
    , const int nSrcX
    , const int nSrcY
    , const char* pAndMask)
{
    CPixelFormat objFormat = pSrcFrameBuffer->GetPixelFormat();
    
    if (!m_objPixelFormat.IsEqualTo(&objFormat))
    {
        return false;
    }

    if (m_objPixelFormat.nBitsPerPixel == 32)
    {
        return OverlayT<U32>(pDstRect, pSrcFrameBuffer, nSrcX, nSrcY, pAndMask);
    }
    else if (m_objPixelFormat.nBitsPerPixel == 16)
    {
        return OverlayT<U16>(pDstRect, pSrcFrameBuffer, nSrcX, nSrcY, pAndMask);
    }
    else if (m_objPixelFormat.nBitsPerPixel == 8)
    {
        return OverlayT<U8>(pDstRect, pSrcFrameBuffer, nSrcX, nSrcY, pAndMask);
    }
    else
    {
        assert(false);
    }

    return false;
}

template<class PIXEL_T> bool CFrameBuffer::OverlayT(
    const CRect* pDstRect
    , const CFrameBuffer* pSrcFrameBuffer
    , const int nSrcX
    , const int nSrcY
    , const char* pAndMask)
{
    CRect objSrcClippedRect;
    CRect objDstClippedRect;

    ClipRect(pDstRect, pSrcFrameBuffer, nSrcX, nSrcY, &objDstClippedRect, &objSrcClippedRect);

    if (objDstClippedRect.Area() <= 0 || objSrcClippedRect.Area() <= 0)
    {
        return true;
    }

    PIXEL_T* pDstPixels = (PIXEL_T*)GetBuffer();
    PIXEL_T* pSrcPixels = (PIXEL_T*)pSrcFrameBuffer->GetBuffer();
    const int nSrcWidth = pSrcFrameBuffer->GetDimension().width;
    const int nDstWidth = GetDimension().width;
    const size_t nBytesPerRow = (nSrcWidth + 7) / 8;

    for (int iRow = objSrcClippedRect.top; iRow < objSrcClippedRect.bottom; iRow++)
    {
        for (int iCol = objSrcClippedRect.left; iCol < objSrcClippedRect.right; iCol++)
        {
            const unsigned char nCurByte = pAndMask[iRow * nBytesPerRow + iCol / 8];
            const bool bAndBit = (nCurByte & 128 >> iCol % 8) != 0;

            if (bAndBit)
            {
                const int iDstRow = objDstClippedRect.top + iRow - nSrcY - objSrcClippedRect.top;
                const int iDstCol = objDstClippedRect.left + iCol - nSrcX - objSrcClippedRect.left;

                pDstPixels[iDstRow * nDstWidth + iDstCol] = pSrcPixels[iRow * nSrcWidth + iCol];
            }
        }
    }
    return true;
}

// ********************************************************************************
/// <summary>
/// øΩ±¥ ˝æ›
/// </summary>
/// <param name="pDstRect">ƒø±Í«¯”Ú</param>
/// <param name="pSrcFrameBuffer">÷ªœÎ‘¥ª∫≥Â«¯∂‘œÛµƒ÷∏’Î</param>
/// <param name="nSrcX">‘¥X◊¯±Í</param>
/// <param name="nSrcY">‘¥Y◊¯±Í</param>
/// <returns>true:≥…π¶£¨false: ß∞‹</returns>
/// <created>Andy,2020/9/28</created>
/// <changed>Andy,2020/9/28</changed>
// ********************************************************************************
bool CFrameBuffer::CopyFrom(
    const CRect* pDstRect
    , const CFrameBuffer* pSrcFrameBuffer
    , const int nSrcX
    , const int nSrcY)
{
    std::unique_lock<std::recursive_mutex> objAutoLocker(m_objLock);
    
    CPixelFormat objFormat = pSrcFrameBuffer->GetPixelFormat();
    
    if (!m_objPixelFormat.IsEqualTo(&objFormat))
    {
        return false;
    }

    CRect objSrcClippedRect;
    CRect objDstClippedRect;

    ClipRect(pDstRect, pSrcFrameBuffer, nSrcX, nSrcY, &objDstClippedRect, &objSrcClippedRect);

    if (objDstClippedRect.Area() <= 0 || objSrcClippedRect.Area() <= 0)
    {
        return true;
    }

    // Shortcuts
    const int nPixelSize = m_objPixelFormat.nBitsPerPixel / 8;
    const int nDstStrideBytes = m_objDimension.width * nPixelSize;
    const int nSrcStrideBytes = pSrcFrameBuffer->GetDimension().width * nPixelSize;
    const int nResultHeight = objDstClippedRect.getHeight();
    const int nResultWidthBytes = objDstClippedRect.getWidth() * nPixelSize;

    U8* pDst = (U8*)m_pBuffer
        + objDstClippedRect.top * nDstStrideBytes
        + nPixelSize * objDstClippedRect.left;

    U8* pSrc = (U8*)pSrcFrameBuffer->GetBuffer()
        + objSrcClippedRect.top * nSrcStrideBytes
        + nPixelSize * objSrcClippedRect.left;

    for (int i = 0; i < nResultHeight; i++)
    {
        memcpy(pDst, pSrc, nResultWidthBytes);
        pDst += nDstStrideBytes;
        pSrc += nSrcStrideBytes;
    }

    return true;
}

bool CFrameBuffer::CopyFrom(const CFrameBuffer* pSrcFrameBuffer,
    int nSrcX, int nSrcY)
{
    CRect objRect = m_objDimension.GetRect();
    return CopyFrom(&objRect, pSrcFrameBuffer, nSrcX, nSrcY);
}

// ********************************************************************************
/// <summary>
///
/// </summary>
/// <param name="pDstRect"></param>
/// <param name="pSrcFrameBuffer"></param>
/// <param name="nSrcX"></param>
/// <param name="nSrcY"></param>
/// <returns></returns>
/// <created>Andy,2021/3/2</created>
/// <changed>Andy,2021/3/2</changed>
// ********************************************************************************
bool CFrameBuffer::CopyFromRotated90(
    const CRect* pDstRect
    , const CFrameBuffer* pSrcFrameBuffer
    , const int nSrcX
    , const int nSrcY)
{
    CPixelFormat objFormat = pSrcFrameBuffer->GetPixelFormat();
    
    if (m_objPixelFormat.nBitsPerPixel != 32 || !m_objPixelFormat.IsEqualTo(&objFormat))
    {
        return false;
    }

    // Shortcuts
    const int nPixelSize = m_objPixelFormat.nBitsPerPixel / 8;
    const int nDstStrideBytesByX = m_objDimension.width * nPixelSize;
    const int nSrcStrideBytes = pSrcFrameBuffer->GetDimension().width * nPixelSize;
    CRect objSrcClippedRect;
    CRect objDstClippedRect;
    CSize objSrcBuffTransposedDim = pSrcFrameBuffer->GetDimension().getTransposition();
    CRect objSrcBuffTransposedRect = objSrcBuffTransposedDim.GetRect();
    CRect objSrcRotatedCoordinates(nSrcX, nSrcY, nSrcX + pDstRect->getHeight(), nSrcY + pDstRect->getWidth());
    
    objSrcRotatedCoordinates.rotateOn90InsideDimension(pSrcFrameBuffer->GetDimension().height);
    
    const int nSrcXinDstRotation = objSrcRotatedCoordinates.left;
    const int nSrcYinDstRotation = objSrcRotatedCoordinates.top;

    ClipRect(pDstRect, &objSrcBuffTransposedRect, nSrcXinDstRotation, nSrcYinDstRotation, &objDstClippedRect, &objSrcClippedRect);

    if (objDstClippedRect.Area() <= 0 || objSrcClippedRect.Area() <= 0)
    {
        return true;
    }
    // Rotate pSrc rect back in pSrc rotation.
    objSrcClippedRect.rotateOn270InsideDimension(objSrcBuffTransposedDim.width);

    int nResultHeight = objSrcClippedRect.getHeight();
    int nResultWidth = objSrcClippedRect.getWidth();

    U8* pBaseDst = (U8*)m_pBuffer
        + objDstClippedRect.top * nDstStrideBytesByX
        + nPixelSize * (objDstClippedRect.right - 1);

    U8* pBaseSrc = (U8*)pSrcFrameBuffer->GetBuffer()
        + objSrcClippedRect.top * nSrcStrideBytes
        + nPixelSize * objSrcClippedRect.left;

    for (int iRow = 0; iRow < nResultHeight; iRow++)
    {
        U32* pSrc = (U32*)pBaseSrc;
        U8* pDst = pBaseDst;

        for (int iCol = 0; iCol < nResultWidth; iCol++)
        {
            *(U32*)pDst = *pSrc;
            pSrc++;
            pDst += nDstStrideBytesByX;
        }

        pBaseDst -= nPixelSize;
        pBaseSrc += nSrcStrideBytes;
    }

    return true;
}

bool CFrameBuffer::CopyFromRotated180(
    const CRect* pDstRect
    , const CFrameBuffer* pSrcFrameBuffer
    , const int nSrcX
    , const int nSrcY)
{
    CPixelFormat objFormat = pSrcFrameBuffer->GetPixelFormat();

    if (m_objPixelFormat.nBitsPerPixel != 32 || !m_objPixelFormat.IsEqualTo(&objFormat))
    {
        return false;
    }

    // Shortcuts
    int nPixelSize = m_objPixelFormat.nBitsPerPixel / 8;
    int nDstStrideBytesByX = m_objDimension.width * nPixelSize;
    int nSrcStrideBytes = pSrcFrameBuffer->GetDimension().width * nPixelSize;

    CRect objSrcClippedRect, objDstClippedRect;

    CSize objSrcBuffTransposedDim = pSrcFrameBuffer->GetDimension();
    CRect objSrcBuffTransposedRect = objSrcBuffTransposedDim.GetRect();
    CRect objSrcRotatedCoordinates(nSrcX, nSrcY, nSrcX + pDstRect->getWidth(), nSrcY + pDstRect->getHeight());
    
    objSrcRotatedCoordinates.rotateOn180InsideDimension(
        pSrcFrameBuffer->GetDimension().width,
        pSrcFrameBuffer->GetDimension().height);

    int nSrcXinDstRotation = objSrcRotatedCoordinates.left;
    int nSrcYinDstRotation = objSrcRotatedCoordinates.top;

    ClipRect(pDstRect, &objSrcBuffTransposedRect, nSrcXinDstRotation, nSrcYinDstRotation, &objDstClippedRect, &objSrcClippedRect);

    if (objDstClippedRect.Area() <= 0 || objSrcClippedRect.Area() <= 0)
    {
        return true;
    }
    // Rotate pSrc rect back in pSrc rotation.
    objSrcClippedRect.rotateOn180InsideDimension(pSrcFrameBuffer->GetDimension().width,
        pSrcFrameBuffer->GetDimension().height);

    int nResultHeight = objSrcClippedRect.getHeight();
    int nResultWidth = objSrcClippedRect.getWidth();

    U8* pBaseDst = (U8*)m_pBuffer
        + (objDstClippedRect.bottom - 1) * nDstStrideBytesByX
        + nPixelSize * (objDstClippedRect.right - 1);

    U8* pBaseSrc = (U8*)pSrcFrameBuffer->GetBuffer()
        + objSrcClippedRect.top * nSrcStrideBytes
        + nPixelSize * objSrcClippedRect.left;

    for (int iRow = 0; iRow < nResultHeight; iRow++)
    {
        U32* pSrc = (U32*)pBaseSrc;
        U32* pDst = (U32*)pBaseDst;

        for (int iCol = 0; iCol < nResultWidth; iCol++)
        {
            *(U32*)pDst = *pSrc;
            pSrc++;
            pDst--;
        }

        pBaseDst -= nDstStrideBytesByX;
        pBaseSrc += nSrcStrideBytes;
    }

    return true;
}

bool CFrameBuffer::CopyFromRotated270(
    const CRect* pDstRect
    , const CFrameBuffer* pSrcFrameBuffer
    , const int nSrcX
    , const int nSrcY)
{
    CPixelFormat objFormat = pSrcFrameBuffer->GetPixelFormat();

    if (m_objPixelFormat.nBitsPerPixel != 32 || !m_objPixelFormat.IsEqualTo(&objFormat))
    {
        return false;
    }

    // Shortcuts
    const int nPixelSize = m_objPixelFormat.nBitsPerPixel / 8;
    const int nDstStrideBytesByX = m_objDimension.width * nPixelSize;
    const int nSrcStrideBytes = pSrcFrameBuffer->GetDimension().width * nPixelSize;

    CRect objSrcClippedRect;
    CRect objDstClippedRect;
    CSize objSrcBuffTransposedDim = pSrcFrameBuffer->GetDimension().getTransposition();
    CRect objSrcBuffTransposedRect = objSrcBuffTransposedDim.GetRect();
    CRect objSrcRotatedCoordinates(nSrcX, nSrcY, nSrcX + pDstRect->getHeight(), nSrcY + pDstRect->getWidth());
    
    objSrcRotatedCoordinates.rotateOn270InsideDimension(pSrcFrameBuffer->GetDimension().width);

    const int nSrcXinDstRotation = objSrcRotatedCoordinates.left;
    const int nSrcYinDstRotation = objSrcRotatedCoordinates.top;

    ClipRect(pDstRect, &objSrcBuffTransposedRect, nSrcXinDstRotation, nSrcYinDstRotation, &objDstClippedRect, &objSrcClippedRect);

    if (objDstClippedRect.Area() <= 0 || objSrcClippedRect.Area() <= 0)
    {
        return true;
    }
    // Rotate pSrc rect back in pSrc rotation.
    objSrcClippedRect.rotateOn90InsideDimension(objSrcBuffTransposedDim.height);

    int nResultHeight = objSrcClippedRect.getHeight();
    int nResultWidth = objSrcClippedRect.getWidth();

    U8* pBaseDst = (U8*)m_pBuffer
        + (objDstClippedRect.bottom - 1) * nDstStrideBytesByX
        + nPixelSize * objDstClippedRect.left;

    U8* pBaseSrc = (U8*)pSrcFrameBuffer->GetBuffer()
        + objSrcClippedRect.top * nSrcStrideBytes
        + nPixelSize * objSrcClippedRect.left;

    for (int iRow = 0; iRow < nResultHeight; iRow++)
    {
        U32* pSrc = (U32*)pBaseSrc;
        U8* pDst = pBaseDst;

        for (int iCol = 0; iCol < nResultWidth; iCol++)
        {
            *(U32*)pDst = *pSrc;
            pSrc++;
            pDst -= nDstStrideBytesByX;
        }

        pBaseDst += nPixelSize;
        pBaseSrc += nSrcStrideBytes;
    }

    return true;
}

bool CFrameBuffer::CmpFrom(
    const CRect* pDstRect
    , const CFrameBuffer* pSrcFrameBuffer
    , const int nSrcX
    , const int nSrcY)
{
    CPixelFormat objFormat = pSrcFrameBuffer->GetPixelFormat();

    if (!m_objPixelFormat.IsEqualTo(&objFormat))
    {
        return false;
    }

    CRect objSrcClippedRect;
    CRect objDstClippedRect;

    ClipRect(pDstRect, pSrcFrameBuffer, nSrcX, nSrcY, &objDstClippedRect, &objSrcClippedRect);

    if (objDstClippedRect.Area() <= 0 || objSrcClippedRect.Area() <= 0)
    {
        return true;
    }

    // Shortcuts
    int nPixelSize = m_objPixelFormat.nBitsPerPixel / 8;
    int nDstStrideBytes = m_objDimension.width * nPixelSize;
    int nSrcStrideBytes = pSrcFrameBuffer->GetDimension().width * nPixelSize;
    int nResultHeight = objDstClippedRect.getHeight();
    int nResultWidthBytes = objDstClippedRect.getWidth() * nPixelSize;

    U8* pDst = (U8*)m_pBuffer
        + objDstClippedRect.top * nDstStrideBytes
        + nPixelSize * objDstClippedRect.left;

    U8* pSrc = (U8*)pSrcFrameBuffer->GetBuffer()
        + objSrcClippedRect.top * nSrcStrideBytes
        + nPixelSize * objSrcClippedRect.left;

    for (int i = 0; i < nResultHeight; i++)
    {
        if (memcmp(pDst, pSrc, nResultWidthBytes) != 0)
        {
            return false;
        }

        pDst += nDstStrideBytes;
        pSrc += nSrcStrideBytes;
    }

    return true;
}

void CFrameBuffer::Move(const CRect* pDstRect, const int nSrcX, const int nSrcY)
{
    CRect objSrcClippedRect;
    CRect objDstClippedRect;

    ClipRect(pDstRect, this, nSrcX, nSrcY, &objDstClippedRect, &objSrcClippedRect);

    if (objDstClippedRect.Area() <= 0 || objSrcClippedRect.Area() <= 0)
    {
        return;
    }

    // Data copy
    int nPixelSize = m_objPixelFormat.nBitsPerPixel / 8;
    int nStrideBytes = m_objDimension.width * nPixelSize;

    const int nResultHeight = objDstClippedRect.getHeight();
    const int nResultWidthBytes = objDstClippedRect.getWidth() * nPixelSize;

    if (nSrcY > pDstRect->top)
    {
        // Pointers set to first string of the rectanles
        U8*  pDst = (U8*)m_pBuffer + objDstClippedRect.top * nStrideBytes
            + nPixelSize * objDstClippedRect.left;
        U8*  pSrc = (U8*)m_pBuffer + objSrcClippedRect.top * nStrideBytes
            + nPixelSize * objSrcClippedRect.left;

        for (int i = 0; i < nResultHeight; i++, pDst += nStrideBytes, pSrc += nStrideBytes)
        {
            memcpy(pDst, pSrc, nResultWidthBytes);
        }

    }
    else
    {
        // Pointers set to last string of the rectanles
        U8*  pDst = (U8*)m_pBuffer + (objDstClippedRect.bottom - 1) * nStrideBytes
            + nPixelSize * objDstClippedRect.left;
        U8* pSrc = (U8*)m_pBuffer + (objSrcClippedRect.bottom - 1) * nStrideBytes
            + nPixelSize * objSrcClippedRect.left;

        for (int i = nResultHeight - 1; i >= 0; i--, pDst -= nStrideBytes, pSrc -= nStrideBytes)
        {
            memmove(pDst, pSrc, nResultWidthBytes);
        }
    }
}

bool CFrameBuffer::SetPixelFormat(const CPixelFormat* pPixelFormat)
{
    std::unique_lock<std::recursive_mutex> objAutoLocker(m_objLock);

    m_objPixelFormat = *pPixelFormat;

    return ResizeBuffer();
}

bool CFrameBuffer::SetDimension(const CSize* pNewDim)
{
    std::unique_lock<std::recursive_mutex> objAutoLocker(m_objLock);
    bool bResult = false;

    if (m_objDimension != *pNewDim)
    {
        // œ¬√Êµƒ¡Ω––¥˙¬Î≤ªƒ‹ΩªªªÀ≥–Ú£¨“ÚŒ™GetBufferSizeª·”√µΩ◊Ó–¬µƒ≥ﬂ¥Á
        m_objDimension = *pNewDim;
        bResult = ResizeBuffer();
    }

    return bResult;
}

void CFrameBuffer::SetEmptyDimension(const CRect* pDimByRect)
{
    m_objDimension.SetDim(pDimByRect);
}

void CFrameBuffer::SetEmptyPixelFmt(const CPixelFormat* pPixelFormat)
{
    m_objPixelFormat = *pPixelFormat;
}

void CFrameBuffer::SetPropertiesWithoutResize(const CSize* pNewDim, const CPixelFormat* pPixelFormat)
{
    m_objDimension = *pNewDim;
    m_objPixelFormat = *pPixelFormat;
}

// ********************************************************************************
/// <summary>
/// …Ë÷√÷°ª∫≥Â«¯µƒ∏Ò Ω£¨÷˜“™∞¸¿®£∫≥ﬂ¥Á∫ÕœÒÀÿ∏Ò Ω
/// </summary>
/// <param name="pNewDim">–¬µƒ≥ﬂ¥Á</param>
/// <param name="pPixelFormat">œÒÀÿ∏Ò Ω</param>
/// <returns>true:≥…π¶,false: ß∞‹</returns>
/// <created>Andy,2020/9/28</created>
/// <changed>Andy,2020/9/28</changed>
// ********************************************************************************
bool CFrameBuffer::SetProperties(const CSize& refFBDimension, const CPixelFormat& refPixelFormat)
{
    std::unique_lock<std::recursive_mutex> objAutoLocker(m_objLock);

    m_objDimension = refFBDimension;
    m_objPixelFormat = refPixelFormat;

    return ResizeBuffer();
}

// ********************************************************************************
/// <summary>
/// …Ë÷√÷°ª∫≥Â«¯µƒ∏Ò Ω£¨÷˜“™∞¸¿®£∫≥ﬂ¥Á∫ÕœÒÀÿ∏Ò Ω
/// </summary>
/// <param name="pDimByRect">–¬µƒ≥ﬂ¥Á</param>
/// <param name="pPixelFormat">œÒÀÿ∏Ò Ω</param>
/// <returns>true:≥…π¶,false: ß∞‹</returns>
/// <created>Andy,2020/9/29</created>
/// <changed>Andy,2020/9/29</changed>
// ********************************************************************************
bool CFrameBuffer::SetProperties(const CRect* pDimByRect, const CPixelFormat* pPixelFormat)
{
    std::unique_lock<std::recursive_mutex> objAutoLocker(m_objLock);

    m_objPixelFormat = *pPixelFormat;
    m_objDimension.SetDim(pDimByRect);

    return ResizeBuffer();
}

U8 CFrameBuffer::GetBitsPerPixel() const
{
    assert((U8)m_objPixelFormat.nBitsPerPixel == m_objPixelFormat.nBitsPerPixel);

    return (U8)m_objPixelFormat.nBitsPerPixel;
}

U8 CFrameBuffer::GetBytesPerPixel() const
{
    return (U8)(m_objPixelFormat.nBitsPerPixel / 8);
}

// ********************************************************************************
/// <summary>
/// Return a pointer to the pixel data specified by the coordinates of that
/// pixel. GetBufferPtr(0, 0) should be equivalent to GetBuffer(). This
/// function does not check if the coordinates are within the frame buffer
/// boundaries.
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns>÷∏œÚª∫≥Â«¯÷–µƒ“ª∏ˆµÿ÷∑</returns>
/// <created>Andy,2020/9/29</created>
/// <changed>Andy,2020/9/29</changed>
// ********************************************************************************
void* CFrameBuffer::GetBufferPtr(const int x, const int y) const
{
    char* pResult = (char*)m_pBuffer;

    pResult += (y * m_objDimension.width + x) * GetBytesPerPixel();

    return (void*)pResult;
}

// ********************************************************************************
/// <summary>
/// ªÒ»°ª∫≥Â«¯¥Û–°(µ•Œª£∫◊÷Ω⁄)
/// </summary>
/// <returns>ª∫≥Â«¯¥Û–°</returns>
/// <created>Andy,2020/9/29</created>
/// <changed>Andy,2020/9/29</changed>
// ********************************************************************************
int CFrameBuffer::GetBufferSize() const
{
    return (m_objDimension.Area() * m_objPixelFormat.nBitsPerPixel) / 8;
}

// ********************************************************************************
/// <summary>
/// ÷ÿ–¬∑÷≈‰ª∫≥Â«¯
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/9/29</created>
/// <changed>Andy,2020/9/29</changed>
// ********************************************************************************
bool CFrameBuffer::ResizeBuffer()
{
    if (nullptr != m_pBuffer)
    {
        delete[]m_pBuffer;
    }

    m_pBuffer = new U8[GetBufferSize()];

    return (nullptr != m_pBuffer);
}



// ********************************************************************************
/// <summary>
/// ±£¥Êµ±«∞µƒ÷° ˝æ›µΩ¥≈≈ÃŒƒº˛
/// </summary>
/// <param name="lpPath">Œƒº˛¬∑æ∂</param>
/// <returns>true:≥…π¶£¨false: ß∞‹</returns>
/// <created>Andy,2021/5/10</created>
/// <changed>Andy,2021/5/10</changed>
// ********************************************************************************
bool CFrameBuffer::SaveBitmap(const TCHAR* lpPath)
{
//    HANDLE hFile = CreateFile(lpPath, GENERIC_WRITE,FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
//
//    if (INVALID_HANDLE_VALUE == hFile)
//    {
//        return false;
//    }
//
//    // –¥Œƒº˛Õ∑
//    BITMAPFILEHEADER objFileHeader;
//
//    ZeroMemory(&objFileHeader, sizeof(BITMAPFILEHEADER));
//    objFileHeader.bfType = 'MB';
//    objFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + GetBufferSize();
//    objFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
//
//    DWORD dwWritter = 0;
//
//    WriteFile(hFile, &objFileHeader, sizeof(BITMAPFILEHEADER), &dwWritter, NULL);
//
//    // –¥ŒƒÕº∏Ò Ω
//    BITMAPINFOHEADER objInfoHeader;
//
//    ZeroMemory(&objInfoHeader, sizeof(BITMAPINFOHEADER));
//    objInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
//    objInfoHeader.biSizeImage = GetBufferSize();
//    objInfoHeader.biWidth = m_objDimension.width;
//    objInfoHeader.biHeight = m_objDimension.height;
//    objInfoHeader.biBitCount = m_objPixelFormat.nBitsPerPixel;
//
//    WriteFile(hFile, &objInfoHeader, sizeof(BITMAPINFOHEADER), &dwWritter, NULL);
//
//    // –¥ŒªÕº ˝æ›
//    WriteFile(hFile, m_pBuffer, GetBufferSize(), &dwWritter, NULL);
//    CloseHandle(hFile);
    
    return 0;
}

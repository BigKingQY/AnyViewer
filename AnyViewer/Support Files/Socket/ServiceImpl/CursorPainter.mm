//
//  CursorPainter.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/10.
//

#include "CursorPainter.h"
#include "Log.h"

CCursorPainter::CCursorPainter(CFrameBuffer *pFrameBuffer)
    : m_pFrameBuffer(pFrameBuffer),
    m_bCursorIsMoveable(false),
    m_bIgnoreShapeUpdates(false),
    m_bExist(false)
{
}

CCursorPainter::~CCursorPainter()
{
}

void CCursorPainter::UpdatePointerPos(const CPoint *pPosition)
{
    CHECK_POINTER(pPosition);

    std::lock_guard<std::recursive_mutex> objAutoLocker(m_objMutex);

    m_ptPointerPosition = *pPosition;
    m_bCursorIsMoveable = true;

    // Now, pCursor is ready for painting.
}

void CCursorPainter::SetNewCursor(
    const CPoint *pHotSpot
    , U16 nWidth
    , U16 nHeight
    , const vector<U8> *pCursor
    , const vector<U8> *pBitmask)
{
    CHECK_POINTER(pHotSpot);
    CHECK_POINTER(pCursor);
    CHECK_POINTER(pBitmask);

    std::lock_guard<std::recursive_mutex> objAutoLocker(m_objMutex);

    LOG_DEBUG("Cursor hot-spot is (%d, %d)", pHotSpot->x, pHotSpot->y);
    m_objCursor.setHotSpot(pHotSpot->x, pHotSpot->y);

    LOG_DEBUG("Cursor size is (%d, %d)", nWidth, nHeight);

    const CSize objCursorDimension(nWidth, nHeight);
    const CPixelFormat objPixelFormat = m_pFrameBuffer->GetPixelFormat();

    m_objCursor.SetProperties(&objCursorDimension, &objPixelFormat);
    m_objCursorOverlayFrameBuf.SetProperties(objCursorDimension, objPixelFormat);

    const size_t nPixelSize = m_pFrameBuffer->GetBytesPerPixel();
    const size_t nCursorSize = nWidth * nHeight * nPixelSize;

    // Server is allowed to specify zero as nWidth and/or nHeight of the pCursor.
    if (nCursorSize != 0)
    {
        LOG_DEBUG("Set image of pCursor...");
        memcpy(m_objCursor.getPixels()->GetBuffer(), &pCursor->front(), nCursorSize);
        LOG_DEBUG("Set pBitmask of pCursor...");
        m_objCursor.assignMaskFromRfb(reinterpret_cast<const char *>(&pBitmask->front()));
    }
}

void CCursorPainter::SetIgnoreShapeUpdates(const bool bIgnore)
{
    LOG_DEBUG("Set flag of ignor by pCursor Update is '%d'", bIgnore);

    std::lock_guard<std::recursive_mutex> objAutoLocker(m_objMutex);

    m_bIgnoreShapeUpdates = bIgnore;
}

CRect CCursorPainter::HideCursor()
{
    std::lock_guard<std::recursive_mutex> objAutoLocker(m_objMutex);

    if (!m_bExist)
    {
        return CRect();
    }

    m_bExist = false;

    CRect objEraseRect(m_objCursorOverlayFrameBuf.GetDimension().GetRect());
    const CPoint ptCorner = getUpperLeftPoint(&m_ptLastPosition);

    objEraseRect.move(ptCorner.x, ptCorner.y);

    //LOG_INFO("Cursor rect: (%d, %d), (%d, %d)", objEraseRect.left, objEraseRect.top, objEraseRect.right, objEraseRect.bottom);

    if (objEraseRect.Area() == 0)
    {
        return CRect();
    }

    //LOG_INFO("Erasing pCursor...");
    m_pFrameBuffer->CopyFrom(&objEraseRect, &m_objCursorOverlayFrameBuf, 0, 0);

    return objEraseRect;
}

CRect CCursorPainter::showCursor()
{
    std::lock_guard<std::recursive_mutex> objAutoLocker(m_objMutex);

    m_ptLastPosition = m_ptPointerPosition;

    if (m_bExist)
    {
        LOG_WARN("Error in CCursorPainter: painting double copy of pCursor.");
        assert(true);
    }

    if (!m_bIgnoreShapeUpdates && m_bCursorIsMoveable && m_objCursor.GetDimension().Area() != 0)
    {
        //LOG_INFO("Painting pCursor...");

        CPoint ptCorner = getUpperLeftPoint(&m_ptLastPosition);

        m_objCursorOverlayFrameBuf.CopyFrom(m_pFrameBuffer, ptCorner.x, ptCorner.y);
        
        CRect objRect = m_objCursor.GetDimension().GetRect();
        
        CRect overlayRect(&objRect);
        
        overlayRect.move(ptCorner.x, ptCorner.y);
        m_pFrameBuffer->Overlay(&overlayRect, m_objCursor.getPixels(), 0, 0, m_objCursor.GetMask());
        m_bExist = true;

        return overlayRect;
    }

    return CRect();
}

CPoint CCursorPainter::getUpperLeftPoint(const CPoint *pPosition) const
{
    CPoint ptUpperLeftPoint = *pPosition;

    ptUpperLeftPoint.move(-m_objCursor.GetHotSpot().x, -m_objCursor.GetHotSpot().y);

    return ptUpperLeftPoint;
}

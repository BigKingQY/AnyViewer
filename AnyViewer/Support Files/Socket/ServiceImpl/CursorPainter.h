//
//  CursorPainter.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/10.
//

#ifndef CursorPainter_h
#define CursorPainter_h

#include "Typedef.h"
#include "CursorShape.h"

#include <vector>

using namespace std;

class CCursorPainter
{
public:
    CCursorPainter(CFrameBuffer *pFrameBuffer);
    virtual ~CCursorPainter();

    // Do not allow copying objects.
    CCursorPainter(const CCursorPainter &) = delete;
    CCursorPainter &operator=(const CCursorPainter &) = delete;

public:
    // this functions is thread-safe for private data of pCursor, but need external lock of frame buffer
    CRect HideCursor();

    CRect showCursor();

    // this functions is thread-safe
    void SetIgnoreShapeUpdates(const bool bIgnore);

    void UpdatePointerPos(const CPoint *pPosition);

    void SetNewCursor(const CPoint *pHotSpot,
        U16 nWidth, U16 nHeight,
        const vector<U8> *pCursor,
        const vector<U8> *pBitmask);
private:
    // This function is thread-save.
    CPoint getUpperLeftPoint(const CPoint *pPosition) const;

private:

    CFrameBuffer *const      m_pFrameBuffer = nullptr;

    std::recursive_mutex               m_objMutex;
    CCursorShape             m_objCursor;

    // Actual pPosition of pointer
    CPoint                    m_ptPointerPosition;

    // Last painted pPosition of pointer
    CPoint                    m_ptLastPosition;

    // Copy of rect frame buffer under pCursor
    CFrameBuffer             m_objCursorOverlayFrameBuf;

    // Flag is set, if pCursor is showed.
    bool                     m_bExist = false;

    // Flag is set after first call updatePointerPosition().
    // If flag is unset then pointer isn't painted.
    bool                     m_bCursorIsMoveable = false;
    bool                     m_bIgnoreShapeUpdates = false;
};



#endif /* CursorPainter_h */

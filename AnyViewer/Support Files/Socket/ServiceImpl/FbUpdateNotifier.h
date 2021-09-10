////
////  FbUpdateNotifier.h
////  AnyViewer
////
////  Created by Aomei on 2021/9/10.
////
//
//#ifndef FbUpdateNotifier_h
//#define FbUpdateNotifier_h
//
//#pragma once
//
//#include "FrameBuffer.h"
//#include "Point.h"
//#include "Rect.h"
//#include "Region.h"
//#include "Thread.h"
//#include "Ax3Event.h"
//#include "CursorPainter.h"
//
//class CCoreEventsAdapter;
//
//class CFBUpdateNotifier : public CThread
//{
//public:
//    CFBUpdateNotifier(CFrameBuffer *pFrameBuffer,CWatermarksController* pWatermarkController);
//    virtual ~CFBUpdateNotifier();
//
//    // Do not allow copying objects.
//    CFBUpdateNotifier(const CFBUpdateNotifier &) = delete;
//    CFBUpdateNotifier &operator=(const CFBUpdateNotifier &) = delete;
//
//public:
//    virtual bool Start() override;
//
//public:
//    void OnUpdate(const Rect *pUpdatedRect);
//    
//    void OnPropertiesFb();
//
//    void UpdatePointerPos(const Point *pPosition);
//
//    void SetNewCursor(
//        const Point *pHotSpot
//        , const UINT16 nWidth
//        , const UINT16 nHeight
//        , const vector<UINT8> *pCursor
//        , const vector<UINT8> *pBitmask);
//
//    void SetIgnoreShapeUpdates(const bool bIgnore);
//
//protected:
//    // Inherited from CThread
//    void Execute();
//    void OnTerminate();
//
//protected:
//    CFrameBuffer* m_pFrameBuffer{ nullptr };
//    CCursorPainter           m_objCursorPainter;
//
//    // Pointer to pCoreEventsAdapter.
//    // Nothing event (changing properties of frame buffer, Update frame buffer
//    // or Update pCursor) don't sended to pCoreEventsAdapter, while m_pCoreEventsAdapter is 0.
//    DECLARE_MEMBER_AND_METHOD_V11(CCoreEventsAdapter*, m_pCoreEventsAdapter, CoreEventsAdapter, nullptr)
//
//    std::recursive_mutex               m_objUpdateLock;
//    CAx3Event            m_objEventUpdate;
//
//    //It is used for adding watermarks in demo version.
//    CWatermarksController* m_pWatermarksController{ nullptr };
//
//    // In this region added all updates of frame buffer and pCursor updates.
//    CRegion                  m_objUpdateRegion;
//
//    // This rectangle save pPosition of pCursor.
//    Rect                    m_objOldPosition;
//
//    // This flag is true after call OnPropertiesFb().
//    bool                    m_bNewSize{ false };
//
//    // This flag is true after set new pCursor or Update pPosition.
//    bool                    m_bCursorChange{ false };
//};
//
//
//#endif /* FbUpdateNotifier_h */

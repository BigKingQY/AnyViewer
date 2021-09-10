////
////  FbUpdateNotifier.m
////  AnyViewer
////
////  Created by Aomei on 2021/9/10.
////
//
//#include "FbUpdateNotifier.h"
//#include "CoreEventsAdapter.h"
//
//CFBUpdateNotifier::CFBUpdateNotifier(
//    CFrameBuffer *pFrameBuffer
//    , CWatermarksController* pWatermarkController)
//    : m_pFrameBuffer(pFrameBuffer)
//    , m_objCursorPainter(pFrameBuffer)
//    , m_bNewSize(false)
//    , m_bCursorChange(false)
//    , m_pCoreEventsAdapter(0)
//    , m_pWatermarksController(pWatermarkController)
//{
//    m_objOldPosition = m_objCursorPainter.HideCursor();
//}
//
//CFBUpdateNotifier::~CFBUpdateNotifier()
//{
//    Stop();
//}
//
//bool CFBUpdateNotifier::Start()
//{
//    bool bResult = false;
//
//    assert(nullptr != m_pCoreEventsAdapter);
//
//    if (nullptr != m_pCoreEventsAdapter)
//    {
//        bResult = CThread::Start();
//    }
//    else
//    {
//        LOG_ERROR("Don't set CoreEventsAdapter");
//    }
//
//    return bResult;
//}
//
//
//void CFBUpdateNotifier::Execute()
//{
//    //LOG_DEBUG("Begin FBUpdateNotifier::Execute");
//
//    // Send event to pCoreEventsAdapter, while tread isn't terminated.
//    while (!IsTerminating())
//    {
//        // If flag is set, then thread going to sleep (Wait event).
//        bool bNoUpdates = true;
//
//        // Move updates to local variable with blocking notifier mutex "m_objUpdateLock".
//        bool bNewSize = false;
//        bool bCursorChange = false;
//        CRegion objUpdateRegion;
//
//        {
//            std::lock_guard<std::recursive_mutex> objAutoLocker(m_objUpdateLock);
//
//            bNewSize = m_bNewSize;
//            m_bNewSize = false;
//
//            bCursorChange = m_bCursorChange;
//            m_bCursorChange = false;
//
//            objUpdateRegion = m_objUpdateRegion;
//            m_objUpdateRegion.Clear();
//        }
//
//        // Send event "Change properties of frame buffer" to pCoreEventsAdapter
//        // with blocking frame buffer mutex "m_pFrameBufLock".
//
//        if (bNewSize)
//        {
//            bNoUpdates = false;
//            //LOG_INFO("CFBUpdateNotifier (event): new size of frame buffer");
//
//            try
//            {
//                std::lock_guard<std::recursive_mutex> objAutoLocker(m_pFrameBuffer->GetLock());
//
//                m_pCoreEventsAdapter->OnFrameBufferPropChange(m_pFrameBuffer);
//
//                // FIXME: it's bad code. Must work without one next line, but not it.
//                m_pCoreEventsAdapter->OnFrameBufferUpdate(m_pFrameBuffer, &m_pFrameBuffer->GetDimension().GetRect());
//            }
//            catch (...)
//            {
//                LOG_WARN("CFBUpdateNotifier (event): error in set new size");
//            }
//        }
//
//        // objUpdateRegion pPosition on pCursor and send frame buffer objUpdateRegion event to pCoreEventsAdapter
//        // with blocking frame buffer mutex "m_pFrameBufLock".
//        if (bCursorChange || !objUpdateRegion.IsEmpty())
//        {
//            bNoUpdates = false;
//
//            std::lock_guard<std::recursive_mutex> objAutoLocker(m_pFrameBuffer->GetLock());
//            Rect objCursorRect = m_objCursorPainter.showCursor();
//
//            objUpdateRegion.AddRect(&objCursorRect);
//            objUpdateRegion.AddRect(&m_objOldPosition);
//
//#ifdef _DEMO_VERSION_
//            Rect curWmRect = m_pWatermarksController->CurrentRect();
//            CRegion reg(curWmRect);
//            reg.intersect(&objUpdateRegion);
//            bool isIntersect = !reg.IsEmpty();
//            if (isIntersect)
//            {
//                m_pWatermarksController->showWaterMarks(m_pFrameBuffer, m_pFrameBufLock);
//                objUpdateRegion.AddRect(curWmRect);
//            }
//#endif
//
//            vector<Rect> arrUpdatedRects;
//
//            objUpdateRegion.GetRectVector(&arrUpdatedRects);
//            //LOG_INFO("CFBUpdateNotifier (event): %u updates", arrUpdatedRects.size());
//
//            try
//            {
//                for (auto i = arrUpdatedRects.begin(); i != arrUpdatedRects.end(); ++i)
//                {
//                    m_pCoreEventsAdapter->OnFrameBufferUpdate(m_pFrameBuffer, &*i);
//                }
//            }
//            catch (...)
//            {
//                LOG_WARN("CFBUpdateNotifier (event): error in objUpdateRegion");
//            }
//
//
//#ifdef _DEMO_VERSION_
//            if (isIntersect)
//            {
//                m_pWatermarksController->hideWatermarks(m_pFrameBuffer, m_pFrameBufLock);
//            }
//#endif
//
//            m_objOldPosition = m_objCursorPainter.HideCursor();
//
//        }
//
//        // Pause this thread, if there are no updates (pCursor, frame buffer).
//        if (bNoUpdates)
//        {
//            m_objEventUpdate.WaitForEvent();
//        }
//    }
//
//    //LOG_DEBUG("End FBUpdateNotifier::Execute");
//}
//
//void CFBUpdateNotifier::OnTerminate()
//{
//    m_objEventUpdate.Notify();
//}
//
//void CFBUpdateNotifier::OnUpdate(const Rect *pUpdatedRect)
//{
//    CHECK_POINTER(pUpdatedRect);
//
//    {
//        std::lock_guard<std::recursive_mutex> objAutoLocker(m_objUpdateLock);
//
//        m_objUpdateRegion.AddRect(pUpdatedRect);
//    }
//
//    m_objEventUpdate.Notify();
//
//
//
//#ifdef DEBUG
//    LOG_DEBUG("CFBUpdateNotifier: added rectangle");
//#endif
//}
//
//void CFBUpdateNotifier::OnPropertiesFb()
//{
//    {
//        std::lock_guard<std::recursive_mutex> objAutoLocker(m_objUpdateLock);
//
//        m_objUpdateRegion.Clear();
//        m_bNewSize = true;
//    }
//
//    m_objEventUpdate.Notify();
//    LOG_INFO("CFBUpdateNotifier: new size of frame buffer");
//}
//
//void CFBUpdateNotifier::UpdatePointerPos(const Point *pPosition)
//{
//    CHECK_POINTER(pPosition);
//
//    std::lock_guard<std::recursive_mutex> objAutoLocker(m_objUpdateLock);
//
//    m_objCursorPainter.UpdatePointerPos(pPosition);
//    m_bCursorChange = true;
//    m_objEventUpdate.Notify();
//}
//
//void CFBUpdateNotifier::SetNewCursor(
//    const Point *pHotSpot
//    , const UINT16 nWidth
//    , const UINT16 nHeight
//    , const vector<UINT8> *pCursor
//    , const vector<UINT8> *pBitmask)
//{
//    CHECK_POINTER(pHotSpot);
//    CHECK_POINTER(pCursor);
//    CHECK_POINTER(pBitmask);
//
//    {
//        std::lock_guard<std::recursive_mutex> objAutoLocker(m_pFrameBuffer->GetLock());
//
//        m_objCursorPainter.SetNewCursor(pHotSpot, nWidth, nHeight, pCursor, pBitmask);
//    }
//
//    std::lock_guard<std::recursive_mutex> objAutoLocker(m_objUpdateLock);
//
//    m_bCursorChange = true;
//    m_objEventUpdate.Notify();
//}
//
//void CFBUpdateNotifier::SetIgnoreShapeUpdates(const bool bIgnore)
//{
//    std::lock_guard<std::recursive_mutex> objAutoLocker(m_objUpdateLock);
//
//    m_objCursorPainter.SetIgnoreShapeUpdates(bIgnore);
//    m_bCursorChange = true;
//    m_objEventUpdate.Notify();
//}

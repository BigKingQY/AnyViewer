////
////  Region.m
////  AnyViewer
////
////  Created by Aomei on 2021/9/10.
////
//
//#include "Region.h"
//#include "log.h"
//
//CRegion::CRegion()
//{
//    miRegionInit(&m_reg, NullBox, 0);
//}
//
//// FIXME: Make BoxRec and Rect identical to get rid of conversions.
//CRegion::CRegion(const CRect &rect)
//{
//    if (!rect.IsEmpty())
//    {
//        BoxRec box;
//        box.x1 = rect.left;
//        box.x2 = rect.right;
//        box.y1 = rect.top;
//        box.y2 = rect.bottom;
//        miRegionInit(&m_reg, &box, 0);
//    }
//    else
//    {
//        miRegionInit(&m_reg, NullBox, 0);
//    }
//}
//
//CRegion::CRegion(const CRegion &src)
//{
//    miRegionInit(&m_reg, NullBox, 0);
//    set(&src);
//}
//
//CRegion::~CRegion()
//{
//    miRegionUninit(&m_reg);
//}
//
//void CRegion::Clear()
//{
//    miRegionEmpty(&m_reg);
//}
//
//void CRegion::set(const CRegion *src)
//{
//    miRegionCopy(&m_reg, (RegionPtr)&src->m_reg);
//}
//
//CRegion & CRegion::operator=(const CRegion &src)
//{
//    set(&src);
//    return *this;
//}
//
//void CRegion::AddRect(const CRect *rect)
//{
//    if (!rect->IsEmpty())
//    {
//        CRegion temp(rect);
//
//        add(&temp);
//    }
//}
//
//void CRegion::translate(int dx, int dy)
//{
//    miTranslateRegion(&m_reg, dx, dy);
//}
//
//void CRegion::add(const CRegion *other)
//{
//    miUnion(&m_reg, &m_reg, (RegionPtr)&other->m_reg);
//}
//
//void CRegion::add(const CRegion &refSrc)
//{
//    miUnion(&m_reg, &m_reg, (RegionPtr)&refSrc.m_reg);
//}
//
//void CRegion::Subtract(const CRegion *other)
//{
//    miSubtract(&m_reg, &m_reg, (RegionPtr)&other->m_reg);
//}
//
//void CRegion::intersect(const CRegion *other)
//{
//    miIntersect(&m_reg, &m_reg, (RegionPtr)&other->m_reg);
//}
//
//void CRegion::Crop(const CRect *rect)
//{
//    CRegion temp(rect);
//
//    intersect(&temp);
//}
//
//bool CRegion::IsEmpty() const
//{
//    return (miRegionNotEmpty((RegionPtr)&m_reg) == FALSE);
//}
//
//bool CRegion::isPointInside(int x, int y) const
//{
//    BoxRec stubBox; // Ignore returning rect.
//
//    return !!miPointInRegion((RegionPtr)&m_reg, x, y, &stubBox);
//}
//
//bool CRegion::equals(const CRegion *other) const
//{
//    // Handle a special case when both regions are empty.
//    // Such regions may be considered different by miRegionsEqual().
//    if (this->IsEmpty() && other->IsEmpty())
//    {
//        return true;
//    }
//
//    return (miRegionsEqual((RegionPtr)&m_reg,
//        (RegionPtr)&other->m_reg) == TRUE);
//}
//
//// FIXME: Optimize, make BoxRec and Rect identical to get rid of conversions.
//void CRegion::GetRectVector(CRectArray *dst) const
//{
//    dst->clear();
//
//    const BoxRec *boxPtr = REGION_RECTS(&m_reg);
//    long numRects = REGION_NUM_RECTS(&m_reg);
//
//    dst->reserve((size_t)numRects);
//
//    for (long i = 0; i < numRects; i++)
//    {
//        CRect rect(boxPtr[i].x1, boxPtr[i].y1, boxPtr[i].x2, boxPtr[i].y2);
//
//        dst->push_back(rect);
//    }
//}
//
//// FIXME: Optimize, make BoxRec and Rect identical to get rid of conversions.
//void CRegion::getRectList(std::list<CRect> *dst) const
//{
//    dst->clear();
//
//    const BoxRec *boxPtr = REGION_RECTS(&m_reg);
//    long numRects = REGION_NUM_RECTS(&m_reg);
//
//    for (long i = 0; i < numRects; i++)
//    {
//        CRect rect(boxPtr[i].x1, boxPtr[i].y1, boxPtr[i].x2, boxPtr[i].y2);
//
//        dst->push_back(rect);
//    }
//}
//
//size_t CRegion::getCount() const
//{
//    return REGION_NUM_RECTS(&m_reg);
//}
//
//C???Rect CRegion::getBounds() const
//{
//    const BoxRec *boxPtr = REGION_EXTENTS(&m_reg);
//    return CRect(boxPtr->x1, boxPtr->y1, boxPtr->x2, boxPtr->y2);
//}
//
//
//void CRegion::Print()
//{
//    const int nResult = REGION_NUM_RECTS(&m_reg);
//    const int nSize = REGION_SIZE(&m_reg);
//    const BoxPtr pRects = REGION_RECTS(&m_reg);
//
//    LOG_INFO("num: %d size: %d", nResult, nSize);
//    LOG_INFO("extents: %d %d %d %d",
//        m_reg.extents.x1
//        , m_reg.extents.y1
//        , m_reg.extents.x2
//        , m_reg.extents.y2);
//
//    for (int i = 0; i < nResult; i++)
//    {
//        LOG_INFO("%d %d %d %d",
//            pRects[i].x1
//            , pRects[i].y1
//            , pRects[i].x2
//            , pRects[i].y2);
//    }
//}
//

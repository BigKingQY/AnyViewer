////
////  Region.h
////  AnyViewer
////
////  Created by Aomei on 2021/9/10.
////
//
//#ifndef Region_h
//#define Region_h
//
//#pragma once
//
//#include <vector>
//#include <list>
//
//#include "Rect.h"
//
////extern "C" {
////#include "x11region.h"
////}
//
//// --------------------------------------------------------------------------------
///// <summary>
///// A CRegion is an area which can be represented by a set of rectangles with
///// integer coordinates. It maintains the list of rectangles such way that
///// individual rectangles never overlap. When a rectangle is added to a CRegion,
///// only its non-overlapping part will be actually added. Note that adding a
///// rectangle will not necessarily increment the number of rectangles by one.
///// On such addition, the underlying list of rectangles may change dramatically
///// and its length may increase, decrease or remain the same.
///// </summary>
//// --------------------------------------------------------------------------------
//class CRegion
//{
//public:
//    /**
//     * Creates an empty region.
//     */
//    CRegion();
//    /**
//     * Creates a region from the given rectangle.
//     * @param rect a pointer to the source rectangle.
//     */
//    CRegion(const CRect &rect);
//    /**
//     * Creates a copy of another region.
//     * @param rect a reference to the source region.
//     */
//    CRegion(const CRegion &src);
//    /**
//     * The destructor.
//     */
//    virtual ~CRegion();
//
//    /**
//     * Clears the region so that it will consist of zero rectangles.
//     */
//    void Clear();
//    /**
//     * Replaces this region with a copy of another region.
//     * @param src a pointer to the source region.
//     */
//    void set(const CRegion *src);
//    /**
//     * Overriden assignment operator. Replaces this region with a copy of
//     * another region.
//     * @param src a reference to the source region.
//     */
//    CRegion & operator=(const CRegion &src);
//
//    /**
//     * Adds a rectangle to this region.
//     * @param rect rectangle to add.
//     */
//    void AddRect(const CRect *rect);
//    /**
//     * Adds offset to all rectangles in region.
//     * @param dx horizontal offset to add.
//     * @param dy vertical offset to add.
//     */
//    void translate(int dx, int dy);
//    /**
//     * Replaces this region by its union with another region.
//     * @param other region to add.
//     */
//    void add(const CRegion *other);
//    void add(const CRegion &refSrc);
//    /**
//     * Subtracts another region from this region.
//     * @param other region to Subtract.
//     */
//    void Subtract(const CRegion *other);
//    /**
//     * Replace this region by its Intersection with another region.
//     * @param other region to intersect with.
//     */
//    void intersect(const CRegion *other);
//    /**
//     * Sets this region data to Intersection of this region and the specified
//     * rectangle.
//     * @param rect rectangle to intersect with.
//     */
//    void Crop(const CRect *rect);
//
//    /**
//     * Checks if this region is empty.
//     * @return true if this region is empty, false otherwise.
//     */
//    bool IsEmpty() const;
//
//    /**
//     * Tests point location relative of the region.
//     * @return true if this point locates inside the region, false otherwise.
//     */
//    bool isPointInside(int x, int y) const;
//
//    /**
//     * Checks if this region equals to another region.
//     * @param other a pointer to another region to compare this region to.
//     * @return true if this region equals to other, false otherwise.
//     */
//    bool equals(const CRegion *other) const;
//
//    /**
//     * Get the vector of rectangles that constitute this region.
//     * @param dst pointer to an std::vector where the list of rectangles will be
//     *            saved to. The previous contents of the vector will be cleared.
//     */
//    void GetRectVector(CRectArray *dst) const;
//    /**
//     * Get the list of rectangles that constitute this region.
//     * @param dst pointer to an std::list where the list of rectangles will be
//     *            saved to. The previous contents of the list will be cleared.
//     */
//    void getRectList(std::list<CRect> *dst) const;
//
//    // Returns count of rectangles in the region.
//    size_t getCount() const;
//
//    CRect getBounds() const;
//
//public:
//    void Print();
//private:
//    /**
//     * The underlying X11 region structure.
//     */
//    RegionRec m_reg;
//};
//
//
//
//#endif /* Region_h */

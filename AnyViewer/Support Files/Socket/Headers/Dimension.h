//
//  Dimension.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/9.
//

#ifndef Dimension_h
#define Dimension_h

#pragma once

#include "Rect.h"

class CSize
{
public:
    CSize(void) : width(0), height(0) {}
    CSize(const int w, const int h) { SetDim(w, h); }
    CSize(const CRect *r) { width = r->getWidth(); height = r->getHeight(); }
    CSize(const CSize &refSrc) : width(refSrc.width), height(refSrc.height) {}

    const CSize &operator=(const CSize &refSrc) {
        width = refSrc.width;
        height = refSrc.height;
        return *this;
    }
    bool operator==(const CSize &refSrc) { return IsEqualTo(&refSrc); }
    bool operator!=(const CSize &refSrc) { return !IsEqualTo(&refSrc); }

    virtual ~CSize(void) {}

    CRect GetRect() const
    {
        CRect r(width, height);
        return r;
    }

    CSize getTransposition() const
    {
        CSize r(height, width);
        return r;
    }

    // Exchanges width and height
    void transpose()
    {
        int temp = width;
        width = height;
        height = temp;
    }

    inline void SetDim(const int w, const int h) { width = w; height = h; }
    inline void SetDim(const CRect *rect) {
        width = rect->getWidth();
        height = rect->getHeight();
    }

    inline bool cmpDim(const CSize *dim) const {
        return dim->width == width &&
            dim->height == height;
    }

    inline bool IsEmpty() const { return width <= 0 || height <= 0; }
    inline bool IsEqualTo(const CSize *other) const
    {
        return (width == other->width) && (height == other->height);
    }
    inline int Area() const { return IsEmpty() ? 0 : width * height; }

    inline void Clear() { width = height = 0; }

    inline CSize Intersection(const CSize *other) const
    {
        return CSize(width < other->width ? width : other->width,
            height < other->height ? height : other->height);
    }

    int width{ 0 };
    int height{ 0 };
};




#endif /* Dimension_h */

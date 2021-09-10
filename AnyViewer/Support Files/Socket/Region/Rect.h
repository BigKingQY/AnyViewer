//
//  Rect.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/9.
//

#ifndef Rect_h
#define Rect_h

//#include "CommonHeader.h"

#include <vector>

class CRect;
using CRectArray = std::vector<CRect>;


class CRect
{
public:
    CRect(void) : left(0), top(0), right(0), bottom(0) {}
    CRect(const CRect *rect) { setCRect(rect); }
    CRect(const CRect &rect) { setCRect(&rect); }
    CRect(int lt, int tp, int rt, int bm) { setCRect(lt, tp, rt, bm); }
    CRect(int width, int height) { setCRect(0, 0, width, height); }

    virtual ~CRect(void) {}

    int left{0};
    int top{0};
    int right{0};
    int bottom{0};

    inline void setCRect(int lt, int tp, int rt, int bm)
    {
        left = lt;
        top = tp;
        right = rt;
        bottom = bm;
    }

    inline void setCRect(const CRect *rect)
    {
        left = rect->left;
        top = rect->top;
        right = rect->right;
        bottom = rect->bottom;
    }

    inline CRect & operator=(const CRect &rect)
    {
        setCRect(&rect);
        return *this;
    }

    inline bool isValid() const
    {
        if (right < left || bottom < top)
        {
            return false;
        }
        return true;
    }

    inline bool isPointInRect(int pointX, int pointY)
    {
        if (pointX < left || pointX >= right)
        {
            return false;
        }

        if (pointY < top || pointY >= bottom)
        {
            return false;
        }
        return true;
    }

    // Return true this Rect fully contain the inner Rect, otherwise return false.
    inline bool isFullyContainRect(const CRect *inner)
    {
        if (inner->left < left || inner->top < top ||
            inner->right > right || inner->bottom > bottom)
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    inline void move(int offsetX, int offsetY)
    {
        left += offsetX;
        right += offsetX;
        top += offsetY;
        bottom += offsetY;
    }

    inline void SetLocation(int destX, int destY)
    {
        int offsetX = destX - left;
        int offsetY = destY - top;
        left = destX;
        right += offsetX;
        top = destY;
        bottom += offsetY;
    }

    inline bool IsEqualTo(const CRect *rect)  const {
        return  rect->left == left &&
            rect->top == top &&
            rect->right == right &&
            rect->bottom == bottom;
    }

    inline void setWidth(int value) { right = left + value; }
    inline void setHeight(int value) { bottom = top + value; }

    inline int getWidth()  const { return right - left; }
    inline int getHeight() const { return bottom - top; }

    void rotateOn90InsideDimension(int dimHeight)
    {
        CRect localCopy(this);
        setWidth(localCopy.getHeight());
        setHeight(localCopy.getWidth());
        int newLeft = dimHeight - localCopy.top - localCopy.getHeight();
        int newTop = localCopy.left;
        SetLocation(newLeft, newTop);
    }

    void rotateOn180InsideDimension(int dimWidth, int dimHeight)
    {
        int newLeft = dimWidth - left - getWidth();
        int newTop = dimHeight - top - getHeight();
        SetLocation(newLeft, newTop);
    }

    void rotateOn270InsideDimension(int dimWidth)
    {
        CRect localCopy(this);
        setWidth(localCopy.getHeight());
        setHeight(localCopy.getWidth());
        int newLeft = localCopy.top;
        int newTop = dimWidth - localCopy.left - localCopy.getWidth();
        SetLocation(newLeft, newTop);
    }

    inline bool IsEmpty() const { return getWidth() <= 0 || getHeight() <= 0; }
    inline int Area() const { return IsEmpty() ? 0 : getWidth() * getHeight(); }

    static int totalArea(CRectArray &v) {
        int area = 0;
        for (CRectArray::iterator i = v.begin(); i < v.end(); i++)
            area += i->Area();
        return area;
    }

    inline void Clear() { left = top = right = bottom = 0; }

    // ********************************************************************************
    /// <summary>
    /// º∆À„”Î∆‰À¸«¯”ÚµƒΩªºØ
    /// </summary>
    /// <param name="other"></param>
    /// <returns></returns>
    /// <created>Andy,2020/9/27</created>
    /// <changed>Andy,2020/9/27</changed>
    // ********************************************************************************
    CRect Intersection(const CRect *other) const
    {
        CRect result;

        result.setCRect((left > other->left) ? left : other->left,
            (top > other->top) ? top : other->top,
            (right < other->right) ? right : other->right,
            (bottom < other->bottom) ? bottom : other->bottom);

        if (!result.isValid())
        {
            result.Clear();
        }
        return result;
    }
};

#endif /* CRect_h */

//
//  Point.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/9.
//

#ifndef Point_h
#define Point_h

struct CPoint
{
  CPoint() : x(0), y(0) {}
  CPoint(int x_, int y_) : x(x_), y(y_) {}

  inline void Clear() { x = 0; y = 0; }
  inline void setPoint(int x_, int y_) { x = x_; y = y_; }
  inline void move(int deltaX, int deltaY) { x += deltaX; y += deltaY; }
  bool IsEqualTo(const CPoint *other) const { return x == other->x &&
                                                    y == other->y; }

  int x;
  int y;
};



#endif /* Point_h */

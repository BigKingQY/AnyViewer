//
//  PixelFormat.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/9.
//

#ifndef PixelFormat_h
#define PixelFormat_h

#pragma once

class CPixelFormat
{
public:
    CPixelFormat();
    
    CPixelFormat& operator=(const CPixelFormat& refSrc);

public:

    // Initializes the bigEndian field by native machine value.
    void initBigEndianByNative();

    bool IsEqualTo(const CPixelFormat *pf) const;


public:

    unsigned short nBitsPerPixel;
    unsigned short colorDepth;

    unsigned short redMax;
    unsigned short greenMax;
    unsigned short blueMax;

    unsigned short redShift;
    unsigned short greenShift;
    unsigned short blueShift;

    bool bigEndian;
};



#endif /* PixelFormat_h */

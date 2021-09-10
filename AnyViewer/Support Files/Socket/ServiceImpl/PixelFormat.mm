//
//  PixelFormat.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/9.
//

#include "PixelFormat.h"

#include <string.h>

CPixelFormat::CPixelFormat()
{
  memset(this, 0, sizeof(CPixelFormat));
}

CPixelFormat& CPixelFormat::operator=(const CPixelFormat& refSrc)
{
    if (this == &refSrc)
    {
        return *this;
    }

    nBitsPerPixel = refSrc.nBitsPerPixel;
    colorDepth = refSrc.colorDepth;
    redMax = refSrc.redMax;
    greenMax = refSrc.greenMax;
    blueMax = refSrc.blueMax;
    redShift = refSrc.redShift;
    greenShift = refSrc.greenShift;
    blueShift = refSrc.blueShift;
    bigEndian = refSrc.bigEndian;

    return *this;
}

void CPixelFormat::initBigEndianByNative()
{
  union {
    char test;
    int i;
  } testBigEndian;
  testBigEndian.i = 1;
  bigEndian = (testBigEndian.test == 0);
}

bool CPixelFormat::IsEqualTo(const CPixelFormat *pf) const
{
  return nBitsPerPixel == pf->nBitsPerPixel &&
         colorDepth   == pf->colorDepth &&
         redMax       == pf->redMax &&
         greenMax     == pf->greenMax &&
         blueMax      == pf->blueMax &&
         redShift     == pf->redShift &&
         greenShift   == pf->greenShift &&
         blueShift    == pf->blueShift &&
         bigEndian    == pf->bigEndian;
}

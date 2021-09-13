//
//  CViewEvent.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/13.
//

#include "CViewEvent.h"
#include "LocalMessageBus.h"

#include <utility>

void CViewEvent::OnBell()
{
    
}

void CViewEvent::OnError(const std::string& exception)
{
    
}

void CViewEvent::OnFrameBufferUpdate(const CFrameBuffer* pFrameBuffer, const CRect* rect)
{
 
    CSize size = pFrameBuffer->GetDimension();
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGImageByteOrder32Little | kCGImageAlphaPremultipliedFirst;

    CGContextRef context = CGBitmapContextCreate(pFrameBuffer->GetBuffer(), size.width, size.height, 8, size.width * 4, colorSpace, bitmapInfo);
    
    CFRelease(colorSpace);

    CGImageRef cgImage = CGBitmapContextCreateImage(context);
    UIImage *image = [UIImage imageWithCGImage:cgImage];

    CGColorSpaceRelease(colorSpace);
    CGImageRelease(cgImage);
    CGContextRelease(context);
    
    CMessageBus& refMessageBus = GetLocalMessageBus();
    
    //发送一个自定义的消息到本地的消息总线
    refMessageBus.SendReq<void, UIImage *>(MSG_FRAME_UPDATE, std::forward <UIImage *>(image));

}

void CViewEvent::OnFrameBufferPropChange(const CFrameBuffer* pFrameBuffer)
{
    
}

//void CViewEvent::OnFrameBufferSizeChange(const unsigned int nWidth, const unsigned int nHeight)
//{
//
//}

void CViewEvent::OnCutText(const std::string& pCutText)
{
    
}

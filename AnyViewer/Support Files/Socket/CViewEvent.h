//
//  CViewEvent.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/13.
//  界面的事件处理类

#ifndef CViewEvent_h
#define CViewEvent_h

#include "CoreEventsAdapter.h"

class CViewEvent : public CCoreEventsAdapter
{
    
private:
    virtual void OnBell() override;
    
    virtual void OnError(const std::string& exception) override;
    
    virtual void OnFrameBufferUpdate(const CFrameBuffer* pFrameBuffer, const CRect* rect) override;
    
    virtual void OnFrameBufferPropChange(const CFrameBuffer* pFrameBuffer) override;
    
//    virtual void OnFrameBufferSizeChange(const unsigned int nWidth, const unsigned int nHeight) override;
    
    virtual void OnCutText(const std::string& pCutText) override;
    
    
};
 
using CViewEventPtr = std::shared_ptr<CViewEvent>;

#endif /* CViewEvent_h */

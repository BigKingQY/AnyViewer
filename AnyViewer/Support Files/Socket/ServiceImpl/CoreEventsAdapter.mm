//
//  CoreEventsAdapter.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/10.
//

#include "CoreEventsAdapter.h"

// It's abstract, but still needs an implementation.
CCoreEventsAdapter::~CCoreEventsAdapter()
{
}

void CCoreEventsAdapter::OnBell()
{
}

void CCoreEventsAdapter::OnError(const std::string& exception)
{
}

void CCoreEventsAdapter::OnCutText(const std::string& /*pCutText*/)
{
}

void CCoreEventsAdapter::OnEstablished()
{
}

void CCoreEventsAdapter::OnConnected()
{
}

void CCoreEventsAdapter::OnDisconnect(const std::string& pMsg)
{
}


//void CCoreEventsAdapter::OnError(const CException *pException)
//{
//}

void CCoreEventsAdapter::OnFrameBufferUpdate(const CFrameBuffer *pFrameBuffer,
                                            const CRect *rect)
{
}

void CCoreEventsAdapter::OnFrameBufferPropChange(const CFrameBuffer *pFrameBuffer)
{
}

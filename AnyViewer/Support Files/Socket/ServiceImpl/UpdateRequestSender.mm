//
//  UpdateRequestSender.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/10.
//

#include "UpdateRequestSender.h"


CUpdateRequestSender::CUpdateRequestSender(CFrameBuffer* pFrameBuf)
    : m_bUpdateRecieved(false)
    , m_nTimeout(0)
    , m_pFrameBuffer(pFrameBuf)
{
}

CUpdateRequestSender::~CUpdateRequestSender()
{
    Stop();
}

void CUpdateRequestSender::SetWasUpdated()
{
    m_bUpdateRecieved = true;
}

bool CUpdateRequestSender::Start()
{
    bool bResult = false;

    assert(nullptr != m_pFrameBuffer && nullptr != m_pVNCProxy);

    if (nullptr != m_pFrameBuffer && nullptr != m_pVNCProxy)
    {
        bResult = CThread::Start();
    }
    else
    {
        LOG_ERROR("Don't set FrameBuffer");
    }

    return bResult;
}

bool CUpdateRequestSender::IsUpdated()
{
    const bool bResult = m_bUpdateRecieved;

    m_bUpdateRecieved = false;

    return bResult;
}



void CUpdateRequestSender::SendFbUpdateRequest()
{
    CHECK_POINTER(m_pVNCProxy);

    CRect objUpdateRect = m_pFrameBuffer->GetDimension().GetRect();

    m_pVNCProxy->SendFBUpdateRequest(false, objUpdateRect);
    LOG_DEBUG("Sent frame buffer request [%dx%d]..."
        , objUpdateRect.getWidth()
        , objUpdateRect.getHeight());
}


void CUpdateRequestSender::Execute()
{
    while (!IsTerminating())
    {
        if (IsUpdated())
        {
            SendFbUpdateRequest();
        }

        int nTimeout = GetTimeout();

        nTimeout = nTimeout > 0 ? nTimeout : 500;
        CThread::sleep(nTimeout);       //todo: check if it is best approoach?
    }
}

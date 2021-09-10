//
//  UpdateRequestSender.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/10.
//

#ifndef UpdateRequestSender_h
#define UpdateRequestSender_h

#pragma once

#include "Thread.h"
#include "FrameBuffer.h"
#include "CVNCProxy.h"
#include <atomic>

// --------------------------------------------------------------------------------
/// <summary>
///
/// </summary>
// --------------------------------------------------------------------------------
class CUpdateRequestSender : public CThread
{
public:
    CUpdateRequestSender(CFrameBuffer* pFrameBuf);

    ~CUpdateRequestSender();

public:
    void SetWasUpdated();

    virtual bool Start() override;

protected:
    virtual void Execute() override;

private:
    void SendFbUpdateRequest();

    bool IsUpdated();

private:
    DECLARE_MEMBER_AND_METHOD_V11(bool, m_bUpdateRecieved, UpdateRecieved, false);
    DECLARE_MEMBER_AND_METHOD_V11(int, m_nTimeout, Timeout, 0);

    CFrameBuffer* m_pFrameBuffer{ nullptr };

    DECLARE_MEMBER_AND_METHOD_V11(CVNCProxyPtr, m_pVNCProxy, VNCProxy, nullptr);
};





#endif /* UpdateRequestSender_h */

//
//  Ax3Event.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/9.
//

#ifndef Ax3Event_h
#define Ax3Event_h

#pragma once
#include "Typedef.h"
#include <mutex>

#define INFINITE            0xFFFFFFFF

class CAx3Event
{
public:
  CAx3Event();
  virtual ~CAx3Event();

public:
  void Notify();

  bool WaitForEvent(DWORD nMilliseconds = INFINITE);

protected:
    std::mutex               m_objMutex;
    std::condition_variable  m_objCondition;
};



#endif /* Ax3Event_h */

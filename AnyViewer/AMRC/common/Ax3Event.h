#pragma once

#include <mutex>

#define INFINITE            0xFFFFFFFF  // Infinite timeout

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



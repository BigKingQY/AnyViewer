#pragma once

#include "MiscellaneousFunctions.h"
#include <string>
#include <time.h>

class CAIMTime
{
public:
	CAIMTime();
	CAIMTime(time_t nTime);
	virtual ~CAIMTime();

public:
	// 格式化输出当前的时间
	std::string Format(const char* lpFormat);

	// 
	int GetDay();

public:
	// 得到当前时间并格式化输出
	static void GetCurTime(const char* lpFormat, std::string& refResult);

	static unsigned long long GetCurMilliseconds();

	static unsigned long long GetCurseconds();
private:
	DECLARE_MEMBER_AND_METHOD_V11(time_t, m_nTime, Time, 0)
};


//
//  CAIMTime.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/3.
//

#ifndef CAIMTime_h
#define CAIMTime_h

#include "Typedef.h"
#include <string>
#include <time.h>

class CAIMTime
{
public:
    CAIMTime();
    CAIMTime(time_t nTime);
    virtual ~CAIMTime();

public:
    // ∏Ò ΩªØ ‰≥ˆµ±«∞µƒ ±º‰
    std::string Format(const char* lpFormat);

    //
    int GetDay();

public:
    // µ√µΩµ±«∞ ±º‰≤¢∏Ò ΩªØ ‰≥ˆ
    static void GetCurTime(const char* lpFormat, std::string& refResult);

    static unsigned long long GetCurMilliseconds();

    static unsigned long long GetCurseconds();
private:
    DECLARE_MEMBER_AND_METHOD_V11(time_t, m_nTime, Time, 0)
};

#endif

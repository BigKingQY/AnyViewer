#include "stdafx.h"
#include "AIMTime.h"

#include <chrono>

using namespace std::chrono;

CAIMTime::CAIMTime()
{
	m_nTime = GetCurseconds();
}

CAIMTime::CAIMTime(time_t nTime)
	: m_nTime(nTime)
{
}

CAIMTime::~CAIMTime()
{
}

//--------------------------------------------------------------------------------
/// <summary>
/// ��ʽ�������ǰ��ʱ��
/// </summary>
/// <param name="lpFormat">��ʽ�ַ���</param>
/// <returns>���</returns>
/// <created>Andy,2020/7/6</created>
/// <changed>Andy,2020/7/6</changed>
//--------------------------------------------------------------------------------
std::string CAIMTime::Format(const char * lpFormat)
{
	char szBuffer[64] = { 0 };
	struct tm objNowTime;

#ifdef WIN32
	localtime_s(&objNowTime, &m_nTime);
#else
	localtime_r(&m_nTime,&objNowTime);
#endif // WIN32

	
	strftime(szBuffer, sizeof(szBuffer), lpFormat, &objNowTime);

	return std::string(szBuffer);
}

//--------------------------------------------------------------------------------
/// <summary>
/// �õ���ǰ���ڵ���
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/7/7</created>
/// <changed>Andy,2020/7/7</changed>
//--------------------------------------------------------------------------------
int CAIMTime::GetDay()
{
	struct tm objNowTime;

#ifdef WIN32
	gmtime_s(&objNowTime,&m_nTime);
#else
	gmtime_r(&m_nTime,&objNowTime);
#endif

	return objNowTime.tm_mday;
}

//--------------------------------------------------------------------------------
/// <summary>
/// �õ���ǰʱ�䲢��ʽ�����
/// </summary>
/// <param name="lpFormat">��ʽ�ַ���</param>
/// <returns></returns>
/// <created>Andy,2020/7/6</created>
/// <changed>Andy,2020/7/6</changed>
//--------------------------------------------------------------------------------
void CAIMTime::GetCurTime(const char* lpFormat,std::string& refResult)
{
	char szBuffer[64] = { 0 };
	time_t nNow = GetCurseconds();
	struct tm objNowTime;

#ifdef WIN32
	localtime_s(&objNowTime, &nNow);	
#else
	localtime_r(&nNow,&objNowTime);
#endif
	strftime(szBuffer, sizeof(szBuffer), lpFormat, &objNowTime);
	refResult = szBuffer;
}


// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/12/25</created>
/// <changed>Andy,2020/12/25</changed>
// ********************************************************************************
unsigned long long CAIMTime::GetCurMilliseconds()
{
	const system_clock::time_point time_point_now = system_clock::now(); // ��ȡ��ǰʱ���
	const system_clock::duration duration_since_epoch = time_point_now.time_since_epoch(); // ��1970-01-01 00:00:00����ǰʱ����ʱ��
	const time_t nResult = duration_cast<milliseconds>(duration_since_epoch).count(); // ��ʱ��ת��Ϊ������

	return  nResult; // ��ǰʱ��ĺ�����
}

unsigned long long CAIMTime::GetCurseconds()
{
	const system_clock::time_point time_point_now = system_clock::now(); // ��ȡ��ǰʱ���
	const system_clock::duration duration_since_epoch = time_point_now.time_since_epoch(); // ��1970-01-01 00:00:00����ǰʱ����ʱ��
	const time_t nResult = duration_cast<seconds>(duration_since_epoch).count(); // ��ʱ��ת��Ϊ������

	return  nResult; // ��ǰʱ��ĺ�����
}

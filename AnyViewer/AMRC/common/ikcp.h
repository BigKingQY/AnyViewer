//=====================================================================
//
// KCP - A Better ARQ Protocol Implementation
// skywind3000 (at) gmail.com, 2010-2011
//  
// Features:
// + Average RTT reduce 30% - 40% vs traditional ARQ like tcp.
// + Maximum RTT reduce three times vs tcp.
// + Lightweight, distributed as a single source file.
// 
// https://blog.csdn.net/yongkai0214/article/details/85156452?utm_medium=distribute.pc_relevant.none-task-blog-BlogCommendFromBaidu-1.control&depth_1-utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromBaidu-1.control
//=====================================================================
#pragma once

#include  "MiscellaneousFunctions.h"
#include "TypeDefine.h"
#include "blockingconcurrentqueue.h"
#include "Ax3Event.h"

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <atomic>


#ifndef INLINE
#if defined(__GNUC__)

#if (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1))
#define INLINE         __inline__ __attribute__((always_inline))
#else
#define INLINE         __inline__
#endif

#elif (defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__))
#define INLINE __inline
#else
#define INLINE 
#endif
#endif

#if (!defined(__cplusplus)) && (!defined(inline))
#define inline INLINE
#endif



#ifdef _MSC_VER
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4996)
#endif


//---------------------------------------------------------------------
// BYTE ORDER & ALIGNMENT
//---------------------------------------------------------------------
#ifndef IWORDS_BIG_ENDIAN
    #ifdef _BIG_ENDIAN_
        #if _BIG_ENDIAN_
            #define IWORDS_BIG_ENDIAN 1
        #endif
    #endif
    #ifndef IWORDS_BIG_ENDIAN
        #if defined(__hppa__) || \
            defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
            (defined(__MIPS__) && defined(__MIPSEB__)) || \
            defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
            defined(__sparc__) || defined(__powerpc__) || \
            defined(__mc68000__) || defined(__s390x__) || defined(__s390__)
            #define IWORDS_BIG_ENDIAN 1
        #endif
    #endif
    #ifndef IWORDS_BIG_ENDIAN
        #define IWORDS_BIG_ENDIAN  0
    #endif
#endif

#ifndef IWORDS_MUST_ALIGN
	#if defined(__i386__) || defined(__i386) || defined(_i386_)
		#define IWORDS_MUST_ALIGN 0
	#elif defined(_M_IX86) || defined(_X86_) || defined(__x86_64__)
		#define IWORDS_MUST_ALIGN 0
	#elif defined(__amd64) || defined(__amd64__)
		#define IWORDS_MUST_ALIGN 0
	#else
		#define IWORDS_MUST_ALIGN 1
	#endif
#endif


//=====================================================================
// ���ݷ�Ƭ��Ϣ
//=====================================================================
typedef struct 
{
	U32 nConversationID;    ///< Ϊһ����ʾ�Ự��ŵ���������TCP�� convһ����ͨ��˫���豣֤ conv��ͬ���໥�����ݰ����ܹ������ܡ�convΨһ��ʶһ���Ự����ͨ��˫������ͬʱ���ڶ���Ự��
	U8 nCMD;               ///< �������ַ�Ƭ�����á�IKCP_CMD_PUSH�����ݷ�Ƭ��IKCP_CMD_ACK��ack��Ƭ�� IKCP_CMD_WASK�������֪���ڴ�С��IKCP_CMD_WINS����֪���ڴ�С��
	U8 nFrag;              ///< �û����ݿ��ܻᱻ�ֳɶ��KCP�����ͣ�frag��ʶsegment��ƬID����message�е��������ɴ�С��0��ʾ���һ����Ƭ��
	U16 nWnd;               ///< ʣ����մ��ڴ�С�����մ��ڴ�С-���ն��д�С�������ͷ��ķ��ʹ��ڲ��ܳ������շ���������ֵ��
	U32 nTimestamp;         ///< ����ʱ�̵�ʱ���  
	U32 nSN;                ///< ��Ƭsegment����ţ���1�۴ε�����
	U32 nUnAckSN;           ///< ��������Ϣ���(���ջ����������)������δ������������˵��una����һ���ɽ��յ���ţ����յ�sn=10�İ���unaΪ11��
	U32 nLen;               ///< �غ����ݳ��ȡ�  
	U32 nResendTimestamp;   ///< �´γ�ʱ�ش���ʱ�����
	U32 nRTO;               ///< �÷�Ƭ�ĳ�ʱ�ش��ȴ�ʱ�䣬����㷽��ͬTCP��
	U32 nFastACK;           ///< �յ�ackʱ����ĸ÷�Ƭ���������ۼƴ��������ֶ����ڿ����ش����Զ�����Ҫ����ȷ�Ͽ�ʼ�����ش���
	U32 nXMIT;              ///< ���ͷ�Ƭ�Ĵ�����ÿ����һ�μ�һ�����͵Ĵ�����RTO�ļ�����Ӱ�죬���Ǳ�TCP��˵��Ӱ���СһЩ������˼������
	char pData[1];
}IKCP_FRAGMENT,*PIKCP_FRAGMENT;

using IKCP_FRAGMENT_LIST = std::vector<PIKCP_FRAGMENT>;


class CACKRec
{
public:
	CACKRec(U32 nTimestamp = 0,U32 nSN = 0)
		: m_nTimestamp(nTimestamp)
		, m_nSN(nSN)
	{}
	CACKRec(const CACKRec& refSrc)
		: m_nTimestamp(refSrc.m_nTimestamp)
		, m_nSN(refSrc.m_nSN)
	{
	}

	CACKRec& operator=(const CACKRec& refSrc)
	{
		if (this == &refSrc)
		{
			return *this;
		}

		m_nTimestamp = (refSrc.m_nTimestamp);
		m_nSN = (refSrc.m_nSN);

		return *this;
	}
public:
	U32 m_nTimestamp{0};         ///< ����ʱ�̵�ʱ���  
	U32 m_nSN{0};                ///< ��Ƭsegment����ţ���1�۴ε�����
};

using CACKRecQueue = moodycamel::BlockingConcurrentQueue<CACKRec>;


class CKCP
{
public:
	CKCP();

	virtual ~CKCP();

	/// <summary>��־����ö�ٶ���</summary>
	enum LOG_TYPE
	{
		IKCP_LOG_OUTPUT = 1,
		IKCP_LOG_INPUT = 2,
		IKCP_LOG_SEND = 4,
		IKCP_LOG_RECV = 8,
		IKCP_LOG_IN_DATA = 16,
		IKCP_LOG_IN_ACK = 32,
		IKCP_LOG_IN_PROBE = 64,
		IKCP_LOG_IN_WINS = 128,
		IKCP_LOG_OUT_DATA = 256,
		IKCP_LOG_OUT_ACK = 512,
		IKCP_LOG_OUT_PROBE = 1024,
		IKCP_LOG_OUT_WINS = 2048,
	};
public:
	// ��ȡ���ն��еĴ�С
	size_t GetRecevingQueueSize();

	// ��ȡ���Ͷ��еĴ�С
	size_t GetSendingQueueSize();

public:

	// create a new kcp control object, 'nCoonversationID' must equal in two endpoint
	// from the same connection. 'user' will be passed to the output callback
	// output callback can be setup like this: 'kcp->output = my_udp_output'
	bool KCPInit(const U32 nCoonversationID);

	// user/upper level recv: returns size, returns below zero for EAGAIN
	int KCPRecv(char *pBuffer, const int nLen);

	// user/upper level send, returns below zero for error
	int KCPSend(const char *pBuffer, const int nLen);

	// update m_nState (call it repeatedly, every 10ms-100ms), or you can ask 
	// KCPCheck when to call it again (without KCPInput/_send calling).
	// 'm_nCurrentTime' - m_nCurrentTime timestamp in millisec. 
	void KCPUpdate(const U32 nCurrentTime);

	// Determine when should you invoke KCPUpdate:
	// returns when you should invoke KCPUpdate in millisec, if there 
	// is no KCPInput/_send calling. you can call KCPUpdate in that
	// time, instead of call update repeatly.
	// Important to reduce unnacessary KCPUpdate invoking. use it to 
	// schedule KCPUpdate (eg. implementing an epoll-like mechanism, 
	// or optimize KCPUpdate when handling massive kcp connections)
	U32 KCPCheck(U32 m_nCurrentTime);

	// when you received a low level packet (eg. UDP packet), call it
	int KCPInput(const char *pData, const int nInLen);


	// change MTU size, default is 1400
	int KCPSetMTU(const int nMTU);

	// set maximum window size: sndwnd=32, rcvwnd=32 by default
	int KCPSetWinSize(const int nSendingWnd, const int nRecevingWin);

	// fastest: KCPSetNoDelay(kcp, 1, 20, 2, 1)
	// nodelay: 0:disable(default), 1:enable
	// interval: internal update timer interval in millisec, default is 100ms 
	// nResend: 0:disable fast nResend(default), 1:enable fast nResend
	// nDisenableStreamCtrl: 0:normal congestion control(default), 1:disable congestion control
	int KCPSetNoDelay(int nNodelay, int nInterval, int nResend, int nDisenableStreamCtrl);

	// setup allocator
	void KCPSetAllocator(void* (*new_malloc)(size_t), void(*new_free)(void*));

	// read nCoonversationID
	U32 KCPGetConversationID(const void *ptr);

protected:

	// �������
	virtual int KCPOutput(const char *pBuffer, const int nLen) = 0;

	// д��־
	virtual void Writelog(const char * /*lpLog*/) {}

private:  // ���������

	// �����յ���ACK����
	void OnACK(IKCP_FRAGMENT objFragment, U32& nMaxACK, int& nFlag, U32& nLastTimestamp);

	// �����յ���Push����
	void OnPush(IKCP_FRAGMENT objFragment, const char *pData);
private:
	// flush pending data
	void Flush();

	// check the size of pNext message in the recv queue
	int CalcNextMsgSize();

	// �ж��Ƿ�ͻ������ǰ����־����
	int Canlog(const int nMask);

	// �����ش���ʱ
	void CaleRTO(const int nRTT);

	// ���¼����һ��û��Ӧ������ݰ�
	void CaleSNFirstPacketNAck();

	// ���Ѿ���������ȷ�ϵ����ݰ��ӷ��Ͷ�����ɾ��
	void RemoveFragAcked(const U32 nSN);

	// ���Ѿ���������ȷ�ϵ����ݰ��ӷ��Ͷ�����ɾ��
	void RemoveFragUnAcked(const U32 nSN);

	// �����Ƭ�������Ĵ���
	void CalcFaskAck(const U32 nSN, const U32 nTimestamp);

	// ���һ��ACK���б���
	void PushACK(const U32 nSN, const U32 nTimestamp);

	// ��ACK�б��л�ȡһ��ָ����ACK
	bool GetACK(CACKRec& refACKRec);

	//  �����µķ�Ƭ�����ջ�����
	void StoreNewFrag(IKCP_FRAGMENT *pNewSegment);

	// ���µķ�Ƭ���ı����ָ���Ļ�������
	char *EncodeFrag(const IKCP_FRAGMENT *pFragment, char *pData);

	// �õ����д��ڵĳߴ�
	int GetFreeRecevingWinSize();

	// ����ʱ����
	int SetInterval(const int nInterval);

	// �õ��ȴ����͵ķ�Ƭ����
	int GetFragsBeSent();

	// �����Ƭ�б�
	void ClearFragList(IKCP_FRAGMENT_LIST& refFragList);

	// �����־
	void Log(int nMask, const char *fmt, ...);
private:
	///< ��ʼ�� kcp����nCoonversationIDΪһ����ʾ�Ự��ŵ���������tcp�� nCoonversationIDһ����ͨ��˫
    ///< ���豣֤ nCoonversationID��ͬ���໥�����ݰ����ܹ����Ͽɣ�user��һ�����ص�������ָ��
	U32                  m_nConversationID{ 0 };
	
	///< ����䵥Ԫ��Ĭ������Ϊ1400����СΪ50��
	DECLARE_MEMBER_AND_METHOD_V11(U32, m_nMTU, MTU, 1400);

	///< ����Ƭ��С��������mtu��
	DECLARE_MEMBER_AND_METHOD_V11(U32,m_nMSS, MSS, 0);

	///<  ����״̬��0xFFFFFFFF��ʾ�Ͽ����ӣ���
	U32                  m_nState{ 0 };

	///< ��һ��δȷ�ϵİ���
	U32                  m_nSNFirstPacketNAck{ 0 };

	///< ��һ��������İ�����ţ�
	std::atomic<U32>     m_nSNNext{ 0 };

	///< ��������Ϣ��š�Ϊ�˱�֤����˳�򣬽��շ���ά��һ�����մ��ڣ����մ�����һ����ʼ���m_nSNNextRecving����������Ϣ��ţ��Լ�β��� m_nSNNextRecving + m_nRecevingWin�����մ��ڴ�С����
	U32                  m_nSNNextRecving{ 0 };

	
	U32                  m_nRecentTimestamp{ 0 };


	U32                  m_nLastACKTimestamp{ 0 };

	///< ӵ��������ֵ���԰�Ϊ��λ��TCP���ֽ�Ϊ��λ����
	U32                  m_nCongestionWinthrd{ 0 };

	///<  RTT�ı仯�����������ӵĶ���������������ڼ����ش���ʱ����
	int                   m_nRXRTTVal{ 0 };

	///< smoothed round trip time��ƽ�����RTT���������ڼ����ش���ʱ����
	U32                  m_nRXSmoothedRTT{ 0 };

	///<  ��ACK�����ӳټ���������ش���ʱʱ�䣻
	U32                  m_nRXRetrTimeout{ 0 };

	///< ��С�ش���ʱʱ�䣻
	U32                  m_nRXMinRetrTimeout{ 0 };

	///< ���ʹ��ڴ�С��
	U32                  m_nSendingWin{ 0 };

	///< ���մ��ڴ�С��
	U32                  m_nRecevingWin{ 0 };

	///< Զ�˽��մ��ڴ�С��
	U32                  m_nPeerRecevingWin{ 0 };
	
	///< ӵ�����ڴ�С
	U32                  m_nCongestionWin{ 0 };

	///< ̽�������IKCP_ASK_TELL��ʾ��֪Զ�˴��ڴ�С��IKCP_ASK_SEND��ʾ����Զ�˸�֪���ڴ�С��
	U32                  m_nProbe{ 0 };

	///< ��ǰ��ʱ��
	U32                  m_nCurrentTime{ 0 };

	///< �ڲ�flushˢ�¼������ϵͳѭ��Ч���зǳ���ҪӰ�죻
	U32                  m_nInterval{ 0 };

	///< �´�flushˢ��ʱ�����
	U32                  m_nNextFlushTimestamp{ 0 };

	///< ����segment�Ĵ�������segment��m_nXMIT����ʱ��m_nXMIT���ӣ���һ�λ��ش����⣩��
	U32                  m_nXMIT{ 0 };


	///< �Ƿ��������ӳ�ģʽ�����ӳ�ģʽrtomin������Ϊ0��ӵ�����Ʋ�������
	int                  m_bNodelay{ 0 };

	///< �Ƿ���ù�update�����ı�ʶ��
	U32                  m_bUpdated{ 0 };

	///< �´�̽�鴰�ڵ�ʱ�����
	U32                  m_nNextProbeTimestamp{ 0 };

	///< ̽�鴰����Ҫ�ȴ���ʱ�䣻    
	U32                  m_nProbeWaitingTime{ 0 };

	///< ����ش�����������Ϊ�����жϣ�
	U32                  m_nDeadLink{ 0 };
	
	///< �ɷ��͵������������
	U32                  m_nIncr{ 0 };

	
	std::recursive_mutex      m_objSendingQueueMutex;
	
	// ������
	std::mutex               m_objSendingMutex;

	///< ���Ͷ��е����ֵ,���������ֵ��ֹͣ�����û�����
	DECLARE_MEMBER_AND_METHOD_V11(U32, m_nSendingQueueSizeMax, SendingQueueSizeMax, 1024);
	
	CAx3Event                 m_objSendingQueueEvents;

	///< ������Ϣ�Ķ��У�
	IKCP_FRAGMENT_LIST        m_objSendingQueue;

	///< ������Ϣ�Ļ��棻
	IKCP_FRAGMENT_LIST        m_objSendingBuffer;


	std::recursive_mutex      m_objRecevingMutex;

	///< ������Ϣ�Ķ��У�
	IKCP_FRAGMENT_LIST        m_objRecevingQueue;

	///< ������Ϣ�Ķ���
	IKCP_FRAGMENT_LIST        m_objRecevingBuffer;

	///<  �����͵�ack�б�
	CACKRecQueue              m_queACKList{1024};

	///<  �洢�������ݰ���Ϣ�ֽ�����
	char                     *m_pBuffer{ nullptr };
	
	///<  ���������ش����ظ�ACK������
	int                      m_nFastResendAckThd{ 0 };

	
	int                      m_nFastLimit{ 0 };
	
	///< ȡ��ӵ������
	int                      m_bCancelCongestionCtrl{ 0 };

	///<  �Ƿ����������ģʽ��
	DECLARE_MEMBER_AND_METHOD_V11(bool, m_bStreamMod, StreamMod, true);

	int                      m_nLogMask{ 0x00FFFFFF };


};


template<class T>
inline T safe_pop_front(std::vector<T>& refList, std::recursive_mutex& refMutex)
{
	std::unique_lock<std::recursive_mutex> objAutoLocker(refMutex);
	T objResult = nullptr;

	if (refList.size() > 0)
	{
		objResult = *(refList.begin());
		refList.erase(refList.begin());
	}

	return objResult;
}

template<class T>
inline  T safe_pop_back(std::vector<T>& refList, std::recursive_mutex& refMutex)
{
	std::unique_lock<std::recursive_mutex> objAutoLocker(refMutex);
	T objResult = nullptr;

	if (refList.size() > 0)
	{
		objResult = *(refList.rbegin());
		refList.pop_back();
	}

	return objResult;
}

template<class T>
inline  void safe_push_back(std::vector<T>& refList, T objNewEntry, std::recursive_mutex& refMutex)
{
	std::unique_lock<std::recursive_mutex> objAutoLocker(refMutex);

	refList.push_back(objNewEntry);
}

template<class T>
inline  size_t safe_containers_size(std::vector<T>& refList,std::recursive_mutex& refMutex)
{
	std::unique_lock<std::recursive_mutex> objAutoLocker(refMutex);

	return refList.size();
}
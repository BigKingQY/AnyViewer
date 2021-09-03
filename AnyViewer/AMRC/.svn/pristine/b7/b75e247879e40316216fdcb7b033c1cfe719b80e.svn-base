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
// 数据分片信息
//=====================================================================
typedef struct 
{
	U32 nConversationID;    ///< 为一个表示会话编号的整数，和TCP的 conv一样，通信双方需保证 conv相同，相互的数据包才能够被接受。conv唯一标识一个会话，但通信双方可以同时存在多个会话。
	U8 nCMD;               ///< 用来区分分片的作用。IKCP_CMD_PUSH：数据分片；IKCP_CMD_ACK：ack分片； IKCP_CMD_WASK：请求告知窗口大小；IKCP_CMD_WINS：告知窗口大小。
	U8 nFrag;              ///< 用户数据可能会被分成多个KCP包发送，frag标识segment分片ID（在message中的索引，由大到小，0表示最后一个分片）
	U16 nWnd;               ///< 剩余接收窗口大小（接收窗口大小-接收队列大小），发送方的发送窗口不能超过接收方给出的数值。
	U32 nTimestamp;         ///< 发送时刻的时间戳  
	U32 nSN;                ///< 分片segment的序号，按1累次递增。
	U32 nUnAckSN;           ///< 待接收消息序号(接收滑动窗口左端)。对于未丢包的网络来说，una是下一个可接收的序号，如收到sn=10的包，una为11。
	U32 nLen;               ///< 载荷数据长度。  
	U32 nResendTimestamp;   ///< 下次超时重传的时间戳。
	U32 nRTO;               ///< 该分片的超时重传等待时间，其计算方法同TCP。
	U32 nFastACK;           ///< 收到ack时计算的该分片被跳过的累计次数，此字段用于快速重传，自定义需要几次确认开始快速重传。
	U32 nXMIT;              ///< 发送分片的次数，每发送一次加一。发送的次数对RTO的计算有影响，但是比TCP来说，影响会小一些，计算思想类似
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
	U32 m_nTimestamp{0};         ///< 发送时刻的时间戳  
	U32 m_nSN{0};                ///< 分片segment的序号，按1累次递增。
};

using CACKRecQueue = moodycamel::BlockingConcurrentQueue<CACKRec>;


class CKCP
{
public:
	CKCP();

	virtual ~CKCP();

	/// <summary>日志类型枚举定义</summary>
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
	// 获取接收队列的大小
	size_t GetRecevingQueueSize();

	// 获取发送队列的大小
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

	// 输出数据
	virtual int KCPOutput(const char *pBuffer, const int nLen) = 0;

	// 写日志
	virtual void Writelog(const char * /*lpLog*/) {}

private:  // 命令处理函数组

	// 处理收到的ACK命令
	void OnACK(IKCP_FRAGMENT objFragment, U32& nMaxACK, int& nFlag, U32& nLastTimestamp);

	// 处理收到的Push命令
	void OnPush(IKCP_FRAGMENT objFragment, const char *pData);
private:
	// flush pending data
	void Flush();

	// check the size of pNext message in the recv queue
	int CalcNextMsgSize();

	// 判断是否客户输出当前的日志类型
	int Canlog(const int nMask);

	// 计算重传超时
	void CaleRTO(const int nRTT);

	// 重新计算第一个没有应答的数据包
	void CaleSNFirstPacketNAck();

	// 把已经被接收者确认的数据包从发送队列中删除
	void RemoveFragAcked(const U32 nSN);

	// 把已经被接收者确认的数据包从发送队列中删除
	void RemoveFragUnAcked(const U32 nSN);

	// 计算分片被跳过的次数
	void CalcFaskAck(const U32 nSN, const U32 nTimestamp);

	// 添加一个ACK到列表中
	void PushACK(const U32 nSN, const U32 nTimestamp);

	// 从ACK列表中获取一个指定的ACK
	bool GetACK(CACKRec& refACKRec);

	//  保存新的分片到接收缓冲区
	void StoreNewFrag(IKCP_FRAGMENT *pNewSegment);

	// 把新的分片信心编码大指定的缓冲区中
	char *EncodeFrag(const IKCP_FRAGMENT *pFragment, char *pData);

	// 得到空闲窗口的尺寸
	int GetFreeRecevingWinSize();

	// 设置时间间隔
	int SetInterval(const int nInterval);

	// 得到等待发送的分片数量
	int GetFragsBeSent();

	// 清除分片列表
	void ClearFragList(IKCP_FRAGMENT_LIST& refFragList);

	// 输出日志
	void Log(int nMask, const char *fmt, ...);
private:
	///< 初始化 kcp对象，nCoonversationID为一个表示会话编号的整数，和tcp的 nCoonversationID一样，通信双
    ///< 方需保证 nCoonversationID相同，相互的数据包才能够被认可，user是一个给回调函数的指针
	U32                  m_nConversationID{ 0 };
	
	///< 最大传输单元，默认数据为1400，最小为50；
	DECLARE_MEMBER_AND_METHOD_V11(U32, m_nMTU, MTU, 1400);

	///< 最大分片大小，不大于mtu；
	DECLARE_MEMBER_AND_METHOD_V11(U32,m_nMSS, MSS, 0);

	///<  连接状态（0xFFFFFFFF表示断开连接）；
	U32                  m_nState{ 0 };

	///< 第一个未确认的包；
	U32                  m_nSNFirstPacketNAck{ 0 };

	///< 下一个待分配的包的序号；
	std::atomic<U32>     m_nSNNext{ 0 };

	///< 待接收消息序号。为了保证包的顺序，接收方会维护一个接收窗口，接收窗口有一个起始序号m_nSNNextRecving（待接收消息序号）以及尾序号 m_nSNNextRecving + m_nRecevingWin（接收窗口大小）；
	U32                  m_nSNNextRecving{ 0 };

	
	U32                  m_nRecentTimestamp{ 0 };


	U32                  m_nLastACKTimestamp{ 0 };

	///< 拥塞窗口阈值，以包为单位（TCP以字节为单位）；
	U32                  m_nCongestionWinthrd{ 0 };

	///<  RTT的变化量，代表连接的抖动情况（仅仅用于计算重传超时）；
	int                   m_nRXRTTVal{ 0 };

	///< smoothed round trip time，平滑后的RTT（仅仅用于计算重传超时）；
	U32                  m_nRXSmoothedRTT{ 0 };

	///<  由ACK接收延迟计算出来的重传超时时间；
	U32                  m_nRXRetrTimeout{ 0 };

	///< 最小重传超时时间；
	U32                  m_nRXMinRetrTimeout{ 0 };

	///< 发送窗口大小；
	U32                  m_nSendingWin{ 0 };

	///< 接收窗口大小；
	U32                  m_nRecevingWin{ 0 };

	///< 远端接收窗口大小；
	U32                  m_nPeerRecevingWin{ 0 };
	
	///< 拥塞窗口大小
	U32                  m_nCongestionWin{ 0 };

	///< 探查变量，IKCP_ASK_TELL表示告知远端窗口大小。IKCP_ASK_SEND表示请求远端告知窗口大小；
	U32                  m_nProbe{ 0 };

	///< 当前的时间
	U32                  m_nCurrentTime{ 0 };

	///< 内部flush刷新间隔，对系统循环效率有非常重要影响；
	U32                  m_nInterval{ 0 };

	///< 下次flush刷新时间戳；
	U32                  m_nNextFlushTimestamp{ 0 };

	///< 发送segment的次数，当segment的m_nXMIT增加时，m_nXMIT增加（第一次或重传除外）；
	U32                  m_nXMIT{ 0 };


	///< 是否启动无延迟模式。无延迟模式rtomin将设置为0，拥塞控制不启动；
	int                  m_bNodelay{ 0 };

	///< 是否调用过update函数的标识；
	U32                  m_bUpdated{ 0 };

	///< 下次探查窗口的时间戳；
	U32                  m_nNextProbeTimestamp{ 0 };

	///< 探查窗口需要等待的时间；    
	U32                  m_nProbeWaitingTime{ 0 };

	///< 最大重传次数，被认为连接中断；
	U32                  m_nDeadLink{ 0 };
	
	///< 可发送的最大数据量；
	U32                  m_nIncr{ 0 };

	
	std::recursive_mutex      m_objSendingQueueMutex;
	
	// 发送锁
	std::mutex               m_objSendingMutex;

	///< 发送队列的最大值,当到达最大值后将停止接收用户输入
	DECLARE_MEMBER_AND_METHOD_V11(U32, m_nSendingQueueSizeMax, SendingQueueSizeMax, 1024);
	
	CAx3Event                 m_objSendingQueueEvents;

	///< 发送消息的队列；
	IKCP_FRAGMENT_LIST        m_objSendingQueue;

	///< 发送消息的缓存；
	IKCP_FRAGMENT_LIST        m_objSendingBuffer;


	std::recursive_mutex      m_objRecevingMutex;

	///< 接收消息的队列；
	IKCP_FRAGMENT_LIST        m_objRecevingQueue;

	///< 接收消息的队列
	IKCP_FRAGMENT_LIST        m_objRecevingBuffer;

	///<  待发送的ack列表；
	CACKRecQueue              m_queACKList{1024};

	///<  存储各种数据包消息字节流；
	char                     *m_pBuffer{ nullptr };
	
	///<  触发快速重传的重复ACK个数；
	int                      m_nFastResendAckThd{ 0 };

	
	int                      m_nFastLimit{ 0 };
	
	///< 取消拥塞控制
	int                      m_bCancelCongestionCtrl{ 0 };

	///<  是否采用流传输模式；
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
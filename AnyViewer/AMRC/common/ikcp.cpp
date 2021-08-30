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
// 参考资料 ： https://blog.csdn.net/yongkai0214/article/details/85212831
//=====================================================================
//#include "stdafx.h"
#include "ikcp.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>



//=====================================================================
// KCP BASIC
//=====================================================================
const U32 IKCP_RTO_NDL = 30;		// no delay min nRTO
const U32 IKCP_RTO_MIN = 100;		// normal min nRTO
const U32 IKCP_RTO_DEF = 200;
const U32 IKCP_RTO_MAX = 60000;

const U32 IKCP_ASK_SEND = 1;		// need to send IKCP_CMD_WASK
const U32 IKCP_ASK_TELL = 2;		// need to send IKCP_CMD_WINS
const U32 IKCP_WND_SND = 32;
const U32 IKCP_WND_RCV = 128;       // must >= max nFragment nSize
const U32 IKCP_MTU_DEF = 1400;
const U32 IKCP_ACK_FAST = 3;
const U32 IKCP_INTERVAL = 100;
const U32 IKCP_OVERHEAD = 24;
const U32 IKCP_DEADLINK = 20;
const U32 IKCP_THRESH_INIT = 2;
const U32 IKCP_THRESH_MIN = 2;
const U32 IKCP_PROBE_INIT = 7000;		// 7 secs to m_nProbe window nSize
const U32 IKCP_PROBE_LIMIT = 120000;	// up to 120 secs to m_nProbe window
const U32 IKCP_FASTACK_LIMIT = 5;		// max times to trigger nFastACK


/// <summary>KCP报文类型枚举定义</summary>
enum IKCP_CMD_TYPE
{
	IKCP_CMD_PUSH = 81,		///< 枚举常量, push data (数据报文)
	IKCP_CMD_ACK = 82,		///< 枚举常量, ack （ACK报文）
	IKCP_CMD_WASK = 83,		///< 枚举常量, window probe (ask)。，探测窗口报文
	IKCP_CMD_WINS = 84,		///< 枚举常量, window size (tell)，响应窗口报文
};


//---------------------------------------------------------------------
// encode / decode
//---------------------------------------------------------------------

/* encode 8 bits unsigned int */
static inline char *ikcp_encode8u(char *pQueueHead, unsigned char c)
{
	*(unsigned char*)pQueueHead++ = c;
	return pQueueHead;
}

/* decode 8 bits unsigned int */
static inline const char *ikcp_decode8u(const char *pQueueHead, unsigned char *c)
{
	*c = *(unsigned char*)pQueueHead++;
	return pQueueHead;
}

/* encode 16 bits unsigned int (lsb) */
static inline char *ikcp_encode16u(char *pQueueHead, unsigned short w)
{
#if IWORDS_BIG_ENDIAN || IWORDS_MUST_ALIGN
	* (unsigned char*)(pQueueHead + 0) = (w & 255);
	*(unsigned char*)(pQueueHead + 1) = (w >> 8);
#else
	memcpy(pQueueHead, &w, 2);
#endif
	pQueueHead += 2;
	return pQueueHead;
}

/* decode 16 bits unsigned int (lsb) */
static inline const char *ikcp_decode16u(const char *pQueueHead, unsigned short *w)
{
#if IWORDS_BIG_ENDIAN || IWORDS_MUST_ALIGN
	* w = *(const unsigned char*)(pQueueHead + 1);
	*w = *(const unsigned char*)(pQueueHead + 0) + (*w << 8);
#else
	memcpy(w, pQueueHead, 2);
#endif
	pQueueHead += 2;
	return pQueueHead;
}

/* encode 32 bits unsigned int (lsb) */
static inline char *ikcp_encode32u(char *pQueueHead, U32 l)
{
#if IWORDS_BIG_ENDIAN || IWORDS_MUST_ALIGN
	* (unsigned char*)(pQueueHead + 0) = (unsigned char)((l >> 0) & 0xff);
	*(unsigned char*)(pQueueHead + 1) = (unsigned char)((l >> 8) & 0xff);
	*(unsigned char*)(pQueueHead + 2) = (unsigned char)((l >> 16) & 0xff);
	*(unsigned char*)(pQueueHead + 3) = (unsigned char)((l >> 24) & 0xff);
#else
	memcpy(pQueueHead, &l, 4);
#endif
	pQueueHead += 4;
	return pQueueHead;
}

/* decode 32 bits unsigned int (lsb) */
static inline const char *ikcp_decode32u(const char *pQueueHead, U32 *l)
{
#if IWORDS_BIG_ENDIAN || IWORDS_MUST_ALIGN
	* l = *(const unsigned char*)(pQueueHead + 3);
	*l = *(const unsigned char*)(pQueueHead + 2) + (*l << 8);
	*l = *(const unsigned char*)(pQueueHead + 1) + (*l << 8);
	*l = *(const unsigned char*)(pQueueHead + 0) + (*l << 8);
#else 
	memcpy(l, pQueueHead, 4);
#endif
	pQueueHead += 4;
	return pQueueHead;
}

static inline U32 _imin_(U32 a, U32 b)
{
	return a <= b ? a : b;
}

static inline U32 _imax_(U32 a, U32 b)
{
	return a >= b ? a : b;
}

// ********************************************************************************
/// <summary>
///  把lower限制在(middle和upper之间)
/// </summary>
/// <param name="lower"></param>
/// <param name="middle"></param>
/// <param name="upper"></param>
/// <returns></returns>
/// <created>Andy,2020/12/29</created>
/// <changed>Andy,2020/12/29</changed>
// ********************************************************************************
static inline U32 Bound(U32 lower, U32 middle, U32 upper)
{
	return _imin_(_imax_(lower, middle), upper);
}

static inline long Difference(U32 later, U32 earlier)
{
	return ((I32)(later - earlier));
}

//---------------------------------------------------------------------
// manage pCurFragment
//---------------------------------------------------------------------


static void* (*ikcp_malloc_hook)(size_t) = NULL;
static void(*ikcp_free_hook)(void *) = NULL;

// internal malloc
static void* ikcp_malloc(size_t nSize) 
{
	if (ikcp_malloc_hook)
	{
		return ikcp_malloc_hook(nSize);
	}

	return malloc(nSize);
}

// internal free
static void ikcp_free(void *ptr) 
{
	if (ikcp_free_hook) 
	{
		ikcp_free_hook(ptr);
	}
	else
	{
		free(ptr);
	}
}



// allocate a new kcp pCurFragment
static IKCP_FRAGMENT* CreateSegment(int nSize)
{
	return (IKCP_FRAGMENT*)ikcp_malloc(sizeof(IKCP_FRAGMENT) + nSize);
}

// delete a pCurFragment
static void DeleteSegment(IKCP_FRAGMENT *pFragment)
{
	ikcp_free(pFragment);
}


CKCP::CKCP()
{	
}

CKCP::~CKCP()
{
	ClearFragList(m_objSendingBuffer);
	ClearFragList(m_objRecevingBuffer);
	ClearFragList(m_objSendingQueue);
	ClearFragList(m_objRecevingQueue);	

	if (nullptr != m_pBuffer)
	{
		ikcp_free(m_pBuffer);
	}

	m_pBuffer = NULL;

}

// ********************************************************************************
/// <summary>
/// 获取接收队列的大小
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/12/30</created>
/// <changed>Andy,2020/12/30</changed>
// ********************************************************************************
size_t CKCP::GetRecevingQueueSize()
{
	return  safe_containers_size<PIKCP_FRAGMENT>(m_objRecevingQueue, m_objRecevingMutex);
}

// ********************************************************************************
/// <summary>
/// 获取发送队列的大小
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/12/30</created>
/// <changed>Andy,2020/12/30</changed>
// ********************************************************************************
size_t CKCP::GetSendingQueueSize()
{
	return  safe_containers_size<PIKCP_FRAGMENT>(m_objSendingQueue, m_objSendingQueueMutex);
}

// ********************************************************************************
/// <summary>
/// 初始化 
/// </summary>
/// <param name="nConversationID"></param>
/// <param name="user"></param>
/// <returns></returns>
/// <created>Andy,2020/12/28</created>
/// <changed>Andy,2020/12/28</changed>
// ********************************************************************************
bool CKCP::KCPInit(const U32 nConversationID)
{
	bool nResult = false;

	m_nMTU = IKCP_MTU_DEF;
	m_pBuffer = (char*)ikcp_malloc((m_nMTU + IKCP_OVERHEAD) * 3);

	if (nullptr != m_pBuffer)
	{
		m_nConversationID = nConversationID;
		m_nSNFirstPacketNAck = 0;
		m_nSNNext = 0;
		m_nSNNextRecving = 0;
		m_nRecentTimestamp = 0;
		m_nLastACKTimestamp = 0;
		m_nNextProbeTimestamp = 0;
		m_nProbeWaitingTime = 0;
		m_nSendingWin = IKCP_WND_SND;
		m_nRecevingWin = IKCP_WND_RCV;
		m_nPeerRecevingWin = IKCP_WND_RCV;
		m_nCongestionWin = 0;
		m_nIncr = 0;
		m_nProbe = 0;
		m_nMSS = m_nMTU - IKCP_OVERHEAD;
		m_bStreamMod = true;

		m_nState = 0;
		m_nRXSmoothedRTT = 0;
		m_nRXRTTVal = 0;
		m_nRXRetrTimeout = IKCP_RTO_DEF;
		m_nRXMinRetrTimeout = IKCP_RTO_MIN;
		m_nCurrentTime = 0;
		m_nInterval = IKCP_INTERVAL;
		m_nNextFlushTimestamp = IKCP_INTERVAL;
		m_bNodelay = 0;
		m_bUpdated = 0;
		m_nLogMask = 0;
		m_nCongestionWinthrd = IKCP_THRESH_INIT;
		m_nFastResendAckThd = 0;
		m_nFastLimit = IKCP_FASTACK_LIMIT;
		m_bCancelCongestionCtrl = 0;
		m_nXMIT = 0;
		m_nDeadLink = IKCP_DEADLINK;
		nResult = true;
	}

	return nResult;
}

// check log nMask
int CKCP::Canlog(const int nMask)
{
	return (nMask & m_nLogMask);
}

// ********************************************************************************
/// <summary>
/// 应用程序调用该函数从KCP中接收数据
/// </summary>
/// <param name="buffer"></param>
/// <param name="nLen"></param>
/// <returns>实际接收到的数据长度</returns>
/// <created>Andy,2020/12/28</created>
/// <changed>Andy,2020/12/28</changed>
// ********************************************************************************
int CKCP::KCPRecv(char *pBuffer, const int nLen)
{
	if (nLen < 0 || nullptr == pBuffer)
	{
		return -1;
	}

	if (safe_containers_size<PIKCP_FRAGMENT>(m_objRecevingQueue, m_objRecevingMutex) <= 0)
	{
		return -1;
	}

	//计算当前接收队列中的属于同一个消息的数据总长度，这个长度应该比参数中的len小，如果大于，导致数据不能导出  
	const int nPeekSize = CalcNextMsgSize();

	if (nPeekSize < 0)
	{
		return -2;
	}

	if (nPeekSize > nLen)
	{
		Log(IKCP_LOG_RECV, "The input buffer is too small");
		return -3;
	}

	int nRecover = 0;

	// 接收队列segment数量大于等于接收窗口，标记窗口可以恢复
	if (GetRecevingQueueSize() >= m_nRecevingWin)
	{
		nRecover = 1;
	}

	int nResult = 0;
	const bool bPeek = (nLen < 0);

	// 合并分片,将属于同一个消息的各分片重组完整数据，并删除rcv_queue中segment，nrcv_que减少  
	{
		std::unique_lock<std::recursive_mutex> objAutoLocker(m_objRecevingMutex);

		for (auto itr = m_objRecevingQueue.begin(); m_objRecevingQueue.end() != itr;)
		{
			IKCP_FRAGMENT* pFragment = (*itr);

			memcpy(&pBuffer[nResult], pFragment->pData, pFragment->nLen);
			nResult += pFragment->nLen;

			const int nFragment = pFragment->nFrag;

			if (Canlog(IKCP_LOG_RECV))
			{
				Log(IKCP_LOG_RECV, "recv nSN=%lu", (unsigned long)pFragment->nSN);
			}

			if (!bPeek)
			{
				itr = m_objRecevingQueue.erase(itr);
				DeleteSegment(pFragment);
			}
			else
			{
				itr++;
			}

			if (nFragment == 0)
			{
				///< 说明是一段数据中的最后一个分片

				break;
			}
		}


		assert(nResult == nPeekSize);

		// 把数据从接收缓冲区移到接收队列  
		while (m_objRecevingBuffer.size())
		{
			auto itr = m_objRecevingBuffer.begin();
			IKCP_FRAGMENT* pFragment = (*itr);

			// 1. 根据 sn 确保数据是按序转移到 rcv_queue 中  
			// 2. 根据接收窗口大小来判断是否可以接收数据  

			if (pFragment->nSN == m_nSNNextRecving && GetRecevingQueueSize() < m_nRecevingWin)
			{
				m_objRecevingBuffer.erase(itr);
				m_objRecevingQueue.push_back(pFragment);
				m_nSNNextRecving++;
			}
			else
			{
				break;
			}
		}
	}

	// fast recover
	if (GetRecevingQueueSize() < m_nRecevingWin && nRecover)
	{
		// ready to send back IKCP_CMD_WINS in Flush
		// tell remote my window nSize
		m_nProbe |= IKCP_ASK_TELL;
	}

	return nResult;
}


// ********************************************************************************
/// <summary>
/// 计算当前需要的缓冲区尺寸
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/12/29</created>
/// <changed>Andy,2020/12/29</changed>
// ********************************************************************************
int CKCP::CalcNextMsgSize()
{
	std::unique_lock<std::recursive_mutex> objAutoLocker(m_objRecevingMutex);

	if (m_objRecevingQueue.size() <= 0)
	{
		return -1;
	}

	IKCP_FRAGMENT * pFirstFragment = *(m_objRecevingQueue.begin());

	if (pFirstFragment->nFrag == 0)
	{
		return pFirstFragment->nLen;
	}

	if (GetRecevingQueueSize() < size_t(pFirstFragment->nFrag + 1))
	{
		return -1;
	}

	int nLength = 0;

	for (auto pFragment : m_objRecevingQueue)
	{
		nLength += pFragment->nLen;

		if (pFragment->nFrag == 0)
		{
			break;
		}
	}

	return nLength;
}


// ********************************************************************************
/// <summary>
/// 发送数据
/// </summary>
/// <param name="buffer">指向被发送数据的缓冲区指针</param>
/// <param name="nLen">被发送数据的长度</param>
/// <returns>大于等于0标识成功发送数据的长度，否则标识出现错误</returns>
/// <created>Andy,2020/12/28</created>
/// <changed>Andy,2020/12/28</changed>
// ********************************************************************************
int CKCP::KCPSend(const char *pBuffer, const int nLen)
{
	CHECK_POINTER_EX(pBuffer,-1);
	assert(m_nMSS > 0);

	if (nLen < 0)
	{
		return -1;
	}

	std::unique_lock<std::mutex> objAutoLocker(m_objSendingMutex);

	int nRemainLen = nLen;
	IKCP_FRAGMENT *pNewFragment = nullptr;
	
	if (m_bStreamMod)
	{
		// 如果当前采用的是流模式，那么试着把数据追加到发送队列中的最后一个分片

		IKCP_FRAGMENT *pOldSegment = safe_pop_back<PIKCP_FRAGMENT>(m_objSendingQueue, m_objSendingQueueMutex);

		if (nullptr != pOldSegment)
		{
			if (pOldSegment->nLen < m_nMSS)
			{
				// 节点内数据长度小于mss，计算还可容纳的数据大小，以及本次占用的空间大小，以此新建segment，
				// 将新建segment附加到发送队列尾，将old节点内数据拷贝过去，然后将buffer中也拷贝其中，
				// 如果buffer中的数据没有拷贝完，extend为拷贝数据，开始frg计数。更新len为剩余数据，删除old  
				
				const int nCapacity = m_nMSS - pOldSegment->nLen;
				const int nExtend = (nRemainLen < nCapacity) ? nRemainLen : nCapacity;    ///< 新的分片实际需要的长度

				pNewFragment = CreateSegment(pOldSegment->nLen + nExtend);
				assert(pNewFragment);

				if (pNewFragment != nullptr)
				{
					memcpy(pNewFragment->pData, pOldSegment->pData, pOldSegment->nLen);

					if (pBuffer)
					{
						memcpy(pNewFragment->pData + pOldSegment->nLen, pBuffer, nExtend);
						pBuffer += nExtend;
					}

					pNewFragment->nLen = pOldSegment->nLen + nExtend;
					pNewFragment->nFrag = 0;
					nRemainLen -= nExtend;
					
					safe_push_back<PIKCP_FRAGMENT>(m_objSendingQueue, pNewFragment, m_objSendingQueueMutex);
					DeleteSegment(pOldSegment);
					pOldSegment = nullptr;
				}
				else
				{
					safe_push_back<PIKCP_FRAGMENT>(m_objSendingQueue, pOldSegment, m_objSendingQueueMutex);
					return -2;
				}			
			}
			else
			{
				safe_push_back<PIKCP_FRAGMENT>(m_objSendingQueue, pOldSegment, m_objSendingQueueMutex);
			}
		}

		if (nRemainLen <= 0)
		{
			return nLen;
		}
	}

	// 计算剩下的数据可以被最多分成多少个分片
	int nFragCount = 0;

	if (nRemainLen <= (int)m_nMSS)
	{
		nFragCount = 1;
	}
	else
	{
		// 下面的代码非常棒
		nFragCount = (nRemainLen + m_nMSS - 1) / m_nMSS;
	}

	if (nFragCount == 0)
	{
		nFragCount = 1;
	}

	// 分片
	for (int i = 0; i < nFragCount; i++)
	{
		const int nSize = nRemainLen > (int)m_nMSS ? (int)m_nMSS : nRemainLen;

		pNewFragment = CreateSegment(nSize);
		assert(pNewFragment);

		if (nullptr == pNewFragment)
		{
			break;
		}

		if (pBuffer && nRemainLen > 0)
		{
			memcpy(pNewFragment->pData, pBuffer, nSize);
		}

		pNewFragment->nLen = nSize;
		pNewFragment->nFrag = U8((!m_bStreamMod) ? (nFragCount - i - 1) : 0);

		safe_push_back<PIKCP_FRAGMENT>(m_objSendingQueue, pNewFragment, m_objSendingQueueMutex);

		if (safe_containers_size<PIKCP_FRAGMENT>(m_objSendingQueue, m_objSendingQueueMutex) >= GetSendingQueueSizeMax())
		{
			// 如果发送队列已经到达上限，那么等待
			Log(IKCP_LOG_IN_DATA, "The sending queue is full!");
			m_objSendingQueueEvents.WaitForEvent();
		}

		if (pBuffer)
		{
			pBuffer += nSize;
		}

		nRemainLen -= nSize;
	}

	return (nLen - nRemainLen);
}


// ********************************************************************************
/// <summary>
/// 计算重传超时
///CaleRTO 会更新RTT和RTO等参数，该算法与TCP保持一致 :
///
/// 第一次测量，rtt 是我们测量的结果，rx_srtt = rtt，rx_rttval = rtt / 2
/// 以后每次测量：
/// rx_srtt = (1 - a) * rx_srtt + a * rtt，a取值1 / 8
/// rx_rttval = (1 - b) * rx_rttval + b * | rtt - rx_srtt | ，b取值1 / 4
/// rto = rx_srtt + 4 * rx_rttval
/// rx_rto = MIN(MAX(rx_minrto, rto), IKCP_RTO_MAX)
/// </summary>
/// <param name="nRTT"></param>
/// <created>Andy,2020/12/29</created>
/// <changed>Andy,2020/12/29</changed>
// ********************************************************************************
void CKCP::CaleRTO(const int nRTT)
{
	if (m_nRXSmoothedRTT == 0)
	{
		m_nRXSmoothedRTT = nRTT;
		m_nRXRTTVal = nRTT / 2;
	}
	else
	{
		int nDelta = nRTT - m_nRXSmoothedRTT;

		if (nDelta < 0)
		{
			nDelta = -nDelta;
		}

		m_nRXRTTVal = (3 *m_nRXRTTVal + nDelta) / 4;
		m_nRXSmoothedRTT = (7 * m_nRXSmoothedRTT + nRTT) / 8;

		if (m_nRXSmoothedRTT < 1)
		{
			m_nRXSmoothedRTT = 1;
		}
	}

	const I32 nRTO = m_nRXSmoothedRTT + _imax_(m_nInterval, 4 *m_nRXRTTVal);

	m_nRXRetrTimeout = Bound(m_nRXMinRetrTimeout, nRTO, IKCP_RTO_MAX);
}

// ********************************************************************************
/// <summary>
/// 重新计算第一个没有应答的数据包
/// </summary>
/// <created>Andy,2020/12/29</created>
/// <changed>Andy,2020/12/29</changed>
// ********************************************************************************
void CKCP::CaleSNFirstPacketNAck()
{
	std::unique_lock<std::recursive_mutex> objAutoLocker(m_objSendingQueueMutex);

	if (m_objSendingBuffer.size() > 0)
	{
		IKCP_FRAGMENT *pFragment = *(m_objSendingBuffer.begin());

		m_nSNFirstPacketNAck = pFragment->nSN;
	}
	else
	{
		m_nSNFirstPacketNAck = m_nSNNext;
	}
}

// ********************************************************************************
/// <summary>
/// 把已经被接收者确认的数据包从发送队列中删除
/// </summary>
/// <param name="nSN">被确认的分片的序号</param>
/// <created>Andy,2020/12/29</created>
/// <changed>Andy,2020/12/29</changed>
// ********************************************************************************
void CKCP::RemoveFragAcked(const U32 nSN)
{
	if (Difference(nSN, m_nSNFirstPacketNAck) < 0 || Difference(nSN, m_nSNNext) >= 0)
	{
		return;
	}

	{
		std::unique_lock<std::recursive_mutex> objAutoLocker(m_objSendingQueueMutex);

		for (auto itr = m_objSendingBuffer.begin(); itr != m_objSendingBuffer.end(); itr++)
		{
			IKCP_FRAGMENT *pFragment = (*itr);

			if (nSN == pFragment->nSN)
			{
				m_objSendingBuffer.erase(itr);
				DeleteSegment(pFragment);
				break;
			}

			if (Difference(nSN, pFragment->nSN) < 0)
			{
				break;
			}
		}
	}
}

// ********************************************************************************
/// <summary>
/// 把已经被接收者确认的数据包从发送队列中删除（第二种确认方式）
/// 序号小于nSN的分片，表明被接收端正确的接收，可以把他们从发送缓冲区中删除了
/// </summary>
/// <param name="nSN">期望的下一个SN</param>
/// <created>Andy,2020/12/29</created>
/// <changed>Andy,2020/12/30</changed>
// ********************************************************************************
void CKCP::RemoveFragUnAcked(const U32 nSN)
{
	std::unique_lock<std::recursive_mutex> objAutoLocker(m_objSendingQueueMutex);

	for (auto itr = m_objSendingBuffer.begin(); itr != m_objSendingBuffer.end();)
	{
		IKCP_FRAGMENT *pFragment = *itr;

		if (Difference(nSN, pFragment->nSN) > 0)
		{
			itr =m_objSendingBuffer.erase(itr);
			DeleteSegment(pFragment);
		}
		else
		{
			break;
		}
	}
}

// ********************************************************************************
/// <summary>
/// 计算分片被跳过的次数
/// 发送队列中所有小于序号nSN分片的nFastACK增加,该数字越大表明该分片丢失的可能性就越大
/// </summary>
/// <param name="nSN"></param>
/// <param name="nTimestamp"></param>
/// <created>Andy,2020/12/29</created>
/// <changed>Andy,2020/12/29</changed>
// ********************************************************************************
void CKCP::CalcFaskAck(const U32 nSN, const U32 /*nTimestamp*/)
{
	if (Difference(nSN, m_nSNFirstPacketNAck) < 0 || Difference(nSN, m_nSNNext) >= 0)
	{
		return;
	}

	{
		std::unique_lock<std::recursive_mutex> objAutoLocker(m_objSendingQueueMutex);

		for (auto pFragment : m_objSendingBuffer)
		{
			if (Difference(nSN, pFragment->nSN) < 0)
			{
				break;
			}
			else if (nSN != pFragment->nSN)
			{
#ifndef IKCP_FASTACK_CONSERVE
				pFragment->nFastACK++;
#else
				if (Difference(nTimestamp, pFragment->nTimestamp) >= 0)
				{
					pFragment->nFastACK++;
				}
#endif
			}
		}
	}
}


// ********************************************************************************
/// <summary>
/// 添加一个ACK到列表中
/// </summary>
/// <param name="nSN">新收到报文序号</param>
/// <param name="nTimestamp">新收到报文的时间</param>
/// <created>Andy,2020/12/29</created>
/// <changed>Andy,2020/12/29</changed>
// ********************************************************************************
void CKCP::PushACK(const U32 nSN, const U32 nTimestamp)
{
	m_queACKList.enqueue(CACKRec(nSN, nTimestamp));
}

// ********************************************************************************
/// <summary>
/// 从ACK列表中获取一个指定的ACK
/// </summary>
/// <param name="nIndex">索引</param>
/// <param name="refSN">返回报文序号</param>
/// <param name="refTimestamp">返回报文发送的时间</param>
/// <created>Andy,2020/12/29</created>
/// <changed>Andy,2020/12/30</changed>
// ********************************************************************************
bool CKCP::GetACK(CACKRec& refACKRec)
{
	const bool bResult = m_queACKList.try_dequeue(refACKRec);

	return bResult;
}


// ********************************************************************************
/// <summary>
/// 保存新的分片到接收缓冲区
/// </summary>
/// <param name="pNewFragment">指向新分片的指针</param>
/// <created>Andy,2020/12/29</created>
/// <changed>Andy,2020/12/29</changed>
// ********************************************************************************
void CKCP::StoreNewFrag(IKCP_FRAGMENT *pNewFragment)
{
	const U32 nSN = pNewFragment->nSN;
	bool bRepeat = false;

	if (Difference(nSN, m_nSNNextRecving + m_nRecevingWin) >= 0
		|| Difference(nSN, m_nSNNextRecving) < 0)
	{
		DeleteSegment(pNewFragment);
		return;
	}

	std::unique_lock<std::recursive_mutex> objAutoLocker(m_objRecevingMutex);
	// 下面代码重点测试
	int nCount = 0;
	auto itr = m_objRecevingBuffer.rbegin();

	for (; m_objRecevingBuffer.rend() != itr;itr++)
	{
		IKCP_FRAGMENT* pFragment = (*itr);

		if (nSN > pFragment->nSN)
		{
			break;
		}
		else if (nSN == pFragment->nSN)
		{
			bRepeat = true;
			break;
		}

		nCount++;
	}

	if (!bRepeat)
	{
		m_objRecevingBuffer.insert(m_objRecevingBuffer.end() - nCount, pNewFragment);
	}
	else 
	{
		DeleteSegment(pNewFragment);
	}



	// move available Data from m_objRecevingBuffer -> m_objRecevingQueue
	while (m_objRecevingBuffer.size() > 0)
	{
		auto first_itr = m_objRecevingBuffer.begin();
		IKCP_FRAGMENT *pFragment = *(first_itr);

		if (pFragment->nSN == m_nSNNextRecving && GetRecevingQueueSize() < m_nRecevingWin)
		{
			m_objRecevingBuffer.erase(first_itr);
			m_objRecevingQueue.push_back(pFragment);
			m_nSNNextRecving++;
		}
		else
		{
			break;
		}
	}
}

// ********************************************************************************
/// <summary>
/// 处理收到的ACK命令
/// </summary>
/// <param name="nSN"></param>
/// <param name="nTimestamp"></param>
/// <param name="nMaxACK"></param>
/// <param name="nFlag"></param>
/// <param name="nLastTimestamp"></param>
/// <created>Andy,2020/12/30</created>
/// <changed>Andy,2020/12/30</changed>
// ********************************************************************************
void CKCP::OnACK(IKCP_FRAGMENT objFragment, U32& nMaxACK, int& nFlag, U32& nLastTimestamp)
{
	if (Difference(m_nCurrentTime, objFragment.nTimestamp) >= 0)
	{
		CaleRTO(Difference(m_nCurrentTime, objFragment.nTimestamp));
	}

	//遍历snd_buf中（snd_una, snd_nxt），将sn相等的删除，直到大于sn
	RemoveFragAcked(objFragment.nSN);
	CaleSNFirstPacketNAck();

	if (nFlag == 0)
	{
		nFlag = 1;
		nMaxACK = objFragment.nSN;
		nLastTimestamp = objFragment.nTimestamp;
	}
	else
	{
		if (Difference(objFragment.nSN, nMaxACK) > 0)
		{
#ifndef IKCP_FASTACK_CONSERVE
			nMaxACK = objFragment.nSN;
			nLastTimestamp = objFragment.nTimestamp;
#else
			if (Difference(objFragment.nTimestamp, nLastTimestamp) > 0)
			{
				nMaxACK = objFragment.nSN;
				nLastTimestamp = objFragment.nTimestamp;
			}
#endif
		}
	}

	if (Canlog(IKCP_LOG_IN_ACK))
	{
		Log(IKCP_LOG_IN_ACK
			, "input ack: nSN=%lu nRTT=%ld nRTO=%ld"
			, (unsigned long)objFragment.nSN
			, (long)Difference(m_nCurrentTime, objFragment.nTimestamp)
			, (long)m_nRXRetrTimeout);
	}
}

// ********************************************************************************
/// <summary>
/// 处理远端发送来的数据
/// </summary>
/// <param name="objFragment"></param>
/// <param name="pData"></param>
/// <param name="nLen"></param>
/// <created>Andy,2020/12/30</created>
/// <changed>Andy,2020/12/30</changed>
// ********************************************************************************
void CKCP::OnPush(IKCP_FRAGMENT objFragment, const char *pData)
{
	if (Canlog(IKCP_LOG_IN_DATA))
	{
		Log(IKCP_LOG_IN_DATA, "input psh: nSN=%lu nTimestamp=%lu", (unsigned long)objFragment.nSN, (unsigned long)objFragment.nTimestamp);
	}

	if (Difference(objFragment.nSN, m_nSNNextRecving + m_nRecevingWin) < 0)
	{
		PushACK(objFragment.nSN, objFragment.nTimestamp);

		if (Difference(objFragment.nSN, m_nSNNextRecving) >= 0)
		{
			IKCP_FRAGMENT * pFragment = CreateSegment(objFragment.nLen);

			pFragment->nConversationID = objFragment.nConversationID;
			pFragment->nCMD = objFragment.nCMD;
			pFragment->nFrag = objFragment.nFrag;
			pFragment->nWnd = objFragment.nWnd;
			pFragment->nTimestamp = objFragment.nTimestamp;
			pFragment->nSN = objFragment.nSN;
			pFragment->nUnAckSN = objFragment.nUnAckSN;
			pFragment->nLen = objFragment.nLen;

			if (objFragment.nLen > 0)
			{
				memcpy(pFragment->pData, pData, objFragment.nLen);
			}

			//1. 丢弃sn > kcp->rcv_nxt + kcp->rcv_wnd的segment;
//2. 逐一比较rcv_buf中的segment，若重复丢弃，非重复，新建segment加入;
//3. 检查rcv_buf的包序号sn，如果是待接收的序号rcv_nxt，且可以接收（接收队列小 于接收窗口），转移segment到rcv_buf，nrcv_buf减少，nrcv_que增加，rcv_nxt增加;

			StoreNewFrag(pFragment);
		}
	}
}

// ********************************************************************************
/// <summary>
/// 应用把从底层收到的数据输入KCP
/// 该函数假定收到的数据是从一个完整的数据报文开始的
/// </summary>
/// <param name="pData">指向数据的指针</param>
/// <param name="nSize">指向数据的长度</param>
/// <returns></returns>
/// <created>Andy,2020/12/28</created>
/// <changed>Andy,2020/12/28</changed>
// ********************************************************************************
int CKCP::KCPInput(const char *pData, const int nInLen)
{
	int nSize = nInLen;

	if (Canlog(IKCP_LOG_INPUT))
	{
		Log(IKCP_LOG_INPUT, "[RI] %d bytes", (int)nSize);
	}

	if (pData == NULL || (int)nSize < (int)IKCP_OVERHEAD)
	{
		Log(IKCP_LOG_INPUT, "[RI] Invalid packet!");
		return -1;
	}

	U32 nPrevUNA = m_nSNFirstPacketNAck;
	U32 nMaxACK = 0;
	U32 nLastTimestamp = 0;
	int nFlag = 0;

	while (true)
	{		
		IKCP_FRAGMENT objFragment;

		if (nSize < (int)IKCP_OVERHEAD)
		{
			Log(IKCP_LOG_INPUT, "[RI] Invalid packet!");
			break;
		}

		pData = ikcp_decode32u(pData, &objFragment.nConversationID);

		if (m_nConversationID != objFragment.nConversationID)
		{
			return -1;
		}

		pData = ikcp_decode8u(pData, &objFragment.nCMD);
		pData = ikcp_decode8u(pData, &objFragment.nFrag);
		pData = ikcp_decode16u(pData, &objFragment.nWnd);
		pData = ikcp_decode32u(pData, &objFragment.nTimestamp);
		pData = ikcp_decode32u(pData, &objFragment.nSN);
		pData = ikcp_decode32u(pData, &objFragment.nUnAckSN);
		pData = ikcp_decode32u(pData, &objFragment.nLen);

		nSize -= IKCP_OVERHEAD;

		if ((long)nSize < (long)objFragment.nLen || (int)objFragment.nLen < 0)
		{
			return -2;
		}

		if (objFragment.nCMD != IKCP_CMD_PUSH
			&& objFragment.nCMD != IKCP_CMD_ACK
			&& objFragment.nCMD != IKCP_CMD_WASK
			&& objFragment.nCMD != IKCP_CMD_WINS)
		{
			return -3;
		}

		m_nPeerRecevingWin = objFragment.nWnd;
		RemoveFragUnAcked(objFragment.nUnAckSN);   ///<  删除小于snd_buf中小于una的segment
		CaleSNFirstPacketNAck();          ///< 更新snd_una为snd_buf中seg->sn或kcp->snd_nxt

		switch(objFragment.nCMD)
		{
		case IKCP_CMD_ACK:
		{
			OnACK(objFragment,nMaxACK, nFlag,nLastTimestamp);
		}
		break;

		case IKCP_CMD_PUSH:
		{
			OnPush(objFragment, pData);
		}
		break;

		case IKCP_CMD_WASK:
		{
			// ready to send back IKCP_CMD_WINS in Flush
			// tell remote my window nSize
			m_nProbe |= IKCP_ASK_TELL;

			if (Canlog(IKCP_LOG_IN_PROBE))
			{
				Log(IKCP_LOG_IN_PROBE, "input m_nProbe");
			}
		}
		break;

		case IKCP_CMD_WINS:
		{
			// do nothing
			if (Canlog(IKCP_LOG_IN_WINS))
			{
				Log(IKCP_LOG_IN_WINS, "input wins: %lu", (unsigned long)(objFragment.nWnd));
			}
		}
		break;

		default:
			return -3;
		}

		pData += objFragment.nLen;
		nSize -= objFragment.nLen;
	}

	if (nFlag != 0)
	{
		//sn 大于snd_buf中包序号，可能有丢包发生

		CalcFaskAck(nMaxACK, nLastTimestamp);
	}

	// // 如果m_nSNFirstPacketNAck增加了那么就说明对端正常收到且回应了发送方发送缓冲区第一个待确认的包，此时需要更新cwnd（拥塞窗口）
	if (Difference(m_nSNFirstPacketNAck, nPrevUNA) > 0)
	{
		if (m_nCongestionWin < m_nPeerRecevingWin)
		{
			const U32 nMSS = m_nMSS;

			if (m_nCongestionWin < m_nCongestionWinthrd)
			{
				m_nCongestionWin++;
				m_nIncr += nMSS;
			}
			else
			{
				if (m_nIncr < nMSS)
				{
					m_nIncr = nMSS;
				}

				m_nIncr += (nMSS * nMSS) / m_nIncr + (nMSS / 16);

				if ((m_nCongestionWin + 1) * nMSS <= m_nIncr)
				{
#if 1
					m_nCongestionWin = (m_nIncr + nMSS - 1) / ((nMSS > 0) ? nMSS : 1);
#else
					m_nCongestionWin++;
#endif
				}
			}

			if (m_nCongestionWin > m_nPeerRecevingWin)
			{
				m_nCongestionWin = m_nPeerRecevingWin;
				m_nIncr = m_nPeerRecevingWin * nMSS;
			}
		}
	}

	return 0;
}


// ********************************************************************************
/// <summary>
/// 把新的分片信心编码大指定的缓冲区中
/// </summary>
/// <param name="pData">输出缓冲区</param>
/// <param name="pFragment">指向分片的指针</param>
/// <returns></returns>
/// <created>Andy,2020/12/28</created>
/// <changed>Andy,2020/12/28</changed>
// ********************************************************************************
char *CKCP::EncodeFrag(const IKCP_FRAGMENT *pFragment,char *pData)
{
	pData = ikcp_encode32u(pData, pFragment->nConversationID);
	pData = ikcp_encode8u(pData, (U8)pFragment->nCMD);
	pData = ikcp_encode8u(pData, (U8)pFragment->nFrag);
	pData = ikcp_encode16u(pData, (U16)pFragment->nWnd);
	pData = ikcp_encode32u(pData, pFragment->nTimestamp);
	pData = ikcp_encode32u(pData, pFragment->nSN);
	pData = ikcp_encode32u(pData, pFragment->nUnAckSN);
	pData = ikcp_encode32u(pData, pFragment->nLen);

	return pData;
}

// ********************************************************************************
/// <summary>
/// 得到空闲窗口的尺寸
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/12/30</created>
/// <changed>Andy,2020/12/30</changed>
// ********************************************************************************
int CKCP::GetFreeRecevingWinSize()
{
	if (GetRecevingQueueSize() < m_nRecevingWin) 
	{
		return m_nRecevingWin - GetRecevingQueueSize();
	}

	return 0;
}


// ********************************************************************************
/// <summary>
/// Flush
/// </summary>
/// <created>Andy,2020/12/29</created>
/// <changed>Andy,2020/12/29</changed>
// ********************************************************************************
void CKCP::Flush()
{
	U32 nCurrentTime = m_nCurrentTime;
	char *pBufferHeadPtr = m_pBuffer;
	char *pBufferTailPtr = pBufferHeadPtr;
	int nSize = 0;
	U32 nResentThd =0;
	U32 nCongestionWin = 0;
	U32 nMinRTO = 0;
	int bChange = 0;
	int bLost = 0;
	IKCP_FRAGMENT objFragment;

	// 'KCPUpdate' haven't been called. 
	if (m_bUpdated == 0)
	{
		return;
	}

	objFragment.nConversationID = m_nConversationID;
	objFragment.nCMD = IKCP_CMD_ACK;
	objFragment.nFrag = 0;
	objFragment.nWnd = U16(GetFreeRecevingWinSize());
	objFragment.nUnAckSN = m_nSNNextRecving;
	objFragment.nLen = 0;
	objFragment.nSN = 0;
	objFragment.nTimestamp = 0;

	// 逐一获取acklist中的sn和ts，编码成segment,以流的方式凑够mtu再发送  
	// flush acknowledges
	do
	{
		nSize = (int)(pBufferTailPtr - pBufferHeadPtr);

		if (nSize + (int)IKCP_OVERHEAD > (int)m_nMTU)
		{
			KCPOutput(pBufferHeadPtr, nSize);
			pBufferTailPtr = pBufferHeadPtr;
		}
		
		CACKRec objACKRec;
		const bool bSuccess = GetACK(objACKRec);

		if (!bSuccess)
		{
			break;
		}

		objFragment.nSN = objACKRec.m_nSN;
		objFragment.nTimestamp = objACKRec.m_nTimestamp;
		pBufferTailPtr = EncodeFrag(&objFragment,pBufferTailPtr);

	} while (true);


	// probe window size (if remote window size equals zero)  
	if (m_nPeerRecevingWin == 0)
	{
		if (m_nProbeWaitingTime == 0)
		{
			m_nProbeWaitingTime = IKCP_PROBE_INIT;
			m_nNextProbeTimestamp = m_nCurrentTime + m_nProbeWaitingTime;
		}
		else
		{
			//远端窗口为0，发送过探测请求，但是已经超过下次探测的时间  
			//更新probe_wait，增加为IKCP_PROBE_INIT+  probe_wait /2,但满足KCP_PROBE_LIMIT 
			//更新下次探测时间 ts_probe与 探测变量 为 IKCP_ASK_SEND，立即发送探测消息  

			if (Difference(m_nCurrentTime, m_nNextProbeTimestamp) >= 0)
			{
				if (m_nProbeWaitingTime < IKCP_PROBE_INIT)
				{
					m_nProbeWaitingTime = IKCP_PROBE_INIT;
				}

				m_nProbeWaitingTime += m_nProbeWaitingTime / 2;

				if (m_nProbeWaitingTime > IKCP_PROBE_LIMIT)
				{
					m_nProbeWaitingTime = IKCP_PROBE_LIMIT;
				}

				m_nNextProbeTimestamp = m_nCurrentTime + m_nProbeWaitingTime;
				m_nProbe |= IKCP_ASK_SEND;
			}
		}
	}
	else 
	{
		// 远端窗口不等于0，更新下次探测时间与探测窗口等待时间为0，不发送窗口探测  

		m_nNextProbeTimestamp = 0;
		m_nProbeWaitingTime = 0;
	}

	// flush window probing commands
	if (m_nProbe & IKCP_ASK_SEND)
	{
		objFragment.nCMD = IKCP_CMD_WASK;
		nSize = (int)(pBufferTailPtr - pBufferHeadPtr);

		if (nSize + (int)IKCP_OVERHEAD > (int)m_nMTU)
		{
			KCPOutput(pBufferHeadPtr, nSize);
			pBufferTailPtr = pBufferHeadPtr;
		}

		pBufferTailPtr = EncodeFrag(&objFragment,pBufferTailPtr);
	}

	// flush window probing commands
	if (m_nProbe & IKCP_ASK_TELL)
	{
		objFragment.nCMD = IKCP_CMD_WINS;
		nSize = (int)(pBufferTailPtr - pBufferHeadPtr);

		if (nSize + (int)IKCP_OVERHEAD > (int)m_nMTU)
		{
			KCPOutput(pBufferHeadPtr, nSize);
			pBufferTailPtr = pBufferHeadPtr;
		}

		pBufferTailPtr = EncodeFrag(&objFragment,pBufferTailPtr);
	}

	m_nProbe = 0;

	// calculate window size  
	// 如果没有流控，窗口为发送窗口与远程窗口的最小值  
	// 如果存在流控，窗口为当前拥塞窗口、发送窗口，远程接收窗口三者最小值  
	nCongestionWin = _imin_(m_nSendingWin, m_nPeerRecevingWin);
	
	if (m_bCancelCongestionCtrl == 0)
	{
		nCongestionWin = _imin_(m_nCongestionWin,nCongestionWin);
	}

	// move data from m_objSendingQueue to m_objSendingBuffer
	// 从发送队列移动到发送缓冲区的分片数量不能超出对方的接收能力  
	while (Difference(m_nSNNext, m_nSNFirstPacketNAck + nCongestionWin) < 0)
	{
		IKCP_FRAGMENT * pNewFragment = safe_pop_front<PIKCP_FRAGMENT>(m_objSendingQueue, m_objSendingQueueMutex);

		if (nullptr != pNewFragment)
		{
			m_objSendingQueueEvents.Notify();

			pNewFragment->nConversationID = m_nConversationID;
			pNewFragment->nCMD = IKCP_CMD_PUSH;
			pNewFragment->nWnd = objFragment.nWnd;
			pNewFragment->nTimestamp = nCurrentTime;
			pNewFragment->nSN = m_nSNNext++;
			pNewFragment->nUnAckSN = m_nSNNextRecving;
			pNewFragment->nResendTimestamp = nCurrentTime;
			pNewFragment->nRTO = m_nRXRetrTimeout;
			pNewFragment->nFastACK = 0;
			pNewFragment->nXMIT = 0;

			safe_push_back<PIKCP_FRAGMENT>(m_objSendingBuffer, pNewFragment, m_objSendingQueueMutex);
		}
		else
		{
			break;
		}
	}

	// calculate resent  
	nResentThd = (m_nFastResendAckThd > 0) ? (U32)m_nFastResendAckThd : 0xffffffff;
	nMinRTO = (m_bNodelay == 0) ? (m_nRXRetrTimeout >> 3) : 0;

	// flush data segments  
	{
		std::unique_lock<std::recursive_mutex> objAutoLocker(m_objSendingQueueMutex);

		for (auto itr = m_objSendingBuffer.begin(); itr != m_objSendingBuffer.end(); itr++)
		{
			IKCP_FRAGMENT *pCurFragment = (*itr);
			int nNeedsending = 0;

			//该segment 第一次发送  
			if (pCurFragment->nXMIT == 0)
			{
				nNeedsending = 1;
				pCurFragment->nXMIT++;
				pCurFragment->nRTO = m_nRXRetrTimeout;
				pCurFragment->nResendTimestamp = nCurrentTime + pCurFragment->nRTO + nMinRTO;
			}
			else if (Difference(nCurrentTime, pCurFragment->nResendTimestamp) >= 0)
			{
				//当前时间达到了重发时间，但并没有新的包到达，出现丢包, 重传  

				nNeedsending = 1;
				pCurFragment->nXMIT++;
				m_nXMIT++;

				if (m_bNodelay == 0)
				{
					pCurFragment->nRTO += _imax_(pCurFragment->nRTO, (U32)m_nRXRetrTimeout);
				}
				else
				{
					I32 nStep = (m_bNodelay < 2) ? ((I32)(pCurFragment->nRTO)) : m_nRXRetrTimeout;
					pCurFragment->nRTO += nStep / 2;
				}

				pCurFragment->nResendTimestamp = nCurrentTime + pCurFragment->nRTO;
				bLost = 1;
			}
			else if (pCurFragment->nFastACK >= nResentThd)
			{
				//segment的累计被跳过次数大于快速重传设定，需要重传  

				if ((int)pCurFragment->nXMIT <= m_nFastLimit || m_nFastLimit <= 0)
				{
					nNeedsending = 1;
					pCurFragment->nXMIT++;
					pCurFragment->nFastACK = 0;
					pCurFragment->nResendTimestamp = nCurrentTime + pCurFragment->nRTO;
					bChange++;
				}
			}

			if (nNeedsending)
			{
				int nNeed = 0;
				pCurFragment->nTimestamp = nCurrentTime;
				pCurFragment->nWnd = objFragment.nWnd;
				pCurFragment->nUnAckSN = m_nSNNextRecving;

				nSize = (int)(pBufferTailPtr - pBufferHeadPtr);
				nNeed = IKCP_OVERHEAD + pCurFragment->nLen;

				if (nSize + nNeed > (int)m_nMTU)
				{
					KCPOutput(pBufferHeadPtr, nSize);
					pBufferTailPtr = pBufferHeadPtr;
				}

				pBufferTailPtr = EncodeFrag(pCurFragment,pBufferTailPtr);

				if (pCurFragment->nLen > 0)
				{
					memcpy(pBufferTailPtr, pCurFragment->pData, pCurFragment->nLen);
					pBufferTailPtr += pCurFragment->nLen;
				}

				if (pCurFragment->nXMIT >= m_nDeadLink)
				{
					m_nState = (U32)-1;
				}
			}
		}
	}

	// flash remain segments
	nSize = (int)(pBufferTailPtr - pBufferHeadPtr);
	
	if (nSize > 0) 
	{
		KCPOutput(pBufferHeadPtr, nSize);
	}

	// update m_nCongestionWinthrd
	if (bChange) 
	{
		const U32 nInflight = m_nSNNext - m_nSNFirstPacketNAck;

		m_nCongestionWinthrd = nInflight / 2;

		if (m_nCongestionWinthrd < IKCP_THRESH_MIN)
		{
			m_nCongestionWinthrd = IKCP_THRESH_MIN;
		}

		m_nCongestionWin = m_nCongestionWinthrd + nResentThd;
		m_nIncr = m_nCongestionWin * m_nMSS;
	}

	if (bLost)
	{
		m_nCongestionWinthrd = nCongestionWin / 2;

		if (m_nCongestionWinthrd < IKCP_THRESH_MIN)
		{
			m_nCongestionWinthrd = IKCP_THRESH_MIN;
		}

		m_nCongestionWin = 1;
		m_nIncr = m_nMSS;
	}

	if (m_nCongestionWin < 1)
	{
		m_nCongestionWin = 1;
		m_nIncr = m_nMSS;
	}
}


// ********************************************************************************
/// <summary>
/// update m_nState (call it repeatedly, every 10ms-100ms), or you can ask
/// KCPCheck when to call it again (without KCPInput/_send calling).
/// 'm_nCurrentTime' - m_nCurrentTime timestamp in millisec.
/// </summary>
/// <param name="nCurrentTime"></param>
/// <created>Andy,2020/12/29</created>
/// <changed>Andy,2020/12/29</changed>
// ********************************************************************************
void CKCP::KCPUpdate(const U32 nCurrentTime)
{
	I32 nSlap = 0;

	m_nCurrentTime = nCurrentTime;

	if (m_bUpdated == 0) 
	{
		m_bUpdated = 1;
		m_nNextFlushTimestamp = m_nCurrentTime;
	}

	nSlap = Difference(m_nCurrentTime, m_nNextFlushTimestamp);

	if (nSlap >= 10000 || nSlap < -10000) 
	{
		m_nNextFlushTimestamp = m_nCurrentTime;
		nSlap = 0;
	}

	if (nSlap >= 0) 
	{
		m_nNextFlushTimestamp += m_nInterval;
		
		if (Difference(m_nCurrentTime, m_nNextFlushTimestamp) >= 0)
		{
			m_nNextFlushTimestamp = m_nCurrentTime + m_nInterval;
		}

		Flush();
	}
}


// ********************************************************************************
/// <summary>
/// Determine when should you invoke KCPUpdate:
/// returns when you should invoke KCPUpdate in millisec, if there
/// is no KCPInput/_send calling. you can call KCPUpdate in that
/// time, instead of call update repeatly.
/// Important to reduce unnacessary KCPUpdate invoking. use it to
/// schedule KCPUpdate (eg. implementing an epoll-like mechanism,
/// or optimize KCPUpdate when handling massive kcp connections)
/// </summary>
/// <param name="nCurrentTime"></param>
/// <returns></returns>
/// <created>Andy,2020/12/30</created>
/// <changed>Andy,2020/12/30</changed>
// ********************************************************************************
U32 CKCP::KCPCheck(const U32 nCurrentTime)
{
	U32 nNextFlushTimestamp = m_nNextFlushTimestamp;
	I32 nTMFlush = 0x7fffffff;
	I32 nTMPacket = 0x7fffffff;
	U32 nMinimal = 0;

	if (m_bUpdated == 0)
	{
		return nCurrentTime;
	}

	if (Difference(nCurrentTime, nNextFlushTimestamp) >= 10000 ||
		Difference(nCurrentTime, nNextFlushTimestamp) < -10000)
	{
		nNextFlushTimestamp = nCurrentTime;
	}

	if (Difference(nCurrentTime, nNextFlushTimestamp) >= 0)
	{
		return nCurrentTime;
	}

	nTMFlush = Difference(nNextFlushTimestamp, nCurrentTime);

	{
		std::unique_lock<std::recursive_mutex> objAutoLocker(m_objSendingQueueMutex);

		for (auto itr = m_objSendingBuffer.begin(); itr != m_objSendingBuffer.end(); itr++)
		{
			const IKCP_FRAGMENT *pFragment = (*itr);
			const I32 nDiff = Difference(pFragment->nResendTimestamp, nCurrentTime);

			if (nDiff <= 0)
			{
				return nCurrentTime;
			}

			if (nDiff < nTMPacket)
			{
				nTMPacket = nDiff;
			}
		}
	}

	nMinimal = (U32)(nTMPacket < nTMFlush ? nTMPacket : nTMFlush);

	if (nMinimal >= m_nInterval)
	{
		nMinimal = m_nInterval;
	}

	return nCurrentTime + nMinimal;
}



int CKCP::KCPSetMTU(const int nMTU)
{
	char *pBuffer = nullptr;
	
	if (nMTU < 50 || nMTU < (int)IKCP_OVERHEAD)
	{
		return -1;
	}

	pBuffer = (char*)ikcp_malloc((nMTU + IKCP_OVERHEAD) * 3);
	
	if (pBuffer == NULL)
	{
		return -2;
	}

	m_nMTU = nMTU;
	m_nMSS = m_nMTU - IKCP_OVERHEAD;
	ikcp_free(pBuffer);
	m_pBuffer = pBuffer;

	return 0;
}

// ********************************************************************************
/// <summary>
/// 设置时间间隔
/// </summary>
/// <param name="nInterval"></param>
/// <returns></returns>
/// <created>Andy,2020/12/30</created>
/// <changed>Andy,2020/12/30</changed>
// ********************************************************************************
int CKCP::SetInterval(const int nInterval)
{
	m_nInterval = nInterval;

	if (m_nInterval > 5000)
	{
		m_nInterval = 5000;
	}
	else if (m_nInterval < 10)
	{
		m_nInterval = 10;
	}	

	return 0;
}

// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <param name="nNodelay">是否启用 nodelay模式，0不启用；1启用。</param>
/// <param name="nInterval">协议内部工作的 interval，单位毫秒，比如 10ms或者 20ms</param>
/// <param name="nResend">快速重传模式，默认0关闭，可以设置2（2次ACK跨越将会直接重传）</param>
/// <param name="nDisenableStreamCtrl">是否关闭流控，默认是0代表不关闭，1代表关闭。</param>
/// 普通模式： KCPSetNoDelay(kcp, 0, 40, 0, 0);
/// 极速模式： KCPSetNoDelay(kcp, 1, 10, 2, 1);
/// <returns></returns>
/// <created>Andy,2020/12/28</created>
/// <changed>Andy,2020/12/28</changed>
// ********************************************************************************
int CKCP::KCPSetNoDelay(
	const int nNodelay
	, const int nInterval
	, const int nResend
	, const int nDisenableStreamCtrl)
{
	if (nNodelay >= 0)
	{
		m_bNodelay = nNodelay;

		if (m_bNodelay) 
		{
			m_nRXMinRetrTimeout = IKCP_RTO_NDL;
		}
		else 
		{
			m_nRXMinRetrTimeout = IKCP_RTO_MIN;
		}
	}

	if (nInterval >= 0)
	{
		SetInterval(nInterval);	
	}

	if (nResend >= 0)
	{
		m_nFastResendAckThd = nResend;
	}

	if (nDisenableStreamCtrl >= 0)
	{
		m_bCancelCongestionCtrl = nDisenableStreamCtrl;
	}

	return 0;
}


// ********************************************************************************
/// <summary>
/// 该调用将会设置协议的最大发送窗口和最大接收窗口大小，默认为32. 这个可以理解为 
/// TCP的 SND_BUF 和 RCV_BUF，只不过单位不一样 SND/RCV_BUF 单位是字节，这个单位是包。
/// </summary>
/// <param name="nSendingWnd">发送窗口</param>
/// <param name="nRecevingWin">接收窗口</param>
/// <returns></returns>
/// <created>Andy,2020/12/28</created>
/// <changed>Andy,2020/12/28</changed>
// ********************************************************************************
int CKCP::KCPSetWinSize(const int nSendingWnd, const int nRecevingWin)
{
	if (nSendingWnd > 0)
	{
		m_nSendingWin = nSendingWnd;
	}

	if (nRecevingWin > 0)
	{   // must >= max nFragment nSize
		m_nRecevingWin = _imax_(nRecevingWin, IKCP_WND_RCV);
	}

	return 0;
}

// ********************************************************************************
/// <summary>
/// 得到等待发送的分片数量
/// </summary>
/// <returns></returns>
/// <created>Andy,2020/12/30</created>
/// <changed>Andy,2020/12/30</changed>
// ********************************************************************************
int CKCP::GetFragsBeSent()
{
	std::unique_lock<std::recursive_mutex> objAutoLocker(m_objSendingQueueMutex);

	return m_objSendingQueue.size() + m_objSendingBuffer.size();
}

// ********************************************************************************
/// <summary>
/// 清除分片列表
/// </summary>
/// <param name="refFragList"></param>
/// <created>Andy,2020/12/29</created>
/// <changed>Andy,2020/12/29</changed>
// ********************************************************************************
void CKCP::ClearFragList(IKCP_FRAGMENT_LIST & refFragList)
{
	for (auto pFragment : refFragList)
	{
		DeleteSegment(pFragment);
	}

	refFragList.clear();
}


// ********************************************************************************
/// <summary>
/// 从数据包中读取会话ID
/// </summary>
/// <param name="ptr"></param>
/// <returns></returns>
/// <created>Andy,2020/12/28</created>
/// <changed>Andy,2020/12/28</changed>
// ********************************************************************************
U32 CKCP::KCPGetConversationID(const void *ptr)
{
	U32 nConversationID = 0;

	ikcp_decode32u((const char*)ptr, &nConversationID);

	return nConversationID;
}


// ********************************************************************************
/// <summary>
///输出日志
/// </summary>
/// <param name="nMask"></param>
/// <param name="fmt"></param>
/// <param name="..."></param>
/// <created>Andy,2020/12/30</created>
/// <changed>Andy,2020/12/30</changed>
// ********************************************************************************
void CKCP::Log(int nMask, const char *fmt, ...)
{
	char szBuffer[1024] = { 0 };
	va_list argptr;

	if ((nMask & m_nLogMask) == 0)
	{
		return;
	}
	
	va_start(argptr, fmt);
	vsprintf(szBuffer, fmt, argptr);
	va_end(argptr);
	
	Writelog(szBuffer);
}

// redefine allocator
void CKCP::KCPSetAllocator(void* (*new_malloc)(size_t), void(*new_free)(void*))
{
	ikcp_malloc_hook = new_malloc;
	ikcp_free_hook = new_free;
}

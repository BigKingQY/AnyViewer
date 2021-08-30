#pragma once
#include <vector>
#include <memory>

#define  STATIC_BUFFER_SIZE 1024


/*专门用于存储不可预知长度的输出结果，比如解码*/
class COutBuffer
{
public:
	COutBuffer(int nBufferSize = 0)
	{
		if(nBufferSize > STATIC_BUFFER_SIZE)
		{
			m_pDynamicBufferPtr = new char[nBufferSize];
			if(m_pDynamicBufferPtr)
			{
				m_nBufferSize       = nBufferSize;
				memset(m_pDynamicBufferPtr,0,m_nBufferSize);
			}
		}
		else
		{
			m_nBufferSize = STATIC_BUFFER_SIZE;
			memset(m_szStaticBuffer,0,STATIC_BUFFER_SIZE);
			m_pDynamicBufferPtr = NULL;
		}
	}
	~COutBuffer()
	{
		if(m_pDynamicBufferPtr)
		{
			delete []m_pDynamicBufferPtr;
			m_pDynamicBufferPtr = NULL;
		}
	}
public:
	unsigned int GetLength()                           { return (nullptr != m_pDynamicBufferPtr)? m_nBufferSize: STATIC_BUFFER_SIZE;}

	void SetDataLen(const unsigned int nDataLen) { m_nDataLen = nDataLen; }
	
	unsigned int GetDataLen() { return m_nDataLen; }

	char* GetDataPtr()
	{
		if (m_pDynamicBufferPtr)
		{
			return m_pDynamicBufferPtr;
		}
		else
		{
			return m_szStaticBuffer;
		}
	}

	operator char*()
	{
		return GetDataPtr();
	}	

public:
	void Clear()
	{
		if(m_pDynamicBufferPtr)
		{
           memset(m_pDynamicBufferPtr,0,m_nBufferSize);
		}
		else
		{
			memset(m_szStaticBuffer,0,STATIC_BUFFER_SIZE);
		}
	}

	bool Allocat(const unsigned int nSize)
	{
		if (nullptr != m_pDynamicBufferPtr)
		{
			delete []m_pDynamicBufferPtr;
		}

		m_pDynamicBufferPtr = new char[nSize];

		if (m_pDynamicBufferPtr)
		{
			m_nBufferSize = nSize;
		}

		return (nullptr != m_pDynamicBufferPtr);
	}

private:
	char            m_szStaticBuffer[STATIC_BUFFER_SIZE] = {0};
	unsigned int    m_nBufferSize;
	char*           m_pDynamicBufferPtr;
	unsigned int    m_nDataLen;
};

#if(_MSC_VER >= 1700  || __GNUC__ > 3)
using COutBufferPtr = std::shared_ptr<COutBuffer>;
using COutBufferArray = std::vector<COutBufferPtr>;
#endif

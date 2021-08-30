/******************************************************************************
*
* PAETCPModule
* Version
* Copyright (C) 2007 AxSoft Co., Ltd.
* All Rights Reserved
* $Header: /cvsdata/vc/Common/ObjectPoolImpl.h,v 1.9 2015/09/17 07:53:59 administrator Exp $
*
*******************************************************************************
*
* Description:
******************************************************************************/
#pragma once

#include "Typedef.h"
#include "log.h"

#include <queue>
#include <mutex>
#include <fstream>
#include <sstream>

#define _WRITE_LOG



typedef struct tagMemory_Block
{
	struct tagMemory_Block *pNext;
}MEMORY_BLOCK;


template <class TObject, const unsigned int nInitSize, const unsigned int nGrowObjects>
class CMemoryPoolImpl
{
public:
	CMemoryPoolImpl()
		: m_pPoolHead(nullptr)
#ifdef _WRITE_LOG
		, m_nTotalBytes(0)
		, m_nTotalObjects(0)
#endif
	{
		m_nGrowObjects = (nGrowObjects > 4) ? nGrowObjects : 4;
		AddMemory(nInitSize);
	}
	~CMemoryPoolImpl()
	{
		DestroyMemBlock();
	}
public:
	TObject* Allocate()
	{
		std::lock_guard<std::mutex> objAutoLock(m_csLock);

		TObject* pObject = nullptr;

		if (IsEmpty())
		{
			AddMemory(nGrowObjects);
		}

		if (!IsEmpty())
		{
			pObject = m_lstFreeList.front();
			m_lstFreeList.pop();
		}

		return (pObject);
	}
	void Deallocate(TObject* pFree)
	{
		std::lock_guard<std::mutex>  objAutoLock(m_csLock);
		m_lstFreeList.push(pFree);
	}
	size_t IsEmpty()
	{
		if (m_lstFreeList.size() == 0)
		{
			return true;
		}

		return false;
	}
private:
	bool AddMemory(const unsigned int nObjects)
	{
		MEMORY_BLOCK* pNewMemoryBlock = (MEMORY_BLOCK*)malloc(
			sizeof(MEMORY_BLOCK) + (sizeof(TObject)) * nObjects);

		if (pNewMemoryBlock)
		{

#ifdef _WRITE_LOG
			m_nTotalBytes += sizeof(MEMORY_BLOCK) + (sizeof(MEMORY_BLOCK) + sizeof(TObject)) * nObjects;
			m_nTotalObjects += nObjects;

			std::ostringstream  ossMsg;

			ossMsg << typeid(TObject).name()
				<< ": The Class Mem Pool has allocated memroy ("
				<< sizeof(MEMORY_BLOCK) + sizeof(TObject) * nObjects
				<< "Bytes, total bytes: "
				<< m_nTotalBytes / 1024
				<< " Kb,total object = "
				<< m_nTotalObjects
				<< "\r\n";

			LOG_DEBUG("%s",ossMsg.str().c_str());
#endif

			pNewMemoryBlock->pNext = m_pPoolHead;
			m_pPoolHead = pNewMemoryBlock;

			char* pNewObjectsPtr = ((char*)pNewMemoryBlock + sizeof(MEMORY_BLOCK));
			for (unsigned int i = 0; i < nObjects; i++)   //把新分配的对象增加到空闲列表
			{
				m_lstFreeList.push((TObject*)pNewObjectsPtr);
				pNewObjectsPtr += sizeof(TObject);
			}
		}

		return (nullptr != pNewMemoryBlock);
	}
private:
	void DestroyMemBlock()
	{
		MEMORY_BLOCK* pFreedMemBlock = nullptr;

		while (nullptr != m_pPoolHead)
		{
			pFreedMemBlock = m_pPoolHead;
			m_pPoolHead = m_pPoolHead->pNext;
			free(pFreedMemBlock);
		}
	}

private:
	MEMORY_BLOCK*            m_pPoolHead;  //内存池头
	std::queue<TObject*>     m_lstFreeList;
	unsigned int             m_nGrowObjects;
	std::mutex               m_csLock;

#ifdef _WRITE_LOG
	unsigned int             m_nTotalBytes;
	unsigned int             m_nTotalObjects;
#endif

};

template <class TObject, unsigned int nInitSize, unsigned int nGrowObjects>
class CEnableMemoryPoolImpl
{
public:
	void* operator new(size_t nBytes)
	{
		if (nBytes != sizeof(TObject))
		{
			return ::operator new(nBytes);
		}
		else
		{
			return (GetPool().Allocate());
		}
	}
	void operator delete(void* pFree, size_t nBytes)
	{
		if (!pFree) return;
		if (nBytes != sizeof(TObject))
		{
			::operator delete(pFree);
			return;
		}
		GetPool().Deallocate((TObject*)pFree);
	}
	static CMemoryPoolImpl<TObject, nInitSize, nGrowObjects>& GetPool()
	{
		static CMemoryPoolImpl<TObject, nInitSize, nGrowObjects> objMemroyObjectsPool;
		return objMemroyObjectsPool;
	}
};

#define USE_CLASS_MEMORY_POOL(TYPE, INIT_SIZE, GROW_OBJECTS) \
public: \
	void* operator new(size_t nBytes) \
{ \
	if(nBytes != sizeof(TYPE)) \
{ \
	return ::operator new(nBytes); \
} \
	else \
{ \
	return (GetPool().Allocate()); \
} \
} \
	void operator delete(void* pFree,size_t nBytes) \
{ \
	if(!pFree) return; \
	if(nBytes != sizeof(TYPE)) \
{ \
	::operator delete(pFree); \
	return; \
} \
	GetPool().Deallocate((TYPE*)pFree); \
} \
	static CMemoryPoolImpl<TYPE,INIT_SIZE,GROW_OBJECTS>& GetPool() \
{ \
static CMemoryPoolImpl<TYPE,INIT_SIZE,GROW_OBJECTS> obj##TYPE##ObjectsPool; \
	return (obj##TYPE##ObjectsPool); \
} \


#define INIT_MEMORY_POOL(TYPE) \
class TYPE##InitFatory \
{ \
public: \
	TYPE## InitFatory() \
{ \
	TYPE##::GetPool(); \
} \
}; \
	TYPE## InitFatory g_obj##TYPE##InitFatoryObj;




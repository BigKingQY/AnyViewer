//
//  TypeDefine.h
//  AnyViewer
//
//  Created by Aomei on 2021/8/31.
//

#pragma once
#include <stdint.h>
#include <queue>
#include <mutex>
#include <fstream>
#include <sstream>

#ifndef WIN32
#ifndef TRUE
#define TRUE                true
#endif

#ifndef FALSE
#define FALSE                false
#endif


#ifndef UINT
typedef uint32_t        UINT;
#endif

#ifndef INT
typedef int32_t        INT;
#endif

#ifndef LONGLONG
typedef int64_t LONGLONG;
#endif

#ifndef ULONGLONG
typedef uint64_t ULONGLONG;
#endif

#ifndef WORD
typedef uint16_t      WORD;
#endif

#ifndef DWORD
typedef uint32_t      DWORD;
#endif

#ifndef BYTE
typedef unsigned char       BYTE;
#endif

#ifndef NULL
#define NULL                nullptr
#endif

#ifndef HANDLE
typedef void *HANDLE;
#endif
#endif


#ifndef I8
typedef int8_t                I8;
#endif

#ifndef U8
typedef uint8_t                U8;
#endif

#ifndef I16
typedef int16_t                I16;
#endif

#ifndef U16
typedef uint16_t            U16;
#endif

#ifndef I32
typedef int32_t                I32;
#endif

#ifndef U32
typedef uint32_t            U32;
#endif

#ifndef I64
typedef int64_t                I64;
#endif

#ifndef U64
typedef uint64_t            U64;
#endif

#ifndef USHORT
typedef uint16_t                USHORT;
#endif

#ifndef UCHAR
typedef unsigned char                UCHAR;
#endif



#ifndef BYTE
typedef unsigned char       BYTE;
#endif


typedef struct
{
    unsigned int    nValue;
    const char*     lpDesc;
}ID2DESC;




#define DECLARE_REF_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
public: \
    TYPE Get##PROPERTY_NAME() const\
    { \
        return MEMBER_NAME; \
    } \
    
#define DECLARE_REF_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
public: \
    void Set##PROPERTY_NAME(const TYPE& nVal) \
    { \
        MEMBER_NAME = nVal; \
    } \

#define DECLARE_REF_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_REF_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_REF_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \

#define DECLARE_REF_MEMBER_AND_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
protected: \
    TYPE MEMBER_NAME; \
DECLARE_REF_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_REF_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \

#define DECLARE_REF_MEMBER_AND_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
protected: \
    TYPE MEMBER_NAME; \
DECLARE_REF_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \

#define DECLARE_REF_MEMBER_AND_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
protected: \
    TYPE MEMBER_NAME; \
DECLARE_REF_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \



#ifndef DECLARE_GET_METHOD
#define DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
public: \
    TYPE Get##PROPERTY_NAME() const\
    { \
        return MEMBER_NAME; \
    } \

#endif
    
#define DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
public: \
    void Set##PROPERTY_NAME(TYPE nVal) \
    { \
        MEMBER_NAME = nVal; \
    } \

#define DECLARE_SET_METHOD_EX(TYPE, MEMBER_NAME, PROPERTY_NAME,MODIFYING) \
public: \
    void Set##PROPERTY_NAME(TYPE nVal) \
    { \
    if(MEMBER_NAME != nVal) { MODIFYING = true;} \
        MEMBER_NAME = nVal; \
    } \

#define DECLARE_PROPERTY_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
protected: \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \

#define DECLARE_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
protected: \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \

#define DECLARE_MEMBER_AND_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
protected: \
    TYPE MEMBER_NAME; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \




#if(_MSC_VER >= 1700  || __GNUC__ > 3)

#define DECLARE_MEMBER_AND_METHOD_V11(TYPE, MEMBER_NAME, PROPERTY_NAME,DEFAULT_VALUE) \
protected: \
    TYPE MEMBER_NAME {DEFAULT_VALUE}; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \

#define DECLARE_MEMBER_AND_GET_METHOD_V11(TYPE, MEMBER_NAME, PROPERTY_NAME,DEFAULT_VALUE) \
protected: \
    TYPE MEMBER_NAME {DEFAULT_VALUE}; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \

#define DECLARE_MEMBER_AND_SET_METHOD_V11(TYPE, MEMBER_NAME, PROPERTY_NAME,DEFAULT_VALUE) \
protected: \
    TYPE MEMBER_NAME {DEFAULT_VALUE}; \
DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \


#define DECLARE_MEMBER_AND_METHOD11_EX(TYPE, MEMBER_NAME, PROPERTY_NAME,DEFAULT_VALUE,MODIFYING) \
protected: \
    TYPE MEMBER_NAME; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_SET_METHOD_EX(TYPE, MEMBER_NAME, PROPERTY_NAME,MODIFYING) \

#else
#define DECLARE_MEMBER_AND_METHOD_V11(TYPE, MEMBER_NAME, PROPERTY_NAME,DEFAULT_VALUE) \
protected: \
    TYPE MEMBER_NAME; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \

#define DECLARE_MEMBER_AND_GET_METHOD_V11(TYPE, MEMBER_NAME, PROPERTY_NAME,DEFAULT_VALUE) \
protected: \
    TYPE MEMBER_NAME; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \

#define DECLARE_MEMBER_AND_SET_METHOD_V11(TYPE, MEMBER_NAME, PROPERTY_NAME,DEFAULT_VALUE) \
protected: \
    TYPE MEMBER_NAME; \
DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \


#define DECLARE_MEMBER_AND_METHOD11_EX(TYPE, MEMBER_NAME, PROPERTY_NAME,DEFAULT_VALUE,MODIFYING) \
protected: \
    TYPE MEMBER_NAME; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_SET_METHOD_EX(TYPE, MEMBER_NAME, PROPERTY_NAME,MODIFYING) \

#endif

#define DECLARE_MEMBER_AND_METHOD_EX(TYPE, MEMBER_NAME, PROPERTY_NAME,MODIFYING) \
protected: \
    TYPE MEMBER_NAME; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_SET_METHOD_EX(TYPE, MEMBER_NAME, PROPERTY_NAME,MODIFYING) \

#define DECLARE_MEMBER_AND_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
protected: \
    TYPE MEMBER_NAME; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME)

#define DECLARE_MEMBER_AND_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
protected: \
    TYPE MEMBER_NAME; \
DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME)


#define DECLARE_VIRTUAL_GET_METHOD(TYPE, PROPERTY_NAME) \
public: \
    virtual TYPE Get##PROPERTY_NAME() const = 0;

#define DECLARE_VIRTUAL_SET_METHOD(TYPE, PROPERTY_NAME) \
public: \
    virtual void Set##PROPERTY_NAME(TYPE nVal) =0;

#define DECLARE_VIRTUAL_REF_SET_METHOD(TYPE, PROPERTY_NAME) \
public: \
    virtual void Set##PROPERTY_NAME(const TYPE& nVal) =0;

#define DECLARE_VIRTUAL_METHOD(TYPE, PROPERTY_NAME) \
protected: \
DECLARE_VIRTUAL_GET_METHOD(TYPE, PROPERTY_NAME) \
DECLARE_VIRTUAL_SET_METHOD(TYPE, PROPERTY_NAME) \

#define DECLARE_VIRTUAL_REF_METHOD(TYPE, PROPERTY_NAME) \
protected: \
DECLARE_VIRTUAL_GET_METHOD(TYPE, PROPERTY_NAME) \
DECLARE_VIRTUAL_REF_SET_METHOD(TYPE, PROPERTY_NAME) \



#ifndef custom_max
#define custom_max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef custom_min
#define custom_min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define CHECK_POINTER(POINTER) \
assert(nullptr != POINTER); \
if(nullptr == POINTER) \
{ return;}

#define CHECK_POINTER_EX(POINTER,RESULT) \
assert(nullptr != POINTER); \
if(nullptr == POINTER) \
{ return RESULT;}





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



#define DECLARE_MEMBER_AND_METHOD_V11(TYPE, MEMBER_NAME, PROPERTY_NAME,DEFAULT_VALUE) \
protected: \
    TYPE MEMBER_NAME {DEFAULT_VALUE}; \
DECLARE_GET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \





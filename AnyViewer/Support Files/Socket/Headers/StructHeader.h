//
//  StructHeader.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/2.
//

#ifndef StructHeader_h
#define StructHeader_h

#include "Typedef.h"

/*所有的消息都有相同的消息头,定义如下,整个消息头包含6个域：
1: 包头识别码'0XFCFDFEFF'
2: 消息唯一的ID
3: 版本号
4: 标志参考类型定义PACKET_FLAG
5: 消息所携带的载荷数据长度
6: 校验和.
----------------------------
|                          |
|                          |
| 消息头                |
-----------------------------
|                          |
| (可选数据)           |
----------------------------*/
/*------------------------消息头定义部分---------------------------------------------*/


#pragma pack(1)
/// @brief 数据包头
///
/// 专用的数据包头,用于可靠的数据通信
typedef struct  _sax2_packet_header
{
    U32                  nMagic;                  ///< 包头识别码'0xFFFEFDFC'
    U32                  nID;                      ///< 消息ID
    BYTE                nVersion;               ///< 版本号，当前是1
    BYTE                nFlags;                  ///< 标志， 前4位表示版本，紧接着4位是
    U32                  nChecksum;           ///< 校验和，采用与TCP相同计算方法，需要计算的数据包括包头和载荷数据
    U32                  nPayloadLen;          ///< 携带的数据长度
    U16                  nPaddingLen;          ///< 填充数据长度的长度
}SAX2_PACKET_HEADER;
#pragma pack()




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




typedef struct
{
    unsigned int    nValue;
    const char*     lpDesc;
}ID2DESC;


#endif /* StructHeader_h */

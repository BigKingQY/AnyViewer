//
//  BufferImpl.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//

#ifndef BufferImpl_h
#define BufferImpl_h


#pragma once
#include "Typedef.h"
#include <assert.h>
#include <string.h>

enum MEMORY_ALIGNMENT                //内存对齐方式
{
    MA_BYTES_1 = 1,
    MA_BYTES_2 = 2,
    MA_BYTES_4 = 4,
    MA_BYTES_8 = 8,
    MA_BYTES_16 = 16,
    MA_BYTES_32 = 32,
    MA_BYTES_512 = 512,
};



template <typename ElementType, MEMORY_ALIGNMENT nMemoryAligment = MA_BYTES_512>                                            //保存数组的模板类
class CBufferImplT
{
public:
    CBufferImplT()
    {
    }

    CBufferImplT(int nSize)  throw()
    {
        assert(nSize > 0);
        Allocate(nSize);
    }

    CBufferImplT(const ElementType* pData, int nSize)  throw()
    {
        assert(nSize > 0);
        Allocate(nSize);

        memcpy(m_pWorkingBuffer, pData, nSize * sizeof(ElementType));
        m_nCurWritingPos = nSize;
    }

    ~CBufferImplT()
    {
        if (!m_bAttached)
        {
            if (m_pWorkingBuffer)
            {
                delete[]m_pWorkingBuffer;
                m_pWorkingBuffer = nullptr;
                m_nSize = 0;
            }
        }
    }

public:
    unsigned int GetSize() const { return m_nSize; }
    int GetDataLen() const { return m_nCurWritingPos - m_nCurReadingPos; }

    int GetFreeSize() const { return m_nSize - m_nCurWritingPos; }

    ElementType* GetBufferHeadPtr() const { return m_pWorkingBuffer; }
    ElementType* GetBufferFreeHeadPtr() { return m_pWorkingBuffer ? &m_pWorkingBuffer[m_nCurWritingPos] : nullptr; }
    const ElementType* GetDataHeadPtr() const { return &m_pWorkingBuffer[m_nCurReadingPos]; }

    bool IsValidity() { return nullptr != m_pWorkingBuffer; }
    void IncreaseDataLen(int nDataLen)
    {
        m_nCurWritingPos += nDataLen;
        assert(m_nCurWritingPos <= m_nSize);
    }

    void MoveReadingPos(int nRelativeDisplacement)
    {
        m_nCurReadingPos += nRelativeDisplacement;
        if (m_nCurReadingPos <= 0)
        {
            m_nCurReadingPos = 0;
        }
        else if (m_nCurReadingPos > m_nCurWritingPos)
        {
            m_nCurReadingPos = m_nCurWritingPos;
        }
    }

    int GetCurReadingPos() const { return m_nCurReadingPos; }
    void SetCurReadingPos(unsigned int nPos) { m_nCurReadingPos = nPos; }
    int GetCurWritingPos() const { return m_nCurWritingPos; }
    void SetCurWritingPos(unsigned int nPos) { m_nCurWritingPos = nPos; }

public:
    CBufferImplT& operator=(const CBufferImplT& objSrc)
    {
        if (&objSrc == this)
        {
            return *this;
        }

        Deallocate();
        Allocate(objSrc.GetSize());
        memcpy(m_pWorkingBuffer, objSrc.GetBufferHeadPtr(), objSrc.GetSize() * sizeof(ElementType));
        m_nCurReadingPos = objSrc.GetCurReadingPos();
        m_nCurWritingPos = objSrc.GetCurWritingPos();

        return *this;
    };

    CBufferImplT(const CBufferImplT& objSrc)
    {
        Allocate(objSrc.GetSize());
        memcpy(m_pWorkingBuffer, objSrc.GetBufferHeadPtr(), objSrc.GetSize() * sizeof(ElementType));

        m_nCurReadingPos = objSrc.GetCurReadingPos();
        m_nCurWritingPos = objSrc.GetCurWritingPos();
    };

public:
    /*--------------------------------------------------------------------------
    *函数名称：FillPadding
    *描述：向当前的缓冲区中填充指定长度的数据
    *         该函数主要用于辅助加密
    *参数 : int nLen->需要填充的数据长度
    *参数 : ElementType nPadding->需要填充的数据
    *修改记录:
    *---------------------------------------------------------------------------*/
    void FillPadding(int nLen, ElementType nPadding)
    {
        if (nLen > 0)
        {
            if (nLen + m_nCurWritingPos > m_nSize)
            {
                Reallocate(nLen + m_nCurWritingPos);
            }

            for (int i = 0; i < nLen; i++)
            {
                m_pWorkingBuffer[m_nCurWritingPos] = nPadding;
                m_nCurWritingPos++;
            }
        }
    }

    /*--------------------------------------------------------------------------
    *函数名称：Write
    *描述：写数据到当前的缓冲区中
    *参数 : const ElementType* pData->一个指向被写数据的指针
    *参数 : int nLen->被写数据的长度  (注意单位是ElementType的长度)
    *参数 :bool bExtend->当没有足够的缓冲区时，是否允许扩展缓冲区（默认是允许）
    *返回值: 返回实际写入数据的长度
    *修改记录:
    *---------------------------------------------------------------------------*/
    int Write(const ElementType* pData, int nLen, bool bExtend = true)
    {
        assert(pData && nLen);
        if (nullptr == pData || 0 == nLen)
        {
            return 0;
        }

        /*
        下面的代码是判断是否有足够的空间存放数据，如果没有并且允许扩展缓冲区
        那么就重新分配缓冲区
        */
        if (GetFreeSize() < nLen)
        {
            if (bExtend)
            {
                Reallocate(GetDataLen() + nLen);
            }
        }

        int nBytesWritten = custom_min(nLen, GetFreeSize());
        if (nBytesWritten > 0)
        {
            memcpy(&m_pWorkingBuffer[m_nCurWritingPos], pData, nBytesWritten * sizeof(ElementType));
            m_nCurWritingPos += nBytesWritten;
        }

        return nBytesWritten;
    }

    /*--------------------------------------------------------------------------
    *函数名称：Read
    *描述：从当前的缓冲区中读取数据
    *参数 : const ElementType* pData->一个指向存储数据的指针
    *参数 : int nLen->期望读取的数据长度  (注意单位是ElementType的长度)
    *参数 : bool bDestroyData->是否把读取的数据从缓冲区中清除掉
    *返回值: 返回实际的读取数据的长度
    *修改记录:
    *---------------------------------------------------------------------------*/
    int Read(ElementType* pData, int nLen, bool bDestroyData = true)
    {
        assert(pData && nLen);
        if (nullptr == pData || 0 == nLen)
        {
            return 0;
        }

        int nBytesReaded = custom_min(nLen, GetDataLen());

        if (nBytesReaded > 0)
        {
            memcpy((BYTE*)pData, &m_pWorkingBuffer[m_nCurReadingPos], nBytesReaded * sizeof(ElementType));

            if (bDestroyData)
            {
                m_nCurReadingPos += nBytesReaded;
                if (m_nCurReadingPos == m_nCurWritingPos)
                {
                    Empty();
                }
            }
        }

        return nBytesReaded;
    }

    /*--------------------------------------------------------------------------
    *函数名称：Read
    *描述：从当前的缓冲区中读取数据
    *参数 : CBufferImplT& refOutBuffer->一个指向存储数据的缓冲区对象的参考
    *参数 : int nLen->期望读取的数据长度  (注意单位是ElementType的长度)
    *参数 : bool bDestroyData->是否把读取的数据从缓冲区中清除掉
    *返回值: 返回实际的读取数据的长度  (注意单位是ElementType的长度)
    *修改记录:
    *---------------------------------------------------------------------------*/
    int Read(CBufferImplT& refOutBuffer, int nLen, bool bDestroyData = true)
    {
        assert(refOutBuffer.GetFreeSize() >= nLen);

        int nBytesReaded = Read(refOutBuffer.GetBufferFreeHeadPtr(), custom_min(refOutBuffer.GetFreeSize(), nLen), bDestroyData);
        if (nBytesReaded)
        {
            refOutBuffer.IncreaseDataLen(nBytesReaded);
        }

        return nBytesReaded;
    }

    /*--------------------------------------------------------------------------
    *函数名称：MoveDataForward
    *描述：把缓冲区末尾的数据移动到缓冲区头部
    *修改记录:
    *---------------------------------------------------------------------------*/
    void MoveDataForward()
    {
        if (m_nCurReadingPos < m_nCurWritingPos)
        {
            if (m_nCurReadingPos > 0)
            {
                const int nDataLen = GetDataLen();

                memmove((BYTE*)m_pWorkingBuffer, (BYTE*)&m_pWorkingBuffer[m_nCurReadingPos], nDataLen);
                m_nCurReadingPos = 0;
                m_nCurWritingPos = nDataLen;
            }
        }
        else
        {
            Empty();
        }
    }


    void Empty()
    {
        m_nCurWritingPos = 0;
        m_nCurReadingPos = 0;
    }


    /*****************************************
    名称 : SetDataLen
    描述：设置缓冲区数据的长度
    参数：int nDataLen->新的长度 (注意单位是ElementType的长度)
    修改日期:
    2016-01-012 add by Lei
    ******************************************/
    void SetDataLen(int nDataLen)
    {
        m_nCurWritingPos = m_nCurReadingPos + nDataLen;
    }

    /*--------------------------------------------------------------------------
    *函数名称：Attach
    *描述：把当前的缓冲区附加到一个指定的外部存储空间，如果当前缓冲区分配有空间将释放掉
    *参数 : const ElementType* pData->一个指向外部内存的指针
    *参数 : int nLen->缓冲区的长度  (注意单位是ElementType的长度)
    *修改记录:
    *---------------------------------------------------------------------------*/
    void Attach(ElementType* pData, int nLen)
    {
        assert(pData && nLen);
        if (pData && nLen)
        {
            if (m_pWorkingBuffer)
            {
                delete[]m_pWorkingBuffer;
            }

            m_pWorkingBuffer = pData;
            m_nSize = nLen;
            m_nCurWritingPos = 0;
            m_nCurReadingPos = 0;
            m_bAttached = true;
        }
    }

    /*--------------------------------------------------------------------------
*函数名称：Dettach
*描述：把当前的缓冲区附加到一个指定的外部存储空间，如果当前缓冲区分配有空间将释放掉
*参数 : const ElementType* pData->一个指向外部内存的指针
*参数 : int nLen->缓冲区的长度  (注意单位是ElementType的长度)
*修改记录:
*---------------------------------------------------------------------------*/
    void Dettach()
    {
        assert(m_bAttached);
        if (m_bAttached)
        {
            m_pWorkingBuffer = nullptr;
            m_nSize = 0;
            m_nCurWritingPos = 0;
            m_nCurReadingPos = 0;
        }
    }

public:
    /*****************************************
    名称:  Allocate
    描述：分配缓冲区的空间,如果当前缓冲区已经附加到一个外部空间，那么分配将失败
    参数：int nSize->新缓冲区的尺寸
    返回值：true ->分配陈功,false->失败
    修改日期:
    2016-01-012 add by Lei
    ******************************************/
    bool Allocate(int nSize)
    {
        bool bResult = false;

        assert(!m_bAttached);
        if (!m_bAttached)
        {
            nSize = CalcBufferAlign(nSize, nMemoryAligment);

            assert(nSize > 0);
            if (nSize && nSize != m_nSize)
            {
                ElementType* pNewBuffer = new ElementType[nSize];
                if (pNewBuffer)
                {
                    memset(pNewBuffer, 0, nSize * sizeof(ElementType));
                    if (m_pWorkingBuffer)
                    {
                        delete[]m_pWorkingBuffer;
                    }
                    m_pWorkingBuffer = pNewBuffer;
                    m_nSize = nSize;

                    bResult = true;
                }
            }
        }

        return bResult;
    }

    /*****************************************
    名称:  Reallocate
    描述：重新分配缓冲区的空间,并把旧缓冲区中的数据拷贝到新缓冲区中,如果当前缓冲区已经附加到一个外部空间，那么分配将失败
    参数：int nSize->新缓冲区的尺寸 (注意单位是ElementType的长度)
    返回值：true ->分配陈功,false->失败
    修改日期:
    2016-01-012 add by Lei
    ******************************************/
    bool Reallocate(int nSize)
    {
        bool bResult = false;

        assert(!m_bAttached);
        if (!m_bAttached)
        {
            nSize = CalcBufferAlign(nSize, nMemoryAligment);

            //assert(nSize && nSize != m_nSize);
            if (nSize && nSize != m_nSize)
            {
                ElementType* pNewBuffer = new ElementType[nSize];
                if (pNewBuffer)
                {
                    memset(pNewBuffer, 0, nSize * sizeof(ElementType));
                    if (m_pWorkingBuffer)
                    {
                        memcpy(pNewBuffer, m_pWorkingBuffer, custom_min(nSize, m_nSize) * sizeof(ElementType));
                        if (m_pWorkingBuffer)
                        {
                            delete[]m_pWorkingBuffer;
                        }
                    }

                    m_pWorkingBuffer = pNewBuffer;
                    m_nSize = nSize;

                    bResult = true;
                }
            }
            else
            {
                bResult = (nSize == m_nSize);
            }
        }
        return bResult;
    }


    /*****************************************
    名称:  Deallocate
    描述：释放缓冲区的空间(注意：只有自己分配的空间，才能释放)
    修改日期:
    2016-01-012 add by Lei
    ******************************************/
    void Deallocate()
    {
        assert(!m_bAttached);
        if (!m_bAttached)
        {
            if (m_pWorkingBuffer)
            {
                delete[]m_pWorkingBuffer;

                m_nSize = 0;
                m_pWorkingBuffer = nullptr;
            }
        }
    }

    /*--------------------------------------------------------------------------
    *函数名称：CalcBufferAlign
    *描述：内存对齐算法
    *参数：unsigned int n->需要计算的数据
    *参数：unsigned align->对齐长度
    *返回值: 对齐后的值
    *原理：

    2字节对齐，要求地址位为2,4,6,8...，要求二进制位最后一位为0（2的1次方）
    4字节对齐，要求地址位为4,8,12,16...，要求二进制位最后两位为0（2的2次方）
    8字节对齐，要求地址位为8,16,24,32...，要求二进制位最后三位为0（2的3次方）
    16字节对齐，要求地址位为16,32,48,64...，要求二进制位最后四位为0（2的4次方）
    ...
    由此可见，我们只要对数据补齐对齐所需最少数据，然后将补齐位置0就可以实现对齐计算。

    （1）(align-1)，表示对齐所需的对齐位，如：2字节对齐为1，4字节为11，8字节为111，16字节为1111...
    （2）(x+(align-1))，表示x补齐对齐所需数据
    （3）&~(align-1)，表示去除由于补齐造成的多余数据
    （4） (x+(align-1))&~(align-1)，表示对齐后的数据

    举个例子：如8字节对齐。起始地始是6
    6 + （8 - 1）=0000 0110 + 0000 0111 = 0000 1101
    0000 1101 & ~(0000 0111) = 0000 1000  //去除由于补齐造成的多余数据
    *修改记录:
    *---------------------------------------------------------------------------*/
    unsigned int CalcBufferAlign(unsigned int n, unsigned align)
    {
        return ((n + align - 1) & (~(align - 1)));
    }

public:
    operator ElementType*() { return GetBufferHeadPtr(); }

private:
    ElementType*          m_pWorkingBuffer = nullptr;      //指向缓冲区的指针
    int                          m_nSize = 0;                 //缓冲区的长度
    int                          m_nCurWritingPos = 0;    //当前写的位置
    int                          m_nCurReadingPos = 0;   //当前读的位置
    bool                      m_bAttached = false;           //是否附加到一个外部空间
};


#endif /* BufferImpl_h */

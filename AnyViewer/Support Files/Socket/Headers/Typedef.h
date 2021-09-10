//
//  Typedef.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//

#ifndef Typedef_h
#define Typedef_h

//MARK: ---- 类型定义 ----

#include <stdint.h>

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

#ifndef TCHAR
typedef char                TCHAR;
#endif

#ifndef UCHAR
typedef unsigned char                UCHAR;
#endif

#ifndef WCHAR
typedef  wchar_t                WCHAR;
#endif

//MARK: ---- max & min函数 ----

#ifndef custom_max
#define custom_max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef custom_min
#define custom_min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


//MARK: ---- socket相关宏定义 ----

#define MAX_USER  1000                                  ///< 服务器最大的并发连接数
#define LIST_MAX 1024                                     ///< 队列最大长度

#define RESEND_TIME_OUT 20 * 1000                ///< 重发超时时间20秒
#define SENDING_TIME_OUT      5                              ///< 发送超时5秒
#define RECEIVING_TIME_OUT    2                             ///< 接收超时2秒

#define SAX2_PACKET_HEADER_MAGIC     0XFCFDFEFF   ///< 消息包头标志
#define SAX2_PACKET_VERSION  1                              ///< 当前数据包格式的版本号
#define INIT_INPUT_BUFFE_SIZE 1024 * 10                    ///< 存放接收数据的缓冲区的最大长度

#define CONCURRENCY_CONNECTIONS_MAX 10240       ///< 最大的并发连接数
#define EPOLL_MAX 128                                              ///< 单次可以处理的最大连接数
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define M_LISTEN_MAX 1024
#define DEFAULT_POLL_TIMEOUT 5000
#define DEFAULT_KEEP_ALIVE_TIMEOUT 300    ///<单位秒


//MARK: Windows相关的宏
#define WTS_CONSOLE_CONNECT                0x1
#define WTS_CONSOLE_DISCONNECT             0x2
#define WTS_REMOTE_CONNECT                 0x3
#define WTS_REMOTE_DISCONNECT              0x4
#define WTS_SESSION_LOGON                  0x5
#define WTS_SESSION_LOGOFF                 0x6
#define WTS_SESSION_LOCK                   0x7
#define WTS_SESSION_UNLOCK                 0x8
#define WTS_SESSION_REMOTE_CONTROL         0x9
#define WTS_SESSION_CREATE                 0xa
#define WTS_SESSION_TERMINATE              0xb


//MARK: ---- memory pool 相关宏定义 ----

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



//MARK: ---- 成员变量声明 相关宏定义 ----

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


#define DECLARE_MEMBER_AND_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME) \
protected: \
    TYPE MEMBER_NAME; \
DECLARE_SET_METHOD(TYPE, MEMBER_NAME, PROPERTY_NAME)


//MARK: ---- 指针检查 相关宏定义 ----

#define CHECK_POINTER(POINTER) \
assert(nullptr != POINTER); \
if(nullptr == POINTER) \
{ return;}

#define CHECK_POINTER_EX(POINTER,RESULT) \
assert(nullptr != POINTER); \
if(nullptr == POINTER) \
{ return RESULT;}





#endif /* Typedef_h */

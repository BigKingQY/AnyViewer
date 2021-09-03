//
//  SDPSingleton.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/1.
//

#ifndef SDPSingleton_h
#define SDPSingleton_h


#pragma once
#include <mutex>


// --------------------------------------------------------------------------------
/// <summary>
/// »´æ÷µ• µ¡–
/// </summary>
// --------------------------------------------------------------------------------
template <typename T>
class CGlobleSingleton
{
private:
    CGlobleSingleton() {}
    CGlobleSingleton(const CGlobleSingleton& rhs) = delete;
    CGlobleSingleton& operator=(const CGlobleSingleton& rhs) =delete;

public:
    static T * Instance()
    {
        static std::mutex m_objInstanceMutex;
        std::unique_lock<std::mutex> objLocker(m_objInstanceMutex);

        static T * pT = nullptr;
        if (!pT)
        {
            static T instance;
            pT = &instance;
        }
        return pT;
    }

private:
    //static std::mutex m_objInstanceMutex;

};

//template<class T> std::mutex CGlobleSingleton<T>::m_objInstanceMutex;


template<class T>
class CSingleton
{
public:
    CSingleton()
    {
        std::unique_lock<std::mutex> objLocker(m_objInstanceMutex);

        if (nullptr == s_pInstance)
        {
            s_pInstance = (T*)this;
        }
        //else
        //{
        //    assert(false);
        //}
    }

    virtual ~CSingleton()
    {
        std::unique_lock<std::mutex> objLocker(m_objInstanceMutex);

        s_pInstance = 0;
    }

    static T* Instance()
    {
        {
            std::unique_lock<std::mutex> objLocker(m_objInstanceMutex);

            //if (nullptr == s_pInstance)
            //{
            //    assert(false);
            //}
        }

        return (T*)s_pInstance;
    }

private:
    static std::mutex m_objInstanceMutex;
    static void* s_pInstance;
};

template<class T> std::mutex CSingleton<T>::m_objInstanceMutex;
template<class T> void *CSingleton<T>::s_pInstance = 0;

#endif

//
//  SyncQueue.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/9.
//

#ifndef SyncQueue_h
#define SyncQueue_h

#pragma once
#include "Typedef.h"

#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream>
#include <atomic>


template<typename T>
    class CSyncQueue
    {
    public:
        CSyncQueue(const unsigned int nMaxSize)
            : m_nMaxSize(nMaxSize)
            , m_bClose(false)
        {
        }

            
        void Put(const T& objNewTask)
        {
            Add(objNewTask);
        }

        void Put(T&& objNewTask)
        {
            Add(std::forward<T>(objNewTask));
        }

//--------------------------------------------------------------------------------------------------------------------
//∫Ø ˝√˚≥∆£∫Take
/// @brief Ã·»°µ±«∞∂”¡–÷–µƒÀ˘”–»ŒŒÒ
///
/// @param _in refResultList->”√”⁄Ω” ’ªÒ»°µƒ»ŒŒÒ
/// @author ª∆¿ˆ‘∆
//--------------------------------------------------------------------------------------------------------------------
        void Take(std::list<T>& refResultList)
        {
            std::unique_lock<std::mutex> objLocker(m_objMutex);
            m_objNotEmpty.wait(objLocker, [this]{return m_bClose || NotEmpty(); });
        
            if (m_bClose)
            {
                return;
            }
                
            refResultList = std::move(m_objTaskQueue);
            m_objNotFull.notify_one();
        }

//--------------------------------------------------------------------------------------------------------------------
//∫Ø ˝√˚≥∆£∫Take
/// @brief ¥”∂”¡–÷–Ã·»°“ª∏ˆ»ŒŒÒ
///
/// @param _in refResult->”√”⁄Ω” ’ªÒ»°µƒ»ŒŒÒ
/// @author ª∆¿ˆ‘∆
//--------------------------------------------------------------------------------------------------------------------
        void Take(T& refResult)
        {
            std::unique_lock<std::mutex> objLocker(m_objMutex);
            m_objNotEmpty.wait(objLocker, [this]{return m_bClose || NotEmpty(); });
        
            if (m_bClose)
            {
                return;
            }
                
            refResult = m_objTaskQueue.front();
            m_objTaskQueue.pop_front();
            m_objNotFull.notify_one();
        }

//--------------------------------------------------------------------------------------------------------------------
//∫Ø ˝√˚≥∆£∫Close
/// @brief πÿ±’µ±«∞µƒ∂”¡–
///
/// @author ª∆¿ˆ‘∆
//--------------------------------------------------------------------------------------------------------------------
        void Close()
        {
            {
                std::unique_lock<std::mutex> objLocker(m_objMutex);

                m_bClose = true;
            }

            m_objNotFull.notify_all();
            m_objNotEmpty.notify_all();
        }

        /// @brief ≈–∂œ∂”¡– «∑ÒŒ™ø’
        bool Empty()
        {
            std::unique_lock<std::mutex> objLocker(m_objMutex);
            return m_objTaskQueue.empty();
        }

        /// @brief ≈–∂œ∂”¡– «∑Ò“—¬˙
        bool Full()
        {
            std::unique_lock<std::mutex> objLocker(m_objMutex);
            return m_objTaskQueue.size() == m_nMaxSize;
        }

        /// @brief ∑µªÿ∂”¡–÷–µƒ»ŒŒÒ ˝
        size_t Size()
        {
            std::unique_lock<std::mutex> objLocker(m_objMutex);
            return m_objTaskQueue.size();
        }

        /// @brief ∑µªÿ∂”¡–÷–µƒ»ŒŒÒ ˝
        size_t Count()
        {
            return m_objTaskQueue.size();
        }
    private:
//--------------------------------------------------------------------------------------------------------------------
//∫Ø ˝√˚≥∆£∫NotFull
/// @brief ≈–∂œµ±«∞µƒ∂”¡– «∑Ò“—æ≠¬˙
///
/// @return true£∫“—¬˙£¨∑Ò‘ÚŒ¥¬˙
/// @author ª∆¿ˆ‘∆
//--------------------------------------------------------------------------------------------------------------------
        bool NotFull() const
        {
            bool bFull = (m_objTaskQueue.size() >= m_nMaxSize);

            if (bFull)
            {
                std::cout << "full, waiting£¨thread id: " << std::this_thread::get_id() << std::endl;
            }
                
            return !bFull;
        }

//--------------------------------------------------------------------------------------------------------------------
//∫Ø ˝√˚≥∆£∫NotEmpty
/// @brief ≈–∂œµ±«∞µƒ∂”¡– «∑Ò≤ªŒ™ø’
///
/// @return true£∫≤ªŒ™ø’£¨∑Ò‘ÚŒ™ø’
/// @author ª∆¿ˆ‘∆
//--------------------------------------------------------------------------------------------------------------------
        bool NotEmpty() const
        {
            bool bEmpty = m_objTaskQueue.empty();
            //if (bEmpty)
            //{
            //    std::cout << "empty,waiting£¨thread id: " << std::this_thread::get_id() << std::endl;
            //}
                
            return !bEmpty;
        }

//--------------------------------------------------------------------------------------------------------------------
//∫Ø ˝√˚≥∆£∫Add
/// @brief ‘ˆº”–¬µƒ»ŒŒÒµΩ∂”¡–÷–,≤¢œÚπÿ◊¢Ω¯≥Ã∑¢≥ˆÕ®÷™
///
/// @param _in objNewTask->“ª∏ˆ»ŒŒÒ∂‘œÛµƒ”“÷µ≤Œøº
/// @author ª∆¿ˆ‘∆
//--------------------------------------------------------------------------------------------------------------------
        template<typename F>
            void Add(F&& objNewTask)
            {
                std::unique_lock< std::mutex> objLocker(m_objMutex);
                
                m_objNotFull.wait(objLocker, [this]{return m_bClose || NotFull(); });

                if (m_bClose)
                {
                    return;
                }

                m_objTaskQueue.push_back(std::forward<F>(objNewTask));
                m_objNotEmpty.notify_one();
            }

    private:
        std::list<T>              m_objTaskQueue;          ///< ª∫≥Â«¯
        std::mutex                m_objMutex;                ///< ª•≥‚¡ø∫ÕÃıº˛±‰¡øΩ·∫œ∆¿¥ π”√
        std::condition_variable m_objNotEmpty;          ///< ≤ªŒ™ø’µƒÃıº˛±‰¡ø
        std::condition_variable m_objNotFull;              ///< √ª”–¬˙µƒÃıº˛±‰¡ø
        unsigned int                        m_nMaxSize;               ///< Õ¨≤Ω∂”¡–◊Ó¥Ûµƒsize

        std::atomic_bool        m_bClose;                   ///< πÿ±’∂”¡–±Í÷æ
    };


#endif /* SyncQueue_h */

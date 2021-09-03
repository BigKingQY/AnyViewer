//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file SyncQueue.h
/// @brief 同步队列
/// 
/// @author 黄丽云
/// @version 1.0
/// @date   2017.7.7
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Typedef.h"

#include<list>
#include<mutex>
#include<thread>
#include<condition_variable>
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
//函数名称：Take
/// @brief 提取当前队列中的所有任务
///
/// @param _in refResultList->用于接收获取的任务
/// @author 黄丽云
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
//函数名称：Take
/// @brief 从队列中提取一个任务
///
/// @param _in refResult->用于接收获取的任务
/// @author 黄丽云
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
//函数名称：Close
/// @brief 关闭当前的队列
///
/// @author 黄丽云
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

		/// @brief 判断队列是否为空
		bool Empty()
		{
			std::unique_lock<std::mutex> objLocker(m_objMutex);
			return m_objTaskQueue.empty();
		}

		/// @brief 判断队列是否已满
		bool Full()
		{
			std::unique_lock<std::mutex> objLocker(m_objMutex);
			return m_objTaskQueue.size() == m_nMaxSize;
		}

		/// @brief 返回队列中的任务数
		size_t Size()
		{
			std::unique_lock<std::mutex> objLocker(m_objMutex);
			return m_objTaskQueue.size();
		}

		/// @brief 返回队列中的任务数
		size_t Count() 
		{
			return m_objTaskQueue.size();
		}
	private:
//--------------------------------------------------------------------------------------------------------------------
//函数名称：NotFull
/// @brief 判断当前的队列是否已经满
///
/// @return true：已满，否则未满
/// @author 黄丽云
//--------------------------------------------------------------------------------------------------------------------
		bool NotFull() const 
		{
			bool bFull = (m_objTaskQueue.size() >= m_nMaxSize);

			if (bFull)
			{
				std::cout << "full, waiting，thread id: " << std::this_thread::get_id() << std::endl;
			}
				
			return !bFull;
		}

//--------------------------------------------------------------------------------------------------------------------
//函数名称：NotEmpty
/// @brief 判断当前的队列是否不为空
///
/// @return true：不为空，否则为空
/// @author 黄丽云
//--------------------------------------------------------------------------------------------------------------------
		bool NotEmpty() const
		{
			bool bEmpty = m_objTaskQueue.empty();
			//if (bEmpty)
			//{
			//	std::cout << "empty,waiting，thread id: " << std::this_thread::get_id() << std::endl;
			//}
				
			return !bEmpty;
		}

//--------------------------------------------------------------------------------------------------------------------
//函数名称：Add
/// @brief 增加新的任务到队列中,并向关注进程发出通知
///
/// @param _in objNewTask->一个任务对象的右值参考
/// @author 黄丽云
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
		std::list<T>              m_objTaskQueue;          ///< 缓冲区
		std::mutex                m_objMutex;                ///< 互斥量和条件变量结合起来使用
		std::condition_variable m_objNotEmpty;          ///< 不为空的条件变量
		std::condition_variable m_objNotFull;              ///< 没有满的条件变量
		unsigned int                        m_nMaxSize;               ///< 同步队列最大的size

		std::atomic_bool        m_bClose;                   ///< 关闭队列标志
	};
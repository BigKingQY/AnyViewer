//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file SyncQueue.h
/// @brief ͬ������
/// 
/// @author ������
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
//�������ƣ�Take
/// @brief ��ȡ��ǰ�����е���������
///
/// @param _in refResultList->���ڽ��ջ�ȡ������
/// @author ������
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
//�������ƣ�Take
/// @brief �Ӷ�������ȡһ������
///
/// @param _in refResult->���ڽ��ջ�ȡ������
/// @author ������
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
//�������ƣ�Close
/// @brief �رյ�ǰ�Ķ���
///
/// @author ������
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

		/// @brief �ж϶����Ƿ�Ϊ��
		bool Empty()
		{
			std::unique_lock<std::mutex> objLocker(m_objMutex);
			return m_objTaskQueue.empty();
		}

		/// @brief �ж϶����Ƿ�����
		bool Full()
		{
			std::unique_lock<std::mutex> objLocker(m_objMutex);
			return m_objTaskQueue.size() == m_nMaxSize;
		}

		/// @brief ���ض����е�������
		size_t Size()
		{
			std::unique_lock<std::mutex> objLocker(m_objMutex);
			return m_objTaskQueue.size();
		}

		/// @brief ���ض����е�������
		size_t Count() 
		{
			return m_objTaskQueue.size();
		}
	private:
//--------------------------------------------------------------------------------------------------------------------
//�������ƣ�NotFull
/// @brief �жϵ�ǰ�Ķ����Ƿ��Ѿ���
///
/// @return true������������δ��
/// @author ������
//--------------------------------------------------------------------------------------------------------------------
		bool NotFull() const 
		{
			bool bFull = (m_objTaskQueue.size() >= m_nMaxSize);

			if (bFull)
			{
				std::cout << "full, waiting��thread id: " << std::this_thread::get_id() << std::endl;
			}
				
			return !bFull;
		}

//--------------------------------------------------------------------------------------------------------------------
//�������ƣ�NotEmpty
/// @brief �жϵ�ǰ�Ķ����Ƿ�Ϊ��
///
/// @return true����Ϊ�գ�����Ϊ��
/// @author ������
//--------------------------------------------------------------------------------------------------------------------
		bool NotEmpty() const
		{
			bool bEmpty = m_objTaskQueue.empty();
			//if (bEmpty)
			//{
			//	std::cout << "empty,waiting��thread id: " << std::this_thread::get_id() << std::endl;
			//}
				
			return !bEmpty;
		}

//--------------------------------------------------------------------------------------------------------------------
//�������ƣ�Add
/// @brief �����µ����񵽶�����,�����ע���̷���֪ͨ
///
/// @param _in objNewTask->һ������������ֵ�ο�
/// @author ������
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
		std::list<T>              m_objTaskQueue;          ///< ������
		std::mutex                m_objMutex;                ///< �����������������������ʹ��
		std::condition_variable m_objNotEmpty;          ///< ��Ϊ�յ���������
		std::condition_variable m_objNotFull;              ///< û��������������
		unsigned int                        m_nMaxSize;               ///< ͬ����������size

		std::atomic_bool        m_bClose;                   ///< �رն��б�־
	};
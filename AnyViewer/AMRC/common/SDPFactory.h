///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  
/// @brief 对象工厂设计模式实现
/// 
///
/// @author 黄丽云
/// @version 1.0
/// @date        2018-10-19
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
//#include "public.h"
#include "SDPSingleton.h"
#include <map>
#include <memory>
#include <assert.h>

#define N2S(x) #x     


//*******************************************************************
// 对象工厂
///
/// 
//*******************************************************************

template<class IdentifierType,typename ProductCreator = void * (*)()>
class CFactory
{
public:
	CFactory() {};


public:
	bool Register(const IdentifierType& id, ProductCreator creator)
	{
		associations_[id] = creator;
		return true;
	}

	bool Unregister(const IdentifierType& id)
	{
		return associations_.erase(id) == 1;
	}

	void * CreateObject(const IdentifierType& id)
	{
		typename ASSOCIATION_MAP::const_iterator i = associations_.find(id);
		
		if (i != associations_.end())
		{
			return (i->second)();
		}
		else
		{
			assert(false);
		}

		return nullptr;
	}	
private:
	CFactory(CFactory& factory);
	CFactory& operator=(const CFactory& factory);

private:  
	typedef std::map<IdentifierType, ProductCreator> ASSOCIATION_MAP;
	ASSOCIATION_MAP associations_;
};


template<typename ProductCreator>
inline bool RegistClass(const std::string& strId, ProductCreator creator)
{
	return CGlobleSingleton<CFactory<std::string> >::Instance()->Register(strId, creator);
}


template <class TClass>
inline TClass * CreateObject(const std::string& strId)
{
	return (TClass *)CGlobleSingleton<CFactory<std::string> >::Instance()->CreateObject(strId);
}

template <class TClass>
inline std::shared_ptr<TClass> CreateSharedObject(const std::string& strId)
{
	return std::shared_ptr<TClass>((TClass *)CGlobleSingleton<CFactory<std::string> >::Instance()->CreateObject(strId));
}


//*******************************************************************
//  注册类到对象工厂
///
/// 
//*******************************************************************
template <class TClass>
class CRegisterClass
{
public:
	CRegisterClass(const std::string& strId)
	{
		RegistClass<>(strId, CRegisterClass::Create);
	}

public:
	static void * Create()
	{
		return new TClass;
	}	
};

#define REGISTER_CLASS(TClass) CRegisterClass<TClass> g_ob##TClass(N2S(TClass));




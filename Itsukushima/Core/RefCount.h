/**
* Refence Count master class, like a smart pointer
*
* @author: Kai Yang
**/

#ifndef REFERENCECOUNT_H
#define REFERENCECOUNT_H

#include "TypeDefine.h"
#include <stddef.h>

class RefCountBase
{
public:
	RefCountBase()
	{
		m_nRefCount = 0;
	}
	virtual ~RefCountBase()
	{

	}

	void Retain()
	{
		++m_nRefCount;
	}

	void Release()
	{
		--m_nRefCount;

		if(m_nRefCount == 0) 
		{
			delete this;
		}
	}

private:
	int32 m_nRefCount;
};

template <class T>
class RefCountPtr
{
public:
	RefCountPtr(T* pPointer = nullptr)
	{
		m_pActualPtr = pPointer;
		if(m_pActualPtr != nullptr) 
		{
			m_pActualPtr->Retain();
		}
	}

	RefCountPtr(const RefCountPtr& pPointer)
	{
		m_pActualPtr = pPointer.m_pActualPtr;
		if(m_pActualPtr != nullptr) 
		{
			m_pActualPtr->Retain();
		}
	}

	~RefCountPtr()
	{
		if(m_pActualPtr != nullptr) 
		{
			m_pActualPtr->Release();
		}
	}

	RefCountPtr &operator=(T* pPointer)
	{
		//retain new one first, pPointer and m_pActualPtr could be the same
		if(pPointer != nullptr) 
		{
			pPointer->Retain();
		}
		
		//release old one
		if(m_pActualPtr != nullptr) 
		{
			m_pActualPtr->Release();
		}

		m_pActualPtr = pPointer;
		return (*this);
	}

	RefCountPtr &operator=(const RefCountPtr &pPointer)
	{
		return (*this) = pPointer.m_pActualPtr;
	}

	T* get() const	{ return m_pActualPtr; }
	T* operator->() const {return m_pActualPtr;}
	T &operator*() const {return *m_pActualPtr;}
	operator T*() const {return m_pActualPtr;}
	operator bool() const {return m_pActualPtr != nullptr;}
	bool operator==(const RefCountPtr &pPointer) {return m_pActualPtr == pPointer.m_pActualPtr;}
	bool operator==(const T *pPointer) {return m_pActualPtr == pPointer;}

private:
	T *m_pActualPtr;
};

#endif
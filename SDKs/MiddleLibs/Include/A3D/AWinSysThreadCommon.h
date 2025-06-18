//-------------------------------------------------------------------------------------------------
//FileName: AWinSysThreadCommon.h
//Created by miaoyu 2013,2,28
//-------------------------------------------------------------------------------------------------
#ifndef _A_WINSYSTHREADCOMMON_H_
#define _A_WINSYSTHREADCOMMON_H_

#include "ASysThreadCommon.h"

#include <windows.h>

//-------------------------------------------------------------------------------------------------
//	AWinRawAtomic and AWinRawMutex are not depend on our memory manager, so you
//	can use them at any time, even before memory manager is initialized.
//-------------------------------------------------------------------------------------------------

class AWinRawAtomic
{
public:

	AWinRawAtomic(aint32 value) :
	m_iValue(value)
	{
	}

	aint32 GetValue() const { return m_iValue; }
	void Reset();
	aint32 Fetch_Add(aint32 iAmount);
	aint32 Fetch_Set(aint32 iAmount);
	aint32 Fetch_CompareSet(aint32 iAmount, aint32 iComparand);
	aint32 Increment_Fetch();
	aint32 Decrement_Fetch();

private:

	__declspec(align(4)) volatile along	m_iValue;
};

class AWinRawMutex
{
public:

	AWinRawMutex();
	~AWinRawMutex();

	void Lock();
	void Unlock();

private:

	CRITICAL_SECTION	m_cs;
};

//-------------------------------------------------------------------------------------------------
//AWinSysThreadAtomic	
//-------------------------------------------------------------------------------------------------

class AWinSysThreadAtomic : public ASysThreadAtomic
{
public:

	AWinSysThreadAtomic(aint32 value) :
	m_atomic(value)
	{
	}

	virtual void Release() { delete this; }
	virtual aint32 GetValue() const { return m_atomic.GetValue(); }

	virtual aint32 Fetch_Add(aint32 iAmount) { return m_atomic.Fetch_Add(iAmount); }
	virtual aint32 Fetch_Set(aint32 iAmount) { return m_atomic.Fetch_Set(iAmount); }
	virtual aint32 Fetch_CompareSet(aint32 iAmount, aint32 iComparand) { return m_atomic.Fetch_CompareSet(iAmount, iComparand); }

	virtual aint32 Increment_Fetch() { return m_atomic.Increment_Fetch(); }
	virtual aint32 Decrement_Fetch() { return m_atomic.Decrement_Fetch(); }

private:

	AWinRawAtomic	m_atomic;
};

class AWinSysThreadMutex : public ASysThreadMutex
{
public:

	virtual void Release() { delete this; }
	virtual void Lock() { m_mutex.Lock(); }
	virtual void Unlock() { m_mutex.Unlock(); }

private:

	AWinRawMutex	m_mutex;
};

#endif
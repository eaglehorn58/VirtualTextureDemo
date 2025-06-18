//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "AWinSysThreadCommon.h"

//-------------------------------------------------------------------------------------------------
//	ASysThreadAtomic
//-------------------------------------------------------------------------------------------------

aint32 AWinRawAtomic::Fetch_Add(aint32 iAmount)
{
	return (aint32)InterlockedExchangeAdd((LPLONG)&m_iValue, (LONG)iAmount);
}

void AWinRawAtomic::Reset()
{
	InterlockedExchange((LPLONG)&m_iValue, 0);
}

aint32 AWinRawAtomic::Fetch_Set(aint32 iAmount)
{
	return (aint32)InterlockedExchange((LPLONG)&m_iValue, (LONG)iAmount);
}

aint32 AWinRawAtomic::Fetch_CompareSet(aint32 iAmount, aint32 iComparand)
{
	return (aint32)InterlockedCompareExchange((LPLONG)m_iValue, (LONG)iAmount, (LONG)iComparand);
}

aint32 AWinRawAtomic::Increment_Fetch()
{
	return (aint32)InterlockedIncrement((LPLONG)&m_iValue);
}

aint32 AWinRawAtomic::Decrement_Fetch()
{
	return (aint32)InterlockedDecrement((LPLONG)&m_iValue);
}

//-------------------------------------------------------------------------------------------------
//	AWinRawMutex
//-------------------------------------------------------------------------------------------------

AWinRawMutex::AWinRawMutex()
{
	InitializeCriticalSection(&m_cs);
}

AWinRawMutex::~AWinRawMutex()
{
	DeleteCriticalSection(&m_cs);
}

void AWinRawMutex::Lock()
{
	EnterCriticalSection(&m_cs);
}

void AWinRawMutex::Unlock()
{
	LeaveCriticalSection(&m_cs);
}



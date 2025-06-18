//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#ifndef _A_RAWTHREAD_H_
#define _A_RAWTHREAD_H_

#include "ATypes.h"

#if A_PLATFORM_WIN_DESKTOP
	#include "AWinSysThreadCommon.h"
#elif A_PLATFORM_ANDROID
    #include "AAndroidSysThreadCommon.h"
#elif A_PLATFORM_LINUX
//
#elif A_PLATFORM_XOS
	#include "AXOSSysThreadCommon.h"
#elif A_PLATFORM_WINRT
	#include "AWinRTSysThreadCommon.h"
#endif

//-------------------------------------------------------------------------------------------------
//	ARawThreadAtomic
//-------------------------------------------------------------------------------------------------

class ARawThreadAtomic
{
public:

	ARawThreadAtomic() :
	m_atomic(0)
	{
	}

	ARawThreadAtomic(aint32 value) :
	m_atomic(value)
	{
	}

public:

	aint32 GetValue() const { return m_atomic.GetValue(); }
	aint32 Fetch_Add(aint32 iAmount) { return m_atomic.Fetch_Add(iAmount); }
	aint32 Fetch_Set(aint32 iAmount) { return m_atomic.Fetch_Set(iAmount); }
	aint32 Fetch_CompareSet(aint32 iAmount, aint32 iComparand) { return m_atomic.Fetch_CompareSet(iAmount, iComparand); }
	aint32 Increment_Fetch() { return m_atomic.Increment_Fetch(); }
	aint32 Decrement_Fetch() { return m_atomic.Decrement_Fetch(); }

private:

#if A_PLATFORM_WIN_DESKTOP
	AWinRawAtomic	m_atomic;
#elif A_PLATFORM_ANDROID
    AAndroidRawAtomic m_atomic;
#elif A_PLATFORM_LINUX
//
#elif A_PLATFORM_XOS
	AXOSRawAtomic	m_atomic;
#elif A_PLATFORM_WINRT
	AWinRTRawAtomic m_atomic;
#endif
};

//-------------------------------------------------------------------------------------------------
//	ARawThreadSpin
//-------------------------------------------------------------------------------------------------

class ARawThreadSpin
{
public:

	ARawThreadSpin() :
	m_atomic(0)
	{
	}

public:

	//	Return atrue if lock successfully
	bool Lock() { return m_atomic.Fetch_Set(1) == 0; }
	void Unlock() { return m_atomic.Reset(); }

private:

#if A_PLATFORM_WIN_DESKTOP
	AWinRawAtomic	m_atomic;
#elif A_PLATFORM_ANDROID
	AAndroidRawAtomic m_atomic;
#elif A_PLATFORM_LINUX
	//
#elif A_PLATFORM_XOS
	AXOSRawAtomic	m_atomic;
#elif A_PLATFORM_WINRT
	AWinRTRawAtomic m_atomic;
#endif
};

//-------------------------------------------------------------------------------------------------
//	ARawThreadMutex
//-------------------------------------------------------------------------------------------------

class ARawThreadMutex
{
public:

	void Lock() { m_mutex.Lock(); }
	void Unlock() { m_mutex.Unlock(); }

private:

#if A_PLATFORM_WIN_DESKTOP
	AWinRawMutex	m_mutex;
#elif A_PLATFORM_ANDROID
    AAndroidRawMutex m_mutex;
#elif A_PLATFORM_LINUX
	//
#elif A_PLATFORM_XOS
	AXOSRawMutex	m_mutex;
#elif A_PLATFORM_WINRT
	AWinRTRawMutex	m_mutex;
#endif
};

#endif	//	_A_RAWTHREAD_H_
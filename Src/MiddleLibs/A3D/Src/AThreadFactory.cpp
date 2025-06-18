//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "AThreadFactory.h"
#include "ASysThreadCommon.h"
//#include "AThreadPool.h"

#if defined(A_PLATFORM_XOS)
#include "AXOSSysThreadCommon.h"
#include "AXOSSysMainThread.h"
#include "AXOSSysThread.h"
#elif defined(A_PLATFORM_ANDROID)
#include "AAndroidSysThreadCommon.h"
#include "AAndroidSysMainThread.h"
#include "AAndroidSysThread.h"
#elif defined(A_PLATFORM_WINRT)
#include "AWinRTSysThreadCommon.h"
#include "AWinRTSysMainThread.h"
#include "AWinRTSysThread.h"
#else
#include "AWinSysThreadCommon.h"
// #include "AWinSysMainThread.h"
// #include "AWinSysThread.h"
#endif

// ALinkList<ASysMainThread>	AThreadFactory::m_MainThreadList;
// ALinkList<ASysThread>		AThreadFactory::m_ThreadList;
// ALinkList<AThreadPool>		AThreadFactory::m_ThreadPoolList;

ASysThreadAtomic* AThreadFactory::CreateThreadAtomic(aint32 iValue)
{
	ASysThreadAtomic* pAtomic;
#if defined(A_PLATFORM_XOS)
	pAtomic = new AXOSSysThreadAtomic(iValue);
#elif defined(A_PLATFORM_ANDROID)
    pAtomic = new AAndroidSysThreadAtomic(iValue);
#elif defined(A_PLATFORM_WINRT)
	pAtomic = new AWinRTSysThreadAtomic(iValue);
#else
	pAtomic = new AWinSysThreadAtomic(iValue);
#endif

	return pAtomic;
}

ASysThreadMutex* AThreadFactory::CreateThreadMutex(abool bRecursive/* atrue */)
{
	ASysThreadMutex* pMutex;
#if defined(A_PLATFORM_XOS)
//	pMutex = new AXOSSysThreadMutex(bRecursive);
	pMutex = new AXOSSysThreadMutex();
#elif defined(A_PLATFORM_ANDROID)
    pMutex = new AAndroidSysThreadMutex();
#elif defined(A_PLATFORM_WINRT)
	pMutex = new AWinRTSysThreadMutex();
#else
	pMutex = new AWinSysThreadMutex();
#endif

	return pMutex;
}

/*
ASysMainThread* AThreadFactory::CreateMainThread( A_LPFNATHREADPROC lpfnWorkProc, ASysMainThread::EVENT_DESC* aEvents, aint32 iNumEvent, AThreadData* pUserData )
{
	ASysMainThread* _thread;

#if defined(A_PLATFORM_XOS)
	_thread = new AXOSSysMainThread();
#elif defined(A_PLATFORM_ANDROID)
    _thread = new AAndroidSysMainThread();
#elif defined(A_PLATFORM_WINRT)
	_thread = new AWinRTSysMainThread();
#else
	_thread = new AWinSysMainThread();
#endif

	if(_thread->Create(lpfnWorkProc, aEvents, iNumEvent, pUserData))
	{

		return _thread;
	}
	else
	{
		delete _thread;
		return NULL;
	}
}

ASysThread* AThreadFactory::CreateThread( A_LPFNATHREADPROC lpfnWorkProc, ASysThread::EVENT_DESC* aEvents, aint32 iNumEvent, AThreadData* pUserData, abool bMainThread)
{
	ASysThread* _thread;

#if defined(A_PLATFORM_XOS)
	_thread = new AXOSSysThread();
#elif defined(A_PLATFORM_ANDROID)
    _thread = new AAndroidSysThread();
#elif defined(A_PLATFORM_WINRT)
	_thread = new AWinRTSysThread();
#else
	_thread = new AWinSysThread();
#endif

	if(_thread->Create(lpfnWorkProc, aEvents, iNumEvent, pUserData, bMainThread))
	{
		ALinkList<ASysThread>* pNode =  new ALinkList<ASysThread>;
		pNode->SetOwner(_thread);
		pNode->AddToEnd(m_ThreadList);
		return _thread;
	}
	else
	{
		delete _thread;
		return NULL;
	}
}

AThreadPool* AThreadFactory::CreateThreadPool(auint32 iThreadNum, aint32 iProcessorIndex)
{
	AThreadPool* pThreadPool = new AThreadPool;
	if (!pThreadPool->Create(iThreadNum, iProcessorIndex))
	{
		delete pThreadPool;
		return NULL;
	}

	ALinkList<AThreadPool>* pNode = new ALinkList<AThreadPool>;
	pNode->SetOwner(pThreadPool);
	pNode->AddToEnd(m_ThreadPoolList);

	return pThreadPool;
}


void AThreadFactory::ReleaseMainThread( ASysMainThread* pMainThread )
{
	if (!pMainThread)
		return;

	ALinkList<ASysMainThread>* pNode = m_MainThreadList.NextNode();
	while (pNode)
	{
		ASysMainThread* pt = pNode->Owner();
		if (pt == pMainThread)
		{
			delete pMainThread;
			pNode->Remove();
			delete pNode;
			break;
		}
		pNode = pNode->NextNode();
	}
}

void AThreadFactory::ReleaseThread( ASysThread* pThread )
{
	if (!pThread)
		return;

	ALinkList<ASysThread>* pNode = m_ThreadList.NextNode();
	while (pNode)
	{
		ASysThread* pt = pNode->Owner();

		if (pt == pThread)
		{
			delete pThread;
			pNode->Remove();
			delete pNode;
			break;
		}
		pNode = pNode->NextNode();

	}
}

void AThreadFactory::ReleaseThreadPool( AThreadPool* pThreadPool )
{
	if (!pThreadPool)
		return;

	ALinkList<AThreadPool>* pNode = m_ThreadPoolList.NextNode();
	while (pNode)
	{
		AThreadPool* pPool = pNode->Owner();
		if (pPool == pThreadPool)
		{
			delete pThreadPool;
			pNode->Remove();
			delete pNode;
			break;
		}

		pNode = pNode->NextNode();
	}
}

*/

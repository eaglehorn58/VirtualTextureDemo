//-------------------------------------------------------------------------------------------------
//FileName: AThreadFactory.h
//Created by miaoyu 2013,2,28
//-------------------------------------------------------------------------------------------------
#ifndef _A_THREADFACTORY_H_
#define _A_THREADFACTORY_H_

#include "ATypes.h"
#include "ASysThreadCommon.h"
//#include "ASysMainThread.h"
//#include "ASysThread.h"
#include "AMemBase.h"
//#include "ALinkList.h"

class AThreadPool;

class AThreadFactory : public AMemBase
{
public:

	static ASysThreadAtomic*	CreateThreadAtomic(aint32 iValue);
	static ASysThreadMutex*		CreateThreadMutex(abool bRecursive=atrue);

// 	static ASysMainThread*		CreateMainThread(A_LPFNATHREADPROC lpfnWorkProc, ASysMainThread::EVENT_DESC* aEvents, aint32 iNumEvent, AThreadData* pUserData);
// 	static ASysThread*			CreateThread(A_LPFNATHREADPROC lpfnWorkProc, ASysThread::EVENT_DESC* aEvents, aint32 iNumEvent, AThreadData* pUserData, abool bMainThread=afalse);
// 	static AThreadPool*			CreateThreadPool(auint32 iThreadNum, aint32 iProcessorIndex);
// 
// 	static void ReleaseMainThread(ASysMainThread* pMainThread);
// 	static void ReleaseThread(ASysThread* pThread);
// 	static void ReleaseThreadPool(AThreadPool* pThreadPool);

private:

// 	static ALinkList<ASysMainThread>	m_MainThreadList;
// 	static ALinkList<ASysThread>		m_ThreadList;
// 	static ALinkList<AThreadPool>		m_ThreadPoolList;
};

#endif
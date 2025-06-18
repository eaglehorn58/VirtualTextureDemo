//-------------------------------------------------------------------------------------------------
//FileName:AMemBase.h 
//Created by liyi 2013, 2, 26
//-------------------------------------------------------------------------------------------------

#ifndef _A_MEM_BASE_H_
#define _A_MEM_BASE_H_

//#define : public AMemBase : public AMemBase

//#include "ASys.h"
#include "AMemory.h"

class  AMemBase
{
public:

#ifndef _A_FORBID_MALLOC
	inline void*	operator new(size_t nSize) throw();
	inline void*	operator new( size_t nSize, void* _Ptr) throw();
	inline void		operator delete(void* pData);

	inline void*	operator new[](size_t nSize) throw();
	inline void		operator delete[](void* pData);
	inline void		operator delete(void *, void *) throw() {}
#endif

};

#ifndef _A_FORBID_MALLOC
void* AMemBase::operator new(size_t nSize) throw()
{
	return a_malloc(nSize);
}

 void* AMemBase::operator new(size_t nSize, void* _Ptr) throw()
 {
 	return _Ptr;
 }

void AMemBase::operator delete(void* pData)
{
	a_free(pData);
}

void* AMemBase::operator new[](size_t nSize) throw()
{
	return a_malloc(nSize);
}

void AMemBase::operator delete[](void* pData)
{
	a_free(pData);
}

#endif

#endif //_A_MEM_BASE_H_
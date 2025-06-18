//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "AMemory.h"
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////

void* a_malloc(size_t size)
{
	return malloc(size);
}

void* a_realloc(void* pMem, size_t size)
{
	return realloc(pMem, size);
}

void a_free(void *p)
{
	free(p);
}

void* a_malloctemp(size_t size)
{
	return malloc(size);
}

void a_freetemp(void* p)
{
	free(p);
}


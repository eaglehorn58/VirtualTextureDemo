//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _AMEMORY_H_
#define _AMEMORY_H_

#pragma once

///////////////////////////////////////////////////////////////////////////

void* a_malloc(size_t size);
void* a_realloc(void* pMem, size_t size);
void a_free(void *p);
void* a_malloctemp(size_t size);
void a_freetemp(void* p);



#endif	//	_AMEMORY_H_

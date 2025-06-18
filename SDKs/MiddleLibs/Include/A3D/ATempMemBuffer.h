/*
 * FILE: AXCSWrapper.h
 *
 * DESCRIPTION: Temporary memory buffer wrapper
 *
 * CREATED BY: Duyuxin, 2013/7/2
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _ATEMPMEMBUFFER_H_
#define _ATEMPMEMBUFFER_H_

#include "ATypes.h"
#include "AMemBase.h"
#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////
//
//	ATempMemBuffer: Basic temporary memory buffer
//
///////////////////////////////////////////////////////////////////////////

class ATempMemBuffer : public AMemBase
{
public:
	
	ATempMemBuffer(auint32 uSize) :
	m_uSize(0),
	m_pBuffer(NULL)
	{
		Resize(uSize);
	}

	~ATempMemBuffer()
	{
		Free();
	}

	//	Free buffer
	void Free()
	{
		if (m_pBuffer)
		{
			m_uSize = 0;
			a_freetemp(m_pBuffer);
			m_pBuffer = NULL;
		}
	}

	//	Resize buffer
	void Resize(auint32 uNewSize)
	{
		if (m_uSize == uNewSize)
			return;		//	No size change

		//	Free old buffer
		Free();

		m_uSize = uNewSize;
		m_pBuffer = uNewSize ? a_malloctemp(uNewSize) : NULL;
	}

	//	Get buffer pointer
	void* GetBuffer() const { return m_pBuffer; }
	//	Get buffer size
	auint32 GetSize() const { return m_uSize; }

protected:

	auint32		m_uSize;	//	Buffer size in bytes
	void*		m_pBuffer;	//	Buffer pointer
};

///////////////////////////////////////////////////////////////////////////
//
//	ATempMemTempl: Temporary memory buffer template easy to use
//
///////////////////////////////////////////////////////////////////////////

template <class T>
class ATempMemTempl
{
public:		//	Types

public:		//	Constructions and Destructions

	//	iNumItem: number of item
	ATempMemTempl(auint32 iNumItem) :
	m_tb(sizeof (T) * iNumItem)
	{
	}

	~ATempMemTempl() {}

public:		//	Attributes

public:		//	Operaitons

	//	Free buffer
	void Free() { m_tb.Free(); }
	//	Resize buffer
	void Resize(auint32 iNumItem) { m_tb.Resize(iNumItem * sizeof (T)); }

	//	Get buffer pointer
	T* GetBuffer() const { return (T*)m_tb.GetBuffer(); }
	//	Get buffer size
	auint32 GetSize() const { return m_tb.GetSize(); }
	//	Get maximum item number
	auint32 GetItemNum() const { return GetSize() / sizeof (T); }

protected:	//	Attributes

	ATempMemBuffer	m_tb;

protected:	//	Operations

};

#endif	//	_ATEMPMEMBUFFER_H_



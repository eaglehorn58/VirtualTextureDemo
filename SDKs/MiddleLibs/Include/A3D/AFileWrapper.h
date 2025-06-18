/*
 * FILE: AFileWrapper.h
 *
 * CREATED BY: duyuxin, 2018/11/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AFILEWRAPPER_H_
#define _AFILEWRAPPER_H_

#include <stdio.h>
#include "ATypes.h"
#include "AString.h"

class AFileWrapper
{
public:
	
	AFileWrapper(const char* szFile, const char* mode);
	~AFileWrapper();

	//	Close file
	void Close();
	//	Read data from file
	bool Read(void* pBuf, auint32 size);
	//	Write data to file
	bool Write(const void* pBuf, auint32 size);
	//	Read string from file
	bool ReadString(char* szBuf, int iBufLen);
	bool ReadString(AString& str);
	//	Write string to file
	bool WriteString(const char* sz, int iLen = -1);
	bool WriteString(const AString& str);
	//	Seek to position
	void Seek(aint64 offset, int origin);
	//	Get current position
	auint64 GetPos();
	//	Flush
	void Flush();

	//	Get file length
	auint64 GetLength() const;
	//	Get FILE handle
	FILE* GetFile() { return m_fp; }
	//	Get file name
	const char* GetName() { return m_strName; }

protected:

	FILE*		m_fp;
	AString		m_strName;
};


#endif	//	_FILEWRAPPER_H_



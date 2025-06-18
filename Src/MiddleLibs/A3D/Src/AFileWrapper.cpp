//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "AFileWrapper.h"
#include "ATempMemBuffer.h"

///////////////////////////////////////////////////////////////////////////

AFileWrapper::AFileWrapper(const char* szFile, const char* mode)
{
	ASSERT(szFile && szFile[0]);

	m_fp = fopen(szFile, mode);
	if (!m_fp)
	{
		ASSERT(m_fp);
	//	glb_OutputLog("Failed to open file [%s] !", szFile);
	}

	m_strName = szFile;
}

AFileWrapper::~AFileWrapper()
{
	AFileWrapper::Close();
}

void AFileWrapper::Close()
{
	if (m_fp)
	{
		fclose(m_fp);
		m_fp = NULL;
	}
}

bool AFileWrapper::Read(void* pBuf, auint32 size)
{
	auint32 read_size = fread(pBuf, 1, size, m_fp);
	if (read_size != size)
	{
		ASSERT(read_size == size);
	//	glb_OutputLog("file read error ! want [%d] bytes, read [%d] bytes!", m_strName, size, read_size);
		return false;
	}

	return true;
}

bool AFileWrapper::Write(const void* pBuf, auint32 size)
{
	auint32 write_size = fwrite(pBuf, 1, size, m_fp);
	if (write_size != size)
	{
		ASSERT(write_size == size);
	//	glb_OutputLog("file write error ! want [%d] bytes, write [%d] bytes!", m_strName, size, write_size);
		return false;
	}

	return true;
}

bool AFileWrapper::ReadString(char* szBuf, int iBufLen)
{
	ASSERT(szBuf);

	//	Read length of string
	int iLen = 0;
	if (!Read(&iLen, sizeof(iLen)))
		return false;

	//	Read string data
	if (iLen > 0)
	{
		if (iLen >= iBufLen)
		{
			ASSERT(iLen < iBufLen);
			return false;
		}

		if (!Read(szBuf, iLen))
			return false;

		szBuf[iLen] = '\0';
	}
	else if (!iLen)
	{
		szBuf[0] = '\0';
	}
	else
	{
		ASSERT(0);
		return false;
	}

	return true;
}

bool AFileWrapper::WriteString(const char* sz, int iLen)
{
	if (iLen < 0)
		iLen = strlen(sz);

	//	Write length of string
	if (!Write(&iLen, sizeof(iLen)))
		return false;

	//	Write string data, without the last '\0'
	if (iLen)
	{
		if (!Write(sz, iLen))
			return false;
	}

	return true;
}

bool AFileWrapper::WriteString(const AString& str)
{
	//	Write length of string
	int iLen = str.GetLength();
	if (!Write(&iLen, sizeof(int)))
		return false;

	//	Write string data, without the last '\0'
	if (iLen)
	{
		if (!Write(str, iLen))
			return false;
	}

	return true;
}

bool AFileWrapper::ReadString(AString& str)
{
	//	Read length of string
	int iLen = 0;
	if (!Read(&iLen, sizeof(int)))
		return false;

	//	Read string data
	if (iLen)
	{
		ATempMemTempl<char> tempBuf(iLen + 1);
		char* szBuf = tempBuf.GetBuffer();
		if (!szBuf)
			return false;

		if (!Read(szBuf, iLen))
			return false;

		szBuf[iLen] = '\0';
		str = szBuf;
	}
	else
		str = "";

	return true;
}

void AFileWrapper::Seek(aint64 offset, int origin)
{
	_fseeki64(m_fp, offset, origin);
}

auint64 AFileWrapper::GetPos()
{
	return _ftelli64(m_fp);
}

void AFileWrapper::Flush()
{
	fflush(m_fp);
}

auint64 AFileWrapper::GetLength() const
{
	//	Record current pos
	auint64 cur_pos = _ftelli64(m_fp);

	_fseeki64(m_fp, 0, SEEK_END);
	auint64 len = _ftelli64(m_fp);

	//	Restore pos
	_fseeki64(m_fp, cur_pos, SEEK_SET);

	return len;
}





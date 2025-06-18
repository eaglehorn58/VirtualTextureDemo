#ifndef __MPG123LIB_H__
#define __MPG123LIB_H__

#include <windows.h>
#include <stdio.h>
#include <mmreg.h>

#include "mpg123.h"

class CDecoder;
class CReaders;

class CMpg123File
{
public:

	//	open file
	//	return true for success
	virtual bool open(const char* szfilename) = 0;
	//	close file
	virtual void close() = 0;
	//	seek file
	//	return file pointer position after seeking, -1 for failure
	virtual int seek(int offset, int origin) = 0;
	//	read data from file
	//	return real bytes that was read
	virtual int read(unsigned char *buf, int count) = 0;
	//	get current file position
	virtual int pos() = 0;
};

class CMpg123
{
	friend class CDecoder;

public:

	CMpg123(CMpg123File* pFile = NULL);
	~CMpg123();

public:

	int mpg123_open(char * szfilename);
	int mpg123_close();
	int mpg123_seek(int nSample);
	int mpg123_read(char * buffer, long size, long * preadsize, int * reachend);
	int mpg123_get_total_sample();
	int mpg123_getinfo(long *, int *);

private:

	CReaders* GetReaders() { return m_readers; }// just for internal use.

	int mpg123_init();
	int mpg123_release();
	int get_songlen(struct frame *fr,int no);
	long GetFileOffset();
	int play_frame(int init,struct frame *fr);
	int mpg123_reset();
	int mpg123_play(char * buffer, int size);
	double compute_tpf(struct frame *fr);
	double compute_bpf(struct frame *fr);

private:

	CDecoder* m_decoder;
	CReaders* m_readers;
	int  stereo;
	int frequency;

	unsigned char *pcm_sample;
	int pcm_point;
};
#endif
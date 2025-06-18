#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

#include "mpg123.h"
#include "common.h"
#include "readers.h"
#include "Mp3Lib.h"

class CDefaultMpg123File : public CMpg123File
{
public:

	CDefaultMpg123File() :
	m_fp(NULL)
	{
	}

	virtual ~CDefaultMpg123File()
	{
		if (m_fp)
		{
			fclose(m_fp);
			m_fp = NULL;
		}
	}

public:

	//	open file
	//	return true for success
	virtual bool open(const char* szfilename)
	{
		assert(!m_fp);

		if (!(m_fp = fopen(szfilename, "rb")))
			return false;

		return true;
	}

	//	close file
	virtual void close()
	{
		if (m_fp)
		{
			fclose(m_fp);
			m_fp = NULL;
		}
	}

	//	seek file
	//	return file pointer position after seeking, -1 for failure
	virtual int seek(int offset, int origin)
	{
		assert(m_fp);
		int ret = fseek(m_fp, offset, origin);
		return (ret == 0) ? ftell(m_fp) : -1;
	}

	//	read data from file
	//	return real bytes that was read
	virtual int read(unsigned char *buf, int count)
	{
		assert(m_fp);
		int read_cnt = (int)fread(buf, 1, count, m_fp);
		return read_cnt;
	}

	//	get current file position
	virtual int pos()
	{
		assert(m_fp);
		return (int)ftell(m_fp);
	}

protected:

	FILE*	m_fp;
};


/*******************************************************************
 * stream based operation
 */
int CReaders::default_init(struct reader *rds, const char *bs_filenam)
{
	if (!rds->filept->open(bs_filenam))
		return -1;

	char buf[128];

	rds->filepos = 0;
	rds->filelen = get_fileinfo(rds, buf);

	if (rds->filelen > 0)
	{
		if (!strncmp(buf, "TAG", 3))
		{
			rds->flags |= READER_ID3TAG;
			memcpy(rds->id3buf, buf, 128);
		}
	}

	return 0;
}

void CReaders::stream_close(struct reader *rds)
{
    if (rds->flags & READER_FD_OPENED)
	{
		if (rds->filept)
		{
			rds->filept->close();
		}
	}
}

/**************************************** 
 * HACK,HACK,HACK: step back <num> frames 
 * can only work if the 'stream' isn't a real stream but a file
 */
int CReaders::stream_back_bytes(struct reader *rds,int bytes)
{
	return 0;
}
int CReaders::stream_back_frame(struct reader *rds,struct frame *fr,int num)
{
	return 0;
}

int CReaders::stream_head_read(struct reader *rds,unsigned long *newhead)
{
	unsigned char hbuf[4];

	if(rds->filept->read(hbuf, 4) != 4)
		return 0;
  
	*newhead = ((unsigned long) hbuf[0] << 24) |
		((unsigned long) hbuf[1] << 16) |
		((unsigned long) hbuf[2] << 8)  |
		(unsigned long) hbuf[3];
  
	return 1;
}

int CReaders::stream_head_shift(struct reader *rds,unsigned long *head)
{
	unsigned char hbuf;

	if(rds->filept->read(&hbuf, 1) != 1)
		return 0;

	*head <<= 8;
	*head |= hbuf;
	*head &= 0xffffffff;
	return 1;
}

int CReaders::stream_skip_bytes(struct reader *rds,int len)
{
	return rds->filept->seek(len, SEEK_CUR);
}

int CReaders::stream_read_frame_body(struct reader *rds,unsigned char *buf, int size)
{
	long l;

	if((l=rds->filept->read(buf,size)) != size)
	{
		if(l <= 0)
			return 0;

		memset(buf+l,0,size-l);
	}

	return 1;
}

long CReaders::stream_tell(struct reader *rds)
{
  return rds->filept->seek(0, SEEK_CUR);
}

void CReaders::stream_rewind(struct reader *rds)
{
	rds->filept->seek(0, SEEK_SET);
}

/*
 * returns length of a file (if filept points to a file)
 * reads the last 128 bytes information into buffer
 */
int CReaders::get_fileinfo(struct reader *rds, char *buf)
{
	int len;

    if((len= rds->filept->seek(0, SEEK_END)) < 0)
		return -1;
    
    if(rds->filept->seek(-128,SEEK_END) < 0)
        return -1;

    if(rds->filept->read((unsigned char *)buf,128) != 128) 
		return -1;

    if(!strncmp(buf,"TAG",3)) 
	{
        len -= 128;
    }

    if(rds->filept->seek(0, SEEK_SET) < 0)
        return -1;

    if(len <= 0)
        return -1;

	return len;
}

/*****************************************************************
 * read frame helper
 */

CReaders::CReaders(CMpg123File* pFile)
{
	numframes = -1;
	sample_per_frame = 0;
		
	memset(&m_reader, 0, sizeof(m_reader));
	m_reader.init= &CReaders::default_init;
	m_reader.close = &CReaders::stream_close;
	m_reader.head_read = &CReaders::stream_head_read;
	m_reader.head_shift = &CReaders::stream_head_shift;
	m_reader.skip_bytes = &CReaders::stream_skip_bytes;
	m_reader.read_frame_body = &CReaders::stream_read_frame_body;
	m_reader.back_bytes = &CReaders::stream_back_bytes;
	m_reader.back_frame = &CReaders::stream_back_frame;
	m_reader.tell = &CReaders::stream_tell;
	m_reader.rewind = &CReaders::stream_rewind;

	//	create default file stream if pFile = NULL
	if (!pFile)
	{
		pFile = new CDefaultMpg123File;
	}

	m_reader.filept = pFile;

	rd = NULL;
	m_decoder = NULL;
	fileoffset= 0;
	firsthead=0;
		
	fsizeold=0;
	bsnum=0;
	oldhead = 0;
	bsbuf=bsspace[1];
		
	halfphase = 0;
	mapbuf = NULL;
	mappnt = NULL;
	mapend = NULL;

	lastdir = NULL;
};

CReaders::~CReaders()
{
	if (m_reader.filept)
	{
		//	default file stream was created by CReaders itself.
		CDefaultMpg123File* pDefFile = dynamic_cast<CDefaultMpg123File*>(m_reader.filept);
		if (pDefFile)
		{
			delete m_reader.filept;
		}

		m_reader.filept = NULL;
	}
}

/* open the device to read the bit stream from it */
int CReaders::open_stream(const char *bs_filenam, int fd)
{
    int i;
    int filept_opened = 1;
	unsigned long tempfirsthead;

    rd = NULL;
	m_reader.filelen = -1;
	m_reader.flags   = 0;
	if (filept_opened)
		m_reader.flags |= READER_FD_OPENED;

	if ((this->*m_reader.init)(&m_reader, bs_filenam) < 0)
	{
		//	TODO: release resource
		return 0;
	}

	rd = &m_reader;

    if(rd && rd->flags & READER_ID3TAG) 
	{
		print_id3_tag(rd->id3buf);
    }		   

	//Now we must skip some junks here, hehe :)
	fileoffset = 0;

Init_Resync:
	if( !(this->*rd->head_read)(rd, &tempfirsthead) )
		return 0;
	if( !head_check(tempfirsthead) )
	{
		/* I even saw RIFF headers at the beginning of MPEG streams ;( */
		if(tempfirsthead == ('R'<<24)+('I'<<16)+('F'<<8)+'F') 
		{
			if( !(this->*rd->head_read)(rd, &tempfirsthead) )
				return 0;
			while(tempfirsthead != ('d'<<24)+('a'<<16)+('t'<<8)+'a')
			{
				if(!(this->*rd->head_shift)(rd, &tempfirsthead))
					return 0;
			}
			if(!(this->*rd->head_read)(rd, &tempfirsthead))
				return 0;
		}
		else
		{
			/* step in byte steps through next 64K */
			for(i=0;i<65536;i++)
			{
				if(!(this->*rd->head_shift)(rd, &tempfirsthead))
					return 0;
				if(head_check(tempfirsthead))
					break;
			}
			if(i == 65536) 
			{
				//fprintf(stderr,"Giving up searching valid MPEG header\n");
				return 0;
			}
		}
	}

	//Check the first frame again;
	{
	unsigned long nexthead;

	if(!m_decoder->decode_header(&fr, tempfirsthead))
		goto Init_Resync;

	rd->filept->seek(fr.framesize, SEEK_CUR);
	
	(this->*rd->head_read)(rd, &nexthead);
	rd->filept->seek(-(fr.framesize + 4), SEEK_CUR);

	//If we search for 64K and does not find the head, we must abort :(
	if( rd->filept->pos() > 64 * 1024 ) 
		return 0;
	if( !head_check(nexthead) )
	{
		goto Init_Resync;
	}
	}
	if( !m_decoder->decode_header(&fr, tempfirsthead) )
		goto Init_Resync;

	fileoffset = rd->filept->pos() - 4;

	//Calculate framenum here;
	numframes = (rd->filelen - fileoffset) / (fr.framesize + 4);
	sample_per_frame = get_sample_per_frame(&fr);

	rd->filept->seek(fileoffset, SEEK_SET);
	return 1;
}

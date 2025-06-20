#ifndef _READERS_H_
#define _READERS_H_

#include "mpg123.h"
#include "common.h"
#include "decoder.h"

using namespace std;

class CReaders;
class CMpg123File;

struct reader
{
	int  (CReaders::*init)(struct reader *, const char *bs_filenam);
	void (CReaders::*close)(struct reader *);
	int  (CReaders::*head_read)(struct reader *,unsigned long *newhead);
	int  (CReaders::*head_shift)(struct reader *,unsigned long *head);
	int  (CReaders::*skip_bytes)(struct reader *,int len);
	int  (CReaders::*read_frame_body)(struct reader *,unsigned char *,int size);
	int  (CReaders::*back_bytes)(struct reader *,int bytes);
	int  (CReaders::*back_frame)(struct reader *,struct frame *,int num);
	long (CReaders::*tell)(struct reader *);
	void (CReaders::*rewind)(struct reader *);
	long filelen;
	long filepos;
	CMpg123File* filept;
	int  flags;
	unsigned char id3buf[128];
};

class CReaders
{
public:

	CReaders(CMpg123File* pFile);
	~CReaders();

    reader* GetReader() { return rd; }
	
	int open_stream(const char *bs_filenam,int fd);
//	void SetReader(reader* r){	rd = r;	}
	long GetFileOffset(){return fileoffset;	}
	unsigned long GetNumFrames(){return numframes;}
	unsigned long GetSamplePerFrame(){	return sample_per_frame;}
//	int decode_header(struct frame *fr,unsigned long newhead);
	int read_frame(struct frame *fr);
	int back_frame(struct reader *rds,struct frame *fr,int num);
	struct frame* GetFR(){	return &fr;	}

	void set_pointer(long backstep);	
	void backbits(int number_of_bits);
	int getbitoffset(void);
	int getbyte(void);
	unsigned int getbits(int number_of_bits);
	unsigned int getbits_fast(int number_of_bits);
	unsigned int get1bit(void);
	void SetDecoder(CDecoder* d){m_decoder = d;}
	int read_frame_init (void);
	void SetOldHead(int head){	oldhead = head;	}
	void SetSszie(int size)	{ssize = size;}
	int GetSsize()	{ return ssize;	}
	int head_check(unsigned long head);

	int split_dir_file (const char *path, char **dname, char **fname);

protected:

	int get_sample_per_frame(struct frame *fr);

	int default_init(struct reader *rds, const char *bs_filenam);
	void stream_close(struct reader *rds);
	int stream_back_bytes(struct reader *rds,int bytes);
	int stream_back_frame(struct reader *rds,struct frame *fr,int num);
	int stream_head_read(struct reader *rds,unsigned long *newhead);
	int stream_head_shift(struct reader *rds,unsigned long *head);
	int stream_skip_bytes(struct reader *rds,int len);
	int stream_read_frame_body(struct reader *rds,unsigned char *buf,int size);
	long stream_tell(struct reader *rds);
	void stream_rewind(struct reader *rds);
	int get_fileinfo(struct reader *rds, char *buf);
	
private:
	struct frame fr; // head frame info, just for retrieve the frequency and stero info.
	int halfphase;
	reader m_reader;
	reader *rd;
	long fileoffset;
	struct bitstream_info bsi;
	CDecoder* m_decoder;
	unsigned long firsthead;

	unsigned long numframes;
	unsigned long sample_per_frame;

	int fsizeold;
	int ssize;
	unsigned char bsspace[2][MAXFRAMESIZE+512]; /* MAXFRAMESIZE */
	unsigned char *bsbuf;
	unsigned char *bsbufold;
	int bsnum;
	unsigned long oldhead;

	unsigned char ssave[34];

	unsigned char *mapbuf;
	unsigned char *mappnt;
	unsigned char *mapend;

	char *lastdir;
};

#endif
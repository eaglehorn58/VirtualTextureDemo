/*
 * Mpeg Layer-1,2,3 audio decoder
 * ------------------------------
 * copyright (c) 1995 by Michael Hipp, All rights reserved. See also 'README'
 * version for slower machines .. decodes only every second sample
 * sounds like 24000,22050 or 16000 kHz .. (depending on original sample freq.)
 *
 */

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "mpg123.h"
#include "decoder.h"

#define WRITE_SAMPLE(samples,sum,clip) \
  if( (sum) > 32767.0) { *(samples) = 0x7fff; (clip)++; } \
  else if( (sum) < -32768.0) { *(samples) = -0x8000; (clip)++; } \
  else { *(samples) = (short)sum; }

int CDecoder::synth_2to1_8bit(real *bandPtr,int channel,unsigned char *samples,int *pnt)
{
  short samples_tmp[32];
  short *tmp1 = samples_tmp + channel;
  int i,ret;
  int pnt1 = 0;

  ret = synth_2to1(bandPtr,channel,(unsigned char *) samples_tmp,&pnt1);
  samples += channel + *pnt;

  for(i=0;i<16;i++) {
    *samples = conv16to8[*tmp1>>AUSHIFT];
    samples += 2;
    tmp1 += 2;
  }
  *pnt += 32;

  return ret;
}

int CDecoder::synth_2to1_8bit_mono(real *bandPtr,unsigned char *samples,int *pnt)
{
  short samples_tmp[32];
  short *tmp1 = samples_tmp;
  int i,ret;
  int pnt1 = 0;

  ret = synth_2to1(bandPtr,0,(unsigned char *) samples_tmp,&pnt1);
  samples += *pnt;

  for(i=0;i<16;i++) {
    *samples++ = conv16to8[*tmp1>>AUSHIFT];
    tmp1 += 2;
  }
  *pnt += 16;

  return ret;
}


int CDecoder::synth_2to1_8bit_mono2stereo(real *bandPtr,unsigned char *samples,int *pnt)
{
  short samples_tmp[32];
  short *tmp1 = samples_tmp;
  int i,ret;
  int pnt1 = 0;

  ret = synth_2to1(bandPtr,0,(unsigned char *) samples_tmp,&pnt1);
  samples += *pnt;

  for(i=0;i<16;i++) {
    *samples++ = conv16to8[*tmp1>>AUSHIFT];	
    *samples++ = conv16to8[*tmp1>>AUSHIFT];
    tmp1 += 2;
  }
  *pnt += 32;

  return ret;
}

int CDecoder::synth_2to1_mono(real *bandPtr,unsigned char *samples,int *pnt)
{
  short samples_tmp[32];
  short *tmp1 = samples_tmp;
  int i,ret;
  int pnt1=0;

  ret = synth_2to1(bandPtr,0,(unsigned char *) samples_tmp,&pnt1);
  samples += *pnt;

  for(i=0;i<16;i++) {
    *( (short *) samples) = *tmp1;
    samples += 2;
    tmp1 += 2;
  }
  *pnt += 32;

  return ret;
}

int CDecoder::synth_2to1_mono2stereo(real *bandPtr,unsigned char *samples,int *pnt)
{
  int i,ret;

  ret = synth_2to1(bandPtr,0,samples,pnt);
  samples = samples + *pnt - 64;

  for(i=0;i<16;i++) {
    ((short *)samples)[1] = ((short *)samples)[0];
    samples+=4;
  }
  
  return ret;
}

int CDecoder::synth_2to1(real *bandPtr,int channel,unsigned char *out,int *pnt)
{

  short *samples = (short *) (out + *pnt);

  real *b0,(*buf)[0x110];
  int clip = 0; 
  int bo1;

  if(equalfile)
    do_equalizer(bandPtr,channel);

  if(!channel) {
    bo_synth_2to1--;
    bo_synth_2to1 &= 0xf;
    buf = buffs_synth_2to1[0];
  }
  else {
    samples++;
    buf = buffs_synth_2to1[1];
  }

  if(bo_synth_2to1 & 0x1) {
    b0 = buf[0];
    bo1 = bo_synth_2to1;
    dct64(buf[1]+((bo_synth_2to1+1)&0xf),buf[0]+bo_synth_2to1,bandPtr);
  }
  else {
    b0 = buf[1];
    bo1 = bo_synth_2to1+1;
    dct64(buf[0]+bo_synth_2to1,buf[1]+bo_synth_2to1+1,bandPtr);
  }

  {
    register int j;
    real *window = decwin + 16 - bo1;

    for (j=8;j;j--,b0+=0x10,window+=0x30)
    {
      real sum;
      sum  = *window++ * *b0++;
      sum -= *window++ * *b0++;
      sum += *window++ * *b0++;
      sum -= *window++ * *b0++;
      sum += *window++ * *b0++;
      sum -= *window++ * *b0++;
      sum += *window++ * *b0++;
      sum -= *window++ * *b0++;
      sum += *window++ * *b0++;
      sum -= *window++ * *b0++;
      sum += *window++ * *b0++;
      sum -= *window++ * *b0++;
      sum += *window++ * *b0++;
      sum -= *window++ * *b0++;
      sum += *window++ * *b0++;
      sum -= *window++ * *b0++;

      WRITE_SAMPLE(samples,sum,clip); 
	  samples += step_synth_2to1;
    }

    {
      real sum;
      sum  = window[0x0] * b0[0x0];
      sum += window[0x2] * b0[0x2];
      sum += window[0x4] * b0[0x4];
      sum += window[0x6] * b0[0x6];
      sum += window[0x8] * b0[0x8];
      sum += window[0xA] * b0[0xA];
      sum += window[0xC] * b0[0xC];
      sum += window[0xE] * b0[0xE];
      WRITE_SAMPLE(samples,sum,clip); samples += step_synth_2to1;
#if 0
      WRITE_SAMPLE(samples,sum,clip); samples += step_synth_2to1;
#endif
      b0-=0x20,window-=0x40;
    }
    window += bo1<<1;

    for (j=7;j;j--,b0-=0x30,window-=0x30)
    {
      real sum;
      sum = -*(--window) * *b0++;
      sum -= *(--window) * *b0++;
      sum -= *(--window) * *b0++;
      sum -= *(--window) * *b0++;
      sum -= *(--window) * *b0++;
      sum -= *(--window) * *b0++;
      sum -= *(--window) * *b0++;
      sum -= *(--window) * *b0++;
      sum -= *(--window) * *b0++;
      sum -= *(--window) * *b0++;
      sum -= *(--window) * *b0++;
      sum -= *(--window) * *b0++;
      sum -= *(--window) * *b0++;
      sum -= *(--window) * *b0++;
      sum -= *(--window) * *b0++;
      sum -= *(--window) * *b0++;

      WRITE_SAMPLE(samples,sum,clip); samples += step_synth_2to1;
#if 0
      WRITE_SAMPLE(samples,sum,clip); samples += step_synth_2to1;
#endif
    }
  }

  *pnt += 64;

  return clip;
}



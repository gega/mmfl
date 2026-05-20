#ifndef __MMFL_H
#define __MMFL_H

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifndef MMFL_FD_TYPE
#define MMFL_FD_TYPE int
#endif

#ifndef MMFL_LEN_BASE
#define MMFL_LEN_BASE 10
#endif

#define MMFL_LOG2_BASE_2 1000000
#define MMFL_LOG2_BASE_3 630929
#define MMFL_LOG2_BASE_4 500000
#define MMFL_LOG2_BASE_5 430676
#define MMFL_LOG2_BASE_6 386852
#define MMFL_LOG2_BASE_7 356207
#define MMFL_LOG2_BASE_8 333333
#define MMFL_LOG2_BASE_9 315464
#define MMFL_LOG2_BASE_10 301029
#define MMFL_LOG2_BASE_11 289064
#define MMFL_LOG2_BASE_12 278942
#define MMFL_LOG2_BASE_13 270238
#define MMFL_LOG2_BASE_14 262649
#define MMFL_LOG2_BASE_15 255958
#define MMFL_LOG2_BASE_16 250000
#define MMFL_LOG2_BASE_17 244650
#define MMFL_LOG2_BASE_18 239812
#define MMFL_LOG2_BASE_19 235408
#define MMFL_LOG2_BASE_20 231378
#define MMFL_LOG2_BASE_21 227670
#define MMFL_LOG2_BASE_22 224243
#define MMFL_LOG2_BASE_23 221064
#define MMFL_LOG2_BASE_24 218104
#define MMFL_LOG2_BASE_25 215338
#define MMFL_LOG2_BASE_26 212746
#define MMFL_LOG2_BASE_27 210309
#define MMFL_LOG2_BASE_28 208014
#define MMFL_LOG2_BASE_29 205846
#define MMFL_LOG2_BASE_30 203795
#define MMFL_LOG2_BASE_31 201849
#define MMFL_LOG2_BASE_32 199999
#define MMFL_LOG2_BASE_33 198239
#define MMFL_LOG2_BASE_34 196561
#define MMFL_LOG2_BASE_35 194959
#define MMFL_LOG2_BASE_36 193426

#define MMFL_CAT(a,b) a##b
#define MMFL_XCAT(a,b) MMFL_CAT(a,b)

#define MMFL_TYPE_BITS(type) (sizeof(type) * 8)
#define MMFL_TYPE_DIGITS(t, base) \
    ((MMFL_TYPE_BITS(t) * MMFL_XCAT(MMFL_LOG2_BASE_, base)) / 1000000 + 1)

#define MMFL_HDR_MAX (3+(MMFL_TYPE_DIGITS(MMFL_FD_TYPE,MMFL_LEN_BASE)))

#define MMFL_PEEK (1)


_Static_assert(MMFL_LEN_BASE>=2 && MMFL_LEN_BASE<=36, "length base must be between 2 and 36");

typedef struct mmfl_s
{
  long bp;
  int bsiz;
  MMFL_FD_TYPE fd;
  char *buf;
  char *ep;
  long mlen;
  long olen;
  char zero;
  int rdy;
  char sep;
} mmfl_t;

/*
 * message format on wire:
 *   "\n11 hello world\n2 ok"
 *    msg1            msg2
 * \n len <space> message
 */

#define MMFL_INIT(r,b,s,f) do { \
    bzero((r),sizeof(struct mmfl_s)); \
    if(s>MMFL_HDR_MAX) { \
      bzero((r),sizeof(struct mmfl_s)); (r)->buf=(b); (r)->bsiz=(s)-1; (r)->mlen=-1; (r)->fd=(f); \
    } \
  } while(0)

#define MMFL_IS_INITED(r) ((r)->buf!=NULL)

#define MMFL_READMSG(rs,rt,ln,rd,pk) \
do { \
  if((rs)->buf==NULL) break; \
  if((rs)->rdy==1+MMFL_PEEK) { \
    (rt)=(rs)->ep+1; \
    (ln)=(rs)->olen; \
    (rs)->rdy=(pk)+1; \
    break; \
  } \
  if((rs)->rdy==1) { \
    memmove((rs)->buf,&(rs)->buf[(rs)->mlen],(rs)->bp-(rs)->mlen); \
    (rs)->buf[0]=(rs)->zero; \
    (rs)->bp-=(rs)->mlen; \
    (rs)->mlen=-1; \
    (rs)->rdy=0; \
    (rs)->sep=0; \
  } \
  while((rs)->rdy==0) { \
    if((rs)->mlen<0&&0==(rs)->sep) { \
      char *n=memchr((rs)->buf,'\n',(rs)->bp); \
      if(NULL!=n) { \
        memmove((rs)->buf,&(rs)->buf[n-(rs)->buf+1],(rs)->bp-(n-(rs)->buf+1)); \
        (rs)->bp-=n-(rs)->buf+1; \
        (rs)->sep=1; \
      } else { \
        (rs)->bp=0; \
        (rs)->mlen=-1; \
      } \
    } \
    if((rs)->mlen<0&&0!=(rs)->sep) { \
      char *s=memchr((rs)->buf,' ',(rs)->bp); \
      if(s!=NULL) { \
        (rs)->mlen=strtol((rs)->buf,&(rs)->ep,MMFL_LEN_BASE); \
        if((rs)->mlen>0) { \
          (rs)->olen=(rs)->mlen; \
          (ln)=(rs)->mlen; \
          (rs)->mlen+=(rs)->ep-(rs)->buf+1; \
        } else { \
          (rs)->mlen=-1; \
          char *n=memchr((rs)->buf,'\n',(rs)->bp); \
          if(NULL!=n&&n<s) s=n; \
          memmove((rs)->buf,&(rs)->buf[s-(rs)->buf+1],(rs)->bp-(s-(rs)->buf+1)); \
          (rs)->bp-=s-(rs)->buf+1; \
          continue; \
        } \
      } else { \
        (rs)->mlen=-1; \
        (rs)->sep=1; \
      } \
    } \
    if((rs)->mlen<0||(rs)->bp<(rs)->mlen) { \
      int len=(rd)((rs)->fd,&(rs)->buf[(rs)->bp],((rs)->bsiz-(rs)->bp)); \
      if(len<=0) { (rt)=NULL; (ln)=len; break; } \
      (rs)->bp+=len; \
    } \
    else if((rs)->mlen>=0&&(rs)->bp>=(rs)->mlen) { \
      (rs)->zero=(rs)->buf[(rs)->mlen]; \
      (rs)->buf[(rs)->mlen]='\0'; \
      (rt)=(rs)->ep+1; \
      (rs)->rdy=(pk)+1; \
    } \
    else break; \
  } \
} while(0)

#endif

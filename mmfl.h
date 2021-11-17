#ifndef __MMFL_H
#define __MMFL_H

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>



typedef struct rb_s
{
  long bp;
  int bsiz;
  int fd;
  char *buf;
  char *ep;
  long mlen;
  char zero;
  int rdy;
  char sep;
} rb_t;

/*
 * message format on wire:
 *   "\n11 hello world\n2 ok"
 *    msg1            msg2
 * \n len <space> message
 */

#define RB_INIT(r,b,s,f) do { bzero((r),sizeof(struct rb_s)); (r)->buf=(b); (r)->bsiz=(s)-1; (r)->mlen=-1; (r)->fd=(f); } while(0)

#define RB_READMSG(rs,rt,ln,rd) \
do { \
  if((rs)->rdy) { \
    memmove((rs)->buf,&(rs)->buf[(rs)->mlen],(rs)->bp-(rs)->mlen); \
    (rs)->buf[0]=(rs)->zero; \
    (rs)->bp-=(rs)->mlen; \
    (rs)->mlen=-1; \
    (rs)->rdy=0; \
    (rs)->sep=0; \
  } \
  while(1) { \
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
        (rs)->mlen=strtol((rs)->buf,&(rs)->ep,10); \
        if((rs)->mlen>0) { \
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
      (rs)->rdy=1; \
      break; \
    } \
  } \
} while(0)

#endif

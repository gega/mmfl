#ifndef __MMFL_H
#define __MMFL_H

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


typedef struct rb_s
{
  int bp;
  int bsiz;
  int fd;
  char *buf;
  char *ep;
  long mlen;
  char zero;
} rb_t;

/*
 * message format on wire:
 *   "\n11 hello world\n2 ok"
 *    msg1            msg2
 * \n len <space> message
 */

#define rb_init(r,b,s,f) do { bzero((r),sizeof(struct rb_s)); (r)->buf=(b); (r)->bsiz=(s); (r)->mlen=-1; (r)->fd=(f); } while(0)

#define rb_readmsg(rs,rt,ln,rd) \
do { \
  char sep=1; \
  if((rs)->mlen>=0) { \
    memmove((rs)->buf,&(rs)->buf[(rs)->mlen],(rs)->bp-(rs)->mlen); \
    (rs)->buf[0]=(rs)->zero; \
    (rs)->bp-=(rs)->mlen; \
    (rs)->mlen=-1; \
  } \
  /* look for new message */ \
  while(sep!='\n') { if(0>=(rd)((rs)->fd,&sep,1)) { sep=0; break; } } \
  if(sep) { while(1) { \
    if((rs)->mlen<0) { \
      if(memchr((rs)->buf,' ',(rs)->bp)!=NULL) { \
        (rs)->mlen=strtol((rs)->buf,&(rs)->ep,10); \
        (ln)=(rs)->mlen; \
        (rs)->mlen+=(rs)->ep-(rs)->buf+1; \
      } else { (ln)=-1; } \
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
      break; \
    } \
  } } else { (rt)=NULL; (ln)=-1; } \
} while(0)

#endif

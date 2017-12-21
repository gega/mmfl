#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "mmfl.h"


int main(void)
{
  rb_t rbv;
  char buf[14];
  char *msg;
  int len,fd;

  fd=open("test.txt",0,O_RDONLY);
  rb_init(&rbv,buf,sizeof(buf),fd);

  do {
    rb_readmsg(&rbv,msg,len,read);
    if(msg==NULL&&len>=0) perror("message truncated or buffer too small");
    else if(msg==NULL&&len<0) perror("eof or error");
    else printf("message: [%d] '%s'\n",len,msg);
  } while(msg!=NULL);
  
  close(fd);

  return(0);
}

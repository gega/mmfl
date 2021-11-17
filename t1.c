#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "mmfl.h"


static int mock_read_count[]=
{
  5,4,5,3,4,3,5,4,2,3,3,1,3,2,5,5,1,2,3,3,1,1,2,4,2,3,1,3,5,4,2,4,5,4,1,5,3,1,4,5,3,
  5,1,1,3,5,2,2,5,2,2,3,1,5,4,1,1,1,1,1,1,2,4,5,5,4,5,5,2,2,1,3,2,3,4,1,2,2,4,1,3,1,
  2,2,4,3,1,4,3,3,3,3,3,4,2,1,1,2,4,1,
  -1
};
static int mock_read_idx=0;


static ssize_t mock_read(int fd, void *buf, size_t count)
{
  int cn;
  
  cn=mock_read_count[++mock_read_idx];
  if(mock_read_count[mock_read_idx]<0) mock_read_idx=0;
  return(read(fd,buf,(cn<count?cn:count)));
}

static int bufsizes[]={ 15, 140, 5, -1 };


int main(int argc, char **argv)
{
  rb_t rbv;
  char *buf;
  char *msg;
  int len,fd;
  char fn[32];
  char rn[32];
  int i,j,ts=0,k;
  char rdb[200];
  FILE *f;

  if(argc>1) ts=atoi(argv[1]);
  for(j=0;bufsizes[j]>0;j++)
  {
    printf("tests for buffer size %d\n",bufsizes[j]);
    buf=calloc(1,bufsizes[j]);
    if(NULL!=buf)
    {
      for(i=ts;;i++)
      {
        snprintf(fn,sizeof(fn),"test%d.txt",i);
        snprintf(rn,sizeof(rn),"test%d.exp",i);
        if(0>(fd=open(fn,0,O_RDONLY))) break;
        if(NULL==(f=fopen(rn,"r"))) break;
        printf("testing '%s'\n",fn);

        for(k=0;k<10;k++)
        {
          mock_read_idx=k;
          
          rewind(f);
          lseek(fd,0,SEEK_SET);

          RB_INIT(&rbv,buf,bufsizes[j],fd);

          do {
            RB_READMSG(&rbv,msg,len,mock_read);          
            if(msg==NULL&&len>=0)
            {
               //printf("END\n");
               ;
            }
            else if(msg==NULL&&len<0) perror("eof or error");
            else 
            {
              if(NULL==(fgets(rdb,sizeof(rdb)-1,f)))
              {
                printf("ERROR: missing expected string!\n");
                exit(1);
              }
              rdb[strlen(rdb)-1]='\0';
              if(strcmp(rdb,msg)!=0)
              {
                printf("FAIL: read '%s' instead of '%s'\n",msg,rdb);
              }
              //else printf("OK\n");
            }
          } while(msg!=NULL);
        }
        
        fclose(f);
        close(fd);
        if(argc>1) break;
      }
      free(buf);
    }
  }

  return(0);
}

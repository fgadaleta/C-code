#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(void)
{

  int i, fd;
  void *buf;
  void *mymem = malloc(4);
 	

  fd = open("test-data", O_RDONLY);

fprintf(stderr, "before: mymem=0x%08x buf=0x%08x\n", mymem, buf);
  
  buf = mmap(
	0,  
	4,
	PROT_READ | PROT_WRITE,
	MAP_PRIVATE, 
	fd, 
	0);

fprintf(stderr, "after: mymem=0x%08x buf=0x%08x\n", mymem, buf);
  
  //woo
  close(fd);
  
//  for(i=0; i <4; i++)
//        printf("%c\n", buf[i]);

  return(0);
}



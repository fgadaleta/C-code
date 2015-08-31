#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFSIZE 128

int main(void)
{
int len;
int fd;
unsigned long* buffer = malloc(sizeof(char)*BUFSIZE);
//char* string  = malloc(sizeof(char)*BUFSIZE);
unsigned long arguments[6];

int num = 0x2;
char tokens[3];
int current;
char *src;


unsigned long gpa = 0xdeadbeaf;
char *text = "text";
int i;

if ((fd = open("/dev/trusted", O_RDWR)) >0  ) {
    fprintf(stderr, "/dev/trusted opened \n");
	memset(buffer, 0, BUFSIZE);
   
  arguments[0] = num; 
  arguments[1] = gpa; 
  arguments[2] = gpa; 
    
    // write arguments to device
    write(fd, arguments, 24);
 
    // read from device and store in buffer 
    fprintf(stdout, "Reading %d from device to buffer at %p\n", BUFSIZE, buffer);
    read(fd, buffer, BUFSIZE);
    
    // debug only: print read buffer
    fprintf(stdout, "[DEBUG]\n");
    for(i=0;i<BUFSIZE;i++)
       fprintf(stdout, "[%#lx]", *(buffer+i));
    fprintf(stdout, "\n");
	
    close(fd);
    free(buffer);
}

else 
 fprintf(stderr, "Cannot open /dev/trusted\n");

return (0);
}

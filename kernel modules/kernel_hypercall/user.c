#include <stdio.h>
#include <unistd.h>


#define DEVSIZE 64

int main(void)
{
int len;
FILE *fd;
char* buffer = malloc(sizeof(char)*DEVSIZE);
char str[2];
str[0] = 'a';
str[1] = 'b';


if ((fd = fopen("/dev/memory", "rw")) != NULL ) {
    fprintf(stderr, "/dev/memory opened \n");
	memset(buffer, 0, sizeof(buffer));
    
    fprintf(stdout, "writing [%c] to device\n", str[0]);
    fwrite(&str, sizeof(char), 1, fd);

    fprintf(stdout, "reading from device to buffer \n");
    fread(buffer ,sizeof(char), 1, fd);
	
    fclose(fd);
}

else 
 fprintf(stderr, "Cannot open /dev/memory\n");

fprintf(stderr, "buffer is %c\n", buffer);

return (0);
}

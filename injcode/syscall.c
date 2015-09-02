#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>


int main(void)
{

// plt
fprintf(stdout, "open is at %p\n", &open );
fprintf(stdout, "fork is at %p\n", &fork );
fprintf(stdout, "read is at %p\n", &read );
fprintf(stdout, "mmap is at %p\n", &mmap );
fprintf(stdout, "mprotect is at %p\n", &mprotect );
system("ls");


return(0);
}

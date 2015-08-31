// Linux Device Driver Template/Skeleton
// Userspace test program

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#define CASE1 1
#define CASE2 2

main() {
	int fd, len, wlen;
	char string[] = "Skeleton Kernel Module Test";
	char receive[128];
	int data, rdata;

	fd = open("/dev/skeleton", O_RDWR);
	if( fd == -1) {
		printf("open error...\n");
		exit(0);
	}

	wlen = strlen(string) + 1;
	len = write(fd, string, wlen);
	if( len == -1 ) {
		printf("write error...\n");
		exit(1);
	}

	printf("String '%s' written to /dev/skeleton\n", string);
	
	len = read(fd, receive, 128);
	if( len == -1 ) {
		printf("read error...\n");
		exit(1);
	}

	printf("String '%s' read from /dev/skeleton\n", receive);

	data = 0x55555555;
	ioctl(fd, CASE1, &data);
	ioctl(fd, CASE2, &rdata);

	printf("IOCTL test: written: '%x' - received: '%x'\n", data, rdata);

	close(fd);
}


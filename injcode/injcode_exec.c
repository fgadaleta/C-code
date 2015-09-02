#include <stdio.h>


char code[] = "\x31\xc0\xbb\x42\x24"
	      "\x80\x7c\x66\xb8\x88"
 	      "\x13\x50\xff\xd3";

const char pause[] = "\x31\xc0\xb0\x1d\xcd\x80";

char vmcall[] = "\x31\xc0\xb0\x05\x31\xdb\xb3\x05\x31\xc9\xb1\x05\x0f\x01\xc1";
   

int (*func)();


int main(void)
{
int i;
int size = sizeof(vmcall);
vmcall[size-1] = 0x90;
fprintf(stderr, "executing injected code [size=%d]\n", sizeof(vmcall));

for(i=0; i< sizeof(vmcall); i++) 
	fprintf(stderr, "0x%02x ", vmcall[i]);

func = (int (*)())vmcall;

func();


//return(0);
}

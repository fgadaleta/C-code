#include <stdio.h>

char flycode[] = "\xbb\x00\x00\x00\x00"
                 "\xb8\x01\x00\x00\x00"
                 "\xcd\x80";

char code[] = "\x31\xc0\xbb\x42\x24"
	      "\x80\x7c\x66\xb8\x88"
 	      "\x13\x50\xff\xd3";

const char pause[] = "\x31\xc0\xb0\x1d\xcd\x80";

int (*func)();


int main(void)
{
fprintf(stderr, "executing code [size=%d]\n", sizeof(pause));

//func = (int (*)())flycode;
func = (int (*)())pause;

func();


//return(0);
}

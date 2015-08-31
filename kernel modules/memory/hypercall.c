#include <stdio.h>

int main(void)
{
unsigned long hyper_n;


char fixed[] = "i_am_content_to_be_protected_\
i_am_content_to_be_protected_i_am_content_to_be_\
protected_i_am_content_to_be_protected_i_am_conten";

printf("Raising hypercall...%d", sizeof(fixed));

asm volatile ("vmcall"
               : "=a" (hyper_n)               // output operand
               : "a" (4),"b"(1), "c"(1), "d"(0xcafebabe)  // obj index
                                               // input operands
                                               // clobbered registers 
              );

printf("done.\n");




return(0);
}

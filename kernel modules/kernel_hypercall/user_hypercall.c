#include <stdio.h>

int main(void)
{
unsigned long hyper_n;
unsigned long cr3val;

printf("Raising hypercall...");

asm volatile ("movl %%cr3, %0\n" 
                :"=r"(cr3val)
             );
printf("CR3 %lx\n", cr3val);

asm volatile ("vmcall"
               : "=a" (hyper_n)               // output operand
               : "a" (4),"b"(1), "c"(1), "d"(0xcafebabe)  // obj index
                                               // input operands
                                               // clobbered registers 
              );

printf("done.\n");



return(0);
}

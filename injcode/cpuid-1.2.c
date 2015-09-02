/*
 * cpuid.c
 *
 * This program tries to determine CPU type and other CPU data
 * from user-level. It displays information similar to linux'
 * /proc/cpuinfo for a single processor along with some explanation
 * as found in Intel and/or AMD manuals.
 *
 * Changes:
 * 05/Jan/00 V1.2 Updated Tables according to current Intel Instruction Set
 *                Reference. Fixed typos. Most changes made by Jean-Pierre
 *                Panziera <jpp@paris.sgi.com> - Thanks Jean-Pierre!
 * 15/Sep/98 V1.1 Added AMD Support and cpu_identifier_map - gjh.
 * 14/Sep/98 V1.0 Initial version - Intel Processors only - gjh.
 * 
 * Copyright by Gerald J. Heim <heim@informatik.uni-tuebingen.de> and the
 * University of Tübingen, Germany. All rights reserved.
 * This software is provided under the terms of the GNU General Public License.
 * It is provided as it is. No guarantee on functionality or absence of bugs 
 * is given. Please send bug reports back to the author.
 *
 * Compile with gcc -Wall -O2 -o cpuid cpuid.c
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

/*
 * asm-CPUID result structure.
 */
struct cpuid_type {
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
};
typedef struct cpuid_type cpuid_t;

/*
 * Table of available Processor descriptions.
 * (Source: Intel/AMD online manuals.)
 */
struct cpu_ident {
    unsigned int family;
    unsigned int model;
    char         manufacturerid[12];
    char         expln[64];
};
typedef struct cpu_ident cpu_identifier;

static cpu_identifier cpu_identifier_map[] =
{ 
    /* Family, Model, Manufacturer, Explanation */
    { 4, 0, "GenuineIntel", "Intel 486 DX" },
    { 4, 1, "GenuineIntel", "Intel 486 DX" },
    { 4, 2, "GenuineIntel", "Intel 486 SX" },
    { 4, 3, "GenuineIntel", "Intel 487, 486 DX2 or DX2 OverDrive" },
    { 4, 4, "GenuineIntel", "Intel 486 SL" },
    { 4, 5, "GenuineIntel", "Intel 486 SX2" },
    { 4, 7, "GenuineIntel", "Intel 486 DX2 w/ writeback" },
    { 4, 8, "GenuineIntel", "Intel 486 DX4 or DX4 OverDrive" },
    { 5, 1, "GenuineIntel", "Intel Pentium 60/66" },
    { 5, 2, "GenuineIntel", "Intel Pentium 75/90/100/120/133/150/166/200" },
    { 5, 3, "GenuineIntel", "Intel Pentium OverDrive for 486 Systems" },
    { 5, 4, "GenuineIntel", "Intel Pentium w/ MMX 166/200/233" },
    { 6, 1, "GenuineIntel", "Intel Pentium Pro" },
    { 6, 3, "GenuineIntel", "Intel Pentium II" },
    { 6, 5, "GenuineIntel", "Future Intel P6" },
    { 5, 0, "AuthenticAMD", "AMD-K5-PR75/90/100" },
    { 5, 1, "AuthenticAMD", "AMD-K5-PR120/133" },
    { 5, 2, "AuthenticAMD", "AMD-K5-PR150/166" },
    { 5, 3, "AuthenticAMD", "AMD-K5-PR200" },
    { 5, 6, "AuthenticAMD", "AMD-K6 Model 6 (2.9 / 3.2 V Types)" },
    { 5, 7, "AuthenticAMD", "AMD-K6 Model 7 (2.2 V Types)" },
    { 5, 8, "AuthenticAMD", "AMD-K6-2 3D Model 8" },
    { 5, 9, "AuthenticAMD", "AMD-K6-2 3D+ Model 9" },
    { 0, 0, "", ""} /* End marker, don't remove! */
};

/*
 * CPU features as returned by CPUID 1
 * Intel (CPUID 1) and AMD (CPUID 0x80000001) maps differ slightly.
 */
struct cpu_ftr {
    unsigned int bit;
    char         name[8];
    char         expln[64];
};
typedef struct cpu_ftr cpu_feature;

/* This is according to Intel's manuals. */
static cpu_feature intel_cpu_feature_map[] =
{   {  0,    "FPU", "FPU on-chip" },
    {  1,    "VME", "Virtual-8086 Mode Enhancement" },
    {  2,     "DE", "Debugging Extensions" },
    {  3,    "PSE", "Page Size Extensions" },
    {  4,    "TSC", "Time Stamp Counter" },
    {  5,    "MSR", "Model Specific Registers with RDMSR/WRMSR Support" },
    {  6,    "PAE", "Physical Adress Extensions" },
    {  7,    "MCE", "Machine Check Exception" },
    {  8,    "CXS", "CMPXCHG8B Instruction Supported" },
    {  9,   "APIC", "On-chip APIC" },
    { 11,    "SEP", "Fast System Call" },
    { 12,   "MTRR", "Memory Type Range Registers" },
    { 13,    "PGE", "PTE Global Bit" },
    { 14,    "MCA", "Machine Check Architecture" },
    { 15,   "CMOV", "Conditional Move/Cmp. Inst." },
    { 16,  "FGPAT", "Page Attribute Table / CMOVcc" },
    { 17, "PSE-36", "36-bit Page Size Extension" },
    { 18,     "PN", "Processor Serial Number (enabled)" },
    { 23,    "MMX", "MMX Extension" },
    { 24,   "FXSR", "Fast FP/MMX Save and Restore" },
    { 25,    "XMM", "Streaming SIMD Extensions" },
    {255,     "", "" } /* End marker, don't remove! */
};

/* These are AMD specific features detected with CPUID 0x80000001. */
static cpu_feature amd_cpu_feature_map[] =
{   {  0,  "FPU", "FPU on-chip" },
    {  1,  "VME", "Virtual-8086 Mode Enhancement" },
    {  2,   "DE", "Debugging Extensions" },
    {  3,  "PSE", "Page Size Extensions" },
    {  4,  "TSC", "Time Stamp Counter" },
    {  5,  "MSR", "Model Specific Registers with RDMSR/WRMSR Support" },
    {  7,  "MCE", "Machine Check Exception" },
    {  8,  "CXS", "CMPXCHG8B Instruction Supported" },
    { 10,  "SEP", "Fast System Call (pobably A-stepping processor)" },
    { 11,  "SEP", "Fast System Call (B or later stepping)" },
    { 13,  "PGE", "PTE Global Bit" },
    { 15, "CMOV", "Conditional Move/Cmp. Inst." },
    { 23,  "MMX", "MMX Extension" },
    { 31,  "3DN", "3D Now Extension" },
    {255,     "", "" } /* End marker, don't remove! */
};
 

/*
 * This table describes the possible cache and TLB configurations
 * as documented by Intel. For now AMD doesn't use this but gives
 * exact cache layout data on CPUID 0x8000000x.
 *
 * MAX_CACHE_FEATURES_ITERATIONS limits the possible cache information
 * to 80 bytes (of which 16 bytes are used in generic Pentii2).
 * With 80 possible caches we are on the safe side for one or two years.
 *
 * Strange enough no BHT, BTB or return stack data is given this way...
 */
#define MAX_CACHE_FEATURES_ITERATIONS 8

struct cache_ftr {
    unsigned char label;
    char          expln[80];
};
typedef struct cache_ftr cache_feature;

static cache_feature intel_cache_feature_map[] =
{   { 0x01 , "Instruction TLB for 4K-Byte Pages: 32 entries, 4-way set associative" },
    { 0x02 , "Instruction TLB for 4M-Byte Pages:  2 entries, fully associative" },
    { 0x03 , "Data        TLB for 4K-Byte Pages: 64 entries, 4-way set associative" },
    { 0x04 , "Data        TLB for 4M-Byte Pages:  8 entries, 4-way set associative" },
    { 0x06 , "Instruction Cache:    8K Bytes, 32 Byte lines, 4-way set associative" },
    { 0x08 , "Instruction Cache:   16K Bytes, 32 Byte lines, 4-way set associative" },
    { 0x0a , "Data Cache:    8K Bytes, 32 Byte lines, 2-way set associative" },
    { 0x0c , "Data Cache:   16K Bytes, 32 Byte lines, 2-way or 4-way set associative" },
    { 0x40 , "No L2 Cache" },
    { 0x41 , "Unified L2 Cache:  128K Bytes, 32 Byte lines, 4-way set associative" },
    { 0x42 , "Unified L2 Cache:  256K Bytes, 32 Byte lines, 4-way set associative" },
    { 0x43 , "Unified L2 Cache:  512K Bytes, 32 Byte lines, 4-way set associative" },
    { 0x44 , "Unified L2 Cache:    1M Bytes, 32 Byte lines, 4-way set associative" },
    { 0x45 , "Unified L2 Cache:    2M Bytes, 32 Byte lines, 4-way set associative" },
    { 0x00 , "NULL" }
};


/*
 * cpuid calls the processor's internal CPUID instruction and returns the
 *       16-byte result. The type of CPUID to return is determined by the
 *       argument number. No range check is performed on number as all 
 *       tested processors return 0s on illegal CPUID input values.
 *       However a call to CPUID 0 gives the maximum reasonable value to 
 *       call CPUID with, so the caller may check this. 
 *
 */
cpuid_t cpuid(unsigned int number) 
{
    cpuid_t result; /* structure to return */
    
    /* Store number in %eax, call CPUID, save %eax, %ebx, %ecx, %edx in variables.
       As output constraint "m=" has been used as this keeps gcc's optimizer
       from overwriting %eax, %ebx, %ecx, %edx by accident */
    asm("movl %4, %%eax; cpuid; movl %%eax, %0; movl %%ebx, %1; movl %%ecx, %2; movl %%edx, %3;"
        : "m=" (result.eax),
          "m=" (result.ebx),
          "m=" (result.ecx),
          "m=" (result.edx)               /* output */
	: "r"  (number)                   /* input */
	: "eax", "ebx", "ecx", "edx"      /* no changed registers except output registers */
	);
    /*  printf("CPUID(0x%.8x) result: eax=0x%.8x  ebx=0x%.8x  ecx=0x%.8x  edx=0x%.8x\n", */
    /*	     number, result.eax, result.ebx, result.ecx, result.edx); */
    return result;
}


/*
 * identify_amd gives AMD specific details for a processor _known_ to be an AMD.
 * This fuction is not called on processors not identified as AMD.
 * AMD Processor identification is similar to Intel but uses 0x8000000x as
 * CPUID argument. Cache and TLB identification is quite a bit different as on
 * Intel - seems more straightforward...
 */
int
identify_amd(void)
{
    cpuid_t cpuid_registers; /* results as returned from cpuid */
    unsigned int largest_function_value;
    unsigned int amd_cpu_family, amd_cpu_model, amd_cpu_stepping; /* brief cpu description */
    char processor_description_text[3*4*sizeof(unsigned long)];
    int i,j;

    /* AMD -> at least some 486 thingy */
    amd_cpu_family=4;

    /* First determine "Largest Extended Function Value" */
    cpuid_registers=cpuid(0x80000000); 
    largest_function_value=cpuid_registers.eax;

    if(largest_function_value >= 0x80000001) {
        cpuid_registers = cpuid(0x80000001);
	amd_cpu_family   = 0xf&(cpuid_registers.eax>>8);
	amd_cpu_model    = 0xf&(cpuid_registers.eax>>4);
	amd_cpu_stepping = 0xf&cpuid_registers.eax;
	printf("CPUID (AMD - 1): CPU is a %u86, Model %u, Stepping %u\n",
	       amd_cpu_family, amd_cpu_model, amd_cpu_stepping);

	printf("           AMD CPU features are:\n");
	i=0;
	while(cpuid_registers.edx > 0) {
	    if(cpuid_registers.edx&0x1) { /* LSB set? */
	        /* Find description */
	        j=0;
		while((amd_cpu_feature_map[j].bit!=i) && (amd_cpu_feature_map[j].bit!=255)) {
		    j++;
		}
		if(amd_cpu_feature_map[j].bit!=255) {
		    /* We found a description */
 		    printf("               %10s - %s\n",
			   amd_cpu_feature_map[j].name, 
			   amd_cpu_feature_map[j].expln );
		} else {
		    /* No description available */
		    printf("               Feature %.2d - No description available\n", i);
		}
	    }
	    i++;
	    cpuid_registers.edx = cpuid_registers.edx >> 1; /* Shift 1 Bit */
	}
	
    }

    /* Get textual AMD processor description */
    /* First part of description text */
    if(largest_function_value >= 0x80000002) {
        cpuid_registers = cpuid(0x80000002);
	strncpy(&processor_description_text[0],
		(char *) &cpuid_registers, 4*sizeof(unsigned long));
    }
    /* 2nd part of description text */
    if(largest_function_value >= 0x80000003) {
        cpuid_registers = cpuid(0x80000003);
	strncpy(&processor_description_text[4*sizeof(unsigned long)],
		(char *) &cpuid_registers, 4*sizeof(unsigned long));
    }
    /* 3rd part of description text */
    if(largest_function_value >= 0x80000004) {
        cpuid_registers = cpuid(0x80000004);
	strncpy(&processor_description_text[2*4*sizeof(unsigned long)],
		(char *) &cpuid_registers, 4*sizeof(unsigned long));
    }
    printf("CPUID(AMD - x) Textual processor description is \"%s\"\n",
	   processor_description_text);

    /* L1 Caches and TLB descriptions */
    if(largest_function_value >= 0x80000005) {
        cpuid_registers = cpuid(0x80000005);
	printf("CPUID(AMD - 5) : Data        TLB with %3d entries, %d-way set associative\n",
	       (cpuid_registers.ebx>>16)&0xff, 
	       (cpuid_registers.ebx>>24)&0xff);
	printf("CPUID(AMD - 5) : Instruction TLB with %3d entries, %d-way set associative\n",
	       cpuid_registers.ebx&0xff,
	       (cpuid_registers.ebx>>8)&0xff);
	printf("CPUID(AMD - 5) : L1 Data        Cache is %3dk (%d lines per tag, %d-byte lines, %d-way set associative\n",
	       (cpuid_registers.ecx>>24)&0xff,
	       (cpuid_registers.ecx>>8)&0xff,
	       cpuid_registers.ecx&0xff,
	       (cpuid_registers.ecx>>16)&0xff);
	printf("CPUID(AMD - 5) : L1 Instruction Cache is %3dk (%d lines per tag, %d-byte lines, %d-way set associative\n",
	       (cpuid_registers.edx>>24)&0xff,
	       (cpuid_registers.edx>>8)&0xff,
	       cpuid_registers.edx&0xff,
	       (cpuid_registers.edx>>16)&0xff);
    }

    /* L2 Cache description (K6-2 3D+ (and higher?)) - untested at this time! */
    if(largest_function_value >= 0x80000006) {
        cpuid_registers = cpuid(0x80000006);
	printf("CPUID(AMD - 6) : WARNING! This has not been tested!\n");
	printf("CPUID(AMD - 6) : L2 Unified     Cache is %3dk (%d lines per tag, %d-byte lines, %d-way set associative\n",
	       (cpuid_registers.ecx>>24)&0xff,
	       (cpuid_registers.ecx>>8)&0xff,
	       cpuid_registers.ecx&0xff,
	       (cpuid_registers.ecx>>16)&0xff);
    }

    return amd_cpu_family;
}


/* our program will detect the CPU type and return 3 on 386, 4 on 486, ... */
/* parts of this routine are copied from linux' head.S */
int 
main(void)
{
    unsigned long flags_before, flags_after;     /* temp to save flags */
    unsigned int max_cpuid_index, cpu_family, 
	         cpu_model, cpu_stepping;        /* brief cpu description */
    char manufacturerid[3*sizeof(unsigned int)]; /* CPUID returns 12 chars in 4 ints */
    cpuid_t cpuid_registers;                     /* results as returned from cpuid */
    int i,j;
    char cachesizes[MAX_CACHE_FEATURES_ITERATIONS*4*sizeof(unsigned int)];
    
    /* 
     * Check for 386 
     */
    
    /* Read Flags;; Save Flags; Flip AC in Flags;
       Write Flags;; Read Flags;; Save new Flags; Restore old Flags */
    asm("pushfl; popl %%eax; movl %%eax, %0; xorl $0x40000, %%eax; pushl %%eax; popfl; pushfl; popl %%eax; movl %%eax, %1; pushl %0; popfl "
	: "r=" (flags_before),
	"r=" (flags_after)              /* output */
	:                                 /* input */
	: "eax"                           /* changed registers */
	);

    
    flags_before &= 0x40000;
    flags_after  &= 0x40000;
    if(flags_before == flags_after) {
	/* We have found a 386 */
	printf("386 CPU found.\n");
	return 3;
    }
    
    /* 
     * Check for 486 w/o working CPUID 
     */
    
    /* Read Flags;; Save Flags; Flip AC in Flags;
       Write Flags;; Read Flags;; Save new Flags; Restore old Flags */
    asm("pushfl; popl %%eax; movl %%eax, %0; xorl $0x200000, %%eax; pushl %%eax; popfl; pushfl; popl %%eax; movl %%eax, %1; pushl %0; popfl "
	: "r=" (flags_before),
	"r=" (flags_after)              /* output */
	:                                 /* input */
	: "eax"                           /* changed registers */
	);
    
    flags_before &= 0x200000;
    flags_after  &= 0x200000;
    if(flags_before == flags_after) {
	/* We have found an old 486 */
	printf("486 CPU w/o working CPUID found.\n");
	return 4;
    }
    
    /* 
     * All processors > 486 and some newer 486 do have CPUID, so we will use that. 
     */
    
    /* First, detect maximum index for CPUID along with CPU manufacturer */
    cpu_family = 4; /* At least it is a 486 */
    cpu_model  = 0;
    
    /* With CPUID 0 we get the maximum reasonable argument to CPUID
       and the 12-char string identifying the manufacturer */
    cpuid_registers=cpuid(0);
    max_cpuid_index=cpuid_registers.eax;
    sprintf(manufacturerid, "%.4s%.4s%.4s", (char *) &cpuid_registers.ebx,
	    (char *) &cpuid_registers.edx,  (char *) &cpuid_registers.ecx);
    printf("CPUID (0): maximum CPUID index is %d, Manufacturer says \"%.12s\"\n", 
	   max_cpuid_index, manufacturerid);
    
    /* With CPUID 1 we get the exact chip revision (eax) and  (not nearly)
       all the features that distinguish this chip from an 80386 (edx) */
    if(max_cpuid_index >= 1) {
	cpuid_registers=cpuid(1);
	cpu_family   = 0xf&(cpuid_registers.eax>>8);
	cpu_model    = 0xf&(cpuid_registers.eax>>4);
	cpu_stepping = 0xf&cpuid_registers.eax;
	printf("CPUID (1): CPU is a %u86, Model %u, Stepping %u\n",
	       cpu_family, cpu_model, cpu_stepping);
	
	/* Search for a match in cpu_identifier_map */
	i=0;
	while((cpu_identifier_map[i].family!=0) &&
	      ((cpu_identifier_map[i].family != cpu_family) || 
	       (cpu_identifier_map[i].model  != cpu_model ) ||
	       strncasecmp(cpu_identifier_map[i].manufacturerid, manufacturerid, 12))) i++;
	if(cpu_identifier_map[i].family!=0) {
	    printf("           CPU seems to be a %s.\n", cpu_identifier_map[i].expln );
	} else {
	    printf("           No description available for this CPU family/model/vendor.");
	}
	
	printf("           CPU features are:\n");
	i=0;
	while(cpuid_registers.edx > 0) {
	    if(cpuid_registers.edx&0x1) { /* LSB set? */
		/* Find description */
		j=0;
		while((intel_cpu_feature_map[j].bit!=i) && (intel_cpu_feature_map[j].bit!=255)) {
		    j++;
		}
		if(intel_cpu_feature_map[j].bit!=255) {
		    /* We found a description */
		    printf("               %10s - %s\n",
			   intel_cpu_feature_map[j].name, intel_cpu_feature_map[j].expln);
		} else {
		    /* No description available */
		    printf("               Feature %.2d - No description available\n", i);
		}
	    }
	    i++;
	    cpuid_registers.edx = cpuid_registers.edx >> 1; /* Shift 1 Bit */
	  }

    }

    /* 
     * Special treatment on AMD processors as those K6 try to behave like 
     * a Intel Pentium but might have somewhat more to reveal 
     */
    if((!strncasecmp("AuthenticAMD", manufacturerid, 12)) && (cpu_model > 0)) {
	return identify_amd();
    }
    
    /* CPUID 2 returns chars describing the cache features. More than 16 cache
       Features are possible by requiring to call CPUID 2 more than once! 
       As one has to look up those chars in a table, this feature seems to be
       somewhat ugly but allows for future extensions (that are not found in the
       lookup table anyways...) */
    if(max_cpuid_index >= 2) {
	
	/* First, clear result array */
	for(i=0; i < 4*4*MAX_CACHE_FEATURES_ITERATIONS; i++) cachesizes[i]=0;
	
	/* Read CPUID 2 for the first time; al (in eax) delivers how often to read */
	cpuid_registers=cpuid(2);
	memcpy(&cachesizes[ 0], &cpuid_registers, sizeof(cpuid_registers));
	cachesizes[0]--; /* Counter how often to call CPUID 2. Already called once. */
	i=1;
	while(cachesizes[0] && ( i < MAX_CACHE_FEATURES_ITERATIONS ) ) {
	    /* Untested, as PentrumII only needs one call to CPUID 2 */
	    printf("WARNING! This Program was not tested on CPUs > PentiumII!\n");
	    cpuid_registers=cpuid(2);
	    memcpy(&cachesizes[i*16 +  0], &cpuid_registers, sizeof(cpuid_registers));
	    i++;
	    cachesizes[0]--; 
	}
	printf("CPUID (2): Cache and TLB Sizes are:\n");
	for(i=1; i < 4*4*MAX_CACHE_FEATURES_ITERATIONS; i++) {
	    /* ignore cachesizes[0] - is the counter */
	    /* printf("0x%.2x ", cachesizes[i]); */
	    if(cachesizes[i]) { /* 0x00 is ignored - NULL value */
		j=0;
		while((intel_cache_feature_map[j].label!=0x00) &&
		      (intel_cache_feature_map[j].label!=cachesizes[i])) j++;
		if(intel_cache_feature_map[j].label==cachesizes[i]) {
		    printf("           %s\n", intel_cache_feature_map[j].expln);
		} else {
		    printf("           Cache Feature 0x%.2x - No Interpretation available.\n", cachesizes[i]);
		}
	    }
	}
    } else {
	printf("CPUID (2): No Cache or TLB information available.\n");
    }
    
    /* Quit with an exit value describing x in a processor nomenclatura of 80x86 */
    return cpu_family;
}




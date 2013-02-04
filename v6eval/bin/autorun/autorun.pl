#!/usr/bin/perl
#
# Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011
# Yokogawa Electric Corporation, YDC Corporation,
# IPA (Information-technology Promotion Agency, Japan).
# All rights reserved.
# 
# Redistribution and use of this software in source and binary forms, with 
# or without modification, are permitted provided that the following 
# conditions and disclaimer are agreed and accepted by the user:
# 
# 1. Redistributions of source code must retain the above copyright 
# notice, this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright 
# notice, this list of conditions and the following disclaimer in the 
# documentation and/or other materials provided with the distribution.
# 
# 3. Neither the names of the copyrighters, the name of the project which 
# is related to this software (hereinafter referred to as "project") nor 
# the names of the contributors may be used to endorse or promote products 
# derived from this software without specific prior written permission.
# 
# 4. No merchantable use may be permitted without prior written 
# notification to the copyrighters. However, using this software for the 
# purpose of testing or evaluating any products including merchantable 
# products may be permitted without any notification to the copyrighters.
# 
# 
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHTERS, THE PROJECT AND 
# CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING 
# BUT NOT LIMITED THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
# FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.  IN NO EVENT SHALL THE 
# COPYRIGHTERS, THE PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
# CONTRACT,STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
# THE POSSIBILITY OF SUCH DAMAGE.
#
# $TAHI: v6eval/bin/autorun/autorun.pl,v 1.35 2010/03/18 01:44:42 doo Exp $

use Digest::MD5;

use V6evalCommon;
use File::Basename;
use Config;
use Pod::Html;
use YAML;
use YAML::Dumper;


sub printHeader();
sub printTitle($);
sub printINDEX();
sub printResult();
sub printFooter();
sub mkComment(%);
sub printTableData($;$$);
sub getStatus($);
sub execScript($$$$$);
sub checkPacket($$);
sub seq2html($$);
sub insertMD5($);
sub getDateString();
sub parseArgs();
sub printReport();
sub parseIndex();
sub makeFrame();
sub printSummary();
sub dump_yaml();

$|=1;
pathinit();

my @ARGV_INDEXGEN = @ARGV;
{
	# refill ' for -title option
	# @ARGV_INDEXGEN is used as arguments for indexgen
	foreach my $elm (@ARGV_INDEXGEN) {
		$elm =~ s/^-title=(.+)$/-title='$1'/g;
	}
}

parseArgs();

if (-e "index.html" && !$opt_f) {
	print "Error : Already exist index.html.\n";
	print "        please run after delete index.html or run with -f option.\n";
	exit 2;
}

if($opt_tiny != 0) {
	if(-e "summary.html" && !$opt_f) {
		print "Error : Already exist summary.html.\n";
		print "        please run after delete summary.html or run with -f option.\n";
		exit 2;
	}

	if(-e "report.html" && !$opt_f) {
		print "Error : Already exist report.html.\n";
		print "        please run after delete report.html or run with -f option.\n";
		exit 2;
	}
}

$SIG{INT} = sub {exit 2;};

# get signal name & number
my %sig_num;
my @sig_name;
unless($Config{sig_name} && $Config{sig_num}) {
	die "No sigs?";
} else {
	my @names = split ' ', $Config{sig_name};
	my @num = split ' ', $Config{sig_num};
	@sig_num{@names} = @num;
	foreach (@names) {
		$sig_name[$sig_num{$_}] ||= $_;
	}
}

# get tool & test version
my $dummy;
($dummy,$VERTOOLS) = ('$Name: REL_3_3_2 $' =~ /\$(Name): (.*) \$/ );
$VERTOOLS='undefined' if(!$VERTOOLS);

$StartTime=getDateString();
@INDEX=@ARGV;

parseIndex();

my $num;
for($num=1;$status[$num]{type};$num++) {
	local (*current)=$status[$num];
	next if($current{type} ne "DO");
	my $rc;
	if($dochk){
		print "===== Check $current{packet} =====\n";
		$rc=checkPacket($current{packet},$current{opt});
		$statusStr=getStatus($rc);
		$statusStr =~ s/<.*?>//g;
		print "  -> $statusStr\n";
	}
	last if(($rc>>8) == 64 || $rc == $sig_num{INT});

	if($dogen){
		print "== Generate HTML Document from $current{script} ==\n";
		$rc=seq2html($current{script}, $current{doc});
		$current{status}=0;
		$current{type}="DOTEST";
	}
	last if(($rc>>8) == 64 || $rc == $sig_num{INT});

	if($dotest){
		dump_yaml();

		print "========== TEST $current{number} ==========\n";
		$rc=execScript( $current{script},
				$current{packet},
				$current{opt},
				$current{log},
				$current{desc});
		$current{status}=$rc;
		$current{type}="DOTEST";
		$statusStr=getStatus($rc);
		$statusStr =~ s/<.*?>//g;
		print "  -> $statusStr\n";
	}
	last if(($rc>>8) == 64 || $rc == $sig_num{INT});
}

$EndTime=getDateString();
0;

END {
	if($mkindex) {
		dump_yaml();
	}

	if($dogen){
		unlink("pod2html-dircache");
		unlink("pod2html-itemcache");
	}
}

sub parseIndex(){
	my $testnum=1;
	my $num;
	for($num=1;<>;$num++) {
		chomp;
		$status[$num]{type}="";
		local (*current)=$status[$num];
		#print "$num($testnum) -- $_\n";
		$VERTESTS = $2 if( /\$(Name): (.*) \$/ );
		if(/^\s*$/ || /^#/) {       # remove comment
			$current{type}="COMMENT";
			next;
		}

		if(/^&print:(.*)/){ # print comment
			$current{type}="PRINT";
			$current{strings}="$1";
			next;
		}

		if(/^&pause:(.*)/){ # make pause
			$current{type}="PAUSE";
			$current{strings}="$1";
			next;
		}

		if($StartNum > $testnum || $testnum > $EndNum || /^&\#/ ) {
			s/^&\#//;
			$current{type}="SKIP";
			$current{status}= 73 << 8;
			$skip ++;
		}
		else {
			$current{type}="DO";
			$current{status}= 72 << 8;
		}

		my ($script,$packet,$opt,$doc,$desc,$links) = split ':';
		($script) = ( $script =~ /^\s*(\S*)\s*$/  );
		($packet) = ( $packet =~ /^\s*(\S*)\s*$/  );
		($doc)    = ( $doc    =~ /^\s*(\S*)\s*$/  );
		($desc)   = ( $desc   =~ /^\s*(.*\S)\s*$/ );
		($links)  = ( $links  =~ /^\s*(\S*)\s*$/  );
		my ($SeqName,$SeqDir,$SeqSuffix) = fileparse($script,'.seq');
		$desc||=$SeqName;
		$current{number}=$testnum;
		$current{script}=$script;
		$current{packet}=$packet;
		$current{opt}=$opt;
		$current{log}="$testnum.html";
		$current{desc}=$desc;
		$current{SeqName}=$SeqName;
		$current{SeqDir}=$SeqDir;
		$current{doc}=$doc;
		$current{links}=$links ? $links : 1;
		$testnum++;
		$total ++;
	}
	$VERTESTS = "undefined" if(!$VERTESTS);
}


#
# Generate Report Index
#
sub printReport() {
	if($opt_tiny != 0) {
		open LOG ,">report.html";    
	} else {
		open LOG ,">index.html";    
	}

	printHeader();

	if($opt_tiny == 0) {
		printTitle(LOG);
		#sub printINDEX();
		print LOG "<HR>\n";
	}

	printResult();
	printFooter();
	close LOG;

	insertMD5($opt_tiny ? "report.html" : "index.html");

	return;
}

sub printHeader() {
if($opt_profile) {
    print LOG <<ECHO;
<HTML>
<HEAD>
<TITLE>IPv6 Test Specification</TITLE>
<META NAME="GENERATOR" CONTENT="TAHI IPv6 Conformance Test Suite">
</HEAD>
<BODY BGCOLOR="#FFFFFF">
ECHO
} else {
    print LOG <<ECHO;
<HTML>
<HEAD>
<TITLE>$Title</TITLE>
<META NAME="GENERATOR" CONTENT="TAHI IPv6 Conformance Test Suite">
</HEAD>
<BODY BGCOLOR="#FFFFFF">
ECHO
}
}

sub printTitle($){
	my($fd) = @_;

	if($opt_tiny != 0) {
	if($opt_profile) {
	print $fd <<ECHO;
<CENTER>
<H1>$Title</H1>
</CENTER>
ECHO
	} else {	# if($opt_profile)
	print $fd <<ECHO;
<CENTER>
<H1>$Title</H1>
</CENTER>
<HR>
<TABLE>
<TR><TD>Tool Version</TD><TD>:</TD><TD>$VERTOOLS</TD></TR>
<TR><TD>Test Program Version</TD><TD>:</TD><TD>$VERTESTS</TD></TR>
</TABLE>
<HR>
<TABLE>
<TR><TD>Start</TD><TD>:</TD><TD>$StartTime</TD></TR>
<TR><TD>End</TD><TD>:</TD><TD>$EndTime</TD></TR>
</TABLE>
ECHO
	}		# if($opt_profile)
	} else {
	if($opt_profile) {
	print $fd <<ECHO;
<CENTER>
<H1>$Title</H1>
ECHO
	} else {	# if($opt_profile)
	print $fd <<ECHO;
<CENTER>
<H1>$Title</H1>
<HR>
<TABLE BORDER=0>
<TR><TD>Tool Version :</TD><TD>$VERTOOLS</TD></TR>
<TR><TD>Test Program Version :</TD><TD>$VERTESTS</TD></TR>
</TABLE>
<HR>
<TABLE BORDER=0>
<TR><TD>Start:</TD><TD>$StartTime</TD></TR>
<TR><TD>End :</TD><TD>$EndTime</TD></TR>
</TABLE>
<HR>
ECHO
	}		# if($opt_profile)
	}
}

sub printINDEX(){
	#
	# Output Test Report Index
	#
	print LOG "INDEX: ";
	foreach(@INDEX) {
		print LOG "<A HREF=\"$_\">$_</A> ";
	}
}

sub printResult(){
	#
	# print result table
	#

	my @color = ('#EEEEEE', '#DDDDDD');
	my $flag = 0;
 
	if($opt_tiny != 0) {
		print LOG "<CENTER>";
	}

	if($opt_tiny) {
		if($opt_profile) {
			print LOG <<ECHO;
<TABLE WIDTH="100%" BORDER=1>
<TR BGCOLOR=\"#CCCCCC\">
<TH>No.</TH><TH>Title</TH>
ECHO
		} else {
			print LOG <<ECHO;
<TABLE BORDER=1>
<TR BGCOLOR=\"#CCCCCC\">
<TH>No.</TH><TH>Title</TH>
<TH>Result</TH><TH>Log</TH><TH>Script</TH><TH>Packet</TH><TH>Dump<BR>(bin)</TH>
ECHO
		}
	} else {
		if($opt_profile) {
			print LOG <<ECHO;
<TABLE WIDTH="100%" BORDER=1>
<TR>
<TH>No.</TH><TH>Title</TH>
ECHO
		} else {
			print LOG <<ECHO;
<TABLE BORDER=1>
<TR>
<TH>No.</TH><TH>Title</TH>
<TH>Result</TH><TH>Log</TH><TH>Script</TH><TH>Packet</TH><TH>Dump<BR>(bin)</TH>
ECHO
		}
	}

	unless($opt_profile) {
		print LOG "<TH>Dump<BR>(txt)</TH>" if $opt_dump2txt;
	}

	print LOG "</TR>\n";
	my $num;
	for($num=1;$status[$num]{type};$num++) {
		local (*current)=$status[$num];
		if ($current{type} eq "DOTEST" || $current{type} eq "DO" ) {
			if($opt_tiny != 0) {
				print LOG "<TR BGCOLOR=\"@color[$flag]\">\n";

				if($flag != 0) {
					$flag = 0;
				} else {
					$flag = 1;
				}
			} else {
				print LOG "<TR>\n";
			}

			$htmldoc = $current{doc} ? $current{doc} :
					"$current{SeqDir}$current{SeqName}.html";
			$htmldoc = "" if(! -r $htmldoc);
			printTableData("$current{number}");
			printTableData("$current{desc}",$htmldoc);

			unless($opt_profile) {
			$statusStr=getStatus($current{status});
			if(($current{status} == 3 << 8 || $current{status} == 32 << 8) && !$opt_F){
				mkComment(\%current);
				$statusStr.= ", <A HREF=\"$current{number}.comment.html\">Why</A>";
			}
			if($opt_nolcna) {
				#printTableData("-","","ALIGN=\"CENTER\"");
				printTableData("<FONT COLOR=\"#00FF00\">Not support LCNA<FONT>","","ALIGN=\"CENTER\"");
			} elsif($opt_onlylcna) {
				#printTableData("-","","ALIGN=\"CENTER\"");
				printTableData("<FONT COLOR=\"#00FF00\">Support only LCNA<FONT>","","ALIGN=\"CENTER\"");
			} else {
				printTableData("$statusStr","","ALIGN=\"CENTER\"");
			}
			if($mklink) {
			    printTableData("X","$current{log}","ALIGN=\"CENTER\"");
			} else {
			    printTableData("-","","ALIGN=\"CENTER\"");
			}
			printTableData("X","$current{script}","ALIGN=\"CENTER\"");
			if($current{packet} =~ /^\//){
				printTableData("-","","ALIGN=\"CENTER\"");
			}else {
				printTableData("X","$current{packet}","ALIGN=\"CENTER\"");
			}
			undef $dumplink;
			undef $dumplinktxt;
			if($mklink) {
			    for($i=0;$i<$current{links};$i++) {
				$dumplink    .= "<A HREF=\"$current{log}.Link$i.dump\">Link$i</A> ";
				$dumplinktxt .= "<A HREF=\"$current{log}.Link$i.dump.txt\">Link$i</A> ";
			    }
			    printTableData("$dumplink","","ALIGN=\"CENTER\"");
			    printTableData("$dumplinktxt","","ALIGN=\"CENTER\"") if $opt_dump2txt;
			} else {
			    printTableData("-","","ALIGN=\"CENTER\"");
			    printTableData("-","","ALIGN=\"CENTER\"") if $opt_dump2txt;
			}
			}	# unless($opt_profile)

			print LOG "</TR>\n";
		}
		elsif ($current{type} eq "SKIP" ) {
			if($opt_tiny != 0) {
				print LOG "<TR BGCOLOR=\"@color[$flag]\">\n";

				if($flag != 0) {
					$flag = 0;
				} else {
					$flag = 1;
				}
			} else {
				print LOG "<TR>\n";
			}

			$htmldoc = $current{doc} ? $current{doc} :
					"$current{SeqDir}$current{SeqName}.html";
			$htmldoc = "" if(! -r $htmldoc);
			printTableData("$current{number}");
			printTableData("$current{desc}",$htmldoc);

			unless($opt_profile) {
			$statusStr=getStatus($current{status});
			printTableData("$statusStr","","ALIGN=\"CENTER\"");
			printTableData("-","","ALIGN=\"CENTER\"");
			printTableData("X","$current{script}","ALIGN=\"CENTER\"");
			if($current{packet} =~ /^\//){
				printTableData("-","","ALIGN=\"CENTER\"");
			}else {
				printTableData("X","$current{packet}","ALIGN=\"CENTER\"");
			}
			printTableData("-","","ALIGN=\"CENTER\"");
			printTableData("-","","ALIGN=\"CENTER\"") if $opt_dump2txt;
			}	# unless($opt_profile)

			print LOG "</TR>\n";
		}
		elsif ($status[$num]{type} eq "PRINT") {
			print LOG "<TR>";
			printTableData("<BR>");
			printTableData("$current{strings}");
			unless($opt_profile) {
			printTableData("<BR>");
			printTableData("<BR>");
			printTableData("<BR>");
			printTableData("<BR>");
			printTableData("<BR>");
			printTableData("<BR>") if $opt_dump2txt;
			}
			print LOG "</TR>\n";
		}
		elsif ($status[$num]{type} eq "PAUSE") {
		        print STDERR ($current{strings}=~/^\s*$/ ?
				      "Hit [enter] key " : $current{strings});
			<STDIN>;
		}
	}

	print LOG "</TABLE>\n";
	print LOG "</CENTER>\n";
}

sub printFooter() {
	if(($opt_tiny != 0) || ($opt_nofooter)) {
		print LOG "</BODY>\n";
		print LOG "</HTML>\n";
	} else {
		if(-e ".footer") {
			print LOG "<HR>\n";
			open FOOTER, ".footer" or die;
			while(<FOOTER>){
			    print LOG $_;
			}
			close FOOTER;
		}

		if($opt_profile) {
		print LOG <<ECHO;
<HR>
</BODY>
</HTML>
ECHO
		} else {
		print LOG <<ECHO;
<HR>
This Report was generated by
<A HREF=\"http://www.tahi.org/\">TAHI</A> IPv6 Conformance Test Suite
</BODY>
</HTML>
ECHO
		}
	}
}

sub mkComment(%) { local (
	*status
    ) = @_;

	my $htmldoc = $status{doc} ? $status{doc}: "$status{SeqDir}$status{SeqName}.html";
	$htmldoc = "" if(! -r $htmldoc);

	my $pktlink= "<A HREF=\"$status{packet}\">X</A>";
	$pktlink = "-" if($status{packet} =~ /^\//);
	$pktlink = "<A HREF=\"packet.def\">X</A>" if($status{packet} eq "");

	undef $dumplink;
	undef $dumplinktxt;
	for($i=0;$i<$status{links};$i++) {
		$dumplink    .= "<A HREF=\"$status{log}.Link$i.dump\">Link$i</A> ";
		$dumplinktxt .= "<A HREF=\"$status{log}.Link$i.dump.txt\">Link$i</A> ";
	}

	$statusStr=getStatus($status{status});

	open COMMENT, ">$status{number}.comment.html" or die;
	print COMMENT <<ECHO;
<HTML>
<HEAD>
<TITLE>Comments about the test</TITLE>
</HEAD>
<BODY BGCOLOR=\"#FFFFFF\">
<CENTER><H1>Comments about the test</H1></CENTER>

<TABLE BORDER=1>
<TR>
  <TH>No.</TH>
  <TH>Title</TH>
  <TH>Result</TH>
  <TH>Log</TH>
  <TH>Script</TH>
  <TH>Packet</TH>
  <TH>Dump<BR>(bin)</TH>
  <TH>Dump<BR>(txt)</TH>
</TR>

<TR>
  <TD>$status{number}</TD>
  <TD><A HREF=\"$htmldoc\">$status{desc}</A></TD>
  <TD ALIGN=\"CENTER\">$statusStr</TD>
  <TD ALIGN=\"CENTER\"><A HREF=\"$status{log}\">X</A></TD>
  <TD ALIGN=\"CENTER\"><A HREF=\"$status{script}\">X</A></TD>
  <TD ALIGN=\"CENTER\">$pktlink</TD>
  <TD ALIGN=\"CENTER\">$dumplink</TD>
  <TD ALIGN=\"CENTER\">$dumplinktxt</TD>
</TR>
</TABLE>

<H2>Category</H2>

<H2>Commentaries</H2>
<P><FONT COLOR=\"#FF0000\"> Now analyzing.</FONT>

<H2>References</H2>

<H2>Notes</H2>

<H2>Implementors information</H2>

<HR>
<A HREF="mailto:contact\@tahi.org">contact\@tahi.org</A>
</BODY>
</HTML>
ECHO
	close COMMENT;
}

sub printTableData($;$$) { my (
	$desc,
	$link,
	$td_opt
    ) = @_;
	$td_opt = " $td_opt" if $td_opt;
	print LOG "<TD$td_opt>";
	print LOG "<A HREF=\"$link\">" if $link;
	print LOG "$desc";
	print LOG "</A>" if $link;
	print LOG "</TD>";
}

sub getStatus($) { my (
	$status
    ) = @_;
	$result{0}="PASS";
	if(!$dotest){
		$result{1}="error";
		if(($opt_nolcna) || ($opt_onlylcna)) {
			$result{73}="<FONT COLOR=\"#AAAAAA\">SKIP</FONT>";
		}
	} else {
		$result{1} ="-";
		$result{2} ="<FONT COLOR=\"#00FF00\">Not yet supported</FONT>";
		$result{3} ="<FONT COLOR=\"#00FF00\">WARN</FONT>";
		$result{4} ="<FONT COLOR=\"#00FF00\">Host Only<FONT>";
		$result{5} ="<FONT COLOR=\"#00FF00\">Router Only</FONT>";
		$result{6} ="<FONT COLOR=\"#00FF00\">Special Only</FONT>";
		$result{7} ="<FONT COLOR=\"#00FF00\">Except Host<FONT>";
		$result{8} ="<FONT COLOR=\"#00FF00\">Except Router</FONT>";
		$result{9} ="<FONT COLOR=\"#00FF00\">Except Special</FONT>";
		$result{10}="<FONT COLOR=\"#AAAAAA\">SKIP</FONT>";
		$result{11}="<FONT COLOR=\"#00FF00\">Type Mismatch</FONT>";
		$result{32}="<FONT COLOR=\"#FF0000\">FAIL</FONT>";
		$result{33}="<FONT COLOR=\"#FF0000\">Initialization Fail</FONT>";
		$result{34}="<FONT COLOR=\"#FF0000\">Cleanup Fail</FONT>";
		$result{64}="<FONT COLOR=\"#FF0000\">internal error</FONT>";
		$result{72}="TBD";
		$result{73}="<FONT COLOR=\"#AAAAAA\">SKIP</FONT>";
	}
	my $rc="internal error";
		   
	if ($status == 0){
		$rc=$result{0};
	}
	elsif ($status == 0xff00){
		$rc = "no test sequence";
	}
	elsif ($status  > 0x80){
		my $err = $status >> 8;
		$rc = $result{$err} ||
		      "<FONT COLOR=\"#FF0000\">FAIL ($err)</FONT>";
	}
	elsif ($status < 0x80) {
		$rc = "<FONT COLOR=\"#FF0000\">SIGNAL ($status)</FONT>";
	}
	$rc;
}

sub execScript($$$$$) { my (
	$script,
	$packet,
	$opt,
	$log,
	$desc  
    ) = @_;
	my $cmd= $script;
	$cmd.=" -pkt $packet" if $packet;
	$cmd.=" $opt" if $opt;
	$cmd.=" -log $log" if $log;
	$cmd.=" -ti \"$desc\"" if $desc;
	print "$cmd\n";
	$rc = system($cmd);
	$rc;
}

sub checkPacket($$) { my (
	$packet,
	$opt
    ) = @_;
	my $cmd=searchPath($V6EVALBIN, "pktlint") || "pktlint";
	$cmd.=" -pkt $packet" if $packet;
	$cmd.=" $opt" if $opt;
	$cmd.=" -log /dev/null";
	$rc = 0xffff & system($cmd);
	$rc;
}

sub seq2html($$) { my (
	$script,
	$doc
    ) = @_;
	my $rc;
	my ($SeqName,$SeqDir,$SeqSuffix) = fileparse($script,'.seq');
	my $outfile= $doc ? $doc : "${SeqDir}${SeqName}.html";
	if(-f $outfile && !$opt_f) {
		print "$outfile: Already exist\n";
		return 0;
	}
	$rc=pod2html(
		     "--infile=$script",
		     "--outfile=$outfile",
		     "--noindex");
	if($rc == 0 && !-r $outfile) {
		print "warning : manual part is not included in $script .\n";
	}
	$rc;
}



#--------------------------------------------------------------#
# void insertMD5(char **, char *);                             #
#--------------------------------------------------------------#
sub
insertMD5($) {
	my($log) = @_;

	local(*LOG);

	unless(open(LOG, $log)) {
		return;
	}

	my $ctx = Digest::MD5->new;

	$ctx->addfile(*LOG);

	my $digest = $ctx->hexdigest;

	close(LOG);

	unless(open(LOG, ">> $log")) {
		return;
	}

	print LOG "<!-- $digest -->\n";

	close(LOG);

	return;
}

sub getDateString() {
	my ($sec,$min,$hour,$day,$mon,$year) = localtime;
	my $datestr = sprintf '%04d/%02d/%02d %02d:%02d:%02d',
			$year+1900, $mon+1, $day, $hour, $min, $sec;
	$datestr;
}



#
# make frame
#
################################################################
sub makeFrame() {
	open(FRAME ,">index.html");

	print FRAME "<HTML>\n";
	print FRAME "<HEAD>\n";

	if($opt_profile) {
		print FRAME "<TITLE>IPv6 Test Specification</TITLE>\n";
	} else {
		print FRAME "<TITLE>$Title</TITLE>\n";
	}

	print FRAME "</HEAD>\n";
	print FRAME "<FRAMESET COLS=\"30%,70%\">\n";
	print FRAME "<FRAME SRC=\"summary.html\">\n";
	print FRAME "<FRAME SRC=\"report.html\">\n";
	print FRAME "</FRAMESET>\n";
	print FRAME "</HTML>\n";

	close(FRAME);

	insertMD5("index.html");

	return;
}



#
# print summary.html
#
################################################################
sub printSummary() {
	my $total = 0;
	my $tbd = 0;
	my $skip = 0;
	my $pass = 0;
	my $fail = 0;
	my $warn = 0;
	my $na = 0;

	open(SUMMARY ,">summary.html");

	print SUMMARY "<HTML>\n";
	print SUMMARY "<HEAD>\n";

	if($opt_profile) {
		print SUMMARY "<TITLE>IPv6 Test Specification</TITLE>\n";
	} else {
		print SUMMARY "<TITLE>$Title</TITLE>\n";
	}

	print SUMMARY "</HEAD>\n";
	print SUMMARY "<BODY BGCOLOR=\"#FFFFFF\">\n";

	printTitle(SUMMARY);

	print SUMMARY "<HR>\n";

	for(my $d = 1; $status[$d]{type}; $d ++) {
		if($status[$d]{type} eq "DO" ) {
			$tbd ++;
			$total ++;
			next;
		}

		if($status[$d]{type} eq "DOTEST") {
			my $s = $status[$d]{status};

			if($s == 0) {
				if(($opt_nolcna) || ($opt_onlylcna)) {
					$na ++;
				} else {
					$pass ++;
				}
			} elsif($s > 0x80) {
				my $err = $s >> 8;

				if($err == 3) {
					$warn ++;
				} elsif($err == 32) {
					$fail ++;
				} else {
					$na ++;
				}
			} else {
				$na ++;
			}

			$total ++;
			next;
		}

		if($status[$d]{type} eq "SKIP") {
			$skip ++;
			$total ++;
			next;
		}
	}

	print SUMMARY "Test Results\n";
	print SUMMARY "<BLOCKQUOTE>\n";
	print SUMMARY "<TABLE>\n";
	print SUMMARY "<TR><TD><B>TOTAL</B></TD><TD>:</TD><TD>$total</TD></TR>\n";
	print SUMMARY "<TR><TD><B>PASS</B></TD><TD>:</TD><TD>$pass</TD></TR>\n";
	print SUMMARY "<TR><TD><B><FONT COLOR=\"#FF0000\">FAIL</FONT></B></TD><TD>:</TD><TD>$fail</TD></TR>\n";
	print SUMMARY "<TR><TD><B><FONT COLOR=\"#00FF00\">WARN</FONT></B></TD><TD>:</TD><TD>$warn</TD></TR>\n";
	print SUMMARY "<TR><TD><B><FONT COLOR=\"#AAAAAA\">SKIP</FONT></B></TD><TD>:</TD><TD>$skip</TD></TR>\n";
	print SUMMARY "<TR><TD><B>N/A</B></TD><TD>:</TD><TD>$na</TD></TR>\n";

	if($tbd != 0) {
		print SUMMARY "<TR><TD><B>TBD</B></TD><TD>:</TD><TD>$tbd</TD></TR>\n";
	}

	print SUMMARY "</TABLE>\n";
	print SUMMARY "</BLOCKQUOTE>\n";

	print SUMMARY "<HR>\n";

	if(-e ".footer") {
		if(defined(open(FOOTER, ".footer"))) {
			while(<FOOTER>){
				my $str = $_;
				$str =~ s/<A /<A TARGET="_top" /;

				print SUMMARY $str;
			}

			close FOOTER;
		}
	}

	print SUMMARY "<HR>\n";

	print SUMMARY "This Report was generated by<BR>\n";
	print SUMMARY "<A HREF=\"http://www.tahi.org/\">TAHI</A> IPv6 Conformance Test Suite<BR>\n";

	print SUMMARY "</BODY>\n";
	print SUMMARY "</HTML>\n";

	close(SUMMARY);

	insertMD5("summary.html");

	return;
}

sub dump_yaml()
{
	my $data = {
		'VERTESTS'	=> $VERTESTS,
		'VERTOOLS'	=> $VERTOOLS,
		'StartTime'	=> $StartTime,
		'EndTime'	=> $EndTime,
		'results'	=> \@status,
	};

	my $yaml_file = '/tmp/indexgen.yaml';
	my $dumper = YAML::Dumper->new;
	local(*YAML);
	open(YAML, '>'. $yaml_file);
	print YAML $dumper->dump($data);
	close(YAML);

	my $cmd = searchPath($V6EVALBIN, "indexgen") || "indexgen";
	$cmd .= ' ' . join(' ', @ARGV_INDEXGEN) ;
	my $ret = system($cmd);

	unlink($yaml_file);

	return($ret);
}

########################################################################
#	parse Args
#
#	Getopt::Long module is used for argument parse
#-----------------------------------------------------------------------
sub parseArgs() {
	use Getopt::Long;
	$Getopt::Long::ignorecase=undef;
	my $optStat = GetOptions(
		"t|TestMode!",
		"g|GenerateMode!",
		"G|GenerateMode2!",
		"s|Start=s",
		"e|End=s",
		"f!",
		"title=s",
		"dump2txt!",
		"F!",
		"nolcna",
		"onlylcna",
		"tiny",
		"profile",
		"nofooter",
	);
	                  $dotest=1; $dochk=0; $dogen=0; $mkindex=1; $mklink=1;
	if($opt_profile) {
		$dotest=0;
		$dogen=1;
	}

	if($opt_t)      { $dotest=0; $dochk=1;           $mkindex=0;}
	if($opt_g)      { $dotest=0;           $dogen=1;}
	if($opt_G)      { $dotest=0;           $dogen=1;             $mklink=0}
	if($opt_nolcna) { $dotest=0;           $dogen=1;             $mklink=0}
	if($opt_onlylcna) { $dotest=0;           $dogen=1;             $mklink=0}
	$StartNum= $opt_s ? $opt_s : 0;
	$EndNum= $opt_e ? $opt_e : 0xffff; # xxx
	if($StartNum > $EndNum) {
		print "Start Test Number greater than End Test Number\n";
		exit 1;
	}
	$Title= $opt_title ? $opt_title : "IPv6 Conformance Test";
}

########################################################################
__END__

=head1 NAME

B<autorun> - run all Conformance Test Script in INDEX.

=head1 SYNOPSIS

B<autorun> [C<-t>] [C<-g>|C<-G>] [C<-f>] [I<INDEX> ...]

=head1 DESCRIPTION

B<autorun> run Test Script by order in INDEX file.

The options are as follows:

=over 1

=item -t
Check Packet Definition syntax.

=item -g
Genarate HTML document from test sequence file.

=item -G
This option is same as -g option except
not generating links of log and tcpdump.

=item -f
Force to generate index.html even if the file already exists.

=item -F
Not generate "Why link".

=back 1

=head2 INDEX file format

This file specify how to run tests. This file has the following
format :

    Empty lines and lines starting with '#' are comments.

    Each line five field separated ':'. the fields are :

       1'st field specify test sequence script.

       2'nd field specify packet definition file.

       3'rd field specify option for test script.

       4'th field specify HTML document if you use insted of auto
       generate document from test script specified 1st field.

       5'th field specify test name.

       6'th field specify # of used Links.

=head1 OUTPUT

B<autorun> output index.html. It is result of tests, you can see
the result report by Web Browser.

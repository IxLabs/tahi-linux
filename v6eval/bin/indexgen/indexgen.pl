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
# $TAHI: v6eval/bin/indexgen/indexgen.pl,v 1.4 2010/03/25 05:51:03 doo Exp $

use strict;
use warnings;

use File::Basename;
use Getopt::Long;
use Config;
use Digest::MD5;
use YAML;
use YAML::Dumper;

# prototypes
sub main();
sub parse_args();
sub parse_tests();
sub initialize($);
sub generate_index($);

sub print_index();
sub print_summary($);
sub print_report($);
sub print_result($);
sub print_header();
sub print_footer();
sub make_comment($);
sub print_title($);
sub print_td($;$$);
sub insert_md5($);
sub get_status($);
sub print_err($);


# constant variables
use constant {
	DEFAULT_YAML_NAME => '/tmp/indexgen.yaml',
};

# global variables
my $input_filename = DEFAULT_YAML_NAME;

# global variables read from arguments
my $basename = undef;
my $opt_e = 0;
my $opt_f = 0;
my $opt_F = 0;
my $opt_g = 0;
my $opt_G = 0;
my $opt_i = 0;
my $opt_s = 0;
my $opt_nolcna = 0;
my $opt_onlylcna = 0;
my $opt_tiny = 0;
my $opt_profile = 0;
my $opt_nofooter = 0;
my $opt_dump2txt = 0;
my $opt_title = 0;
my $start_num = 0;
my $end_num = 0;

# global variables read from intermediate file
my $title;
my $start_time = undef; # passed from autorun
my $end_time = undef; # passed from autorun
my $VERTOOLS = undef; # passed from autorun
my $VERTESTS = undef; # passed from autorun

my @status;

my $dotest=1;
my $dochk=0;
my $dogen=0;
my $mkindex=1;
my $mklink=1;
my $dumplink;
my $dumplinktxt;

# body
main();
exit(0);
# NOTREACHED


sub main()
{
	$| = 1;
	$basename = basename($0, '.pl');

	parse_args();

	my $tests = parse_tests();

	initialize($tests);

	generate_index($tests);

	return (0);
}

sub parse_args()
{
	$Getopt::Long::ignorecase=undef;
	my $optStat = GetOptions(
		"g|GenerateMode!"	=> \$opt_g,
		"G|GenerateMode2!"	=> \$opt_G,
		"s|Start=s"	=> \$opt_s,
		"e|End=s"	=> \$opt_e,
		"f!"	=> \$opt_f,
		"F!"	=> \$opt_F,
		"i=s"	=> \$opt_i,
		"title=s"	=> \$opt_title,
		"dump2txt!"	=> \$opt_dump2txt,
		"nolcna"	=> \$opt_nolcna,
		"onlylcna"	=> \$opt_onlylcna,
		"tiny"	=> \$opt_tiny,
		"profile"	=> \$opt_profile,
		"nofooter"	=> \$opt_nofooter,
	);
	$dotest=1;
	$dochk=0;
	$dogen=0;
	$mkindex=1;
	$mklink=1;

	if ($opt_profile) {  $dotest=0; $dogen=1; }
	if ($opt_g) {        $dotest=0; $dogen=1; }
	if ($opt_G) {        $dotest=0; $dogen=1; $mklink=0; }
	if ($opt_nolcna) {   $dotest=0; $dogen=1; $mklink=0; }
	if ($opt_onlylcna) { $dotest=0; $dogen=1; $mklink=0; }

	unless (-f $input_filename) {
		print_err("Cannot find $input_filename\n");
		exit(2);
	}

	$start_num= $opt_s ? $opt_s : 0;
	$end_num= $opt_e ? $opt_e : 0xffff; # xxx
	if ($start_num > $end_num) {
		print_err("Start Test Number greater than End Test Number\n");
		exit(1);
	}
	$title= $opt_title ? $opt_title : "IPv6 Conformance Test";
}

sub parse_tests()
{
        my $data = YAML::LoadFile($input_filename);

	return($data);
}

sub initialize($)
{
	my ($data) = @_;

	$SIG{INT} = sub {exit(2);};

	# get signal name & number
	my %sig_num;
	my @sig_name;
	unless ($Config{sig_name} && $Config{sig_num}) {
		die "No sigs?";
	}
	else {
		my @names = split ' ', $Config{sig_name};
		my @num = split ' ', $Config{sig_num};
		@sig_num{@names} = @num;
		foreach (@names) {
			$sig_name[$sig_num{$_}] ||= $_;
		}
	}

	# 
	$VERTESTS = $data->{'VERTESTS'};
	unless (defined($VERTESTS)) {
		$VERTESTS = 'undefined';
	}

	my $dummy;
	($dummy, $VERTOOLS) = ('$Name: REL_3_3_2 $' =~ /\$(Name): (.*) \$/ );
	if (!$VERTOOLS) {
		$VERTOOLS = 'undefined';
	}

	$start_time = $data->{'StartTime'};
	unless (defined($start_time)) {
		$start_time = 'undefined';
	}

	$end_time = $data->{'EndTime'};
	unless (defined($end_time)) {
		$end_time = '';
	}

	return;
}

sub generate_index($)
{
	my ($tests) = @_;

	my $filename = undef;
	if ($opt_tiny != 0) {
		print_index();
		print_summary($tests);
		$filename = 'report.html';
	}
	else {
		$filename = 'index.html';
	}

	local(*LOG);
	open(LOG ,'>' . $filename);
	print_report($tests);
	close(LOG);

	insert_md5($filename);
}

sub print_index()
{
	my $str_title = undef;
	if ($opt_profile) {
		$str_title = 'IPv6 Test Specification';
	}
	else {
		$str_title = $title;
	}

	my $filename = 'index.html';
	open(FRAME, '>' . $filename);
	print FRAME <<ECHO;
<HTML>
<HEAD>
<TITLE>$str_title</TITLE>
</HEAD>
<FRAMESET COLS=\"30%,70%\">
<FRAME SRC=\"summary.html\">
<FRAME SRC=\"report.html\">
</FRAMESET>
</HTML>
ECHO
	close(FRAME);

	insert_md5($filename);

	return;
}

sub print_summary($)
{
	my ($tests) = @_;

	my $filename = "summary.html";
	my $total = 0;
	my $tbd = 0;
	my $skip = 0;
	my $pass = 0;
	my $fail = 0;
	my $warn = 0;
	my $na = 0;

	local(*SUMMARY);
	open(SUMMARY, '>' . $filename);

	print SUMMARY "<HTML>\n";
	print SUMMARY "<HEAD>\n";

	if ($opt_profile) {
		print SUMMARY "<TITLE>IPv6 Test Specification</TITLE>\n";
	}
	else {
		print SUMMARY "<TITLE>$title</TITLE>\n";
	}

	print SUMMARY "</HEAD>\n";
	print SUMMARY "<BODY BGCOLOR=\"#FFFFFF\">\n";

	print_title(*SUMMARY);

	print SUMMARY "<HR>\n";

	foreach my $result (@{$tests->{'results'}}) {
		if (!exists($result->{'type'})) {
			next;
		}

		if ($result->{'type'} eq "DO" ) {
			$tbd ++;
			$total ++;
			next;
		}

		if ($result->{'type'} eq "DOTEST") {
			my $s = $result->{'status'};

			if ($s == 0) {
				if (($opt_nolcna) || ($opt_onlylcna)) {
					$na ++;
				}
				else {
					$pass ++;
				}
			}
			elsif ($s > 0x80) {
				my $err = $s >> 8;

				if ($err == 3) {
					$warn ++;
				}
				elsif ($err == 32) {
					$fail ++;
				}
				else {
					$na ++;
				}
			}
			else {
				$na ++;
			}

			$total ++;
			next;
		}

		if ($result->{'type'} eq "SKIP") {
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

	if ($tbd != 0) {
		print SUMMARY "<TR><TD><B>TBD</B></TD><TD>:</TD><TD>$tbd</TD></TR>\n";
	}

	print SUMMARY "</TABLE>\n";
	print SUMMARY "</BLOCKQUOTE>\n";

	print SUMMARY "<HR>\n";

	if (-e ".footer") {
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

	insert_md5($filename);

	return;
}

sub print_report($)
{
	my ($tests) = @_;

	print_header();

	if ($opt_tiny == 0) {
		print_title(*LOG);
		print LOG "<HR>\n";
	}

	print_result($tests);
	print_footer();

	return;
}

sub print_header()
{
	my $str_title = undef;
	if ($opt_profile) {
		$str_title = 'IPv6 Test Specification';
	}
	else {
		$str_title = $title;
	}

	print LOG <<ECHO;
<HTML>
<HEAD>
<TITLE>$str_title</TITLE>
<META NAME="GENERATOR" CONTENT="TAHI IPv6 Conformance Test Suite">
</HEAD>
<BODY BGCOLOR="#FFFFFF">
ECHO
	return;
}

sub print_result($)
{
	my ($tests) = @_;
	#
	# print result table
	#
	if ($opt_tiny != 0) {
		print LOG "<CENTER>";
	}

	if ($opt_tiny) {
		if ($opt_profile) {
			print LOG <<ECHO;
<TABLE WIDTH="100%" BORDER=1>
<TR BGCOLOR=\"#CCCCCC\">
<TH>No.</TH><TH>Title</TH>
ECHO
		}
		else {
			print LOG <<ECHO;
<TABLE BORDER=1>
<TR BGCOLOR=\"#CCCCCC\">
<TH>No.</TH><TH>Title</TH>
<TH>Result</TH><TH>Log</TH><TH>Script</TH><TH>Packet</TH><TH>Dump<BR>(bin)</TH>
ECHO
		}
	}
	elsif ($opt_profile) {
		print LOG <<ECHO;
<TABLE WIDTH="100%" BORDER=1>
<TR>
<TH>No.</TH><TH>Title</TH>
ECHO
	}
	else {
		print LOG <<ECHO;
<TABLE BORDER=1>
<TR>
<TH>No.</TH><TH>Title</TH>
<TH>Result</TH><TH>Log</TH><TH>Script</TH><TH>Packet</TH><TH>Dump<BR>(bin)</TH>
ECHO
	}

	unless ($opt_profile) {
		print LOG "<TH>Dump<BR>(txt)</TH>" if $opt_dump2txt;
	}

	print LOG "</TR>\n";

	my @color = ('#EEEEEE', '#DDDDDD');
	my $flag = 0;
	my $htmldoc = '';
	my $statusStr = '';

	my $i = 0;
	foreach my $result (@{$tests->{'results'}}) {
		if (!exists($result->{'type'})) {
			next;
		}

		if ($result->{'type'} eq "DOTEST" || $result->{'type'} eq "DO" ) {
			if ($opt_tiny != 0) {
				print LOG "<TR BGCOLOR=\"$color[$flag]\">\n";

				if ($flag != 0) {
					$flag = 0;
				}
				else {
					$flag = 1;
				}
			}
			else {
				print LOG "<TR>\n";
			}

			$htmldoc = $result->{'doc'} ? $result->{'doc'} :
					"$result->{'SeqDir'}$result->{'SeqName'}.html";
			$htmldoc = "" if(! -r $htmldoc);
			print_td("$result->{'number'}");
			print_td("$result->{'desc'}",$htmldoc);

			unless($opt_profile) {
				$statusStr=get_status($result->{'status'});
				if (($result->{'status'} == 3 << 8 || $result->{'status'} == 32 << 8) && !$opt_F){
					make_comment($result);
					$statusStr.= ", <A HREF=\"$result->{'number'}.comment.html\">Why</A>";
				}

				if ($opt_nolcna) {
					#print_td("-","","ALIGN=\"CENTER\"");
					print_td("<FONT COLOR=\"#00FF00\">Not support LCNA<FONT>","","ALIGN=\"CENTER\"");
				}
				elsif ($opt_onlylcna) {
					#print_td("-","","ALIGN=\"CENTER\"");
					print_td("<FONT COLOR=\"#00FF00\">Support only LCNA<FONT>","","ALIGN=\"CENTER\"");
				}
				else {
					print_td("$statusStr","","ALIGN=\"CENTER\"");
				}

				if ($mklink) {
					print_td("X","$result->{'log'}","ALIGN=\"CENTER\"");
				}
				else {
					print_td("-","","ALIGN=\"CENTER\"");
				}

				print_td("X","$result->{'script'}","ALIGN=\"CENTER\"");

				if ($result->{'packet'} =~ /^\//){
					print_td("-","","ALIGN=\"CENTER\"");
				}
				else {
					print_td("X","$result->{'packet'}","ALIGN=\"CENTER\"");
				}

				undef $dumplink;
				undef $dumplinktxt;

				if ($mklink) {
					for (my $i=0;$i<$result->{'links'};$i++) {
						$dumplink    .= "<A HREF=\"$result->{'number'}_html_Link${i}.pcap\">Link$i</A> ";
						$dumplinktxt .= "<A HREF=\"$result->{'number'}_html_Link${i}_pcap.txt\">Link$i</A> ";
					}
					print_td("$dumplink","","ALIGN=\"CENTER\"");
					print_td("$dumplinktxt","","ALIGN=\"CENTER\"") if $opt_dump2txt;
				}
				else {
					print_td("-","","ALIGN=\"CENTER\"");
					print_td("-","","ALIGN=\"CENTER\"") if $opt_dump2txt;
				}
			}	# unless($opt_profile)

			print LOG "</TR>\n";
		}
		elsif ($result->{'type'} eq "SKIP" ) {
			if ($opt_tiny != 0) {
				print LOG "<TR BGCOLOR=\"$color[$flag]\">\n";

				if ($flag != 0) {
					$flag = 0;
				}
				else {
					$flag = 1;
				}
			}
			else {
				print LOG "<TR>\n";
			}

			$htmldoc = $result->{'doc'} ? $result->{'doc'} :
					"$result->{'SeqDir'}$result->{'SeqName'}.html";
			$htmldoc = "" if (! -r $htmldoc);
			print_td("$result->{'number'}");
			print_td("$result->{'desc'}",$htmldoc);

			unless($opt_profile) {
				$statusStr=get_status($result->{'status'});
				print_td("$statusStr","","ALIGN=\"CENTER\"");
				print_td("-","","ALIGN=\"CENTER\"");
				print_td("X","$result->{'script'}","ALIGN=\"CENTER\"");
				if ($result->{'packet'} =~ /^\//) {
					print_td("-","","ALIGN=\"CENTER\"");
				}
				else {
					print_td("X","$result->{'packet'}","ALIGN=\"CENTER\"");
				}
				print_td("-","","ALIGN=\"CENTER\"");
				print_td("-","","ALIGN=\"CENTER\"") if $opt_dump2txt;
			}	# unless($opt_profile)

			print LOG "</TR>\n";
		}
		elsif ($result->{'type'} eq "PRINT") {
			print LOG "<TR>";
			print_td("<BR>");
			print_td("$result->{'strings'}");
			unless ($opt_profile) {
				print_td("<BR>");
				print_td("<BR>");
				print_td("<BR>");
				print_td("<BR>");
				print_td("<BR>");
				print_td("<BR>") if $opt_dump2txt;
			}
			print LOG "</TR>\n";
		}
		elsif ($result->{'type'} eq "PAUSE") {
		        print STDERR ($result->{'strings'}=~/^\s*$/ ?
				      "Hit [enter] key " : $result->{'strings'});
			<STDIN>;
		}
	}

	print LOG "</TABLE>\n";
	print LOG "</CENTER>\n";
}

sub print_footer()
{
	if (($opt_tiny != 0) || ($opt_nofooter)) {
		print LOG "</BODY>\n";
		print LOG "</HTML>\n";
		return;
	}

	if (-e ".footer") {
		print LOG "<HR>\n";
		open FOOTER, ".footer" or die;
		while(<FOOTER>){
			print LOG $_;
		}
		close FOOTER;
	}

	if ($opt_profile) {
		print LOG <<ECHO;
<HR>
</BODY>
</HTML>
ECHO
	}
	else {
		print LOG <<ECHO;
<HR>
This Report was generated by
<A HREF=\"http://www.tahi.org/\">TAHI</A> IPv6 Conformance Test Suite
</BODY>
</HTML>
ECHO
	}
}

sub make_comment($)
{
	 my ($result) = @_;

	my $htmldoc = $result->{'doc'} ? $result->{'doc'}: "$result->{'SeqDir'}$result->{'SeqName'}.html";
	$htmldoc = "" if(! -r $htmldoc);

	undef($dumplink);
	undef($dumplinktxt);
	for (my $i=0; $i < $result->{'links'}; $i++) {
		$dumplink    .= "<A HREF=\"$result->{'log'}.Link$i.dump\">Link$i</A> ";
		$dumplinktxt .= "<A HREF=\"$result->{'log'}.Link$i.dump.txt\">Link$i</A> ";
	}

	 my $statusStr = get_status($result->{'status'});

	open COMMENT, ">$result->{'number'}.comment.html" or die;
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
  <TH>Dump</TH>
</TR>

<TR>
  <TD>$result->{'number'}</TD>
  <TD><A HREF=\"$htmldoc\">$result->{'desc'}</A></TD>
  <TD ALIGN=\"CENTER\">$statusStr</TD>
  <TD ALIGN=\"CENTER\"><A HREF=\"$result->{'log'}\">X</A></TD>
  <TD ALIGN=\"CENTER\"><A HREF=\"$result->{'script'}\">X</A></TD>
  <TD ALIGN=\"CENTER\">$dumplink</TD>
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

sub print_title($)
{
	my($fd) = @_;

	if ($opt_tiny != 0) {
		if ($opt_profile) {
			print $fd <<ECHO;
<CENTER>
<H1>$title</H1>
</CENTER>
ECHO
		}
		else {	# if($opt_profile)
			print $fd <<ECHO;
<CENTER>
<H1>$title</H1>
</CENTER>
<HR>
<TABLE>
<TR><TD>Tool Version</TD><TD>:</TD><TD>$VERTOOLS</TD></TR>
<TR><TD>Test Program Version</TD><TD>:</TD><TD>$VERTESTS</TD></TR>
</TABLE>
<HR>
<TABLE>
<TR><TD>Start</TD><TD>:</TD><TD>$start_time</TD></TR>
<TR><TD>End</TD><TD>:</TD><TD>$end_time</TD></TR>
</TABLE>
ECHO
		}		# if($opt_profile)f
	}
	else {
		if ($opt_profile) {
			print $fd <<ECHO;
<CENTER>
<H1>$title</H1>
ECHO
		}
		else {	# if($opt_profile)
			print $fd <<ECHO;
<CENTER>
<H1>$title</H1>
<HR>
<TABLE BORDER=0>
<TR><TD>Tool Version :</TD><TD>$VERTOOLS</TD></TR>
<TR><TD>Test Program Version :</TD><TD>$VERTESTS</TD></TR>
</TABLE>
<HR>
<TABLE BORDER=0>
<TR><TD>Start:</TD><TD>$start_time</TD></TR>
<TR><TD>End :</TD><TD>$end_time</TD></TR>
</TABLE>
<HR>
ECHO
		}		# if($opt_profile)
	}
}

sub print_td($;$$)
{
	my ($desc, $link, $td_opt) = @_;

	if (defined($td_opt)) {
		$td_opt = " $td_opt";
	}
	else {
		$td_opt = '';
	}
	print LOG "<TD$td_opt>";
	print LOG "<A HREF=\"$link\">" if $link;
	print LOG "$desc";
	print LOG "</A>" if $link;
	print LOG "</TD>";
}

sub insert_md5($)
{
	my($log) = @_;

	local(*LOG);

	unless (open(LOG, $log)) {
		return;
	}

	my $ctx = Digest::MD5->new;

	$ctx->addfile(*LOG);

	my $digest = $ctx->hexdigest;

	close(LOG);

	unless (open(LOG, ">> $log")) {
		return;
	}

	print LOG "<!-- $digest -->\n";

	close(LOG);

	return;
}

sub get_status($)
{
	my ($status) = @_;

	my %result;
	$result{0}="PASS";

	if (!$dotest) {
		$result{1}="error";
		if (($opt_nolcna) || ($opt_onlylcna)) {
			$result{73}="<FONT COLOR=\"#AAAAAA\">SKIP</FONT>";
		}
	}
	else {
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

	return($rc);
}

sub print_err($)
{
	my ($str) = @_;
	print(STDERR
	      $basename . ': ' . $str);
}


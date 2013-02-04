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
# $TAHI: v6eval/bin/pktlint/pktlint.pl,v 1.5 2001/10/05 11:09:54 tanaka Exp $

use File::Basename;
use Getopt::Long;

########################################################################

################################################################
# Create Intermediate File for Packet Definition 
# and get included files
#---------------------------------------------------------------
sub doCpp() {
	my @cmd = ($PktCompiler, "-I./", "-I$V6Root/include");

	if ($NoStd) {
		prTrace('$NoStd is 1');
	}else {
		my $includepath = searchPath("./:$V6Root/include/",'std.def');
		push(@cmd, ("-include", $includepath));
		prTrace('$NoStd is 0');
	}
	push(@cmd, ($PktDef, $PktImd));
	prTrace("Creating Intermiciate file: @cmd");

	my $ret = system(@cmd);

	my $exit_value = $ret >> 8;
	if ($ret != 0) {
		prErrExit("$PktCompiler failed exit value:$exit_value");
	}

	#
	# get included file names
	#

	open( FILE, "$PktImd") || prErrExit("Can not open $PktImd");
	while( <FILE> ){
		next unless(/^#/);
		chomp;
		($incfile) = /^\S+\s+\d+\s+"(\S+)"/;
		next if ($incfile eq "");
		next if ($incfile eq $PktDef);
		unless (grep {$incfile eq $_} @IncFiles) {
			push(@IncFiles, $incfile);
		}
	}
	close FILE;
	
}

################################################################
# run checker
#---------------------------------------------------------------

sub doCheck() {

	my $cmd = "$BinDir/checker -t $TnDef -n $NutDef -p $PktImd -i $IfName $ENV{V6EVALOPT}";
#print "$cmd\n";

	pipe P_READ, C_WRITE;
	pipe C_READ, P_WRITE;
	my $pid = fork;
	unless ($pid) {		# child
		open(STDIN, "<&C_READ");
		open(STDOUT, ">&C_WRITE");
		open(STDERR, ">&C_WRITE");
		exec "$cmd";
		prErrExit("$cmd failed");
	}
	# parent
	close(C_READ);
	close(C_WRITE);
	select(P_READ); $| = 1;
	select(P_WRITE); $| = 1;
	select(STDOUT);

	while (<P_READ>) {
		#print $_;
		chomp;
		($type, $line) = /^(\w\w\w):(.*)$/;
		if ($type eq "log") {
			prLog("  $line");
		}
		elsif ($type eq "err") {
			prErr("$line");
		}
		elsif ($type eq "std") {
			prErr("$line");
		}
		else {
			prLog("  dbg:$allmsg", 100);
		}
	}
	wait;
	$rc=$?;
	if ($?) {
		prErrExit("check failed");
	}
	$rc;
}

########################################################################
#	To handle log request.
#
#	If $level is specified, this will be compare with $LogLevel,
#	the outer level specified by '-l' arguments at the script invocaiton.
#	If it is less or equal,	than the message will be output.
#	So, value 0 is the most strong request, In case of this, 
#	the message will be logged even if any value is specified 
#	for $LogLevel.
#	In case of omittion, it is treated as 0. (always logged)
#
#	If you have some message wants to output in case LogLevel is 2
#	but not for 1, you shoud specify $level here as 1.
#
#------------------------------------------------------------------------
sub prLog($;$) { my (
	$message,	# message for logging
	$level		# log level, this will be compared with the 
			# outer level specifid by '-l' arguments
    ) = @_;
	$level=0 unless defined($level);  # default 0;
	#prTrace("LOG $level $LogLevel");
	print LOG "$message\n" if($level<=$LogLevel);
	print STDOUT "$message\n" if($VLog && $level<=$LogLevel);
}

########################################################################
#	Print message to stderr
#-----------------------------------------------------------------------
sub prOut($) { my (
	$message	# message for output
    ) = @_;
	print STDOUT "$message\n";
}

########################################################################
#	Handle trace messge just for debug for this perl module
#-----------------------------------------------------------------------
sub prTrace($) { my (
	$message	# message for output
    ) = @_;
	prOut("##### $message") if $Trace;
}

########################################################################
#	Handle error but keep continue
#-----------------------------------------------------------------------
sub prErr($) { my (
	$message	# message for error
    ) = @_;
	prLog("!!! $message");
	print STDERR "$message\n";
}

########################################################################
#	Handle error and exit
#-----------------------------------------------------------------------
sub prErrExit($) { my (
	$message	# message for error
    ) = @_;
	prErr("pktlint: $message"); # assume it is error detected
					 # in this module leval.
	exit 1;
}

########################################################################
#	parse Args
#
#	Getopt::Long module is used for argument parse
#-----------------------------------------------------------------------
sub parseArgs() {

	#printUsage() unless(@ARGV);

	$Getopt::Long::ignorecase=undef;
	my $optStat = GetOptions(
		"tn|TnDef=s",
		"nut|NutDef=s",
		"pkt|PktDef=s",
		"if|IfName=s",
		"l|LogLevel=i",
		"d|DebugLevel=i",
		"log|LogFile=s",
		"h|Help",
		"trace",	# for debug of this, output trace 
		"keepImd",	# for debug of this, keep intermidiate file 
		"v|VLog",       # print STDOUT log file
		"nostd|NoStd",
		"vroot|V6Root=s",
                "remote=s",
                "noremote",
	);
	printUsage() if( $opt_h || !$optStat);

	while( @ARGV > 0) {
		$opt = shift (@ARGV);
		print STDOUT "warning: unknown option $opt\n";
	};

	$V6Root = $opt_vroot || $ENV{V6EVALROOT} || "/usr/local/v6eval/" ;

	$BinDir=	"$V6Root/bin";
	$EtcDir=	"$V6Root/etc";

	$NoStd	= $opt_nostd	? $opt_nostd	: 0;
	$VLog   = $opt_v     ? $opt_v   : 0;
	$Trace	    =($opt_trace==1);
	$KeepImd    =($opt_keepImd==1);

	$TnDef  = $opt_tn    ? $opt_tn  : "tn.def";
	$TnDef  = searchPath("./:${EtcDir}", $TnDef); 

	$NutDef = $opt_nut   ? $opt_nut : "nut.def";
	$NutDef  = searchPath("./:${EtcDir}", $NutDef); 

	$PktDef = $opt_pkt   ? $opt_pkt : "packet.def";

	$IfName = $opt_if   ? $opt_if : "Link0";

	$LogFile= $opt_log   ? $opt_log : "packet.chk";
	$LogLevel   = $opt_l     ? $opt_l   : 1;
	$DbgLevel   = $opt_d     ? $opt_d   : 0;

	checkFile(  $TnDef );
	checkFile( $NutDef );
	checkFile( $PktDef );

	$PktCompiler=	"/usr/libexec/cpp";
	$SocketPath=	"/tmp";


	$PktImd = '/tmp/' . basename($PktDef) . '.i';
}

########################################################################
#       Search Path
#-----------------------------------------------------------------------
sub searchPath($$) { my(
        $path,           # path  
        $filename        # filename for search
    ) = @_; 
        my $fullname="";
        if($filename =~ m!.*/.+!) {
                $fullname=$filename;
        }       
	my @paths=split(/:/,($path));
        foreach(@paths) {
                my $tmpname=$_ . "/" . $filename;
                #my $tmpname=$_ .  $filename;
                if($fullname eq "") {
                        $fullname=$tmpname if(-e $tmpname);
                }       
        }
	prErr("$filename don't exist in the path ($path)") if($fullname eq "");       
        $fullname;
}    

########################################################################
#	Check file if it is readble...
#-----------------------------------------------------------------------
sub checkFile($) { my(
	$file		# command string
    ) = @_;
	unless( -r "$file" ){
		prErrExit("$file not exist or cannot read");
	}
}

########################################################################
# Check TN definition and setup environment
#---------------------------------------------------------------

sub checkTN() {
	# double entry not check yet. (OVERWRITE)
	prTrace("Looking up $TnDef");
	open( FILE, "$TnDef") || prErrExit("Can not open $TnDef");
	while( <FILE> ){
		next if(/^\s*$/ || /^#/);	# remove comment
		chomp;
		   if( /^(socketpath)\s+(\S+)/  ) { $SocketPath=$2; }
		elsif( /^(Link[0-9]+)\s+(\S+)\s+(([0-9a-f]{1,2}:){5}[0-9a-f]{1,2})/){
			push(@TnIfs,$1);
			push(@TnSos,$2);
		}
		elsif(/^(RemoteCuPath)\s+(\S+)/ ||
		      /^(RemoteDevice)\s+(\S+)/ || 
		      /^(RemoteDebug)\s+(\S+)/ || 
		      /^(RemoteIntDebug)\s+(\S+)/ || 
		      /^(RemoteLog)\s+(\S+)/ ||
		      /^(RemoteSpeed)\s+(\S+)/ ||
		      /^(RemoteLogout)\s+(\S+)/ ||
		      /^(RemoteMethod)\s+(\S+)/ ||
		      /^(filter)\s+(\S+)/) {
		    $TnDef{$1}=$2;
		}
		else{ prOut("Unknown line in $TnDef: $_\n");}
		prTrace("TN : $_");
	}
	close FILE;
}

########################################################################
# Check NUT definition and setup environment
#---------------------------------------------------------------

sub checkNUT() {
	my $ifname;
	prTrace("Looking up $NutDef");
	open( FILE, "$NutDef") || prErrExit("Can not open $NutDef");
	while( <FILE> ){
		next if(/^\s*$/ || /^#/);	# remove comment
		chomp;
		   if( /^(System)\s+(.*)/	)	{ $System = $2;}
		elsif( /^(TargetName)\s+(.*)/	) 	{ $TargetName = $2;}
		elsif( /^(HostName)\s+(.*)/  	)	{ $HostName = $2;}
		elsif( /^(Type)\s+(.*)/  	)	{ $Type = $2;}
		elsif( /^(Password)\s+(.*)/  	)	{ }
		elsif( /^(User)\s+(.*)/  	)	{ }
		elsif( /^(Link[0-9]+)\s+(\S+)\s+(([0-9a-f]{1,2}:){5}[0-9a-f]{1,2})/){
			push(@NutIfs,$1);
		}
		else{ prOut("Unknown line in $NutDef: $_");}
		prTrace("NUT: $_");
	}
	close FILE;
}

########################################################################
# check commonly used interface
#---------------------------------------------------------------

sub checkIF () {
	my $ifname;
	foreach $ifname (@TnIfs) {
		if( grep {$ifname eq $_} @NutIfs ){
			push(@IfNames, $ifname);
		}
	}
	if (@IfNames == 0) {
		prErrExit("No interface to use commonly\n".
				"TN : @TnIfs\nNUT: @NutIfs");
	}
	unless (grep {$IfName eq $_} @IfNames) {
		prErrExit("No interface definition for $IfName");
	}
}

########################################################################
#	Print usage and exit
#-----------------------------------------------------------------------
sub printUsage()  {
	prOut("Usage: $PROGRAM_NAME [-h]"    );
	prOut("       [-tn  tn_def_file]"   );
	prOut("       [-nut nut_def_file]"   );
	prOut("       [-pkt pkt_def_file]");
	prOut("       [-if interface_name]");
	#prOut("       [-log log_file] [-l log_level]");
	prOut("       [-log log_file]");
	prOut("       [-v]"	);
	prOut("       [-nostd]"	);
	prOut("	      [-vroot default directory]");
	exit 1;
}

########################################################################
# main
########################################################################

#---------------------------------------------------------------
# Parse args and complete global valiables
#---------------------------------------------------------------
parseArgs();
checkTN();
checkNUT();
checkIF();
doCpp();

#---------------------------------------------------------------
# Open log file
#---------------------------------------------------------------
open(LOG, ">$LogFile") || (
	prOut("Can not create $LogFile for logging ($!)"),exit 1) ;
prTrace("Log will be outputed to  $LogFile");

prLog('-'x72);

prLog("PktDef:  $PktDef");
foreach(@IncFiles) {
	prLog("Include: $_");
}
prLog('-'x72);

prLog("TnDef: $TnDef");
prLog('-'x72);

prLog("NutDef:     $NutDef");
if ($System)		{ prLog("System:     $System");}
if ($TargetName)	{ prLog("TargetName: $TargetName");}
if ($HostName)		{ prLog("HostName:   $HostName");}
if ($Type)		{ prLog("Type:       $Type"); }
prLog('-'x72);

$rc=doCheck();

prLog('-'x72);
close(LOG);
exit $rc;

END {
    if( $KeepImd ) {	# just for degug
	prOut("Keep packet ImdFile <$PktImd>");
    }
    else {
	unlink("$PktImd");
    }	# normally unlink
}

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
# $TAHI: v6eval/script/pmod/V6evalTool/V6evalTool.pm,v 1.204 2011/04/22 01:27:06 doo Exp $
#
########################################################################
package V6evalTool;
use Exporter;
@ISA = qw(Exporter);
use Fcntl;
use POSIX qw(tmpnam);

use File::Basename;

@EXPORT = qw(
	vSend
	vSend2
	vSend3
	vRecv
	vRecv2
	vRecv3
	vCapture
	vStop
	vClear
	vLog
	vLogHTML
	vErrmsg
	vRoundoff
	vRemote
	vRemoteAsync
	vRemoteAsyncWait
	vSleep
	vSleep2
	vCPP
	vNewPacketFile
	vOriginalPacketFile
	vMAC2LLAddr
	getChildStatus
	getChildPid
	checkChild
	vDHGen
	vDHGen2
	vDHComp
	vDHComp2
	vPRFComp
	vIVComp
	vX509dec
	vSigSign
	vSigVrfy
	vFile2Hex
);

my %storedSIG = ();

########################################################################
$exitPass=0;		# PASS
$exitIgnore=1;		# Ignore (ex. initializeation script)
$exitNS=2;		# Not yet supported
$exitWarn=3;		# WARN
$exitHostOnly=4;	# Host Only
$exitRouterOnly=5;	# Router Only
$exitSpecialOnly=6;	# Special Only
$exitExceptHost=7;	# Except Host
$exitExceptRouter=8;	# Except Router
$exitExceptSpecial=9;	# Except Special
$exitSkip=10;		# Skip
$exitTypeMismatch=11;	# Type Mismatch

$exitFail=32;		# FAIL
$exitInitFail=33;	# Initialization Fail
$exitCleanupFail=34;	# Cleanup Fail
			# 35 - 63: reserved for future use

$exitFatal=64;		# FATAL (terminate series of related tests)

########################################################################
sub checkArgs();
sub checkFile($);
sub initialize();
sub getDateString();
sub getVersion();
sub printBanner();
sub printUsage();
sub execPktctl($$@;$);
sub execCmd($;$);
sub forkCmd($$);
sub prLog($;$);
sub prOut($);
sub prTrace($);
sub prErr($);
sub prErrExit($);
sub checkChild();
sub searchPath($$);
sub getField(@);
sub getField2(@);
sub getField3(@);
sub HTML2TXT($);
sub vCPP(;@);
sub vNewPacketFile($;@);
sub vOriginalPacketFile();
sub baseArgs();
sub vDHGen($$);
sub vDHGen2($$);
sub vDHComp($$$$$);
sub vDHComp2($$$$$);
sub vPRFComp($$$);
sub vIVComp($$);
sub vX509dec($);
sub vSigSign($$);
sub vSigVrfy($$$);
sub vFile2Hex($);
sub addMD5($$$);
sub getMD5($);



########################################################################
sub
vSend($@)
{
	my ($ifname,	# target interface name
	@frames		# frame names to send
    ) = @_;

	my $timestr=getTimeStamp();
	if($vLogStat==$vLogStatOpenRow){
		prLog("</TD>\n</TR>");
	}
	prLog("<TR VALIGN=\"TOP\">\n<TD>$timestr</TD><TD>");
	prLog("vSend($ifname,@frames)<BR>");

	my $cmd= "$ToolDir/pktsend $BaseArgs -i $ifname @frames";
	my @lines; my %ret; my $fCnt=0; my $status;
	($status, @lines) = execCmd($cmd);
	%ret = getField(@lines);
	$ret{status}=$status;
	undef @pktrevers;
	foreach( @lines ){
		if( /^\s*(\d+\.\d+)$/ ) {
               		$fCnt++;
               		$ret{"sentTime${fCnt}"}	= $1;
		} else {
			$pktrevers[$fCnt+1] .= "$_\n";
		}
	}

	for($i=1;$i<=$fCnt;$i++){
		$_= $frames[$i-1];
		if($main::pktdesc{$_}){
			$msg="$main::pktdesc{$_}";
		} else {
			$msg="send $_";
		}
		prOut(HTML2TXT($msg));
		prLog("<A NAME=\"vSend${vSendPKT}\"></A>");
		prLog("<A HREF=\"#vSendPKT${vSendPKT}\">$msg</A><BR>");

		$senttime=getTimeString($ret{"sentTime${i}"});
		$pktrevlog .="<A NAME=\"vSendPKT${vSendPKT}\"></A>";
		$pktrevlog .="<A HREF=\"#vSend${vSendPKT}\">$msg at $senttime</A>\n";
		$pktrevlog .="<PRE STYLE=\"line-height:70%\">";

		my $Xpktrevers = $pktrevers[$i];

		$Xpktrevers =~ s/&/&amp;/g;
		$Xpktrevers =~ s/"/&quot;/g;
		$Xpktrevers =~ s/</&lt;/g;
		$Xpktrevers =~ s/>/&gt;/g;

		$pktrevlog .=$Xpktrevers;

		$pktrevlog .="</PRE><HR>";

		$vSendPKT++;
	}
	if($ret{status}){		
		prErrExit("vSend() return status=$ret{status}\n");
	}
	prLog("\n</TD>\n</TR>");
	$vLogStat=$vLogStatCloseRow;
	%ret;
}



################################################################
# vSend2($ifname, @frames)                                     #
#                                                              #
#     $ifname - target interface name                          #
#     @frames - frame names to send                            #
################################################################
sub
vSend2($@)
{
	my ($ifname, @frames) = @_;

	my $timestr = getTimeStamp();

	if($vLogStat == $vLogStatOpenRow){
		prLog("</TD>\n");
		prLog("</TR>");
	}

	prLog("<TR VALIGN=\"top\">\n");
	prLog("<TD>$timestr</TD><TD>");
	prLog("vSend($ifname,@frames)<BR>");

	my $cmd = "$ToolDir/pktsend $BaseArgs -i $ifname @frames";

	my $fCnt = 0;

	my ($status, @lines) = execCmd($cmd);

	my %ret = getField2(@lines);
	$ret{'status'}=$status;

	undef(@pktrevers);

	foreach(@lines){
		if($_ =~ /^\s*(\d+\.\d+)$/) {
			$fCnt ++;
			$ret{"sentTime${fCnt}"}	= $1;
		} else {
			$pktrevers[$fCnt + 1] .= "$_\n";
		}
	}

	for(my $i = 1; $i <= $fCnt; $i ++) {
		$_ = $frames[$i - 1];
		if($main::pktdesc{$_}){
			$msg = "$main::pktdesc{$_}";
		} else {
			$msg = "send $_";
		}

		prOut(HTML2TXT($msg));

		prLog("<A NAME=\"vSend${vSendPKT}\"></A>");
		prLog("<A HREF=\"#vSendPKT${vSendPKT}\">$msg</A><BR>");

		$senttime = getTimeString($ret{"sentTime${i}"});

		$pktrevlog .="<A NAME=\"vSendPKT${vSendPKT}\"></A>";
		$pktrevlog .="<A HREF=\"#vSend${vSendPKT}\">$msg at $senttime</A>\n";
		$pktrevlog .="<PRE STYLE=\"line-height:70%\">";

		my $Xpktrevers = $pktrevers[$i];

		$Xpktrevers =~ s/&/&amp;/g;
		$Xpktrevers =~ s/"/&quot;/g;
		$Xpktrevers =~ s/</&lt;/g;
		$Xpktrevers =~ s/>/&gt;/g;

		$pktrevlog .=$Xpktrevers;

		$pktrevlog .="</PRE><HR>";

		$vSendPKT ++;
	}

	if($ret{status}){		
		prErrExit("vSend() return status=$ret{status}\n");
	}

	prLog("\n</TD>\n</TR>");
	$vLogStat = $vLogStatCloseRow;

	return(%ret);
}



sub
vSend3($@)
{
	my ($ifname, @frames) = @_;

	my $timestr = getTimeStamp();

	if($vLogStat == $vLogStatOpenRow){
		prLog("</TD>\n");
		prLog("</TR>");
	}

	prLog("<TR VALIGN=\"top\">\n");
	prLog("<TD>$timestr</TD><TD>");
	prLog("vSend($ifname,@frames)<BR>");

	my $cmd = "$ToolDir/pktsend $BaseArgs -i $ifname @frames";

	my $fCnt = 0;

	my ($status, @lines) = execCmd($cmd);

	my %ret = getField3(@lines);
	$ret{'status'}=$status;

	undef(@pktrevers);

	foreach(@lines){
		if($_ =~ /^\s*(\d+\.\d+)$/) {
			$fCnt ++;
			$ret{"sentTime${fCnt}"}	= $1;
		} else {
			$pktrevers[$fCnt + 1] .= "$_\n";
		}
	}

	for(my $i = 1; $i <= $fCnt; $i ++) {
		$_ = $frames[$i - 1];
		if($main::pktdesc{$_}){
			$msg = "$main::pktdesc{$_}";
		} else {
			$msg = "send $_";
		}

		prOut(HTML2TXT($msg));

		prLog("<A NAME=\"vSend${vSendPKT}\"></A>");
		prLog("<A HREF=\"#vSendPKT${vSendPKT}\">$msg</A><BR>");

		$senttime = getTimeString($ret{"sentTime${i}"});

		$pktrevlog .="<A NAME=\"vSendPKT${vSendPKT}\"></A>";
		$pktrevlog .="<A HREF=\"#vSend${vSendPKT}\">$msg at $senttime</A>\n";
		$pktrevlog .="<PRE STYLE=\"line-height:70%\">";

		my $Xpktrevers = $pktrevers[$i];

		$Xpktrevers =~ s/&/&amp;/g;
		$Xpktrevers =~ s/"/&quot;/g;
		$Xpktrevers =~ s/</&lt;/g;
		$Xpktrevers =~ s/>/&gt;/g;

		$pktrevlog .=$Xpktrevers;

		$pktrevlog .="</PRE><HR>";

		$vSendPKT ++;
	}

	if($ret{status}){		
		prErrExit("vSend() return status=$ret{status}\n");
	}

	prLog("\n</TD>\n</TR>");
	$vLogStat = $vLogStatCloseRow;

	return(%ret);
}



########################################################################
sub
vRecv($$$$@)
{
	my ($ifname,	# target interface name
	$timeout,	# expire time(second)
			#	-1: No limitation
	$seektime,	# seek to the packet at the time(%ld.%06d)
			#	0: seek from the beginning 
			#	   of the captured buffer
			#	You may use $retval{'sentTime(n)'}
			#	returned by vSend().
	$count,		# How many frames to wait 
			#	0: No limitation
	@frames		# frame names to send
    ) = @_;
	my $cmd= "$ToolDir/pktrecv $BaseArgs";
	$cmd.=" -i $ifname"   if $ifname;
	$cmd.=" -e $timeout"  if $timeout >= 0;
	$cmd.=" -s $seektime" if $seektime;
	$cmd.=" -c $count"    if $count;
	$cmd.=" @frames";

	my $timestr=getTimeStamp();
	if($vLogStat==$vLogStatOpenRow){
		prLog("</TD>\n</TR>");
	}
	prLog("<TR VALIGN=\"TOP\"><TD>$timestr</TD>");
	prLog("<TD>vRecv($ifname,@frames)". 
	      " timeout:$timeout cntLimit:$count seektime:$seektime<BR>");
	my @lines; my %ret; my $fCnt=0; my $status;
	($status, @lines) = execCmd($cmd);
	%ret = getField(@lines);
	$ret{status}=$status;
	undef @pktrevers;
	foreach( @lines ){
		if( /^\s*(\d+\.\d+)\s+(\S*)$/ ) {
               		$fCnt++;
               		$ret{"recvTime${fCnt}"}	= $1;
               		$ret{recvFrame}		= $2;
		} else {
			$pktrevers[$fCnt+1] .= "$_\n";
		}
	}
	undef($ret{recvFrame}) if ($ret{recvFrame} eq '-');
	$ret{recvCount}=$fCnt;

	for($i=1;$i<=$fCnt;$i++){
		my $recvtime=getTimeString($ret{"recvTime${i}"});

		if($i!=$fCnt || $ret{status}!=0){
			prLog("<A NAME=\"vRecv${vRecvPKT}\"></A>");
			if(@frames){
			    prLog("<A HREF=\"#vRecvPKT${vRecvPKT}\">recv unexpect packet at $recvtime</A>");
			} else {
			    prLog("<A HREF=\"#vRecvPKT${vRecvPKT}\">recv a packet at $recvtime</A>");
			}
			prLog("<BR>");
		}
		$pktrevlog .="<A NAME=\"vRecvPKT${vRecvPKT}\"></A>";
		$pktrevlog .="<A HREF=\"#vRecv${vRecvPKT}\">Recv at $recvtime</A>\n";
		$pktrevlog .="<PRE STYLE=\"line-height:70%\">";

		my $Xpktrevers = $pktrevers[$i];

		$Xpktrevers =~ s/&/&amp;/g;
		$Xpktrevers =~ s/"/&quot;/g;
		$Xpktrevers =~ s/</&lt;/g;
		$Xpktrevers =~ s/>/&gt;/g;

		$pktrevlog .="$Xpktrevers";

		$pktrevlog .="</PRE><HR>";

		$vRecvPKT++;
	}

	if($ret{status}>=3){
		prErrExit("vRecv() return status=$ret{status}\n");
	}
	if($ret{status}==0){
		$_= $ret{recvFrame};
		$vRecvPKT--;
		if($main::pktdesc{$_}){
			$msg="$main::pktdesc{$_}";
		} else {
			$msg="recv $_";
		}
		prOut(HTML2TXT($msg));
		prLog("<A NAME=\"vRecv${vRecvPKT}\"></A>");
		prLog("<A HREF=\"#vRecvPKT${vRecvPKT}\">$msg</A><BR>");
		$vRecvPKT++;
	} else {
		prLog("vRecv() return status=$ret{status}");
	}
	prLog("\n</TD>\n</TR>\n");
	$vLogStat=$vLogStatCloseRow;
	%ret;
}



################################################################
# vRecv2($ifname, $timeout, $seektime, $count, @frames)        #
#                                                              #
#     $ifname   - target interface name                        #
#     $timeout  - expire time(second)                          #
#                     -1: No limitation                        #
#     $seektime - seek to the packet at the time(%ld.%06d)     #
#                     0: seek from the beginning of            #
#                         the captured buffer                  #
#                     You may use $retval{'sentTime(n)'}       #
#                         returned by vSend().                 #
#     $count    - How many frames to wait                      #
#                     0: No limitation                         #
#     @frames   - frame names to send                          #
################################################################
sub
vRecv2($$$$@)
{
	my ($ifname, $timeout, $seektime, $count, @frames) = @_;

	my $cmd  = "$ToolDir/pktrecv $BaseArgs";
	   $cmd .= " -i $ifname"	if($ifname);
	   $cmd .= " -e $timeout"	if($timeout >= 0);
	   $cmd .= " -s $seektime"	if($seektime);
	   $cmd .= " -c $count"		if($count);
	   $cmd .= " @frames";

	my $timestr = getTimeStamp();

	if($vLogStat == $vLogStatOpenRow){
		prLog("</TD>\n");
		prLog("</TR>");
	}

	prLog("<TR VALIGN=\"top\"><TD>$timestr</TD>");
	prLog("<TD>vRecv($ifname,@frames)". 
	      " timeout:$timeout cntLimit:$count seektime:$seektime<BR>");

	my $fCnt = 0;

	my ($status, @lines) = execCmd($cmd);

	my %ret = getField2(@lines);
	$ret{'status'} = $status;

	undef(@pktrevers);

	foreach(@lines){
		if($_ =~ /^\s*(\d+\.\d+)\s+(\S*)$/ ) {
			$fCnt ++;
			$ret{"recvTime${fCnt}"}	= $1;
			$ret{'recvFrame'}	= $2;
		} else {
			$pktrevers[$fCnt + 1] .= "$_\n";
		}
	}

	undef($ret{'recvFrame'}) if($ret{'recvFrame'} eq '-');

	$ret{'recvCount'} = $fCnt;

	for(my $i = 1; $i <= $fCnt; $i ++){
		my $recvtime = getTimeString($ret{"recvTime${i}"});

		if(($i != $fCnt) || ($ret{'status'})){
			prLog("<A NAME=\"vRecv${vRecvPKT}\"></A>");

			if(@frames){
				prLog("<A HREF=\"#vRecvPKT${vRecvPKT}\">recv unexpect packet at $recvtime</A>");
			} else {
				prLog("<A HREF=\"#vRecvPKT${vRecvPKT}\">recv a packet at $recvtime</A>");
			}

			prLog("<BR>");
		}

		$pktrevlog .= "<A NAME=\"vRecvPKT${vRecvPKT}\"></A>";
		$pktrevlog .= "<A HREF=\"#vRecv${vRecvPKT}\">Recv at $recvtime</A>\n";
		$pktrevlog .= "<PRE STYLE=\"line-height:70%\">";

		my $Xpktrevers = $pktrevers[$i];

		$Xpktrevers =~ s/&/&amp;/g;
		$Xpktrevers =~ s/"/&quot;/g;
		$Xpktrevers =~ s/</&lt;/g;
		$Xpktrevers =~ s/>/&gt;/g;

		$pktrevlog .= $Xpktrevers;

		$pktrevlog .= "</PRE><HR>";

		$vRecvPKT ++;
	}

	if($ret{'status'} >= 3){
		prErrExit("vRecv() return status=$ret{status}\n");
	}

	if($ret{'status'}){
		prLog("vRecv() return status=$ret{status}");
	} else {
		$_ = $ret{'recvFrame'};

		$vRecvPKT --;

		if($main::pktdesc{$_}){
			$msg = "$main::pktdesc{$_}";
		} else {
			$msg = "recv $_";
		}

		prOut(HTML2TXT($msg));
		prLog("<A NAME=\"vRecv${vRecvPKT}\"></A>");
		prLog("<A HREF=\"#vRecvPKT${vRecvPKT}\">$msg</A><BR>");

		$vRecvPKT ++;
	}

	prLog("\n");
	prLog("</TD>\n");
	prLog("</TR>\n");
	$vLogStat = $vLogStatCloseRow;

	return(%ret);
}



########################################################################
sub
vRecv3($$$$@)
{
	my ($ifname, $timeout, $seektime, $count, @frames) = @_;

	my $cmd  = "$ToolDir/pktrecv $BaseArgs";
	   $cmd .= " -i $ifname"	if($ifname);
	   $cmd .= " -e $timeout"	if($timeout >= 0);
	   $cmd .= " -s $seektime"	if($seektime);
	   $cmd .= " -c $count"		if($count);
	   $cmd .= " @frames";

	my $timestr = getTimeStamp();

	if($vLogStat == $vLogStatOpenRow){
		prLog("</TD>\n");
		prLog("</TR>");
	}

	prLog("<TR VALIGN=\"top\"><TD>$timestr</TD>");
	prLog("<TD>vRecv($ifname,@frames)". 
	      " timeout:$timeout cntLimit:$count seektime:$seektime<BR>");

	my $fCnt = 0;

	my ($status, @lines) = execCmd($cmd);

	my %ret = getField3(@lines);	### differ to vRecv2
	$ret{'status'} = $status;

	undef(@pktrevers);

	foreach(@lines){
		if($_ =~ /^\s*(\d+\.\d+)\s+(\S*)$/ ) {
			$fCnt ++;
			$ret{"recvTime${fCnt}"}	= $1;
			$ret{'recvFrame'}	= $2;
		} else {
			$pktrevers[$fCnt + 1] .= "$_\n";
		}
	}

	undef($ret{'recvFrame'}) if($ret{'recvFrame'} eq '-');

	$ret{'recvCount'} = $fCnt;

	for(my $i = 1; $i <= $fCnt; $i ++){
		my $recvtime = getTimeString($ret{"recvTime${i}"});

		if(($i != $fCnt) || ($ret{'status'})){
			prLog("<A NAME=\"vRecv${vRecvPKT}\"></A>");

			if(@frames){
				prLog("<A HREF=\"#vRecvPKT${vRecvPKT}\">recv unexpect packet at $recvtime</A>");
			} else {
				prLog("<A HREF=\"#vRecvPKT${vRecvPKT}\">recv a packet at $recvtime</A>");
			}

			prLog("<BR>");
		}

		$pktrevlog .= "<A NAME=\"vRecvPKT${vRecvPKT}\"></A>";
		$pktrevlog .= "<A HREF=\"#vRecv${vRecvPKT}\">Recv at $recvtime</A>\n";
		$pktrevlog .= "<PRE STYLE=\"line-height:70%\">";

		my $Xpktrevers = $pktrevers[$i];

		$Xpktrevers =~ s/&/&amp;/g;
		$Xpktrevers =~ s/"/&quot;/g;
		$Xpktrevers =~ s/</&lt;/g;
		$Xpktrevers =~ s/>/&gt;/g;

		$pktrevlog .= $Xpktrevers;

		$pktrevlog .= "</PRE><HR>";

		$vRecvPKT ++;
	}

	if($ret{'status'} >= 3){
		prErrExit("vRecv() return status=$ret{status}\n");
	}

	if($ret{'status'}){
		prLog("vRecv() return status=$ret{status}");
	} else {
		$_ = $ret{'recvFrame'};

		$vRecvPKT --;

		if($main::pktdesc{$_}){
			$msg = "$main::pktdesc{$_}";
		} else {
			$msg = "recv $_";
		}

		prOut(HTML2TXT($msg));
		prLog("<A NAME=\"vRecv${vRecvPKT}\"></A>");
		prLog("<A HREF=\"#vRecvPKT${vRecvPKT}\">$msg</A><BR>");

		$vRecvPKT ++;
	}

	prLog("\n");
	prLog("</TD>\n");
	prLog("</TR>\n");
	$vLogStat = $vLogStatCloseRow;

	return(%ret);
}



########################################################################
sub
vStop($)
{
	my ($ifname,	# target interface name
    ) = @_;
	execPktctl('Stop Capturing Packets', $ifname, 'stop');
	return 0;
}



########################################################################
sub
vClear($)
{
	my ($ifname,	# target interface name
    ) = @_;
	execPktctl('Clear Captured Packets', $ifname, 'clear');
	return 0;
}



########################################################################
sub
vCapture($)
{
	my ($ifname,	# target interface name
    ) = @_;
	execPktctl('Start Capturing Packets', $ifname, 'capture');
	return 0;
}



########################################################################
sub
vWrap($)
{
	my ($ifname,	# target interface name
    ) = @_;
	execPktctl('vWrap', $ifname, 'wrap');
	return 0;
}



########################################################################
sub
vNowrap($)
{
	my ($ifname,	# target interface name
    ) = @_;
	execPktctl('vNowrap', $ifname, 'nowrap');
	return 0;
}



########################################################################
sub
vLog($)
{
	my ($message,	# message to be logged
    ) = @_;
	if($vLogStat==$vLogStatCloseRow){
		prLog("<TR><TD><BR></TD><TD>");
		$vLogStat=$vLogStatOpenRow;
	};
	prLog("<PRE>$message</PRE>");
	prOut "$message" if $VLog == 0;
}



########################################################################
sub
vLogHTML($)
{
	my ($message,	# message to be logged
    ) = @_;
	if($vLogStat==$vLogStatCloseRow){
		prLog("<TR><TD><BR></TD><TD>");
		$vLogStat=$vLogStatOpenRow;
	};
	prLogHTML($message);
	prOut "$message" if $VLog == 0;
}



########################################################################
sub
vErrmsg(%)
{
	my (%stat		# return status
    ) = @_;
	my($s1, $s2);

	return if $stat{status} == 0;

	if($stat{status} > 3) {
		return "ERROR: Internal error";
	} elsif($stat{status} == 1) {
		$s1="Time-out";
	} elsif($stat{status} == 2) {
		$s1="Count-out";
	}
	if($stat{recvCount} == 0) {
		$s2 = "Could not get any packet";
	} else {
		$s2 = "Got unexpected packet(s)";
	}
	return "ERROR: $s1: $s2";
}



########################################################################
sub
vRoundoff($)
{
	my ($timeval		# sec.usec
    ) = @_;
	my($sec, $usec, $val);

	($sec, $usec)=split(/\./, $timeval);
	if(length($usec) >=6) {
		$val=$timeval;
	} else {
		$val=sprintf("%d.%06d", $sec, $usec);
	}
	$val+=0.05;
	($sec, $usec)=split(/\./, $val);
	substr($usec, 1) = "";
	sprintf("%d.%01d", $sec, $usec);
}



########################################################################
sub
vStatus($)
{
	my (
	$ifname,	# target interface name
    ) = @_;
	my @lines; my %ret;
	($ret{status}, @lines) = execPktctl('vStatus', $ifname, 'status');
	foreach( @lines ){
		if( /^(\S+)\s+(\S+)$/ ) {
               		$ret{$1}=$2;
		}
	}
	%ret;
}



########################################################################
sub
vDump($$)
{
	my (
	$ifname,	# target interface name
	$count		# the number of frames to be dumped
    ) = @_;
	my ($status) = execPktctl('vDump', $ifname, 'dump', $count);
	$status;
}



########################################################################
sub
vSleep($;$)
{
	my (
	$seconds,	# sleep time
	$msg		# comment
    ) = @_;

	my $timestr=getTimeStamp();
	if(!$msg){ $msg = "Wait $seconds second"; }

	prLog("</TD></TR>\n") if($vLogStat==$vLogStatOpenRow);
	prLog("<TR VALIGN=\"TOP\">\n<TD>$timestr</TD>\n<TD>$msg\n</TD></TR>");
	$vLogStat=$vLogStatCloseRow;
	prOut("$msg\n");
	# sleep $seconds;
	# use following sequence instead of sleep $seconds;
	# derived from [users:01956]
	my $timeBeg=time;
	my $timeAtEnd=($timeBeg+$seconds);
	while(1){
		my $timeNow=time;
		if ( $timeNow ge $timeAtEnd ){ last };
		sleep 1;
	}
}



########################################################################
sub
vSleep2($;$)
{
	my (
	$seconds,	# sleep time
	$msg		# comment
    ) = @_;

	my $timestr=getTimeStamp();
	if(!$msg){ $msg = "Wait $seconds second"; }

	prLog("</TD></TR>\n") if($vLogStat==$vLogStatOpenRow);
	prLog("<TR VALIGN=\"TOP\">\n<TD>$timestr</TD>\n<TD>$msg\n</TD></TR>");
	$vLogStat=$vLogStatCloseRow;
	prOut("$msg\n");
	select(undef, undef, undef, $seconds);
}



########################################################################
sub
vRemote($;$@)
{
	my (
	$fname,		# remote file name
	$opts,		# options
	@args,		# variable args
    ) = @_;

	my $rpath = "./";
	$rpath .= ":./$NutDef{System}/" if $NutDef{System};
	$rpath .= ":${V6Root}/bin/$NutDef{System}/" if $NutDef{System};
	$rpath .= ":${V6Root}/bin/unknown/";

	my $cmd  = searchPath($rpath, $fname); 
	$cmd .= " -t $NutDef{System}"		if $NutDef{System};
	$cmd .= " -u $NutDef{User}"		if $NutDef{User};
	$cmd .= " -p $NutDef{Password}"		if $NutDef{Password};
	$cmd .= " -T $TnDef{RemoteCuPath}"	if $TnDef{RemoteCuPath};
	$cmd .= " -d $TnDef{RemoteDevice}"	if $TnDef{RemoteDevice};
	$cmd .= " -v $TnDef{RemoteDebug}"	if $TnDef{RemoteDebug};
	$cmd .= " -i $TnDef{RemoteIntDebug}"	if $TnDef{RemoteIntDebug};
	$cmd .= " -o $TnDef{RemoteLog}"		if $TnDef{RemoteLog};
	$cmd .= " -s $TnDef{RemoteSpeed}"	if $TnDef{RemoteSpeed};
	$cmd .= " -l $TnDef{RemoteLogout}"	if $TnDef{RemoteLogout};
	$cmd .= " $RemoteOption $opts @args";

	prLog("</TD>\n</TR>") if($vLogStat==$vLogStatOpenRow);
	my $timestr=getTimeStamp();
	prLogHTML("<TR VALIGN=\"TOP\"><TD>$timestr</TD>\n");
	prLogHTML("<TD>\nvRemote($fname) ``$cmd''\n");

        if($vrasync_state eq RUNNING){
		prErrExit("vRemoteAsync process already exists: pid=$pid_vrasync"); 
	}

	if($TnDef{RemoteMethod} eq 'none') {
		$_ = "Remote command is disabled by ``RemoteMethod: none''";
		prOut($_) if $VLog == 0;
		prLogHTML("<BR>$_\n</TD></TR>");
		$vLogStat=$vLogStatCloseRow;
		exit $exitNS;
	}

	prLogHTML("<PRE>");
	pipe PIN,COUT;
	$ChildPid = 0;
	my $localExecutingPid;
	unless(defined($ExecutingPid = $localExecutingPid = fork)){
		$localExecutingPid=0;
		prErrExit("Fork failed for <$cmd>"); 
	}
	unless( $localExecutingPid ) { # Children
		open(STDOUT, ">&COUT");
		open(STDERR, ">&COUT");
		close(PIN);
		# close(STDIN);
		# should close if any explicit open files exit
		exec "$cmd";
#		prErrExit("Exec failed for <$cmd>"); 
	}

	my $pid=$localExecutingPid;
	close(COUT);
	prTrace("vRemote()... $cmd");

	$stdout = '';
	while( <PIN> ) {
		$stdout .= $_;
		chomp;
		s/\r//g;
		s/&/&amp;/g;
		s/"/&quot;/g;
		s/</&lt;/g;
		s/>/&gt;/g;
		prLog("$_");
		prOut("$_") if $VLog == 0;
	}
	close(PIN);
	while($ChildPid == 0){};
	my $status = getChildStatus();
	prErrExit("Unknown child died $pid $ChildPid (status=$status)!!") if($ChildPid != $pid);
	if($status & 0xff) { # died by signal
		prErrExit("Catch signal <$cmd>");
	}
	my $rc=($status >> 8);
	if($rc == $exitNS) {
		$_ = "NUT($NutDef{System}) does not support the functionality";
		prLog($_);
		prOut($_) if $VLog == 0;
		exit $exitNS;
	}
	prLog("</PRE>\n</TD></TR>");
	$vLogStat=$vLogStatCloseRow;
	$rc;
}



########################################################################
sub
vRemoteAsync($;$@)
{
	my (
	$fname,		# remote file name
	$opts,		# options
	@args,		# variable args
    ) = @_;

	my $rpath = "./";
	$rpath .= ":./$NutDef{System}/" if $NutDef{System};
	$rpath .= ":${V6Root}/bin/$NutDef{System}/" if $NutDef{System};
	$rpath .= ":${V6Root}/bin/unknown/";

	my $cmd  = searchPath($rpath, $fname); 
	$cmd .= " -t $NutDef{System}"		if $NutDef{System};
	$cmd .= " -u $NutDef{User}"		if $NutDef{User};
	$cmd .= " -p $NutDef{Password}"		if $NutDef{Password};
	$cmd .= " -T $TnDef{RemoteCuPath}"	if $TnDef{RemoteCuPath};
	$cmd .= " -d $TnDef{RemoteDevice}"	if $TnDef{RemoteDevice};
	$cmd .= " -v $TnDef{RemoteDebug}"	if $TnDef{RemoteDebug};
	$cmd .= " -i $TnDef{RemoteIntDebug}"	if $TnDef{RemoteIntDebug};
	$cmd .= " -o $TnDef{RemoteLog}"		if $TnDef{RemoteLog};
	$cmd .= " -s $TnDef{RemoteSpeed}"	if $TnDef{RemoteSpeed};
	$cmd .= " -l $TnDef{RemoteLogout}"	if $TnDef{RemoteLogout};
	$cmd .= " $RemoteOption $opts @args";

	prLog("</TD>\n</TR>") if($vLogStat==$vLogStatOpenRow);
	my $timestr=getTimeStamp();
	prLogHTML("<TR VALIGN=\"TOP\"><TD>$timestr</TD>\n");
	prLogHTML("<TD>\nvRemoteAsync($fname) ``$cmd''\n");

        if($vrasync_state eq RUNNING){
		prErrExit("vRemoteAsync process already exists: pid=$pid_vrasync"); 
	}
        $vrasync_state = RUNNING;

	if($TnDef{RemoteMethod} eq 'none') {
		$_ = "Remote command is disabled by ``RemoteMethod: none''";
		prOut($_) if $VLog == 0;
		prLogHTML("<BR>$_\n</TD></TR>");
		$vLogStat=$vLogStatCloseRow;
		exit $exitNS;
	}

	prLogHTML("<PRE>");
	pipe PIN_VRASYNC,COUT_VRASYNC;
	my $localExecutingPid_vrasync;
	unless(defined($ExecutingPid_vrasync = $localExecutingPid_vrasync = fork)){
		$localExecutingPid_vrasync=0;
		prErrExit("Fork failed for <$cmd>"); 
	}
	unless( $localExecutingPid_vrasync ) { # Children
		open(STDOUT, ">&COUT_VRASYNC");
		open(STDERR, ">&COUT_VRASYNC");
		close(PIN_VRASYNC);
		# close(STDIN);
		# should close if any explicit open files exit
		print("sleep 3 [sec] for escaping critical point of asynchronous remoteconf.\n\n");
		sleep 3;
		exec "$cmd";
#		prErrExit("Exec failed for <$cmd>"); 
	}

	$pid_vrasync=$localExecutingPid_vrasync;
	close(COUT_VRASYNC);
	prTrace("vRemoteAsync()... $cmd");

        prLog("<A NAME=\"vRemoteAsync$pid_vrasync\"></A>");
        prLog("<A HREF=\"#vRemoteAsyncWait$pid_vrasync\">Link to remote control log</A>");

	prLog("</PRE>\n</TD></TR>");
	$vLogStat=$vLogStatCloseRow;
        return 0;
}



########################################################################
sub
vRemoteAsyncWait()
{
	prLog("</TD>\n</TR>") if($vLogStat==$vLogStatOpenRow);
	my $timestr=getTimeStamp();
	prLogHTML("<TR VALIGN=\"TOP\"><TD>$timestr</TD>\n");
	prLogHTML("<TD>\nvRemoteAsyncWait()\n");

        if($vrasync_state ne RUNNING){
		prErrExit("vRemoteAsyncWait() called without vRemoteAsync()"); 
	}
        undef $vrasync_state;

	prLogHTML("<PRE>");
        prLog("<A NAME=\"vRemoteAsyncWait$pid_vrasync\"></A>");
        prLog("<A HREF=\"#vRemoteAsync$pid_vrasync\">Link to remote control start point</A>");

	$stdout = '';
	while( <PIN_VRASYNC> ) {
		$stdout .= $_;
		chomp;
		s/\r//g;
		s/&/&amp;/g;
		s/"/&quot;/g;
		s/</&lt;/g;
		s/>/&gt;/g;
		prLog("$_");
		prOut("$_") if $VLog == 0;
	}
	close(PIN_VRASYNC);
        
	while($ExecutingPid_vrasync){};
	my $status = $ChildStatus_vrasync;
        if($ChildPid_vrasync != $pid_vrasync) {
	    prErrExit("Unknown child died $pid_vrasync $ChildPid_vrasync (status=$status)!!");
        }

	if($status & 0xff) { # died by signal
		prErrExit("Catch signal <$cmd>");
	}
	my $rc=($status >> 8);
	if($rc == $exitNS) {
		$_ = "NUT($NutDef{System}) does not support the functionality";
		prLog($_);
		prOut($_) if $VLog == 0;
		exit $exitNS;
	}
	prLog("</PRE>\n</TD></TR>");
	$vLogStat=$vLogStatCloseRow;
	$rc;
}



########################################################################
# Create Intermediate File for Packet Definition 
########################################################################
sub
vCPP(;@)
{
	my (
	@opts		# pre-proceser options
    ) = @_;
	my $PktCompiler="cc -E -x c-header";
	my $baseopt="-I./ -I./ -I$V6Root/include/";
	my $deffile=$PktDef;

	prTrace("vCPP($deffile)");
	prTrace("\$NoStd is $NoStd");
	if (!$NoStd) {
		my $includepath = searchPath("./:$V6Root/include/",'std.def');
		$baseopt .= " -include $includepath"
	}

	if(!$PktImd){
		if($KeepImd){
			$PktImd = '/tmp/' . basename($deffile) . '.i';
		} else {
			do { $PktImd = tmpnam() }
				until sysopen(FH, $PktImd, O_RDWR|O_CREAT|O_EXCL);
			close FH;
		}
	}

	my $cmd = "$PktCompiler $baseopt @opts $deffile > $PktImd";
	prTrace("Creating Intermediate file: $cmd");
	my ($sts) = execCmd($cmd);
	if( $sts!=0 ){
		prErrExit("$cmd failed status:$sts\n");
	}
}



########################################################################
# Replace the command line specified Packet Definition File
########################################################################
sub
vNewPacketFile($;@)
{
	my (
	$pktfile,	# Packet definition file
	@opts		# Pre-processor options
	) = @_;

	vOriginalPacketFile();
	$OriginalPktDef=$PktDef;
	$OriginalPktImd=$PktImd;

	$PktDef=$pktfile;
	$PktImd='';
	vCPP(@opts, $CppOption);

	# Rebuild the base arguments (which specifies the file)
	baseArgs();
}



########################################################################
# Restore the command line specified Packet Definition File
########################################################################
sub
vOriginalPacketFile()
{
	if (defined $OriginalPktDef) {
		if( $KeepImd ) {
			prOut("Keep packet ImdFile <$PktImd>");
		}
		else {
			unlink("$PktImd");
		}
		$PktDef=$OriginalPktDef;
		$PktImd=$OriginalPktImd;
		undef $OriginalPktDef;
		undef $OriginalPktImd;

		# Rebuild the base arguments (which specifies the file)
		baseArgs();
	}
}



sub
vDHGen($$)
{
	my (
	$prime,
	$gen
	) = @_;

	my $cmd= "$ToolDir/dhgen -p $prime -g $gen";
	my @lines; my %ret; my $status;
	($status, @lines) = execCmd($cmd);
	%ret = getField(@lines);
	$ret{status}=$status;
	if($ret{status}){
		prErrExit("vDHGen() return status=$ret{status}\n");
	}
	%ret;
}



sub
vDHGen2($$)
{
	my (
	$prime,
	$gen
	) = @_;

	my $cmd= "$ToolDir/dhgen -@ -p $prime -g $gen";
	my @lines; my %ret; my $status;
	($status, @lines) = execCmd($cmd);
	%ret = getField(@lines);
	$ret{status}=$status;
	if($ret{status}){
		prErrExit("vDHGen2() return status=$ret{status}\n");
	}
	%ret;
}



sub
vDHComp($$$$$)
{
	my (
	$prime,
	$gen,
	$pubkey,
	$privkey,
	$recvpubkey
	) = @_;

	my $cmd= "$ToolDir/dhcomp -p $prime -g $gen -x $pubkey -y $privkey -z $recvpubkey";
	my @lines; my %ret; my $status;
	($status, @lines) = execCmd($cmd);
	%ret = getField(@lines);
	$ret{status}=$status;
	if($ret{status}){
		prErrExit("vDHComp() return status=$ret{status}\n");
	}
	%ret;
}



sub
vDHComp2($$$$$)
{
	my (
	$prime,
	$gen,
	$pubkey,
	$privkey,
	$recvpubkey
	) = @_;

	my $cmd= "$ToolDir/dhcomp -p $prime -@ -g $gen -x $pubkey -y $privkey -z $recvpubkey";
	my @lines;my %ret; my $status;
	($status, @lines) = execCmd($cmd);
	%ret = getField(@lines);
	$ret{status}=$status;
	if($ret{status}){
		prErrExit("vDHComp2() return status=$ret{status}\n");
	}
	%ret;
}



sub
vPRFComp($$$)
{
	my ($algorithm, $key, $msg) = @_;

	my $cmd= "$ToolDir/prfcomp -a $algorithm -k $key -m $msg";
	my @lines;my %ret;my $status;
	($status, @lines) = execCmd($cmd);
	%ret = getField(@lines);
	$ret{status}=$status;
	if($ret{status}){
		prErrExit("vPRFComp() return status=$ret{status}\n");
	}
	%ret;
}



sub
vIVComp($$)
{
	my ($algorithm, $msg) = @_;

	my $cmd= "$ToolDir/ivcomp -a $algorithm -m $msg";
	my @lines;my %ret;my $status;
	($status, @lines) = execCmd($cmd);
	%ret = getField(@lines);
	$ret{status}=$status;
	if($ret{status}){
		prErrExit("vIVComp() return status=$ret{status}\n");
	}
	%ret;
}



sub
vX509dec($)
{
	my ($certification) = @_;

	my $cmd= "$ToolDir/x509dec -d $certification";
	my @lines;my %ret;my $status;
	($status, @lines) = execCmd($cmd);
	%ret = getField(@lines);
	$ret{status}=$status;
	if($ret{status}){
		prErrExit("vX509dec() return status=$ret{status}\n");
	}
	%ret;
}



sub
vSigSign($$)
{
	my ($key, $msg) = @_;

	my $cmd= "$ToolDir/sigcomp -k $key -m $msg";
	my @lines;my %ret;my $status;
	($status, @lines) = execCmd($cmd);
	%ret = getField(@lines);
	$ret{status}=$status;
	if($ret{status}){
		prErrExit("vSigSign() return status=$ret{status}\n");
	}
	%ret;
}



sub
vSigVrfy($$$)
{
	my ($key, $msg, $signature) = @_;

	my $cmd= "$ToolDir/sigcomp -k $key -m $msg -s $signature";
	my @lines;my %ret;my $status;
	($status, @lines) = execCmd($cmd);
	%ret = getField(@lines);
	$ret{status}=$status;
	if($ret{status}){
		prErrExit("vSigVrfy() return status=$ret{status}\n");
	}
	%ret;
}



sub
vFile2Hex($)
{
	my ($file) = @_;

	my $cmd= "$ToolDir/file2hex $file";
	my @lines;my %ret;my $status;
	($status, @lines) = execCmd($cmd);
	%ret = getField(@lines);
	$ret{status}=$status;
	if($ret{status}){
		prErrExit("vFile2Hex() return status=$ret{status}\n");
	}
	%ret;
}



#--------------------------------------------------------------#
# void addMD5(char *, char *, char *);                         #
#--------------------------------------------------------------#
sub
addMD5($$$)
{
	my($seq, $def, $log) = @_;

	my @md5 = ();

	local(*LOG);

	push(@md5, getMD5($seq));
	push(@md5, getMD5($def));
	push(@md5, getMD5($log));

	unless(open(LOG, ">> $log")) {
		return;
	}

	for(my $d = 0; $d <= $#md5; $d ++) {
		print LOG "<!-- $md5[$d] -->\n";
	}

	close(LOG);

	return;
}



#--------------------------------------------------------------#
# char *getMD5(char *);                                        #
#--------------------------------------------------------------#
sub
getMD5($)
{
	my($file) = @_;

	use Digest::MD5;

	local(*FILE);

	unless(open(FILE, $file)) {
		return('');
	}

	my $ctx = Digest::MD5->new;
	$ctx->addfile(*FILE);
	my $digest = $ctx->hexdigest;

	close(FILE);

	return($digest);
}



########################################################################
#	parse Args
#
#	Getopt::Long module is used for argument parse
#-----------------------------------------------------------------------
sub
parseArgs()
{
	use Getopt::Long;
	$Getopt::Long::ignorecase=0;
	my $optStat = GetOptions(
		"tn|TnDef=s",
		"nut|NutDef=s",
		"pkt|PktDef=s",
		"l|LogLevel=i",
		"ti|TestTitle=s",
		"log|LogFile=s",
		"h|Help",
		"trace",	# for debug of this, output trace 
		"keepImd",	# for debug of this, keep intermidiate file 
		"v|VLog",       # print STDOUT log file
		"nostd|NoStd",
		"vroot|V6Root=s",
		"remote=s",
		"cpp=s",
	);
	printUsage() if( $opt_h || !$optStat );

	$V6Root = $opt_vroot || $ENV{'V6EVALROOT'} || "/usr/local/v6eval/" ;
	$NoStd	= $opt_nostd	? $opt_nostd	: 0;
	$VLog   = $opt_v     ? $opt_v   : 0;
#	$Trace	    = ($opt_trace==1);
#	$KeepImd    = ($opt_keepImd==1);
	$Trace	    = $opt_trace ? $opt_trace : 0;
	$KeepImd    = $opt_keepImd ? $opt_keepImd : 0;
	$TnDef  = $opt_tn    ? $opt_tn  : "tn.def";
	$TnDef  = searchPath("${SeqDir}:./:${V6Root}/etc/", $TnDef); 
	$NutDef = $opt_nut   ? $opt_nut : "nut.def";
	$NutDef  = searchPath("${SeqDir}:./:${V6Root}/etc/", $NutDef); 
	$PktDef = $opt_pkt   ? $opt_pkt : "${SeqDir}packet.def";
	$TestTitle= $opt_ti   ? $opt_ti : "${SeqName}";
	$LogFile= $opt_log   ? $opt_log : "${SeqDir}${SeqName}.log";
	$LogLevel   = $opt_l     ? $opt_l   : 1;
	$DbgLevel   = $opt_d     ? $opt_d   : 0;
	$RemoteOption = $opt_remote || "";
	$CppOption = $opt_cpp || "";

	checkFile( $PktDef );
}



########################################################################
#	Set base args for commands
#-----------------------------------------------------------------------
sub
baseArgs()
{
	$BaseArgs = "-t $TnDef -n $NutDef -p $PktImd -l$LogLevel";
	$BaseArgs.= " $ENV{'V6EVALOPT'}" if(defined($ENV{'V6EVALOPT'}));
}



########################################################################
#       Search Path
#-----------------------------------------------------------------------
sub
searchPath($$)
{
	my(
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
                        $fullname=$tmpname if(-r $tmpname);
                }       
        }
	prErrExit("$filename don't exist in the path or cannot read in ``$path''\n") if($fullname eq "");       
        $fullname;
}    



########################################################################
#	Check file if it is readble...
#-----------------------------------------------------------------------
sub
checkFile($)
{
	my(
	$file		# command string
    ) = @_;
	unless( -r "$file" ){
		prErrExit("$file not exist or cannot read\n");
	}
}



########################################################################
#	Get Each Field Value
#-----------------------------------------------------------------------
sub
getField(@)
{
	my (
	@lines
    ) = @_;

	my %rc;

	foreach( @lines ){
		if(/^\s*((\|\s)*)(\S*)\s*\(\S*\)/) {
			my $block=$3;
			if($block eq 'Frame_Ether'){undef %rc;};
			if($block eq 'DHGen_Results'){undef %rc;};
			if($block eq 'DHComp_Results'){undef %rc;};
			if($block eq 'PRFComp_Results'){undef %rc;};
			if($block eq 'IVComp_Results'){undef %rc;};
#			undef %rc if($block eq 'Frame_Ether');
	    		$curDepth=length($1)/2;
	    		while(@Struct>$curDepth) {
				pop @Struct;
			}
			push (@Struct,$block);
			my $xstruct=join(".",@Struct);
			my $num=++$rc{"$xstruct#"};
			if($num>1) {
				pop @Struct;
				$block.="$num"; # xxx
				push (@Struct,$block);
	    		}
			pop @Struct;
			if (@Struct!=0) {
				my $name=join(".",@Struct);
				$rc{"$name"}.= " " if ($rc{"$name"});
				$rc{"$name"}.= "$block";
#				print "Struct: ($name) = ($rc{$name})\n";
			}
	    		push (@Struct,$block);
		}
		elsif( /^\s*((\|\s)+)(\S*)\s+=\s+(\S*)/ ) {
			my $name=$3;
			my $member=join(".",@Struct);
			$member.=".$name";
			$rc{"$member#"}++;
			$name.="_$rc{\"$member#\"}" if($rc{"$member#"}>1); # xxx
			$member = join(".",@Struct);
			$member.=".$name";
			$xstruct=join(".",@Struct);
			$rc{"$xstruct"}.=" " if($rc{"$xstruct"});
			$rc{"$xstruct"}.="$name";
			$rc{"$member"}=$4;
#			print "Struct: ($xstruct) = $rc{$xstruct}\n";
#			print "Value : ($member) = $rc{$member}\n";
		}
		else {
#			print "not reverse output\n";
		}
	}
	%rc;
}



########################################################################
#	Get Each Field Value
#-----------------------------------------------------------------------
sub
getField2(@)
{
	my (@lines) = @_;

	my %rc = ();
	my @Struct = ();
	my $Xmember = '';

	foreach(@lines){
		if($_ =~ /^\s*((\|\s)*)(\S*)\s*\(\S*\)/) {
			my $Xmember = '';

			my $block = $3;
			if($block eq 'Frame_Ether') {
				%rc = ();
				@Struct = ();
			}

			if($block eq 'DHGen_Results'){
				%rc = ();
				@Struct = ();
			};

			if($block eq 'DHComp_Results'){
				%rc = ();
				@Struct = ();
			};

			if($block eq 'PRFComp_Results'){
				%rc = ();
				@Struct = ();
			};

			if($block eq 'IVComp_Results'){
				%rc = ();
				@Struct = ();
			};

			my $curDepth = length($1)/2;
	    		while($#Struct >= $curDepth) {
				pop(@Struct);
			}

			push(@Struct, $block);

			my $xstruct = join('.', @Struct);

			unless(defined($rc{"$xstruct#"})) {
				$rc{"$xstruct#"} = 0;
			}

			my $num = ++ $rc{"$xstruct#"};

			if($num > 1) {
				pop(@Struct);
				$block .= "$num"; # xxx
				push(@Struct, $block);
			}

			pop(@Struct);

			if($#Struct >= 0) {
				my $name = join('.', @Struct);
				$rc{"$name"} .= " " if(defined($rc{"$name"}));
				$rc{"$name"} .= "$block";
			}

			push(@Struct, $block);
		} elsif($_ =~ /^\s*((\|\s)+)(\S*)\s+=\s+(.*)$/) {
			my $name = $3;
			my $member = join('.', @Struct);
			$member .= ".$name";

			unless(defined($rc{"$member#"})) {
				$rc{"$member#"} = 0;
			}

			$rc{"$member#"}++;

			$name .= "_$rc{\"$member#\"}"
				if($rc{"$member#"} > 1); # xxx

			$member  = join('.', @Struct);
			$member .= ".$name";

			$xstruct = join('.', @Struct);
			$rc{"$xstruct"} .= ' ' if(defined($rc{"$xstruct"}));
			$rc{"$xstruct"} .= "$name";

			my $Xdata = $4;

			if($Xdata =~ /^[0-9a-f]{8}\s([0-9a-f]{8}\s{1,2})*[0-9a-f]{1,8}$/) {
				$Xdata =~ s/\s//g;
			}

			$rc{"$member"} = $Xdata;

			$Xmember = $member;
		} elsif($_ =~ /^\s{2}(.*)$/) {
			my $Xdata = $1;

			if($Xdata =~ /^[0-9a-f]{8}\s([0-9a-f]{8}\s{1,2})*[0-9a-f]{1,8}$/) {
				$Xdata =~ s/\s//g;
			} else {
				$Xdata .= "\n";
			}

			if(($Xmember ne '') && (defined($rc{$Xmember}))) {
				$rc{$Xmember} .= $Xdata;
			}
		} else {
		}
	}

	return(%rc);
}



########################################################################
#	Get Each Field Value
#-----------------------------------------------------------------------
sub
getField3(@)
{
	my (@lines) = @_;

	my %rc = ();
	my @Struct = ();
	my $Xmember = '';

	foreach(@lines){
		if($_ =~ /^\s*((\|\s)*)(\S*)\s*\(\S*\)/) {
			my $Xmember = '';

			my $block = $3;
			if($block eq 'Frame_Ether') {
				%rc = ();
				@Struct = ();
			}

			if($block eq 'DHGen_Results'){
				%rc = ();
				@Struct = ();
			};

			if($block eq 'DHComp_Results'){
				%rc = ();
				@Struct = ();
			};

			if($block eq 'PRFComp_Results'){
				%rc = ();
				@Struct = ();
			};

			if($block eq 'IVComp_Results'){
				%rc = ();
				@Struct = ();
			};

			my $curDepth = length($1)/2;
	    		while($#Struct >= $curDepth) {
				pop(@Struct);
			}

			push(@Struct, $block);

			my $xstruct = join('.', @Struct);

			unless(defined($rc{"$xstruct#"})) {
				$rc{"$xstruct#"} = 0;
			}

			my $num = ++ $rc{"$xstruct#"};

			if($num > 1) {
				pop(@Struct);
				$block .= "$num"; # xxx
				push(@Struct, $block);
			}

			pop(@Struct);

			if($#Struct >= 0) {
				my $name = join('.', @Struct);
				$rc{"$name"} .= " " if(defined($rc{"$name"}));
				$rc{"$name"} .= "$block";
			}

			push(@Struct, $block);
		} elsif($_ =~ /^\s*((\|\s)+)(\S*)\s+=\s+(.*)$/) {
			my $name = $3;
			my $member = join('.', @Struct);
			$member .= ".$name";

			unless(defined($rc{"$member#"})) {
				$rc{"$member#"} = 0;
			}

			$rc{"$member#"}++;

			$name .= "_$rc{\"$member#\"}"
				if($rc{"$member#"} > 1); # xxx

			$member  = join('.', @Struct);
			$member .= ".$name";

			$xstruct = join('.', @Struct);
			$rc{"$xstruct"} .= ' ' if(defined($rc{"$xstruct"}));
			$rc{"$xstruct"} .= "$name";

			my $Xdata = $4;

			if($Xdata =~ /^[0-9a-f]{8}\s([0-9a-f]{8}\s{1,2})*[0-9a-f]{1,8}$/) {
				$Xdata =~ s/\s//g;
			}

			$rc{"$member"} = $Xdata;

			$Xmember = $member;
		} elsif($_ =~ /^\s*((\|\s)+)\s{2}(.*)$/) {
		### add to getField2
			my $Xdata = $3;

			if($Xdata =~ /^[0-9a-f]{8}\s([0-9a-f]{8}\s{1,2})*[0-9a-f]{1,8}$/) {
				$Xdata =~ s/\s//g;
			} else {
				$Xdata .= "\n";
			}

			if(($Xmember ne '') && (defined($rc{$Xmember}))) {
				$rc{$Xmember} .= $Xdata;
			}
		} elsif($_ =~ /^\s{2}(.*)$/) {
			my $Xdata = $1;

			if($Xdata =~ /^[0-9a-f]{8}\s([0-9a-f]{8}\s{1,2})*[0-9a-f]{1,8}$/) {
				$Xdata =~ s/\s//g;
			} else {
				$Xdata .= "\n";
			}

			if(($Xmember ne '') && (defined($rc{$Xmember}))) {
				$rc{$Xmember} .= $Xdata;
			}
		} else {
		}
	}

	return(%rc);
}



########################################################################
#	module initialization
#	
#	This is calld at the start time of the script.
#	Actually called from BEGIN{} of this module.
#	do some preparation procedure for using this module.
#-----------------------------------------------------------------------
sub
initialize()
{ 
	#---------------------------------------------------------------
	# Get script path
	#---------------------------------------------------------------
	($SeqName,$SeqDir,$SeqSuffix) = fileparse($0,'.seq');

	#---------------------------------------------------------------
	# Parse args 
	#---------------------------------------------------------------
        $CommandLine = "$0 @ARGV";  # keep commandline for logging
	my @ARGVtmp = @ARGV;
	parseArgs();
	if(defined($ENV{'V6EVAL_WITH_KOI'})) {
		@ARGV = @ARGVtmp;
	}

	#---------------------------------------------------------------
	# Setup initial value
	#---------------------------------------------------------------
	$ToolDir=	"$V6Root/bin";
	$SocketPath=	"/tmp";

	#---------------------------------------------------------------
	# Set Version value
	#---------------------------------------------------------------
	getVersion();

	#---------------------------------------------------------------
	# Open log file
	#---------------------------------------------------------------
	unless(defined($ENV{'V6EVAL_WITH_KOI'})) {
	open(LOG, ">$LogFile") || 
		prErrExit("Can not create $LogFile for logging ($!)");
	}
	prTrace("Log will be outputed to  $LogFile");

	#---------------------------------------------------------------
	# print Banner 
	#---------------------------------------------------------------
	prLog("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\"".
	      "\"http://www.w3c.org/TR/REC-html40/strict.dtd\">");
	prLog("<HTML>\n<HEAD>");
	prLog("<TITLE>IPv6 Conformance Test Report</TITLE>");
	prLog("<META NAME=\"GENERATOR\" CONTENT=\"TAHI IPv6 Conformance Test Kit\">");
	prLog("</HEAD>\n");
	prLog("<BODY BGCOLOR=\"#F0F0F0\">");

	prLog("<H1>Test Information</H1>");
	prLog("<TABLE BORDER=1>");
	printBanner();

	#---------------------------------------------------------------
	# Check TN definition and setup environment
	#---------------------------------------------------------------
	# double entry not check yet. (OVERWRITE)
	prTrace("Looking up $TnDef");
	open( FILE, "$TnDef") || prErrExit("Can not open $TnDef");
	while( <FILE> ){
		next if(/^\s*$/ || /^#/);	# remove comment
		chomp;
		if( /^(\S+)\s+(.*)/ )		{
		    $TnDef{$1} = $2;
		}
		   if( /^(socketpath)\s+(\S+)/  ) { $SocketPath=$2; }
		elsif( /^(Link[0-9]+)\s+(\S+)\s+(([0-9a-f]{1,2}:){5}[0-9a-f]{1,2})/){
			push(@TnIfs,$1);
			push(@TnSos,$2);
			$TnDef{$1."_device"}=$2;
			$TnDef{$1."_addr"}=$3;
		}
		elsif( /^(RemoteDevice)\s+(\S+)/   ||
		       /^(RemoteDebug)\s+(\S+)/    ||
		       /^(RemoteIntDebug)\s+(\S+)/ ||
		       /^(RemoteLog)\s+(\S+)/      ||
		       /^(RemoteSpeed)\s+(\S+)/    ||
		       /^(RemoteLogout)\s+(\S+)/   ||
		       /^(RemoteMethod)\s+(\S+)/   ||
		       /^(RemoteCuPath)\s+(\S+)/   ||
		       /^(filter)\s+(\S+)/){

		}
		else{ prOut("Unknown line in $TnDef: $_\n");}
		prTrace("TN : $_");
	}
	close FILE;
	#---------------------------------------------------------------
	# remove unix domain sockets
	#---------------------------------------------------------------
	my $soname;
	foreach $soname (@TnSos) {
		prTrace("TN : unlink $SocketPath/$soname");
		unlink("$SocketPath/$soname");
	}

	#---------------------------------------------------------------
	# Check NUT definition and setup environment
	#---------------------------------------------------------------
	prTrace("Looking up $NutDef");
	open( FILE, "$NutDef") || prErrExit("Can not open $NutDef");
	while( <FILE> ){
		next if(/^\s*$/ || /^#/);	# remove comment
		chomp;
		if( /^(\S+)\s+(.*)/ )		{
		    $NutDef{$1} = $2;
		}
		if( /^(System)\s+(.*)/              ||
		    /^(TargetName)\s+(.*)/	    ||
		    /^(HostName)\s+(.*)/            ||
		    /^(Type)\s+(.*)/         ){
		    prLog("<TR><TD>$1</TD><TD>$2</TD></TR>");
		} elsif(
		    /^(User)\s+(\S+)/               ||
		    /^(Password)\s+(\S+)/    ){
		}
		elsif( /^(Link[0-9]+)\s+(\S+)\s+(([0-9a-fA-F]{1,2}:){5}[0-9a-fA-F]{1,2})/){
			push(@NutIfs,$1);
			$NutDef{$1."_device"}=$2;
			$NutDef{$1."_addr"}=$3;
		}
		else{ prOut("Unknown line in $NutDef: $_");}
		prTrace("NUT: $_");
	}
	close FILE;
	prLog("</TABLE>");
	#---------------------------------------------------------------
	# Fork pktbuf for necessary interface
	#---------------------------------------------------------------
	my $cmdBase = "$ToolDir/pktbuf -t $TnDef -i ";
	my $dumpCmd = "tcpdump -n -s 2000 -x -e";
	my $ifname; my $log;
	foreach $ifname (@TnIfs) {
		if( grep {$ifname eq $_} @NutIfs ){
			$cmd="$cmdBase $ifname";
			$log="/tmp/pktbuf_$ifname.log";
			push(@PktbufPids, forkCmd($cmd,$log) );
			# run tcpdump
			my($if)=$TnDef{$ifname."_device"};
			my ($basename, $dirname) = fileparse($LogFile);
			$basename =~ s/\./_/g;
			my $pcapfile = $dirname . $basename . '_' . $ifname;
			$cmd="$dumpCmd ".
			    "-i $if ".
			    "-w $pcapfile.pcap ";
			unless(defined($ENV{'V6EVAL_WITH_KOI'})) {
			push(@TcpdumpPids, forkCmdWoCheck($cmd,'/dev/null') );
			}
		}
	}
	if(@PktbufPids==0) {
		prErrExit("No interface to use commonly\n".
				"TN : @TnIfs\nNUT: @NutIfs");
	}

	vCPP("$CppOption -D__INITIALIZE_V6EVAL__");
	undef $OriginalPktDef;
	undef $OriginalPktImd;

	#---------------------------------------------------------------
	# Setup global value for convinience
	#---------------------------------------------------------------
	baseArgs();

	prLog("<HR>");
	prLog("<H1>Test Sequence Execution Log</H1>");
	prLog("<TABLE BORDER=1>");
	my $timestamp=getTimeStamp();
	prLog("<TR><TD>$timestamp</TD><TD>Start</TD></TR>");
	$vLogStat=$vLogStatCloseRow;
}



########################################################################
#	Get time string
#
#	If more detailed format is required, you may switch to use
#	prepared perl modlue.
#-----------------------------------------------------------------------
sub
getTimeStamp()
{
	my ($sec,$min,$hour) = localtime;
	my $timestr = sprintf('%02d:%02d:%02d', $hour, $min, $sec);
	$timestr;
}



sub
getDateString()
{
	my ($sec,$min,$hour,$day,$mon,$year) = localtime;
	my $datestr = sprintf '%02d/%02d/%02d %02d:%02d:%02d',
			$year+1900, $mon+1, $day, $hour, $min, $sec;
	$datestr;
}



sub
getTimeString($)
{
	my (
	$epoch
    ) = @_;
	my ($sec,$min,$hour) = localtime($epoch);
	my $timestr = sprintf('%02d:%02d:%02d', $hour, $min, $sec);
	$timestr;
}



########################################################################
#	Get Version
#
#          set following variables
#          $ToolVersion: version number of v6eval tool
#          $TestVersion: version number of test
#-----------------------------------------------------------------------
sub
getVersion()
{
	my $dummy;

	($dummy, $ToolVersion) = ('$Name: REL_3_3_2 $' =~ /\$(Name): (.*) \$/ );
	if(!$ToolVersion){
		$ToolVersion=   'undefined';
	}
	if (defined($TestVersion)) { # it should be defined in *.seq file.
        	($dummy, $TestVersion) = ($TestVersion =~ /\$(Name): (.*) \$/  );
	}else{
		$TestVersion=   'undefined';
	}
}



########################################################################
#	Print log banner
#-----------------------------------------------------------------------
sub
printBanner()
{
	my $datestr = getDateString();
	prLog("<TR><TD>Title</TD><TD>$TestTitle</TD></TR>");
	prLog("<TR><TD>CommandLine</TD><TD>$CommandLine</TD></TR>");
	prLog("<TR><TD>TestVersion</TD><TD>$TestVersion</TD></TR>");
	prLog("<TR><TD>ToolVersion</TD><TD>$ToolVersion</TD></TR>");
	prLog("<TR><TD>Start</TD><TD>$datestr</TD></TR>");
	prLog("<TR><TD>Tn</TD><TD>$TnDef</TD></TR>");
	prLog("<TR><TD>Nu</TD><TD>$NutDef</TD></TR>");
	prLog("<TR><TD>Pkt</TD><TD>$PktDef</TD></TR>");
}



########################################################################
#	Print usage and exit
#-----------------------------------------------------------------------
sub
printUsage()
{
	prOut("Usage: $PROGRAM_NAME [-h]"    );
	prOut("       [-tn  tn_def_file]"   );
	prOut("       [-nut nut_def_file]"   );
	prOut("       [-pkt pkt_def_file]");
	prOut("       [-log log_file] [-l log_level]");
	prOut("       [-v]"	);
	prOut("       [-nostd]"	);
	prOut("       [-vroot default_directory]");
	exit;
}



########################################################################
#	Subroutine used commonly who wants to invoke pktctl
#
#	This calls the execCmd() internally.
#	If the $funcname is 'vDump', then indicate execCmd() to
#	output stdout information to the STDOUT as soon as possible
#	without buffering to array value which returnd by execCmd().
#	This is for not cosuming amount of memmorry for the array
#	in case of many information is outputed by pktctl command.
#-----------------------------------------------------------------------
sub
execPktctl($$@;$)
{
	my (
	$funcname,	# the name of the fuction who call me 
	$ifname,	# target interface name
	@arg		# arguments
    ) = @_;
	my $cmd= "$ToolDir/pktctl -t $TnDef";
	$cmd.=" -i $ifname" if $ifname;
	$cmd.=" @arg";

	my $timestr=getTimeStamp();
	if($vLogStat==$vLogStatOpenRow){
		prLog("</TD>\n</TR>");
	}
	prLog("<TR VALIGN=\"TOP\"><TD>$timestr</TD><TD>$funcname ($ifname)<BR>");
	prOut("$funcname ($ifname)");
	my @ret = execCmd($cmd, ($funcname eq 'vDump') );

	if($ret[0]){		
		prErrExit("$funcname($ifname) return status=$ret[0]\n");
	}
	prLog("</TD></TR>\n");
	$vLogStat=$vLogStatCloseRow;
	@ret;
}



########################################################################
#	Subroutine used commonly who wants to invoke pktxxx command 
#
#	Execute command and get its sdtout and stderr information
#	concurrently. This assumes the format of the output is 
#	prefixed with the following keywords.
#	log:	treat it as log information
#	err:	treat it as error information
#	std:	treat it as the information outputed from stdout
#
#	This returns array, the first element indicates the exit code
#	of the command. And the next to end of the array indicates
#	standard output devided into pieaces by '\n'.
#
#	In case of command execution failure, call exit.
#-----------------------------------------------------------------------
sub
execCmd($;$)
{
	my (
	$cmd,		# command string
	$std_redirect	# if set, 'std:' message will be output here.
    ) = @_;
	my $type; my $line; my $pid; my $localExecutingPid;
	my @ret;

	pipe PIN,COUT;
	$ChildPid = 0;
	unless(defined($ExecutingPid = $localExecutingPid = fork)){
		$localExecutingPid=0;
		prErrExit("Fork failed for <$cmd>"); 
        }
	unless( $localExecutingPid ) { # Children
		open(STDOUT, ">&COUT");
		open(STDERR, ">&COUT");
		close(PIN);
		close(STDIN);
		# should close if any explicit open files exit
#		exec "$cmd; exit $?;";
		exec "$cmd;";
#		prErrExit("Exec failed for <$cmd>"); 
	}

	$pid=$localExecutingPid;
	close(COUT);
	prTrace("execCmd()... $cmd");

	while( <PIN> ) {
		chomp;
		$allmsg=$_;
		($type, $line) = /^(\w\w\w):(.*)$/;
		   if($type eq 'log') {
#			prLog("  $line");
			push(@ret, $line);
			prTrace("execCmd()ret... log:$line");
		}
        	elsif($type eq 'err') { prErr("$line");}
		elsif($type eq 'std') { 
			$std_redirect ? prOut($line) : push(@ret, $line);
			prTrace("execCmd()ret... std:$line");
		}else {
			prLog("  dbg: $allmsg",100);
		}
    	}
	close(PIN);
	while($ChildPid == 0){};
	my $status = getChildStatus();
	prErrExit("Unknown child died $pid $ChildPid (status=$status)!!") if($ChildPid != $pid);
	if( $status & 0xff ) {	# died by signal
		prErrExit("Catch signal <$cmd>");
	}
	unshift(@ret, ($status>>8)); # command exit code
	@ret;
}



########################################################################
#	Subroutine used to detach pktbuf process 
#
#	Do fork and exec $command,  and returns $pid
#-----------------------------------------------------------------------
sub
forkCmd($$)
{
	my (
	$cmd,	# command string
	$log	# log file
    ) = @_;
	my $pid;
	($arg0) = split ' ' ,$cmd;

	pipe PIN,COUT;
	unless(defined($pid = fork)){
		prErrExit("Fork failed for <$cmd>"); 
        }
	unless( $pid ) { # Children
		# should close if any explicit open files exit
		close(PIN);
		open(STDOUT, ">&COUT");
		open(STDERR, ">$log");
		exec "$cmd";
#		print COUT "err:Exec fail for $arg0 ($!)\n";
#		prErrExit("Exec failed for <$opt>"); 
#		prErrExit("Exec failed for <$cmd>"); 
	}
	close(COUT);
	prTrace("forkCmd()... $cmd");
	my $ready=0;
	while( <PIN> ) {	# wait until socket ready
		chomp;
		($type, $line) = /^(\w\w\w):(.*)$/;
		if($_ eq 'std: ready'){
			$ready=1;
			last;
		} elsif ($type eq 'err') { prErr("$line");}
    	}
	close(PIN);
	unless($ready){
		prErrExit("Could not get ready response from <$arg0>");
	}
	$pid;
}



sub
forkCmdWoCheck($$)
{
	my (
	$cmd,	# command string
	$log	# log file
    ) = @_;
	my $pid;
	($arg0) = split ' ' ,$cmd;

	pipe PIN,COUT;
	unless(defined($pid = fork)){
		prErrExit("Fork failed for <$cmd>"); 
        }
	unless( $pid ) { # Children
		# should close if any explicit open files exit
		close(PIN);
		open(STDOUT, ">&COUT");
		open(STDERR, ">$log");
		exec "$cmd";
#		print COUT "err:Exec fail for $arg0 ($!)\n";
#		prErrExit("Exec failed for <$opt>"); 
#		prErrExit("Exec failed for <$cmd>"); 
	}
	close(COUT);
	close(PIN);
	prTrace("forkCmdWoCheck()... $cmd");
	$pid;
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
sub
prLog($;$)
{
	my (
	$message,	# message for logging
	$level		# log level, this will be compared with the 
			# outer level specifid by '-l' arguments
    ) = @_;
	$level=0 unless defined($level);  # default 0;
	#prTrace("LOG $level $LogLevel");
	unless(defined($ENV{'V6EVAL_WITH_KOI'})) {
	print LOG "$message\n" if($level<=$LogLevel);
	}
	prOut "$message\n" if($VLog && $level<=$LogLevel);
}



#------------------------------------------------------------------------
sub
prLogHTML($;$)
{
	my (
	$message,	# message for logging
	$level		# log level, this will be compared with the 
			# outer level specifid by '-l' arguments
    ) = @_;
	$level=0 unless defined($level);  # default 0;
	#prTrace("LOG $level $LogLevel");
	unless(defined($ENV{'V6EVAL_WITH_KOI'})) {
		if(defined(LOG)){
			print LOG "$message" if($level<=$LogLevel);
		}
	}
	prOut "$message" if($VLog && $level<=$LogLevel);
}



########################################################################
#	Convert HTML text to Plain Text
#-----------------------------------------------------------------------
sub
HTML2TXT($)
{
	my (
	$message	# HTML message
    ) = @_;
	$message =~ s/<BR>/\n/g;
	$message =~ s/<\/TR>/\n/g;
	$message =~ s/<\/TD>/  /g;
	$message =~ s/<[^>]*>//g;
	$message =~ s/&lt\;/</g;
	$message =~ s/&gt\;/>/g;
	$message =~ s/\n+$//g;
	$message .= "\n";
	return $message;
}



########################################################################
#	Print message to stderr
#-----------------------------------------------------------------------
sub
prOut($)
{
	my (
	$message	# message for output
    ) = @_;
	$message=HTML2TXT($message);
	print STDOUT "$message";
}



########################################################################
#	Handle trace messge just for debug for this perl module
#-----------------------------------------------------------------------
sub
prTrace($)
{
	my (
	$message	# message for output
    ) = @_;
	prOut("##### $message") if $Trace;
}



########################################################################
#	Handle error but keep continue
#-----------------------------------------------------------------------
sub
prErr($)
{
	my (
	$message	# message for error
    ) = @_;
	prLogHTML("<FONT COLOR=\"#FF0000\">!!! $message</FONT><BR>");
	$message=HTML2TXT($message);
	print STDERR "$message";
}



########################################################################
#	Handle error and exit
#-----------------------------------------------------------------------
sub
prErrExit($)
{
	my (
	$message	# message for error
    ) = @_;
	prErr("V6evalTool.pm $message"); # assume it is error detected
					 # in this module leval.
	$!=$InternalErr,die "";
}



########################################################################
#	Handler to chatch SIGCHLD
#
#	In case that the died child is pktbuf process,
#	output the message and exit this script.
#	And, we assume that execCmd() will be invoked one by one.
#-----------------------------------------------------------------------
sub
checkChild()
{
	$ChildPid    = wait;
	$ChildStatus = $?; 	# save child status
	my ($HexStatus) = sprintf("0x%08x",$ChildStatus);
	prTrace("Catch child die pid=$ChildPid status=$HexStatus");

	if($ChildPid==$ExecutingPid_vrasync) {   # vRemoteAsync() finished
	    $ExecutingPid_vrasync=0;
	    $ChildPid_vrasync = $ChildPid;       # save child pid    for vRemoteAsync()
	    $ChildStatus_vrasync = $ChildStatus; # save child status for vRemoteAsync()
	    $ChildPid=0;
	    return;
	}

	$ExecutingPid=0 if($ChildPid==$ExecutingPid); # execCmd() finished
	if( grep {$ChildPid==$_} @PktbufPids, @TcpdumpPids) {
		my $pid;
		for(1..scalar(@PktbufPids)){
			$pid=shift(@PktbufPids);
			if($pid==$ChildPid){
				prErrExit("pktbuf died pid=$ChildPid status=$HexStatus");
			}
			push(@PktbufPids,$pid);
		}
		for(1..scalar(@TcpdumpPids)){
			$pid=shift(@TcpdumpPids);
			if($pid==$ChildPid){
				prErrExit("tcpdump died pid=$ChildPid status=$HexStatus");
			}
			push(@TcpdumpPids,$pid);
		}
	}
}



########################################################################
#	Interface subroutine who needs child's exit status
#-----------------------------------------------------------------------
sub
getChildPid()
{
	$ChildPid;
}



sub
getChildStatus()
{
	$ChildStatus;	# This should be comment out in case you 
			# decide not to chatch SIGCHLD in this module
	# $?;		# Then, you should return $? here, for the 
			# subroutines inside of this module.
}



########################################################################
#	Subroutine which makes link local address from EUI-64
#-----------------------------------------------------------------------
sub
vMAC2LLAddr($)
{
	my (
	$addr		# MAC Address
    ) = @_;
	my (@str, @hex);

	@str=split(/:/,$addr);
	foreach(@str) {
		push @hex,hex($_);
	};
	
	#
	# invert universal/local bit
	$hex[0] ^= 0x02;

	sprintf "fe80::%02x%02x:%02xff:fe%02x:%02x%02x",@hex;
}



########################################################################
#	End procedure
#
#	This will be called when exit is calld .
#	just output time stamp, and do some post processs.
#	To kill pktbuf process, send SIGTERM
#-----------------------------------------------------------------------
END
{ 
	vOriginalPacketFile();

	if($vLogStat==$vLogStatOpenRow){
		prLog("</TD>\n</TR>");
	}

	my $timestr=getTimeStamp();
	prLog("<TR><TD>$timestr</TD><TD>End</TD></TR>");
	prLog("</TABLE>");

	prLog("<HR><H1>Packet Reverse Log</H1>");
	prLog("<UL>\n$pktrevlog</UL>\n");

	prLog("</BODY>\n</HTML>");
	unless(defined($ENV{'V6EVAL_WITH_KOI'})) {
	close(LOG);
	addMD5($0, $PktDef, $LogFile);
	}
	if( $KeepImd ) {	# just for degug
		prOut("Keep packet ImdFile <$PktImd>");
	}
	else { unlink("$PktImd"); }	# normally unlink

	#---------------------------------------------------------------
	# remove unix domain sockets
	#---------------------------------------------------------------
	my $soname;
	foreach $soname (@TnSos) {
		prTrace("unlink $SocketPath/$soname");
		unlink("$SocketPath/$soname");
	}

	# Send SIGTERM to myself for killing all my children (pktbuf & tcpdump)
	$SIG{CHLD} = IGNORE;
	#
	# stop running pktbuf processes and the some comannd if still running 
	#
	push(@PktbufPids,$ExecutingPid) if($ExecutingPid!=0);
	foreach(@PktbufPids){
		prTrace("Exiting... sending SIGTERM to $_");
        	kill('TERM',$_) || prOut("Error in killing pktbuf pid=$_");
	}
	sleep 1;
	foreach(@TcpdumpPids){
		prTrace("Exiting... sending SIGTERM to $_");
        	kill('INT',$_) || prOut("Error in killing tcpdump pid=$_");
	}

	foreach my $key (keys(%SIG)) {
		$SIG{$key} = 'DEFAULT';
	}

	%SIG = %storedSIG;
}



########################################################################
#	Begin procedure
#
#	Since the subroutine define bellow will not be resolved when
#	BEGIN{} is processed. 
#	This module had better locate bottom.
#	
#-----------------------------------------------------------------------
BEGIN
{
	%storedSIG = %SIG;

	foreach my $key (keys(%SIG)) {
		$SIG{$key} = 'DEFAULT';
	}

	$|=1;			# set command buffering mode
	$ExecutingPid=0;	# pid of command executing 
				# such as cpp/pkt{send,recv,ctl}
	$InternalErr=64;        # exit code
	# Set SIGCHLD handler to check pktbuf died while testing
	$SIG{CHLD} = \&checkChild;
	$SIG{TERM} = sub {exit;};	# This is the time to die !!
					# explicit exit here to avoid 
					# printing 'Terminated' message 
					# to console

	$vSendPKT=0;			# send packets counter
	$vRecvPKT=0;			# receive packets counter
	$vLogStatCloseRow=0;		# const 
	$vLogStatOpenRow=1;		# const
	$vLogStat=$vLogStatCloseRow;

	initialize();
}
1;
########################################################################
__END__

=head1 NAME

V6evalTool - Perl interface to write sequence script for IPv6 evaluation.

=head1 SYNOPSIS

  use V6evalTool;

  vStop(Link0);
  vClear(Link0);
  vCapture(Link0);
  %ret= vSend(Link0, frameA, frameB, frameC );
  %ret= vRecv(Link0, 0, 0, 0, (frameX, frameY) );
  if( $ret{status}!=0 ){
	vLog(NG);
	exit 1;
  }
  %ret=vRemote(remote_file, options, args_list);
  vLog(OK);
  $error_message= vErrmsg(%ret);
  $val= vRoundoff(timeval);


=head1 ENVIRONMENT

Directory of IPv6 evaluation environment is set -vroot option.
In -vroot option, first is option setting direcory, second environment
variable C<V6EVALROOT>, third "/usr/local/v6eval".
All commands are assumed to be located at
<-vroot option directory>/bin directory.
Otherwise, this routines will not work.

=head1 DESCRIPTION

These routines allow you to write sequence script for IPv6 
evaluation. Only one sequence script can run at the same time.
There is no assurance for the case that multiple script run 
at the same time.

This module tries to catch SIGCHLD to detect termination of 
pktbuf process.
Since this handler calls wait() to get the process id of the
terminated child here, $? value will be invalid in case 
the user use wait() outside.
In this case, the user should get the staus by using
getChildStatus() function instead of looking up $? value.
Because of the same reason, if the user wants to get the 
process id of child calling wait(), the retruned value will
be also invalid. For this case, user can use getChildPid()
function after calling wait().

The name of the sequence script is assumed 'xxxx.seq', where xxxx 
represents the type of the evaluation test. 
'xxxx.seq' may have the following keyword parameters, 
and these are checked by this module at the package initialization.

                                                    < default value >
                                                    ------------------
  -tn 	    TN(Testing Node) configuration file     <dir>/tn.def
  -nut	    NUT(Node Under Test) configuration file <dir>/nut.def
  -pkt	    Packet definition file                  <dir>/packet.def
  -ti	    Test Title (text)                       <script>
  -log	    Log file                                <dir>/<script>.log
  -l  	    Log Level (0,1,2...)                    1
  -h  	    Help                                    N/A
  -v	    log file print to STDOUT                0
  -nostd    not include std.def			    0
  -vroot    v6eval directory			    /usr/local/v6eval
  -remote   options passed to a remote ctrl. file   N/A

  <dir>:       directory of the sequence script located
  <script>:    sequence script name without extntion

=head1 FILES

 TN(Testing Node) configuration file;
 NUT(Node Under Test) configuration file:
    These files read from
   	First   fullname -tn option sets
		option setting file
   	Second  only file name -tn option sets
		search paths order
		1 <dir>/xxxx
		2 current directory/xxxx
		3 <V6Root>/etc/xxxx
   	Third   -tn option does not set
		search paths order
		1 <dir>/tn.def
		2 current direcrory/tn.def
		3 <V6Root>/etc/tn.def

 Packet definition file:
    If -pkt option does not set
	<dir>/packet.def                                   
    is default file

 Standard Difinition file:
    If -nostd option does not set
	search paths order
	1 current directory/std.def
	2 <V6Root>/include/std.def

 Other include file:
    include order
	1 current directory
	2 <V6Root>/include/ directory

  <V6Root>: directory set by -vroot option
  <dir>:    directory of the sequence script located

=head1 SUPPORTED FUNCTIONS

=over 4

=head1 vSend

Send frame to the interface.

 sub vSend($@) { my (

	$ifname,	# target interface name
	@frames		# frame names to send
 ) = @_;

Returns hash value, and its information can be accessed
by the following keys;

=item status

 =0:	normal
 >0:	error 

=item sentTimeN

Time of the specified N'th frame pushed to the 
specified interface for sending. 
For example, the time for the 1st frame will be referenced 
by $retval{'sentTime1'}.
The value format is as follows;
 %ld[=epochtime].%06d[=microsecond]

=head1 vRecv

Receive frame from the interface.

 sub vRecv($$$$@) { my (
	$ifname,        # target interface name
	$timeout,       # expire time(second)
                	#       -1: No limitation
	$seektime,      # seek to the packet at the time(%ld.%06d)
                	#       0: seek from the beginning
                	#          of the captured buffer
                	#       You may use $retval{'sentTime(n)'}
                	#       returned by vSend().
	$count,         # How many frames to wait
                	#       0: No limitation
	@frames         # frame names to send
 ) = @_;

Returns hash value, and its information can be accessed
by the following keys;

=item status:

 =0:     normal
  1:     receive time  exceeded
  2:     receive count exceeded
 >3:     error

=item recvCount;

The number of frame received while calling this subroutine.

=item recvTime(N):

Time of the specified N'th frame received from the
specified interface.
For example, the time for the 1st frame will be referenced
by $retval{"recvTime1"}.
The value format is as follows;
 %ld[=epochtime].%06d[=microsecond]

=item recvFrame:

The name of the frame
This will be normally one of the name in the frames
specified in the argument. Otherwise, undef value
in case of receive time or count exceeded.

=head1 vCapture

Start frame capture for the interface.

 sub vCapture($) { my (
	$ifname,        # target interface name
 ) = @_;

vCapture returns scaler value which represents the status
 =0:     normal (even if already capturing)
 >0:     error

=head1 vStop

Stop frame capture for the interface.

 sub vStop($) { my (
	$ifname,        # target interface name
 ) = @_;

Returns returns scaler value which represents the status
 =0:     normal ( even if already stopped )
 >0:     error


=head1 vClear

Clear captured buffer for the interface.

 sub vClear($) { my (
	$ifname,        # target interface name
 ) = @_;

Returns scaler value which represents the status
 =0:     normal ( even if already cleared)
 >0:     error


=head1 vSleep

suspend process execution for interval of seconds

 sub vSleep($;$) { my (
	$sec,
	$mesg
 ) = @_;


=head1 vRemote

Execute a remote control program.

 sub vRemote($;$@) { my (
	$remtoe_file	# A file name of remote control program.
	$options	# Options that pass to the program directly.
			# See i/f of the remote control program.
        @argv		# Args that pass to the program directly
			# that depends on the remote control program.
 ) = @_;

Returns exit value that the remote control program shows.
 =0:	noral
 >0:	error (that depend on the remote control program)

=item FILE SEARCH PATH

 A remote control program is searched as the following directories:
   1. The current directory.
   2. $V6EVALROOT/bin if defined.
      Or /usr/local/v6eval/bin if $V6EVALROOT is not defined.

=item EVALUATING THE OPTIONS

 The options are evaluated the following order:
   1. The definitions in TN.def or NUT.def
   2. -remote="..." options.
   3. vRemote(, "...", ) options.

=item THE RELATION TO TN/NUT DIFINITION FILES

 A TN.def VS. a remote control program options:
 ---------------+-----------------------
 TN.def
 Entries         Mapped options
 ---------------+-----------------------
 RemoteDevice	 -d
 RemoteDebug	 -v
 RemoteIntDebug	 -i
 RemoteLog	 -o
 RemoteSpeed     -s
 ---------------+-----------------------

 A NUT.def VS. a remote control program optons:
 ---------------+-----------------------
 NUT.def
 Entries         Mapped optons
 ---------------+-----------------------
 System		 -t
 User		 -u
 Password	 -p
 ---------------+-----------------------

=item SEE ALSO

 V6evalRemote.pm (perldoc V6evalRemote)


=head1 vRemoteAsync

Asynchronous execution of a remote control program.

 sub vRemoteAsync($;$@) { my (
	$remtoe_file	# A file name of remote control program.
	$options	# Options that pass to the program directly.
			# See i/f of the remote control program.
        @argv		# Args that pass to the program directly
			# that depends on the remote control program.
 ) = @_;

Returns scaler value which represents the status
 =0:	normal
 >0:	error

This function forks and executes remote control program.
Main sequence program can wait and synchronize the executing remote
control program by vRemoteAsyncWait() function.
Multiple vRemoteAsync() execution is unavailable.
Executing vRemote() between vRemoteAsync() and vRemoteAsyncWait()
is also unavailable.
Details of other specification are same as vRemote().


=head1 vRemoteAsyncWait

Wait for an asynchronous remote control program executed by vRemoteAsync().

 sub vRemoteAsyncWait()

Returns exit value that the remote control program shows.
 =0:	normal
 >0:	error (that depend on the remote control program)




=head1 vLog

Output message to the log file

 sub vLog($) { my (
	$message,       # message to be logged
 ) = @_;

Returns nothing.

=head1 vErrmsg

Return error messages.

sub vErrmsg(%) { my (
	%stat		# return status
    ) = @_;

=head1 vRoundoff

Return time value that is rounded off to one decimal place

sub vRoundoff($) { my (
	$timeval		# sec.usec
    ) = @_;

=head1 vStatus

Get the status of packet buffering process.

 sub vStatus() { my (

	$ifname,	# target interface name
	@frames		# frame names to send
 ) = @_;

Returns hash value, and its information can be accessed
by the following keys;

=item status

 =0:	normal
 >0:	error 

=item  XXXX

not yet fiexed.

=back

=head1 vMAC2LLAddr

create link local address from MAC address

sub vMAC2LLAddr(%) { my (

	$MACaddr	# MAC address 
    ) = @_;

=head1 SEQUENCE SCRIPT EXAMPLE

  #!/usr/bin/perl

  use V6evalTool;

  #--------------------------------------------
  # Setup test environment
  #--------------------------------------------
  vStop(Link0);
  vClear(Link0);	
  vCapture(Link0);

  #--------------------------------------------
  # Send frame
  #--------------------------------------------
  %sret= vSend(Link0, frame1, frame2 );
  if( $sret{status}!=0 ){
  	vLog(NG);
  	exit 1;
  }
  print "sentTime1:\t$sret{sentTime1}\n";
  print "sentTime2:\t$sret{sentTime2}\n";

  #--------------------------------------------
  # log some site specific messages
  #--------------------------------------------
  vLog("Something you need" );

  #--------------------------------------------
  # Receive frame
  #--------------------------------------------
  %rret= vRecv(Link0, 10, $sret{sentTime1}, 5,(frame1,frame2) );
  if( $rret{status}!=0 ){
	vLog(NG);
	exit 1;
  }
  print "recvFrame:\t$rret{recvFrame}\n";

  #--------------------------------------------
  # Post process
  #--------------------------------------------
  vLog(OK);

=cut

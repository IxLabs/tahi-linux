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
########################################################################
package V6evalCommon;
use Exporter;
@ISA = qw(Exporter);
use File::Basename;

@EXPORT = qw(
	     pathinit
	     searchPath
	     $SeqName
	     $SeqDir
	     $V6EVALPREFX
	     $V6EVALROOT
	     $V6EVALBIN
	     $V6EVALETC
	     $V6EVALINC
	     );

sub pathinit(@);
sub searchPath($$);
#sub readTnDef($);
#sub readNutDef($);

########################################################################
#       Path Init
#-----------------------------------------------------------------------
sub pathinit(@) { my(
        $path,           # path  
    ) = @_; 
	($SeqName,$SeqDir,$SeqSuffix) = fileparse($0,'.seq');
	$V6EVALPREFIX= "/usr/local/v6eval";
	$V6EVALROOT  = $path            || $ENV{V6EVALROOT} || $V6EVALPREFIX;
	$V6EVALBIN   = $ENV{V6EVALBIN}  || "$V6EVALROOT/bin/";
	$V6EVALETC   = $ENV{V6EVALETC}  || "./:$V6EVALROOT/etc/";
	$V6EVALINC   = $ENV{V6EVALINC}  || "$SeqDir:./:$V6EVALROOT/include/";
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
                        $fullname=$tmpname if(-r $tmpname);
                }
        }
        $fullname;
}

__END__

sub readTnDef($) { my(
        $tn
    ) = @_; 
	#---------------------------------------------------------------
	# Check TN definition and setup environment
	#---------------------------------------------------------------
	my (%TnDef);
	prTrace("Looking up $tn");
	open(FILE,"$tn") || prErrExit("Can not open $tn");

	my $lines=0;
	while(<FILE>){
		$lines++;

		# skip comment line
		next if(/^\s*$/ || /^#/);

		#
		chomp;

		# check syntax
		if( /^(socketpath)\s+(\S+)/  ) {
			$SocketPath=$2;
		}
		elsif( /^(Link[0-9]+)\s+(\S+)\s+(([0-9a-f]{1,2}:){5}[0-9a-f]{1,2})/){
			$TnDef{$1."_device"}=$2;
			$TnDef{$1."_addr"}=$3;
		}
		elsif( /^(RemoteCuPath)\s+(\S+)/  ||
		       /^(RemoteDevice)\s+(\S+)/   ||
		       /^(RemoteDebug)\s+(\S+)/    ||
		       /^(RemoteIntDebug)\s+(\S+)/ ||
		       /^(RemoteLog)\s+(\S+)/      ||
		       /^(RemoteSpeed)\s+(\S+)/    ||
		       /^(RemoteLogout)\s+(\S+)/   ||
		       /^(RemoteMethod)\s+(\S+)/   ||
		       /^(filter)\s+(\S+)/){
		}
		else{ $TnDef{'error'} .= "line $lines : unknown directive $_\n";}

		# set
		if( /^(\S+)\s+(.*)/ ) {
			if($TnDef{$1}){
				$TnDef{'error'} .= "line $lines : duplicate entry $_\n";}
			} else {
				$TnDef{$1} = $2;
			}
		}
		prTrace("TN : $_");
	}
	close FILE;

	return %TnDef;
}

sub readNutDef($) { my(
        $nut
    ) = @_; 
	#---------------------------------------------------------------
	# Check NUT definition and setup environment
	#---------------------------------------------------------------
	my (%NutDef);
	prTrace("Looking up $nut");
	open(FILE, "$nut") || prErrExit("Can not open $nut");

	while(<FILE>){
		next if(/^\s*$/ || /^#/);	# remove comment
		chomp;
		if( /^(\S+)\s+(.*)/ )		{
		    $NutDef{$1} = $2;
		}
		if( /^(System)\s+(.*)/              ||
		    /^(TargetName)\s+(.*)/	    ||
		    /^(HostName)\s+(.*)/            ||
		    /^(Type)\s+(.*)/                ) {
		    prLog("<TR><TD>$1</TD><TD>$2</TD></TR>");
		} elsif(
		    /^(User)\s+(\S+)/               ||
		    /^(Password)\s+(\S+)/    ){
		}
		elsif( /^(Link[0-9]+)\s+(\S+)\s+(([0-9a-fA-F]{1,2}:){5}[0-9a-fA-F]{1,2})/){
			$NutDef{$1."_device"}=$2;
			$NutDef{$1."_addr"}=$3;
		}
		else{ $NutDef{'error'} .= "line $lines : unknown directive $_\n";}
		prTrace("NUT: $_");
	}
	close FILE;

	return %NutDef;
}

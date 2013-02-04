#
# Copyright (C) 2002, 2003, 2004, 2005 Yokogawa Electric Corporation, 
# INTAP(Interoperability Technology Association for Information 
# Processing, Japan), IPA (Information-technology Promotion Agency, Japan).
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
# $TAHI: v6eval/script/pmod/V6evalRemote/V6evalRemote.pm,v 1.142 2005/05/25 05:11:45 akisada Exp $
#
########################################################################

package V6evalRemote;
use Exporter;
@ISA = qw(Exporter);

use 5;
use Getopt::Std;
use Expect;

$debug=0;
$retry=2;
$exit_error=1;
$init_timeout=0.2;
$dummy_prompt='DUMMY_PROMPT';

@EXPORT = qw(
	     rType
	     rDevice
	     rUser
	     rUserPrompt
	     rPasswordPrompt
	     rCommandPrompt
	     rDebug
	     rExpInternal
	     rLogStdout
	     rSendSpeed
	     rCmdOutput
	     rEnableLogout
	     rOpen
	     rLogin
	     rLogout
	     rClose
	     rCommandAsync
	     rCommand
	     rRebootAsync
	     rReboot
	     rPutfile
	     rInit
	     rShutdown
	     rArg
	     );

########################################################################

# The following status values must be same as V6evalTool.pm's ones.

$exitPass=0;		# PASS
$exitNS=2;		# Not yet supported
$exitFail=32;		# FAIL
$exitFatal=64;		# FATAL

########################################################################
BEGIN
{
    $SIG{CHLD}=\&_sigchld_handler;
    $SIG{INT}=\&_sig_handler;
    $SIG{TERM}=\&_sig_handler;

    $Bye="~.";
    $Type="tahi-v6eval";
    $CU="cu";
    $Device="cuaa0";
    $User="root";
    $Password="v6eval";
    undef $Remote;
    $Expect::Log_Stdout=0;
    $SendSpeed=0;
    $EnableLogout=0;

    $CmdOutput="";

    # login prompt
    %prompt_user = (
		    'kame-freebsd',	'login: ',
		    'freebsd43-i386',	'login: ',
		    'hitachi-nr60',	'login:',
		    'hitachi-nr60-v2',	'login:',
		    'hitachi-gr2k',	'login:',
		    'hitachi-gr2k-beta',	'login:',
		    'nec-libra',	'login: ',
		    'nec-libra-utcpip',	'login: ',
		    'nec-mip6',		'login: ',
		    'nec-ix1k',		'login: ',
		    'sfc-mip6',		'login: ',
		    'yamaha-ws-one',	'(starts|Yamaha Corporation)',
		    'linux-v6',		'login: ',
		    'sun-solaris',	'login: ',
		    'ms-win2k',		'Login: ',
		    'cisco-ios',	'get started',   
		    'iij-seil',		'login: ',
		    'telebit-tbc2k',	'Username:',
		    'mgcs-sj6',		'login: ',
		    'netbsd-i386',	'login: ',
		    'kame-netbsd-i386',	'login: ',
		    'freebsd-i386',	'login: ',
		    'openbsd-i386',	'login: ',
		    'aix',		'login: ',
		    'hpux',		'login: ',
		    'usagi-i386',	'login: ',
		    'usagi24-i386',	'login: ',
		    'mipl',		'login: ',
		    'lcna',		'login: ',
		    'ttb3010',		'login: ',
		    'unitedlinux',		'login: ',
		   );

    # password prompt
    %prompt_password = (
			'kame-freebsd',		'Password:',
			'freebsd43-i386',	'Password:',
			'hitachi-nr60',		'Password:',
			'hitachi-nr60-v2',	'Password:',
			'hitachi-gr2k',		'Password:',
			'hitachi-gr2k-beta',	'Password:',
			'nec-libra',		'Password:',
			'nec-libra-utcpip',	'Password:',
			'nec-mip6',		'Password:',
			'nec-ix1k',		'Password:',
			'sfc-mip6',		'Password:',
			'yamaha-ws-one',	'Password:',
			'linux-v6',		'Password: ',
			'sun-solaris',		'Password:',
			'ms-win2k',		'Password:',
			'cisco-ios',		'Password:',
			'iij-seil',		'DUMMY_PROMPT',	# dummy
			'telebit-tbc2k',	'Password:',
			'mgcs-sj6',		'Password:',
			'netbsd-i386',		'Password:',
			'kame-netbsd-i386',	'Password:',
			'freebsd-i386',		'Password:',
			'openbsd-i386',		'Password:',
			'aix',			'Password:',
			'hpux',			'Password:',
			'usagi-i386',		'Password: ',
			'usagi24-i386',		'Password: ',
			'mipl',			'Password: ',
			'lcna',			'Password:',
			'ttb3010',		'Password:',
			'unitedlinux',		'Password: ',
			);

    # command prompt
    %prompt_command = (
		       'kame-freebsd',		'(\$|#) ',
		       'freebsd43-i386',	'(\$|#) ',
		       'hitachi-nr60',		'% ',
		       'hitachi-nr60-v2',	'% ',
		       'hitachi-gr2k',		'(command|admin)(:|>) ',
		       'hitachi-gr2k-beta',	'(command|admin): ',
		       'nec-libra',		'(\$|#) ',
		       'nec-libra-utcpip',	'(\$|#) ',
		       'nec-mip6',		'(\$|#) ',
		       'nec-ix1k',		'(\$|#) ',
		       'sfc-mip6',		'(\$|#) ',
		       'yamaha-ws-one',		'(>|#) ',
		       'linux-v6',		'# ',
		       'sun-solaris',		'# ',
		       'ms-win2k',		'.:.>',
		       'cisco-ios',		'(>|#)',
		       'iij-seil',		'\r(>|#) ',
		       'telebit-tbc2k',		'(> |% |accepted)',
		       'mgcs-sj6',		'(\$|#) ',
		       'netbsd-i386',		'(\$|#) ',
		       'kame-netbsd-i386',	'(\$|#) ',
		       'freebsd-i386',		'(\$|#) ',
		       'openbsd-i386',		'(\$|#) ',
		       'aix',			'(\$|#) ',
		       'hpux',			'(\$|#) ',
		       'usagi-i386',		'# ',
		       'usagi24-i386',		'# ',
		       'mipl',			'# ',
		       'lcna',			'(\$|#) ',
		       'ttb3010',		'(>|#) ',
		       'unitedlinux',	'# ',
		       );

    # command of the admin mode if any
    %cmd_admin = (
		  'hitachi-gr2k',  'admin',
		  'hitachi-gr2k-beta',  'admin',
		  'yamaha-ws-one', 'administrator',
		  'cisco-ios',     'enable',
		  'iij-seil',      '!sh',
		  'telebit-tbc2k', "\esystem",
		  'ttb3010', 	'config',
		  );

    # another command of the admin mode if any
    %cmd_admin2 = (
		   'telebit-tbc2k', "\epaxtcl0\r",
		   );

    # logout command
    %cmd_exit = (
		 'kame-freebsd',	'exit',
		 'freebsd43-i386',	'exit',
		 'hitachi-nr60',	'exit',
		 'hitachi-nr60-v2',	'exit',
		 'hitachi-gr2k',	'exit',
		 'hitachi-gr2k-beta',	'exit',
		 'nec-libra',		'exit',
		 'nec-libra-utcpip',	'exit',
		 'nec-mip6',		'exit',
		 'nec-ix1k',		'exit',
		 'sfc-mip6',		'exit',
		 'yamaha-ws-one',	'exit',
		 'linux-v6',		'exit',
		 'sun-solaris',		'exit',
		 'ms-win2k',		'exit',
		 'cisco-ios',		'exit',
		 'iij-seil',		'exit',
		 'telebit-tbc2k',	"\elogout\r",
		 'mgcs-sj6',		'exit',
		 'netbsd-i386',		'exit',
		 'kame-netbsd-i386',	'exit',
		 'freebsd-i386',	'exit',
		 'openbsd-i386',	'exit',
		 'aix',			'exit',
		 'hpux',		'exit',
		 'usagi-i386',		'exit',
		 'usagi24-i386',	'exit',
		 'mipl',		'exit',
		 'lcna',		'exit',
		 'ttb3010',		'logout',
		 'unitedlinux',		'exit',
		 );

    # confirmation for logout
    %prompt_exit_confirm = (
			    'ms-win2k',	'ENTER',
			    );

    # reply of the confirmation for logout
    %reply_exit_confirm = (
			   'ms-win2k',		"\r",
			   );

    # command for initialize
    %cmd_init = (
		  'ttb3010', 	'init all',
		  );

    # confirmation for Initialize
    %prompt_init_confirm = (
			    'ttb3010',	"y/n",
			    );

    # reply of the confirmation for Initialize
    %reply_init_confirm = (
			   'ttb3010',	"y",
			   );

    # command for shutdown
    %cmd_shutdown = (
		  'ttb3010', 	"shutdown",
		  );

    # confirmation for Initialize
    %prompt_shutdown_confirm = (
			    # TTB asks [y/n]
			    'ttb3010',	"y/n",
			    );

    # reply of the confirmation for Initialize
    %reply_shutdown_confirm = (
			   'ttb3010',		"y",
			   );


    # reboot command
    %cmd_reboot = (
		   'kame-freebsd',	'reboot',
		   'freebsd43-i386',	'reboot',
		   'hitachi-nr60',	'reboot',
		   'hitachi-nr60-v2',	'reboot',
		   'hitachi-gr2k',	'boot -nf',
		   'hitachi-gr2k-beta',	'boot -nf',
		   'nec-libra',		'reboot',
		   'nec-libra-utcpip',	'reboot',
		   'nec-mip6',		'reboot',
		   'nec-ix1k',		'reload y',
		   'sfc-mip6',		'reboot',
		   'yamaha-ws-one',	'restart',
		   'linux-v6',		'reboot',
		   'sun-solaris',	'reboot',
		   # ms-win2k
		   'cisco-ios',		'reload',
		   'iij-seil',		'reboot',
		   'telebit-tbc2k',	'restart',
		   'mgcs-sj6',		'reboot',
		   'netbsd-i386',	'reboot',
		   'kame-netbsd-i386',	'reboot',
		   'freebsd-i386',	'reboot',
		   'openbsd-i386',	'reboot',
		   'aix',		'reboot',
		   'hpux',		'reboot',
		   'usagi-i386',	'reboot',
		   'usagi24-i386',	'reboot',
		   'mipl',		'reboot',
		   'lcna',		'reboot',
		   'ttb3010',		'reboot',
		   'unitedlinux',	'reboot',
		   );

    # final confirmation for reboot if any
    %prompt_reboot_confirm = (
			      'ms-win2k',	'ENTER',
			      # Proceed with reload? [confirm]
			      'cisco-ios',	'confirm',
			      # Proceed with init? [y/n]
			      'ttb3010',	'y/n',
			      );

    # reply of the final confirmation for reboot
    %reply_reboot_confirm = (
			     'ms-win2k',	"\r",
			     'cisco-ios',	'y',
			     'ttb3010',		'y',
			     );

    # pre-final confirmation for reboot if any
    %prompt_reboot_confirm2 = (
			       # System configuration has been modified.
			       # Save? [yes/no]:
			      'cisco-ios',	'yes/no',
			      );

    # reply of the pre-final confirmation for reboot
    %reply_reboot_confirm2 = (
			     'cisco-ios',	"no\r",
			     );

    # final confirmation for shutdown if any
    %prompt_shutdown_confirm = (
			      # Proceed with init? [y/n]
			      'ttb3010',	'y/n',
			      );

    # reply of the final confirmation for reboot
    %reply_shutdown_confirm = (
			     'ttb3010',		'y',
			     );

    # command for reading command status
    %cmd_status = (
		   'kame-freebsd',	'echo $status',
		   'freebsd43-i386',	'echo $status',
		   'hitachi-nr60',	'echo $status',
		   'hitachi-nr60-v2',	'echo $status',
		   'hitachi-gr2k',	'echo $status',
		   'hitachi-gr2k-beta',	'echo $status',
		   'nec-libra',		'echo $status',
		   'nec-libra-utcpip',	'echo $status',
		   'nec-mip6',		'echo $status',
		   'sfc-mip6',		'echo $status',
		   'linux-v6',		'echo $?',
		   'sun-solaris',	'echo $?',
		   # ms-win2k
		   # cisco-ios
		   # iij-seil
		   # ttb3010
		   'mgcs-sj6',		'echo $status',
		   'netbsd-i386',	'echo $status',
		   'kame-netbsd-i386',	'echo $status',
		   'freebsd-i386',	'echo $status',
		   'openbsd-i386',	'echo $status',
		   'aix',		'echo $?',
		   'hpux',		'echo $?',
		   'usagi-i386',	'echo $?',
		   'usagi24-i386',	'echo $?',
		   'mipl',		'echo $?',
		   'lcna',		'echo $status',
		   'unitedlinux',	'echo $?',
		   );
}

########################################################################
END
{
    rClose();
}

########################################################################
sub _sig_handler
{
    exit $exitFatal;
    # call END for rClose()
}

########################################################################
sub _sigchld_handler
{
    $! = 0;
    $? = ($exit_error << 8);
    print STDERR "``$CU -l $Device'' was killed with SIGCHLD.\n";
    exit $exitFatal;
}

########################################################################
sub rType(;$)
{
    my($x)=$Type;
    $Type=shift(@_) if @_ != ();
    $x;
}

########################################################################
sub rDevice(;$)
{
    my($x)=$Device;
    $Device=shift(@_) if @_ != ();
    $x;
}

########################################################################
sub rUser(;$)
{
    my($x)=$User;
    $User=shift(@_) if @_ != ();
    $x;
}

########################################################################
sub rUserPrompt(;$)
{
    my($x)=$prompt_user{$Type};
    $prompt_user{$Type}=shift(@_) if @_ != ();
    $x;
}

########################################################################
sub rPasswordPrompt(;$)
{
    my($x)=$prompt_password{$Type};
    $prompt_password{$Type}=shift(@_) if @_ != ();
    $x;
}

########################################################################
sub rCommandPrompt(;$)
{
    my($x)=$prompt_command{$Type};
    $prompt_command{$Type}=shift(@_) if @_ != ();
    $x;
}

########################################################################
sub rDebug(;$)
{
#   return undef if $Remote == undef;
    return undef if (!defined $Remote);
    my($x)=$Remote->debug();
    $Remote->debug(shift(@_)) if @_ != ();
    $x;
}

########################################################################
sub rExpInternal(;$)
{
#   return undef if $Remote == undef;
    return undef if (!defined $Remote);

    my($x)=$Remote->exp_internal();
    $Remote->exp_internal(shift(@_)) if @_ != ();
    $x;
}

########################################################################
sub rLogStdout(;$)
{
#   return undef if $Remote == undef;
    return undef if (!defind $Remote);

    my($x)=$Remote->log_stdout();
    $Remote->log_stdout(shift(@_)) if @_ != ();
    $x
}

########################################################################
sub rSendSpeed(;$)
{
    my($x)=$SendSpeed;
    $SendSpeed=shift(@_) if @_ != ();
    $x;
}

########################################################################
sub rCmdOutput()
{
    my(@x);
    @x=split(/\n/, $CmdOutput);
}

########################################################################
sub rEnableLogout(;$)
{
    my($x)=$EnableLogout;
#   $EnableLogout=shift(@_) if @_ != ();
    $EnableLogout=shift(@_) if (@_ > 0);
    $x;
}

########################################################################
sub rOpen()
{
#   return undef if $Remote != undef;
    return undef if (defined $Remote);

    autoflush STDOUT 1;

    getopts('t:T:d:u:p:v:i:o:U:P:C:Vs:l:h');

    if($opt_h) {
	usage();
	exit 0;
    }

    $Type=$opt_t if defined($opt_t);	# target type
    $CU=$opt_T if defined($opt_T);	# cu command path
    $Device=$opt_d if defined($opt_d);	# serial device name
    $User=$opt_u if defined($opt_u);	# user name
    $Password=$opt_p if defined($opt_p);	# password

    $prompt_user{$Type}=$opt_U if defined($opt_U);	# login prompt
    $prompt_password{$Type}=$opt_P if defined($opt_P);	# password prompt
    $prompt_command{$Type}=$opt_C if defined($opt_C);	# command prompt

    $debug = 1 if defined($opt_V);		# debug flag
    $SendSpeed=$opt_s if defined($opt_s);	# send speed
    $EnableLogout=$opt_l if defined($opt_l);	# enable logout

    my $TermCmd = "$CU -l $Device";
    $Remote = Expect->spawn("$TermCmd");
#   if($Remote == undef) {
    if(!defined $Remote) {
	print STDERR "$TermCmd exec failed\n";
	return undef;
    }

    $Remote->debug($opt_v) if defined($opt_v);	# debug level
    $Remote->exp_internal($opt_i) if defined($opt_i);	# internal debug level
    $Remote->log_stdout($opt_o) if defined($opt_o);	# stdout

    my($v, $lval, $rval);
    while($v = shift(@ARGV)) {
        ($lval, $rval) = split(/=/, $v, 2);
        $rval=1 if $rval =~ /^\s*$/;
        $v='$main::rOpt_'."$lval".'=\''."$rval".'\'';
        eval($v);       # eval ``$main::rOpt<LVAL>=<RVAL>''
	print STDERR "eval $v\n" if $debug;
    }

#   if($Remote->expect(4, "onnect") == undef) {
    if(!defined $Remote->expect(4, "onnect")) {
	print STDERR "Unable to sync with cu\n";
	return undef;
    }
    $Remote;
}

sub usage()
{
    print "V6evalRemote.pm system options:\n".
	  "  -h\n".
	  "  -t<target_type>\n".
	  "  -T<cu command path>\n".
	  "  -d<serial_device_name>\n".
	  "  -u<user_name>\n".
	  "  -p<password>\n".
	  "  -U<login_prompt>\n".
	  "  -P<password_prompt>\n".
	  "  -C<command_prompt>\n".
	  "  -v<debug level>\n".
	  "  -i<internal debug level>\n".
	  "  -o<stdout>\n".
	  "  -V\n".
	  "  -s<interval>\n".
	  "  -l<enable_logout>\n";
}

########################################################################
#
#
#	START
#	  |
#	  | <----- A
#	  |
#	Retry out? ---> ERROR
#	  |
#	  |
#	wait prompt ---------------------------+----+----> timeout
#	  |                                    |    |        OR
#	  | login prompt                       |    |      command prompt &&
#	  |                                    |    |      logout
#	put user_name                          |    |       THEN
#	  |                                    |    |      goto A
#	  |                                    |    |
#	wait prompt ----> timeout THEN goto A  |    |
#	  |                                    |    |
#	  | password prompt                    |    |
#	  |                                    |    |
#	  | <----------------------------------+    |
#	  |  password prompt && !logout             |
#	  |                                         |
#	put password                                |
#	  |                                         |
#	  |                                         |
#	wait prompt ----> timeout THEN goto A       |
#	  |                                         |
#	  |  command prompt                         |
#	  |                                         |
#	  | <---------------------------------------+
#	  |  command prompt
#         |
#	 END
#

sub rLogin($)
{
    my($timeout)=@_;
    my($retry)=8;
    my($i)=0;
    my($t)=$init_timeout;
    my($is_admin)=0;
    my($is_admin2)=0;

    ### $timeout=50;    # special patch for long login timeout (such as netbsd-i386)

#   if($Remote == undef) {
    if(!defined $Remote) {
	print STDERR "rOpen() should be called first.\n";
	return 0;
    }

    if($debug) {
	print STDERR "prompt_user: ``$prompt_user{$Type}'', ".
	    "prompt_password: ``$prompt_password{$Type}'', ".
		"prompt_command: ``$prompt_command{$Type}''\n";
    }

    while($i<$retry) {
	#
	# wait for login prompt
	#
	print STDERR "rLogin: Wait for login prompt ($t sec)\n" if $debug;
#	next if $Remote->expect($t,
#				'-re', "$prompt_user{$Type}",
#				'-re', "$prompt_password{$Type}",
#				'-re', "$prompt_command{$Type}"
#				) == undef;
	next if (!defined $Remote->expect($t,
				'-re', "$prompt_user{$Type}",
				'-re', "$prompt_password{$Type}",
				'-re', "$prompt_command{$Type}"
					  ));
	$t=$timeout;
	my($num)=$Remote->exp_match_number();
	if($num == 1) {
	    #
	    # got login prompt
	    #
	    print STDERR "rLogin: Got login prompt\n" if $debug;
	    sendMessages("$User\r"); # put user name
	} elsif($num == 2) {
	    #
	    # got password prompt
	    #
	    print STDERR "rLogin: Got password prompt\n" if $debug;
	    goto password;
	} elsif($num == 3) {
	    #
	    # got command prompt
	    #
	    print STDERR "rLogin: Got command prompt\n" if $debug;
	    if(!$EnableLogout || $is_admin || $is_admin2) {
		goto command;
	    } else {
		rLogout($timeout);
		next;
	    }
	}

	#
	# wait for password prompt
	#
	if($prompt_password{$Type} eq $dummy_prompt) {
	    print STDERR "rlogin: password prompt is not defined\n"
		if $debug;
	} else {
	    print STDERR "rLogin: Wait for password prompt ($timeout sec)\n"
		if $debug;
#	    next if $Remote->expect($timeout,
#				    '-re', "$prompt_password{$Type}",
#				    ) == undef;
	    next if (!defined $Remote->expect($timeout,
				    '-re', "$prompt_password{$Type}",
					      ));
	}
      password:
	if($prompt_password{$Type} ne $dummy_prompt) {
	    print STDERR "rLogin: Got password prompt\n" if $debug;
	    sendMessages("$Password\r"); # put password
	}

	#
	# wait for command prompt
	#
	print STDERR "rLogin: Wait for command prompt ($timeout sec)\n"
	    if $debug;
#	next if $Remote->expect($timeout,
#				'-re', "$prompt_command{$Type}"
#				) == undef;
	next if (!defined $Remote->expect($timeout,
				'-re', "$prompt_command{$Type}"
					  ));
      command:
	print STDERR "rLogin: Got command prompt\n" if $debug;

	#
	#
	#
	if($is_admin == 0 && $cmd_admin{$Type}) {
	    $is_admin = 1;
	    print STDERR "rLogin: Need admin command: $cmd_admin{$Type}\n" if $debug;
	    sendMessages("$cmd_admin{$Type}\r");
	    redo;
	}

	#
	#
	#
	if($is_admin2 == 0 && $cmd_admin2{$Type}) {
	    $is_admin2 = 1;
	    print STDERR "rLogin: Need admin command2\n" if $debug;
	    sendMessages("$cmd_admin2{$Type}\r");
	    redo;
	}

	last;

    } continue {
	#
	# timeout: never got any prompt
	#
	print STDERR "rLogin: Never got prompt; try again\n" if $debug;
	sendMessages("\r");
	$t=$timeout;
	$i++;
    }

    if($i >= $retry) {
	#
	# Retry out
	#
	print STDERR "rLogin: Retry error (> $retry)\n";
	print STDERR getOutput();
	return 0;
    }

    #
    # login
    #
    return 1;
}

########################################################################
#
#	START
#	  |
#	  | <------- A
#	  |
#	Retry out ? -----> ERROR
#	  |
#	wait for prompt ------------+----------------------+
#	  |                         |                      |
#	  | login prompt OR         | command prompt       | timeout
#	  | passowrd prompt         |                      |
#	  |                  put exit command              |
#	 END                        |                      |
#                                goto A                  goto A
#

sub rLogout($)
{
    my($timeout)=@_;
    my($retry)=6;
    my($i)=0;
    my($t)=$init_timeout;

    if(!defined $Remote) {
	print STDERR "rOpen() should be called first.\n";
	return 0;
    }

    if($debug) {
	print STDERR "prompt_user: ``$prompt_user{$Type}'', ".
	    "prompt_password: ``$prompt_password{$Type}'', ".
		"prompt_command: ``$prompt_command{$Type}''\n";
    }

    #
    # if $EnableLogout == 0 then keep login
    #
    if($EnableLogout == 0) {
	print STDERR "rLogout: skip by not specifing ``-l'' $opt_l\n"
	    if $debug;
	return 1;
    }

    my(@ptn)= (
	       '-re', "$prompt_user{$Type}",
	       '-re', "$prompt_password{$Type}",
	       '-re', "$prompt_command{$Type}"
	       );
    push(@ptn, '-re', $prompt_exit_confirm{$Type})
	if $prompt_exit_confirm{$Type};

    while($i<$retry) {
	#
	# wait for login prompt
	#
	print STDERR "rLogout: Wait for prompt ($t sec)\n" if $debug;
#	next if $Remote->expect($t, @ptn) == undef;
	next if (!defined $Remote->expect($t, @ptn));
	$t=$timeout;
	my($num)=$Remote->exp_match_number();
	if($num == 1) {
	    #
	    # got login prompt
	    #
	    print STDERR "rLogout: Got login prompt\n" if $debug;
	    last;
	} elsif($num == 2) {
	    #
	    # got password prompt
	    #
	    print STDERR "rLogout: Got password prompt\n" if $debug;
	    last;
	} elsif($num == 3) {
	    #
	    # got command prompt
	    #
	    print STDERR "rLogout: Got command prompt\n" if $debug;
	    sendMessagesSync("$cmd_exit{$Type}\r");
	    $i++;
	    redo;
	} elsif ($num == 4) {
	    #
	    # got confirmation message
	    #
	    print STDERR "rLogout: Got confirmation prompt\n" if $debug;
	    sendMessages("$reply_exit_confirm{$Type}");
	    $i++;
	    redo;
	}
    } continue {
	#
	# timeout: never got any prompt
	#
	print STDERR "rLogout: Never got prompt; try again\n" if $debug;
	sendMessages("\r");
	$t=$timeout;
	$i++;
    }

    if($i >= $retry) {
	#
	# Retry out
	#
	print STDERR "rLogout: Retry error (> $retry)\n";
	print STDERR getOutput();
	return 0;
    }

    #
    # logout
    #
    return 1;
}

########################################################################
sub rClose()
{
#   return 0 if $Remote == undef;
    return 0 if (!defined $Remote);
    $SIG{CHLD}='DEFAULT';
    
    print $Remote "$Bye\r";
    print $Remote "$Bye\r";
    print $Remote "$Bye\r";
    $Remote->soft_close();
    undef $Remote;
    1;
}

########################################################################
sub _rCommand($$$)
{
    my($cmd, $timeout, $sync)=@_;
    my($i);

    if(!$prompt_command{$Type}) {
	print STDERR "$Type: Not defining the patterns of the prompts\n";
	return 0;
    }

#   return 0 if $Remote == undef;
    return 0 if (!defined $Remote);

    for($i=0, $t=$init_timeout; $i<$retry; $i++) {
	print STDERR "_rCommand: Try to get command prompt ($t sec.)\n"
	    if $debug;
#	last if $Remote->expect($t, '-re', "$prompt_command{$Type}") != undef;
	last if (defined $Remote->expect($t, '-re', "$prompt_command{$Type}"));
	#
	# Try to get any prompt
	#
	sendMessages("\r");
	$t = $timeout;
    }
    if($i >= $retry) {
	#
	# retry error
	#
	print STDERR "_rCommand: never got command prompt\n";
	print STDERR getOutput();
	return 0;
    }
    print STDERR "_rCommand: Do ``$cmd'' command\n" if $debug;
    if($sync) {
	sendMessagesSync("$cmd\r");
    } else {
	sendMessages("$cmd\r");
    }
    return 3;
}

########################################################################
sub rCommandAsync($$)
{
    my($cmd, $timeout)=@_;
    return _rCommand($cmd, $timeout, 0);
}

########################################################################
sub rCommand($$)
{
    my($cmd, $timeout)=@_;
    my($i, $s, $t);

    return 0 if _rCommand($cmd, $timeout, 1) == 0;

    for($i=0, $t=$init_timeout; $i<$retry; $i++) {
	print STDERR "rCommand: Try to get command prompt ($t sec)\n"
	    if $debug;
#       last if $Remote->expect($t, '-re', "$prompt_command{$Type}") != undef;
        last if (defined $Remote->expect($t, '-re', "$prompt_command{$Type}"));
	sendMessages("\r");
	$t = $timeout;
    }
    if($i >= $retry) {
	#
	# retry error
	#
	print STDERR "rCommand: never got command exit status\n" if $debug;
	return 3;
    }
    $CmdOutput=$Remote->exp_before();
    print STDERR "rCommand: CmdOutput=``$CmdOutput''\n" if $debug;

    if($cmd_status{$Type}) {
	# Invoke for getting exit status
	sendMessagesSync("$cmd_status{$Type}\r");

	# Get exit status
#	if($Remote->expect($timeout, '-re', '[0-9]+') != undef) {
	if(defined $Remote->expect($timeout, '-re', '[0-9]+')) {
	    $s = $Remote->exp_match();
	    print STDERR "rCommand: exit status: $s\n" if $debug;
	    return (($s == 0) ? 1 : 2);
	}
    }
    print STDERR "rCommand: never got command exit status\n" if $debug;
    return 3;
}

########################################################################
sub rRebootAsync($)
{
    my($timeout)=@_;
    my($i);

    if(0) {
	if(!$prompt_command{$Type}) {
	    print STDERR "$Type: Not defining the patterns of the prompts\n";
	    return 0;
	}

#	return 0 if $Remote == undef;
	return 0 if (!defined $Remote);

	my($cmd)=$cmd_reboot{$Type};

	for($i=0, $t=$init_timeout; $i<$retry; $i++) {
	    print STDERR "rRebootAsync: Try to get command prompt ($t sec.)\n"
		if $debug;
#	    last if
#		$Remote->expect($t, '-re', "$prompt_command{$Type}") != undef;
	    last if (defined $Remote->expect($t,
					     '-re',
					     "$prompt_command{$Type}"
					     ));
	    #
	    # Try to get any prompt
	    #
	    sendMessages("\r");
	    $t = $timeout;
	}
	if($i >= $retry) {
	    #
	    # retry error
	    #
	    print STDERR "rRebootAsync: never got command prompt\n";
	    print STDERR getOutput();
	    return 0;
	}
	print STDERR "rRebootAsync: Do ``$cmd'' command\n" if $debug;
	sendMessagesSync("$cmd\r");
    }
    my($cmd)=$cmd_reboot{$Type};
    return 0 if rCommandAsync($cmd, $timeout) == 0;

    my(@ptn)=();
    push(@ptn, '-re', "$prompt_reboot_confirm{$Type}")
	if $prompt_reboot_confirm{$Type};
    push(@ptn, '-re', "$prompt_reboot_confirm2{$Type}")
	if $prompt_reboot_confirm2{$Type};

    if(@ptn > 0) {
	while(1) {
# 	    if($Remote->expect($timeout,
#			       @ptn
#			       ) == undef) {
 	    if(!defined $Remote->expect($timeout,
			       @ptn
					)) {
		print STDERR
		    "Did not get any confirmation message in reboot\n";
		return 0;
	    }
	    my($num)=$Remote->exp_match_number();
	    if($num == 1) {
		print STDERR "rRebootAsync: Got reboot prompt #1\n" if $debug;
		sendMessages("$reply_reboot_confirm{$Type}");
		last;
	    } elsif($num == 2) {
		print STDERR "rRebootAsync: Got reboot prompt #2\n" if $debug;
		sendMessages("$reply_reboot_confirm2{$Type}");
		# try again
	    } else {
		print STDERR "rRebootAsync: Internal error\n";
		return 0;
	    }
	}
    }

    return 1;
}

########################################################################
sub rReboot($)
{
    my($timeout) = @_;
    my($i, $t);

    if(!$prompt_user{$Type}) {
	printf STDERR "$Type: Not defining the pattern of the prompts\n";
	return 0
    }
    if($Type eq 'telebit-tbc2k') {
	printf STDERR "rReboot: $Type: Does not support\n";
	return 0
    }

    #
    # Do reboot command
    #
    rRebootAsync(5) || return 0;

    #
    # The login again.
    #
    for($i=0, $t=$init_timeout; $i<$retry; $i++) {
	print STDERR "rReboot: Try to get login prompt ($t sec)\n"
	    if $debug;
#	return 1 if $Remote->expect($t,
#				    '-re', "$prompt_user{$Type}",
#				    ) != undef;
	return 1 if (defined $Remote->expect($t,
				    '-re', "$prompt_user{$Type}",
					     ));
	sendMessages("\r");
	$t = $timeout;
    }
    print STDERR "rReboot: never got login prompt\n";
    print STDERR getOutput();
    return 0;
}

########################################################################
sub rPutfile($$$)
{
    my($from, $to, $timeout)=@_;

    setTipVar($timeout, 'echocheck') || goto error;
    print STDERR "rPutfile: Starting copying\n" if $debug;

	unless(get_prompt($timeout)) {
		goto error;
	}

	sendMessages("~p");
	sendMessages("$from $to\r");

	unless(get_prompt($timeout)) {
		goto error;
	}
    #
    #
    #
    sendMessages("\r"); # it's important if prompt pattern is ``^(>|#) ''
#   if($Remote->expect($timeout, '-re', "$prompt_command{$Type}") == undef) {
    if(!defined $Remote->expect($timeout, '-re', "$prompt_command{$Type}")) {
	print STDERR "Never sync with copy\n";
	goto error;
    }
    print STDERR "rPutfile: Copying completed\n" if $debug;

    #
    #
    #
    return(1);

error:
    return(0);
}

########################################################################
sub getOutput()
{
    my($s);

    $s = sprintf("Before: ``%s''\nMatch: ``%s''\nAfter: ``%s''\n",
		 $Remote->exp_before(),
		 $Remote->exp_match(),
		 $Remote->exp_after()
		 );
}

########################################################################
sub sendMessages(@)
{
    my(@strings)=@_;

    foreach(@strings) {
	if($SendSpeed == 0){
	    print $Remote $_;
	} else {
	    $Remote-> send_slow($SendSpeed, $_);
	}
    }
}

########################################################################
sub sendMessagesSync(@)
{
    my(@strings)=@_;
    my($timeout)=5;

    foreach(@strings) {
	if($SendSpeed == 0){
	    print $Remote $_;
	} else {
	    $Remote-> send_slow($SendSpeed, $_);
	}

	# Sync with command echo back
	# It is important if a target is WIN2K
#	if($Remote->expect($timeout, "$_") == undef) {
	if(!defined $Remote->expect($timeout, "$_")) {
	    print STDERR "sendMessagesSync: never got $_\n";
	} else {
	    print STDERR "sendMessagesSync: got echo back of $_\n"
		if $debug;
	}
    }
}

########################################################################
sub setTipVar($@)
{
    my($timeout, @var)=@_;

    foreach(@var) {
	my($v)=$_;

	unless(get_prompt($timeout)) {
		return(0);
	}

	sendMessages("~s");
	sendMessages("$v\r");

	unless(get_prompt($timeout)) {
		return(0);
	}
    }
    return 1;
}

########################################################################
sub rInit($)
{
    my($timeout)=@_;
    my($i);

    my($cmd)=$cmd_init{$Type};
    return 0 if rCommandAsync($cmd, $timeout) == 0;

    my(@ptn)=();
    push(@ptn, '-re', "$prompt_init_confirm{$Type}")
	if $prompt_init_confirm{$Type};
    push(@ptn, '-re', "$prompt_command{$Type}")
	if $prompt_command{$Type};

    if(@ptn > 0) {
	while(1) {
# 	    if($Remote->expect($timeout,
#			       @ptn
#			       ) == undef) {
 	    if(!defined $Remote->expect($timeout,
			       @ptn
					)) {
		print STDERR
		    "Did not get any confirmation message in initialize\n";
		return 0;
	    }
	    my($num)=$Remote->exp_match_number();
	    if($num == 1) {
		print STDERR "rInit: Got confirm prompt #1\n" if $debug;
		sendMessages("$reply_init_confirm{$Type}\r");
	    } elsif($num == 2) {
		print STDERR "rInit: Got Command prompt #2\n" if $debug;
		sendMessages("$cmd_exit{$Type}\r");
		last;
	    } else {
		print STDERR "rInit: Internal error\n";
		return 0;
	    }
	}
    }

    return 1;
}

########################################################################
sub rShutdown($)
{
    my($timeout)=@_;
    my($i);

    my($cmd)=$cmd_shutdown{$Type};
    return 0 if rCommandAsync($cmd, $timeout) == 0;

    my(@ptn)=();
    push(@ptn, '-re', "$prompt_shutdown_confirm{$Type}")
	if $prompt_shutdown_confirm{$Type};

    if(@ptn > 0) {
	while(1) {
# 	    if($Remote->expect($timeout,
#			       @ptn
#			       ) == undef) {
 	    if(!defined $Remote->expect($timeout,
			       @ptn
					)) {
		print STDERR
		    "Did not get any confirmation message in shutdown\n";
		return 0;
	    }
	    my($num)=$Remote->exp_match_number();
	    if($num == 1) {
		print STDERR "rShutdown: Got confirm prompt #1\n" if $debug;
		sendMessages("$reply_shutdown_confirm{$Type}\n");
		last;
	    } else {
		print STDERR "rShutdown: Internal error\n";
		return 0;
	    }
	}
    }

    return 1;
}

########################################################################
sub get_prompt($)
{
	my ($timeout) = @_;

	my $count = 0;

	for(my $d = 0; $d < $timeout; $d ++) {
		if(defined($Remote->expect(1,
			'-re', "$prompt_command{$Type}"))) {

			$count ++;
			last;
		}

		sendMessages("\r");
	}

	return($count);
}

########################################################################
#rArg()		for pass argments to *.rmt
#		return:	always 1
#######################################
sub rArg() 
{
    autoflush STDOUT 1;

    my($v, $lval, $rval);
    while($v = shift(@ARGV)) {
        ($lval, $rval) = split(/=/, $v, 2);
        $rval=1 if $rval =~ /^\s*$/;
        $v='$main::rOpt_'."$lval".'=\''."$rval".'\'';
        eval($v);       # eval ``$main::rOpt<LVAL>=<RVAL>''
	print STDERR "eval $v\n" if $debug;
    }
    return 1;
}

########################################################################
1;

########################################################################
__END__

=head1 NAME

V6evalRemote - Perl interface to control NUT via serial line.

=head1 SYNOPSIS

=head1 DESCRIPTION

=head1 INTERFACE OF REMOTE CONTROL SCRIPT

                              <Default>      <Descriptions>
                              -----------    -----------------------
  -t<target_type>             kame-freebsd   kame-freebsd
                                             hitachi-nr60
                                             nec-libra
                                             nec-libra-utcpip
                                             yamaha-ws-one
  -T<cu_command_path>         cu             cu command path
  -d<serial_device_name>      cuaa0c         see cu(1)
  -u<user_name>               root           user name
  -p<password>                v6eval         user password
  -U<login_prompt>            'ogin: '       Regular-expression
  -P<password_prompt>         'assword:'     Reqular-expression
  -C<command_prompt>          '(#|\$|%|>) '  Reqular-expression
  -v<debug level>             0              $Expect::Debug
  -i<internal debug level>    0              $Expect::Exp_Internal
  -o<stdout>                  0              $Expect::Log_Stdout
  -V			      N/A	     enable other debug messages
  -s<interval>                0              send speed (sec.)
  -l{0,1}                     0              enable rLogout()
  -h                          N/A            show this message

=head1 SUPPORTED FUNCTION

=head1 rOpen

  Open the remote control program.

  sub rOpen()

=head2 RETURN VALUES

  undef: error
  Otherwise: Expect handle

=head1 rLogin

  Log into the target machine.

  sub rLogin($) { my (
      $timeout		# timeout sec.
  )=@_;

=head2 RETURN VALUES

  ==0: error
  !=0: success

=head1 rCommand

  Execute a command.

  sub rCommand($$) { my (
      $command	# command
      $timeout	# timeout sec.
  )=@_;

=head2 RETURN VALUES

  ==0: error
  ==1: success: command exit status is 0
  ==2: warning: command exit status isn't 0
  ==3: warning: command exit status unknown

=head1 rReboot

  Reboot the target machine.
  The functin return when getting login prompt.

  sub rReboot($) { my (
      $timeout		# timeout sec.
  )=@_;

=head2 RETURN VALUES

  ==0: error
  !=0: success

=head1 rRebootAsync

  Reboot the target machine.
  The function return immediately when executing reboot command.

  sub rRebootAsync($) { my (
      $timeout		# timeout sec.
  )=@_;

=head2 RETURN VALUES

  ==0: error
  !=0: success

=head1 rPutfile

  Copy a local file to a remote file.

  sub rPutfile($$$) { my (
      $from,
      $to,
      $timeout,
  )=@_;

=head2 RETURN VALUES

  ==0: error
  !=0: success

=head1 rLogout

  Log out of the target machine.

  sub rLogout($) { my (
      $timeout		# timeout sec.
  )=@_;

=head2 RETURN VALUES

  ==0: error
  !=0: success

=head1 rClose

  Terminate the remote control program.

  sub rClose($) { my (
      $timeout		# timeout sec.
  )=@_;

=head2 RETURN VALUES

  ==0: error
  !=0: success

=head1 rType

  Set/get a target machine type.

  sub rType(;$)

=head2 RETURN VALUES

  Returns a target machine type.

=head1 rDevice

  Set/get serial device name.

  sub rDevice(;$)

=head2 RETURN VALUES

  Returns serial device name.

=head1 rUser

  Set/get user name.

  sub rUser(;$)

=head2 RETURN VALUES

  Returns user name.

=head1 rUserPrompt

  Set/get user prompt.

  sub rUserPrompt(;$)

=head2 RETURN VALUES

  Returns user prompt.

=head1 rPasswordPrompt

  Set/get password prompt.

  sub rPasswordPrompt(;$)

=head2 RETURN VALUES

  Returns password prompt.

=head1 rCommandPrompt

  Set/get command prompt.

  sub rCommandPrompt(;$)

=head2 RETURN VALUES

  Returns command prompt.

=head1 rDebug

  Set/get Expect.pm's debug()

  sub rDebug(;$)

=head2 RETURN VALUES

  Returns $Remote->debug().

=head1 rExpInternal

  Set/get Expect.pm's exp_internal()

  sub rExpInternal(;$)

=head2 RETURN VALUES

  Returns $Remote->exp_internal()

=head1 rLogStdout

  Set/get Expect.pm's log_stdout()

  sub rLogStdout(;$)

=head2 RETURN VALUES

  Returns $Remote->log_stdout()

=head1 rRemoteSpeed

  Set/get interval that characters are sent at.

  sub rRemoteSpeed(;$)

=head2 RETURN VALUES

  Returns interval value.

=head1 rEnableLogout

  Enable(!=0)/Desable(==0, default) rLogout()

  sub rEnableLogout(;$)

=head2 RETURN VALUES

  Returns previous value

=head1 EXAMPLE

  #!/usr/bin/perl

  use V6evalRemote;

  print "Open remote\n";
  rOpen() || exit $V6evalRemote::exitFail;
  $type=rType();

  $rOpt_timeout=5 if (!defined $rOpt_timeout);

  if($type eq "kame-freebsd") {
      print "Login\n";
      rLogin($rOpt_timeout) || exit $V6evalRemote::exitFail;
      print "List dir\n";
      rCommand("ls -a", $rOpt_timeout) || exit $V6evalRemote::exitFail;
      print "Logout\n";
      rLogout($rOpt_timeout) || exit $V6evalRemote::exitFail;
  } else {
    print "List dir of NUT($type) manually, ".
        "then press enter key.\n";
    <STDIN>;
  }
  print "Close remote\n";
  rClose();
  exit $V6evalRemote::exitPass;

=head1 REQUIRED PACKAGES

  Expect.pm
  Stty.pm
  Pty.pm

=cut

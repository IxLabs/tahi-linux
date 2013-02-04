#
# Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004, 2005 Yokogawa Electric Corporation,
# YDC Corporation, IPA (Information-technology Promotion Agency, Japan).
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

PREFIX=/usr/local/v6eval
RMTDIR=$(PREFIX)/bin/kame-netbsd-i386
REMOTES=clearnc.rmt clearprefix.rmt cleardefr.rmt reboot.rmt \
	reboot_async.rmt loginout.rmt manualaddrconf.rmt \
	ping6.rmt clearroute.rmt route.rmt rtadvd.rmt \
	ipsecClearAll.rmt ipsecSetSAD.rmt ipsecSetSPD.rmt ipsecEnable.rmt \
	mtuconfig.rmt tunnel.rmt

all:
depend:
clean:

install:
	install -d -o bin -g bin -m 755 $(PREFIX)/bin
	install -d -o bin -g bin -m 755 $(RMTDIR)
	install -c -o bin -g bin -m 555 $(REMOTES) $(RMTDIR)

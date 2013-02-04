/*
 * Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011
 * Yokogawa Electric Corporation, YDC Corporation,
 * IPA (Information-technology Promotion Agency, Japan).
 * All rights reserved.
 * 
 * Redistribution and use of this software in source and binary forms, with 
 * or without modification, are permitted provided that the following 
 * conditions and disclaimer are agreed and accepted by the user:
 * 
 * 1. Redistributions of source code must retain the above copyright 
 * notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the 
 * documentation and/or other materials provided with the distribution.
 * 
 * 3. Neither the names of the copyrighters, the name of the project which 
 * is related to this software (hereinafter referred to as "project") nor 
 * the names of the contributors may be used to endorse or promote products 
 * derived from this software without specific prior written permission.
 * 
 * 4. No merchantable use may be permitted without prior written 
 * notification to the copyrighters. However, using this software for the 
 * purpose of testing or evaluating any products including merchantable 
 * products may be permitted without any notification to the copyrighters.
 * 
 * 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHTERS, THE PROJECT AND 
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING 
 * BUT NOT LIMITED THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.  IN NO EVENT SHALL THE 
 * COPYRIGHTERS, THE PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT,STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $TAHI: v6eval/lib/Pz/McSIP.cc,v 1.3 2003/12/04 04:59:47 akisada Exp $
 */

#include "McSub.h"
#include "MmHeader.h"
#include "ItPosition.h"
#include "WObject.h"
#include "RObject.h"
#include "PControl.h"
#include "PvObject.h"
#include "PvOctets.h"
#include "PAlgorithm.h"
#include "McSIP.h"
#include <stdio.h>
#include <ctype.h>

////////////////////////////////////////////////////////////////
McUdp_SIP::McUdp_SIP(CSTR s): McHeader(s) {}
McUdp_SIP::~McUdp_SIP() {}

uint32_t McUdp_SIP::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const {
	return(buf.remainLength(at.bytes()));
}

////////////////////////////////////////////////////////////////
MmSIPMsg::MmSIPMsg(CSTR s, const PObject *g, const PObject *e): MmVarOctets(s, g, e, 0, 0) {}
MmSIPMsg::~MmSIPMsg() {}

PvObject *MmSIPMsg::reversePv(RControl &, const ItPosition &at, const ItPosition &size, const OCTBUF &buf) const {
	uint32_t offset = at.bytes(), length = size.bytes();
	return(new PvSIPMsg(length, (OCTSTR)buf.string(offset)));
}

////////////////////////////////////////////////////////////////
PvSIPMsg::PvSIPMsg(): PvOctets() {}
PvSIPMsg::PvSIPMsg(uint32_t l, OCTSTR o, bool b): PvOctets(l, o, b) {}
PvSIPMsg::~PvSIPMsg() {}

PvObject *PvSIPMsg::shallowCopy() const {
	return(new PvSIPMsg(length(), (OCTSTR)string()));
}

void PvSIPMsg::log(uint32_t t) const {
	dump("\nlog:  ");
	return;
}
 
void PvSIPMsg::print() const {
	dump();
	return;
}

void PvSIPMsg::dump(CSTR tag) const {
	int i, i9 = length();
	COCTSTR s = string();

	CSTR nl = tag ? tag : "\n";

	if(tag) {
		printf("%s", nl);
	}

	for(i = 0; i < i9; i ++) {
		if(((s[i] & 0xff) == '\r') && ((s[i + 1] & 0xff) == '\n')) {
//			printf("[0x%02x]", s[i] & 0xff);
//			printf("[0x%02x]", s[i + 1] & 0xff);

			if(i + 2 < i9) {
				printf("%s", nl);
			}

			i ++;
			continue;
		}

		if(isprint(s[i] & 0xff)) {
			putchar(s[i] & 0xff);
		} else {
			printf("[0x%02x]", s[i] & 0xff);
		}
	}

	return;
}

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
 * $TAHI: v6eval/lib/Pz/McRIPng.cc,v 1.2 2002/12/02 12:31:30 akisada Exp $
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
#include "McRIPng.h"
#include <stdio.h>

////////////////////////////////////////////////////////////////
McUdp_RIPng::McUdp_RIPng(CSTR s): McHeader(s) {}
McUdp_RIPng::~McUdp_RIPng() {}

uint32_t McUdp_RIPng::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const {
	return(buf.remainLength(at.bytes()));
}

////////////////////////////////////////////////////////////////
#define RTE_LENGTH	20
McRIPng_RTE::McRIPng_RTE(CSTR s): McOption(s) {}
McRIPng_RTE::~McRIPng_RTE() {}

uint32_t McRIPng_RTE::length_for_reverse(RControl &, ItPosition &, OCTBUF &) const {
	return(RTE_LENGTH);
}
#undef RTE_LENGTH

////////////////////////////////////////////////////////////////
MmRTE_onRIPng::MmRTE_onRIPng(CSTR key): MmReference_More0(key) {}
MmRTE_onRIPng::~MmRTE_onRIPng() {}

void MmRTE_onRIPng::add_other(McRIPng_RTE *mc) {
	dict_.add_other(mc);
}

TypevsMcDict MmRTE_onRIPng::dict_;

bool MmRTE_onRIPng::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		c.DictType().other_Set();
		return(true);
	}

	return(false);
}

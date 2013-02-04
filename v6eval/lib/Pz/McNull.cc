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
 * $TAHI: v6eval/lib/Pz/McNull.cc,v 1.9 2001/10/12 04:56:15 tanaka Exp $
 */
#include "McNull.h"
#include "MmHeader.h"
#include "ItPosition.h"
#include "WObject.h"
#include "RObject.h"
#include "PControl.h"
#include "PvOctets.h"
#include "PvAutoItem.h"

#define UN(n) PvNumber::unique(n)

/////////////////////////////////////////////////////////////////////////////
McFrame_Null* McFrame_Null::instance_=0;
McTopHdr_Null* McFrame_Null::tophdr_=0;
McFrame_Null::McFrame_Null(CSTR s):McFrame(s) {instance_=this;}
McFrame_Null::~McFrame_Null() {if(instance_==this)instance_=0;}

// COMPOSE/REVERSE
uint32_t McFrame_Null::length_for_reverse(
		RControl& c,ItPosition& at,OCTBUF& buf) const{
	uint32_t length = tophdr_->Layerlength_for_reverse(c,at,buf);
	return length;}

// INITIALIZE
McFrame_Null* McFrame_Null::create(CSTR key,CSTR tophdrkey) {
	addCompound(tophdr_ = McTopHdr_Null::create(tophdrkey)); //must with
	McFrame_Null* mc = new McFrame_Null(key);
	mc->member( new MmTopHdr(	"header",tophdr_) );
	mc->member( new MmPacket_onNull("packet") );
	// dict
	MmFrame::add(mc);			//frame=
	return mc;}

/////////////////////////////////////////////////////////////////////////////
McTopHdr_Null::McTopHdr_Null(CSTR key):McHeader(key){}
McTopHdr_Null::~McTopHdr_Null(){}

// COMPOSE/REVERSE
uint32_t McTopHdr_Null::Layerlength_for_reverse(
		RControl&,ItPosition& at,OCTBUF& buf) const {
	return buf.remainLength(at.bytes());}

bool McTopHdr_Null::HCGENE(Family)(WControl& cntr,WObject* wmem,OCTBUF& buf) const{
	int32_t val = get_next_protocolFamily(wmem);
	if(val==-1)return false;
	PvNumber def(val);
	return def.generate(cntr,wmem,buf);}
PObject* McTopHdr_Null::HCEVAL(Family)(WObject* wmem) const{
	int32_t val = get_next_protocolFamily(wmem);
	return new PvNumber(val);}

// INITIALIZE
McTopHdr_Null* McTopHdr_Null::create(CSTR key){
	McTopHdr_Null* mc = new McTopHdr_Null(key);
	mc->nextType_member(
		new MmHostUint(	"Family",	32,
			new PvHCgene(mc,(HCgenefunc)&McTopHdr_Null::HCGENE(Family)),
			new PvHCeval(mc,(HCevalfunc)&McTopHdr_Null::HCEVAL(Family))));
	// no dict
	return mc;}

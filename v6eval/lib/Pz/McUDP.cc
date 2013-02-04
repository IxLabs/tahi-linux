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
 * $TAHI: v6eval/lib/Pz/McUDP.cc,v 1.5 2002/12/02 12:31:30 akisada Exp $
 */
#include "McUDP.h"
#include "MmHeader.h"
#include "MmChecksum.h"
#include "ItPosition.h"
#include "WObject.h"
#include "RObject.h"
#include "PControl.h"
#include "PvObject.h"
#include "PvOctets.h"

//////////////////////////////////////////////////////////////////////////////
#define SUPER	McUpper
McUpp_UDP* McUpp_UDP::instance_=0;
McTopHdr_UDP* McUpp_UDP::tophdr_=0;
McUpp_UDP::McUpp_UDP(CSTR key):SUPER(key) {instance_=this;}
McUpp_UDP::~McUpp_UDP(){if(instance_==this)instance_=0;}

// COMPOSE/REVERSE
uint32_t McUpp_UDP::length_for_reverse(
		RControl& c,ItPosition& at,OCTBUF& buf) const{
	uint32_t length = tophdr_->Layerlength_for_reverse(c,at,buf);
	return length;}

RObject* McUpp_UDP::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	RObject* r_self = SUPER::reverse(c,r_parent,at,buf);
	if(!c.error()){
		Con_IPinfo* info = c.IPinfo();
		if(info)info->reverse_postUppChecksum(c,r_self);}
	return r_self;}
bool McUpp_UDP::generate(WControl& c,WObject* w_self,OCTBUF& buf) const {
	bool rtn = SUPER::generate(c,w_self,buf);
	if(!c.error()){
		Con_IPinfo* info = c.IPinfo();
		if(info)info->generate_postUppChecksum(c,buf,w_self);}
	return rtn;}
#undef SUPER

//----------------------------------------------------------------------------
#define SUPER	McHeader
McTopHdr_UDP::McTopHdr_UDP(CSTR key):SUPER(key), SrcPort_meta_(0), DstPort_meta_(0), Layerlength_meta_(0) {
	PrtObjs_ = new PrtObjs();
}

McTopHdr_UDP::~McTopHdr_UDP(){}

// COMPOSE/REVERSE
uint32_t McTopHdr_UDP::Layerlength_for_reverse(
			RControl& c,ItPosition& at,OCTBUF& buf) const{
	if(!Layerlength_meta_)return SUPER::Layerlength_for_reverse(c,at,buf);
	uint32_t valulen	= Layerlength_meta_->value(at,buf);
	uint32_t Layerlength	= valulen;
	return Layerlength;}

bool McTopHdr_UDP::HCGENE(Length)(
			WControl& cntr,WObject* wmem,OCTBUF& buf) const{
	WObject* wc		= wmem->parent();	//TopHdr_UDP
	WObject* upper		= wc->parent();		//Upper_UDP
	uint32_t reallen	= upper->size().bytes();
	uint32_t valulen	= reallen;
	PvNumber def(valulen);
	return def.generate(cntr,wmem,buf);}

RObject *McTopHdr_UDP::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	RObject *r_self = SUPER::reverse(c, r_parent, at, buf);

	if(r_self) {
		upperProto_set_Object(r_self);
	}

	return(r_self);
}

void McTopHdr_UDP::upperProto_set_Object(RObject *r_self) const {
	if((!r_self) || (!SrcPort_meta_) || (!DstPort_meta_) || (!PrtObjs_)) {
		return;
	}

        RObject *src = (RObject *)r_self->corresponding(SrcPort_meta_);
        RObject *dst = (RObject *)r_self->corresponding(DstPort_meta_);

	if(src){
		set_srcobj(src);
	}

	if(dst) {
		set_dstobj(dst);
	}

	return;
}

bool McTopHdr_UDP::HCGENE(SourcePort)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	int32_t val = get_next_protocolPort(wmem);

	if(val == -1) {
		return(false);
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *McTopHdr_UDP::HCEVAL(SourcePort)(WObject *wmem) const {
	int32_t val = get_next_protocolPort(wmem);

	return(new PvNumber(val));
}

bool McTopHdr_UDP::HCGENE(DestinationPort)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	int32_t val = get_next_protocolPort(wmem);

	if(val == -1) {
		return(false);
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *McTopHdr_UDP::HCEVAL(DestinationPort)(WObject *wmem) const {
	int32_t val = get_next_protocolPort(wmem);

	return(new PvNumber(val));
}
#undef SUPER

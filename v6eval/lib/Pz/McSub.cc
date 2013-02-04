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
 * $TAHI: v6eval/lib/Pz/McSub.cc,v 1.37 2002/12/02 12:31:30 akisada Exp $
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
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////
// DataLinkLayer base class
McFrame::McFrame(CSTR s):McObject(s) {}
McFrame::~McFrame() {}
int32_t McFrame::token() const {return metaToken(tkn_frame_);}

//////////////////////////////////////////////////////////////////////////////
// NetworkLayer base class
McPacket::McPacket(CSTR s):McObject(s) {}
McPacket::~McPacket() {}
int32_t McPacket::token() const {return metaToken(tkn_packet_);}

//////////////////////////////////////////////////////////////////////////////
// UpperLayer base class
McUpper::McUpper(CSTR s):McObject(s) {}
McUpper::~McUpper() {}
int32_t McUpper::token() const {return metaToken(tkn_upper_);}

//////////////////////////////////////////////////////////////////////////////
// header base class
McHeader::McHeader(CSTR s):McObject(s),nextType_meta_(0){}
McHeader::~McHeader() {}
int32_t McHeader::token() const {return metaToken(tkn_header_);}

// COMPOSE/REVERSE
void McHeader::nextType_set_Control(RControl& c,RObject* r_self)const{
	if(!nextType_meta_){//othertype overwrite
		c.DictType().other_Set(); return;}
	RObject* next = (RObject*)r_self->corresponding(nextType_meta_);
	if(next){
		PvNumber* pv=(PvNumber*)next->pvalue();
		c.DictType().type_Set(pv->value());
		return;}
	c.DictType().finish_Set();
	}

RObject* McHeader::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	RObject* r_self = McObject::reverse(c,r_parent,at,buf);
	if(r_self)nextType_set_Control(c,r_self);
	return r_self;}

int32_t McHeader::get_next_protocolFamily(WObject* wmem)const{
	WObject* wc	= wmem->parent();	//TopHdr_Null
	WObject* packet = wc->nextSister();	//Packet_{IPv6,IPv4}
	int32_t rtn	= packet? packet->meta()->protocolFamily() : -1;
	if(rtn==-1)wmem->mustDefine(0);		//Payload is error
	return rtn;}

int32_t McHeader::get_next_protocolType(WObject* wmem)const{
	WObject* wc	= wmem->parent();	//TopHdr_Ether
	WObject* packet = wc->nextSister();	//Packet_XX
	int32_t rtn	= packet? packet->meta()->protocolType() : -1;
	if(rtn==-1)wmem->mustDefine(0);		//Payload is error
	return rtn;}

int32_t McHeader::get_next_headerType(WObject* wmem)const{
	WObject* wc	= wmem->parent();	//TopHdr_XX,Hdr_Ext_XX
	WObject* next	= wc->nextSister();	//Packet_XX,Hdr_Ext_XX,Upp_XX
	int32_t rtn	= next? next->meta()->headerType() : -1;
	if(rtn==-1)wmem->mustDefine(0);		//Payload is error
	return rtn;}

int32_t McHeader::get_next_protocolPort(WObject *wmem) const {
	WObject *wc = wmem->parent();
	WObject *next = wc->nextSister();
	int32_t rtn = next ? next->meta()->upperPort() : -1;

	if(rtn == -1) {
		wmem->mustDefine(0);
	}

	return(rtn);
}

uint32_t McHeader::provisional_AH_alignment_v4_v6_=0;

//////////////////////////////////////////////////////////////////////////////
// option base class
McOption::McOption(CSTR s):McObject(s){}
McOption::~McOption() {}
int32_t McOption::token() const {return metaToken(tkn_option_);}

//////////////////////////////////////////////////////////////////////////////
// Payload
McPayload::McPayload(CSTR s):McObject(s) {}
McPayload::~McPayload() {}
int32_t McPayload::token() const {return metaToken(tkn_payload_);}

// COMPOSE/REVERSE
uint32_t McPayload::length_for_reverse(
		RControl&,ItPosition& at,OCTBUF& buf) const{
	return buf.remainLength(at.bytes());}


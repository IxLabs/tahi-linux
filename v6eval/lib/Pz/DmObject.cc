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
 * $TAHI: v6eval/lib/Pz/DmObject.cc,v 1.9 2009/11/25 07:22:17 akisada Exp $
 */
#include "DmObject.h"
#include <stdio.h>
#include "PObject.h"
#include "McObject.h"
void DmObject::describe(const MObject*,const PObject*) const {}
void DmName::describe(const MObject*,const PObject* o) const {
	if(o==0) return;
	CSTR s=name();
	printf(" %s",s!=0?s:0);
	o->print();}

void DmBool::describe(const MObject*,const PObject* o) const {
	if(o==0) return;
	bool bl=false;
	int32_t n=o->intValue(bl);
	CSTR s=name();
	if(bl&&n!=0) {printf(" %s",s!=0?s:"TRUE");}}

void DmCompound::describe(const MObject* m,const PObject*) const {
	if(m==0) return;
	CSTR s=name();
	printf(" %s",s!=0?s:"");}

void DmObject::initialize() {
	McObject* mc=0;
	MObject* m=0;
#define mcname(a,b)if((mc=McObject::find(#a))!=0){mc->describer(new DmCompound(#b));}
#define mmname(a,b)if((m=mc->findMember(#a))!=0){m->describer(new DmName(#b));}
#define mmbool(a,b)if((m=mc->findMember(#a))!=0){m->describer(new DmBool(#b));}
	//--------------------------------------------------------------
	// Network Layer TopHeader
	mcname(Hdr_IPv6,			IPv6);
	if(mc!=0) {
		mmname(SourceAddress,		src=);
		mmname(DestinationAddress,	dst=);}
	mcname(Hdr_IPv4,			IPv4);
	if(mc!=0) {
		mmname(SourceAddress,		src=);
		mmname(DestinationAddress,	dst=);}
	//--------------------------------------------------------------
	// Extension Header
	mcname(Hdr_HopByHop,			HBH);
	mcname(Hdr_Destination,			DST);
	mcname(Hdr_Routing,			RT);
	mcname(Hdr_Fragment,			FRG);
	mcname(Hdr_AH,				AH);
	mcname(Hdr_ESP,				ESP);
	//--------------------------------------------------------------
	// ICMPv6: Upper Layer Protocol
	mcname(ICMPv6_ANY,			ICMPv6_ANY);
	mcname(ICMPv6_RS,			RS);
	mcname(ICMPv6_RA,			RA);
	mcname(ICMPv6_NS,			NS);
	mcname(ICMPv6_NA,			NA);
	mcname(ICMPv6_Redirect,			RED);
	mcname(ICMPv6_MLDQuery,			MLDQ);
#ifndef NOT_USE_MLDV2_QUERY
	mcname(ICMPv6_MLDv2Query,		MLDQV2);
#endif	// NOT_USE_MLDV2_QUERY
	mcname(ICMPv6_MLDReport,		MLDR);
	mcname(ICMPv6_MLDDone,			MLDD);
	mcname(ICMPv6_MLDv2Report,		MLDRV2);
	mcname(ICMPv6_EchoRequest,		EREQ);
	mcname(ICMPv6_EchoReply,		EREP);
	mcname(ICMPv6_DestinationUnreachable,	DUR);
	mcname(ICMPv6_TimeExceeded,		TEX);
	mcname(ICMPv6_ParameterProblem,		PP);
	mcname(ICMPv6_RouterRenumbering,	RR);
	mcname(ICMPv6_NameLookups,		NL);
	mcname(ICMPv6_HAADRequest,		HAADREQ);
	mcname(ICMPv6_HAADReply,		HAADREP);
	mcname(ICMPv6_MobilePrefixSolicit,	MPSOL);
	mcname(ICMPv6_MobilePrefixAdvertise,	MPADV);
	//--------------------------------------------------------------
	// TCP: Upper Layer Protocol
	mcname(Hdr_TCP,				TCP);
	if(mc!=0) {
		mmname(SourcePort,		src=);
		mmname(DestinationPort,		dst=);
		mmbool(URGFlag,			URG);
		mmbool(ACKFlag,			ACK);
		mmbool(PSHFlag,			PSH);
		mmbool(RSTFlag,			RST);
		mmbool(SYNFlag,			SYN);
		mmbool(FINFlag,			FIN);}
	//--------------------------------------------------------------
	// UDP: Upper Layer Protocol
	mcname(Hdr_UDP,				UDP);
	if(mc!=0) {
		mmname(SourcePort,		src=);
		mmname(DestinationPort,		dst=);}

#undef mmbool
#undef mmname
#undef mcname
}

DmObject::DmObject(CSTR s):name_(s) {}
DmObject::~DmObject() {}
DmName::DmName(CSTR s):DmObject(s) {}
DmName::~DmName() {}
DmBool::DmBool(CSTR s):DmObject(s) {}
DmBool::~DmBool() {}
DmCompound::DmCompound(CSTR s):DmObject(s) {}
DmCompound::~DmCompound() {}

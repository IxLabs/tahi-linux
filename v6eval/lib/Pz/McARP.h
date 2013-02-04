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
 * $TAHI: v6eval/lib/Pz/McARP.h,v 1.3 2001/10/12 04:56:14 tanaka Exp $
 */
#if !defined(__McARP_h__)
#define	__McARP_h__	1

#include "McSub.h"

//////////////////////////////////////////////////////////////////////////////
// Network Layer ARP	RFC826

class McPacket_ARP:public McPacket{
static	McPacket_ARP*		instance_;
static	class McTopHdr_ARP*	tophdr_;
	McPacket_ARP(CSTR);
public:
virtual	~McPacket_ARP();
static	McPacket_ARP* create(CSTR key,CSTR tophdrkey);
static	McPacket_ARP* instance(){return instance_;}
	int32_t protocolType()const{return TP_NL_ARP;}
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t length_for_reverse(RControl&,ItPosition& at,OCTBUF& buf) const;
};

class McTopHdr_ARP :public McHeader{
friend class McPacket_ARP;
	McTopHdr_ARP(CSTR);
virtual	~McTopHdr_ARP();
static	McTopHdr_ARP* create(CSTR);
	int32_t protocolType()const{return TP_NL_ARP;}
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t Layerlength_for_reverse(RControl&,ItPosition&,OCTBUF&) const;
};


//////////////////////////////////////////////////////////////////////////////
// Network Layer RARP	RFC903

class McPacket_RARP:public McPacket{
static	McPacket_RARP*		instance_;
static	class McTopHdr_RARP*	tophdr_;
	McPacket_RARP(CSTR);
public:
virtual	~McPacket_RARP();
static	McPacket_RARP* create(CSTR key,CSTR tophdrkey);
static	McPacket_RARP* instance(){return instance_;}
	int32_t protocolType()const{return TP_NL_RARP;}
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t length_for_reverse(RControl&,ItPosition& at,OCTBUF& buf) const;
};

class McTopHdr_RARP :public McHeader{
friend class McPacket_RARP;
	McTopHdr_RARP(CSTR);
virtual	~McTopHdr_RARP();
static	McTopHdr_RARP* create(CSTR);
	int32_t protocolType()const{return TP_NL_RARP;}
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t Layerlength_for_reverse(RControl&,ItPosition&,OCTBUF&) const;
};

//////////////////////////////////////////////////////////////////////////////
#endif

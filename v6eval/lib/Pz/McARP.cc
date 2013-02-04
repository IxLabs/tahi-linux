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
 * $TAHI: v6eval/lib/Pz/McARP.cc,v 1.4 2001/10/12 04:56:14 tanaka Exp $
 */
#include "McARP.h"
#include "ItPosition.h"
#include "WObject.h"
#include "RObject.h"
#include "PControl.h"
#include "PvObject.h"
#include "PvOctets.h"

//////////////////////////////////////////////////////////////////////////////
#define SUPER	McPacket
McPacket_ARP* McPacket_ARP::instance_=0;
McTopHdr_ARP* McPacket_ARP::tophdr_=0;
McPacket_ARP::McPacket_ARP(CSTR s):SUPER(s) {instance_=this;}
McPacket_ARP::~McPacket_ARP() {if(instance_==this)instance_=0;}

// COMPOSE/REVERSE
uint32_t McPacket_ARP::length_for_reverse(
		RControl& c,ItPosition& at,OCTBUF& buf) const{
	uint32_t length = tophdr_->Layerlength_for_reverse(c,at,buf);
	return length;}
#undef SUPER

//----------------------------------------------------------------------------
#define SUPER	McHeader

McTopHdr_ARP::McTopHdr_ARP(CSTR key):SUPER(key){}
McTopHdr_ARP::~McTopHdr_ARP(){}

// COMPOSE/REVERSE
uint32_t McTopHdr_ARP::Layerlength_for_reverse(
			RControl& c,ItPosition& at,OCTBUF& buf) const {
	return SUPER::length_for_reverse(c,at,buf);}
#undef SUPER


//////////////////////////////////////////////////////////////////////////////
#define SUPER	McPacket
McPacket_RARP* McPacket_RARP::instance_=0;
McTopHdr_RARP* McPacket_RARP::tophdr_=0;
McPacket_RARP::McPacket_RARP(CSTR s):SUPER(s) {instance_=this;}
McPacket_RARP::~McPacket_RARP() {if(instance_==this)instance_=0;}

// COMPOSE/REVERSE
uint32_t McPacket_RARP::length_for_reverse(
		RControl& c,ItPosition& at,OCTBUF& buf) const{
	uint32_t length = tophdr_->Layerlength_for_reverse(c,at,buf);
	return length;}
#undef SUPER

//----------------------------------------------------------------------------
#define SUPER	McHeader

McTopHdr_RARP::McTopHdr_RARP(CSTR key):SUPER(key){}
McTopHdr_RARP::~McTopHdr_RARP(){}

// COMPOSE/REVERSE
uint32_t McTopHdr_RARP::Layerlength_for_reverse(
			RControl& c,ItPosition& at,OCTBUF& buf) const {
	return SUPER::length_for_reverse(c,at,buf);}
#undef SUPER

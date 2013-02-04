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
 * $TAHI: v6eval/lib/Pz/McSub.h,v 1.42 2004/09/22 07:31:37 akisada Exp $
 */
#if !defined(__McSub_h__)
#define	__McSub_h__	1

#include "McObject.h"
#include "MmObject.h"

#include <sys/time.h>	//for bpf.h
#include <net/bpf.h>
#include <sys/socket.h>

class PControl;
class WObject;

//////////////////////////////////////////////////////////////////////////////
//define dictType macro	

//DataLinkLayer Type				(from net/bpf.h)
const int32_t TP_DL_Null			=DLT_NULL;
const int32_t TP_DL_Ether			=DLT_EN10MB;

//NetworkLayer Protocol Family			(from sys/socket.h)
const int32_t PF_NL_IPv6			=PF_INET6;
const int32_t PF_NL_IPv4			=PF_INET;

//NetworkLayer Protocol Type
const int32_t TP_NL_IPv6			=0x86dd;
const int32_t TP_NL_IPv4			=0x0800;
const int32_t TP_NL_ARP				=0x0806;
const int32_t TP_NL_RARP			=0x8035;

//////////////////////////////////////////////////////////////////////////////
//Header Type					RFC1700
//(Ext)
const int32_t TP_Ext_HopByHop			=0;
const int32_t TP_Ext_Destination		=60;
const int32_t TP_Ext_Routing			=43;
const int32_t TP_Ext_Fragment			=44;
const int32_t TP_Ext_AH				=51;
const int32_t TP_Ext_ESP			=50;
const int32_t TP_Ext_MH				= 135;

//(Tunnel)
const int32_t TP_Tunnel_IPv6			=41;
const int32_t TP_Tunnel_IPv4			=4;
//(Upp)
const int32_t TP_Upp_NoNext			=59;
const int32_t TP_Upp_ICMPv6			=58;
const int32_t TP_Upp_ICMPv4			=1;
const int32_t TP_Upp_IGMP			=2;
const int32_t TP_Upp_TCP			=6;
const int32_t TP_Upp_UDP			=17;
const int32_t TP_Upp_VRRP			=112;


//////////////////////////////////////////////////////////////////////////////
class McFrame:public McObject {
protected:
	McFrame(CSTR);
public:
virtual	~McFrame();
virtual	int32_t token() const;
};

class McPacket:public McObject {
protected:
	McPacket(CSTR);
public:
virtual	~McPacket();
virtual	int32_t token() const;
};

class McUpper:public McObject {
protected:
	McUpper(CSTR);
public:
virtual	~McUpper();
virtual	int32_t token() const;
};


class McHeader:public McObject {
protected:
	MmUint*		nextType_meta_;
	void nextType_member(MmUint* meta){
			nextType_meta_=meta;
			member(meta);}
	void nextType_set_Control(RControl& c,RObject* r_self)const;
	//
	int32_t get_next_protocolFamily(WObject* wmem)const;
	int32_t get_next_protocolType(WObject* wmem)const;
	int32_t get_next_headerType(WObject* wmem)const;
	int32_t get_next_protocolPort(WObject* wmem)const;
protected:
	McHeader(CSTR);
public:
virtual	~McHeader();
virtual	int32_t token() const;
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
//
protected:
static	uint32_t	provisional_AH_alignment_v4_v6_;
};


class McOption:public McObject {
protected:
	McOption(CSTR);
public:
virtual	~McOption();
virtual	int32_t token() const;
};

//////////////////////////////////////////////////////////////////////////////
// Payload
class McPayload:public McObject {
	McPayload(CSTR);
public:
virtual	~McPayload();
static	McPayload* create(CSTR);
virtual	int32_t token() const;
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t length_for_reverse(RControl&,ItPosition& at,OCTBUF& buf) const;
};

//////////////////////////////////////////////////////////////////////////////
#endif

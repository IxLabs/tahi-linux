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
 * $TAHI: v6eval/lib/Pz/McRR.h,v 1.2 2001/10/12 04:56:15 tanaka Exp $
 */
#if !defined(__McRR_h__)
#define	__McRR_h__	1

#include "McICMPv6.h"

//////////////////////////////////////////////////////////////////////////////
//	Upper ICMPv6_RouterRenumbering (draft-ietf-ipngwg-router-renum-09.txt)

//ICMPv6 Type
const int32_t TP_ICMPv6_RouterRenumbering	=138;

//Code for MessageBody
const int32_t TP_Code_ICMPv6RR_Command		=0;
const int32_t TP_Code_ICMPv6RR_Result		=1;
const int32_t TP_Code_ICMPv6RR_Reset		=255;


//////////////////////////////////////////////////////////////////////////////
//	Header ICMPv6 (AS Upper)
class McUpp_ICMPv6_RouterRenumbering :public McUpp_ICMPv6{
public:
	McUpp_ICMPv6_RouterRenumbering(CSTR);
virtual	~McUpp_ICMPv6_RouterRenumbering();
static	McUpp_ICMPv6_RouterRenumbering* create(CSTR);
	int32_t icmpv6Type()const{return TP_ICMPv6_RouterRenumbering;}
virtual	RObject* reverseRc(RControl&,
		RObject* r_parent,const ItPosition& at,PvObject* pv)const;
	uint32_t get_MessageBodyType(const RObject*)const;
};

//////////////////////////////////////////////////////////////////////////////
#endif

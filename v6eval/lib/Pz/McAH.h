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
 * $TAHI: v6eval/lib/Pz/McAH.h,v 1.6 2001/10/12 04:56:14 tanaka Exp $
 */
#if !defined(__McAH_h__)
#define	__McAH_h__	1

#include "McIPv6.h"

//////////////////////////////////////////////////////////////////////////////
// IPsec AH(Authentication Header) Extention Header	RFC2402

class McHdr_Ext_AH : public McHdr_Ext{
protected:
	McHdr_Ext_AH(CSTR);
public:
static	McHdr_Ext_AH* create(CSTR);
virtual ~McHdr_Ext_AH();
	int32_t headerType()const{return TP_Ext_AH;}
	uint32_t alignment_requirement() const;
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t length_for_reverse(RControl&,ItPosition& at,OCTBUF& buf) const;
virtual RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
virtual RObject* reverseRc(RControl&,
		RObject* r_parent,const ItPosition& at,PvObject* pv) const;
virtual WObject* compose(WControl& c,WObject* w_parent,const PObject* pc) const;
virtual WObject* composeWc(WControl&,WObject* w_parent,const PObject* pc) const;
//HardCording action method
	DEC_HCGENE(HeaderExtLength);
// OVERWRITE ICV
virtual bool overwrite_ICV(ICVControl&,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const;
};

//////////////////////////////////////////////////////////////////////////////
#include "MmHeader.h"
class MmAlgorithm_onAH : public MmReference_Must1{
public:
	MmAlgorithm_onAH(CSTR);
virtual ~MmAlgorithm_onAH();
	int32_t token()const{return metaToken(tkn_algorithm_ref_);}
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual	RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
virtual bool disused() const;
};

//////////////////////////////////////////////////////////////////////////////
class MmAH_ICV : public MmVarOctets{ //unlex,variable octets member
public:
	MmAH_ICV(CSTR str,const ICVoverwriter* ow);
virtual ~MmAH_ICV();
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual ItPosition length_for_reverse(RControl&,ItPosition&,OCTBUF& buf)const;
virtual RObject* reverseRm(RControl& c,RObject* r_parent,
		const ItPosition& at,const ItPosition& size,PvObject* pv)const;
virtual WObject* composeWm(WControl& c,
		WObject* w_parent,const PObject* pl)const;
virtual RObject* reverse(RControl&,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const;
virtual bool geneAuto(WControl& c,WObject* w_self,OCTBUF& buf) const;
};


//////////////////////////////////////////////////////////////////////////////
#endif

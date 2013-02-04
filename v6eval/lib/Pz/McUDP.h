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
 * $TAHI: v6eval/lib/Pz/McUDP.h,v 1.8 2005/05/09 09:35:24 akisada Exp $
 */
#if !defined(__McUDP_h__)
#define	__McUDP_h__	1

#include "McSub.h"

//////////////////////////////////////////////////////////////////////////////
//	Upper UDP

const int32_t TP_Udp_DNS	= 53;
#if !defined(NOT_USE_SNMP)
const int32_t TP_Udp_SNMP	= 161;
const int32_t TP_Udp_SNMPTRAP	= 162;
#endif //NOT_USE_SNMP
const int32_t TP_Udp_ISAKMP	= 500;
const int32_t TP_Udp_RIPng	= 521;
const int32_t TP_Udp_DHCPv6_CL	= 546;
const int32_t TP_Udp_DHCPv6_SV	= 547;
const int32_t TP_Udp_SIP	= 5060;

class McUpp_UDP :public McUpper{
static	McUpp_UDP*		instance_;
static	class McTopHdr_UDP*	tophdr_;
	McUpp_UDP(CSTR);
public:
virtual ~McUpp_UDP();
static	McUpp_UDP* create(CSTR key,CSTR tophdrkey);
static	McUpp_UDP* instance(){return instance_;}
	int32_t headerType()const{return TP_Upp_UDP;}
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t length_for_reverse(RControl&,ItPosition& at,OCTBUF& buf) const;
virtual RObject* reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const;
virtual bool generate(WControl& c,WObject* w_self,OCTBUF& buf)const;
};

////////////////////////////////////////////////////////////////
class PrtObjs {
	RObject *SrcObject_;
	RObject *DstObject_;

	public:
		PrtObjs(): SrcObject_(0), DstObject_(0) {};
		virtual ~PrtObjs() {};

		void set_src(RObject *);
		void set_dst(RObject *);
		const RObject *get_src() const;
		const RObject *get_dst() const;
};

inline void PrtObjs::set_src(RObject *obj) {
	SrcObject_ = obj;
	return;
}

inline void PrtObjs::set_dst(RObject *obj) {
	DstObject_ = obj;
	return;
}

inline const RObject *PrtObjs::get_src() const {
	return(SrcObject_);
}

inline const RObject *PrtObjs::get_dst() const {
	return(DstObject_);
}

////////////////////////////////////////////////////////////////
class McTopHdr_UDP :public McHeader{
friend	class McUpp_UDP;
	MmUint *SrcPort_meta_;
	MmUint *DstPort_meta_;
	MmUint*		Layerlength_meta_;
	PrtObjs *PrtObjs_;

	void SrcPort_member(MmUint *meta) {
		SrcPort_meta_ = meta;
		member(meta);
	}

	void DstPort_member(MmUint *meta) {
		DstPort_meta_ = meta;
		member(meta);
	}

	void Layerlength_member(MmUint* meta){
			Layerlength_meta_=meta; member(meta);}

	void set_srcobj(RObject *obj) const {
		if(PrtObjs_) {
			PrtObjs_->set_src(obj);
		}
	}

	void set_dstobj(RObject *obj) const {
		if(PrtObjs_) {
			PrtObjs_->set_dst(obj);
		}
	}

	McTopHdr_UDP(CSTR);
virtual ~McTopHdr_UDP();
static	McTopHdr_UDP* create(CSTR);
	int32_t headerType()const{return TP_Upp_UDP;}
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t Layerlength_for_reverse(RControl&,ItPosition&,OCTBUF&) const;
//HardCording action method
	DEC_HCGENE(Length);

	public:
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
		void upperProto_set_Object(RObject *) const;
		const RObject *get_srcobj() const;
		const RObject *get_dstobj() const;
		DEC_HCGENE(SourcePort);
		DEC_HCEVAL(SourcePort);
		DEC_HCGENE(DestinationPort);
		DEC_HCEVAL(DestinationPort);
};

inline const RObject *McTopHdr_UDP::get_srcobj() const {
	if(!PrtObjs_) {
		return(0);
	}

	return(PrtObjs_->get_src());
}

inline const RObject *McTopHdr_UDP::get_dstobj() const {
	if(!PrtObjs_) {
		return(0);
	}

	return(PrtObjs_->get_dst());
}
//////////////////////////////////////////////////////////////////////////////
#endif

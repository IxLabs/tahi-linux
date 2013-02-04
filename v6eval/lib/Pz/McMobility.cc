/*
 *
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
 * $TAHI: v6eval/lib/Pz/McMobility.cc,v 1.21 2005/05/09 09:35:24 akisada Exp $
 *
 */


#include "McMobility.h"
#include "ItPosition.h"
#include "WObject.h"
#include "RObject.h"
#include "PControl.h"
#include "PvObject.h"
#include "PvOctets.h"
#include "PcObject.h"
#include <stdio.h>
#include <string.h>

extern "C" {
#include <openssl/hmac.h>
}

////////////////////////////////////////////////////////////////
RmBAstatus::RmBAstatus(RObject *r_parent, const MObject *m, const ItPosition &offset, const ItPosition &size, PvObject *pv):
	RmObject(r_parent, m, offset, size, pv) {}

RmBAstatus::~RmBAstatus() {}

void RmBAstatus::printName(uint32_t t, CSTR) const {
	RObject::printName(t);

	const PvObject *o = pvalue();
	if(o) {
		printf(" = ");
		o->print();

		uint32_t qtype = (uint32_t)(((PvNumber *)o)->value());
		printBAstatus(qtype);
	} else {
		printf("NO OBJECT");
	}
}

void RmBAstatus::logSelf(uint32_t t, CSTR cls) const {
	RObject::logSelf(t, cls);

	const PvObject *o = pvalue();
	if(o) {
		printf(" = ");
		o->log();

		uint32_t qtype = (uint32_t)(((PvNumber *)o)->value());
		printBAstatus(qtype);
	}
}

void RmBAstatus::printBAstatus(uint32_t status) const {
	switch(status) {
		case   0: printf(" (Binding Update accepted)");			break;
		case   1: printf(" (Accepted but prefix discovery necessary)");	break;
		case 128: printf(" (Reason unspecified)");			break;
		case 129: printf(" (Administratively prohibited)");		break;
		case 130: printf(" (Insufficient resources)");			break;
		case 131: printf(" (Home registration not supported)");		break;
		case 132: printf(" (Not home subnet)");				break;
		case 133: printf(" (Not home agent for this mobile node)");	break;
		case 134: printf(" (Duplicate Address Detection failed)");	break;
		case 135: printf(" (Sequence number out of window)");		break;
		case 136: printf(" (Expired home nonce index)");		break;
		case 137: printf(" (Expired care-of nonce index)");		break;
		case 138: printf(" (Expired nonces)");				break;
		case 139: printf(" (Registration type change disallowed)");	break;
		case 140: printf(" (Mobile Router Operation not permitted)");	break;
		case 141: printf(" (Invalid Prefix)");				break;
		case 142: printf(" (Not Authorized for Prefix)");		break;
		case 143: printf(" (Forwarding Setup failed)");			break;
		default:  printf(" (Unknown)");					break;
	}

        return;
}

////////////////////////////////////////////////////////////////
MmBAstatus::MmBAstatus(CSTR s, uint16_t n, const PObject *g, const PObject *e, const ICVoverwriter *ow, METH_HC_ForIPinfo meth):
	MmUint(s, n, g, e, ow, meth) {}
MmBAstatus::~MmBAstatus() {}

RObject *MmBAstatus::reverseRm(RControl &, RObject *r_parent, const ItPosition &at, const ItPosition &size, PvObject *pv) const {
	return(new RmBAstatus(r_parent, this, at, size, pv));
}

////////////////////////////////////////////////////////////////
RmBEstatus::RmBEstatus(RObject *r_parent, const MObject *m, const ItPosition &offset, const ItPosition &size, PvObject *pv):
	RmObject(r_parent, m, offset, size, pv) {}

RmBEstatus::~RmBEstatus() {}

void RmBEstatus::printName(uint32_t t, CSTR) const {
	RObject::printName(t);

	const PvObject *o = pvalue();
	if(o) {
		printf(" = ");
		o->print();

		uint32_t qtype = (uint32_t)(((PvNumber *)o)->value());
		printBEstatus(qtype);
	} else {
		printf("NO OBJECT");
	}
}

void RmBEstatus::logSelf(uint32_t t, CSTR cls) const {
	RObject::logSelf(t, cls);

	const PvObject *o = pvalue();
	if(o) {
		printf(" = ");
		o->log();

		uint32_t qtype = (uint32_t)(((PvNumber *)o)->value());
		printBEstatus(qtype);
	}
}

void RmBEstatus::printBEstatus(uint32_t status) const {
	switch(status) {
		case 1:  printf(" (Unknown binding for Home Address destination option)");	break;
		case 2:  printf(" (Unrecognized MH Type value)");				break;
		default: printf(" (Unknown)");							break;
	}

        return;
}

////////////////////////////////////////////////////////////////
MmBEstatus::MmBEstatus(CSTR s, uint16_t n, const PObject *g, const PObject *e, const ICVoverwriter *ow, METH_HC_ForIPinfo meth):
	MmUint(s, n, g, e, ow, meth) {}
MmBEstatus::~MmBEstatus() {}

RObject *MmBEstatus::reverseRm(RControl &, RObject *r_parent, const ItPosition &at, const ItPosition &size, PvObject *pv) const {
	return(new RmBEstatus(r_parent, this, at, size, pv));
}

//
//  Mobility Header
//
////////////////////////////////////////////////////////////////////////////////

#define SUPER   McHdr_Ext
McHdr_Ext_MH_ONE *McHdr_Ext_MH_ONE::instance_ = 0;

McHdr_Ext_MH_ONE *McHdr_Ext_MH_ONE::instance() {
	if(!instance_) {
		instance_ = new McHdr_Ext_MH_ONE("Ext_MH");
	}

	return(instance_);
}

McHdr_Ext_MH_ONE::McHdr_Ext_MH_ONE(CSTR key):SUPER(key) {
	member(new MmHeader_onMH("mobility"));
	// dict
	MmExtent_onIP::add(this);	// Packet_IPv6::exthdr=
}

McHdr_Ext_MH_ONE::~McHdr_Ext_MH_ONE() {}

// COMPOSE/REVERSE
bool McHdr_Ext_MH_ONE::containsMc(const MObject *mc) const {
	bool rtn = SUPER::containsMc(mc);

	return(rtn ? rtn : members_[0]->containsMc(mc));
}

RObject *McHdr_Ext_MH_ONE::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	return(members_[0]->reverse(c, r_parent, at, buf));
}	// forward McHdr_Ext_MH
#undef SUPER

////////////////////////////////////////////////////////////////////////////////
#define SUPER   McHdr_Ext

#define DEF_LENGTH_ELEM_MH	8
#define DEF_LENGTH_OFFSET_MH	8

McHdr_Ext_MH::McHdr_Ext_MH(CSTR key):SUPER(key), type_(0) {
	LengthCtrl_ = new LengthCtrl();
	McHdr_Ext_MH_ONE::instance();	// wake up
}

McHdr_Ext_MH::~McHdr_Ext_MH() {}

uint32_t McHdr_Ext_MH::length_for_reverse(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(!length_) {
#if 0
		uint32_t x = SUPER::length_for_reverse(c, at, buf);
		set_LengthCtrl(x);
		return(x);
#else
		return(SUPER::length_for_reverse(c, at, buf));
#endif
	}

	uint32_t valulen = length_->value(at, buf);
	uint32_t length = valulen * DEF_LENGTH_ELEM_MH + DEF_LENGTH_OFFSET_MH;
#if 0
	set_LengthCtrl(length);
#endif
	return(length);
}

bool McHdr_Ext_MH::HCGENE(HeaderExtLength)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	WObject *wc = wmem->parent();   // Hdr_Ext_XX
	uint32_t reallen = wc->size().bytes();

	uint32_t valulen = (reallen - DEF_LENGTH_OFFSET_MH) / DEF_LENGTH_ELEM_MH;
	PvNumber def(valulen);

	set_LengthCtrl(reallen);

	return(def.generate(cntr, wmem, buf));
}

bool McHdr_Ext_MH::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	uint32_t limit = buf.remainLength(at.bytes());

	if(!limit) {
		return(false);	// End of Ext Header
	}

	ItPosition tmpat = at;

	RObject *r_self = SUPER::reverse(c, 0, tmpat, buf);
	if(!r_self) {
		return(false);
	}

	RObject *rtype = (RObject *)r_self->corresponding(type_);

	if(!rtype) {
		return(false);	// Type field decode error
	}

	const PvNumber *pv = (const PvNumber *)rtype->pvalue();
	uint32_t typevalue = pv->value();

	c.DictType().type_Set(typevalue);	// self Type set
	delete rtype;

	return(true);
}

bool McHdr_Ext_MH::HCGENE(Type)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	int32_t val = get_mobilityType(wmem);
	if(val == -1) {
		return false;
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *McHdr_Ext_MH::HCEVAL(Type)(WObject *wmem) const {
	int32_t val = get_mobilityType(wmem);
	return(new PvNumber(val));
}

int32_t McHdr_Ext_MH::get_mobilityType(WObject *wmem) const {
	WObject *wc = wmem->parent();
	int32_t rtn = wc ? wc->meta()->mobilityType() : -1;

	if(rtn == -1) {
		wmem->mustDefine(0);
	}

	return(rtn);
}

RObject *McHdr_Ext_MH::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	RObject *r_self = SUPER::reverse(c, r_parent, at, buf);

	if(!c.error()){
		Con_IPinfo *info = c.IPinfo();

		if(info) {
			info->reverse_postBSA(c, r_self);
#if 0
			{
				RObject *rlength = (RObject *)r_self->corresponding(length_);

				if(rlength) {
					const PvNumber *pv = (const PvNumber *)rlength->pvalue();

					if(pv) {
						uint32_t lengthvalue = pv->value();
						set_LengthCtrl(lengthvalue * DEF_LENGTH_ELEM_MH + DEF_LENGTH_OFFSET_MH);
					}
				}
			}
#endif
			info->reverse_postUppChecksum(c, r_self);
		}
	}

	return(r_self);
}

bool McHdr_Ext_MH::generate(WControl &c, WObject *w_self, OCTBUF &buf) const {
	bool rtn = SUPER::generate(c, w_self, buf);

	if(!c.error()) {
		Con_IPinfo *info = c.IPinfo();

		if(info) {
			info->generate_postBSA(c, buf, w_self);

			{
				WObject *wlength = (WObject *)w_self->corresponding(length_);

				if(wlength) {
					const PvNumber *pv = (const PvNumber *)wlength->pvalue();

					if(pv) {
						uint32_t lengthvalue = pv->value();
						set_LengthCtrl(lengthvalue * DEF_LENGTH_ELEM_MH + DEF_LENGTH_OFFSET_MH);
					}
				}
			}

			info->generate_postUppChecksumWithLength(c, buf, w_self, get_LengthCtrl());
		}
	}

	return(rtn);
}
#undef SUPER

////////////////////////////////////////////////////////////////////////////////
#define SUPER	McHdr_Ext_MH
McHdr_Ext_MH_ANY::McHdr_Ext_MH_ANY(CSTR key):SUPER(key) {}
McHdr_Ext_MH_ANY::~McHdr_Ext_MH_ANY() {}

McHdr_Ext_MH_BRR::McHdr_Ext_MH_BRR(CSTR key):SUPER(key) {}
McHdr_Ext_MH_BRR::~McHdr_Ext_MH_BRR() {}

McHdr_Ext_MH_HoTI::McHdr_Ext_MH_HoTI(CSTR key):SUPER(key) {}
McHdr_Ext_MH_HoTI::~McHdr_Ext_MH_HoTI() {}

McHdr_Ext_MH_CoTI::McHdr_Ext_MH_CoTI(CSTR key):SUPER(key) {}
McHdr_Ext_MH_CoTI::~McHdr_Ext_MH_CoTI() {}

McHdr_Ext_MH_HoT::McHdr_Ext_MH_HoT(CSTR key):SUPER(key) {}
McHdr_Ext_MH_HoT::~McHdr_Ext_MH_HoT() {}

McHdr_Ext_MH_CoT::McHdr_Ext_MH_CoT(CSTR key):SUPER(key) {}
McHdr_Ext_MH_CoT::~McHdr_Ext_MH_CoT() {}

McHdr_Ext_MH_BU::McHdr_Ext_MH_BU(CSTR key):SUPER(key) {}
McHdr_Ext_MH_BU::~McHdr_Ext_MH_BU() {}

McHdr_Ext_MH_BA::McHdr_Ext_MH_BA(CSTR key):SUPER(key) {}
McHdr_Ext_MH_BA::~McHdr_Ext_MH_BA() {}

McHdr_Ext_MH_BE::McHdr_Ext_MH_BE(CSTR key):SUPER(key) {}
McHdr_Ext_MH_BE::~McHdr_Ext_MH_BE() {}
#undef SUPER

///////////////////////////////////////////////////////////////////////////////
MmHeader_onMH::MmHeader_onMH(CSTR key):MmReference_Must1(key) {}
MmHeader_onMH::~MmHeader_onMH() {}

void MmHeader_onMH::add(McHdr_Ext_MH *mc) {
	dict_.add(mc->mobilityType(), mc);
}

void MmHeader_onMH::add_other(McHdr_Ext_MH *mc) {
	dict_.add_other(mc);
}

TypevsMcDict MmHeader_onMH::dict_;

// REVERSE
bool MmHeader_onMH::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	McHdr_Ext_MH *any = (McHdr_Ext_MH *)dict_.find_other();

	return(any->overwrite_DictType(c, at, buf));
}

//
//  Mobility Options
//
////////////////////////////////////////////////////////////////////////////////

#define DEF_ALIGNMENT_OptMH	8
#define DEF_LENGTH_ELEM_OptMH	2
McOpt_MH::McOpt_MH(CSTR key):McOption(key), type_(0), length_(0) {}
McOpt_MH::~McOpt_MH() {}

// COMPOSE/REVERSE
uint32_t McOpt_MH::length_for_reverse(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(!length_) {
		return(McOption::length_for_reverse(c, at, buf));
	}

	uint32_t valulen = length_->value(at, buf);
	uint32_t length  = valulen + DEF_LENGTH_ELEM_OptMH;

	return(length);
}

bool McOpt_MH::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
        uint32_t limit = buf.remainLength(at.bytes());

	if(limit == 0) {
		return false;	// End of Mobility Header
	}
	//
	ItPosition tmpat = at;
	RObject *rtype = type_->reverse(c, 0, tmpat, buf);
	if(!rtype) {
		return false;	// Type field decode error
	}
	//
	const PvNumber *pv = (const PvNumber*)rtype->pvalue();
	uint32_t typevalue = pv->value();
	c.DictType().type_Set(typevalue);	// self Type set
	delete rtype;
	return true;
}

bool McOpt_MH::HCGENE(Length)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	WObject *wc = wmem->parent();
	uint32_t reallen = wc->size().bytes();
	uint32_t valulen = reallen - DEF_LENGTH_ELEM_OptMH;
	PvNumber def(valulen);

	return(def.generate(cntr, wmem, buf));
}

bool McOpt_MH::HCGENE(Type)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	int32_t val = get_mobilityOptionType(wmem);
	if(val < 0) {
		return(false);
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *McOpt_MH::HCEVAL(Type)(WObject *wmem) const {
	int32_t val = get_mobilityOptionType(wmem);
	return(new PvNumber(val));
}

int32_t McOpt_MH::get_mobilityOptionType(WObject *wmem) const {
	WObject *wc = wmem->parent();
	int32_t rtn = wc ? wc->meta()->optionType() : -1;

	if(rtn < 0) {
		wmem->mustDefine(0);
	}

	return(rtn);
}

////////////////////////////////////////////////////////////////////////////////
McOpt_MH_ANY::McOpt_MH_ANY(CSTR key):McOpt_MH(key) {}
McOpt_MH_ANY::~McOpt_MH_ANY() {}

McOpt_MH_Pad1::McOpt_MH_Pad1(CSTR key):McOpt_MH(key) {}
McOpt_MH_Pad1::~McOpt_MH_Pad1() {}

McOpt_MH_PadN::McOpt_MH_PadN(CSTR key):McOpt_MH(key) {}
McOpt_MH_PadN::~McOpt_MH_PadN() {}

McOpt_MH_AlternateCoA::McOpt_MH_AlternateCoA(CSTR key):McOpt_MH(key) {}
McOpt_MH_AlternateCoA::~McOpt_MH_AlternateCoA() {}

McOpt_MH_NonceIndices::McOpt_MH_NonceIndices(CSTR key):McOpt_MH(key) {}
McOpt_MH_NonceIndices::~McOpt_MH_NonceIndices() {}

McOpt_MH_BindingAuthData::McOpt_MH_BindingAuthData(CSTR key):McOpt_MH(key) {}
McOpt_MH_BindingAuthData::~McOpt_MH_BindingAuthData() {}

PObject *McOpt_MH_BindingAuthData::tokenObject(int l, CSTR f) const {
	return(new PaBSA(this, f, l));
}

McOpt_MH_BindingRefreshAdvice::McOpt_MH_BindingRefreshAdvice(CSTR key):McOpt_MH(key) {}
McOpt_MH_BindingRefreshAdvice::~McOpt_MH_BindingRefreshAdvice() {}

McOpt_MH_MobNetworkPrefix::McOpt_MH_MobNetworkPrefix(CSTR key):McOpt_MH(key) {}
McOpt_MH_MobNetworkPrefix::~McOpt_MH_MobNetworkPrefix() {}

////////////////////////////////////////////////////////////////////////////////
MmOption_onMH::MmOption_onMH(CSTR key):MmReference_More0(key, true) {}
MmOption_onMH::~MmOption_onMH() {}

void MmOption_onMH::add(McOpt_MH *mc) {
	dict_.add(mc->optionType(), mc);
}

void MmOption_onMH::add_other(McOpt_MH *mc) {
	dict_.add_other(mc);
}

TypevsMcDict MmOption_onMH::dict_;

// REVERSE
bool MmOption_onMH::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	McOpt_MH *any = (McOpt_MH *)dict_.find_other();

	return(any->overwrite_DictType(c, at, buf));
}

////////////////////////////////////////////////////////////////////////////////
#define COOKIEZERO()	PvCookie64::zerocookie()
MmMH_Cookie64::MmMH_Cookie64(CSTR s):MmOctets(s, 8, COOKIEZERO(), COOKIEZERO(), 0, 0) {}
MmMH_Cookie64::~MmMH_Cookie64() {}

// REVERSE
PvObject *MmMH_Cookie64::reversePv(RControl &, const ItPosition &at, const ItPosition &size, const OCTBUF &buf) const {
	OCTSTR str = (OCTSTR)buf.string(at.bytes());
	return(new PvCookie64(str, false));
}
#undef COOKIEZERO

////////////////////////////////////////////////////////////////////////////////
#define SUPER	RmObject
RmBSA::RmBSA(RObject *r_parent, const MObject *m, const ItPosition &offset, const ItPosition &size, PvObject *pv, const PaBSA *bsa):
SUPER(r_parent, m, offset, size, pv), bsa_(bsa), calc_pvalue_(new OCTBUF()) {}

RmBSA::~RmBSA(){
	if(calc_pvalue_) {
		delete calc_pvalue_;
		calc_pvalue_ = 0;
	}
}

void RmBSA::set_calc_pvalue(PvObject *calc) {
	if(calc_pvalue_) {
		delete calc_pvalue_;
	}

	calc_pvalue_ = calc;
}

void RmBSA::post_reverse(Con_IPinfo &info, RControl &c, RObject *base) {
	if(!parent()) {
		return;
	}

	if(!parent()->parent()) {
		return;
	}

	const OCTBUF *basebuf	= (const OCTBUF *)base->pvalue();
	uint32_t mhlen		= base->size().bytes();		// Mobility Header
	uint32_t mhoffset0	= base->offset().bytes();
	OCTSTR mhbuf		= (OCTSTR)basebuf->string();

	uint32_t authlen	= size().bytes();	// Authenticator
	uint32_t authoffset	= parent()->offset().bytes() + offset().bytes();
	uint32_t mhoffset	= parent()->parent()->offset().bytes();

	if(mhoffset0 != mhoffset) {
		return;
	}

	uint32_t restlen = mhlen - (authoffset + authlen);

	OCTBUF bsabuf(mhlen - authlen, (OCTSTR)mhbuf, true);

#define CHECKSUM_OFFSET	4
#define CHECKSUM_LEN	2
	memset((void *)(bsabuf.string() + CHECKSUM_OFFSET), 0, CHECKSUM_LEN);
#undef CHECKSUM_LEN
#undef CHECKSUM_OFFSET

	if(restlen) {
		memcpy((void *)(bsabuf.string() + authoffset), (void *)(mhbuf + authoffset + authlen), restlen);
	}

	OCTBUF *calc = 0;

	if(bsa_) {
		calc = bsa_->BSA_Calculate(bsabuf);
	}

	set_calc_pvalue(calc);

	return;
}
#undef SUPER

////////////////////////////////////////////////////////////////////////////////
#define SUPER	WmObject
WmBSA::WmBSA(WObject *p, const MObject *m, const PObject *po, const PaBSA *bsa):SUPER(p, m, po), bsa_(bsa) {}
WmBSA::~WmBSA(){}

void WmBSA::post_generate(Con_IPinfo &info, WControl &c, OCTBUF &buf, WObject *base) {
	WObject *w_auth = (WObject *)info.postBSA(); 
	if(!w_auth) {
		return;
	}

	const OCTBUF *basebuf	= (const OCTBUF *)base->pvalue();

	uint32_t mhlen		= base->size().bytes();		// Mobility Header
	uint32_t mhoffset	= base->offset().bytes();
	OCTSTR mhbuf		= (OCTSTR)basebuf->string();

	uint32_t authlen	= w_auth->size().bytes();	// Authenticator
	uint32_t authoffset	= w_auth->offset().bytes();

	if(mhoffset > authoffset) {
		return;
	}

	if(mhoffset + mhlen < authoffset + authlen) {
		return;
	}

	uint32_t restlen = mhlen - ((authoffset - mhoffset) + authlen);

	OCTBUF bsabuf(mhlen - authlen, (OCTSTR)mhbuf, true);

#define CHECKSUM_OFFSET 4
#define CHECKSUM_LEN    2
	memset((void *)(bsabuf.string() + CHECKSUM_OFFSET), 0, CHECKSUM_LEN);
#undef CHECKSUM_LEN
#undef CHECKSUM_OFFSET

	if(restlen) {
		memcpy((void *)(bsabuf.string() + authoffset - mhoffset), (void *)(mhbuf + authoffset - mhoffset + authlen), restlen);
	}

	OCTBUF *calc = 0;

	if(bsa_) {
		calc = bsa_->BSA_Calculate(bsabuf);
	}

	if(calc) {
		set_rgenerate(calc);
		SUPER::generate(c, buf);
	}

	return;
}

bool WmBSA::doEvaluate(WControl &c, RObject &r) {
	//typecheck be finished by metaEvaluate()

	RmBSA &rm = (RmBSA &)r;
	const PvObject *ro = rm.pvalue();
	const PvObject *eo = rm.calc_pvalue();

	if(!eo) {
		eo = revaluate();
	}

	return(valueEvaluate(c, ro, eo));
}
#undef SUPER

////////////////////////////////////////////////////////////////////////////////
KeepBSA::KeepBSA() {
//	bsa_ = 0;
	current_ = 0;
	bsa_ = new BSAList();
}

KeepBSA::~KeepBSA() {
	current_ = 0;

	if(bsa_) {
		delete bsa_;
		bsa_ = 0;
	}
}

const PaBSA *KeepBSA::GetBSA(void) {
	uint32_t i = current_ ++;

	const PaBSA *bsa = 0;

	if(bsa_) {
		uint32_t i9 = bsa_->size();
		bsa = i < i9 ? bsa_->index(i) : 0;
	}

	return(bsa);
}

void KeepBSA::SetBSA(const PaBSA *bsa) {
//	bsa_ = bsa;
	bsa_->append((PaBSA *)bsa);
}

////////////////////////////////////////////////////////////////////////////////
#define AUTHENTICATOR_LEN	12
#define BSAZERO()	PvBSA96::zerobsa()
MmMH_Auth::MmMH_Auth(CSTR s):MmOctets(s, AUTHENTICATOR_LEN, BSAZERO(), BSAZERO(), 0, 0) {
	keepbsa_ = new KeepBSA();
}

MmMH_Auth::~MmMH_Auth() {
	if(keepbsa_) {
		delete keepbsa_;
		keepbsa_ = 0;
	}
}

// REVERSE
PvObject *MmMH_Auth::reversePv(RControl &, const ItPosition &at, const ItPosition &, const OCTBUF &buf) const {
	OCTSTR str = (OCTSTR)buf.string(at.bytes());
	return(new PvBSA96(str, false));
}

RObject *MmMH_Auth::reverseRm(RControl &c, RObject *r_parent, const ItPosition &at, const ItPosition &size, PvObject *pv) const {
	const PaBSA *bsa = 0;

	if(keepbsa_) {
		bsa = keepbsa_->GetBSA();
	}

	RmBSA *r_self = new RmBSA(r_parent, this, at, size, pv, bsa);

	return(r_self);
}

WObject *MmMH_Auth::composeWm(WControl &c, WObject *w_parent, const PObject *pl) const {
	const PaBSA *bsa = c.pushing_BSA();

	if(keepbsa_) {
		keepbsa_->SetBSA(bsa);
	}

	return(new WmBSA(w_parent, this, pl, bsa));
}

RObject *MmMH_Auth::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	RObject *r_self = MmOctets::reverse(c, r_parent, at, buf);

	if(!c.error()){
		Con_IPinfo *info = c.IPinfo();
		if(info) {
			info->postBSA(r_self);
		}

	}	// post reverse calcBSA

	return(r_self);
}

bool MmMH_Auth::generate(WControl &c, WObject *w_self, OCTBUF &buf) const {
	bool rtn = MmOctets::generate(c, w_self, buf);

	if(!c.error()) {
		Con_IPinfo *info = c.IPinfo();
		if(info) {
			info->postBSA(w_self);
		}	// post generate calcBSA
	}

	return(rtn);
}
#undef BSAZERO
#undef AUTHENTICATOR_LEN

////////////////////////////////////////////////////////////////////////////////
PfBSA::PfBSA(const MfBSA *a, CSTR b, int c):PvFunction(a, b, c), meta_(a), context_(0) {}
PfBSA::~PfBSA() {};

const MObject *PfBSA::meta() const {
	return(metaClass());
}

void PfBSA::init() {
	const MfBSA *m = metaClass();

	if(m) {
		context_ = m->init(context_, args());
	}

	return;
}

void PfBSA::update(const OCTBUF &s) {
	const MfBSA *m = metaClass();

	if(m) {
		m->update(context_, args(), s);
	}

	return;
}

PvOctets *PfBSA::result() {
	const MfBSA *m = metaClass();

	PvOctets *ret = 0;

	if(m) {
		ret = m->result(context_, args());
	}

	return(ret);
}

////////////////////////////////////////////////////////////////////////////////
PaBSA::PaBSA(const MObject *m, CSTR st, int l):PcObject(m ,st, l), bsa_(0) {}
PaBSA::~PaBSA() {
	if(bsa_ != 0) {
		delete bsa_;
		bsa_ = 0;
	}
}

PObject *PaBSA::bsa_member(PObject *p) {
	return(bsa_ = (PfBSA *)(p));
}

OCTBUF *PaBSA::BSA_Calculate(const OCTBUF &s) const {
	if(!bsa_) {
		return(0);
	}

	bsa_->init();
	bsa_->update(s);
	PvOctets *icv = bsa_->result();

	return(icv);
}

// COMPOSE
WObject *PaBSA::selfCompose(WControl &c, WObject *w_parent) const {
	const MObject *m = meta();

	c.set_push_BSA(this);

	return(m != 0 ? m->compose(c, w_parent, this) : 0);
}

implementCmList(BSAList, PaBSA);

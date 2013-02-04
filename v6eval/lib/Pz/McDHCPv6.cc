/*
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011
 * Yokogawa Electric Corporation, 
 * INTAP (Interoperability Technology Association for Information Processing, Japan).
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
 * $TAHI: v6eval/lib/Pz/McDHCPv6.cc,v 1.9 2005/05/09 09:35:23 akisada Exp $
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
#include "McDHCPv6.h"
#include <stdio.h>
#include <string.h>

////////////////////////////////////////////////////////////////
PvTimezone::PvTimezone(): PvOctets() {}
PvTimezone::PvTimezone(uint32_t l, OCTSTR o, bool b): PvOctets(l, o, b) {}
PvTimezone::~PvTimezone() {}

PvObject *PvTimezone::shallowCopy() const {
	return(new PvTimezone(length(), (OCTSTR)string()));
}

void PvTimezone::log(uint32_t t) const {
	print();
}
 
void PvTimezone::print() const {
	printf("%s", string());
}

////////////////////////////////////////////////////////////////
MmTimezone::MmTimezone(CSTR s, const PObject *g, const PObject *e): MmVarOctets(s, g, e, 0, 0) {}
MmTimezone::~MmTimezone() {}

PvObject *MmTimezone::reversePv(RControl &, const ItPosition &at, const ItPosition &size, const OCTBUF &buf) const {
	uint32_t offset = at.bytes(), length = size.bytes();
	return(new PvTimezone(length, (OCTSTR)buf.string(offset)));
}

////////////////////////////////////////////////////////////////
#define SUPER	RmObject
RmDHCPAuth::RmDHCPAuth(RObject *r_parent,
	const MObject *m, const ItPosition &offset, const ItPosition &size, PvObject *pv, const PaDHCPAuth *dhcpauth):
	SUPER(r_parent, m, offset, size, pv), dhcpauth_(dhcpauth), calc_pvalue_(new OCTBUF()) {}

RmDHCPAuth::~RmDHCPAuth(){
	if(calc_pvalue_) {
		delete calc_pvalue_;
		calc_pvalue_ = 0;
	}
}

void RmDHCPAuth::set_calc_pvalue(PvObject *calc) {
	if(calc_pvalue_) {
		delete calc_pvalue_;
	}

	calc_pvalue_ = calc;
}

void RmDHCPAuth::post_reverse(Con_IPinfo &info, RControl &c, RObject *base) {
	if(!parent()) {
		return;
	}

	if(!parent()->parent()) {
		return;
	}

	if(!parent()->parent()->parent()) {
		return;
	}

	const OCTBUF *basebuf	= (const OCTBUF *)base->pvalue();
	uint32_t dhcplen	= base->size().bytes();
	uint32_t dhcpoffset0	= base->offset().bytes();
	OCTSTR dhcpbuf		= (OCTSTR)basebuf->string();

	uint32_t authlen	= size().bytes();
	uint32_t authoffset	= parent()->parent()->offset().bytes() +	// from Udp_DHCPv6_Solicit
				  parent()->offset().bytes() +			// from Opt_DHCPv6_Authentication
				  offset().bytes();				// from DHCPv6_Auth_Delayed
	uint32_t dhcpoffset	= parent()->parent()->parent()->offset().bytes();

	if(dhcpoffset0 != dhcpoffset) {
		return;
	}

	OCTBUF dhcpauthbuf(dhcplen, (OCTSTR)dhcpbuf, true);

	memset((void *)(dhcpauthbuf.string() + authoffset), 0, authlen);

	OCTBUF *calc = 0;

	if(dhcpauth_) {
		calc = dhcpauth_->DHCPAuth_Calculate(dhcpauthbuf);
	}

	set_calc_pvalue(calc);

	return;
}
#undef SUPER

////////////////////////////////////////////////////////////////
#define SUPER	WmObject
WmDHCPAuth::WmDHCPAuth(WObject *p, const MObject *m, const PObject *po, const PaDHCPAuth *dhcpauth):SUPER(p, m, po), dhcpauth_(dhcpauth) {}
WmDHCPAuth::~WmDHCPAuth(){}

void WmDHCPAuth::post_generate(Con_IPinfo &info, WControl &c, OCTBUF &buf, WObject *base) {
	WObject *w_auth = (WObject *)info.postDHCPAuth(); 
	if(!w_auth) {
		return;
	}

	const OCTBUF *basebuf	= (const OCTBUF *)base->pvalue();

	uint32_t dhcplen	= base->size().bytes();
	uint32_t dhcpoffset	= base->offset().bytes();
	OCTSTR dhcpbuf		= (OCTSTR)basebuf->string();

	uint32_t authlen	= w_auth->size().bytes();
	uint32_t authoffset	= w_auth->offset().bytes();

	if(dhcpoffset > authoffset) {
		return;
	}

	if(dhcpoffset + dhcplen < authoffset + authlen) {
		return;
	}

	OCTBUF dhcpauthbuf(dhcplen, (OCTSTR)dhcpbuf, true);

	OCTBUF *calc = 0;

	if(dhcpauth_) {
		calc = dhcpauth_->DHCPAuth_Calculate(dhcpauthbuf);
	}

	if(calc) {
		set_rgenerate(calc);
		SUPER::generate(c, buf);
	}
}

bool WmDHCPAuth::doEvaluate(WControl &c, RObject &r) {
	RmDHCPAuth &rm = (RmDHCPAuth &)r;
	const PvObject *ro = rm.pvalue();
	const PvObject *eo = rm.calc_pvalue();

	if(!eo) {
		eo = revaluate();
	}

	return(valueEvaluate(c, ro, eo));
}
#undef SUPER

////////////////////////////////////////////////////////////////
PaDHCPAuth::PaDHCPAuth(const MObject *m, CSTR st, int l):PcObject(m ,st, l), dhcpauth_(0) {}
PaDHCPAuth::~PaDHCPAuth() {
	if(dhcpauth_ != 0) {
		delete dhcpauth_;
		dhcpauth_ = 0;
	}
}

PObject *PaDHCPAuth::dhcpauth_member(PObject *p) {
	return(dhcpauth_ = (PfDHCPAuth *)(p));
}

OCTBUF *PaDHCPAuth::DHCPAuth_Calculate(const OCTBUF &s) const {
	if(!dhcpauth_) {
		return(0);
	}

	dhcpauth_->init();
	dhcpauth_->update(s);
	PvOctets *icv = dhcpauth_->result();

	return(icv);
}

WObject *PaDHCPAuth::selfCompose(WControl &c, WObject *w_parent) const {
	const MObject *m = meta();

	c.set_push_DHCPAuth(this);

	return(m != 0 ? m->compose(c, w_parent, this) : 0);
}

////////////////////////////////////////////////////////////////
PfDHCPAuth::PfDHCPAuth(const MfDHCPAuth *a, CSTR b, int c): PvFunction(a, b, c), meta_(a), context_(0) {}
PfDHCPAuth::~PfDHCPAuth() {};

const MObject *PfDHCPAuth::meta() const {
	return(metaClass());
}

void PfDHCPAuth::init() {
	const MfDHCPAuth *m = metaClass();

	if(m) {
		context_ = m->init(context_, args());
	}

	return;
}

void PfDHCPAuth::update(const OCTBUF &s) {
	const MfDHCPAuth *m = metaClass();

	if(m) {
		m->update(context_, args(), s);
	}

	return;
}

PvOctets *PfDHCPAuth::result() {
	const MfDHCPAuth *m = metaClass();

	PvOctets *ret = 0;

	if(m) {
		ret = m->result(context_, args());
	}

	return(ret);
}

////////////////////////////////////////////////////////////////
PvDHCPAuth::PvDHCPAuth(OCTSTR o, bool b): PvOctets(16, o, b) {}
PvDHCPAuth::~PvDHCPAuth() {}

void PvDHCPAuth::log(uint32_t t) const {
	print();
}

void PvDHCPAuth::print() const {
	COCTSTR buf = 0;
	uint32_t buflen = 0, d = 0;

	buf = string();
	buflen = length();

	for(d = 0; d < buflen; d ++) {
		printf("%02x", buf[d]);
	}
}

const PvDHCPAuth *PvDHCPAuth::zeroauth() {
	char tmp[16];
	memset(tmp, 0, 16);
	return(new PvDHCPAuth((OCTSTR)tmp, true));
}

PvObject *PvDHCPAuth::shallowCopy() const {
	return(new PvDHCPAuth((OCTSTR)string()));
}

////////////////////////////////////////////////////////////////
KeepDHCPAuth::KeepDHCPAuth() {
	current_ = 0;
	dhcpauth_ = new DHCPAuthList();
}

KeepDHCPAuth::~KeepDHCPAuth() {
	current_ = 0;

	if(dhcpauth_) {
		delete dhcpauth_;
		dhcpauth_ = 0;
	}
}

const PaDHCPAuth *KeepDHCPAuth::GetDHCPAuth(void) {
	uint32_t i = current_ ++;

	const PaDHCPAuth *dhcpauth = 0;

	if(dhcpauth_) {
		uint32_t i9 = dhcpauth_->size();
		dhcpauth = i < i9 ? dhcpauth_->index(i) : 0;
	}

	return(dhcpauth);
}

void KeepDHCPAuth::SetDHCPAuth(const PaDHCPAuth *dhcpauth) {
	dhcpauth_->append((PaDHCPAuth *)dhcpauth);
}

////////////////////////////////////////////////////////////////
#define AUTHZERO()    PvDHCPAuth::zeroauth()
MmDHCPAuth::MmDHCPAuth(CSTR s):MmOctets(s, 16, AUTHZERO(), AUTHZERO(), 0, 0) {
	keepdhcpauth_ = new KeepDHCPAuth();
}

MmDHCPAuth::~MmDHCPAuth() {
	if(keepdhcpauth_) {
		delete keepdhcpauth_;
		keepdhcpauth_ = 0;
	}
}

PvObject *MmDHCPAuth::reversePv(RControl &, const ItPosition &at, const ItPosition &size, const OCTBUF &buf) const {
	OCTSTR str = (OCTSTR)buf.string(at.bytes());
	return(new PvDHCPAuth(str, false));
}

RObject *MmDHCPAuth::reverseRm(RControl &c, RObject *r_parent, const ItPosition &at, const ItPosition &size, PvObject *pv) const {
	const PaDHCPAuth *dhcpauth = 0;

	if(keepdhcpauth_) {
		dhcpauth = keepdhcpauth_->GetDHCPAuth();
	}

	return(new RmDHCPAuth(r_parent, this, at, size, pv, dhcpauth));
}

WObject *MmDHCPAuth::composeWm(WControl &c, WObject *w_parent, const PObject *pl) const {
	const PaDHCPAuth *dhcpauth = c.pushing_DHCPAuth();

	if(keepdhcpauth_) {
		keepdhcpauth_->SetDHCPAuth(dhcpauth);
	}

	return(new WmDHCPAuth(w_parent, this, pl, dhcpauth));
}

RObject *MmDHCPAuth::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	RObject *r_self = MmOctets::reverse(c, r_parent, at, buf);
	if(!c.error()){
		Con_IPinfo *info = c.IPinfo();
		if(info) {
			info->postDHCPAuth(r_self);
		}
	}

	return(r_self);
}

bool MmDHCPAuth::generate(WControl &c, WObject *w_self, OCTBUF &buf) const {
	bool rtn = MmOctets::generate(c, w_self, buf);
	if(!c.error()) {
		Con_IPinfo *info = c.IPinfo();
		if(info) {
			info->postDHCPAuth(w_self);
		}
	}

        return(rtn);
}
#undef AUTHZERO

////////////////////////////////////////////////////////////////
MmDHCPRealm::MmDHCPRealm(CSTR s, const PObject *g, const PObject *e): MmVarOctets(s, g, e, 0, 0) {}
MmDHCPRealm::~MmDHCPRealm() {}

ItPosition MmDHCPRealm::length_for_reverse(RControl &c, ItPosition &at, OCTBUF &buf) const {
	uint32_t length = buf.remainLength(at.bytes()) - 20;
	return(ItPosition(length, 0));
}

////////////////////////////////////////////////////////////////
#define SUPER McHeader
McUdp_DHCPv6_ONE *McUdp_DHCPv6_ONE::instance_ = 0;

McUdp_DHCPv6_ONE *McUdp_DHCPv6_ONE::instance() {
	if(!instance_) {
		instance_ = new McUdp_DHCPv6_ONE("DHCPv6");
	}

	return(instance_);
}

McUdp_DHCPv6_ONE::McUdp_DHCPv6_ONE(CSTR key): SUPER(key) {
	member(new MmHeader_onDHCPv6("header"));

	MmUpper_onUpper::add_dhcp(this);
}

McUdp_DHCPv6_ONE::~McUdp_DHCPv6_ONE() {}

bool McUdp_DHCPv6_ONE::containsMc(const MObject *mc) const {
	bool rtn = SUPER::containsMc(mc);

	return(rtn ? rtn : members_[0]->containsMc(mc));
}

uint32_t McUdp_DHCPv6_ONE::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const {
	return(buf.remainLength(at.bytes()));
}

RObject *McUdp_DHCPv6_ONE::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	return(members_[0]->reverse(c, r_parent, at, buf));
}
#undef SUPER

////////////////////////////////////////////////////////////////
#define SUPER   McHeader
McUdp_DHCPv6::McUdp_DHCPv6(CSTR key): SUPER(key), type_(0) {
	McUdp_DHCPv6_ONE::instance();
}

McUdp_DHCPv6::~McUdp_DHCPv6() {}

int32_t McUdp_DHCPv6::token() const {
	return(metaToken(tkn_upper_));
}

uint32_t McUdp_DHCPv6::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const {
	return(buf.remainLength(at.bytes()));
}

bool McUdp_DHCPv6::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		ItPosition tmpat = at;
		RObject *rtype = type_->reverse(c, 0, tmpat, buf);

		if(!rtype) {
			return(false);
		}

		const PvNumber *pv = (const PvNumber *)rtype->pvalue();

		uint32_t typevalue = pv->value();

		c.DictType().type_Set(typevalue);

		delete(rtype);

		return(true);
	}

	return(false);
}

bool McUdp_DHCPv6::HCGENE(Type)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	int32_t val = get_dhcpv6Type(wmem);
	if(val < 0) {
		return(false);
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *McUdp_DHCPv6::HCEVAL(Type)(WObject *wmem) const {
	int32_t val = get_dhcpv6Type(wmem);
	return(new PvNumber(val));
}

int32_t McUdp_DHCPv6::get_dhcpv6Type(WObject *wmem) const {
	WObject *wc = wmem->parent();
	int32_t rtn = wc ? wc->meta()->dhcpv6Type() : -1;

	if(rtn < 0) {
		wmem->mustDefine(0);
	}

	return(rtn);
}

RObject *McUdp_DHCPv6::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	RObject *r_self = SUPER::reverse(c, r_parent, at, buf);
	if(!c.error()){
		Con_IPinfo *info = c.IPinfo();
		if(info) {
			info->reverse_postDHCPAuth(c, r_self);
		}
	}

	return(r_self);
}

bool McUdp_DHCPv6::generate(WControl &c, WObject *w_self, OCTBUF &buf) const {
	bool rtn = SUPER::generate(c, w_self, buf);
	if(!c.error()) {
		Con_IPinfo *info = c.IPinfo();

		if(info) {
			info->generate_postDHCPAuth(c, buf, w_self);
		}
	}

	return(rtn);
}
#undef SUPER

////////////////////////////////////////////////////////////////
McUdp_DHCPv6_ANY::McUdp_DHCPv6_ANY(CSTR key): McUdp_DHCPv6(key) {}
McUdp_DHCPv6_ANY::~McUdp_DHCPv6_ANY() {}

////////////////////////////////////////////////////////////////
McUdp_DHCPv6_Solicit::McUdp_DHCPv6_Solicit(CSTR key): McUdp_DHCPv6(key) {}
McUdp_DHCPv6_Solicit::~McUdp_DHCPv6_Solicit() {}

////////////////////////////////////////////////////////////////
McUdp_DHCPv6_Advertise::McUdp_DHCPv6_Advertise(CSTR key): McUdp_DHCPv6(key) {}
McUdp_DHCPv6_Advertise::~McUdp_DHCPv6_Advertise() {}

////////////////////////////////////////////////////////////////
McUdp_DHCPv6_Request::McUdp_DHCPv6_Request(CSTR key): McUdp_DHCPv6(key) {}
McUdp_DHCPv6_Request::~McUdp_DHCPv6_Request() {}

////////////////////////////////////////////////////////////////
McUdp_DHCPv6_Confirm::McUdp_DHCPv6_Confirm(CSTR key): McUdp_DHCPv6(key) {}
McUdp_DHCPv6_Confirm::~McUdp_DHCPv6_Confirm() {}

////////////////////////////////////////////////////////////////
McUdp_DHCPv6_Renew::McUdp_DHCPv6_Renew(CSTR key): McUdp_DHCPv6(key) {}
McUdp_DHCPv6_Renew::~McUdp_DHCPv6_Renew() {}

////////////////////////////////////////////////////////////////
McUdp_DHCPv6_Rebind::McUdp_DHCPv6_Rebind(CSTR key): McUdp_DHCPv6(key) {}
McUdp_DHCPv6_Rebind::~McUdp_DHCPv6_Rebind() {}

////////////////////////////////////////////////////////////////
McUdp_DHCPv6_Reply::McUdp_DHCPv6_Reply(CSTR key): McUdp_DHCPv6(key) {}
McUdp_DHCPv6_Reply::~McUdp_DHCPv6_Reply() {}

////////////////////////////////////////////////////////////////
McUdp_DHCPv6_Release::McUdp_DHCPv6_Release(CSTR key): McUdp_DHCPv6(key) {}
McUdp_DHCPv6_Release::~McUdp_DHCPv6_Release() {}

////////////////////////////////////////////////////////////////
McUdp_DHCPv6_Decline::McUdp_DHCPv6_Decline(CSTR key): McUdp_DHCPv6(key) {}
McUdp_DHCPv6_Decline::~McUdp_DHCPv6_Decline() {}

////////////////////////////////////////////////////////////////
McUdp_DHCPv6_Reconfigure::McUdp_DHCPv6_Reconfigure(CSTR key): McUdp_DHCPv6(key) {}
McUdp_DHCPv6_Reconfigure::~McUdp_DHCPv6_Reconfigure() {}

////////////////////////////////////////////////////////////////
McUdp_DHCPv6_InformationRequest::McUdp_DHCPv6_InformationRequest(CSTR key): McUdp_DHCPv6(key) {}
McUdp_DHCPv6_InformationRequest::~McUdp_DHCPv6_InformationRequest() {}

////////////////////////////////////////////////////////////////
McUdp_DHCPv6_RelayForward::McUdp_DHCPv6_RelayForward(CSTR key): McUdp_DHCPv6(key) {}
McUdp_DHCPv6_RelayForward::~McUdp_DHCPv6_RelayForward() {}

////////////////////////////////////////////////////////////////
McUdp_DHCPv6_RelayReply::McUdp_DHCPv6_RelayReply(CSTR key): McUdp_DHCPv6(key) {}
McUdp_DHCPv6_RelayReply::~McUdp_DHCPv6_RelayReply() {}

////////////////////////////////////////////////////////////////
#define DEF_LENGTH_OFFSET_OptDHCPv6	4

McOpt_DHCPv6::McOpt_DHCPv6(CSTR key): McOption(key), code_(0), length_(0) {}
McOpt_DHCPv6::~McOpt_DHCPv6() {}

uint32_t McOpt_DHCPv6::length_for_reverse(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(!length_) {
		return(McOption::length_for_reverse(c, at, buf));
	}

	uint32_t valulen = length_->value(at, buf);
	uint32_t length = valulen + DEF_LENGTH_OFFSET_OptDHCPv6;

	return(length);
}

bool McOpt_DHCPv6::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		ItPosition tmpat = at;

		RObject *rcode = code_->reverse(c, 0, tmpat,buf);
		if(!rcode) {
			return(false);
		}

		const PvNumber *pv = (const PvNumber *)rcode->pvalue();
		uint32_t codevalue = pv->value();
		c.DictType().type_Set(codevalue);

		delete(rcode);

		return(true);
	}

	return(false);
}

bool McOpt_DHCPv6::HCGENE(Length)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	WObject *wc = wmem->parent();

	uint32_t reallen = wc->size().bytes();
	uint32_t valulen = reallen - DEF_LENGTH_OFFSET_OptDHCPv6;

	PvNumber def(valulen);

	return(def.generate(cntr, wmem, buf));
}
#undef DEF_LENGTH_OFFSET_OptDHCPv6

bool McOpt_DHCPv6::HCGENE(Code)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	int32_t val = get_optionCode(wmem);
	if(val < 0) {
		return(false);
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *McOpt_DHCPv6::HCEVAL(Code)(WObject *wmem) const {
	int32_t val = get_optionCode(wmem);
	return(new PvNumber(val));
}

int32_t McOpt_DHCPv6::get_optionCode(WObject *wmem) const {
	WObject *wc = wmem->parent();
	int32_t rtn = wc ? wc->meta()->optionCode() : -1;

	if(rtn < 0) {
		wmem->mustDefine(0);
	}

	return(rtn);
}

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_ANY::McOpt_DHCPv6_ANY(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_ANY::~McOpt_DHCPv6_ANY() {}

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_CID::McOpt_DHCPv6_CID(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_CID::~McOpt_DHCPv6_CID() {}

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_SID::McOpt_DHCPv6_SID(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_SID::~McOpt_DHCPv6_SID() {}

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_IA_NA::McOpt_DHCPv6_IA_NA(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_IA_NA::~McOpt_DHCPv6_IA_NA() {}

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_IA_TA::McOpt_DHCPv6_IA_TA(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_IA_TA::~McOpt_DHCPv6_IA_TA() {}

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_IA_Address::McOpt_DHCPv6_IA_Address(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_IA_Address::~McOpt_DHCPv6_IA_Address() {}

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_OptionRequest::McOpt_DHCPv6_OptionRequest(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_OptionRequest::~McOpt_DHCPv6_OptionRequest() {}

uint32_t McOpt_DHCPv6_OptionRequest::HC_MLC(OptionCode)(const ItPosition &at, OCTBUF &buf) const {
	uint32_t length = length_->value(at, buf);
	return(length / 2);
} 

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_Preference::McOpt_DHCPv6_Preference(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_Preference::~McOpt_DHCPv6_Preference() {}

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_ElapsedTime::McOpt_DHCPv6_ElapsedTime(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_ElapsedTime::~McOpt_DHCPv6_ElapsedTime() {}

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_RelayMessage::McOpt_DHCPv6_RelayMessage(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_RelayMessage::~McOpt_DHCPv6_RelayMessage() {}

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_Authentication::McOpt_DHCPv6_Authentication(CSTR key): McOpt_DHCPv6(key), proto_(0) {
	AuthInfo_ = new AuthInfo();
}

McOpt_DHCPv6_Authentication::~McOpt_DHCPv6_Authentication() {}

void McOpt_DHCPv6_Authentication::nextAuth_set_Control(RControl &c, RObject *r_self) const {
	if(AuthInfo_) {
		RObject *next = (RObject *)r_self->corresponding(proto_);
		if(next){
			PvNumber *pv = (PvNumber *)next->pvalue();
			AuthInfo_->set_protocol(pv->value());
		}
	}

	return;
}

RObject *McOpt_DHCPv6_Authentication::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	ItPosition tmpat = at;

	RObject *r_self = McOpt_DHCPv6::reverse(c, 0, tmpat, buf);
	if(r_self) {
		nextAuth_set_Control(c, r_self);
	}

	return(McOpt_DHCPv6::reverse(c, r_parent, at, buf));
}

bool McOpt_DHCPv6_Authentication::HCGENE(Protocol)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	int32_t val = get_authProto(wmem);
	if(val < 0) {
		return(false);
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *McOpt_DHCPv6_Authentication::HCEVAL(Protocol)(WObject *wmem) const {
	int32_t val = get_authProto(wmem);
	return(new PvNumber(val));
}

int32_t McOpt_DHCPv6_Authentication::get_authProto(WObject *wmem) const {
	WObject *wc = 0;

	WObject *wparent = wmem->parent();

	if(wparent) {
		for(WObject *x = wparent->nextChild(); x; x = x->nextSister()) {
			wc = x;
		}
	}

	int32_t rtn = wc ? wc->meta()->authProto() : -1;

	if(rtn < 0) {
		wmem->mustDefine(0);
	}

	return(rtn);
}

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_ServerUnicast::McOpt_DHCPv6_ServerUnicast(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_ServerUnicast::~McOpt_DHCPv6_ServerUnicast() {}

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_StatusCode::McOpt_DHCPv6_StatusCode(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_StatusCode::~McOpt_DHCPv6_StatusCode() {}

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_RapidCommit::McOpt_DHCPv6_RapidCommit(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_RapidCommit::~McOpt_DHCPv6_RapidCommit() {}

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_UserClass::McOpt_DHCPv6_UserClass(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_UserClass::~McOpt_DHCPv6_UserClass() {}

////////////////////////////////////////////////////////////////
#define DEF_LENGTH_OFFSET_DHCPv6_UserClassData	2
McDHCPv6_UserClassData::McDHCPv6_UserClassData(CSTR key): McOption(key), length_(0) {}
McDHCPv6_UserClassData::~McDHCPv6_UserClassData() {}

uint32_t McDHCPv6_UserClassData::length_for_reverse(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(!length_) {
		return(McOption::length_for_reverse(c, at, buf));
	}

	uint32_t valulen = length_->value(at, buf);
	uint32_t length = valulen + DEF_LENGTH_OFFSET_DHCPv6_UserClassData;

	return(length);
}

bool McDHCPv6_UserClassData::HCGENE(Length)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	WObject *wc = wmem->parent();

	uint32_t reallen = wc->size().bytes();
	uint32_t valulen = reallen - DEF_LENGTH_OFFSET_DHCPv6_UserClassData;

	PvNumber def(valulen);

	return(def.generate(cntr, wmem, buf));
}
#undef DEF_LENGTH_OFFSET_DHCPv6_UserClassData

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_VendorClass::McOpt_DHCPv6_VendorClass(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_VendorClass::~McOpt_DHCPv6_VendorClass() {}

////////////////////////////////////////////////////////////////
#define DEF_LENGTH_OFFSET_DHCPv6_VendorClassData	2
McDHCPv6_VendorClassData::McDHCPv6_VendorClassData(CSTR key): McOption(key), length_(0) {}
McDHCPv6_VendorClassData::~McDHCPv6_VendorClassData() {}

uint32_t McDHCPv6_VendorClassData::length_for_reverse(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(!length_) {
		return(McOption::length_for_reverse(c, at, buf));
	}

	uint32_t valulen = length_->value(at, buf);
	uint32_t length = valulen + DEF_LENGTH_OFFSET_DHCPv6_VendorClassData;

	return(length);
}

bool McDHCPv6_VendorClassData::HCGENE(Length)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	WObject *wc = wmem->parent();

	uint32_t reallen = wc->size().bytes();
	uint32_t valulen = reallen - DEF_LENGTH_OFFSET_DHCPv6_VendorClassData;

	PvNumber def(valulen);

	return(def.generate(cntr, wmem, buf));
}
#undef DEF_LENGTH_OFFSET_DHCPv6_VendorClassData

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_VendorSpecificInfo::McOpt_DHCPv6_VendorSpecificInfo(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_VendorSpecificInfo::~McOpt_DHCPv6_VendorSpecificInfo() {}

////////////////////////////////////////////////////////////////
#define DEF_LENGTH_OFFSET_DHCPv6_OptionData	4
McDHCPv6_OptionData::McDHCPv6_OptionData(CSTR key): McOption(key), length_(0) {}
McDHCPv6_OptionData::~McDHCPv6_OptionData() {}

uint32_t McDHCPv6_OptionData::length_for_reverse(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(!length_) {
		return(McOption::length_for_reverse(c, at, buf));
	}

	uint32_t valulen = length_->value(at, buf);
	uint32_t length = valulen + DEF_LENGTH_OFFSET_DHCPv6_OptionData;

	return(length);
}

bool McDHCPv6_OptionData::HCGENE(Length)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	WObject *wc = wmem->parent();

	uint32_t reallen = wc->size().bytes();
	uint32_t valulen = reallen - DEF_LENGTH_OFFSET_DHCPv6_OptionData;

	PvNumber def(valulen);

	return(def.generate(cntr, wmem, buf));
}
#undef DEF_LENGTH_OFFSET_DHCPv6_OptionData

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_IID::McOpt_DHCPv6_IID(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_IID::~McOpt_DHCPv6_IID() {}

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_ReconfigureMessage::McOpt_DHCPv6_ReconfigureMessage(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_ReconfigureMessage::~McOpt_DHCPv6_ReconfigureMessage() {}

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_ReconfigureAccept::McOpt_DHCPv6_ReconfigureAccept(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_ReconfigureAccept::~McOpt_DHCPv6_ReconfigureAccept() {}

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_IA_PD::McOpt_DHCPv6_IA_PD(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_IA_PD::~McOpt_DHCPv6_IA_PD() {}

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_IA_Prefix::McOpt_DHCPv6_IA_Prefix(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_IA_Prefix::~McOpt_DHCPv6_IA_Prefix() {}

////////////////////////////////////////////////////////////////
#define SERCHSTROFFSET	4
#define SERCHSTRLEN	16

McOpt_DHCPv6_DNS_Servers::McOpt_DHCPv6_DNS_Servers(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_DNS_Servers::~McOpt_DHCPv6_DNS_Servers() {}

uint32_t McOpt_DHCPv6_DNS_Servers::HC_MLC(Address)(const ItPosition &at, OCTBUF &buf) const {
	uint32_t length = (buf.remainLength(at.bytes()) - SERCHSTROFFSET) / SERCHSTRLEN;
	return(length);
} 

#undef SERCHSTRLEN
#undef SERCHSTROFFSET

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_DNS_SearchList::McOpt_DHCPv6_DNS_SearchList(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_DNS_SearchList::~McOpt_DHCPv6_DNS_SearchList() {}

////////////////////////////////////////////////////////////////
#define NTPSVOFFSET	4
#define NTPSVLEN	16

McOpt_DHCPv6_NTP_Servers::McOpt_DHCPv6_NTP_Servers(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_NTP_Servers::~McOpt_DHCPv6_NTP_Servers() {}

uint32_t McOpt_DHCPv6_NTP_Servers::HC_MLC(Address)(const ItPosition &at, OCTBUF &buf) const {
	uint32_t length = (buf.remainLength(at.bytes()) - NTPSVOFFSET) / NTPSVLEN;
	return(length);
} 

#undef NTPSVLEN
#undef NTPSVOFFSET

////////////////////////////////////////////////////////////////
McOpt_DHCPv6_NTP_Timezone::McOpt_DHCPv6_NTP_Timezone(CSTR key): McOpt_DHCPv6(key) {}
McOpt_DHCPv6_NTP_Timezone::~McOpt_DHCPv6_NTP_Timezone() {}

////////////////////////////////////////////////////////////////
McDHCPv6_DUID::McDHCPv6_DUID(CSTR key): McUpper(key), type_(0) {}
McDHCPv6_DUID::~McDHCPv6_DUID() {}

uint32_t McDHCPv6_DUID::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const {
	return(buf.remainLength(at.bytes()));
}

bool McDHCPv6_DUID::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		ItPosition tmpat = at;

		RObject *rtype = type_->reverse(c, 0, tmpat,buf);
		if(!rtype) {
			return(false);
		}

		const PvNumber *pv = (const PvNumber *)rtype->pvalue();
		uint32_t typevalue = pv->value();
		c.DictType().type_Set(typevalue);

		delete(rtype);

		return(true);
	}

	return(false);
}
#undef DEF_LENGTH_OFFSET_OptDHCPv6

bool McDHCPv6_DUID::HCGENE(Type)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	int32_t val = get_duidType(wmem);
	if(val < 0) {
		return(false);
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *McDHCPv6_DUID::HCEVAL(Type)(WObject *wmem) const {
	int32_t val = get_duidType(wmem);
	return(new PvNumber(val));
}

int32_t McDHCPv6_DUID::get_duidType(WObject *wmem) const {
	WObject *wc = wmem->parent();
	int32_t rtn = wc ? wc->meta()->duidType() : -1;

	if(rtn < 0) {
		wmem->mustDefine(0);
	}

	return(rtn);
}

////////////////////////////////////////////////////////////////
McDHCPv6_DUID_ANY::McDHCPv6_DUID_ANY(CSTR key): McDHCPv6_DUID(key) {}
McDHCPv6_DUID_ANY::~McDHCPv6_DUID_ANY() {}

////////////////////////////////////////////////////////////////
McDHCPv6_DUID_LLT_ONE *McDHCPv6_DUID_LLT_ONE::instance_ = 0;

McDHCPv6_DUID_LLT_ONE *McDHCPv6_DUID_LLT_ONE::instance() {
	if(!instance_) {
		instance_ = new McDHCPv6_DUID_LLT_ONE("DUID_LLT");
	}

	return(instance_);
}

McDHCPv6_DUID_LLT_ONE::McDHCPv6_DUID_LLT_ONE(CSTR key): McDHCPv6_DUID(key) {
	member(new MmLLT_onDUID("llt"));

	MmDUID_onOption::add(this);
}

McDHCPv6_DUID_LLT_ONE::~McDHCPv6_DUID_LLT_ONE() {}

bool McDHCPv6_DUID_LLT_ONE::containsMc(const MObject *mc) const {
	bool rtn = McDHCPv6_DUID::containsMc(mc);

	return(rtn ? rtn : members_[0]->containsMc(mc));
}

uint32_t McDHCPv6_DUID_LLT_ONE::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const {
	return(buf.remainLength(at.bytes()));
}

RObject *McDHCPv6_DUID_LLT_ONE::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	return(members_[0]->reverse(c, r_parent, at, buf));
}

////////////////////////////////////////////////////////////////
McDHCPv6_DUID_LLT::McDHCPv6_DUID_LLT(CSTR key): McDHCPv6_DUID(key) {
	McDHCPv6_DUID_LLT_ONE::instance();
}

McDHCPv6_DUID_LLT::~McDHCPv6_DUID_LLT() {}

uint32_t McDHCPv6_DUID_LLT::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const {
	return(buf.remainLength(at.bytes()));
}

bool McDHCPv6_DUID_LLT::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		ItPosition tmpat = at;

		RObject *r_self = McDHCPv6_DUID::reverse(c, 0, tmpat, buf);
		RObject *rtype = (RObject *)r_self->corresponding(hwtype_);
		if(!rtype) {
			return(false);
		}

		const PvNumber *pv = (const PvNumber *)rtype->pvalue();

		uint32_t typevalue = pv->value();

		c.DictType().type_Set(typevalue);

		delete(rtype);

		return(true);
	}

	return(false);
}

bool McDHCPv6_DUID_LLT::HCGENE(HardwareType)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	int32_t val = get_hardwareType(wmem);
	if(val < 0) {
		return(false);
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *McDHCPv6_DUID_LLT::HCEVAL(HardwareType)(WObject *wmem) const {
	int32_t val = get_hardwareType(wmem);
	return(new PvNumber(val));
}

int32_t McDHCPv6_DUID_LLT::get_hardwareType(WObject *wmem) const {
	WObject *wc = wmem->parent();
	int32_t rtn = wc ? wc->meta()->hardwareType() : -1;
	if(rtn < 0) {
		wmem->mustDefine(0);
	}

	return(rtn);
}

////////////////////////////////////////////////////////////////
McDHCPv6_DUID_LLT_ANY::McDHCPv6_DUID_LLT_ANY(CSTR key): McDHCPv6_DUID_LLT(key) {}
McDHCPv6_DUID_LLT_ANY::~McDHCPv6_DUID_LLT_ANY() {}

////////////////////////////////////////////////////////////////
McDHCPv6_DUID_LLT_Ether::McDHCPv6_DUID_LLT_Ether(CSTR key): McDHCPv6_DUID_LLT(key) {}
McDHCPv6_DUID_LLT_Ether::~McDHCPv6_DUID_LLT_Ether() {}

////////////////////////////////////////////////////////////////
McDHCPv6_DUID_EN::McDHCPv6_DUID_EN(CSTR key): McDHCPv6_DUID(key) {}
McDHCPv6_DUID_EN::~McDHCPv6_DUID_EN() {}

////////////////////////////////////////////////////////////////
McDHCPv6_DUID_LL_ONE *McDHCPv6_DUID_LL_ONE::instance_ = 0;

McDHCPv6_DUID_LL_ONE *McDHCPv6_DUID_LL_ONE::instance() {
	if(!instance_) {
		instance_ = new McDHCPv6_DUID_LL_ONE("DUID_LL");
	}

	return(instance_);
}

McDHCPv6_DUID_LL_ONE::McDHCPv6_DUID_LL_ONE(CSTR key): McDHCPv6_DUID(key) {
	member(new MmLL_onDUID("ll"));

	MmDUID_onOption::add(this);
}

McDHCPv6_DUID_LL_ONE::~McDHCPv6_DUID_LL_ONE() {}

bool McDHCPv6_DUID_LL_ONE::containsMc(const MObject *mc) const {
	bool rtn = McDHCPv6_DUID::containsMc(mc);

	return(rtn ? rtn : members_[0]->containsMc(mc));
}

uint32_t McDHCPv6_DUID_LL_ONE::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const {
	return(buf.remainLength(at.bytes()));
}

RObject *McDHCPv6_DUID_LL_ONE::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	return(members_[0]->reverse(c, r_parent, at, buf));
}

////////////////////////////////////////////////////////////////
McDHCPv6_DUID_LL::McDHCPv6_DUID_LL(CSTR key): McDHCPv6_DUID(key) {
	McDHCPv6_DUID_LL_ONE::instance();
}

McDHCPv6_DUID_LL::~McDHCPv6_DUID_LL() {}

uint32_t McDHCPv6_DUID_LL::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const {
	return(buf.remainLength(at.bytes()));
}

bool McDHCPv6_DUID_LL::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		ItPosition tmpat = at;
		RObject *r_self = McDHCPv6_DUID::reverse(c, 0, tmpat, buf);
		RObject *rtype = (RObject *)r_self->corresponding(hwtype_);
		if(!rtype) {
			return(false);
		}

		const PvNumber *pv = (const PvNumber *)rtype->pvalue();

		uint32_t typevalue = pv->value();

		c.DictType().type_Set(typevalue);

		delete(rtype);

		return(true);
	}

	return(false);
}

bool McDHCPv6_DUID_LL::HCGENE(HardwareType)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	int32_t val = get_hardwareType(wmem);
	if(val < 0) {
		return(false);
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *McDHCPv6_DUID_LL::HCEVAL(HardwareType)(WObject *wmem) const {
	int32_t val = get_hardwareType(wmem);
	return(new PvNumber(val));
}

int32_t McDHCPv6_DUID_LL::get_hardwareType(WObject *wmem) const {
	WObject *wc = wmem->parent();
	int32_t rtn = wc ? wc->meta()->hardwareType() : -1;
	if(rtn < 0) {
		wmem->mustDefine(0);
	}

	return(rtn);
}

////////////////////////////////////////////////////////////////
McDHCPv6_DUID_LL_ANY::McDHCPv6_DUID_LL_ANY(CSTR key): McDHCPv6_DUID_LL(key) {}
McDHCPv6_DUID_LL_ANY::~McDHCPv6_DUID_LL_ANY() {}

////////////////////////////////////////////////////////////////
McDHCPv6_DUID_LL_Ether::McDHCPv6_DUID_LL_Ether(CSTR key): McDHCPv6_DUID_LL(key) {}
McDHCPv6_DUID_LL_Ether::~McDHCPv6_DUID_LL_Ether() {}

////////////////////////////////////////////////////////////////
McDHCPv6_Auth::McDHCPv6_Auth(CSTR key): McUpper(key) {}
McDHCPv6_Auth::~McDHCPv6_Auth() {}

uint32_t McDHCPv6_Auth::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const {
	return(buf.remainLength(at.bytes()));
}

////////////////////////////////////////////////////////////////
McDHCPv6_Auth_ANY::McDHCPv6_Auth_ANY(CSTR key): McDHCPv6_Auth(key) {}
McDHCPv6_Auth_ANY::~McDHCPv6_Auth_ANY() {}

////////////////////////////////////////////////////////////////
McDHCPv6_Auth_Delayed::McDHCPv6_Auth_Delayed(CSTR key): McDHCPv6_Auth(key) {}
McDHCPv6_Auth_Delayed::~McDHCPv6_Auth_Delayed() {}

PObject *McDHCPv6_Auth_Delayed::tokenObject(int l, CSTR f) const {
	return(new PaDHCPAuth(this, f, l));
}

////////////////////////////////////////////////////////////////
McDHCPv6_Auth_ReconfigureKey::McDHCPv6_Auth_ReconfigureKey(CSTR key): McDHCPv6_Auth(key) {}
McDHCPv6_Auth_ReconfigureKey::~McDHCPv6_Auth_ReconfigureKey() {}

PObject *McDHCPv6_Auth_ReconfigureKey::tokenObject(int l, CSTR f) const {
	return(new PaDHCPAuth(this, f, l));
}

////////////////////////////////////////////////////////////////
MmHeader_onDHCPv6::MmHeader_onDHCPv6(CSTR key): MmReference_Less1(key, true) {}

MmHeader_onDHCPv6::~MmHeader_onDHCPv6() {}

void MmHeader_onDHCPv6::add(McUdp_DHCPv6 *mc) {
	dict_.add(mc->dhcpv6Type(), mc);
}

void MmHeader_onDHCPv6::add_other(McUdp_DHCPv6 *mc) {
	dict_.add_other(mc);
}

TypevsMcDict MmHeader_onDHCPv6::dict_;

bool MmHeader_onDHCPv6::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McUdp_DHCPv6 *any = (McUdp_DHCPv6 *)dict_.find_other();
		return(any->overwrite_DictType(c, at, buf));
	}

	return(false);
}

////////////////////////////////////////////////////////////////
MmOption_onDHCPv6::MmOption_onDHCPv6(CSTR key): MmReference_More0(key, true) {}
MmOption_onDHCPv6::~MmOption_onDHCPv6() {}

void MmOption_onDHCPv6::add(McOpt_DHCPv6 *mc) {
	dict_.add(mc->optionCode(), mc);
}

void MmOption_onDHCPv6::add_other(McOpt_DHCPv6 *mc) {
	dict_.add_other(mc);
}

TypevsMcDict MmOption_onDHCPv6::dict_;

bool MmOption_onDHCPv6::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McOpt_DHCPv6 *any = (McOpt_DHCPv6 *)dict_.find_other();

		return(any->overwrite_DictType(c, at, buf));
	}

	return(false);
}

////////////////////////////////////////////////////////////////
MmDUID_onOption::MmDUID_onOption(CSTR key): MmReference_Less1(key, true) {}
MmDUID_onOption::~MmDUID_onOption() {}

void MmDUID_onOption::add(McDHCPv6_DUID *mc) {
	dict_.add(mc->duidType(), mc);
}

void MmDUID_onOption::add_other(McDHCPv6_DUID *mc) {
	dict_.add_other(mc);
}

TypevsMcDict MmDUID_onOption::dict_;

bool MmDUID_onOption::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McDHCPv6_DUID *any = (McDHCPv6_DUID *)dict_.find_other();
		return(any->overwrite_DictType(c, at, buf));
	}

	return(false);
}

////////////////////////////////////////////////////////////////
MmLLT_onDUID::MmLLT_onDUID(CSTR key): MmReference_Must1(key, true) {}
MmLLT_onDUID::~MmLLT_onDUID() {}

void MmLLT_onDUID::add(McDHCPv6_DUID_LLT *mc) {
	dict_.add(mc->hardwareType(), mc);
}

void MmLLT_onDUID::add_other(McDHCPv6_DUID_LLT *mc) {
	dict_.add_other(mc);
}

TypevsMcDict MmLLT_onDUID::dict_;

bool MmLLT_onDUID::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McDHCPv6_DUID_LLT *any = (McDHCPv6_DUID_LLT *)dict_.find_other();
		return(any->overwrite_DictType(c, at, buf));
	}

	return(false);
}

////////////////////////////////////////////////////////////////
MmLL_onDUID::MmLL_onDUID(CSTR key): MmReference_Must1(key, true) {}
MmLL_onDUID::~MmLL_onDUID() {}

void MmLL_onDUID::add(McDHCPv6_DUID_LL *mc) {
	dict_.add(mc->hardwareType(), mc);
}

void MmLL_onDUID::add_other(McDHCPv6_DUID_LL *mc) {
	dict_.add_other(mc);
}

TypevsMcDict MmLL_onDUID::dict_;

bool MmLL_onDUID::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McDHCPv6_DUID_LL *any = (McDHCPv6_DUID_LL *)dict_.find_other();
		return(any->overwrite_DictType(c, at, buf));
	}

	return(false);
}

////////////////////////////////////////////////////////////////
MmUserClass_onOption::MmUserClass_onOption(CSTR key): MmReference_More0(key, true) {}
MmUserClass_onOption::~MmUserClass_onOption() {}

void MmUserClass_onOption::add_other(McDHCPv6_UserClassData *mc) {
	dict_.add_other(mc);
}

TypevsMcDict MmUserClass_onOption::dict_;

bool MmUserClass_onOption::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		c.DictType().other_Set();
		return(true);
	}

	return(false);
}

////////////////////////////////////////////////////////////////
MmVendorClass_onOption::MmVendorClass_onOption(CSTR key): MmReference_More0(key, true) {}
MmVendorClass_onOption::~MmVendorClass_onOption() {}

void MmVendorClass_onOption::add_other(McDHCPv6_VendorClassData *mc) {
	dict_.add_other(mc);
}

TypevsMcDict MmVendorClass_onOption::dict_;

bool MmVendorClass_onOption::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		c.DictType().other_Set();
		return(true);
	}

	return(false);
}

////////////////////////////////////////////////////////////////
MmOptionData_onOption::MmOptionData_onOption(CSTR key): MmReference_More0(key, true) {}
MmOptionData_onOption::~MmOptionData_onOption() {}

void MmOptionData_onOption::add_other(McDHCPv6_OptionData *mc) {
	dict_.add_other(mc);
}

TypevsMcDict MmOptionData_onOption::dict_;

bool MmOptionData_onOption::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		c.DictType().other_Set();
		return(true);
	}

	return(false);
}

////////////////////////////////////////////////////////////////
MmProto_onAuth::MmProto_onAuth(CSTR key, McOpt_DHCPv6_Authentication *opt): MmReference_Must1(key, true), opt_auth_(opt) {}
MmProto_onAuth::~MmProto_onAuth() {}

void MmProto_onAuth::add(McDHCPv6_Auth *mc) {
	dict_.add(mc->authProto(), mc);
}

void MmProto_onAuth::add_other(McDHCPv6_Auth *mc) {
	dict_.add_other(mc);
}

TypevsMcDict MmProto_onAuth::dict_;

bool MmProto_onAuth::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		const McOpt_DHCPv6_Authentication *opt = (const McOpt_DHCPv6_Authentication *)get_opt_auth();

		if(opt) {
			AuthInfo *info = opt->get_auth_info();
			if(info) {
				c.DictType().type_Set(info->get_protocol());
				return(true);
			}
		}
	}

	return(false);
}

implementCmList(DHCPAuthList, PaDHCPAuth);

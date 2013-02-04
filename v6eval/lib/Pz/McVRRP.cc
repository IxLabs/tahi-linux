//
// Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011
// Yokogawa Electric Corporation, YDC Corporation,
// IPA (Information-technology Promotion Agency, Japan).
// All rights reserved.
// 
// Redistribution and use of this software in source and binary forms, with 
// or without modification, are permitted provided that the following 
// conditions and disclaimer are agreed and accepted by the user:
// 
// 1. Redistributions of source code must retain the above copyright 
// notice, this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright 
// notice, this list of conditions and the following disclaimer in the 
// documentation and/or other materials provided with the distribution.
// 
// 3. Neither the names of the copyrighters, the name of the project which 
// is related to this software (hereinafter referred to as "project") nor 
// the names of the contributors may be used to endorse or promote products 
// derived from this software without specific prior written permission.
// 
// 4. No merchantable use may be permitted without prior written 
// notification to the copyrighters. However, using this software for the 
// purpose of testing or evaluating any products including merchantable 
// products may be permitted without any notification to the copyrighters.
// 
// 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHTERS, THE PROJECT AND 
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING 
// BUT NOT LIMITED THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.  IN NO EVENT SHALL THE 
// COPYRIGHTERS, THE PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT,STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
// THE POSSIBILITY OF SUCH DAMAGE.
//
// $TAHI: v6eval/lib/Pz/McVRRP.cc,v 1.2 2004/09/22 08:54:13 akisada Exp $
//

#include "McVRRP.h"
#include "debug.h"

////////////////////////////////////////////////////////////////////////
McUpp_VRRP_ONE *McUpp_VRRP_ONE::instance_ = 0;

McUpp_VRRP_ONE *
McUpp_VRRP_ONE::instance()
{
	if(!instance_) {
		instance_ = new McUpp_VRRP_ONE("VRRP");
	}

        return(instance_);
}

McUpp_VRRP_ONE::McUpp_VRRP_ONE(CSTR key): McUpper(key)
{
	member(new MmHeader_onVRRP("header"));
	MmUpper_onIP::add(this);
}

McUpp_VRRP_ONE::~McUpp_VRRP_ONE() {}

bool
McUpp_VRRP_ONE::containsMc(const MObject *mc) const
{
	bool rtn = McUpper::containsMc(mc);
	return(rtn? rtn: members_[0]->containsMc(mc));
}

uint32_t
McUpp_VRRP_ONE::length_for_reverse(RControl &,
	ItPosition &at, OCTBUF &buf) const
{
#ifdef VRRP_DBG
xdbg("/tmp/vrrp_dbg.txt", "McUpp_VRRP_ONE", "%d\n",
	buf.remainLength(at.bytes()));
#endif	// VRRP_DBG
	return(buf.remainLength(at.bytes()));
}

RObject *
McUpp_VRRP_ONE::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	return(members_[0]->reverse(c, r_parent, at, buf));
}



////////////////////////////////////////////////////////////////////////
McUpp_VRRP::McUpp_VRRP(CSTR key): McHeader(key), version_(0) {
	McUpp_VRRP_ONE::instance();
}

McUpp_VRRP::~McUpp_VRRP() {}

int32_t
McUpp_VRRP::token() const
{
	return(metaToken(tkn_upper_));
}

uint32_t
McUpp_VRRP::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const
{
#ifdef VRRP_DBG
xdbg("/tmp/vrrp_dbg.txt", "McUpp_VRRP", "%d\n",
	buf.remainLength(at.bytes()));
#endif	// VRRP_DBG
	return(buf.remainLength(at.bytes()));
}

bool
McUpp_VRRP::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const
{
	uint32_t limit = buf.remainLength(at.bytes());

	if(!limit) {
		return(false);
	}

	ItPosition tmpat = at;

	RObject *r_self = McHeader::reverse(c, 0, tmpat, buf);
	if(!r_self) {
		return(false);
	}

	RObject *rtype = (RObject *)r_self->corresponding(version_);

	if(!rtype) {
		return(false);
	}

	const PvNumber *pv = (const PvNumber *)rtype->pvalue();
	uint32_t typevalue = pv->value();

	c.DictType().type_Set(typevalue);
	delete rtype;

	return(true);
}

RObject *
McUpp_VRRP::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	RObject *r_self = McHeader::reverse(c, r_parent, at, buf);

	if(!c.error()) {
		Con_IPinfo *info = c.IPinfo();
		if(info) {
			info->reverse_postUppChecksum(c, r_self);
		}
	}

	return(r_self);
}

bool
McUpp_VRRP::generate(WControl &c, WObject *w_self, OCTBUF &buf) const
{
	bool rtn = McHeader::generate(c, w_self, buf);

	if(!c.error()) {
		Con_IPinfo *info = c.IPinfo();
		if(info) {
			info->generate_postUppChecksum(c, buf, w_self);
		}
	}

	return(rtn);
}



////////////////////////////////////////////////////////////////////////
McUpp_VRRPvX::McUpp_VRRPvX(CSTR key): McUpp_VRRP(key) {}
McUpp_VRRPvX::~McUpp_VRRPvX() {}



////////////////////////////////////////////////////////////////////////
McUpp_VRRPv2::McUpp_VRRPv2(CSTR key): McUpp_VRRP(key) {}
McUpp_VRRPv2::~McUpp_VRRPv2() {}

uint32_t
McUpp_VRRPv2::HC_MLC(Address)(const ItPosition &at, OCTBUF &buf) const
{
	uint32_t count = (buf.remainLength(at.bytes()) - 8) / 4;
	return(count);
}



////////////////////////////////////////////////////////////////////////
McUpp_VRRPv3::McUpp_VRRPv3(CSTR key): McUpp_VRRP(key) {}
McUpp_VRRPv3::~McUpp_VRRPv3() {}

uint32_t
McUpp_VRRPv3::HC_MLC(Address)(const ItPosition &at, OCTBUF &buf) const
{
	uint32_t count = (buf.remainLength(at.bytes()) - 8) / 16;
	return(count);
}



////////////////////////////////////////////////////////////////////////
MmHeader_onVRRP::MmHeader_onVRRP(CSTR key): MmReference_Must1(key, true) {}
MmHeader_onVRRP::~MmHeader_onVRRP() {}

void
MmHeader_onVRRP::add(McUpp_VRRP *mc)
{
	dict_.add(mc->vrrpVersion(), mc);
	return;
}

void
MmHeader_onVRRP::add_other(McUpp_VRRP *mc)
{
	dict_.add_other(mc);
	return;
}

TypevsMcDict MmHeader_onVRRP::dict_;

bool
MmHeader_onVRRP::overwrite_DictType(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	McUpp_VRRP *any = (McUpp_VRRP *)dict_.find_other();
	return(any->overwrite_DictType(c, at, buf));
}

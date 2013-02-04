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
// $TAHI: v6eval/lib/Pz/McIKE.cc,v 1.3 2006/01/18 05:09:59 akisada Exp $
//

#include "McSub.h"  
#include "MmHeader.h"
#include "ItPosition.h"
#include "WObject.h"
#include "RObject.h"
#include "PControl.h"  
#include "PvObject.h"
#include "PvOctets.h"
#include "PAlgorithm.h"
#include "McIKE.h"
#include "McESP.h"
#include "CmMain.h"
#include "PvAutoItem.h"
#include "PvAction.h"
#include "MmObject.h"
#include "MfAlgorithm.h"
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>

#include "debug.h"



uint32_t ISAKMP_Message_ID = 0;



//
// ISAKMP
//
////////////////////////////////

#define SUPER	McUpper
McUdp_ISAKMP *McUdp_ISAKMP::instance_	= 0;
McHdr_ISAKMP *McUdp_ISAKMP::tophdr_	= 0;

McUdp_ISAKMP::McUdp_ISAKMP(CSTR key): SUPER(key) { instance_ = this; }
McUdp_ISAKMP::~McUdp_ISAKMP() { if(instance_ == this) { instance_ = 0; } }

uint32_t
McUdp_ISAKMP::length_for_reverse(RControl &c, ItPosition &at, OCTBUF &buf) const
{
	return(tophdr_->Layerlength_for_reverse(c, at, buf));
}
#undef SUPER



//
// ISAKMP Header
//
////////////////////////////////

EFlagValueCtrl::EFlagValueCtrl(): e(0) {}
EFlagValueCtrl::~EFlagValueCtrl() {}

#define SUPER McHeader
McHdr_ISAKMP::McHdr_ISAKMP(CSTR key):
	SUPER(key), Layerlength_(0), EFlag_(0), ectrl_(new EFlagValueCtrl()),
	MID_(0) {}

McHdr_ISAKMP::~McHdr_ISAKMP() { if(ectrl_) { delete ectrl_; ectrl_ = 0; } }

void
McHdr_ISAKMP::MIDmember(MmUint *meta)
{
	MID_ = meta;
	member(meta);
	return;
}

uint32_t
McHdr_ISAKMP::Layerlength_for_reverse(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	uint32_t e = EFlag_? EFlag_->value(at, buf): 0;

	if(ectrl_) { ectrl_->set_e(e); }

	ISAKMP_Message_ID = MID_? MID_->value(at, buf): 0;

	return(
		Layerlength_?
		Layerlength_->value(at, buf):
		SUPER::Layerlength_for_reverse(c, at, buf)
	);
}

bool
McHdr_ISAKMP::HCGENE(Length)(WControl &cntr, WObject *wmem, OCTBUF &buf) const
{
	WObject *wc		= wmem->parent();       // McHdr_ISAKMP
	WObject *packet		= wc->parent();         // McUdp_ISAKMP
	uint32_t pktlen		= packet->size().bytes();
	PvNumber def(pktlen);

	return(def.generate(cntr, wmem, buf));
}

bool
McHdr_ISAKMP::HCGENE(NextPayload)(WControl &cntr,
	WObject *wmem, OCTBUF &buf) const
{
	int32_t val = get_next_headerType(wmem);

	if(val == -1) { return(false); }

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *
McHdr_ISAKMP::HCEVAL(NextPayload)(WObject *wmem) const
{
	int32_t val = get_next_headerType(wmem);
	return(new PvNumber(val));
}

bool
McHdr_ISAKMP::HCGENE(EFlag)(WControl &cntr,
	WObject *wmem, OCTBUF &buf) const
{
	PvNumber def(need_ISAKMP_encryption(wmem)? 1: 0);
	return(def.generate(cntr, wmem, buf));
}

PObject *
McHdr_ISAKMP::HCEVAL(EFlag)(WObject *wmem) const
{
	return(new PvNumber(need_ISAKMP_encryption(wmem)? 1: 0));
}

int32_t
McHdr_ISAKMP::get_next_headerType(WObject *wmem) const
{
	WObject *wc	= wmem->parent();	// Hdr_ISAKMP
	WObject *next	= wc->nextSister();	// Pld_ISAKMP_?, ISAKMP_Enc...

	if(need_ISAKMP_encryption(wmem)) {
		WObject *eval	= next->nextSister();

		if(eval) {
			next = eval; // Pld_ISAKMP_XXX
		} else {
			next = next? next->nextChild() : 0;	// IVEC
			next = next? next->nextSister(): 0;	// Crypted
			next = next? next->wDecrypted(): 0;	// Decrypted
			next = next? next->nextChild() : 0;	// PlainText
			next = next? next->nextChild() : 0;	// Pld_ISAKMP_?
		}
	}

	int32_t rtn  = next? next->meta()->headerType(): TP_ISAKMP_NPTYPE_NONE;

	if(rtn < 0) {
		wmem->mustDefine(0);
	}

	return(rtn);
}

bool
McHdr_ISAKMP::need_ISAKMP_encryption(WObject *wmem) const
{
	WObject *wc   = wmem->parent();
	WObject *next = wc->nextSister();
	bool rtn   = next? next->meta()->ISAKMP_encryption(): false;

	return(rtn);
}
#undef SUPER



MmISAKMP_MessageID::MmISAKMP_MessageID(CSTR s,
	uint16_t n, const PObject *g, const PObject *e, const ICVoverwriter *ow,
	METH_HC_ForIPinfo meth): MmUint(s, n, g, e, ow, meth) {}
MmISAKMP_MessageID::~MmISAKMP_MessageID() {}

bool
MmISAKMP_MessageID::generate(WControl &c, WObject *w_self, OCTBUF &buf) const
{
	bool rtn = MmUint::generate(c, w_self, buf);

	const PvNumber *pv = (const PvNumber *)w_self->pvalue();

	if(pv) {
		ISAKMP_Message_ID = pv->value();
	}

	return(rtn);
}



//
// ISAKMP Encryption
//
////////////////////////////////

McISAKMP_Encryption::McISAKMP_Encryption(CSTR key): McHeader(key) {}
McISAKMP_Encryption::~McISAKMP_Encryption() {}

McISAKMP_Encryption *
McISAKMP_Encryption::create(CSTR key)
{
	McISAKMP_Encryption *mc = new McISAKMP_Encryption(key);

	mc->member(new MmAlgorithm_onISAKMP("algorithm"));
	mc->member(new MmIVEC_onISAKMP("IVEC"));

	mc->member(new MmISAKMP_Crypted("Crypted"));
        
	MmEncrypt_onISAKMP::add_other(mc);

	return(mc);
}

uint32_t
McISAKMP_Encryption::length_for_reverse(RControl &,
	ItPosition &at, OCTBUF &buf) const
{
	return(buf.remainLength(at.bytes()));
}

RObject *
McISAKMP_Encryption::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	const PaObject *keepSA = c.poping_SA();
	c.poping_SA(0);

	Con_ISAKMPinfo keep = c.ISAKMPinfo();

	c.ISAKMPinfo().reset();

	RObject *r_self = McHeader::reverse(c, r_parent, at, buf);

	c.ISAKMPinfo() = keep;

	c.poping_SA(keepSA);

	return(r_self);
}

RObject *
McISAKMP_Encryption::reverseRc(RControl &,
	RObject *r_parent, const ItPosition &at, PvObject *pv) const
{
	return(new RcESP(r_parent, this, at, pv));
}

WObject *
McISAKMP_Encryption::compose(WControl &c,
	WObject *w_parent, const PObject *pc) const
{
	const PaObject *keepSA = c.pushing_SA();
	c.pushing_SA(0);

	WObject *w_self = McHeader::compose(c, w_parent, pc);
	c.pushing_SA(keepSA);

	return(w_self);
}

WObject *
McISAKMP_Encryption::composeWc(WControl &,
	WObject *w_parent, const PObject *pc) const
{
	return(new WcESP(w_parent, this, pc));
}



//----------------------------//
// Decrypted =                //
//----------------------------//
#if 0
McISAKMP_Decrypted::McISAKMP_Decrypted(CSTR str):
	McObject(str), PadLength_(0) {}
#else
McISAKMP_Decrypted::McISAKMP_Decrypted(CSTR str): McObject(str) {}
#endif
McISAKMP_Decrypted::~McISAKMP_Decrypted() {}

McISAKMP_Decrypted *
McISAKMP_Decrypted::create(CSTR key)
{
	McISAKMP_Decrypted *mc = new McISAKMP_Decrypted(key);

	mc->member(McISAKMP_PlainText::create("PlainText"));

	mc->member(new MmISAKMP_Padding("Padding"));
#if 0
	mc->member(mc->PadLength_ = new MmUint("PadLength", 8,
		new PvHCgene(mc,
			(HCgenefunc)&McISAKMP_Decrypted::HCGENE(PadLength)),
		new PvANY()));
#endif

	return(mc);
}

uint32_t
McISAKMP_Decrypted::calc_padlen(RControl &c,
	OCTSTR buf, uint32_t len, bool &ok) const
{
	OCTSTR xbuf		= 0;
	uint32_t padlen		= 0;
	int32_t np		= 0;
	uint16_t plen		= 0;

	struct isakmp_gen *gen	= 0;

#ifdef ISAKMP_DBG
	xdmp("/tmp/isakmp_dbg.txt", "McISAKMP_Decrypted", "[pad] plaintext",
		buf, len);
#endif	// ISAKMP_DBG

	for(xbuf = buf, padlen = len, np = c.DictType().type(), plen = len;
		padlen && np && plen; ) {
#ifdef ISAKMP_DBG
		xdbg("/tmp/isakmp_dbg.txt", "McISAKMP_Decrypted",
			"[pad: 0] padlen: %d\n", padlen);
		xdbg("/tmp/isakmp_dbg.txt", "McISAKMP_Decrypted",
			"[pad: 0] np: %d\n", np);
		xdbg("/tmp/isakmp_dbg.txt", "McISAKMP_Decrypted",
			"[pad: 0] plen: %d\n", plen);
#endif	// ISAKMP_DBG
		if(!xbuf) {
			abort();
			// NOTREACHED
		}

		if(padlen < sizeof(struct isakmp_gen)) {
			fprintf(stderr,
				"err: ISAKMP Pld_ISAKMP decode(%d) "
				"over remain size(%d)\n",
				sizeof(struct isakmp_gen), padlen);

			ok = false;
			return(0);
		}

		if(plen < sizeof(struct isakmp_gen)) {
			fprintf(stderr,
				"err: ISAKMP Pld_ISAKMP decode(%d) "
				"over ISAKMP PayloadLength(%d)\n",
				sizeof(struct isakmp_gen), plen);

			ok = false;
			return(0);
		}

		gen	= (struct isakmp_gen *)xbuf;
		plen	= ntohs(gen->len);
		np	= gen->np;

		if(padlen < plen) {
			fprintf(stderr,
				"err: ISAKMP PayloadLength decode(%d) "
				"over remain size(%d)\n",
				plen, padlen);

			ok = false;
			return(0);
		}

		padlen	-= plen;
		xbuf	+= plen;
	}

	if(np) {
		fprintf(stderr,
			"err: ISAKMP NextPayload decode(%d) "
			",remain at(%d,0)/size(%d,0)\n",
			np, len - padlen, len);

		ok = false;
		return(0);
	}

#ifdef ISAKMP_DBG
	xdbg("/tmp/isakmp_dbg.txt", "McISAKMP_Decrypted",
		"[pad: 1] padlen: %d\n", padlen);
	xdbg("/tmp/isakmp_dbg.txt", "McISAKMP_Decrypted",
		"[pad: 1] np: %d\n", np);
	xdbg("/tmp/isakmp_dbg.txt", "McISAKMP_Decrypted",
		"[pad: 1] plen: %d\n", plen);
#endif	// ISAKMP_DBG

	ok = true;

	return(padlen);
}

RObject *
McISAKMP_Decrypted::Self_reverse(RControl &c, OCTBUF *selfdecrypted) const
{
	bool ok = false;

	OCTSTR		plaintextbuf	= selfdecrypted->buffer();
	uint32_t	plaintextlen	= selfdecrypted->length();

#if 0
	ItPosition RA_at(plaintextlen - 1);
	// XXX: should be RA_at(plaintextlen)
#else
	ItPosition RA_at(plaintextlen);
#endif

#if 0
	uint32_t padlen = PadLength_->rdecode(RA_at, *selfdecrypted, ok);
	if(!ok) {
		fprintf(stderr,
			"err: "
			"ISAKMP PadLength field decode over remain size(%d)\n",
			RA_at.bytes());
		c.set_error(1);
		return(0);
	}
#else
	uint32_t padlen = calc_padlen(c, plaintextbuf, plaintextlen, ok);
	if(!ok) {
		c.set_error(1);
		return(0);
	}
#endif

#ifdef ISAKMP_DBG
	xdbg("/tmp/isakmp_dbg.txt", "McISAKMP_Decrypted",
		"[pad] padlen: %d\n", padlen);
#endif	// ISAKMP_DBG

	ItPosition PDsize(padlen);

	if(RA_at < PDsize) {
		fprintf(stderr,
			"err: ISAKMP Padding size(%d) over remain size(%d)\n",
			padlen, RA_at.bytes());
		c.set_error(1);
		return(0);
	}

	RA_at -= PDsize;

	uint32_t paylen = RA_at.bytes();

	c.ISAKMPinfo().set_PadLength(padlen);
	c.ISAKMPinfo().set_PayloadLength(paylen);

	ItPosition at;
	RObject *r_self = reverseRc(c, 0, at, selfdecrypted);
	ItPosition memat;
	members_reverse(c, r_self, memat, *selfdecrypted);

	return(r_self);
}

WObject *
McISAKMP_Decrypted::Self_composeList(WControl &c, const PObjectList &pls) const
{
	WObject *w_self = composeWc(c, 0, 0);
	members_compose(c, w_self, pls);
	return(w_self);
}

#if 0
bool
McISAKMP_Decrypted::HCGENE(PadLength)(WControl &cntr,
	WObject *wmem, OCTBUF &buf) const
{
	WObject *wm		= wmem->prevSister();   // ESP_Padding
	uint32_t reallen	= wm->size().bytes();
	uint32_t valulen	= reallen;
	PvNumber def(valulen);
	return(def.generate(cntr, wmem, buf));
}
#endif



//----------------------------//
// Crypted =                  //
//----------------------------//
MmISAKMP_Crypted::MmISAKMP_Crypted(CSTR str): MmVarOctets(str) {
	mDecrypted_ = McISAKMP_Decrypted::create("Decrypted"); }

MmISAKMP_Crypted::~MmISAKMP_Crypted() {
	if(mDecrypted_) { delete mDecrypted_; mDecrypted_ = 0; } }

MObject *
MmISAKMP_Crypted::findMember(CSTR str) const
{
	MObject *rtn = MmVarOctets::findMember(str);

	if(!rtn) {
		rtn = mDecrypted_->findMember(str);
	}

	return(rtn);
}

ItPosition
MmISAKMP_Crypted::length_for_reverse(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	return(ItPosition(buf.remainLength(at.bytes())));
}

RObject *
MmISAKMP_Crypted::reverseRm(RControl &c, RObject *r_parent,
	const ItPosition &at, const ItPosition &size, PvObject *pv) const
{
	RmESP_Crypted *r_self =
		new RmESP_Crypted(r_parent, this, at, size, pv);

	const PaObject *sa = c.poping_SA();

	if(!sa) {
		return(r_self);
	}

	OCTBUF &cryptedbuf = (OCTBUF &)*pv;

	if(cryptedbuf.length() == 0) {
		fprintf(stderr, "err: ISAKMP Crypted buffer length is zero\n");
		return(r_self);
	}

	OCTBUF *decryptedbuf = 0, *ivecbuf = 0;

	decryptedbuf = sa->Decrypted_Octets(cryptedbuf, ivecbuf);
	if(!decryptedbuf) {
		fprintf(stderr, "err: SA(%s) doesn't return Decrypted buffer\n",
			sa->nameString());
		c.set_error(1);
		return(r_self);
	}
	r_self->set_rDecrypted(mDecrypted_->Self_reverse(c, decryptedbuf));
	return(r_self);
}

void
MmISAKMP_Crypted::composeList(WControl &c,
	WObject *w_parent, const PObjectList &pls) const
{
	const PaObject *sa = c.pushing_SA();
	WmESP_Crypted *w_self = new WmESP_Crypted(w_parent, this, sa);
	w_self->set_wDecrypted(mDecrypted_->Self_composeList(c, pls));
}

//----------------------------//
// algorithm =                //
//----------------------------//
#define SUPER	MmReference_Must1
#include "PvName.h"
MmAlgorithm_onISAKMP::MmAlgorithm_onISAKMP(CSTR key): SUPER(key) {}
MmAlgorithm_onISAKMP::~MmAlgorithm_onISAKMP() {}

RObject *
MmAlgorithm_onISAKMP::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &) const {

        RObject *r_self = 0;

	c.set_pop_SA(eISAKMP_);
	const PaObject *sa = c.poping_SA();

	if(sa) {
		RcESP *rcesp = (RcESP *)r_parent;

		if(rcesp) {
			rcesp->algorithm(sa);
		}

		PvObject *pv = sa->nameObject()->shallowCopy();
		ItPosition size(0);
		r_self = new RmObject(r_parent, this, at, size, pv);
	}

	return(r_self);
}

bool
MmAlgorithm_onISAKMP::disused() const
{
	return(true);
}
#undef SUPER



//----------------------------//
// IVEC =                     //
//----------------------------//
RObject *
MmIVEC_onISAKMP::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	RObject *r_self = 0;

	ItPosition length = length_for_reverse(c, at, buf);

	if(buf.remainLength(at.bytes()) >= length.bytes()) {
		ItPosition tmpat(buf.remainLength(at.bytes()) - length.bytes(),
			at.bits());

		r_self = MmVarOctets::reverse(c, r_parent, tmpat, buf);
	}

	return(r_self);
}

RObject *
MmIVEC_onISAKMP::reverseRm(RControl &, RObject *r_parent, const ItPosition &at,
	const ItPosition &size, PvObject *pv) const
{
	ItPosition tmpsize(0);
 
	return(new RmObject(r_parent, this, at, tmpsize, pv));
}

ItPosition
MmIVEC_onISAKMP::length_for_reverse(RControl &,
	ItPosition &at, OCTBUF &buf) const
{
	return(ItPosition(8, 0));
}

bool MmIVEC_onISAKMP::disused() const { return(true); }



#define SUPER	McAlgorithm
#include "McAlgorithm.h"
McISAKMP_Algorithm::McISAKMP_Algorithm(CSTR str): SUPER(str) {}
McISAKMP_Algorithm::~McISAKMP_Algorithm() {}

PObject *
McISAKMP_Algorithm::tokenObject(int l, CSTR f) const
{
	return(new PaISAKMP(this, f, l));
}

McISAKMP_Algorithm *
McISAKMP_Algorithm::create(CSTR key)
{
	McISAKMP_Algorithm *mc = new McISAKMP_Algorithm(key);

	mc->member(new MmAlgo_Crypt("crypt"));
	mc->member(new MmAlgo_Pad("pad"));

	return(mc);
}
#undef SUPER



McISAKMP_PlainText::McISAKMP_PlainText(CSTR str): McObject(str) {}
McISAKMP_PlainText::~McISAKMP_PlainText() {}

uint32_t
McISAKMP_PlainText::length_for_reverse(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	uint32_t length = c.ISAKMPinfo().get_PayloadLength();
	return(length);
}

bool
McISAKMP_PlainText::generate(WControl &c, WObject *w_self, OCTBUF &buf) const
{
	bool rtn = McObject::generate(c, w_self, buf);

	if(!c.error()) {
		Con_IPinfo *info = c.IPinfo();

		if(info) {
			info->generate_postP2_HASH_2(c, buf, w_self);
		}
	}

	return(rtn);
}

RObject *
McISAKMP_PlainText::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	RObject *r_self = McObject::reverse(c, r_parent, at, buf);

	if(!c.error()) {
		Con_IPinfo *info = c.IPinfo();
		if(info) {
			info->reverse_postP2_HASH_2(c, r_self);
		}
	}

	return(r_self);
}

void
McISAKMP_PlainText::composeList(WControl &c,
	WObject *w_parent, const PObjectList &) const
{
	WObject *w_self = composeWc(c, w_parent, 0);
	w_self = w_self;
}

McISAKMP_PlainText *
McISAKMP_PlainText::create(CSTR key)
{
	McISAKMP_PlainText *mc = new McISAKMP_PlainText(key);

	mc->member(new MmPayload_onISAKMP("payload"));

        return(mc);
}



ItPosition
MmISAKMP_Padding::length_for_reverse(RControl &c,
	ItPosition &, OCTBUF &) const
{
	uint32_t length = c.ISAKMPinfo().get_PadLength();
	return(ItPosition(length));
}

WObject *
MmISAKMP_Padding::composeWm(WControl &c,
	WObject *w_parent, const PObject *) const
{
	const PaObject *sa = c.pushing_SA();
	WmISAKMP_Padding *w_self = new WmISAKMP_Padding(w_parent, this, sa);
	return(w_self);
}



#define SUPER	WmObject
WmISAKMP_Padding::WmISAKMP_Padding(WObject *p,
	const MObject *m, const PaObject *sa): SUPER(p, m, 0), sa_(sa) {}
WmISAKMP_Padding::~WmISAKMP_Padding() {}

void
WmISAKMP_Padding::internalGeneratePrologue(ItPosition &it)
{
	if(sa_) {
		WObject *payload	= prevSister();
		uint32_t paylen		= payload->length();
#ifdef ISAKMP_DBG
                xdbg("/tmp/isakmp_dbg.txt", "WmISAKMP_Padding",
                        "[pad] paylen: %d\n", paylen);
#endif	// ISAKMP_DBG

		uint32_t paddinglen	= sa_->Padding_Length(paylen + 1);
#ifdef ISAKMP_DBG
		xdbg("/tmp/isakmp_dbg.txt", "WmISAKMP_Padding",
			"[pad] paddinglen: %d\n", paddinglen);
#endif	// ISAKMP_DBG

		OCTBUF *padbuf	= sa_->Padding_Octets(paddinglen + 1);

		OCTSTR buf	= padbuf->buffer();
		uint32_t len	= padbuf->length();
		len --;

		buf[len]	= len & 0xff;

#ifdef ISAKMP_DBG
		xdmp("/tmp/isakmp_dbg.txt", "WmISAKMP_Padding", "[pad] padbuf",
			padbuf->buffer(), padbuf->length());
#endif	// ISAKMP_DBG

		set_rgenerate(padbuf);
		it.addBytes(paddinglen + 1);
	}

	return;
}
#undef SUPER



uint32_t
PaISAKMP::Crypted_Length(uint32_t len) const
{
	return(len);
}

OCTBUF *
PaISAKMP::Padding_Octets(uint32_t len) const
{
	return(pad_?
		pad_->Padding_Octets(len): MfESPPad::IKEdefaultOctets(len));
}



//
// Generic Payload
//
////////////////////////////////

McPld_ISAKMP::McPld_ISAKMP(CSTR key): McHeader(key), Layerlength_(0) {}
McPld_ISAKMP::~McPld_ISAKMP() {}

bool
McPld_ISAKMP::HCGENE(PayloadLength)(WControl &cntr,
	WObject *wmem, OCTBUF &buf) const
{
	WObject *wc		= wmem->parent();       // McPld_ISAKMP
	uint32_t pldlen		= wc->size().bytes();
	PvNumber def(pldlen);

	return(def.generate(cntr, wmem, buf));
}

bool
McPld_ISAKMP::HCGENE(NextPayload)(WControl &cntr,
	WObject *wmem, OCTBUF &buf) const
{
	int32_t val = get_next_headerType(wmem);

	if(val == -1) {
		return(false);
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *
McPld_ISAKMP::HCEVAL(NextPayload)(WObject *wmem) const
{
	int32_t val = get_next_headerType(wmem);
	return(new PvNumber(val));
}

uint32_t
McPld_ISAKMP::length_for_reverse(RControl &c, ItPosition &at, OCTBUF &buf) const
{
	return(
		Layerlength_?
		Layerlength_->value(at, buf):
		McHeader::length_for_reverse(c, at, buf)
	);
}

int32_t
McPld_ISAKMP::get_next_headerType(WObject *wmem) const
{
	WObject *wc   = wmem->parent();
	WObject *next = wc->nextSister();
	int32_t rtn   = next? next->meta()->headerType(): TP_ISAKMP_NPTYPE_NONE;

	if(rtn == -1) {
		wmem->mustDefine(0);
	}

	return(rtn);
}

RObject *
McPld_ISAKMP::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	RObject *r_self = McHeader::reverse(c, r_parent, at, buf);

	if(r_self && nextType_meta_) {
		RObject *next =
			(RObject *)r_self->corresponding(nextType_meta_);

		if(next) {
			PvNumber *pv = (PvNumber *)next->pvalue();

			if(pv) {
				uint32_t nextvalue = pv->value();

				if(!nextvalue) {
					c.DictType().finish_Set();
				}
			}
		}
	}

	return(r_self);
}

McPld_ISAKMP_ANY::McPld_ISAKMP_ANY(CSTR key): McPld_ISAKMP(key) {}
McPld_ISAKMP_ANY::~McPld_ISAKMP_ANY() {}



//
// Security Association Payload
//
////////////////////////////////

McPld_ISAKMP_SA_ONE *McPld_ISAKMP_SA_ONE::instance_ = 0;

McPld_ISAKMP_SA_ONE *
McPld_ISAKMP_SA_ONE::instance()
{
	if(!instance_) {
		instance_ = new McPld_ISAKMP_SA_ONE("Pld_ISAKMP_SA_ONE");
	}

	return(instance_);
}

McPld_ISAKMP_SA_ONE::McPld_ISAKMP_SA_ONE(CSTR key): McPld_ISAKMP(key)
{
	member(new MmDOIonSA("payload"));
	MmPayload_onISAKMP::add(this);
	MmISAKMP_PonSA::add(this);
	MmISAKMP_TonP::add(this);
}

McPld_ISAKMP_SA_ONE::~McPld_ISAKMP_SA_ONE() {}

bool
McPld_ISAKMP_SA_ONE::containsMc(const MObject *mc) const
{
	bool rtn = McPld_ISAKMP::containsMc(mc);
	return(rtn? rtn: members_[0]->containsMc(mc));
}

RObject *
McPld_ISAKMP_SA_ONE::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	return(members_[0]->reverse(c, r_parent, at, buf));
}

McPld_ISAKMP_SA::McPld_ISAKMP_SA(CSTR key): McPld_ISAKMP(key), DOI_(0) {
	McPld_ISAKMP_SA_ONE::instance(); }

McPld_ISAKMP_SA::~McPld_ISAKMP_SA() {}

bool
McPld_ISAKMP_SA::overwrite_DictType(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	if(buf.remainLength(at.bytes())) {
		ItPosition tmpat = at;

		RObject *r_self = McPld_ISAKMP::reverse(c, 0, tmpat, buf);
		if(r_self) {
			RObject *doi = (RObject *)r_self->corresponding(DOI_);
			if(doi) {
				const PvNumber *pv =
					(const PvNumber *)doi->pvalue();

				uint32_t doivalue = pv->value();
				c.DictType().type_Set(doivalue);

				return(true);
			}
		}
	}

	return(false);
}

bool
McPld_ISAKMP_SA::HCGENE(DOI)(WControl &cntr,
	WObject *wmem, OCTBUF &buf) const
{
	int32_t val = get_DOIvalue(wmem);
	if(val == -1) {
		return(false);
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *
McPld_ISAKMP_SA::HCEVAL(DOI)(WObject *wmem) const
{
	int32_t val = get_DOIvalue(wmem);
	return(new PvNumber(val));
}

int32_t
McPld_ISAKMP_SA::get_DOIvalue(WObject *wmem) const
{
	WObject *wc = wmem->parent();

	int32_t rtn = wc? wc->meta()->DOIvalue(): -1;
	if(rtn == -1) {
		wmem->mustDefine(0);
	}

        return(rtn);
}



McPld_ISAKMP_SA_ANY::McPld_ISAKMP_SA_ANY(CSTR key): McPld_ISAKMP_SA(key) {}
McPld_ISAKMP_SA_ANY::~McPld_ISAKMP_SA_ANY() {}



McPld_ISAKMP_SA_IPsec_ONE *McPld_ISAKMP_SA_IPsec_ONE::instance_ = 0;

McPld_ISAKMP_SA_IPsec_ONE *
McPld_ISAKMP_SA_IPsec_ONE::instance()
{
	if(!instance_) {
		instance_ = new McPld_ISAKMP_SA_IPsec_ONE(
			"Pld_ISAKMP_SA_IPsec_ONE");
	}

	return(instance_);
}

McPld_ISAKMP_SA_IPsec_ONE::McPld_ISAKMP_SA_IPsec_ONE(CSTR key):
	McPld_ISAKMP(key)
{
	member(new MmSIT_onSA_IPsec("payload"));
	MmDOIonSA::add(this);
}

McPld_ISAKMP_SA_IPsec_ONE::~McPld_ISAKMP_SA_IPsec_ONE() {}

bool
McPld_ISAKMP_SA_IPsec_ONE::containsMc(const MObject *mc) const
{
	bool rtn = McPld_ISAKMP::containsMc(mc);
	return(rtn ? rtn : members_[0]->containsMc(mc));
}

RObject *
McPld_ISAKMP_SA_IPsec_ONE::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	return(members_[0]->reverse(c, r_parent, at, buf));
}



McPld_ISAKMP_SA_IPsec::McPld_ISAKMP_SA_IPsec(CSTR key):
	McPld_ISAKMP_SA(key), SIT_(0)
{
	McPld_ISAKMP_SA_IPsec_ONE::instance();
}

McPld_ISAKMP_SA_IPsec::~McPld_ISAKMP_SA_IPsec() {}

bool
McPld_ISAKMP_SA_IPsec::HCGENE(Situation)(WControl &cntr,
	WObject *wmem, OCTBUF &buf) const
{
	int32_t val = get_SITvalue(wmem);
	if(val == -1) {
		return(false);
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *
McPld_ISAKMP_SA_IPsec::HCEVAL(Situation)(WObject *wmem) const
{
	int32_t val = get_SITvalue(wmem);
	return(new PvNumber(val));
}

int32_t
McPld_ISAKMP_SA_IPsec::get_SITvalue(WObject *wmem) const
{
	WObject *wc = wmem->parent();

	int32_t rtn = wc ? wc->meta()->SITvalue() : -1;
	if(rtn == -1) {
		wmem->mustDefine(0);
	}

        return(rtn);
}

bool
McPld_ISAKMP_SA_IPsec::overwrite_DictType(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	if(buf.remainLength(at.bytes())) {
		ItPosition tmpat = at;

		RObject *r_self = McPld_ISAKMP_SA::reverse(c, 0, tmpat, buf);
		if(r_self) {
			RObject *sit = (RObject *)r_self->corresponding(SIT_);
			if(sit) {
				const PvNumber *pv =
					(const PvNumber *)sit->pvalue();

				uint32_t sitvalue = pv->value();
				c.DictType().type_Set(sitvalue);

				return(true);
			}
		}
	}

	return(false);
}



McPld_ISAKMP_SA_IPsec_ANY::McPld_ISAKMP_SA_IPsec_ANY(CSTR key):
	McPld_ISAKMP_SA_IPsec(key) {}
McPld_ISAKMP_SA_IPsec_ANY::~McPld_ISAKMP_SA_IPsec_ANY() {}



McPld_ISAKMP_SA_IPsec_IDonly::McPld_ISAKMP_SA_IPsec_IDonly(CSTR key):
	McPld_ISAKMP_SA_IPsec(key) {}
McPld_ISAKMP_SA_IPsec_IDonly::~McPld_ISAKMP_SA_IPsec_IDonly() {}



//
// Proposal Payload
//
////////////////////////////////

McPld_ISAKMP_P_ONE *McPld_ISAKMP_P_ONE::instance_ = 0;

McPld_ISAKMP_P_ONE *
McPld_ISAKMP_P_ONE::instance()
{
	if(!instance_) {
		instance_ = new McPld_ISAKMP_P_ONE("Pld_ISAKMP_P_ONE");
	}

	return(instance_);
}

McPld_ISAKMP_P_ONE::McPld_ISAKMP_P_ONE(CSTR key): McPld_ISAKMP(key)
{
	member(new MmProtocolIDonP("payload"));
	MmPayload_onISAKMP::add(this);
	MmISAKMP_PonSA::add(this);
	MmISAKMP_TonP::add(this);
}

McPld_ISAKMP_P_ONE::~McPld_ISAKMP_P_ONE() {}

bool
McPld_ISAKMP_P_ONE::containsMc(const MObject *mc) const
{
	bool rtn = McPld_ISAKMP::containsMc(mc);
	return(rtn? rtn: members_[0]->containsMc(mc));
}

RObject *
McPld_ISAKMP_P_ONE::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	return(members_[0]->reverse(c, r_parent, at, buf));
}



McPld_ISAKMP_P::McPld_ISAKMP_P(CSTR key):
	McPld_ISAKMP(key), ProtocolID_(0), SPIsize_(0), SPI_(0)
{
	McPld_ISAKMP_P_ONE::instance();
	Tcounter_ = new Tcounter(); 
}

McPld_ISAKMP_P::~McPld_ISAKMP_P() {
	if(Tcounter_) { delete Tcounter_; Tcounter_ = 0; } }

RObject *
McPld_ISAKMP_P::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	bool sizectrl = false;
	MmISAKMP_SPI *mm = 0;

	if(SPI_ && SPIsize_) {
		ItPosition tmpat = at;

		RObject *r_self = McPld_ISAKMP::reverse(c, 0, tmpat, buf);

		if(!c.error()){
			RObject *spisize =
				(RObject *)r_self->corresponding(SPIsize_);

			if(spisize) {
				const PvNumber *pv =
					(const PvNumber *)spisize->pvalue();

				if(pv) {
					uint32_t size = pv->value();

					mm = (MmISAKMP_SPI *)SPI_;
					mm->set_ctrl(false, size);
					sizectrl = true;
				}
			}
		}
	}

	RObject *ro = McPld_ISAKMP::reverse(c, r_parent, at, buf);

	if(sizectrl) {
		mm->set_ctrl(true, 0);
	}

	return(ro);
}

bool
McPld_ISAKMP_P::HCGENE(ProtocolID)(WControl &cntr,
	WObject *wmem, OCTBUF &buf) const
{
	int32_t val = get_ProtocolID(wmem);
	if(val == -1) {
		return(false);
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *
McPld_ISAKMP_P::HCEVAL(ProtocolID)(WObject *wmem) const
{
	int32_t val = get_ProtocolID(wmem);
	return(new PvNumber(val));
}

int32_t
McPld_ISAKMP_P::get_ProtocolID(WObject *wmem) const
{
	WObject *wc = wmem->parent();

	int32_t rtn = wc? wc->meta()->ProtocolID(): -1;
	if(rtn == -1) {
		wmem->mustDefine(0);
	}

	return(rtn);
}

bool
McPld_ISAKMP_P::HCGENE(SPIsize)(WControl &cntr,
	WObject *wmem, OCTBUF &buf) const
{
	WObject *wobj0	= wmem->nextSister();	// NumOfTrasforms
	WObject *wobj1	= wobj0->nextSister();	// SPI
	uint32_t spilen	= wobj1->size().bytes();
	PvNumber def(spilen);

	return(def.generate(cntr, wmem, buf));
}

bool
McPld_ISAKMP_P::HCGENE(NumOfTransforms)(WControl &cntr,
	WObject *wmem, OCTBUF &buf) const
{
	int32_t val = get_transforms();

	PvNumber def(val);
	bool c = def.generate(cntr, wmem, buf);

	return(c);
}

PObject *
McPld_ISAKMP_P::HCEVAL(NumOfTransforms)(WObject *wmem) const
{
	int32_t val = get_transforms();
	PObject *p = new PvNumber(val);

	return(p);
}

bool
McPld_ISAKMP_P::overwrite_DictType(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	if(buf.remainLength(at.bytes())) {
		ItPosition tmpat = at;

		RObject *r_self = McPld_ISAKMP::reverse(c, 0, tmpat, buf);
		if(r_self) {
			RObject *id =
				(RObject *)r_self->corresponding(ProtocolID_);

			if(id) {
				const PvNumber *pv =
					(const PvNumber *)id->pvalue();

				uint32_t idvalue = pv->value();
				c.DictType().type_Set(idvalue);

				return(true);
			}
		}
	}

	return(false);
}

#if 0
bool
McPld_ISAKMP_P::generate(WControl &c, WObject *w_self, OCTBUF &buf) const
{
	bool rtn = McPld_ISAKMP::generate(c, w_self, buf);

	set_transforms(0);

	return(rtn);
}
#else
WObject *
McPld_ISAKMP_P::composeWc(WControl &c,
	WObject *w_parent, const PObject *pc) const
{
	WObject *w_self = McPld_ISAKMP::composeWc(c, w_parent, pc);
#ifdef ISAKMP_DBG
	xdbg("/tmp/isakmp_dbg.txt", "McPld_ISAKMP_P",
		"[P: 0] this: %p\n", this);
#endif	// ISAKMP_DBG
	set_transforms(0);

	return(w_self);
}
#endif



McPld_ISAKMP_P_ISAKMP::McPld_ISAKMP_P_ISAKMP(CSTR key): McPld_ISAKMP_P(key) {}
McPld_ISAKMP_P_ISAKMP::~McPld_ISAKMP_P_ISAKMP() {}

McPld_ISAKMP_P_IPsec_AH::McPld_ISAKMP_P_IPsec_AH(CSTR key):
	McPld_ISAKMP_P(key) {}
McPld_ISAKMP_P_IPsec_AH::~McPld_ISAKMP_P_IPsec_AH() {}

McPld_ISAKMP_P_IPsec_ESP::McPld_ISAKMP_P_IPsec_ESP(CSTR key):
	McPld_ISAKMP_P(key) {}
McPld_ISAKMP_P_IPsec_ESP::~McPld_ISAKMP_P_IPsec_ESP() {}

McPld_ISAKMP_P_IPCOMP::McPld_ISAKMP_P_IPCOMP(CSTR key): McPld_ISAKMP_P(key) {}
McPld_ISAKMP_P_IPCOMP::~McPld_ISAKMP_P_IPCOMP() {}



//
// Transform Payload
//
////////////////////////////////

McPld_ISAKMP_T::McPld_ISAKMP_T(CSTR key): McPld_ISAKMP(key) {}
McPld_ISAKMP_T::~McPld_ISAKMP_T() {}



//
// Key Exchange Payload
//
////////////////////////////////

McPld_ISAKMP_KE::McPld_ISAKMP_KE(CSTR key): McPld_ISAKMP(key) {}
McPld_ISAKMP_KE::~McPld_ISAKMP_KE() {}



//
// Identification Payload
//
////////////////////////////////

McPld_ISAKMP_ID_ONE *McPld_ISAKMP_ID_ONE::instance_ = 0;

McPld_ISAKMP_ID_ONE *
McPld_ISAKMP_ID_ONE::instance()
{
	if(!instance_) {
		instance_ = new McPld_ISAKMP_ID_ONE("Pld_ISAKMP_ID_ONE");
	}

	return(instance_);
}

McPld_ISAKMP_ID_ONE::McPld_ISAKMP_ID_ONE(CSTR key): McPld_ISAKMP(key)
{
	member(new MmIDtype_onID("payload"));
	MmPayload_onISAKMP::add(this);
	MmISAKMP_PonSA::add(this);
	MmISAKMP_TonP::add(this);
}

McPld_ISAKMP_ID_ONE::~McPld_ISAKMP_ID_ONE() {}

bool
McPld_ISAKMP_ID_ONE::containsMc(const MObject *mc) const
{
	bool rtn = McPld_ISAKMP::containsMc(mc);
	return(rtn? rtn: members_[0]->containsMc(mc));
}

RObject *
McPld_ISAKMP_ID_ONE::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	return(members_[0]->reverse(c, r_parent, at, buf));
}

McPld_ISAKMP_ID::McPld_ISAKMP_ID(CSTR key): McPld_ISAKMP(key), IDtype_(0)
{
	McPld_ISAKMP_ID_ONE::instance();
}

McPld_ISAKMP_ID::~McPld_ISAKMP_ID() {}

bool
McPld_ISAKMP_ID::HCGENE(IDtype)(WControl &cntr,
	WObject *wmem, OCTBUF &buf) const
{
	int32_t val = get_IDtype(wmem);
	if(val == -1) {
		return(false);
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *
McPld_ISAKMP_ID::HCEVAL(IDtype)(WObject *wmem) const
{
	int32_t val = get_IDtype(wmem);
	return(new PvNumber(val));
}

int32_t
McPld_ISAKMP_ID::get_IDtype(WObject *wmem) const
{
	WObject *wc = wmem->parent();

	int32_t rtn = wc? wc->meta()->IDtype(): -1;
	if(rtn == -1) {
		wmem->mustDefine(0);
	}

	return(rtn);
}

bool
McPld_ISAKMP_ID::overwrite_DictType(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	if(buf.remainLength(at.bytes())) {
		ItPosition tmpat = at;

		RObject *r_self = McPld_ISAKMP::reverse(c, 0, tmpat, buf);
		if(r_self) {
			RObject *id =
				(RObject *)r_self->corresponding(IDtype_);

			if(id) {
				const PvNumber *pv =
					(const PvNumber *)id->pvalue();

				uint32_t idvalue = pv->value();
				c.DictType().type_Set(idvalue);

				return(true);
			}
		}
	}

	return(false);
}

McPld_ISAKMP_ID_IPV4_ADDR::McPld_ISAKMP_ID_IPV4_ADDR(CSTR key):
	McPld_ISAKMP_ID(key) {}
McPld_ISAKMP_ID_IPV4_ADDR::~McPld_ISAKMP_ID_IPV4_ADDR() {}

McPld_ISAKMP_ID_FQDN::McPld_ISAKMP_ID_FQDN(CSTR key): McPld_ISAKMP_ID(key) {}
McPld_ISAKMP_ID_FQDN::~McPld_ISAKMP_ID_FQDN() {}

McPld_ISAKMP_ID_USER_FQDN::McPld_ISAKMP_ID_USER_FQDN(CSTR key):
	McPld_ISAKMP_ID(key) {}
McPld_ISAKMP_ID_USER_FQDN::~McPld_ISAKMP_ID_USER_FQDN() {}

McPld_ISAKMP_ID_IPV4_ADDR_SUBNET::McPld_ISAKMP_ID_IPV4_ADDR_SUBNET(CSTR key):
	McPld_ISAKMP_ID(key) {}
McPld_ISAKMP_ID_IPV4_ADDR_SUBNET::~McPld_ISAKMP_ID_IPV4_ADDR_SUBNET() {}

McPld_ISAKMP_ID_IPV6_ADDR::McPld_ISAKMP_ID_IPV6_ADDR(CSTR key):
	McPld_ISAKMP_ID(key) {}
McPld_ISAKMP_ID_IPV6_ADDR::~McPld_ISAKMP_ID_IPV6_ADDR() {}

McPld_ISAKMP_ID_IPV6_ADDR_SUBNET::McPld_ISAKMP_ID_IPV6_ADDR_SUBNET(CSTR key):
	McPld_ISAKMP_ID(key) {}
McPld_ISAKMP_ID_IPV6_ADDR_SUBNET::~McPld_ISAKMP_ID_IPV6_ADDR_SUBNET() {}

McPld_ISAKMP_ID_IPV4_ADDR_RANGE::McPld_ISAKMP_ID_IPV4_ADDR_RANGE(CSTR key):
	McPld_ISAKMP_ID(key) {}
McPld_ISAKMP_ID_IPV4_ADDR_RANGE::~McPld_ISAKMP_ID_IPV4_ADDR_RANGE() {}

McPld_ISAKMP_ID_IPV6_ADDR_RANGE::McPld_ISAKMP_ID_IPV6_ADDR_RANGE(CSTR key):
	McPld_ISAKMP_ID(key) {}
McPld_ISAKMP_ID_IPV6_ADDR_RANGE::~McPld_ISAKMP_ID_IPV6_ADDR_RANGE() {}

McPld_ISAKMP_ID_DER_ASN1_DN::McPld_ISAKMP_ID_DER_ASN1_DN(CSTR key):
	McPld_ISAKMP_ID(key) {}
McPld_ISAKMP_ID_DER_ASN1_DN::~McPld_ISAKMP_ID_DER_ASN1_DN() {}

McPld_ISAKMP_ID_DER_ASN1_GN::McPld_ISAKMP_ID_DER_ASN1_GN(CSTR key):
	McPld_ISAKMP_ID(key) {}
McPld_ISAKMP_ID_DER_ASN1_GN::~McPld_ISAKMP_ID_DER_ASN1_GN() {}

McPld_ISAKMP_ID_KEY_ID::McPld_ISAKMP_ID_KEY_ID(CSTR key):
	McPld_ISAKMP_ID(key) {}
McPld_ISAKMP_ID_KEY_ID::~McPld_ISAKMP_ID_KEY_ID() {}



//
// Certificate Payload
//
////////////////////////////////

McPld_ISAKMP_CERT::McPld_ISAKMP_CERT(CSTR key): McPld_ISAKMP(key) {}
McPld_ISAKMP_CERT::~McPld_ISAKMP_CERT() {}



//
// Certificate Request Payload
//
////////////////////////////////

McPld_ISAKMP_CR::McPld_ISAKMP_CR(CSTR key): McPld_ISAKMP(key) {}
McPld_ISAKMP_CR::~McPld_ISAKMP_CR() {}



//
// Hash Payload
//
////////////////////////////////

McPld_ISAKMP_HASH::McPld_ISAKMP_HASH(CSTR key): McPld_ISAKMP(key) {}
McPld_ISAKMP_HASH::~McPld_ISAKMP_HASH() {}

PObject *
McPld_ISAKMP_HASH::tokenObject(int l, CSTR f) const
{
	return(new PaP2_HASH_2(this, f, l));
}

PfP2_HASH_2::PfP2_HASH_2(const MvP2_HASH_2 *a, CSTR b, int c):
	PvFunction(a, b, c), meta_(a), context_(0) {}
PfP2_HASH_2::~PfP2_HASH_2() {}

const MObject *
PfP2_HASH_2::meta() const
{
	return(metaClass());
}

void
PfP2_HASH_2::init()
{
	const MvP2_HASH_2 *m = metaClass();

	if(m) {
		context_ = m->init(context_, args());
	}

	return;
}

void
PfP2_HASH_2::update(const OCTBUF &s)
{
	const MvP2_HASH_2 *m = metaClass();

	if(m) {
		m->update(context_, args(), s);
	}

	return;
}

PvOctets *
PfP2_HASH_2::result()
{
	const MvP2_HASH_2 *m = metaClass();

	PvOctets *ret = 0;

	if(m) {
		ret = m->result(context_, args());
	}

	return(ret);
}



PaP2_HASH_2::PaP2_HASH_2(const MObject *m, CSTR st, int l):
	PcObject(m ,st, l), p2_hash_2_(0) {}

PaP2_HASH_2::~PaP2_HASH_2() {
        if(p2_hash_2_) { delete p2_hash_2_; p2_hash_2_ = 0; } }

PObject *
PaP2_HASH_2::ike_p2_hash_member(PObject *p)
{
        return(p2_hash_2_ = (PfP2_HASH_2 *)(p));
}

OCTBUF *
PaP2_HASH_2::P2_HASH_2_Calculate(const OCTBUF &s) const
{
	if(!p2_hash_2_) {
		return(0);
	}

	p2_hash_2_->init();
	p2_hash_2_->update(s);
	PvOctets *icv = p2_hash_2_->result();

	return(icv);
}

WObject *
PaP2_HASH_2::selfCompose(WControl &c, WObject *w_parent) const
{
	const MObject *m = meta();

	if(p2_hash_2_) {
		c.set_push_P2_HASH_2(this);
	}

	return(m? m->compose(c, w_parent, this): 0);
}

implementCmList(P2_HASH_2List, PaP2_HASH_2);



KeepP2_HASH_2::KeepP2_HASH_2(): p2_hash_2_(new P2_HASH_2List()), current_(0) {}
KeepP2_HASH_2::~KeepP2_HASH_2()
{
	current_ = 0;
	if(p2_hash_2_) { delete p2_hash_2_; p2_hash_2_ = 0; }
}

const PaP2_HASH_2 *
KeepP2_HASH_2::GetP2_HASH_2(void)
{
	const PaP2_HASH_2 *p2_hash_2 = 0;

	if(p2_hash_2_) {
		uint32_t i9 = p2_hash_2_->size();
		uint32_t i = current_;

		current_ ++;
		if(current_ >= i9) {
			current_ = 0;
		}

		p2_hash_2 = i < i9? p2_hash_2_->index(i): 0;
	}

	return(p2_hash_2);
}

void
KeepP2_HASH_2::SetP2_HASH_2(const PaP2_HASH_2 *p2_hash_2)
{
	p2_hash_2_->append((PaP2_HASH_2 *)p2_hash_2);
}



MmP2_HASH_2::MmP2_HASH_2(CSTR s, const PObject *g, const PObject *e,
	const ICVoverwriter *ow, METH_HC_ForIPinfo meth):
	MmVarOctets(s, g, e, ow, meth), keephash_(new KeepP2_HASH_2()) {}

MmP2_HASH_2::~MmP2_HASH_2() {
	if(keephash_) { delete keephash_; keephash_ = 0; } }

RObject *
MmP2_HASH_2::reverseRm(RControl &c, RObject *r_parent, const ItPosition &at,
	const ItPosition &size, PvObject *pv) const
{
	const PaP2_HASH_2 *hash = 0;

	if(keephash_) {
		hash = keephash_->GetP2_HASH_2();
	}

	return(new RmP2_HASH_2(r_parent, this, at, size, pv, hash));
}

WObject *
MmP2_HASH_2::composeWm(WControl &c, WObject *w_parent,
	const PObject *pl) const
{
	const PaP2_HASH_2 *hash = c.pushing_P2_HASH_2();

	if(keephash_) {
		keephash_->SetP2_HASH_2(hash);
	}

	return(new WmP2_HASH_2(w_parent, this, pl, hash));
}

RObject *
MmP2_HASH_2::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	RObject *r_self = MmVarOctets::reverse(c, r_parent, at, buf);

	if(!c.error()){
		Con_IPinfo *info = c.IPinfo();
		if(info) {
			info->postP2_HASH_2(r_self);
		}
	}

	return(r_self);
}

bool
MmP2_HASH_2::generate(WControl &c, WObject *w_self, OCTBUF &buf) const
{
	bool rtn = MmVarOctets::generate(c, w_self, buf);

	if(!c.error()) {
		Con_IPinfo *info = c.IPinfo();
		if(info) {
			info->postP2_HASH_2(w_self);
		}
	}

	return(rtn);
}



#define SUPER	RmObject
RmP2_HASH_2::RmP2_HASH_2(RObject *r_parent, const MObject *m,
	const ItPosition &offset, const ItPosition &size, PvObject *pv,
	const PaP2_HASH_2 *hash):
		SUPER(r_parent, m, offset, size, pv),
		hash_(hash), calc_pvalue_(new OCTBUF()) {}

RmP2_HASH_2::~RmP2_HASH_2() {
	if(calc_pvalue_) { delete calc_pvalue_; calc_pvalue_ = 0; } }

void
RmP2_HASH_2::set_calc_pvalue(PvObject *calc)
{
	if(calc_pvalue_) {
		delete calc_pvalue_;
	}

	calc_pvalue_ = calc;
}

void
RmP2_HASH_2::post_reverse(Con_IPinfo &info, RControl &c, RObject *base)
{
	if(hash_) {
		if(!base) {
			return;
		}

		const OCTBUF *basebuf	= (const OCTBUF *)base->pvalue();
		OCTSTR payloadbuf	= (OCTSTR)basebuf->string();

		uint16_t payloadlen	= 0;
		memcpy(&payloadlen, payloadbuf + 2, 2);

		payloadlen = ntohs(payloadlen);

		OCTBUF *calc = 0;

		OCTBUF hashbuf(basebuf->length() - payloadlen,
			payloadbuf + payloadlen, true);

		calc = hash_->P2_HASH_2_Calculate(hashbuf);

		set_calc_pvalue(calc);
	}

	return;
}
#undef SUPER



#define SUPER   WmObject
WmP2_HASH_2::WmP2_HASH_2(WObject *p, const MObject *m, const PObject *po,
	const PaP2_HASH_2 *hash): SUPER(p, m, po), hash_(hash) {}
WmP2_HASH_2::~WmP2_HASH_2() {}

void
WmP2_HASH_2::post_generate(Con_IPinfo &info, WControl &c, OCTBUF &buf,
	WObject *base)
{
	if(hash_) {
		if(!base) {
			return;
		}

		const OCTBUF *basebuf	= (const OCTBUF *)base->pvalue();
		OCTSTR payloadbuf	= (OCTSTR)basebuf->string();

		uint16_t payloadlen	= 0;
		memcpy(&payloadlen, payloadbuf + 2, 2);

		payloadlen = ntohs(payloadlen);

		OCTBUF *calc = 0;

		OCTBUF hashbuf(basebuf->length() - payloadlen,
			payloadbuf + payloadlen, true);

		calc = hash_->P2_HASH_2_Calculate(hashbuf);

		if(calc) {
			set_rgenerate(calc);
			SUPER::generate(c, buf);
		}
	}

	return;
}

bool
WmP2_HASH_2::doEvaluate(WControl &c, RObject &r)
{
	if(hash_) {
		RmP2_HASH_2 &rm = (RmP2_HASH_2 &)r;
		const PvObject *ro = rm.pvalue();
		const PvObject *eo = rm.calc_pvalue();

		if(!eo) {
			eo = revaluate();
		}

		return(valueEvaluate(c, ro, eo));
	}

	return(WmObject::doEvaluate(c, r));
}
#undef SUPER



//
// Signature Payload
//
////////////////////////////////

McPld_ISAKMP_SIG::McPld_ISAKMP_SIG(CSTR key): McPld_ISAKMP(key) {}
McPld_ISAKMP_SIG::~McPld_ISAKMP_SIG() {}



//
// Nonce Payload
//
////////////////////////////////

McPld_ISAKMP_NONCE::McPld_ISAKMP_NONCE(CSTR key): McPld_ISAKMP(key) {}
McPld_ISAKMP_NONCE::~McPld_ISAKMP_NONCE() {}



//
// Notification Payload
//
////////////////////////////////

McPld_ISAKMP_N_ONE *McPld_ISAKMP_N_ONE::instance_ = 0;

McPld_ISAKMP_N_ONE *
McPld_ISAKMP_N_ONE::instance()
{
	if(!instance_) {
		instance_ = new McPld_ISAKMP_N_ONE("Pld_ISAKMP_N_ONE");
	}

	return(instance_);
}

McPld_ISAKMP_N_ONE::McPld_ISAKMP_N_ONE(CSTR key): McPld_ISAKMP(key)
{
	member(new MmDOIonN("payload"));

	MmPayload_onISAKMP::add(this);
	MmISAKMP_PonSA::add(this);
	MmISAKMP_TonP::add(this);
}

McPld_ISAKMP_N_ONE::~McPld_ISAKMP_N_ONE() {}

bool
McPld_ISAKMP_N_ONE::containsMc(const MObject *mc) const
{
	bool rtn = McPld_ISAKMP::containsMc(mc);
	return(rtn? rtn: members_[0]->containsMc(mc));
}

RObject *
McPld_ISAKMP_N_ONE::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	return(members_[0]->reverse(c, r_parent, at, buf));
}



McPld_ISAKMP_N::McPld_ISAKMP_N(CSTR key):
	McPld_ISAKMP(key), DOI_(0), SPIsize_(0), SPI_(0), NotifyMessageType_(0)
{
	McPld_ISAKMP_N_ONE::instance();
}

McPld_ISAKMP_N::~McPld_ISAKMP_N() {}

bool
McPld_ISAKMP_N::overwrite_DictType(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	if(buf.remainLength(at.bytes())) {
		ItPosition tmpat = at;

		RObject *r_self = McPld_ISAKMP::reverse(c, 0, tmpat, buf);
		if(r_self) {
			RObject *doi = (RObject *)r_self->corresponding(DOI_);
			if(doi) {
				const PvNumber *pv =
					(const PvNumber *)doi->pvalue();

				uint32_t doivalue = pv->value();
				c.DictType().type_Set(doivalue);

				return(true);
			}
		}
	}

	return(false);
}

RObject *
McPld_ISAKMP_N::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
        bool sizectrl = false;
	MmISAKMP_SPI *mm = 0;

	if(SPI_ && SPIsize_) {
		ItPosition tmpat = at;

		RObject *r_self = McPld_ISAKMP::reverse(c, 0, tmpat, buf);

		if(!c.error()){
			RObject *spisize =
				(RObject *)r_self->corresponding(SPIsize_);

			if(spisize) {
				const PvNumber *pv =
					(const PvNumber *)spisize->pvalue();

				if(pv) {
					uint32_t size = pv->value();
#ifdef ISAKMP_DBG
	xdbg("/tmp/isakmp_dbg.txt", "McPld_ISAKMP_N",
		"[N: 1] size: %d\n", size);
#endif  // ISAKMP_DBG
					mm = (MmISAKMP_SPI *)SPI_;
					mm->set_ctrl(false, size);
					sizectrl = true;
				}
			}
		}
	}

	RObject *ro = McPld_ISAKMP::reverse(c, r_parent, at, buf);

	if(sizectrl) {
		mm->set_ctrl(true, 0);
	}

	return(ro);
}

bool
McPld_ISAKMP_N::HCGENE(DOI)(WControl &cntr, WObject *wmem, OCTBUF &buf) const
{
	int32_t val = get_DOIvalue(wmem);
	if(val == -1) {
		return(false);
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *
McPld_ISAKMP_N::HCEVAL(DOI)(WObject *wmem) const
{
	int32_t val = get_DOIvalue(wmem);
	return(new PvNumber(val));
}

int32_t
McPld_ISAKMP_N::get_DOIvalue(WObject *wmem) const
{
	WObject *wc = wmem->parent();

	int32_t rtn = wc? wc->meta()->DOIvalue(): -1;
	if(rtn == -1) {
		wmem->mustDefine(0);
	}

	return(rtn);
}

bool
McPld_ISAKMP_N::HCGENE(NotifyMessageType)(WControl &cntr,
	WObject *wmem, OCTBUF &buf) const
{
	int32_t val = get_NotifyMessageType_value(wmem);
	if(val == -1) {
		return(false);
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *
McPld_ISAKMP_N::HCEVAL(NotifyMessageType)(WObject *wmem) const
{
	int32_t val = get_NotifyMessageType_value(wmem);
	return(new PvNumber(val));
}

int32_t
McPld_ISAKMP_N::get_NotifyMessageType_value(WObject *wmem) const
{
	WObject *wc = wmem->parent();

	int32_t rtn = wc? wc->meta()->notify_value(): -1;
	if(rtn == -1) {
		wmem->mustDefine(0);
	}

	return(rtn);
}

bool
McPld_ISAKMP_N::HCGENE(SPIsize)(WControl &cntr,
        WObject *wmem, OCTBUF &buf) const
{
	WObject *wobj0	= wmem->nextSister();	// NotifyMessageType
	WObject *wobj1	= wobj0->nextSister();	// SPI
	uint32_t spilen	= wobj1->size().bytes();
	PvNumber def(spilen);

	return(def.generate(cntr, wmem, buf));
}



McPld_ISAKMP_N_ANY::McPld_ISAKMP_N_ANY(CSTR key): McPld_ISAKMP_N(key) {}
McPld_ISAKMP_N_ANY::~McPld_ISAKMP_N_ANY() {}

McPld_ISAKMP_N_ISAKMP::McPld_ISAKMP_N_ISAKMP(CSTR key): McPld_ISAKMP_N(key) {}
McPld_ISAKMP_N_ISAKMP::~McPld_ISAKMP_N_ISAKMP() {}

McPld_ISAKMP_N_IPsec_ONE *McPld_ISAKMP_N_IPsec_ONE::instance_ = 0;

McPld_ISAKMP_N_IPsec_ONE *
McPld_ISAKMP_N_IPsec_ONE::instance()
{
	if(!instance_) {
		instance_ = new McPld_ISAKMP_N_IPsec_ONE(
			"Pld_ISAKMP_N_IPsec_ONE");
	}

	return(instance_);
}

McPld_ISAKMP_N_IPsec_ONE::McPld_ISAKMP_N_IPsec_ONE(CSTR key):
	McPld_ISAKMP(key)
{
	member(new MmType_onN_IPsec("payload"));
	MmDOIonN::add(this);
}

McPld_ISAKMP_N_IPsec_ONE::~McPld_ISAKMP_N_IPsec_ONE() {}

bool
McPld_ISAKMP_N_IPsec_ONE::containsMc(const MObject *mc) const
{
	bool rtn = McPld_ISAKMP::containsMc(mc);
	return(rtn ? rtn : members_[0]->containsMc(mc));
}

RObject *
McPld_ISAKMP_N_IPsec_ONE::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	return(members_[0]->reverse(c, r_parent, at, buf));
}

McPld_ISAKMP_N_IPsec::McPld_ISAKMP_N_IPsec(CSTR key): McPld_ISAKMP_N(key)
{
	McPld_ISAKMP_N_IPsec_ONE::instance();
}

McPld_ISAKMP_N_IPsec::~McPld_ISAKMP_N_IPsec() {}

bool
McPld_ISAKMP_N_IPsec::overwrite_DictType(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	if(buf.remainLength(at.bytes())) {
		ItPosition tmpat = at;

		RObject *r_self = McPld_ISAKMP_N::reverse(c, 0, tmpat, buf);
		if(r_self) {
			RObject *type = (RObject *)r_self->corresponding(
						NotifyMessageType_);
			if(type) {
				const PvNumber *pv =
					(const PvNumber *)type->pvalue();

				uint32_t typevalue = pv->value();
				c.DictType().type_Set(typevalue);

				return(true);
			}
		}
	}

	return(false);
}



McPld_ISAKMP_N_IPsec_ANY::McPld_ISAKMP_N_IPsec_ANY(CSTR key):
	McPld_ISAKMP_N_IPsec(key) {}
McPld_ISAKMP_N_IPsec_ANY::~McPld_ISAKMP_N_IPsec_ANY() {}

McPld_ISAKMP_N_IPsec_ResponderLT::McPld_ISAKMP_N_IPsec_ResponderLT(CSTR key):
	McPld_ISAKMP_N_IPsec(key) {}
McPld_ISAKMP_N_IPsec_ResponderLT::~McPld_ISAKMP_N_IPsec_ResponderLT() {}

McPld_ISAKMP_N_IPsec_ReplayStat::McPld_ISAKMP_N_IPsec_ReplayStat(CSTR key):
	McPld_ISAKMP_N_IPsec(key) {}
McPld_ISAKMP_N_IPsec_ReplayStat::~McPld_ISAKMP_N_IPsec_ReplayStat() {}
#if 0
uint32_t
McPld_ISAKMP_N_IPsec_ReplayStat::length_for_reverse(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	uint32_t length = McPld_ISAKMP_N_IPsec::length_for_reverse(c, at, buf);
#ifdef ISAKMP_DBG
	xdbg("/tmp/isakmp_dbg.txt", "McPld_ISAKMP_N_IPsec",
		"[N: 1] length: %d\n", length);
#endif  // ISAKMP_DBG
	return(length);
}
#endif



//
// Delete Payload
//
////////////////////////////////

//#define ISAKMP_D_OFFSET	12
SPIcontrol_onD::SPIcontrol_onD(): size_(0), num_(0) {}
SPIcontrol_onD::~SPIcontrol_onD() {}

McPld_ISAKMP_D::McPld_ISAKMP_D(CSTR key):
	McPld_ISAKMP(key), SPIsize_(0), NumberOfSPI_(0),
	spi_(new SPIcontrol_onD()) {}

McPld_ISAKMP_D::~McPld_ISAKMP_D() { if(spi_) { delete spi_; spi_ = 0; } }

bool
McPld_ISAKMP_D::HCGENE(SPIsize)(WControl &cntr,
	WObject *wmem, OCTBUF &buf) const
{
	PvNumber def(get_size());
//	spi_->set_size(0);
#ifdef ISAKMP_DBG
	xdbg("/tmp/isakmp_dbg.txt",
		"McPld_ISAKMP_D", "[D: 0] this:  %p\n", this);
#endif	// ISAKMP_DBG
	return(def.generate(cntr, wmem, buf));
}

bool
McPld_ISAKMP_D::HCGENE(NumberOfSPI)(WControl &cntr,
	WObject *wmem, OCTBUF &buf) const
{
	PvNumber def(get_num());
//	spi_->set_num(0);
#ifdef ISAKMP_DBG
	xdbg("/tmp/isakmp_dbg.txt",
		"McPld_ISAKMP_D", "[D: 0] this:  %p\n", this);
#endif	// ISAKMP_DBG
	return(def.generate(cntr, wmem, buf));
}

uint32_t
McPld_ISAKMP_D::HC_MLC(SPI)(const ItPosition &at, OCTBUF &buf) const
{
	uint32_t num	= get_num();

#ifdef ISAKMP_DBG
	xdbg("/tmp/isakmp_dbg.txt", "McPld_ISAKMP_D", "[D: 0] num:  %d\n", num);
#endif	// ISAKMP_DBG

	return(num);
} 

WObject *
McPld_ISAKMP_D::composeWc(WControl &c,
	WObject *w_parent, const PObject *pc) const
{
        WObject *w_self = McPld_ISAKMP::composeWc(c, w_parent, pc);
#ifdef ISAKMP_DBG
	xdbg("/tmp/isakmp_dbg.txt", "McPld_ISAKMP_D",
		"[D: 0] this: %p\n", this);
#endif	// ISAKMP_DBG
	if(spi_) {
		spi_->set_size(0);
		spi_->set_num(0);
	}

	return(w_self);
}
//#undef ISAKMP_D_OFFSET



MmSPIsize::MmSPIsize(McPld_ISAKMP_D *parent, CSTR s, uint16_t n,
	const PObject *g, const PObject *e,
	const ICVoverwriter *ow, METH_HC_ForIPinfo meth):
	MmUint(s, n, g, e, ow, meth), delete_(parent) {}
MmSPIsize::~MmSPIsize() {}

RObject *
MmSPIsize::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	RObject *r_self = MmUint::reverse(c, r_parent, at, buf);

	if(r_self) {
		PvNumber *pv = (PvNumber *)r_self->pvalue();

		if(pv) {
			uint32_t size = pv->value();
#ifdef ISAKMP_DBG
			xdbg("/tmp/isakmp_dbg.txt", "MmSPIsize",
				"[D: 0] size: %d\n", size);
#endif	// ISAKMP_DBG
			if(delete_) {
				delete_->set_size(size);
			}
		}
	}

	return(r_self);
}



MmNumberOfSPI::MmNumberOfSPI(McPld_ISAKMP_D *parent, CSTR s, uint16_t n,
	const PObject *g, const PObject *e,
	const ICVoverwriter *ow, METH_HC_ForIPinfo meth):
	MmUint(s, n, g, e, ow, meth), delete_(parent) {}
MmNumberOfSPI::~MmNumberOfSPI() {}

RObject *
MmNumberOfSPI::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	RObject *r_self = MmUint::reverse(c, r_parent, at, buf);
	if(r_self) {
		PvNumber *pv = (PvNumber *)r_self->pvalue();

		if(pv) {
			uint32_t num = pv->value();
#ifdef ISAKMP_DBG
			xdbg("/tmp/isakmp_dbg.txt", "MmNumberOfSPI",
				"[D: 0] num: %d\n", num);
#endif	// ISAKMP_DBG
			if(delete_) {
				delete_->set_num(num);
			}
		}
	}
	return(r_self);
}



MmSPI_onD::MmSPI_onD(McPld_ISAKMP_D *parent, CSTR s,
	const PObject *g, const PObject *e,
	const ICVoverwriter *ow, METH_HC_ForIPinfo meth):
	MmVarOctets(s, g, e, ow, meth), delete_(parent) {}

MmSPI_onD::~MmSPI_onD() {}

ItPosition
MmSPI_onD::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const
{
        uint32_t length = delete_? delete_->get_size(): 0;
#ifdef ISAKMP_DBG
	xdbg("/tmp/isakmp_dbg.txt", "MmSPI_onD", "[D: 0] length: %d\n", length);
#endif	// ISAKMP_DBG
        return(ItPosition(length, 0));
}

WObject *
MmSPI_onD::composeWm(WControl &c, WObject *w_parent, const PObject *pl) const
{
	WObject *w_self = MmVarOctets::composeWm(c, w_parent, pl);

	if(w_self) {
		uint32_t length	= 0;
		uint32_t num	= 0;
		uint32_t size	= 0;

		length = w_self->length();
#ifdef ISAKMP_DBG
		xdbg("/tmp/isakmp_dbg.txt",
			"MmSPI_onD", "[D: 0] length: %d\n", length);
#endif	// ISAKMP_DBG
		if(delete_) {
			size	= delete_->get_size();
			num	= delete_->get_num();

			if(!num) {
				delete_->set_size(length);
			}

			delete_->set_num(num + 1);
		}
	}

	return(w_self);
}



//
// Vendor ID Payload
//
////////////////////////////////

McPld_ISAKMP_VID::McPld_ISAKMP_VID(CSTR key): McPld_ISAKMP(key) {}
McPld_ISAKMP_VID::~McPld_ISAKMP_VID() {}



//
// Data Attributes
//
////////////////////////////////

McAttr_ISAKMP::McAttr_ISAKMP(CSTR key): McOption(key), Attr_(0) {}
McAttr_ISAKMP::~McAttr_ISAKMP() {}

bool
McAttr_ISAKMP::HCGENE(AF)(WControl &cntr, WObject *wmem, OCTBUF &buf) const
{
	int32_t val = get_attr_format(wmem);

	if(val == -1) {
		return(false);
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *
McAttr_ISAKMP::HCEVAL(AF)(WObject *wmem) const
{
	int32_t val = get_attr_format(wmem);
	return(new PvNumber(val));
}

int32_t
McAttr_ISAKMP::get_attr_format(WObject *wmem) const
{
	WObject *wc = wmem->parent();
	int32_t rtn = wc? wc->meta()->headerType(): -1;

	if(rtn == -1) {
		wmem->mustDefine(0);
	}

	return(rtn);
}

bool
McAttr_ISAKMP::overwrite_DictType(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	if(buf.remainLength(at.bytes())) {
		ItPosition tmpat = at;

		RObject *r_self = McOption::reverse(c, 0, tmpat, buf);
		if(r_self) {
			RObject *attr = (RObject *)r_self->corresponding(Attr_);
			if(attr) {
				const PvNumber *pv =
					(const PvNumber *)attr->pvalue();

				uint32_t attrvalue = pv->value();
				c.DictType().type_Set(attrvalue);

				return(true);
			}
		}
	}

	return(false);
}



#ifndef ATTR_V_OFFSET
#define ATTR_V_OFFSET	4
#endif	// ATTR_V_OFFSET
McAttr_ISAKMP_TLV::McAttr_ISAKMP_TLV(CSTR key):
	McAttr_ISAKMP(key), Length_(0) {}

McAttr_ISAKMP_TLV::~McAttr_ISAKMP_TLV() {}

bool
McAttr_ISAKMP_TLV::HCGENE(Length)(WControl &cntr,
	WObject *wmem, OCTBUF &buf) const
{
	WObject *wc	= wmem->parent();	// McAttr_ISAKMP_TLV
	uint32_t length	= wc->size().bytes();
	length		-= ATTR_V_OFFSET;
	PvNumber def(length);

	return(def.generate(cntr, wmem, buf));
}

uint32_t
McAttr_ISAKMP_TLV::length_for_reverse(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	return(
		Length_?
		Length_->value(at, buf) + ATTR_V_OFFSET:
		McAttr_ISAKMP::length_for_reverse(c, at, buf)
	);
}
#ifdef ATTR_V_OFFSET
#undef ATTR_V_OFFSET
#endif	// ATTR_V_OFFSET



McAttr_ISAKMP_TV::McAttr_ISAKMP_TV(CSTR key): McAttr_ISAKMP(key) {}
McAttr_ISAKMP_TV::~McAttr_ISAKMP_TV() {}



//
// SPI (P, N)
//
////////////////////////////////

SPIsizeCtrl::SPIsizeCtrl(): init_(true), length_(0) {}
SPIsizeCtrl::~SPIsizeCtrl() {}

MmISAKMP_SPI::MmISAKMP_SPI(CSTR s, const PObject *g, const PObject *e,
	const ICVoverwriter *ow, METH_HC_ForIPinfo meth):
	MmVarOctets(s, g, e, ow, meth), ctrl_(new SPIsizeCtrl()) {}

MmISAKMP_SPI::~MmISAKMP_SPI() { if(ctrl_) { delete ctrl_; ctrl_ = 0; } }

ItPosition
MmISAKMP_SPI::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const
{
#if 1
	uint32_t length = ctrl_->get_init()?
				buf.remainLength(at.bytes()):
				ctrl_->get_length();
//	uint32_t length = ctrl_->get_init()?
//				0:
//				ctrl_->get_length();
#else
	uint32_t length = 0;

	if(ctrl_->get_init()) {
		length	= buf.remainLength(at.bytes());
	} else {
		length	= ctrl_->get_length();
	}
#endif
	return(ItPosition(length, 0));
}

void
MmISAKMP_SPI::set_ctrl(bool b, uint32_t d)
{
	ctrl_->set_init(b);
	ctrl_->set_length(d);

	return;
}

MmISAKMP_SPI_ReplayStat::MmISAKMP_SPI_ReplayStat(CSTR s,
	const PObject *g, const PObject *e,
	const ICVoverwriter *ow, METH_HC_ForIPinfo meth):
	MmISAKMP_SPI(s, g, e, ow, meth) {}

MmISAKMP_SPI_ReplayStat::~MmISAKMP_SPI_ReplayStat() {
	if(ctrl_) { delete ctrl_; ctrl_ = 0; } }

ItPosition
MmISAKMP_SPI_ReplayStat::length_for_reverse(RControl &,
	ItPosition &at, OCTBUF &buf) const
{
	uint32_t length = ctrl_->get_init()?
				buf.remainLength(at.bytes()) - 4:
				ctrl_->get_length();

	return(ItPosition(length, 0));
}



//
// FQDN, USER_FQDN (ID)
//
////////////////////////////////

MmAsciiString::MmAsciiString(CSTR s,
	const PObject *g, const PObject *e, const ICVoverwriter *ow,
	METH_HC_ForIPinfo meth): MmVarOctets(s, g, e, ow, meth) {}

MmAsciiString::~MmAsciiString() {}

PvObject *
MmAsciiString::reversePv(RControl &, const ItPosition &at,
	const ItPosition &size, const OCTBUF &buf) const
{
	uint32_t offset = at.bytes(), length = size.bytes();
	return(new PvAsciiString(length, (OCTSTR)buf.string(offset)));
}

PvAsciiString::PvAsciiString(): PvOctets() {}
PvAsciiString::PvAsciiString(uint32_t l, OCTSTR o, bool b): PvOctets(l, o, b) {}
PvAsciiString::~PvAsciiString() {}

PvObject *
PvAsciiString::shallowCopy() const
{
	return(new PvAsciiString(length(), (OCTSTR)string()));
}

void
PvAsciiString::print() const
{
	COCTSTR buf = string();
	uint32_t buflen = length(), d = 0;

	for(d = 0; d < buflen; d ++) {
		putchar(buf[d]);
	}

	return;
}

void PvAsciiString::log(uint32_t t) const { print(); return; }



//
// McUdp_ISAKMP <-> McISAKMP_Encryption
//
////////////////////////////////

MmEncrypt_onISAKMP::MmEncrypt_onISAKMP(CSTR key, McHdr_ISAKMP *tophdr):
	MmReference_More0(key, true), tophdr_(tophdr) {}

MmEncrypt_onISAKMP::~MmEncrypt_onISAKMP() {}

void
MmEncrypt_onISAKMP::add_other(McISAKMP_Encryption *mc)
{
	dict_.add_other(mc);
}

TypevsMcDict MmEncrypt_onISAKMP::dict_;

bool
MmEncrypt_onISAKMP::overwrite_DictType(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	if(tophdr_) {
		EFlagValueCtrl *ectrl = tophdr_->get_ectrl();

		if(ectrl && (ectrl->get_e())) {
			return(true);
		}
	}

	return(false);
}

RObject *
MmEncrypt_onISAKMP::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	RObject *rc = 0;
	const TypevsMcDict *dict = get_dict();
	Con_DictType keep = c.DictType();

	if(overwrite_DictType(c, at, buf)){
		MObject *mc = dict->find(c.DictType());
		rc = mc? mc->reverse(c, r_parent, at, buf): 0;
	}

	if(keepType_) {
		c.DictType() = keep;
	}

	return(rc);
}



//
// McUdp_ISAKMP <-> McPld_ISAKMP
//
////////////////////////////////

MmPayload_onISAKMP::MmPayload_onISAKMP(CSTR key):
	MmReference_More0(key, true) {}

MmPayload_onISAKMP::~MmPayload_onISAKMP() {}

void
MmPayload_onISAKMP::add(McPld_ISAKMP *mc)
{
        dict_.add(mc->headerType(), mc);
}

void
MmPayload_onISAKMP::add_other(McPld_ISAKMP *mc)
{
	dict_.add_other(mc);
}

TypevsMcDict MmPayload_onISAKMP::dict_;

bool
MmPayload_onISAKMP::overwrite_DictType(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	if(buf.remainLength(at.bytes())) {
		return(true);
	}

	return(false);
}



//
// McPld_ISAKMP_SA <-> McPld_ISAKMP_P
//
////////////////////////////////

MmISAKMP_PonSA::MmISAKMP_PonSA(CSTR key): MmReference_More0(key, true) {}
MmISAKMP_PonSA::~MmISAKMP_PonSA() {}

void
MmISAKMP_PonSA::add(McPld_ISAKMP *mc)
{
        dict_.add(mc->headerType(), mc);
}

void
MmISAKMP_PonSA::add_other(McPld_ISAKMP *mc)
{
	dict_.add_other(mc);
}

TypevsMcDict MmISAKMP_PonSA::dict_;

bool
MmISAKMP_PonSA::overwrite_DictType(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	if(buf.remainLength(at.bytes())) {
		return(true);
	}

	return(false);
}

RObject *
MmISAKMP_PonSA::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	RObject *rc = 0;
	const TypevsMcDict *dict = get_dict();
	Con_DictType keep = c.DictType();
	bool init = true;

	do{
		rc=0;

		if(overwrite_DictType(c, at, buf)) {
			if(init) {
				c.DictType().type_Set(TP_ISAKMP_NPTYPE_P);
				init = false;
			}

			MObject *mc = dict->find(c.DictType());

			if(!mc) {
				break;
			}

			rc = mc? mc->reverse(c, r_parent, at, buf): 0;
		}
	} while(rc && c);

	if(keepType_) {
		c.DictType() = keep;
	}

	return(rc);
}



//
// McPld_ISAKMP_P <-> McPld_ISAKMP_T
//
////////////////////////////////

MmISAKMP_TonP::MmISAKMP_TonP(McPld_ISAKMP_P *p, CSTR key):
	MmReference_More0(key, true), p_(p) {}

MmISAKMP_TonP::~MmISAKMP_TonP() {}

void
MmISAKMP_TonP::add(McPld_ISAKMP *mc)
{
        dict_.add(mc->headerType(), mc);
}

void
MmISAKMP_TonP::add_other(McPld_ISAKMP *mc)
{
	dict_.add_other(mc);
}

TypevsMcDict MmISAKMP_TonP::dict_;

bool
MmISAKMP_TonP::overwrite_DictType(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	if(buf.remainLength(at.bytes())) {
		return(true);
	}

	return(false);
}

WObject *
MmISAKMP_TonP::compose(WControl &c, WObject *w_parent, const PObject *pl) const
{
	if(!pl) {
		return(0);
	}

	const PObject *right = pl->rvalue();
        if(!right) {
		w_parent->mustDefineMem(this);
		return(0);
	}

	const TypevsMcDict* keep = c.dict();
	c.dict_set(get_dict());
	WObject *wc = right->selfCompose(c, w_parent);
	c.dict_set(keep);

	p_->increase_transforms();

	return(wc);
}

RObject *
MmISAKMP_TonP::reverse(RControl &c,
	RObject *r_parent, ItPosition &at, OCTBUF &buf) const
{
	RObject *rc = 0;
	const TypevsMcDict *dict = get_dict();
	Con_DictType keep = c.DictType();
	bool init = true;

	do{
		rc=0;

		if(overwrite_DictType(c, at, buf)) {
			if(init) {
				c.DictType().type_Set(TP_ISAKMP_NPTYPE_T);
				init = false;
			}

			MObject *mc = dict->find(c.DictType());

			if(!mc) {
				break;
			}

			rc = mc? mc->reverse(c, r_parent, at, buf): 0;
		}
	} while(rc && c);

	if(keepType_) {
		c.DictType() = keep;
	}

	return(rc);
}



//
// McPld_ISAKMP_SA_ONE <-> McPld_ISAKMP_SA
//
////////////////////////////////

MmDOIonSA::MmDOIonSA(CSTR key): MmReference_Must1(key) {}
MmDOIonSA::~MmDOIonSA() {}

void
MmDOIonSA::add(McPld_ISAKMP *mc)
{
	dict_.add(mc->DOIvalue(), mc);
}

void
MmDOIonSA::add_other(McPld_ISAKMP *mc)
{
	dict_.add_other(mc);
}

TypevsMcDict MmDOIonSA::dict_;

bool
MmDOIonSA::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const
{
	McPld_ISAKMP_SA *any = (McPld_ISAKMP_SA *)dict_.find_other();
	return(any->overwrite_DictType(c, at, buf));
}



//
// McPld_ISAKMP_N_ONE <-> McPld_ISAKMP_N
//
////////////////////////////////

MmDOIonN::MmDOIonN(CSTR key): MmReference_Must1(key) {}
MmDOIonN::~MmDOIonN() {}

void
MmDOIonN::add(McPld_ISAKMP *mc)
{
	dict_.add(mc->DOIvalue(), mc);
}

void
MmDOIonN::add_other(McPld_ISAKMP *mc)
{
	dict_.add_other(mc);
}

TypevsMcDict MmDOIonN::dict_;

bool
MmDOIonN::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const
{
	McPld_ISAKMP_N *any = (McPld_ISAKMP_N *)dict_.find_other();
	return(any->overwrite_DictType(c, at, buf));
}



//
// McPld_ISAKMP_SA_IPsec_ONE <-> McPld_ISAKMP_SA_IPsec
//
////////////////////////////////

MmSIT_onSA_IPsec::MmSIT_onSA_IPsec(CSTR key): MmReference_Must1(key) {}
MmSIT_onSA_IPsec::~MmSIT_onSA_IPsec() {}

void
MmSIT_onSA_IPsec::add(McPld_ISAKMP_SA_IPsec *mc)
{
	dict_.add(mc->SITvalue(), mc);
}

void
MmSIT_onSA_IPsec::add_other(McPld_ISAKMP_SA_IPsec *mc)
{
	dict_.add_other(mc);
}

TypevsMcDict MmSIT_onSA_IPsec::dict_;

bool
MmSIT_onSA_IPsec::overwrite_DictType(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	McPld_ISAKMP_SA_IPsec *any
		= (McPld_ISAKMP_SA_IPsec *)dict_.find_other();

	return(any->overwrite_DictType(c, at, buf));
}



//
// McPld_ISAKMP_N_IPsec_ONE <-> McPld_ISAKMP_N_IPsec
//
////////////////////////////////

MmType_onN_IPsec::MmType_onN_IPsec(CSTR key): MmReference_Must1(key) {}
MmType_onN_IPsec::~MmType_onN_IPsec() {}

void
MmType_onN_IPsec::add(McPld_ISAKMP_N_IPsec *mc)
{
	dict_.add(mc->notify_value(), mc);
}

void
MmType_onN_IPsec::add_other(McPld_ISAKMP_N_IPsec *mc)
{
	dict_.add_other(mc);
}

TypevsMcDict MmType_onN_IPsec::dict_;

bool
MmType_onN_IPsec::overwrite_DictType(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	McPld_ISAKMP_N_IPsec *any
		= (McPld_ISAKMP_N_IPsec *)dict_.find_other();

	return(any->overwrite_DictType(c, at, buf));
}



//
// McPld_ISAKMP_P_ONE <-> McPld_ISAKMP_P
//
////////////////////////////////

MmProtocolIDonP::MmProtocolIDonP(CSTR key): MmReference_Must1(key) {}
MmProtocolIDonP::~MmProtocolIDonP() {}

void
MmProtocolIDonP::add(McPld_ISAKMP *mc)
{
	dict_.add(mc->ProtocolID(), mc);
}

void
MmProtocolIDonP::add_other(McPld_ISAKMP *mc)
{
	dict_.add_other(mc);
}

TypevsMcDict MmProtocolIDonP::dict_;

bool
MmProtocolIDonP::overwrite_DictType(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	McPld_ISAKMP_P *any = (McPld_ISAKMP_P *)dict_.find_other();
	return(any->overwrite_DictType(c, at, buf));
}



//
// McPld_ISAKMP_ID_ONE <-> McPld_ISAKMP_ID
//
////////////////////////////////

MmIDtype_onID::MmIDtype_onID(CSTR key): MmReference_Must1(key) {}
MmIDtype_onID::~MmIDtype_onID() {}

void
MmIDtype_onID::add(McPld_ISAKMP *mc)
{
	dict_.add(mc->IDtype(), mc);
}

void
MmIDtype_onID::add_other(McPld_ISAKMP *mc)
{
	dict_.add_other(mc);
}

TypevsMcDict MmIDtype_onID::dict_;

bool
MmIDtype_onID::overwrite_DictType(RControl &c,
	ItPosition &at, OCTBUF &buf) const
{
	McPld_ISAKMP_ID *any = (McPld_ISAKMP_ID *)dict_.find_other();
	return(any->overwrite_DictType(c, at, buf));
}



//
// McPld_ISAKMP_T <-> McAttr_ISAKMP
//
////////////////////////////////

MmAttr_onT::MmAttr_onT(CSTR key): MmReference_More0(key) {}
MmAttr_onT::~MmAttr_onT() {}

void
MmAttr_onT::add(McAttr_ISAKMP *mc)
{
	dict_.add(mc->headerType(), mc);
}

void
MmAttr_onT::add_other(McAttr_ISAKMP *mc)
{
	dict_.add_other(mc);
}

TypevsMcDict MmAttr_onT::dict_;

bool
MmAttr_onT::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const
{
	McAttr_ISAKMP *any = (McAttr_ISAKMP *)dict_.find_other();
	return(any->overwrite_DictType(c, at, buf));
}

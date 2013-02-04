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
 * $TAHI: v6eval/lib/Pz/McDNS.cc,v 1.4 2003/12/04 04:59:47 akisada Exp $
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
#include "McDNS.h"
#include "MmObject.h"
#include <stdio.h>

////////////////////////////////////////////////////////////////
RmQType::RmQType(RObject *r_parent, const MObject *m, const ItPosition &offset, const ItPosition &size, PvObject *pv):
	RmObject(r_parent, m, offset, size, pv) {}

RmQType::~RmQType() {}

void RmQType::printName(uint32_t t, CSTR) const {
	RObject::printName(t);

	const PvObject *o = pvalue();
	if(o) {
		printf(" = ");
		o->print();

		uint32_t qtype = (uint32_t)(((PvNumber *)o)->value());
		printQType(qtype);
	} else {
		printf("NO OBJECT");
	}
}

void RmQType::logSelf(uint32_t t, CSTR cls) const {
	RObject::logSelf(t, cls);

        const PvObject *o = pvalue();
        if(o) {
		printf(" = ");
		o->log();

		uint32_t qtype = (uint32_t)(((PvNumber *)o)->value());
		printQType(qtype);
	}
}

void RmQType::printQType(uint32_t qtype) const {
	switch(qtype) {
		case   1: printf(" (A)");		break;
		case   2: printf(" (NS)");		break;
		case   3: printf(" (MD)");		break;
		case   4: printf(" (MF)");		break;
		case   5: printf(" (CNAME)");		break;
		case   6: printf(" (SOA)");		break;
		case   7: printf(" (MB)");		break;
		case   8: printf(" (MG)");		break;
		case   9: printf(" (MR)");		break;
		case  10: printf(" (NULL)");		break;
		case  11: printf(" (WKS)");		break;
		case  12: printf(" (PTR)");		break;
		case  13: printf(" (HINFO)");		break;
		case  14: printf(" (MINFO)");		break;
		case  15: printf(" (MX)");		break;
		case  16: printf(" (TXT)");		break;
		case  17: printf(" (RP)");		break;
		case  18: printf(" (AFSDB)");		break;
		case  19: printf(" (X25)");		break;
		case  20: printf(" (ISDN)");		break;
		case  21: printf(" (RT)");		break;
		case  22: printf(" (NSAP)");		break;
		case  23: printf(" (NSAP-PTR)");	break;
		case  24: printf(" (SIG)");		break;
		case  25: printf(" (KEY)");		break;
		case  26: printf(" (PX)");		break;
		case  27: printf(" (GPOS)");		break;
		case  28: printf(" (AAAA)");		break;
		case  29: printf(" (LOC)");		break;
		case  30: printf(" (NXT)");		break;
		case  31: printf(" (EID)");		break;
		case  32: printf(" (NIMLOC)");		break;
		case  33: printf(" (SRV)");		break;
		case  34: printf(" (ATMA)");		break;
		case  35: printf(" (NAPTR)");		break;
		case  36: printf(" (KX)");		break;
		case  37: printf(" (CERT)");		break;
		case  38: printf(" (A6)");		break;
		case  39: printf(" (DNAME)");		break;
		case  40: printf(" (SINK)");		break;
		case  41: printf(" (OPT)");		break;
		case  42: printf(" (APL)");		break;
		case 100: printf(" (UINFO)");		break;
		case 101: printf(" (UID)");		break;
		case 102: printf(" (GID)");		break;
		case 103: printf(" (UNSPEC)");		break;
		case 249: printf(" (TKEY)");		break;
		case 250: printf(" (TSIG)");		break;
		case 251: printf(" (IXFR)");		break;
		case 252: printf(" (AXFR)");		break;
		case 253: printf(" (MAILB)");		break;
		case 254: printf(" (MAILA)");		break;
		case 255: printf(" (ANY)");		break;
		default:  printf(" (Unknown)");		break;
	}

	return;
}

////////////////////////////////////////////////////////////////
MmQType::MmQType(CSTR s, uint16_t n, const PObject *g, const PObject *e, const ICVoverwriter *ow, METH_HC_ForIPinfo meth):
	MmUint(s, n, g, e, ow, meth) {}

MmQType::~MmQType() {}

RObject *MmQType::reverseRm(RControl &, RObject *r_parent, const ItPosition &at, const ItPosition &size, PvObject *pv) const {
	return(new RmQType(r_parent, this, at, size, pv));
}

////////////////////////////////////////////////////////////////
PvDnsName::PvDnsName(): PvOctets() {}
PvDnsName::PvDnsName(uint32_t l, OCTSTR o, bool b): PvOctets(l, o, b) {}
PvDnsName::~PvDnsName() {}

PvObject *PvDnsName::shallowCopy() const {
	return(new PvDnsName(length(), (OCTSTR)string()));
}

void PvDnsName::log(uint32_t t) const {
	print();
}
 
void PvDnsName::print() const {
	COCTSTR buf = string();
	uint32_t buflen = length();

	for(uint32_t d = 0; d < buflen; d ++) {
		if((buf[d] & 0xff) & 0xc0) {
			uint32_t offset = ((buf[d] & 0xff) & 0x3f) * 0x100 + (buf[d + 1] & 0xff);
			printf(" (offset:%d)", offset);
			break;
		}

		COCTSTR label = buf + d + 1;
		uint32_t labellen = buf[d] & 0xff;

		if(d) {
			putchar('.');
		}

		for(uint32_t x = 0; x < labellen; x ++) {
			printf("%c", (label[x] & 0xff));
		}

		d += labellen;
	}
}

////////////////////////////////////////////////////////////////
PvDnsStr::PvDnsStr(): PvOctets() {}
PvDnsStr::PvDnsStr(uint32_t l, OCTSTR o, bool b): PvOctets(l, o, b) {}
PvDnsStr::~PvDnsStr() {}

PvObject *PvDnsStr::shallowCopy() const {
	return(new PvDnsStr(length(), (OCTSTR)string()));
}

void PvDnsStr::log(uint32_t t) const {
	print();
}
 
void PvDnsStr::print() const {
	COCTSTR buf = string();
	uint32_t buflen = length();

	for(uint32_t d = 1; d < buflen; d ++) {
		printf("%c", (buf[d] & 0xff));
	}
}

////////////////////////////////////////////////////////////////
MmDNSName::MmDNSName(CSTR s, const PObject *g, const PObject *e): MmVarOctets(s, g, e, 0, 0) {
	LengthCtl_ = new LengthCtl();
}

MmDNSName::~MmDNSName() {}

ItPosition MmDNSName::length_for_reverse(RControl &c, ItPosition &at, OCTBUF &buf) const {
	uint32_t length = buf.remainLength(at.bytes());

	if(LengthCtl_) {
		uint32_t reverse_length = LengthCtl_->get_length();

		if(reverse_length) {
			length = reverse_length;
			LengthCtl_->set_length(0);
		}
	}

	return(ItPosition(length, 0));
}

RObject *MmDNSName::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	if(LengthCtl_) {
		unsigned char *str = (unsigned char *)buf.buffer() + at.bytes();
		uint32_t length = buf.length() - at.bytes();

		uint32_t reallength = 0;

		for(uint32_t d = 0; d < length; d += (str[d] & 0xff), d ++) {
			reallength ++;

			if((str[d] & 0xff) == 0x00) {
				break;
			}

			if((str[d] & 0xff) & 0xc0) {
				reallength ++;
				break;
			}

			reallength += (str[d] & 0xff);
		}

		LengthCtl_->set_length(reallength);
	}

	return(MmVarOctets::reverse(c, r_parent, at, buf));
}

PvObject *MmDNSName::reversePv(RControl &, const ItPosition &at, const ItPosition &size, const OCTBUF &buf) const {
	uint32_t offset = at.bytes(), length = size.bytes();
	return(new PvDnsName(length, (OCTSTR)buf.string(offset)));
}

////////////////////////////////////////////////////////////////
MmDNSStr::MmDNSStr(CSTR s, const PObject *g, const PObject *e): MmVarOctets(s, g, e, 0, 0) {
	LengthCtl_ = new LengthCtl();
}

MmDNSStr::~MmDNSStr() {}

ItPosition MmDNSStr::length_for_reverse(RControl &c, ItPosition &at, OCTBUF &buf) const {
	uint32_t length = buf.remainLength(at.bytes());

	if(LengthCtl_) {
		uint32_t reverse_length = LengthCtl_->get_length();

		if(reverse_length) {
			length = reverse_length;
			LengthCtl_->set_length(0);
		}
	}

	return(ItPosition(length, 0));
}

RObject *MmDNSStr::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	if(LengthCtl_) {
		unsigned char *str = (unsigned char *)buf.buffer() + at.bytes();
		uint32_t reallength = (str[0] & 0xff) + 1;
		LengthCtl_->set_length(reallength);
	}

	return(MmVarOctets::reverse(c, r_parent, at, buf));
}

PvObject *MmDNSStr::reversePv(RControl &, const ItPosition &at, const ItPosition &size, const OCTBUF &buf) const {
	uint32_t offset = at.bytes(), length = size.bytes();
	return(new PvDnsStr(length, (OCTSTR)buf.string(offset)));
}

////////////////////////////////////////////////////////////////
McUdp_DNS *McUdp_DNS::instance_=0;

McUdp_DNS::McUdp_DNS(CSTR s): McHeader(s), QDCount_(0), ANCount_(0), NSCount_(0), ARCount_(0) {
	instance_ = this;
	CountMgr_ = new CountMgr();
}

McUdp_DNS::~McUdp_DNS() {
	if(instance_ == this) {
		instance_ = 0;
	}
}

uint32_t McUdp_DNS::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const {
	return(buf.remainLength(at.bytes()));
}

RObject *McUdp_DNS::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	ItPosition tmpat = at;

	RObject *r_self = McHeader::reverse(c, 0, tmpat, buf);

	if(r_self) {
		set_CountMgr(r_self);
	}

	return(McHeader::reverse(c, r_parent, at, buf));
}

void McUdp_DNS::set_CountMgr(RObject *r_self) const {
	if(CountMgr_) {
		RObject *QDCount = (RObject *)r_self->corresponding(QDCount_);
		RObject *ANCount = (RObject *)r_self->corresponding(ANCount_);
		RObject *NSCount = (RObject *)r_self->corresponding(NSCount_);
		RObject *ARCount = (RObject *)r_self->corresponding(ARCount_);

		if(QDCount) {
			PvNumber *pv = (PvNumber *)QDCount->pvalue();

			if(pv) {
				CountMgr_->set_reverse_QD(pv->value());
			}
		}

		if(ANCount) {
			PvNumber *pv = (PvNumber *)ANCount->pvalue();

			if(pv) {
				CountMgr_->set_reverse_AN(pv->value());
			}
		}

		if(NSCount) {
			PvNumber *pv = (PvNumber *)NSCount->pvalue();

			if(pv) {
				CountMgr_->set_reverse_NS(pv->value());
			}
		}

		if(ARCount) {
			PvNumber *pv = (PvNumber *)ARCount->pvalue();

			if(pv) {
				CountMgr_->set_reverse_AR(pv->value());
			}
		}
	}

        return;
}

bool McUdp_DNS::HCGENE(QDCount)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	if(CountMgr_) {
		int32_t val = CountMgr_->get_generate_QD();
		PvNumber def(val);
		return(def.generate(cntr, wmem, buf));
	}

	return(false);
}

bool McUdp_DNS::HCGENE(ANCount)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	if(CountMgr_) {
		int32_t val = CountMgr_->get_generate_AN();
		PvNumber def(val);
		return(def.generate(cntr, wmem, buf));
	}

	return(false);
}

bool McUdp_DNS::HCGENE(NSCount)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	if(CountMgr_) {
		int32_t val = CountMgr_->get_generate_NS();
		PvNumber def(val);
		return(def.generate(cntr, wmem, buf));
	}

	return(false);
}

bool McUdp_DNS::HCGENE(ARCount)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	if(CountMgr_) {
		int32_t val = CountMgr_->get_generate_AR();
		PvNumber def(val);
		return(def.generate(cntr, wmem, buf));
	}

	return(false);
}

////////////////////////////////////////////////////////////////
PObject *McUdp_DNS::HCEVAL(QDCount)(WObject *wmem) const {
	int32_t val = 0;

	if(CountMgr_) {
		val = CountMgr_->get_generate_QD();
	}

        return(new PvNumber(val));
}

PObject *McUdp_DNS::HCEVAL(ANCount)(WObject *wmem) const {
	int32_t val = 0;

	if(CountMgr_) {
		val = CountMgr_->get_generate_AN();
	}

        return(new PvNumber(val));
}

PObject *McUdp_DNS::HCEVAL(NSCount)(WObject *wmem) const {
	int32_t val = 0;

	if(CountMgr_) {
		val = CountMgr_->get_generate_NS();
	}

        return(new PvNumber(val));
}

PObject *McUdp_DNS::HCEVAL(ARCount)(WObject *wmem) const {
	int32_t val = 0;

	if(CountMgr_) {
		val = CountMgr_->get_generate_AR();
	}

        return(new PvNumber(val));
}

////////////////////////////////////////////////////////////////
McDNS_Question::McDNS_Question(CSTR s): McOption(s), McUdp_DNS_(0) {}
McDNS_Question::~McDNS_Question() {}

uint32_t McDNS_Question::length_for_reverse(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(McUdp_DNS_) {
		CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

		if(countmgr) {
			uint32_t len = countmgr->get_length_QD();

			if(len) {
				countmgr->set_length_QD(0);
				return(len);
			}
		}
	}

	return(buf.remainLength(at.bytes()));
}

RObject *McDNS_Question::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	if(McUdp_DNS_) {
		CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

		if(countmgr) {
			ItPosition tmpat = at;

			RObject *r_self = McOption::reverse(c, 0, tmpat, buf);

			if(r_self) {
				uint32_t len = 0;
				RObject *r = r_self->nextChild();

				for(uint32_t d = 0; d < countmgr->get_reverse_QD(); d ++, r = r->nextSister()) {
					if(!r) {
						break;
					}

					len += (r->size()).bytes();
				}

				countmgr->set_length_QD(len);
			}
		}
	}

	return(McOption::reverse(c, r_parent, at, buf));
}

////////////////////////////////////////////////////////////////
McDNS_Answer::McDNS_Answer(CSTR s): McOption(s) {}
McDNS_Answer::~McDNS_Answer() {}

uint32_t McDNS_Answer::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const {
	if(McUdp_DNS_) {
		CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

		if(countmgr) {
			uint32_t len = countmgr->get_length_AN();

			if(len) {
				countmgr->set_length_AN(0);
				return(len);
			}
		}
	}

	return(buf.remainLength(at.bytes()));
}

RObject *McDNS_Answer::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	if(McUdp_DNS_) {
		CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

		if(countmgr) {
			ItPosition tmpat = at;

			RObject *r_self = McOption::reverse(c, 0, tmpat, buf);

			if(r_self) {
				uint32_t len = 0;
				RObject *r = r_self->nextChild();

				for(uint32_t d = 0; d < countmgr->get_reverse_AN(); d ++, r = r->nextSister()) {
					if(!r) {
						break;
					}

					len += (r->size()).bytes();
				}

				countmgr->set_length_AN(len);
			}
		}
	}

	return(McOption::reverse(c, r_parent, at, buf));
}

////////////////////////////////////////////////////////////////
McDNS_Authority::McDNS_Authority(CSTR s): McOption(s) {}
McDNS_Authority::~McDNS_Authority() {}

uint32_t McDNS_Authority::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const {
	if(McUdp_DNS_) {
		CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

		if(countmgr) {
			uint32_t len = countmgr->get_length_NS();

			if(len) {
				countmgr->set_length_NS(0);
				return(len);
			}
		}
	}

	return(buf.remainLength(at.bytes()));
}

RObject *McDNS_Authority::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	if(McUdp_DNS_) {
		CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

		if(countmgr) {
			ItPosition tmpat = at;

			RObject *r_self = McOption::reverse(c, 0, tmpat, buf);

			if(r_self) {
				uint32_t len = 0;
				RObject *r = r_self->nextChild();

				for(uint32_t d = 0; d < countmgr->get_reverse_NS(); d ++, r = r->nextSister()) {
					if(!r) {
						break;
					}

					len += (r->size()).bytes();
				}

				countmgr->set_length_NS(len);
			}
		}
	}

	return(McOption::reverse(c, r_parent, at, buf));
}

////////////////////////////////////////////////////////////////
McDNS_Additional::McDNS_Additional(CSTR s): McOption(s) {}
McDNS_Additional::~McDNS_Additional() {}

uint32_t McDNS_Additional::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const {
	if(McUdp_DNS_) {
		CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

		if(countmgr) {
			uint32_t len = countmgr->get_length_AR();

			if(len) {
				countmgr->set_length_AR(0);
				return(len);
			}
		}
	}

	return(buf.remainLength(at.bytes()));
}

RObject *McDNS_Additional::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	if(McUdp_DNS_) {
		CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

		if(countmgr) {
			ItPosition tmpat = at;

			RObject *r_self = McOption::reverse(c, 0, tmpat, buf);

			if(r_self) {
				uint32_t len = 0;
				RObject *r = r_self->nextChild();

				for(uint32_t d = 0; d < countmgr->get_reverse_AR(); d ++, r = r->nextSister()) {
					if(!r) {
						break;
					}

					len += (r->size()).bytes();
				}

				countmgr->set_length_AR(len);
			}
		}
	}

	return(McOption::reverse(c, r_parent, at, buf));
}

////////////////////////////////////////////////////////////////
McDNS_QuestionEntry::McDNS_QuestionEntry(CSTR s): McOption(s) {
	LengthCtl_ = new LengthCtl();
}

McDNS_QuestionEntry::~McDNS_QuestionEntry() {}

RObject *McDNS_QuestionEntry::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	if(LengthCtl_) {
		ItPosition tmpat = at;

		RObject *r_self = McOption::reverse(c, 0, tmpat, buf);

		if(r_self) {
			uint32_t len = 0;

			for(RObject *r = r_self->nextEvaluation(0); r != 0; r = r_self->nextEvaluation(r)) {
				len += (r->size()).bytes();
			}

			LengthCtl_->set_length(len);
		}
	}

	return(McOption::reverse(c, r_parent, at, buf));
}

uint32_t McDNS_QuestionEntry::length_for_reverse(RControl &c, ItPosition &at, OCTBUF &buf) const {
	uint32_t length = buf.remainLength(at.bytes());

	if(LengthCtl_) {
		uint32_t reverse_length = LengthCtl_->get_length();

		if(reverse_length) {
			length = reverse_length;
			LengthCtl_->set_length(0);
		}
	}

	return(length);
}

////////////////////////////////////////////////////////////////
McDNS_RR::McDNS_RR(CSTR key): McOption(key), type_(0), length_(0) {
	LengthCtl_ = new LengthCtl();
}

McDNS_RR::~McDNS_RR() {}

RObject *McDNS_RR::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	if(LengthCtl_) {
		ItPosition tmpat = at;

		RObject *r_self = McOption::reverse(c, 0, tmpat, buf);

		if(r_self) {
			RObject *rrlength = (RObject *)r_self->corresponding(length_);
			if(rrlength) {
				PvNumber *pv = (PvNumber *)rrlength->pvalue();

				if(pv) {
					LengthCtl_->set_rrlength(pv->value());
				}
			}

			uint32_t len = 0;

			for(RObject *r = r_self->nextEvaluation(0); r != 0; r = r_self->nextEvaluation(r)) {
				len += (r->size()).bytes();

				if(r == rrlength) {
					break;
				}
			}

			LengthCtl_->set_length(len);
		}
	}

	return(McOption::reverse(c, r_parent, at, buf));
}

uint32_t McDNS_RR::length_for_reverse(RControl &c, ItPosition &at, OCTBUF &buf) const {
	uint32_t length = buf.remainLength(at.bytes());

	if(LengthCtl_) {
		uint32_t base_length = LengthCtl_->get_length();
		uint32_t rdata_length = LengthCtl_->get_rrlength();

		if(base_length) {
			length = base_length;
			LengthCtl_->set_length(0);

			if(rdata_length) {
				length += rdata_length;
				LengthCtl_->set_rrlength(0);
			}
		}
	}

	return(length);
}

bool McDNS_RR::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	uint32_t limit = buf.remainLength(at.bytes());

	if(limit) {
		ItPosition tmpat = at;

		RObject *r_self = McOption::reverse(c, 0, tmpat, buf);
		RObject *rtype = (RObject *)r_self->corresponding(type_);

		if(!rtype) {
			return(false);
		}

		const PvNumber *pv = (const PvNumber *)rtype->pvalue();
		uint32_t typevalue = pv->value();

		c.DictType().type_Set(typevalue);
		delete rtype;

		return(true);
	}

	return(false);
}

bool McDNS_RR::HCGENE(Type)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	int32_t val = get_rrType(wmem);
	if(val == -1) {
		return(false);
	}

	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}

PObject *McDNS_RR::HCEVAL(Type)(WObject *wmem) const {
	int32_t val = get_rrType(wmem);
	return(new PvNumber(val));
}

int32_t McDNS_RR::get_rrType(WObject *wmem) const {
	WObject *wc = wmem->parent();
	int32_t rtn = wc ? wc->meta()->rrType() : -1;

	if(rtn == -1) {
		wmem->mustDefine(0);
	}

	return(rtn);
}

bool McDNS_RR::HCGENE(Length)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	uint32_t len = 0;
	WObject *wc = wmem->parent();

	if(wc) {
		uint32_t parentsize = wc->size().bytes();
		uint32_t thisdistance = wmem->distance();
		uint32_t thissize = wmem->size().bytes();
		len = parentsize - thisdistance - thissize;
	}

	PvNumber def(len);

	return(def.generate(cntr, wmem, buf));
}

////////////////////////////////////////////////////////////////
McDNS_RR_ANY::McDNS_RR_ANY(CSTR key): McDNS_RR(key) {}
McDNS_RR_ANY::~McDNS_RR_ANY() {}

////////////////////////////////////////////////////////////////
McDNS_RR_A::McDNS_RR_A(CSTR key): McDNS_RR(key) {}
McDNS_RR_A::~McDNS_RR_A() {}

////////////////////////////////////////////////////////////////
McDNS_RR_NS::McDNS_RR_NS(CSTR key): McDNS_RR(key) {}
McDNS_RR_NS::~McDNS_RR_NS() {}

////////////////////////////////////////////////////////////////
McDNS_RR_MD::McDNS_RR_MD(CSTR key): McDNS_RR(key) {}
McDNS_RR_MD::~McDNS_RR_MD() {}

////////////////////////////////////////////////////////////////
McDNS_RR_MF::McDNS_RR_MF(CSTR key): McDNS_RR(key) {}
McDNS_RR_MF::~McDNS_RR_MF() {}

////////////////////////////////////////////////////////////////
McDNS_RR_CNAME::McDNS_RR_CNAME(CSTR key): McDNS_RR(key) {}
McDNS_RR_CNAME::~McDNS_RR_CNAME() {}

////////////////////////////////////////////////////////////////
McDNS_RR_SOA::McDNS_RR_SOA(CSTR key): McDNS_RR(key) {}
McDNS_RR_SOA::~McDNS_RR_SOA() {}

////////////////////////////////////////////////////////////////
McDNS_RR_MB::McDNS_RR_MB(CSTR key): McDNS_RR(key) {}
McDNS_RR_MB::~McDNS_RR_MB() {}

////////////////////////////////////////////////////////////////
McDNS_RR_MG::McDNS_RR_MG(CSTR key): McDNS_RR(key) {}
McDNS_RR_MG::~McDNS_RR_MG() {}

////////////////////////////////////////////////////////////////
McDNS_RR_MR::McDNS_RR_MR(CSTR key): McDNS_RR(key) {}
McDNS_RR_MR::~McDNS_RR_MR() {}

////////////////////////////////////////////////////////////////
McDNS_RR_NULL::McDNS_RR_NULL(CSTR key): McDNS_RR(key) {}
McDNS_RR_NULL::~McDNS_RR_NULL() {}

////////////////////////////////////////////////////////////////
McDNS_RR_WKS::McDNS_RR_WKS(CSTR key): McDNS_RR(key) {}
McDNS_RR_WKS::~McDNS_RR_WKS() {}

////////////////////////////////////////////////////////////////
McDNS_RR_PTR::McDNS_RR_PTR(CSTR key): McDNS_RR(key) {}
McDNS_RR_PTR::~McDNS_RR_PTR() {}

////////////////////////////////////////////////////////////////
McDNS_RR_HINFO::McDNS_RR_HINFO(CSTR key): McDNS_RR(key) {}
McDNS_RR_HINFO::~McDNS_RR_HINFO() {}

////////////////////////////////////////////////////////////////
McDNS_RR_MINFO::McDNS_RR_MINFO(CSTR key): McDNS_RR(key) {}
McDNS_RR_MINFO::~McDNS_RR_MINFO() {}

////////////////////////////////////////////////////////////////
McDNS_RR_MX::McDNS_RR_MX(CSTR key): McDNS_RR(key) {}
McDNS_RR_MX::~McDNS_RR_MX() {}

////////////////////////////////////////////////////////////////
#define RDLENGTHOFFSET	8
McDNS_RR_TXT::McDNS_RR_TXT(CSTR key): McDNS_RR(key) {}
McDNS_RR_TXT::~McDNS_RR_TXT() {}

uint32_t McDNS_RR_TXT::HC_MLC(TXT)(const ItPosition &at, OCTBUF &buf) const {
	unsigned char *str = (unsigned char *)buf.buffer() + at.bytes();
	uint32_t length = buf.remainLength(at.bytes());

	uint32_t offset = 0;
	for(uint32_t d = 0; d < length; d += (str[d] & 0xff), d ++) {
		offset ++;

		if((str[d] & 0xff) == 0x00) {
			break;
		}

		if((str[d] & 0xff) & 0xc0) {
			offset ++;
			break;
		}

		offset += (str[d] & 0xff);
	}

	offset += RDLENGTHOFFSET;
	uint32_t rdlength = 0x100 * (str[offset] & 0xff) + (str[offset + 1] & 0xff);
	offset += 2;

	uint32_t count = 0;
	str += offset;
	for(uint32_t d = 0; d < rdlength; d += (str[d] & 0xff), d ++) {
		count ++;
	}

	return(count);
}
#undef RDLENGTHOFFSET

////////////////////////////////////////////////////////////////
McDNS_RR_RP::McDNS_RR_RP(CSTR key): McDNS_RR(key) {}
McDNS_RR_RP::~McDNS_RR_RP() {}

////////////////////////////////////////////////////////////////
McDNS_RR_AFSDB::McDNS_RR_AFSDB(CSTR key): McDNS_RR(key) {}
McDNS_RR_AFSDB::~McDNS_RR_AFSDB() {}

////////////////////////////////////////////////////////////////
McDNS_RR_X25::McDNS_RR_X25(CSTR key): McDNS_RR(key) {}
McDNS_RR_X25::~McDNS_RR_X25() {}

////////////////////////////////////////////////////////////////
McDNS_RR_ISDN::McDNS_RR_ISDN(CSTR key): McDNS_RR(key) {}
McDNS_RR_ISDN::~McDNS_RR_ISDN() {}

////////////////////////////////////////////////////////////////
McDNS_RR_RT::McDNS_RR_RT(CSTR key): McDNS_RR(key) {}
McDNS_RR_RT::~McDNS_RR_RT() {}

////////////////////////////////////////////////////////////////
McDNS_RR_PX::McDNS_RR_PX(CSTR key): McDNS_RR(key) {}
McDNS_RR_PX::~McDNS_RR_PX() {}

////////////////////////////////////////////////////////////////
McDNS_RR_AAAA::McDNS_RR_AAAA(CSTR key): McDNS_RR(key) {}
McDNS_RR_AAAA::~McDNS_RR_AAAA() {}

////////////////////////////////////////////////////////////////
McDNS_RR_SRV::McDNS_RR_SRV(CSTR key): McDNS_RR(key) {}
McDNS_RR_SRV::~McDNS_RR_SRV() {}

////////////////////////////////////////////////////////////////
McDNS_RR_NAPTR::McDNS_RR_NAPTR(CSTR key): McDNS_RR(key) {}
McDNS_RR_NAPTR::~McDNS_RR_NAPTR() {}

////////////////////////////////////////////////////////////////
MmQuestion_onDNS::MmQuestion_onDNS(CSTR key, McUdp_DNS *dnshdr): MmReference_Less1(key, true), McUdp_DNS_(dnshdr) {}
MmQuestion_onDNS::~MmQuestion_onDNS() {}

TypevsMcDict MmQuestion_onDNS::dict_;

void MmQuestion_onDNS::add_other(McDNS_Question *mc) {
	dict_.add_other(mc);
}

bool MmQuestion_onDNS::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(!McUdp_DNS_) {
		return(false);
	}

	CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

	if(!countmgr) {
		return(false);
	}

	if(buf.remainLength(at.bytes()) && (countmgr->get_reverse_QD() > 0)) {
		c.DictType().other_Set();
		return(true);
	}

	return(false);
}

////////////////////////////////////////////////////////////////
MmAnswer_onDNS::MmAnswer_onDNS(CSTR key, McUdp_DNS *dnshdr): MmReference_Less1(key, true), McUdp_DNS_(dnshdr) {}
MmAnswer_onDNS::~MmAnswer_onDNS() {}

TypevsMcDict MmAnswer_onDNS::dict_;

void MmAnswer_onDNS::add_other(McDNS_Answer *mc) {
	dict_.add_other(mc);
}

bool MmAnswer_onDNS::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(!McUdp_DNS_) {
		return(false);
	}

	CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

	if(!countmgr) {
		return(false);
	}

	if(buf.remainLength(at.bytes()) && (countmgr->get_reverse_AN() > 0)) {
		c.DictType().other_Set();
		return(true);
	}

	return(false);
}

////////////////////////////////////////////////////////////////
MmAuthority_onDNS::MmAuthority_onDNS(CSTR key, McUdp_DNS *dnshdr): MmReference_Less1(key, true), McUdp_DNS_(dnshdr) {}
MmAuthority_onDNS::~MmAuthority_onDNS() {}

TypevsMcDict MmAuthority_onDNS::dict_;

void MmAuthority_onDNS::add_other(McDNS_Authority *mc) {
	dict_.add_other(mc);
}

bool MmAuthority_onDNS::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(!McUdp_DNS_) {
		return(false);
	}

	CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

	if(!countmgr) {
		return(false);
	}

	if(buf.remainLength(at.bytes()) && (countmgr->get_reverse_NS() > 0)) {
		c.DictType().other_Set();
		return(true);
	}

	return(false);
}

////////////////////////////////////////////////////////////////
MmAdditional_onDNS::MmAdditional_onDNS(CSTR key, McUdp_DNS *dnshdr): MmReference_Less1(key, true), McUdp_DNS_(dnshdr) {}
MmAdditional_onDNS::~MmAdditional_onDNS() {}

TypevsMcDict MmAdditional_onDNS::dict_;

void MmAdditional_onDNS::add_other(McDNS_Additional *mc) {
	dict_.add_other(mc);
}

bool MmAdditional_onDNS::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(!McUdp_DNS_) {
		return(false);
	}

	CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

	if(!countmgr) {
		return(false);
	}

	if(buf.remainLength(at.bytes()) && (countmgr->get_reverse_AR() > 0)) {
		c.DictType().other_Set();
		return(true);
	}

	return(false);
}

////////////////////////////////////////////////////////////////
MmEntry_onQuestion::MmEntry_onQuestion(CSTR key, McUdp_DNS *mc): MmReference_More0(key), McUdp_DNS_(mc) {}
MmEntry_onQuestion::~MmEntry_onQuestion() {}

void MmEntry_onQuestion::add_other(McDNS_QuestionEntry *mc) {
	dict_.add_other(mc);
}

TypevsMcDict MmEntry_onQuestion::dict_;

void MmEntry_onQuestion::composeList(WControl &c, WObject *w_parent, const PObjectList &pls) const {
	const PObject *pl = pls.reverseMatching(this, (PObjectEqFunc)&PObject::isEqualMeta);

	if(pl) {
		pls.elementsPerformWith((PObjectFunc)&PObject::vmatchselfCompose, &c, w_parent, this);
	} else {
		compose(c, w_parent, 0);
	}
}

bool MmEntry_onQuestion::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(!McUdp_DNS_) {
		return(false);
	}

	CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

	if(!countmgr) {
		return(false);
	}

	if(buf.remainLength(at.bytes()) && (countmgr->get_current_QD() < countmgr->get_reverse_QD())) {
		c.DictType().other_Set();
		countmgr->increase_current_QD();
		return(true);
	}

	countmgr->init_current_QD();

	return(false);
}

WObject *MmEntry_onQuestion::compose(WControl &c, WObject *w_parent, const PObject *pl) const {
	if(!pl) {
		w_parent->mustDefineMem(this);
		return(0);
	}

	const PObject *right = pl->rvalue();
	if(!right) {
		w_parent->mustDefineMem(this);
		return(0);
	}

	const TypevsMcDict *keep = c.dict();
	c.dict_set(get_dict());

	WObject *wc = right->selfCompose(c, w_parent);
	c.dict_set(keep);

	CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

	if(countmgr) {
		countmgr->increase_generate_QD();
	}

        return(wc);
}

////////////////////////////////////////////////////////////////
MmRR_onAnswer::MmRR_onAnswer(CSTR key, McUdp_DNS *mc): MmReference_More0(key), McUdp_DNS_(mc) {}
MmRR_onAnswer::~MmRR_onAnswer() {}

void MmRR_onAnswer::add(McDNS_RR *mc) {
	dict_.add(mc->rrType(), mc);
}

void MmRR_onAnswer::add_other(McDNS_RR *mc) {
	dict_.add_other(mc);
}

TypevsMcDict MmRR_onAnswer::dict_;

void MmRR_onAnswer::composeList(WControl &c, WObject *w_parent, const PObjectList &pls) const {
	const PObject *pl = pls.reverseMatching(this, (PObjectEqFunc)&PObject::isEqualMeta);

	if(pl) {
		pls.elementsPerformWith((PObjectFunc)&PObject::vmatchselfCompose, &c, w_parent, this);
	} else {
		compose(c, w_parent, 0);
	}
}

bool MmRR_onAnswer::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(!McUdp_DNS_) {
		return(false);
	}

	CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

	if(!countmgr) {
		return(false);
	}

	if(buf.remainLength(at.bytes()) && (countmgr->get_current_AN() < countmgr->get_reverse_AN())) {
		McDNS_RR *any = (McDNS_RR *)dict_.find_other();
		countmgr->increase_current_AN();

		return(any->overwrite_DictType(c, at, buf));
	}

	countmgr->init_current_AN();

	return(false);
}

WObject *MmRR_onAnswer::compose(WControl &c, WObject *w_parent, const PObject *pl) const {
	if(!pl) {
		w_parent->mustDefineMem(this);
		return(0);
	}

	const PObject *right = pl->rvalue();
	if(!right) {
		w_parent->mustDefineMem(this);
		return(0);
	}

	const TypevsMcDict *keep = c.dict();
	c.dict_set(get_dict());

	WObject *wc = right->selfCompose(c, w_parent);
	c.dict_set(keep);

	CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

	if(countmgr) {
		countmgr->increase_generate_AN();
	}

        return(wc);
}

////////////////////////////////////////////////////////////////
MmRR_onAuthority::MmRR_onAuthority(CSTR key, McUdp_DNS *mc): MmReference_More0(key), McUdp_DNS_(mc) {}
MmRR_onAuthority::~MmRR_onAuthority() {}

void MmRR_onAuthority::add(McDNS_RR *mc) {
	dict_.add(mc->rrType(), mc);
}

void MmRR_onAuthority::add_other(McDNS_RR *mc) {
	dict_.add_other(mc);
}

TypevsMcDict MmRR_onAuthority::dict_;

void MmRR_onAuthority::composeList(WControl &c, WObject *w_parent, const PObjectList &pls) const {
	const PObject *pl = pls.reverseMatching(this, (PObjectEqFunc)&PObject::isEqualMeta);

	if(pl) {
		pls.elementsPerformWith((PObjectFunc)&PObject::vmatchselfCompose, &c, w_parent, this);
	} else {
		compose(c, w_parent, 0);
	}
}

bool MmRR_onAuthority::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(!McUdp_DNS_) {
		return(false);
	}

	CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

	if(!countmgr) {
		return(false);
	}

	if(buf.remainLength(at.bytes()) && (countmgr->get_current_NS() < countmgr->get_reverse_NS())) {
		McDNS_RR *any = (McDNS_RR *)dict_.find_other();
		countmgr->increase_current_NS();

		return(any->overwrite_DictType(c, at, buf));
	}

	countmgr->init_current_NS();

	return(false);
}

WObject *MmRR_onAuthority::compose(WControl &c, WObject *w_parent, const PObject *pl) const {
	if(!pl) {
		w_parent->mustDefineMem(this);
		return(0);
	}

	const PObject *right = pl->rvalue();
	if(!right) {
		w_parent->mustDefineMem(this);
		return(0);
	}

	const TypevsMcDict *keep = c.dict();
	c.dict_set(get_dict());

	WObject *wc = right->selfCompose(c, w_parent);
	c.dict_set(keep);

	CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

	if(countmgr) {
		countmgr->increase_generate_NS();
	}

        return(wc);
}

////////////////////////////////////////////////////////////////
MmRR_onAdditional::MmRR_onAdditional(CSTR key, McUdp_DNS *mc): MmReference_More0(key), McUdp_DNS_(mc) {}
MmRR_onAdditional::~MmRR_onAdditional() {}

void MmRR_onAdditional::add(McDNS_RR *mc) {
	dict_.add(mc->rrType(), mc);
}

void MmRR_onAdditional::add_other(McDNS_RR *mc) {
	dict_.add_other(mc);
}

TypevsMcDict MmRR_onAdditional::dict_;

void MmRR_onAdditional::composeList(WControl &c, WObject *w_parent, const PObjectList &pls) const {
	const PObject *pl = pls.reverseMatching(this, (PObjectEqFunc)&PObject::isEqualMeta);

	if(pl) {
		pls.elementsPerformWith((PObjectFunc)&PObject::vmatchselfCompose, &c, w_parent, this);
	} else {
		compose(c, w_parent, 0);
	}
}

bool MmRR_onAdditional::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(!McUdp_DNS_) {
		return(false);
	}

	CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

	if(!countmgr) {
		return(false);
	}

	if(buf.remainLength(at.bytes()) && (countmgr->get_current_AR() < countmgr->get_reverse_AR())) {
		McDNS_RR *any = (McDNS_RR *)dict_.find_other();
		countmgr->increase_current_AR();

		return(any->overwrite_DictType(c, at, buf));
	}

	countmgr->init_current_AR();

	return(false);
}

WObject *MmRR_onAdditional::compose(WControl &c, WObject *w_parent, const PObject *pl) const {
	if(!pl) {
		w_parent->mustDefineMem(this);
		return(0);
	}

	const PObject *right = pl->rvalue();
	if(!right) {
		w_parent->mustDefineMem(this);
		return(0);
	}

	const TypevsMcDict *keep = c.dict();
	c.dict_set(get_dict());

	WObject *wc = right->selfCompose(c, w_parent);
	c.dict_set(keep);

	CountMgr *countmgr = McUdp_DNS_->get_CountMgr();

	if(countmgr) {
		countmgr->increase_generate_AR();
	}

        return(wc);
}

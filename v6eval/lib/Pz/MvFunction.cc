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
 * $TAHI: v6eval/lib/Pz/MvFunction.cc,v 1.54 2006/01/18 05:09:59 akisada Exp $
 */
#include "MvFunction.h"
#include "PvObject.h"
#include "PvOctets.h"
#include "PvAction.h"
#include "WObject.h"
#include "PvIfName.h"
#include "PControl.h"
#include "McMobility.h"
#include "McDHCPv6.h"
#include "McDNS.h"
#include "McSIP.h"
#include "McSNMP.h"
#include "McIKE.h"
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>

#include <unistd.h>
#include "debug.h"

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
}



MvOperator::MvOperator(CSTR s,int32_t t,int32_t o):MvKeyword(s,t),operation_(o) {}
MvOperator::~MvOperator() {}
PObject* MvOperator::tokenObject(int l,CSTR f) const {
	return new PvOperator(this,f,l);}

//======================================================================
MvAction::MvAction(CSTR s,int32_t t):MvKeyword(s,t) {}
MvAction::~MvAction(){}
WObject* MvAction::compose(WControl&,WObject* w_parent,const PObject* po) const {
	return new WmObject(w_parent,this,po);}

//----------------------------------------------------------------------
MvANY::MvANY(CSTR s,int32_t t):MvAction(s,t) {}
MvANY::~MvANY(){}
PObject* MvANY::tokenObject(int l,CSTR f) const {
	return new PvANY(this,f,l);}
WObject* MvANY::compose(WControl& c,WObject* w_parent,const PObject* po) const {
	return MvAction::compose(c,w_parent,po);
}

//----------------------------------------------------------------------
MvSTOP::MvSTOP(CSTR s,int32_t t):MvAction(s,t) {}
MvSTOP::~MvSTOP(){}
PObject* MvSTOP::tokenObject(int l,CSTR f) const {
	return new PvSTOP(this,f,l);}
WObject* MvSTOP::compose(WControl& c,WObject* w_parent,const PObject* po) const {
	return MvAction::compose(c,w_parent,po);
}

//----------------------------------------------------------------------
MvAUTO::MvAUTO(CSTR s,int32_t t):MvAction(s,t) {}
MvAUTO::~MvAUTO(){}
PObject* MvAUTO::tokenObject(int l,CSTR f) const {
	return new PvAUTO(this,f,l);}

//----------------------------------------------------------------------
MvFILL::MvFILL(CSTR s,int32_t t):MvAction(s,t) {}
MvFILL::~MvFILL(){}
PObject* MvFILL::tokenObject(int l,CSTR f) const {
	return new PvFILL(this,f,l);}

//----------------------------------------------------------------------
MvZERO::MvZERO(CSTR s,int32_t t):MvAction(s,t) {}
MvZERO::~MvZERO(){}
PObject* MvZERO::tokenObject(int l,CSTR f) const {
	return new PvZERO(this,f,l);}

//======================================================================
MvFunction::MvFunction(CSTR s):MObject(s) {}
MvFunction::~MvFunction() {}
int32_t MvFunction::token() const {return metaToken(tkn_function_);}
PObject* MvFunction::tokenObject(int l,CSTR f) const {
	return new PvFunction(this,f,l);}

bool MvFunction::functionGenerate(WControl& c,WObject* w,OCTBUF&,const PObjectList&) const {
	const PObject* s=w->object();
	s->generateNotAllow(string());
	c.result(1);
	return c;}

uint32_t MvFunction::objectLength(const PObject* o,const WObject* w) const {
	return o!=0?o->objectLength(w):0;}

PvObject* MvFunction::generateValue(WObject*,const PObjectList&) const {
	return 0;}

PvObject* MvFunction::evaluateValue(WObject*,const PObjectList&) const {
	return 0;}

bool MvFunction::generateV6Addr(const PObjectList&,PvV6Addr& into) const {
	into.zero();
	return false;}
bool MvFunction::evaluateV6Addr(const PObjectList&,PvV6Addr& into) const {
	into.zero();
	return false;}

bool MvFunction::generateOctetsWith(const PObjectList&,PvOctets& into,WObject*) const {
	into.zero();
	return false;}
int32_t MvFunction::compareObject(const PObject& r,const PObjectList& a) const {
	PvObject* eval=evaluateValue(0,a);
	int32_t rc=eval!=0?eval->compareObject(r):-1;
	delete eval;
	return rc;}

// COMPOSE
WObject* MvFunction::compose(WControl& c,
		WObject* w_parent,const PObject* pv) const {
	WObject* w_self = composeWv(c,w_parent,pv);
	const PObjectList& pas = pv->args();
	args_compose(c,w_self,pas);
	return w_self;}

WObject* MvFunction::composeWv(WControl&,
		WObject* w_parent,const PObject* pv) const {
	return new WvObject(w_parent,this,pv);}

void MvFunction::args_compose(WControl&,WObject*,const PObjectList&)const{}
	
//======================================================================
MvWithin::MvWithin(CSTR s):MvFunction(s) {}
MvWithin::~MvWithin() {}

int32_t MvWithin::compareObject(const PObject& r,const PObjectList& a) const {
	int32_t cl=r.compareObject(*a[0]);
	int32_t cr=r.compareObject(*a[1]);
	return (cl>=0&&cr<=0)?0:-1;}

//======================================================================
MvOneof::MvOneof(CSTR s):MvFunction(s) {}
MvOneof::~MvOneof() {}

int32_t MvOneof::compareObject(const PObject& r,const PObjectList& a) const {
	PObject* f=a.findMatching(&r,(PObjectEqFunc)&PObject::isEqualObject);
	return f!=0?0:-1;}

// COMPOSE
WObject* MvOneof::composeWv(WControl&,
		WObject* w_parent,const PObject* pv) const {
	return new WvOneof(w_parent,this,pv);}
void MvOneof::args_compose(WControl& c,
		WObject* w_self,const PObjectList& pas) const{
	pas.elementsPerformWith((PObjectFunc)&PObject::vselfCompose,&c,w_self);}

//======================================================================
MvComb::MvComb(CSTR s):MvFunction(s) {}
MvComb::~MvComb() {}

// COMPOSE
WObject* MvComb::composeWv(WControl&,
		WObject* w_parent,const PObject* pv) const {
	return new WvComb(w_parent,this,pv);}
void MvComb::args_compose(WControl& c,
		WObject* w_self,const PObjectList& pas) const{
	pas.elementsPerformWith((PObjectFunc)&PObject::vselfCompose,&c,w_self);}

//======================================================================
MvOctets::MvOctets(CSTR s):MvFunction(s) {}
MvOctets::~MvOctets() {}
bool MvOctets::functionGenerate(WControl& c,WObject* w,OCTBUF& b,const PObjectList& a) const {
	PvOctets tmp;
	if(!generateOctetsWith(a,tmp,0)) {
		const PObject* s=w->object();
		s->error("E %s generate error",string());
		c.result(1); return c;}
	return tmp.generate(c,w,b);}

PvObject* MvOctets::generateValue(WObject*,const PObjectList& a) const {
	PvOctets* tmp=new PvOctets;
	if(!generateOctetsWith(a,*tmp,0)) {delete tmp; return 0;}
	return tmp;}

PvObject* MvOctets::evaluateValue(WObject*,const PObjectList& a) const {
	PvOctets* tmp=new PvOctets;
	if(!generateOctetsWith(a,*tmp,0)) {delete tmp; return 0;}
	return tmp;}

//======================================================================
MvRepeat::MvRepeat(CSTR s):MvOctets(s) {}
MvRepeat::~MvRepeat() {}
uint32_t MvRepeat::functionLength(const PObjectList& a,const WObject*) const {
	bool ok=true;
	return a[1]->intValue(ok);}

bool MvRepeat::generateOctetsWith(const PObjectList& a,PvOctets& oct,WObject*) const {
	bool ok=true;
	uint32_t l=a[1]->intValue(ok);
	int v=a[0]->intValue(ok);
	oct.set(l);
	memset(oct.buffer(),v,l);
	return true;}

//======================================================================
MvHexStr::MvHexStr(CSTR s):MvOctets(s) {}
MvHexStr::~MvHexStr() {}

uint32_t MvHexStr::functionLength(const PObjectList &a, const WObject *) const {
	uint32_t len = 0;
	uint32_t n = a.size();
	bool ok = true;

	if(n < 2) {
		len = a[0]->length();
		len += 1;	/* len = ((len + 1) / 2 */
		len /= 2;
	} else {
		len = a[1]->intValue(ok);
	}

	return(len);
}

bool MvHexStr::generateOctetsWith(const PObjectList &a, PvOctets &oct, WObject *) const {
	bool ok = true, reverse = false;
	CSTR work = 0;
	uint32_t buflen = a[0]->length(), len = 0;
	OCTSTR buf = 0;
	uint32_t n = a.size();

	if(n < 2) {
		len = buflen;
		len += 1;	/* len = ((len + 1) / 2 */
		len /= 2;
	} else {
		len = a[1]->intValue(ok);
	}

	if((n > 2) && (a[2]->intValue(ok) != 0)) {
		reverse = true;
	}

	work = a[0]->strValue(ok);

	oct.set(len);
	buf = oct.buffer();

	return(hex_pton(buf, len, work, buflen, reverse));
}

bool MvHexStr::hex_pton(OCTSTR dst, uint32_t dstlen, CSTR src, uint32_t srclen, bool reverse) const {
	bool upper = true;
	const char xdigits_l[] = "0123456789abcdef", xdigits_u[] = "0123456789ABCDEF", *xdigits = 0;
	uint32_t x = 0, y = 0;

	memset(dst, 0, dstlen);

	if(reverse) {
		upper = false;

		for(x = srclen, y = dstlen; ((x > 0) && (y > 0)); x --) {
			const char *cptr = 0;

			if((cptr = strchr((xdigits = xdigits_l), src[x - 1])) == 0) {
				if((cptr = strchr((xdigits = xdigits_u), src[x - 1])) == 0) {
					return false;
				}
			}

			if(upper) {
				dst[y - 1] |= (cptr - xdigits) << 4;
				upper = false;
				y --;
			} else {
				dst[y - 1] = (cptr - xdigits);
				upper = true;
			}
		}
	} else {
		for(x = 0, y = 0; ((x < srclen) && (y < dstlen)); x ++) {
			const char *cptr = 0;

			if((cptr = strchr((xdigits = xdigits_l), src[x])) == 0) {
				if((cptr = strchr((xdigits = xdigits_u), src[x])) == 0) {
					return false;
				}
			}

			if(upper) {
				dst[y] = (cptr - xdigits) << 4;
				upper = false;
			} else {
				dst[y] |= (cptr - xdigits);
				upper = true;
				y ++;
			}
		}
	}

	return true;
}

bool MvHexStr::isHexStr(CSTR buf, uint32_t buflen) const {
	uint32_t d = 0;

	for(d = 0; d < buflen; d ++) {
		if((buf[d] < '0') || (buf[d] > '9') &&
		   (buf[d] < 'A') || (buf[d] > 'F') &&
		   (buf[d] < 'a') || (buf[d] > 'f')) {
			return false;
		}
	}

	return true;
}

//======================================================================
MvInt2Hex::MvInt2Hex(CSTR s): MvOctets(s) {}
MvInt2Hex::~MvInt2Hex() {}

uint32_t
MvInt2Hex::functionLength(const PObjectList &a, const WObject *) const
{
	bool ok		= true;
	uint32_t val	= a[0]->intValue(ok);
	uint32_t len	= 0;
	uint32_t n	= a.size();

	if(n < 2) {
		uint32_t tmp = val;
		for( ; tmp; tmp /= 0x100, len ++) {}
	} else {
		len = a[1]->intValue(ok);
	}

	return(len);
}



bool
MvInt2Hex::generateOctetsWith(const PObjectList &a,
	PvOctets &oct, WObject *) const
{
	bool ok		= true;
	uint32_t len	= functionLength(a, 0);
	uint32_t val	= a[0]->intValue(ok);

	OCTSTR buf = 0;

	oct.set(len);
	buf = oct.buffer();

	memset(buf, 0, len);

	for( ; val; ) {
		len --;
		uint32_t remainder = val % 0x100;
		val /= 0x100;

		buf[len] = remainder;
	}

	return(true);
}



//======================================================================
MvFile::MvFile(CSTR s): MvOctets(s) {}
MvFile::~MvFile() {}

uint32_t MvFile::functionLength(const PObjectList &a, const WObject *) const {
	uint32_t len = 0;
	uint32_t n = a.size();
	bool ok = true;

	CSTR a0 = a[0]->strValue(ok);

	struct stat st;
	memset(&st, 0, sizeof(struct stat));

	if(stat(a0, &st) < 0) {
		return(len);
	}

	len = st.st_size;

	if(n > 1) {
		uint32_t offset = a[1]->intValue(ok);
		len -= offset;
	}

	if(n > 2) {
		len = a[2]->intValue(ok);
	}

	return(len);
}

bool MvFile::generateOctetsWith(const PObjectList &a, PvOctets &oct, WObject *) const {
	bool ok = true;

	uint32_t n = a.size();

	CSTR a0 = a[0]->strValue(ok);

	struct stat st;
	memset(&st, 0, sizeof(struct stat));

	if(stat(a0, &st) < 0) {
		return(false);
	}

	uint32_t length = st.st_size;

	if(n > 2) {
		length = a[2]->intValue(ok);
	}

	oct.set(length);

	int fd = 0;

	if((fd = open(a0, O_RDONLY)) < 0) {
		return(false);
	}

	uint32_t offset = 0;

	if(n > 1) {
		offset = a[1]->intValue(ok);
	}

	if(lseek(fd, offset, SEEK_SET) < 0) {
		return(false);
	}

	if(read(fd, oct.buffer(), length) < 0) {
		return(false);
	}

	close(fd);

	return(true);
}

//======================================================================
MvSipTxt::MvSipTxt(CSTR s): MvFile(s) {}
MvSipTxt::~MvSipTxt() {}

PvObject *MvSipTxt::generateValue(WObject *, const PObjectList &a) const {
	PvSIPMsg *tmp = new PvSIPMsg();

	if(!generateOctetsWith(a, *tmp, 0)) {
		delete(tmp);
		tmp = 0;
		return(0);
	}

	return(tmp);
}

PvObject *MvSipTxt::evaluateValue(WObject *, const PObjectList &a) const {
	PvSIPMsg *tmp = new PvSIPMsg();

	if(!generateOctetsWith(a, *tmp, 0)) {
		delete(tmp);
		tmp = 0;
		return(0);
	}

	return(tmp);
}

//======================================================================
MvAscii::MvAscii(CSTR s): MvOctets(s) {}
MvAscii::~MvAscii() {}

uint32_t
MvAscii::functionLength(const PObjectList &a, const WObject *) const
{
	uint32_t len = 0;
	uint32_t n = a.size();
	bool ok=true;

	if(n < 2) {
		len = a[0]->length();
	} else {
		len = a[1]->intValue(ok);
	}

	return(len);
}

bool
MvAscii::generateOctetsWith(const PObjectList &a,
	PvOctets &oct, WObject *) const
{
	uint32_t len = 0;
	uint32_t n = a.size();
	bool ok = true;
	CSTR a0 = a[0]->strValue(ok);
	uint32_t a0len = a[0]->length();

	if(n < 2) {
		len = a0len;
	} else {
		len = a[1]->intValue(ok);
	}

	oct.set(len);

	memset(oct.buffer(), 0, len);

	if(len < a0len) {
		memcpy(oct.buffer(), a0, len);
	} else {
		memcpy(oct.buffer(), a0, a0len);
	}

	return(true);
}

PvObject *
MvAscii::generateValue(WObject *, const PObjectList &a) const
{
	PvOctets *tmp = new PvAsciiString;

	if(!generateOctetsWith(a, *tmp, 0)) {
		delete tmp;
		tmp = 0;
		return(0);
	}

	return(tmp);
}

PvObject *
MvAscii::evaluateValue(WObject *, const PObjectList &a) const
{
	PvOctets *tmp = new PvAsciiString;

	if(!generateOctetsWith(a,*tmp,0)) {
		delete tmp;
		tmp = 0;
		return(0);
	}

	return(tmp);
}

//======================================================================
MvDnsStr::MvDnsStr(CSTR s): MvOctets(s) {}
MvDnsStr::~MvDnsStr() {}

uint32_t MvDnsStr::functionLength(const PObjectList &a, const WObject *) const {
	return((a[0]->length()) + 1);
}

bool MvDnsStr::generateOctetsWith(const PObjectList &a, PvOctets &oct, WObject *) const {
	bool ok = true;
	CSTR str = a[0]->strValue(ok);
	uint32_t len = a[0]->length();

	oct.set(len + 1);

	unsigned char *dst = (unsigned char *)oct.buffer();

	dst[0] = len;

	memcpy(dst + 1, str, len);

	return(true);
}

PvObject *MvDnsStr::generateValue(WObject *, const PObjectList &a) const {
	PvDnsStr *tmp = new PvDnsStr();

	if(!generateOctetsWith(a, *tmp, 0)) {
		delete tmp;
		tmp = 0;
		return(0);
	}

	return(tmp);
}

PvObject *MvDnsStr::evaluateValue(WObject *, const PObjectList &a) const {
	PvDnsStr *tmp = new PvDnsStr();

	if(!generateOctetsWith(a, *tmp, 0)) {
		delete tmp;
		tmp = 0;
		return(0);
	}

	return(tmp);
}

//======================================================================
MvDnsName::MvDnsName(CSTR s): MvOctets(s) {}
MvDnsName::~MvDnsName() {}

uint32_t MvDnsName::functionLength(const PObjectList &a, const WObject *) const {
	bool ok = true;
	unsigned char *str = (unsigned char *)a[0]->strValue(ok);
	uint32_t len = a[0]->length();
	uint32_t n = a.size();

	if(len > 0) {
		if(str[len - 1] == '.') {
			len --;		// remove last dot
		}

		if(len > 0) {
			len ++;		// add first length field
		}
	}

	if(n < 2) {
		len ++;		// add last NULL
	} else {
		len += 2;	// add offset
	}

	return(len);
}

bool MvDnsName::generateOctetsWith(const PObjectList &a, PvOctets &oct, WObject *) const {
	bool ok = true;
	unsigned char *str = (unsigned char *)a[0]->strValue(ok);
	uint32_t len = a[0]->length();
	uint32_t funclen = len;
	uint32_t n = a.size();

	if(len > 0) {
		if(str[len - 1] == '.') {
			funclen --;
		}

		if(funclen > 0) {
			funclen ++;
		}
	}

	if(n < 2) {
		funclen ++;
	} else {
		funclen += 2;
	}

	oct.set(funclen);

	unsigned char *dst = (unsigned char *)oct.buffer();

	uint32_t l = 0;
	uint32_t d = 0;
	for(d = 0; d < len; d ++) {
		if(str[d] != '.') {
			dst[d + 1] = str[d];
			l ++;
		} else {
			dst[d - l] = l;
			l = 0;
		}
	}

	if(len > 0) {
		if(str[len - 1] != '.') {
				dst[d - l] = l;
		}
	}

	if(n < 2) {
		dst[funclen - 1] = 0;
	} else {
		uint32_t offset = (uint32_t)(a[1]->intValue(ok));
		uint32_t quotient = offset / 0x100;
		uint32_t remnants = offset % 0x100;

		dst[funclen - 2] = quotient | 0xc0;
		dst[funclen - 1] = remnants;
	}

	return(true);
}

PvObject *MvDnsName::generateValue(WObject *, const PObjectList &a) const {
	PvDnsName *tmp = new PvDnsName();

	if(!generateOctetsWith(a, *tmp, 0)) {
		delete tmp;
		tmp = 0;
		return(0);
	}

	return(tmp);
}

PvObject* MvDnsName::evaluateValue(WObject *, const PObjectList &a) const {
	PvDnsName *tmp = new PvDnsName();

	if(!generateOctetsWith(a, *tmp, 0)) {
		delete tmp;
		tmp = 0;
		return(0);
	}

	return(tmp);
}

bool MvDnsName::isDnsName(CSTR buf, uint32_t buflen) const {
	if(buflen > 1) {
		for(uint32_t d = 0; d < buflen - 1; d ++) {
			if((buf[d] == '.') && (buf[d + 1] == '.')) {
				return(false);
			}
		}
	}

	if((buflen != 1) && (buf[0] == '.')) {
		return(false);
	}

	return(true);
}

//======================================================================

////////////////////////////////////////////////////////////////////////
// home keygen token :=                                               //
//     First (64, HMAC_SHA1 (Kcn, (home address | nonce | 0)))        //
// care-of keygen token :=                                            //
//     First (64, HMAC_SHA1 (Kcn, (care-of address | nonce | 1)))     //
////////////////////////////////////////////////////////////////////////

#define COOKIE_LEN	8
MvCookie64::MvCookie64(CSTR s): MvOctets(s) {}
MvCookie64::~MvCookie64() {}

uint32_t
MvCookie64::functionLength(const PObjectList &, const WObject *) const
{
	return(COOKIE_LEN);
}

bool
MvCookie64::generateOctetsWith(const PObjectList &a,
	PvOctets &oct, WObject *) const
{
	////////////////////////////////
	// 1st argument - Kcn         //
	////////////////////////////////
	PObject *p0 = a[0];
	PvOctets kcn;

	if(!p0->generateOctetsWith(kcn, 0)) {
		return false;
	}

	////////////////////////////////
	// 2nd argument - address     //
	////////////////////////////////
	PObject *p1 = a[1];
	PvV6Addr v6;
	if(!p1->generateV6Addr(v6)) {
		return false;
	}

	////////////////////////////////
	// 3rd argument - nonce       //
	////////////////////////////////
	PObject *p2 = a[2];
	PvOctets nonce;

	if(!p2->generateOctetsWith(nonce, 0)) {
		return false;
	}

	bool ok = true;
	uint32_t flag = a[3]->intValue(ok);

	uint32_t cookielen	= EVP_MAX_MD_SIZE;

	char cookie[EVP_MAX_MD_SIZE];

	HMAC_CTX hmac_ctx;
	HMAC_Init(&hmac_ctx, kcn.buffer(), kcn.length(), EVP_sha1());
#ifdef RR_DBG
xdmp("/tmp/rr_dbg.txt", "MvCookie64", "HMAC_Init(Kcn)",
	kcn.buffer(), kcn.length());
#endif	// RR_DBG
	HMAC_Update(&hmac_ctx, (const unsigned char *)v6.buffer(), v6.length());
#ifdef RR_DBG
xdmp("/tmp/rr_dbg.txt", "MvCookie64", "HMAC_Update(address)",
	v6.buffer(), v6.length());
#endif	// RR_DBG
	HMAC_Update(&hmac_ctx,
		(const unsigned char *)nonce.buffer(), nonce.length());
#ifdef RR_DBG
xdmp("/tmp/rr_dbg.txt", "MvCookie64", "HMAC_Update(nonce)",
	nonce.buffer(), nonce.length());
#endif	// RR_DBG
	unsigned char one = 0;
	if(flag) {
		one = 1;
	}

	HMAC_Update(&hmac_ctx, (const unsigned char *)&one, 1);
#ifdef RR_DBG
xdmp("/tmp/rr_dbg.txt", "MvCookie64", "HMAC_Update(one)", &one, 1);
#endif	// RR_DBG
	HMAC_Final(&hmac_ctx, (unsigned char *)cookie, &cookielen);
#ifdef RR_DBG
xdmp("/tmp/rr_dbg.txt", "MvCookie64", "HMAC_Final(keygen token)",
	cookie, COOKIE_LEN);
#endif	// RR_DBG
	HMAC_cleanup(&hmac_ctx);

	oct.set(COOKIE_LEN);
	memcpy(oct.buffer(), cookie, COOKIE_LEN);

	return(true);
}

PvObject *
MvCookie64::generateValue(WObject *, const PObjectList &a) const
{
	PvOctets *tmp = new PvCookie64;

	if(!generateOctetsWith(a, *tmp, 0)) {
		delete tmp;	tmp = 0;
	}

	return(tmp);
}

PvObject *
MvCookie64::evaluateValue(WObject *, const PObjectList &a) const
{
	PvOctets *tmp = new PvCookie64;

	if(!generateOctetsWith(a,*tmp,0)) {
		delete tmp;	tmp = 0;
	}

	return(tmp);
}
#undef COOKIE_LEN

//======================================================================

////////////////////////////////////////////////////////////////
// Kbm = SHA1 (home keygen token | care-of keygen token)      //
// Kbm = SHA1(home keygen token)                              //
////////////////////////////////////////////////////////////////

#define KBU_LEN	20
MvKbu::MvKbu(CSTR s): MvOctets(s) {}
MvKbu::~MvKbu() {}

uint32_t
MvKbu::functionLength(const PObjectList &a, const WObject *) const
{
	return(KBU_LEN);
}

bool
MvKbu::generateOctetsWith(const PObjectList &a, PvOctets &oct, WObject *) const
{
	////////////////////////////////
	// 1st argument - home cookie //
	////////////////////////////////
	PObject *p0 = a[0];
	PvOctets home;

	if(!p0->generateOctetsWith(home, 0)) {
		return(false);
	}

	uint32_t n = a.size();
	PObject *p1 = 0;
	PvOctets care;

	if(n > 1) {
		p1 = a[1];
		if(!p1->generateOctetsWith(care, 0)) {
			return(false);
		}
	}

	unsigned char kbu[SHA_DIGEST_LENGTH];

	SHA_CTX sha_ctx;

	SHA1_Init(&sha_ctx);
	SHA1_Update(&sha_ctx,
		(const unsigned char *)home.buffer(), home.length());
#ifdef RR_DBG
xdmp("/tmp/rr_dbg.txt", "MvKbu", "SHA1_Update(home keygen token)",
	home.buffer(), home.length());
#endif	// RR_DBG

	if(n > 1) {
		SHA1_Update(&sha_ctx,
			(const unsigned char *)care.buffer(), care.length());
#ifdef RR_DBG
xdmp("/tmp/rr_dbg.txt", "MvKbu", "SHA1_Update(care-of keygen token)",
	care.buffer(), care.length());
#endif	// RR_DBG
	}

	SHA1_Final((unsigned char *)kbu, &sha_ctx);
#ifdef RR_DBG
xdmp("/tmp/rr_dbg.txt", "MvKbu", "SHA1_Final(Kbm)", kbu, sizeof(kbu));
#endif	// RR_DBG
	oct.set(KBU_LEN);
	memcpy(oct.buffer(), kbu, KBU_LEN);

	return(true);
}
#undef KBU_LEN

//======================================================================

////////////////////////////////////////////////////////////////
// Mobility Data = care-of address | correspondent | MH Data  //
// Authenticator = First (96, HMAC_SHA1 (Kbm, Mobility Data)) //
////////////////////////////////////////////////////////////////

#define AUTHENTICATOR_LEN	12
MfBSA::MfBSA(CSTR s): MvOctets(s) {}
MfBSA::~MfBSA() {}

uint32_t
MfBSA::functionLength(const PObjectList &, const WObject *) const
{
	return(AUTHENTICATOR_LEN);
}

bool
MfBSA::generateOctetsWith(const PObjectList &, PvOctets &oct, WObject *) const
{
	oct.set(AUTHENTICATOR_LEN);
	memset(oct.buffer(), 0, AUTHENTICATOR_LEN);
	return(true);
}

OCTSTR
MfBSA::init(OCTSTR cp, const PObjectList &a) const
{
	HMAC_CTX *ctx = cp? (HMAC_CTX *)cp: new HMAC_CTX;

	COCTSTR key = 0;
	uint32_t keylen = 0;

	////////////////////////////////
	// 1st argument - Kbu         //
	////////////////////////////////
	PObject *p0 = a[0];
	PvOctets kbu;

	if(p0->generateOctetsWith(kbu, 0)) {
		key = kbu.buffer();
		keylen = kbu.length();
	}

	HMAC_Init(ctx, (OCTSTR)key, keylen, EVP_sha1());
#ifdef RR_DBG
xdmp("/tmp/rr_dbg.txt", "MfBSA", "HMAC_Init(Kbm)", key, keylen);
#endif	// RR_DBG
	return((OCTSTR)ctx);
}

void
MfBSA::update(OCTSTR cp, const PObjectList &a, const OCTBUF &s) const
{
	HMAC_CTX *ctx = (HMAC_CTX *)cp;

	////////////////////////////////
	// 2nd argument - address     //
	////////////////////////////////
	PObject *p1 = a[1];
	PvV6Addr addr0;
	if(p1->generateV6Addr(addr0)) {
		HMAC_Update(ctx, (OCTSTR)addr0.buffer(), addr0.length());
#ifdef RR_DBG
xdmp("/tmp/rr_dbg.txt", "MfBSA", "HMAC_Update(CoA)",
	addr0.buffer(), addr0.length());
#endif	// RR_DBG
	}

	////////////////////////////////
	// 3rd argument - address     //
	////////////////////////////////
	PObject *p2 = a[2];
	PvV6Addr addr1;
	if(p2->generateV6Addr(addr1)) {
		HMAC_Update(ctx, (OCTSTR)addr1.buffer(), addr1.length());
#ifdef RR_DBG
xdmp("/tmp/rr_dbg.txt", "MfBSA", "HMAC_Update(CN)",
	addr1.buffer(), addr1.length());
#endif	// RR_DBG
	}

	HMAC_Update(ctx, (OCTSTR)s.string(), s.length());
#ifdef RR_DBG
xdmp("/tmp/rr_dbg.txt", "MfBSA", "HMAC_Update(MH)", s.string(), s.length());
#endif	// RR_DBG
	return;
}

PvOctets *
MfBSA::result(OCTSTR cp, const PObjectList &) const
{
	HMAC_CTX *ctx = (HMAC_CTX *)cp;
	uint32_t len = HMAC_MAX_MD_CBLOCK;
	octet m[HMAC_MAX_MD_CBLOCK];

	HMAC_Final(ctx, m, &len);
#ifdef RR_DBG
xdmp("/tmp/rr_dbg.txt", "MfBSA", "HMAC_Final(Authenticator)", m, len);
#endif	// RR_DBG
	PvOctets *rc = new PvBSA96();
	OCTSTR os = rc->buffer();

	memcpy(os, m, AUTHENTICATOR_LEN);
	HMAC_cleanup(ctx);

	return(rc);
}

PObject *
MfBSA::tokenObject(int l, CSTR f) const
{
	return(new PfBSA(this, f, l));
}
#undef AUTHENTICATOR_LEN



//======================================================================
#define DHCP_HMAC_MD5	16
MfDHCPAuth::MfDHCPAuth(CSTR s): MvOctets(s) {}
MfDHCPAuth::~MfDHCPAuth() {}

uint32_t MfDHCPAuth::functionLength(const PObjectList &a, const WObject *) const {
	return(DHCP_HMAC_MD5);
}

bool MfDHCPAuth::generateOctetsWith(const PObjectList &a, PvOctets &oct, WObject *) const {
	oct.set(DHCP_HMAC_MD5);
	memset(oct.buffer(), 0, DHCP_HMAC_MD5);
	return(true);
}

OCTSTR MfDHCPAuth::init(OCTSTR cp, const PObjectList &a) const {
	HMAC_CTX *ctx = cp != 0 ? (HMAC_CTX *)cp : new HMAC_CTX;
	bool ok = true;
	COCTSTR key = 0;
	uint32_t keylen = 0;
	if(a.size() > 0) {
		key = a[0]->octetsValue(ok);
		keylen = a[0]->length();
	}

	HMAC_Init(ctx, (OCTSTR)key, keylen, EVP_md5());

#ifdef DHCP_DBG
xdmp("/tmp/dhcp_dbg.txt", "MfDHCPAuth", "key", key, keylen);
#endif	// DHCP_DBG

	return((OCTSTR)ctx);
}

void MfDHCPAuth::update(OCTSTR cp, const PObjectList &a, const OCTBUF &s) const {
	HMAC_CTX *ctx = (HMAC_CTX *)cp;
	HMAC_Update(ctx, (OCTSTR)s.string(), s.length());

#ifdef DHCP_DBG
xdmp("/tmp/dhcp_dbg.txt", "MfDHCPAuth", "s", s.string(), s.length());
#endif	// DHCP_DBG
}

PvOctets *MfDHCPAuth::result(OCTSTR cp, const PObjectList &) const {
	HMAC_CTX *ctx = (HMAC_CTX *)cp;
	uint32_t len = HMAC_MAX_MD_CBLOCK;
	octet m[HMAC_MAX_MD_CBLOCK];
	HMAC_Final(ctx, m, &len);

	PvOctets *rc = new PvDHCPAuth();

	OCTSTR os = rc->buffer();
	memcpy(os, m, DHCP_HMAC_MD5);

#ifdef DHCP_DBG
xdmp("/tmp/dhcp_dbg.txt", "MfDHCPAuth", "m", m, DHCP_HMAC_MD5);
#endif	// DHCP_DBG

	HMAC_cleanup(ctx);

	return(rc);
}

PObject *MfDHCPAuth::tokenObject(int l, CSTR f) const {
	return(new PfDHCPAuth(this, f, l));
}
#undef DHCP_HMAC_MD5

//======================================================================
MvSubstr::MvSubstr(CSTR s):MvOctets(s) {}
MvSubstr::~MvSubstr() {}

uint32_t MvSubstr::functionLength(const PObjectList& a,const WObject*) const {
	bool ok=true;
	return a[2]->intValue(ok);}

bool MvSubstr::generateSubstr(WObject* w,PObject* o,PObject* l,PvOctets& oct) const {
	bool ok=true;
	uint32_t tl=w->length();
	uint32_t ol=o!=0?o->intValue(ok):0;
	uint32_t ll=l!=0?l->intValue(ok):tl-ol;
	if(tl<ol+ll) {
		WObject* p=w->parent();
		const PObject* s=p->object();
		s->error("E substr(%s=%d,%d,%d) exceeded",w->nameString(),tl,ol,ll);
		return false;}
	PvOctets ref(tl);
	WControl wc;
	w->generate(wc,ref);
	if(!wc) {return false;}
	oct.set(ll);
	ref.substr(ol,ll,&oct);
	return true;}

bool MvSubstr::generateOctetsWith(const PObjectList& a,PvOctets& oct,WObject* w) const {
	if(w==0) {return MvOctets::generateOctetsWith(a,oct,w);}
	return generateSubstr(w,a[1],a[2],oct);}

// COMPOSE
WObject* MvSubstr::composeWv(WControl&,
		WObject* w_parent,const PObject* pv) const {
	return new WvSubstr(w_parent,this,pv);}
void MvSubstr::args_compose(WControl& c,
		WObject* w_self,const PObjectList& pas) const{
	WControl wc; //new WControl, don't use c(break pushSA,IPinfo,etc..)
	const PObject* pa = pas[0];
	pa->selfCompose(wc,w_self);
	if(wc.error())c.set_error(wc.resultcode());
	}

//======================================================================
MvPatch::MvPatch(CSTR s):MvSubstr(s) {}
MvPatch::~MvPatch() {}

uint32_t MvPatch::functionLength(const PObjectList&,const WObject* w) const {
	WObject *n=w->nextChild();
	return n!=0?n->length():0;}

bool MvPatch::generateOctetsWith(const PObjectList& a,PvOctets& oct,WObject* w) const {
	if(w==0) {return MvOctets::generateOctetsWith(a,oct,w);}
	bool ok=true;
	uint32_t tl=w->length();
	int offset=a[1]!=0?a[1]->intValue(ok):0; // xxx
	int val=a[2]!=0?a[2]->intValue(ok):0;    // xxx
       	oct.set(tl);
	WControl wc;
	w->generate(wc,oct);
	if(!wc) {return false;}
	ItPosition at;
	at.addBytes(offset);
	uint32_t orgval=oct.decodeUint(at,8);
	printf("patch: offset(%d): (%02x) -> (%02x)\n",offset,orgval,val);
	oct.encodeUint(val,at,8);
	return true;}

//======================================================================
MvLeft::MvLeft(CSTR s):MvSubstr(s) {}
MvLeft::~MvLeft() {}

uint32_t MvLeft::functionLength(const PObjectList& a,const WObject*) const {
	bool ok=true;
	return a[1]->intValue(ok);}

bool MvLeft::generateOctetsWith(const PObjectList& a,PvOctets& oct,WObject* w) const {
	if(w==0) {return MvOctets::generateOctetsWith(a,oct,w);}
	return generateSubstr(w,0,a[1],oct);}

//======================================================================
MvRight::MvRight(CSTR s):MvSubstr(s) {}
MvRight::~MvRight() {}

uint32_t MvRight::functionLength(const PObjectList& a,const WObject* w) const {
	bool ok=true;
	int l=a[1]->intValue(ok);
	WObject *n=w->nextChild();
	return n!=0?n->length()-l:0;}

bool MvRight::generateOctetsWith(const PObjectList& a,PvOctets& oct,WObject* w) const {
	if(w==0) {return MvOctets::generateOctetsWith(a,oct,w);}
	return generateSubstr(w,a[1],0,oct);}

//======================================================================
MvV4::MvV4(CSTR s):MvFunction(s) {}
MvV4::~MvV4() {}
bool MvV4::generateV4Addr(const PObjectList& a,PvV4Addr& into) const {
	bool ok=true;
	CSTR s=a[0]->strValue(ok);
	return into.pton(s);}

bool MvV4::functionGenerate(WControl& c,WObject* w,OCTBUF& b,const PObjectList& a) const {
	PvV4Addr tmp;
	if(!generateV4Addr(a,tmp)) {
		const PObject* s=w->object();
		s->error("E %s generate error",string());
		c.result(1); return c;}
	return tmp.generate(c,w,b);}

PvObject* MvV4::generateValue(WObject*,const PObjectList& a) const {
	PvV4Addr* tmp=new PvV4Addr;
	if(!generateV4Addr(a,*tmp)) {delete tmp; return 0;}
	return tmp;}

PvObject* MvV4::evaluateValue(WObject*,const PObjectList& a) const {
	PvV4Addr* tmp=new PvV4Addr;
	if(!generateV4Addr(a,*tmp)) {delete tmp; return 0;}
	return tmp;}

//======================================================================
MvEther::MvEther(CSTR s):MvFunction(s) {}
MvEther::~MvEther() {}
bool MvEther::generateEther(const PObjectList& a,PvEther& into) const {
	bool ok=true;
	CSTR s=a[0]->strValue(ok);
	return into.pton(s);}

bool MvEther::evaluateEther(const PObjectList& a,PvEther& into) const {
	return generateEther(a,into);}

bool MvEther::generateTN(const PObject* t,PvEther& into) const {
	bool ok=true;
	CSTR ifn=(t!=0)?t->strValue(ok):0;
	PvIfName* pvif=PvIfName::findTn(ifn);
	if(pvif==0) 	{into.zero(); return false;}
	else		{into=*pvif->ether(); return true;}}

bool MvEther::generateNUT(const PObject* t,PvEther& into) const {
	bool ok=true;
	CSTR ifn=(t!=0)?t->strValue(ok):0;
	PvIfName* pvif=PvIfName::findNut(ifn);
	if(pvif==0) 	{into.zero(); return false;}
	else		{into=*pvif->ether(); return true;}}

bool MvEther::functionGenerate(WControl& c,WObject* w,OCTBUF& b,const PObjectList& a) const {
	PvEther tmp;
	if(!generateEther(a,tmp)) {
		const PObject* s=w->object();
		s->error("E %s generate error",string());
		c.result(1); return c;}
	return tmp.generate(c,w,b);}

PvObject* MvEther::generateValue(WObject*,const PObjectList& a) const {
	PvEther* tmp=new PvEther;
	if(!generateEther(a,*tmp)) {delete tmp; return 0;}
	return tmp;}

PvObject* MvEther::evaluateValue(WObject*,const PObjectList& a) const {
	PvEther* tmp=new PvEther;
	if(!evaluateEther(a,*tmp)) {delete tmp; return 0;}
	return tmp;}

//======================================================================
MvEtherSRC::MvEtherSRC(CSTR s):MvEther(s) {}
MvEtherSRC::~MvEtherSRC() {}
bool MvEtherSRC::generateEther(const PObjectList& a,PvEther& into) const {
	uint32_t n=a.size();
	return generateTN(n>0?a[0]:0,into);}

bool MvEtherSRC::evaluateEther(const PObjectList& a,PvEther& into) const {
	uint32_t n=a.size();
	return generateNUT(n>0?a[0]:0,into);}

//======================================================================
MvEtherDST::MvEtherDST(CSTR s):MvEther(s) {}
MvEtherDST::~MvEtherDST() {}
bool MvEtherDST::generateEther(const PObjectList& a,PvEther& into) const {
	uint32_t n=a.size();
	return generateNUT(n>0?a[0]:0,into);}

bool MvEtherDST::evaluateEther(const PObjectList& a,PvEther& into) const {
	uint32_t n=a.size();
	return generateTN(n>0?a[0]:0,into);}

//======================================================================
MvEtherNUT::MvEtherNUT(CSTR s):MvEther(s) {}
MvEtherNUT::~MvEtherNUT() {}
bool MvEtherNUT::generateEther(const PObjectList& a,PvEther& into) const {
	uint32_t n=a.size();
	return generateNUT(n>0?a[0]:0,into);}

//======================================================================
MvEtherTN::MvEtherTN(CSTR s):MvEther(s) {}
MvEtherTN::~MvEtherTN() {}
bool MvEtherTN::generateEther(const PObjectList& a,PvEther& into) const {
	uint32_t n=a.size();
	return generateTN(n>0?a[0]:0,into);}

//======================================================================
MvEtherMulti::MvEtherMulti(CSTR s):MvEther(s) {}
MvEtherMulti::~MvEtherMulti() {}
bool MvEtherMulti::generateEther(const PObjectList& a,PvEther& into) const {
	PvV6Addr v6;
	PObject* p=a[0];
	if(!p->generateV6Addr(v6)) {return false;}
	into.multicast(v6);
	return true;}

PvObject* MvEtherMulti::generateValue(WObject*,const PObjectList& a) const {
	PvV6Addr v6;
	PObject* p=a[0];
	if(!p->generateV6Addr(v6)) {return 0;}
	PvEther* tmp=new PvEther;
	tmp->multicast(v6);
	return tmp;}

PvObject* MvEtherMulti::evaluateValue(WObject*,const PObjectList& a) const {
	PvV6Addr v6;
	PObject* p=a[0];
	if(!p->evaluateV6Addr(v6)) {return 0;}
	PvEther* tmp=new PvEther;
	tmp->multicast(v6);
	return tmp;}

//======================================================================
MvV6::MvV6(CSTR s):MvFunction(s) {}
MvV6::~MvV6() {}
bool MvV6::generateV6Addr(const PObjectList& a,PvV6Addr& into) const {
	bool ok=true;
	CSTR s=a[0]->strValue(ok);
	return into.pton(s);}

bool MvV6::evaluateV6Addr(const PObjectList& a,PvV6Addr& into) const {
	return generateV6Addr(a,into);}

bool MvV6::generateTN(const PObject* t,PvV6Addr& into) const {
	bool ok=true;
	CSTR ifn=(t!=0)?t->strValue(ok):0;
	PvIfName* pvif=PvIfName::findTn(ifn);
	if(pvif==0) 	{into.zero(); return false;}
	else		{into=*pvif->v6addr(); return true;}}

bool MvV6::generateNUT(const PObject* t,PvV6Addr& into) const {
	bool ok=true;
	CSTR ifn=(t!=0)?t->strValue(ok):0;
	PvIfName* pvif=PvIfName::findNut(ifn);
	if(pvif==0) 	{into.zero(); return false;}
	else		{into=*pvif->v6addr(); return true;}}

bool MvV6::functionGenerate(WControl& c,WObject* w,OCTBUF& b,const PObjectList& a) const {
	PvV6Addr tmp;
	if(!generateV6Addr(a,tmp)) {
		const PObject* s=w->object();
		s->error("E %s generate error",string());
		c.result(1); return c;}
	return tmp.generate(c,w,b);}

PvObject* MvV6::generateValue(WObject*,const PObjectList& a) const {
	PvV6Addr* tmp=new PvV6Addr;
	if(!generateV6Addr(a,*tmp)) {delete tmp; return 0;}
	return tmp;}

PvObject* MvV6::evaluateValue(WObject*,const PObjectList& a) const {
	PvV6Addr* tmp=new PvV6Addr;
	if(!evaluateV6Addr(a,*tmp)) {delete tmp; return 0;}
	return tmp;}

//======================================================================
MvV6SRC::MvV6SRC(CSTR s):MvV6(s) {}
MvV6SRC::~MvV6SRC() {}
bool MvV6SRC::generateV6Addr(const PObjectList& a,PvV6Addr& into) const {
	uint32_t n=a.size();
	return generateTN(n>0?a[0]:0,into);}

bool MvV6SRC::evaluateV6Addr(const PObjectList& a,PvV6Addr& into) const {
	uint32_t n=a.size();
	return generateNUT(n>0?a[0]:0,into);}

//======================================================================
MvV6DST::MvV6DST(CSTR s):MvV6(s) {}
MvV6DST::~MvV6DST() {}
bool MvV6DST::generateV6Addr(const PObjectList& a,PvV6Addr& into) const {
	uint32_t n=a.size();
	return generateNUT(n>0?a[0]:0,into);}

bool MvV6DST::evaluateV6Addr(const PObjectList& a,PvV6Addr& into) const {
	uint32_t n=a.size();
	return generateTN(n>0?a[0]:0,into);}

//======================================================================
MvV6TN::MvV6TN(CSTR s):MvV6(s) {}
MvV6TN::~MvV6TN() {}
bool MvV6TN::generateV6Addr(const PObjectList& a,PvV6Addr& into) const {
	uint32_t n=a.size();
	return generateTN(n>0?a[0]:0,into);}

//======================================================================
MvV6NUT::MvV6NUT(CSTR s):MvV6(s) {}
MvV6NUT::~MvV6NUT() {}
bool MvV6NUT::generateV6Addr(const PObjectList& a,PvV6Addr& into) const {
	uint32_t n=a.size();
	return generateNUT(n>0?a[0]:0,into);}

//======================================================================
MvV6PTN::MvV6PTN(CSTR s):MvV6(s) {}
MvV6PTN::~MvV6PTN() {}
bool MvV6PTN::generateV6Addr(const PObjectList& a,PvV6Addr& into) const {
	bool ok=true;
	uint32_t n=a.size();
	CSTR net=a[0]->strValue(ok);
	int len=a[1]->intValue(ok);
	PvV6Addr tmp;
	if(!generateTN(n>2?a[2]:0,tmp)) {return false;}
	return into.netMerge(net,len,tmp);}

//======================================================================
MvV6PNUT::MvV6PNUT(CSTR s):MvV6(s) {}
MvV6PNUT::~MvV6PNUT() {}
bool MvV6PNUT::generateV6Addr(const PObjectList& a,PvV6Addr& into) const {
	bool ok=true;
	uint32_t n=a.size();
	CSTR net=a[0]->strValue(ok);
	int len=a[1]->intValue(ok);
	PvV6Addr tmp;
	if(!generateNUT(n>2?a[2]:0,tmp)) {return false;}
	return into.netMerge(net,len,tmp);}

//======================================================================
MvV6Merge::MvV6Merge(CSTR s):MvV6(s) {}
MvV6Merge::~MvV6Merge() {}
bool MvV6Merge::generateV6Addr(const PObjectList& a,PvV6Addr& into) const {
	bool ok=true;
	CSTR net=a[0]->strValue(ok);
	int len=a[1]->intValue(ok);
	PvV6Addr v6;
	PObject* p=a[2];
	if(!p->generateV6Addr(v6)) {return false;}
	return into.netMerge(net,len,v6);}

PvObject* MvV6Merge::generateValue(WObject*,const PObjectList& a) const {
	bool ok=true;
	CSTR net=a[0]->strValue(ok);
	int len=a[1]->intValue(ok);
	PvV6Addr v6;
	PObject* p=a[2];
	if(!p->generateV6Addr(v6)) {return 0;}
	PvV6Addr* tmp=new PvV6Addr;
	tmp->netMerge(net,len,v6);
	return tmp;}

PvObject* MvV6Merge::evaluateValue(WObject*,const PObjectList& a) const {
	bool ok=true;
	CSTR net=a[0]->strValue(ok);
	int len=a[1]->intValue(ok);
	PvV6Addr v6;
	PObject* p=a[2];
	if(!p->evaluateV6Addr(v6)) {return 0;}
	PvV6Addr* tmp=new PvV6Addr;
	tmp->netMerge(net,len,v6);
	return tmp;}

//======================================================================
MvV6V6::MvV6V6(CSTR s):MvV6(s) {}
MvV6V6::~MvV6V6() {}
bool MvV6V6::generateV6Addr(const PObjectList& a,PvV6Addr& into) const {
	bool ok=true;
	CSTR net=a[0]->strValue(ok);
	int len=a[1]->intValue(ok);
	CSTR host=a[2]->strValue(ok);
	PvV6Addr hostAddr(host,ok);	if(!ok) {return false;}
	return into.netMerge(net,len,hostAddr);}

//======================================================================
MvV6Ether::MvV6Ether(CSTR s):MvV6(s) {}
MvV6Ether::~MvV6Ether() {}
bool MvV6Ether::generateV6Addr(const PObjectList& a,PvV6Addr& into) const {
	bool ok=true;
	CSTR ep=a[0]->strValue(ok);
	PvEther eth(ep,ok);
	PvEUI64 eui64(eth);
	const PvV6Addr& l=PvV6Addr::linkLocal();
	return l.merge(64,eui64,&into);}

//======================================================================
MvObjectID::MvObjectID(CSTR s):MvOctets(s) {}
MvObjectID::~MvObjectID() {}

uint32_t MvObjectID::functionLength(const PObjectList &a, const WObject *) const {
	bool ok = true;
	CSTR work = a[0]->strValue(ok);
	uint32_t buflen = a[0]->length();
	char dstbuf[128];
	uint32_t dstlen = 0;
	
	asn1encode((char*)&dstbuf, &dstlen, work, buflen);
	return(dstlen);
}

bool MvObjectID::generateOctetsWith(const PObjectList &a, PvOctets &oct, WObject *) const {
	OCTSTR buf = 0;
	bool ok = true;
	CSTR work = a[0]->strValue(ok);
	uint32_t buflen = a[0]->length();
	char dstbuf[128];
	uint32_t dstlen = 0;

	work = a[0]->strValue(ok);
	asn1encode((char*)&dstbuf, &dstlen, work, buflen);
	oct.set(dstlen);
	buf = oct.buffer();
	memcpy(buf, dstbuf, dstlen);
	return(true);
}
bool MvObjectID::asn1encode(char *dst, uint32_t *dstlen, CSTR src, uint32_t srclen) const {
	int l=0, first_digit = -1;
	uint32_t i=0, j=0, k=0, buflen=0;
	char buf0[32];
		
	memset(buf0, 0, 32);
	k=0;
	for (i = 0; i < srclen; i++) {
		buflen=0;
		if (src[i] != '.') {
			for (j=1; j< srclen - i ; j++)
				if (src[i+j] == '.') break;
			buflen = j;
			strncpy(buf0, &src[i], buflen);
			buf0[buflen] = '\0';
			if (first_digit == -1) {
					first_digit = atoi(buf0);
			} else {
				if (k == 0) {
					dst[k] = first_digit * 40 + atoi(buf0);
					k++;
				} else {
					if (atoi(buf0) > 127) {
						for (j=0; j< srclen - i ; j++)
							if (0 == (atoi(buf0) >> 7*j)) break;
						
						for (l=j-1; l>=0; l--) {
							dst[k] = ((atoi(buf0) >> 7*l)& 0x7F);
							if (l!=0) dst[k] |= 0x80;
							k++;
						}
					} else {
						dst[k] = atoi(buf0);
						k++;
					}
				}
			}
		i+=buflen;
		}
	}
	*dstlen = (uint32_t)k;
	return true;
}
bool MvObjectID::isOidStr(CSTR buf, uint32_t buflen) const {
	// check 1. must not start and end with "."
	// check 2. must consist of [0-9] and ".".
	if (buf[0] == '.' || buf[buflen-1] == '.')
		return false;

	for(uint32_t d = 0; d < buflen; d ++)
		if(((buf[d] < '0') || (buf[d] > '9')) && (buf[d] != '.') )
			return false;

	return true;
}

//======================================================================
#ifndef ISAKMP_PAYLOAD_BODY_OFFSET
#define ISAKMP_PAYLOAD_BODY_OFFSET	4
#endif	// ISAKMP_PAYLOAD_BODY_OFFSET
MvP1_HASH::MvP1_HASH(CSTR s): MvOctets(s) {}
MvP1_HASH::~MvP1_HASH() {}

uint32_t
MvP1_HASH::functionLength(const PObjectList &a, const WObject *) const
{
	bool ok	= true;

	uint32_t length	= 0;
	CSTR algorithm	= a[0]->strValue(ok);

	for( ; ; ) {
		if(!strcmp(algorithm, "hmacmd5")) {
			length = EVP_MD_size(EVP_md5());
			break;
		}

		if(!strcmp(algorithm, "hmacsha1")) {
			length = EVP_MD_size(EVP_sha1());
			break;
		}

		break;
	}

	return(length);
}

bool
MvP1_HASH::generateOctetsWith(const PObjectList &a,
	PvOctets &oct, WObject *) const
{
	bool ok	= true;

	////////////////////////////////
	// algorithm                  //
	////////////////////////////////
	const EVP_MD *evp_md	= 0;
	CSTR algorithm		= a[0]->strValue(ok);

	for( ; ; ) {
		if(!strcmp(algorithm, "hmacmd5")) {
			evp_md	= EVP_md5();
			break;
		}

		if(!strcmp(algorithm, "hmacsha1")) {
			evp_md	= EVP_sha1();
			break;
		}

		break;
	}

	if(!evp_md) {
		return(false);
	}

	////////////////////////////////
	// SKEYID                     //
	////////////////////////////////
	HMAC_CTX ctx;
	memset(&ctx, 0, sizeof(ctx));

        PObject *SKEYID		= a[1];

        PvOctets skeyid;

	if(!(SKEYID->generateOctetsWith(skeyid, 0))) {
		return(false);
	}

	COCTSTR key		= skeyid.buffer();
	uint32_t key_len	= skeyid.length();

	HMAC_Init(&ctx, key, key_len, evp_md);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP1_HASH", "HMAC_Init(SKEYID)", key, key_len);
#endif	// ISAKMP_DBG
	////////////////////////////////////////
	// g^xi | g^xr | CKY-I | CKY-R        //
	// g^xr | g^xi | CKY-R | CKY-I        //
	////////////////////////////////////////
	uint32_t d	= 0;

	for(d = 2; d < 6; d ++) {
		PObject *p	= a[d];
	        PvOctets pv;

		if(!(p->generateOctetsWith(pv, 0))) {
			return(false);
		}

		COCTSTR data	= pv.buffer();
		uint32_t len	= pv.length();

		HMAC_Update(&ctx, data, len);
#ifdef ISAKMP_DBG
{
unsigned int idx = d - 2;
const char *const keyword[] = {
	"HMAC_Update(HASH_I(g^xi), HASH_R(g^xr))",
	"HMAC_Update(HASH_I(g^xr), HASH_R(g^xi))",
	"HMAC_Update(HASH_I(CKY-I), HASH_R(CKY-R))",
	"HMAC_Update(HASH_I(CKY-R), HASH_R(CKY-I))",
	NULL
};
xdmp("/tmp/isakmp_dbg.txt", "MvP1_HASH", keyword[idx], data, len);
}
#endif	// ISAKMP_DBG
	}



	////////////////////////////////
	// SAi_b | IDii_b             //
	// SAi_b | IDir_b             //
	////////////////////////////////
	for(d = 6; d < 8; d ++) {
		PObject *p	= a[d];
		WControl wc;

		WObject *w_self	= p->selfCompose(wc, 0);
		if((!w_self) || (wc.error())) {
			return(false);
		}

		ItPosition it;
		w_self->generatePrologue(it);
		uint32_t tl = w_self->length();

		PvOctets pv(tl);
		w_self->generate(wc, pv);
		if((!wc) || (!w_self) || (wc.error())) {
			return(false);
		}

		COCTSTR data	= pv.buffer() + ISAKMP_PAYLOAD_BODY_OFFSET;
		uint32_t len	= pv.length() - ISAKMP_PAYLOAD_BODY_OFFSET;

		HMAC_Update(&ctx, data, len);
#ifdef ISAKMP_DBG
{
unsigned int idx = d - 6;
const char *const keyword[] = {
	"HMAC_Update(SAi_b)",
	"HMAC_Update(HASH_I(IDii_b), HASH_R(IDir_b))",
	NULL
};
xdmp("/tmp/isakmp_dbg.txt", "MvP1_HASH", keyword[idx], data, len);
}
#endif	// ISAKMP_DBG
	}



	uint32_t md_len	= EVP_MAX_MD_SIZE;
	octet md[EVP_MAX_MD_SIZE];

	HMAC_Final(&ctx, md, &md_len);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP1_HASH", "HMAC_Final(HASH_I, HASH_R)",
	md, md_len);
#endif	// ISAKMP_DBG
	oct.set(md_len);
	memcpy(oct.buffer(), md, md_len);

	HMAC_cleanup(&ctx);

	return(true);
}
#ifdef ISAKMP_PAYLOAD_BODY_OFFSET
#undef ISAKMP_PAYLOAD_BODY_OFFSET
#endif	// ISAKMP_PAYLOAD_BODY_OFFSET

//======================================================================
MvP1_IV::MvP1_IV(CSTR s): MvOctets(s) {}
MvP1_IV::~MvP1_IV() {}

uint32_t
MvP1_IV::functionLength(const PObjectList &a, const WObject *) const
{
	uint32_t len	= 0;
	uint32_t n	= a.size();
	bool ok		= true;
	CSTR algorithm  = a[0]->strValue(ok);

	if(n < 4) {
		for( ; ; ) {
			if(!strcmp(algorithm, "md5")) {
				len = MD5_DIGEST_LENGTH;
				break;
			}

			if(!strcmp(algorithm, "sha1")) {
				len = SHA_DIGEST_LENGTH;
				break;
			}

			break;
		}
	} else {
		len = a[3]->intValue(ok);
	}

	return(len);
}

bool
MvP1_IV::generateOctetsWith(const PObjectList &a,
	PvOctets &oct, WObject *) const
{
	bool ok = true;

	////////////////////////////////
	// g^xi                       //
	////////////////////////////////
	PObject *p1 = a[1];
	PvOctets g_xi;

	if(!p1->generateOctetsWith(g_xi, 0)) {
		return(false);
	}

	////////////////////////////////
	// g^xr                       //
	////////////////////////////////
	PObject *p2 = a[2];
	PvOctets g_xr;

	if(!p2->generateOctetsWith(g_xr, 0)) {
		return(false);
	}

	////////////////////////////////
	// algorithm                  //
	////////////////////////////////
	CSTR algorithm  = a[0]->strValue(ok);

	for( ; ; ) {
		if(!strcmp(algorithm, "md5")) {
			unsigned char digest[MD5_DIGEST_LENGTH];

			MD5_CTX md5_ctx;
			MD5_Init(&md5_ctx);
			MD5_Update(&md5_ctx,
				(const unsigned char *)g_xi.buffer(),
				g_xi.length());
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP1_IV", "MD5_Update(g^xi)",
	g_xi.buffer(), g_xi.length());
#endif	// ISAKMP_DBG
			MD5_Update(&md5_ctx,
				(const unsigned char *)g_xr.buffer(),
				g_xr.length());
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP1_IV", "MD5_Update(g^xr)",
	g_xr.buffer(), g_xr.length());
#endif	// ISAKMP_DBG
			MD5_Final((unsigned char *)digest, &md5_ctx);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP1_IV", "MD5_Final(IV)",
	digest, functionLength(a, 0));
#endif	// ISAKMP_DBG
			oct.set(functionLength(a, 0));
			memcpy(oct.buffer(), digest, functionLength(a, 0));

			break;
		}

		if(!strcmp(algorithm, "sha1")) {
			unsigned char digest[SHA_DIGEST_LENGTH];

			SHA_CTX sha_ctx;
			SHA1_Init(&sha_ctx);
			SHA1_Update(&sha_ctx,
				(const unsigned char *)g_xi.buffer(),
				g_xi.length());
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP1_IV", "SHA1_Update(g^xi)",
	g_xi.buffer(), g_xi.length());
#endif	// ISAKMP_DBG
			SHA1_Update(&sha_ctx,
				(const unsigned char *)g_xr.buffer(),
				g_xr.length());
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP1_IV", "SHA1_Update(g^xr)",
	g_xr.buffer(), g_xr.length());
#endif	// ISAKMP_DBG
			SHA1_Final((unsigned char *)digest, &sha_ctx);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP1_IV", "SHA1_Final(IV)",
	digest, functionLength(a, 0));
#endif	// ISAKMP_DBG
			oct.set(functionLength(a, 0));
			memcpy(oct.buffer(), digest, functionLength(a, 0));

			break;
		}

	        return(false);
		break;
	}

        return(true);
}

//======================================================================
MvP2_IV::MvP2_IV(CSTR s): MvOctets(s) {}
MvP2_IV::~MvP2_IV() {}

uint32_t
MvP2_IV::functionLength(const PObjectList &a, const WObject *) const
{
	uint32_t len	= 0;
	uint32_t n	= a.size();
	bool ok		= true;
        CSTR algorithm	= a[0]->strValue(ok);

	if(n < 3) {
		for( ; ; ) {
			if(!strcmp(algorithm, "md5")) {
				len = MD5_DIGEST_LENGTH;
				break;
			}

			if(!strcmp(algorithm, "sha1")) {
				len = SHA_DIGEST_LENGTH;
				break;
			}

			break;
		}
	} else {
		len = a[2]->intValue(ok);
	}

	return(len);
}

bool
MvP2_IV::generateOctetsWith(const PObjectList &a,
	PvOctets &oct, WObject *) const
{
	bool ok = true;

	////////////////////////////////
	// CBC                        //
	////////////////////////////////
	PObject *p1 = a[1];
	PvOctets last;

	if(!p1->generateOctetsWith(last, 0)) {
		return(false);
	}

        ////////////////////////////////
        // algorithm                  //
        ////////////////////////////////
        CSTR algorithm  = a[0]->strValue(ok);

	uint32_t mid = htonl(ISAKMP_Message_ID);

        for( ; ; ) {
                if(!strcmp(algorithm, "md5")) {
                        unsigned char digest[MD5_DIGEST_LENGTH];

                        MD5_CTX md5_ctx;
                        MD5_Init(&md5_ctx);
                        MD5_Update(&md5_ctx,
                                (const unsigned char *)last.buffer(),
                                last.length());
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP2_IV", "MD5_Update(CBC)",
	last.buffer(), last.length());
#endif	// ISAKMP_DBG
                        MD5_Update(&md5_ctx,
                                (const unsigned char *)&mid,
                                sizeof(mid));
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP2_IV", "MD5_Update(M-ID)",
	&mid, sizeof(mid));
#endif	// ISAKMP_DBG
                        MD5_Final((unsigned char *)digest, &md5_ctx);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP2_IV", "MD5_Final(IV)",
	digest, functionLength(a, 0));
#endif	// ISAKMP_DBG
                        oct.set(functionLength(a, 0));
                        memcpy(oct.buffer(), digest, functionLength(a, 0));

                        break;
                }

                if(!strcmp(algorithm, "sha1")) {
                        unsigned char digest[SHA_DIGEST_LENGTH];

                        SHA_CTX sha_ctx;
                        SHA1_Init(&sha_ctx);
                        SHA1_Update(&sha_ctx,
                                (const unsigned char *)last.buffer(),
                                last.length());
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP2_IV", "SHA1_Update(CBC)",
	last.buffer(), last.length());
#endif	// ISAKMP_DBG
                        SHA1_Update(&sha_ctx,
                                (const unsigned char *)&mid,
                                sizeof(mid));
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP2_IV", "SHA1_Update(M-ID)",
	&mid, sizeof(mid));
#endif	// ISAKMP_DBG
                        SHA1_Final((unsigned char *)digest, &sha_ctx);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP2_IV", "SHA1_Final(IV)",
	digest, functionLength(a, 0));
#endif	// ISAKMP_DBG
                        oct.set(functionLength(a, 0));
                        memcpy(oct.buffer(), digest, functionLength(a, 0));

                        break;
                }

                return(false);
                break;
        }

        return(true);
}

//======================================================================
MvP2_HASH_2::MvP2_HASH_2(CSTR s): MvOctets(s) {}
MvP2_HASH_2::~MvP2_HASH_2() {}

uint32_t
MvP2_HASH_2::functionLength(const PObjectList &a, const WObject *) const
{
	bool ok = true;

	uint32_t length = 0;
	CSTR algorithm  = a[0]->strValue(ok);

	for( ; ; ) {
		if(!strcmp(algorithm, "hmacmd5")) {
			length = EVP_MD_size(EVP_md5());
			break;
		}

		if(!strcmp(algorithm, "hmacsha1")) {
			length = EVP_MD_size(EVP_sha1());
			break;
		}

		break;
	}

	return(length);
}

bool
MvP2_HASH_2::generateOctetsWith(const PObjectList &a,
	PvOctets &oct, WObject *) const
{
	oct.set(functionLength(a, 0));
	memset(oct.buffer(), 0x00, functionLength(a, 0));
        return(true);
}

OCTSTR
MvP2_HASH_2::init(OCTSTR cp, const PObjectList &a) const
{
	HMAC_CTX *ctx = cp? (HMAC_CTX *)cp: new HMAC_CTX;

	//----------------------------//
        // 1st argument - algorithm   //
	//----------------------------//
	bool ok = true;
	const EVP_MD *evp_md	= 0;
	CSTR algorithm		= a[0]->strValue(ok);

	COCTSTR key	= 0;
	uint32_t keylen	= 0;

	for( ; ; ) {
		if(!strcmp(algorithm, "hmacmd5")) {
			evp_md	= EVP_md5();
			break;
		}

		if(!strcmp(algorithm, "hmacsha1")) {
			evp_md	= EVP_sha1();
			break;
		}

		break;
	}

	//----------------------------//
	// 2nd argument - SKEYID_a    //
	//----------------------------//
	PObject *p1 = a[1];
	PvOctets skeyid_a;

	if(p1->generateOctetsWith(skeyid_a, 0)) {
		key	= skeyid_a.buffer();
		keylen	= skeyid_a.length();
	}

	HMAC_Init(ctx, (OCTSTR)key, keylen, evp_md);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP2_HASH_2", "HMAC_Init(SKEYID_a)", key, keylen);
#endif	// ISAKMP_DBG
	return((OCTSTR)ctx);
}

void
MvP2_HASH_1::update(OCTSTR cp, const PObjectList &a, const OCTBUF &s) const
{
	HMAC_CTX *ctx = (HMAC_CTX *)cp;

	uint32_t mid = htonl(ISAKMP_Message_ID);

	HMAC_Update(ctx, (OCTSTR)&mid, sizeof(mid));
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP2_HASH_1", "HMAC_Update(M-ID)",
	&mid, sizeof(mid));
#endif	// ISAKMP_DBG
	HMAC_Update(ctx, (OCTSTR)s.string(), s.length());
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP2_HASH_1",
	"HMAC_Update(SA | Ni [ | KE ] [ | IDci | IDcr ])",
	s.string(), s.length());
#endif	// ISAKMP_DBG
	return;
}

void
MvP2_HASH_2::update(OCTSTR cp, const PObjectList &a, const OCTBUF &s) const
{
	HMAC_CTX *ctx = (HMAC_CTX *)cp;

	uint32_t mid = htonl(ISAKMP_Message_ID);

	HMAC_Update(ctx, (OCTSTR)&mid, sizeof(mid));
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP2_HASH_2", "HMAC_Update(M-ID)",
	&mid, sizeof(mid));
#endif	// ISAKMP_DBG
	//----------------------------//
	// 3rd argument - Ni_b        //
	//----------------------------//
	PObject *p2 = a[2];
	PvOctets ni_b;

	COCTSTR msg2		= 0;
	uint32_t msg2len	= 0;

	if(p2->generateOctetsWith(ni_b, 0)) {
		msg2	= ni_b.buffer();
		msg2len	= ni_b.length();
	}

        HMAC_Update(ctx, (OCTSTR)msg2, msg2len);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP2_HASH_2", "HMAC_Update(Ni_b)", msg2, msg2len);
#endif	// ISAKMP_DBG
	HMAC_Update(ctx, (OCTSTR)s.string(), s.length());
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP2_HASH_2",
	"HMAC_Update(SA | Nr [ | KE ] [ | IDci | IDcr ])",
	s.string(), s.length());
#endif	// ISAKMP_DBG
	return;
}

void
MvP2_HASH_3::update(OCTSTR cp, const PObjectList &a, const OCTBUF &s) const
{
	HMAC_CTX *ctx = (HMAC_CTX *)cp;

	uint32_t mid = htonl(ISAKMP_Message_ID);

	char zero = 0x00;

	HMAC_Update(ctx, (OCTSTR)&zero, sizeof(zero));
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP2_HASH_3", "HMAC_Update(0)",
	&zero, sizeof(zero));
#endif	// ISAKMP_DBG
	HMAC_Update(ctx, (OCTSTR)&mid, sizeof(mid));
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP2_HASH_3", "HMAC_Update(M-ID)",
	&mid, sizeof(mid));
#endif	// ISAKMP_DBG
	//----------------------------//
	// 3rd argument - Ni_b        //
	//----------------------------//
	PObject *p2 = a[2];
	PvOctets ni_b;

	COCTSTR msg2		= 0;
	uint32_t msg2len	= 0;

	if(p2->generateOctetsWith(ni_b, 0)) {
		msg2	= ni_b.buffer();
		msg2len	= ni_b.length();
	}

        HMAC_Update(ctx, (OCTSTR)msg2, msg2len);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP2_HASH_3", "HMAC_Update(Ni_b)",
	msg2, msg2len);
#endif	// ISAKMP_DBG
	//----------------------------//
	// 4th argument - Nr_b        //
	//----------------------------//
	PObject *p3 = a[3];
	PvOctets nr_b;

	COCTSTR msg3		= 0;
	uint32_t msg3len	= 0;

	if(p3->generateOctetsWith(nr_b, 0)) {
		msg3	= nr_b.buffer();
		msg3len	= nr_b.length();
	}

        HMAC_Update(ctx, (OCTSTR)msg3, msg3len);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP2_HASH_3", "HMAC_Update(Nr_b)",
	msg3, msg3len);
#endif	// ISAKMP_DBG
	return;
}

PvOctets *
MvP2_HASH_2::result(OCTSTR cp, const PObjectList &a) const
{
	HMAC_CTX *ctx = (HMAC_CTX *)cp;

	uint32_t len = HMAC_MAX_MD_CBLOCK;
	octet m[HMAC_MAX_MD_CBLOCK];

	HMAC_Final(ctx, m, &len);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MvP2_HASH_2",
	"HMAC_Final(HASH(1), HASH(2), HASH(3))",
	m, functionLength(a, 0));
#endif	// ISAKMP_DBG
	PvOctets *rc = new PvOctets(functionLength(a, 0));

	OCTSTR os = rc->buffer();

	memcpy(os, m, functionLength(a, 0));

	return(rc);
}

PObject *
MvP2_HASH_2::tokenObject(int l, CSTR f) const
{
	return(new PfP2_HASH_2(this, f, l));
}

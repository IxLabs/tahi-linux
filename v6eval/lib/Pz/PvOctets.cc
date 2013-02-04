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
 * $TAHI: v6eval/lib/Pz/PvOctets.cc,v 1.46 2005/05/09 09:35:24 akisada Exp $
 */
#include "PvOctets.h"
#include "RObject.h"
#include "WObject.h"
#include "ItPosition.h"
#include "PvIfName.h"
#include "PControl.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#if defined(__FreeBSD__)
#include <net/if.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
/*
extern "C" {
char *ether_ntoa __P((const struct ether_addr *));
struct ether_addr *ether_aton __P((const char *));
int ether_ntohost __P((char *, const struct ether_addr *));
int ether_hostton __P((const char *, struct ether_addr *));
int ether_line __P((const char *, struct ether_addr *, char *));
}
*/
#endif
#include <string.h>

PvOctets::~PvOctets() {
	if(allocated_!=0) {
		delete [] buffer_; allocated_=0;}
	buffer_=0;}
PvOctets::PvOctets():PvObject(),allocated_(0),length_(0),buffer_(0) {}
PvOctets::PvOctets(const PvOctets& o):PvObject(o),allocated_(0),length_(0),buffer_(0) {
	uint32_t l=o.length();
	length_=l;
	OCTSTR s=(OCTSTR)o.string();
	if(o.allocated_==0) {buffer_=s;}
	else {
		buffer_=new octet[allocated_=l];
		memcpy(buffer_,s,l);}}
PvOctets::PvOctets(uint32_t l,OCTSTR o,bool b):allocated_(0),length_(l),buffer_(o) {
	if(b||(buffer_==0&&l>0)) {buffer_=new octet[allocated_=l];}
	if(o!=0&&o!=buffer_) {memcpy(buffer_,o,l);}}
PvObject* PvOctets::shallowCopy() const {
	return new PvOctets(length(),(OCTSTR)string());}

OCTSTR PvOctets::buffer(uint32_t n) {
	uint32_t l=length();
	if(n>l) {abort();}
	if(buffer_==0&&l>0) {buffer_=new octet[allocated_=l];}
	return buffer_+n;}

bool PvOctets::isEqual(const PvOctets* o) const {
	uint32_t ml=length();
	uint32_t ol=o->length();
	return (ml==ol && memcmp(string(),o->string(),ml)==0);}

COCTSTR PvOctets::octetsValue(bool& b) const {
	b=true;
	return string();}
PvOctets* PvOctets::octetString() const {return (PvOctets*)this;}

PvV4Addr::~PvV4Addr() {}
PvV4Addr::PvV4Addr(OCTSTR o,bool b):PvOctets(sizeof(v4addr),o,b) {}
PvObject* PvV4Addr::shallowCopy() const {
	return new PvV4Addr((OCTSTR)string());}
PvV4Addr::PvV4Addr(CSTR p,bool& c,OCTSTR o):PvOctets(sizeof(v4addr),o) {
	c=pton(p);}
bool PvV4Addr::pton(CSTR p) {
	if(p==0) {return false;}
	return inet_pton(AF_INET,p,buffer())==1;}

PvEther::~PvEther() {}
PvEther::PvEther(OCTSTR o,bool b):PvOctets(sizeof(etheraddr),o,b) {}
PvEther::PvEther(CSTR p,bool& c,OCTSTR o):PvOctets(sizeof(etheraddr),o) {
	c=pton(p);}
PvObject* PvEther::shallowCopy() const {
	return new PvEther((OCTSTR)string());}
bool PvEther::pton(CSTR p) {
	if(p==0) {return false;}
	struct ether_addr *n=ether_aton((char*)p);
	if(n!=0) {set(length(),n->octet);}
	return (n!=0);}
PvEther::PvEther(const PvV6Addr& v6,OCTSTR o):PvOctets(sizeof(etheraddr),o) {
	multicast(v6);}
void PvEther::multicast(const PvV6Addr& v6) {
	COCTSTR src=v6.string();
	OCTSTR buf=buffer();
	buf[0]=0x33; buf[1]=0x33;
	buf[2]=src[12]; buf[3]=src[13]; buf[4]=src[14]; buf[5]=src[15];}

PvEUI64::~PvEUI64() {}
PvEUI64::PvEUI64(OCTSTR o,bool b):PvOctets(sizeof(eui64addr),o,b) {}
PvEUI64::PvEUI64(const PvEther& et,OCTSTR o):PvOctets(sizeof(eui64addr),o) {
	create(et);}
void PvEUI64::create(const PvEther& et) {
	COCTSTR src=et.string();
	OCTSTR buf=buffer();
	buf[0]=src[0]^0x02;	buf[1]=src[1];		buf[2]=src[2];
	buf[3]=0xff;		buf[4]=0xfe;
	buf[5]=src[3];		buf[6]=src[4];		buf[7]=src[5];}

PvV6Addr::~PvV6Addr() {}
PvV6Addr::PvV6Addr(OCTSTR o,bool b):PvOctets(sizeof(v6addr),o,b) {}
PvV6Addr::PvV6Addr(CSTR p,bool& c,OCTSTR o):PvOctets(sizeof(v6addr),o) {
	c=pton(p);}
PvObject* PvV6Addr::shallowCopy() const {
	return new PvV6Addr((OCTSTR)string());}
bool PvV6Addr::pton(CSTR p) {
	if(p==0) {return false;}
	return inet_pton(AF_INET6,p,buffer())==1;}
PvV6Addr* PvV6Addr::merge(uint16_t w,const PvOctets& h,PvV6Addr* o) const {
	uint32_t tw=length()*8;			// v6addr*8
	if(o==0) {o=new PvV6Addr;}
	if(o!=this) {o->zero();}
	uint32_t hw=h.length()*8;		// host width
	if(hw>tw) {abort();}
	uint16_t pw=(w+hw<tw)?tw-hw:w;		// prefix width
	uint32_t rw=tw-pw;
	ItPosition it;
	o->encodeLAs(string(),it,w);
	it.addBytes(pw/8); it.addBits(pw%8);
	uint32_t ho=(hw-rw)/8;			// host offset;
//	printf("it(%d,%d) ho=%d rw=%d\n",it.bytes(),it.bits(),ho,rw);
	o->encodeRAs(h.string(ho),it,rw);
	return o;}

bool PvV6Addr::isV6Addr() const {return true;}

PvOctets* PvOctets::set(uint32_t l,OCTSTR s) {
	length_=l;
	if(allocated_==0) {
		if(s!=0) {buffer_=s;}
		return this;}
	if(allocated_<l) {
		OCTSTR old=buffer_;
		buffer_=new octet[allocated_=l];
		if(s==0&&old!=0) {memcpy(buffer_,old,l);}
		delete [] old;}
	if(s!=0) {memcpy(buffer_,s,l);}
	return this;}

PvOctets& PvOctets::operator=(const PvOctets& o) {
	if(this!=&o) {
		set(o.length(),(OCTSTR)o.string());}
	return *this;}

PvOctets* PvOctets::substr(uint32_t n,uint32_t l,PvOctets* o) {
	OCTSTR s=buffer(n);
	if(o==0) {return new PvOctets(l,s);}
	return o->set(l,s);}

//======================================================================
//         at: position
//         bytes
//         |  bits
//         |  |
//         |  | w:indicate width
//         |  <-(w)->
//         |  |      |
//        (o)(b)    (e)
//         v  v      v
// +--//---0-(d0)--8-(d1)--+-------+-------+
//         first   second
//
//----------------------------------------------------------------------
// Left Adjusted value(Octets)
//      n:
//      <(w)->
//      +-------
// mask 11111100 = (2^w-1)<<(8-w)
//
void PvOctets::encodeLA(octet n,const ItPosition& at,uint16_t w) {
	if(w>8) {abort();}
	OCTSTR dst=buffer();
	uint32_t o=at.bytes();			// octet offset
	uint16_t b=at.bits();			// bits offset
	uint16_t e=b+w;				// bits end offset
	octet& d0=dst[o];			// first destination
	octet m=((1<<w)-1)<<(8-w);		// mask for efficent width
	octet nm=(n&m);				// masked network value
	octet t=0, sm=0, rm=0;
	t=nm>>b; sm=m>>b; rm=~sm;
	d0=(t&sm)|(d0&rm);
	if(e<8) {return;}		// It's fit in one octet !!
	octet& d1=dst[o+1];		// second destination
	uint16_t s=8-b;
	t=nm<<s; sm=m<<s; rm=~sm;
	d1=(t&sm)|(d1&rm);}

octet PvOctets::decodeLA(const ItPosition& at,uint16_t w) const {
	if(w>8) {abort();}
	COCTSTR src=string();
	uint32_t o=at.bytes();			// octet offset
	uint16_t b=at.bits();			// bits offset
	uint16_t e=b+w;				// bits end offset
	const octet& d0=src[o];			// first source
	octet m=((1<<w)-1)<<(8-w);		// mask for efficent width
	octet t=0;				// target octet
	octet sm=0;				// shifted mask
	if(e<8) {			// It's fit in one octet !!
		sm=m>>b;
		t=(d0&sm)<<b;}
	else {
		sm=m>>b;
		uint16_t r=8-b;
		octet rm=m<<r;
		const octet& d1=src[o+1];	// second source
		t=((d0&sm)<<b)|((d1&rm)>>r);}
	return t;}

//----------------------------------------------------------------------
// Right Adjusted value(Numbers)
//             n:
//        <-(w)>
//      +-------
// mask 00111111 = (2^w-1)
//
void PvOctets::encodeRA(octet n,const ItPosition& at,uint16_t w) {
	if(w>8) {abort();}
	OCTSTR dst=buffer();
	uint32_t o=at.bytes();			// octet offset
	uint16_t e=at.bits()+w;			// bits end offset
	octet& d0=dst[o];			// first destination
	octet m=(1<<w)-1;			// mask for efficent width
	octet nm=(n&m);				// masked network value
	uint16_t s=0;				// shift width
	octet t=0, sm=0, rm=0;
	if(e<8) {			// It's fit in one octet !!
		s=8-e; t=nm<<s; sm=m<<s; rm=~sm;
		d0=(t&sm)|(d0&rm);}
	else {
		s=e-8; t=nm>>s; sm=m>>s; rm=~sm;
		d0=(t&sm)|(d0&rm);
		octet& d1=dst[o+1];		// second destination
		s=8-s; t=nm<<s; sm=m<<s; rm=~sm;
		d1=(t&sm)|(d1&rm);}}

octet PvOctets::decodeRA(const ItPosition& at,uint16_t w) const {
	if(w>8) {abort();}
	COCTSTR src=string();
	uint32_t o=at.bytes();			// octet offset
	uint16_t e=at.bits()+w;			// bits end offset
	const octet& d0=src[o];			// first source
	octet m=(1<<w)-1;			// mask for efficent width
	uint16_t s=0;				// shift width
	octet t=0;				// target octet
	octet sm=0;				// shifted mask
	if(e<8) {			// It's fit in one octet !!
		s=8-e; sm=m<<s;
		t=(d0&sm)>>s;}
	else {
		s=e-8; sm=m>>s;
		uint16_t r=8-s;
		octet rm=m<<r;
		const octet& d1=src[o+1];	// second source
		t=((d0&sm)<<s)|((d1&rm)>>r);}
	return t;}

//----------------------------------------------------------------------
void PvOctets::encodeLAs(COCTSTR np,const ItPosition& at,uint16_t w) {
	OCTSTR dst=buffer();
	uint16_t cw=0;				// current bit width
	ItPosition it(at);
	for(;w>0;w-=cw,np++) {
		cw=w>8?8:w;
		if(cw==8&&it.bits()==0) {
			dst[it.bytes()]=*np;}
		else {
			encodeLA(*np,it,cw);}
		it.addBits(cw);}}

void PvOctets::decodeLAs(OCTSTR np,const ItPosition& at,uint16_t w) const {
	COCTSTR src=string();
	uint16_t cw=0;				// current bit width
	ItPosition it(at);
	for(;w>0;w-=cw,np++) {
		cw=w>8?8:w;
		if(cw==8&&it.bits()==0) {
			*np=src[it.bytes()];}
		else {
			*np=decodeLA(it,cw);}
		it.addBits(cw);}}

//----------------------------------------------------------------------
void PvOctets::encodeRAs(COCTSTR np,const ItPosition& at,uint16_t w) {
	OCTSTR dst=buffer();
	uint16_t cw=0;				// current bit width
	ItPosition it(at);
	for(;w>0;w-=cw,np++) {
		cw=(w-1)%8+1;
		if(cw==8&&it.bits()==0) {
			dst[it.bytes()]=*np;}
		else {
			encodeRA(*np,it,cw);}
		it.addBits(cw);}}


void PvOctets::decodeRAs(OCTSTR np,const ItPosition& at,uint16_t w) const {
	COCTSTR src=string();
	uint16_t cw=0;				// current bit width
	ItPosition it(at);
	for(;w>0;w-=cw,np++) {
		cw=(w-1)%8+1;
		if(cw==8&&it.bits()==0) {
			*np=src[it.bytes()];}
		else {
			*np=decodeRA(it,cw);}
		it.addBits(cw);}}

//----------------------------------------------------------------------
// Right Adjusted value(Numbers)
//                                     n:
//                   <-----(w)--------->
//      +-------+-------+-------+-------
// mask 00000000000001111111111111111111 = (2^w-1)
//
void PvOctets::encodeNUint(uint32_t n,const ItPosition& at,uint16_t w) {
	uint16_t uo=(32-w)/8;			// offset in n(uint32_t)
	COCTSTR np=((OCTSTR)&n)+uo;
	encodeRAs(np,at,w);}

void PvOctets::encodeUint(uint32_t h,const ItPosition& at,uint16_t w) {
	encodeNUint(htonl(h),at,w);}

uint32_t PvOctets::decodeNUint(const ItPosition& at,uint16_t w) const {
	uint32_t n=0;
	uint16_t uo=(32-w)/8;			// offset in n(uint32_t)
	OCTSTR np=((OCTSTR)&n)+uo;
	decodeRAs(np,at,w);
	return n;}

uint32_t PvOctets::decodeUint(const ItPosition& at,uint16_t w) const {
	return ntohl(decodeNUint(at,w));}

//----------------------------------------------------------------------
void PvOctets::encode(const ItPosition& at,const PvOctets& o) {
	COCTSTR s=o.string();
	uint32_t l=o.length();
	if(at.bits()>0) {encodeLAs(s,at,l*8);}
	else {memcpy(buffer(at.bytes()),s,l);}}

void PvOctets::encodeZero(const ItPosition& at,const ItPosition& size) {
	ItPosition offset = at;
	uint32_t len = size.bytes();
	if(len>0){//bytes zero
		PvOctets zerobuf(len); zerobuf.zero();
		encode(offset,zerobuf);
		offset.addBytes(len);}
	uint16_t bitlen = size.bits();
	if(bitlen>0){//remain bits zero
		uint32_t zerobits = 0;
		encodeUint(zerobits,offset,bitlen);}
	}

int32_t PvOctets::compareWidth(const PvOctets& r,uint32_t w) const {
	int32_t ow=w/8;
	int32_t bw=w%8;
	octet m=((1<<bw)-1)<<(8-bw);		// mask for efficent width
	COCTSTR ls=string();
	COCTSTR rs=r.string();
	int x=memcmp(ls,rs,ow);
	if(x==0&&bw>0) {x=(ls[ow]&m)-(rs[ow]&m);}
	return x;}

int32_t PvOctets::compareOctets(const PvOctets& r) const {
	int32_t ll=length();
	int32_t rl=r.length();
	int32_t ml=ll<rl?ll:rl;
	COCTSTR ls=string();
	COCTSTR rs=r.string();
	int32_t x=memcmp(ls,rs,ml);
//	printf("%d=memecmp(",x); print(); printf(","); r.print(); printf(",%d);\n",ml);
	return x!=0?x:ll-rl;}

int32_t PvOctets::compareObject(const PObject& r) const {
	return -(r.compareOctets(*this));}

bool PvOctets::generate(WControl& c,WObject* w,OCTBUF& v) const {
	w->encodeOctets(c,v,*this);
	return c;}

uint32_t PvOctets::sum1Octet() const {
	uint32_t t=0;
	uint32_t l=length();
	COCTSTR s=string(), e;
	for(e=s+l;s<e;s++) {
		t+=*s;}
	return t;}

uint32_t PvOctets::sum2Octets() const {
	uint32_t t=0, n=0;
	uint32_t i,l=length();
	COCTSTR s=string();
	for(i=0;i+1<l;i+=2,s+=2) {
		n=s[0]<<8|s[1];
		t+=n;}
	if(l!=i) {
		n=s[0]<<8;
		t+=n;}
	return t;}

uint32_t PvOctets::sum4Octets() const {
	uint32_t t=0, n=0;
	uint32_t i,l=length();
	COCTSTR s=string();
	for(i=0;i+3<l;i+=4,s+=4) {
		n=s[0]<<24|s[1]<<16|s[2]<<8|s[3];
		t+=n;}
	n=0;
	switch(l-i) {
		case 3: n=s[0]<<24|s[1]<<16|s[2]<<8;	break;
		case 2: n=s[0]<<24|s[1]<<16;		break;
		case 1: n=s[0]<<24;			break;}
	t+=n;
	return t;}

void PvOctets::zero() {
	memset(buffer(),0x00,length());}
void PvOctets::fill() {
	memset(buffer(),0xff,length());}

void PvOctets::dump(CSTR tag) const {
	int i, i9=length();
	COCTSTR s=string();
	CSTR nl=tag!=0?tag:"\n";
	CSTR pad=(tag!=0&&length()>16)?tag:"";
	for(i=0;i<i9;i++) {
		printf("%s%02x",pad,s[i]&0xff);
		switch((i%32)+1) {
			case 4: case 12: case 20: case 28:	pad=" "; break;
			case 8:	case 16: case 24:		pad="  "; break;
			case 32:				pad=nl; break;
			default:				pad=""; break;}}}

void PvOctets::print() const {dump(length()>16?0:"");}

void PvOctets::log(uint32_t t) const {
	CmCString s("\nlog:");
	for(uint32_t i=0;i<t;i++) {s += "| ";}
	s += "  ";
	dump(s.string());}
void PvV4Addr::log(uint32_t t) const {print();}
void PvV6Addr::log(uint32_t t) const {print();}
void PvEther::log(uint32_t t) const {print();}

void PvV4Addr::print() const {
	char tmp[sizeof "255.255.255.255"];
	inet_ntop(AF_INET,string(),tmp,sizeof(tmp));
	printf("%s",tmp);}

void PvEther::print() const {
	printf("%s",ether_ntoa((ether_addr*)string()));}

void PvV6Addr::print() const {
	char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"];
	inet_ntop(AF_INET6,string(),tmp,sizeof(tmp));
	printf("%s",tmp);}

const PvEther* PvEther::TN() {
	PvIfName* tn=defaultTN();
	return tn->ether();}
const PvEther* PvEther::NUT() {
	PvIfName* tn=defaultNUT();
	return tn->ether();}
const PvV6Addr* PvV6Addr::TN() {
	PvIfName* tn=defaultTN();
	return tn->v6addr();}
const PvV6Addr* PvV6Addr::NUT() {
	PvIfName* tn=defaultNUT();
	return tn->v6addr();}

const PvV6Addr& PvV6Addr::linkLocal() {
	if(linkLocal_==0) {
		bool ok=false;
		linkLocal_=new PvV6Addr(linkLocalPrefixPresentation_,ok);}
	return *linkLocal_;}

bool PvV6Addr::netMerge(CSTR net,int len,const PvV6Addr& hostAddr) {
	bool ok=true;
	PvV6Addr netAddr(net,ok);	if(!ok) {return false;}
	if(len<0||len>128) {return false;}
	netAddr.merge(len,hostAddr,this);
	return true;}

PvIfName* PvOctets::defaultTN() {
	if(defaultTN_==0) {defaultTN_=PvIfName::findTn();}
	return defaultTN_;}
PvIfName* PvOctets::defaultNUT() {
	if(defaultNUT_==0) {defaultNUT_=PvIfName::findNut();}
	return defaultNUT_;}

void PvOctets::defaultNotUsed() {
	PvIfName* p=new PvIfName("local");
	defaultTN_=p;
	defaultNUT_=p;}

bool PvOctets::isOctets() const {return true;}

int32_t PvOctets::addressFamily() const {return AF_UNSPEC;}
int32_t PvV4Addr::addressFamily() const {return AF_INET;}
int32_t PvV6Addr::addressFamily() const {return AF_INET6;}

PvIfName* PvOctets::defaultTN_=0;
PvIfName* PvOctets::defaultNUT_=0;
PvV6Addr* PvV6Addr::linkLocal_=0;
implementCmList(PvOctetsList,PvOctets);
const CSTR PvOctets::linkLocalPrefixPresentation_="fe80::";

////////////////////////////////////////////////////////////////
PvCookie64::PvCookie64(OCTSTR o, bool b):PvOctets(8, o, b) {}
PvCookie64::~PvCookie64() {}

void PvCookie64::log(uint32_t t) const {
	print();
}

void PvCookie64::print() const {
	COCTSTR buf = string();
	uint32_t buflen = length(), d = 0;

	for(d = 0; d < buflen; d ++) {
		printf("%02x", buf[d]);
	}
}

const PvCookie64 *PvCookie64::zerocookie() {
	char tmp[8];

	memset(tmp, 0, 8);

	return(new PvCookie64((OCTSTR)tmp, true));
}

PvObject *PvCookie64::shallowCopy() const {
	return(new PvCookie64((OCTSTR)string()));
}

////////////////////////////////////////////////////////////////
PvBSA96::PvBSA96(OCTSTR o, bool b):PvOctets(12, o, b) {}
PvBSA96::~PvBSA96() {}

void PvBSA96::log(uint32_t t) const {
	print();
}

void PvBSA96::print() const {
	COCTSTR buf = 0;
	uint32_t buflen = 0, d = 0;

	buf = string();
	buflen = length();

	for(d = 0; d < buflen; d ++) {
		printf("%02x", buf[d]);
	}
}

const PvBSA96 *PvBSA96::zerobsa() {
	char tmp[12];

	memset(tmp, 0, 12);

	return(new PvBSA96((OCTSTR)tmp, true));
}

PvObject *PvBSA96::shallowCopy() const {
	return(new PvBSA96((OCTSTR)string()));
}

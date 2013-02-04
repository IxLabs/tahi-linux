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
 * $TAHI: v6eval/lib/Pz/MmObject.cc,v 1.58 2005/05/09 09:35:24 akisada Exp $
 */
#include "MmObject.h"
#include "PObject.h"
#include "PlObject.h"
#include "PvOctets.h"
#include "WObject.h"
#include "ItPosition.h"
#include "PControl.h"
#include <stdio.h>

//======================================================================
MmObject::MmObject(CSTR s):MObject(s),compound_(0) {}
MmObject::~MmObject() {}
int32_t MmObject::token() const {return metaToken(tkn_member_);}
PObject* MmObject::tokenObject(int l,CSTR f) const {
	return new PlObject(this,f,l);}
void MmObject::compound(MObject* c) {compound_=(McObject*)c;}
McObject* MmObject::compound() const {return compound_;}

//======================================================================
#define SUPER	MmObject

MmDecorator::MmDecorator(MmObject* body):SUPER(0),body_(0){
	set_body(body);}
MmDecorator::~MmDecorator(){
	if(body_)delete body_;}
void MmDecorator::set_body(MmObject* body){
	if(body_){
		delete body_;
		SUPER::set(0,0);
		SUPER::compound(0);}
	body_=body;
	if(body_){
		SUPER::operator=(*body);
		SUPER::compound(body->compound());}
	}

//decorate from CmToken
void MmDecorator::print() const{
	body_?	body_->print() : SUPER::print();}
int32_t MmDecorator::token()const{
	return body_?	body_->token() : SUPER::token();}
PObject* MmDecorator::tokenObject(int l,CSTR f)const{
	return body_?	body_->tokenObject(l,f) : SUPER::tokenObject(l,f);}

//decorate from MObject
void MmDecorator::postProcess(void* v){
	body_? body_->postProcess(v) : SUPER::postProcess(v);}
void MmDecorator::stepPostion(ItPosition& at) const{
	body_? body_->stepPostion(at) : SUPER::stepPostion(at);}
uint32_t MmDecorator::objectLength(const PObject* po,const WObject* w)const{
	return body_? body_->objectLength(po,w):SUPER::objectLength(po,w);}
uint16_t MmDecorator::width() const{
	return body_?	body_->width() : SUPER::width();}
MObject* MmDecorator::findMember(CSTR str) const{
	return body_?	body_->findMember(str) : SUPER::findMember(str);}

bool MmDecorator::generate(WControl& c,WObject* w,OCTBUF& buf) const{
	return body_?	body_->generate(c,w,buf) : SUPER::generate(c,w,buf);}
bool MmDecorator::geneAuto(WControl& c,WObject* w,OCTBUF& buf) const{
	return body_?	body_->geneAuto(c,w,buf) : SUPER::geneAuto(c,w,buf);}
bool MmDecorator::geneFix(WControl& c,WObject* w,OCTBUF& buf) const{
	return body_?	body_->geneFix(c,w,buf) : SUPER::geneFix(c,w,buf);}
bool MmDecorator::encodeNumber(WControl& c,
		const ItPosition& at,OCTBUF& buf,const PvNumber& pv) const{
	return body_?	body_->encodeNumber(c,at,buf,pv) :
			SUPER::encodeNumber(c,at,buf,pv);}
	
bool MmDecorator::encodeOctets(WControl& c,
		const ItPosition& at,OCTBUF& buf,const PvOctets& pv) const{
	return body_?	body_->encodeOctets(c,at,buf,pv) :
			SUPER::encodeOctets(c,at,buf,pv);}

PvObject* MmDecorator::geneAutoValue(WObject* w) const{
	return body_?	body_->geneAutoValue(w) : SUPER::geneAutoValue(w);}
PvObject* MmDecorator::evalAutoValue(WObject* w) const{
	return body_?	body_->evalAutoValue(w) : SUPER::evalAutoValue(w);}
bool MmDecorator::overwrite_ICV(ICVControl& c,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const{
	return body_?	body_->overwrite_ICV(c,at,buf,t) :
			SUPER::overwrite_ICV(c,at,buf,t);}

bool MmDecorator::disused() const {
	return body_?	body_->disused() : SUPER::disused();}
bool MmDecorator::isEqual(const MObject* m) const {
	bool rtn = SUPER::isEqual(m);
	return !rtn&&body_ ? body_->isEqual(m) : rtn;}
bool MmDecorator::containsMc(const MObject* m) const {
	bool rtn = SUPER::containsMc(m);
	return !rtn&&body_ ? body_->containsMc(m) : rtn;}
bool MmDecorator::comparableMeta(const MObject* m) const {
	bool rtn = SUPER::comparableMeta(m);
	return !rtn&&body_ ? body_->comparableMeta(m) : rtn;}

int32_t MmDecorator::DataLinkLayer_Type() const{
	return body_?	body_->DataLinkLayer_Type() :
			SUPER::DataLinkLayer_Type();}
int32_t MmDecorator::protocolFamily() const{
	return body_?	body_->protocolFamily() : SUPER::protocolFamily();}
int32_t MmDecorator::protocolType() const{
	return body_?	body_->protocolType() : SUPER::protocolType();}
int32_t MmDecorator::headerType() const{
	return body_?	body_->headerType() : SUPER::headerType();}
int32_t MmDecorator::optionType() const{
	return body_?	body_->optionType() : SUPER::optionType();}
int32_t MmDecorator::optionCode() const{
	return body_?	body_->optionCode() : SUPER::optionCode();}
int32_t MmDecorator::icmpv6Type() const{
	return body_?	body_->icmpv6Type() : SUPER::icmpv6Type();}
int32_t MmDecorator::mobilityType() const{
	return body_?	body_->mobilityType() : SUPER::mobilityType();}
int32_t MmDecorator::vrrpVersion() const{
	return body_?	body_->vrrpVersion() : SUPER::vrrpVersion();}
bool MmDecorator::ISAKMP_encryption() const{
	return body_?	body_->ISAKMP_encryption() : SUPER::ISAKMP_encryption();}
int32_t MmDecorator::rrType() const{
	return body_?	body_->rrType() : SUPER::rrType();}
int32_t MmDecorator::hardwareType() const{
	return body_?	body_->hardwareType() : SUPER::hardwareType();}
int32_t MmDecorator::authProto() const{
	return body_?	body_->authProto() : SUPER::authProto();}
int32_t MmDecorator::icmpv4Type() const{
	return body_?	body_->icmpv4Type() : SUPER::icmpv4Type();}
int32_t MmDecorator::igmpType() const{
	return body_?	body_->igmpType() : SUPER::igmpType();}

int32_t MmDecorator::upperPort() const {
	return(body_ ? body_->upperPort() : SUPER::upperPort());
}

int32_t MmDecorator::dhcpv6Type() const {
	return(body_ ? body_->dhcpv6Type() : SUPER::dhcpv6Type());
}

int32_t MmDecorator::DOIvalue() const {
	return(body_ ? body_->DOIvalue() : SUPER::DOIvalue());
}

int32_t MmDecorator::notify_value() const {
	return(body_ ? body_->notify_value() : SUPER::notify_value());
}

int32_t MmDecorator::ProtocolID() const {
	return(body_ ? body_->ProtocolID() : SUPER::ProtocolID());
}

int32_t MmDecorator::IDtype() const {
	return(body_ ? body_->IDtype() : SUPER::IDtype());
}

int32_t MmDecorator::SITvalue() const {
	return(body_ ? body_->SITvalue() : SUPER::SITvalue());
}

int32_t MmDecorator::duidType() const {
	return(body_ ? body_->duidType() : SUPER::duidType());
}

#if !defined(NOT_USE_SNMP)
int32_t MmDecorator::pduType() const {
	return(body_ ? body_->pduType() : SUPER::pduType());
}
#endif //NOT_USE_SNMP

const TypevsMcDict* MmDecorator::get_dict() const {
	return body_ ?	body_->get_dict(): SUPER::get_dict();}
uint32_t MmDecorator::alignment_requirement() const {
	return body_ ?	body_->alignment_requirement():
			SUPER::alignment_requirement();}

bool MmDecorator::icvUpdate(const PaObject& pa,PvOctets* pv) const{
	return body_?	body_->icvUpdate(pa,pv) : SUPER::icvUpdate(pa,pv);}

WObject* MmDecorator::compose(WControl& c,
		WObject* w_parent,const PObject* po) const{
	return body_?	body_->compose(c,w_parent,po) :
			SUPER::compose(c,w_parent,po);}
RObject* MmDecorator::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf) const{
	return body_?	body_->reverse(c,r_parent,at,buf) :
			SUPER::reverse(c,r_parent,at,buf);}
RObject* MmDecorator::rreverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf) const{
	return body_?	body_->rreverse(c,r_parent,at,buf) :
			SUPER::rreverse(c,r_parent,at,buf);}
void MmDecorator::composeList(WControl& c,
                WObject* w_parent,const PObjectList& pls) const{
	body_?	body_->composeList(c,w_parent,pls) :
		SUPER::composeList(c,w_parent,pls);}
	
void MmDecorator::describe(const PObject* po) const{
	body_?	body_->describe(po) : SUPER::describe(po);}

//decorate from MmObject
void MmDecorator::compound(MObject* m) {
	SUPER::compound(m); if(body_)body_->compound(m);}
McObject* MmDecorator::compound() const {
	return body_? body_->compound() : SUPER::compound();}

#undef SUPER

//======================================================================
//more than 0
MmMultiple::MmMultiple(MmObject* e,METH_HC_MLC meth):MmDecorator(e),
	METH_HC_MLC_(meth) {}
MmMultiple::~MmMultiple() {}

// COMPOSE
void MmMultiple::composeList(WControl& c,
		WObject* w_parent,const PObjectList& pls)const{
	c.dict_set(0);	//clear composeable Mc type dict
	MmObject* mbody = body();
	pls.elementsPerformWith(
		(PObjectFunc)&PObject::vmatchselfCompose,&c,w_parent,mbody);
	}

// REVERSE
RObject* MmMultiple::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf) const {
	RObject* r=0;
	MmObject* mbody = body();
	uint32_t cou = (compound()->*METH_HC_MLC_)(ItPosition(),buf);
	for(uint32_t i=0;i<cou;i++){
		r = mbody->reverse(c,r_parent,at,buf);}//forward
	return r;}


//======================================================================
#define SUPER	MmObject
MmElement::MmElement(CSTR s,
	const PObject* g,const PObject* e,const ICVoverwriter* ow,
	METH_HC_ForIPinfo meth):SUPER(s),
	type_(0),offset_(0),generater_(g),evaluater_(e),ICVoverwriter_(ow),
	METH_HC_ForIPinfo_(meth) {}
MmElement::~MmElement() {}

bool MmElement::geneAuto(WControl& c,WObject* w,OCTBUF& s) const {
	const PObject* g=generater();
	return (g!=0)?g->generate(c,w,s):!c.error();}

PvObject* MmElement::geneAutoValue(WObject* w) const {
	const PObject* g=generater();
	return (g!=0)?g->generateValue(w):0;}

PvObject* MmElement::evalAutoValue(WObject* w) const {
	const PObject* e=evaluater();
	return (e!=0)?e->evaluateValue(w):0;}

bool MmElement::overwrite_ICV(ICVControl& c,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const{
	return ICVoverwriter_? ICVoverwriter_->overwrite(c,at,buf,t) :true;}

void MmElement::stepPostion(ItPosition& it) const {
	it.addBits(width());
	it.addBytes(objectLength());}

void MmElement::postProcess(void* v) {
	ItPosition& it=*(ItPosition*)v;
	offset(it.bytes());
	position(it.bits());
	stepPostion(it);}

void MmElement::print() const {
	uint32_t o=offset();
	uint16_t b=position();
	printf("%-20s\tbytes=%2d bits=%2d width=%2d length=%2d offset=%2d\n",
		string(),o,b,width(),objectLength(),o+b/8);}

// COMPOSE
void MmElement::composeList(WControl& c,
		WObject* w_parent,const PObjectList& pls)const{
	const PObject* pl =
		pls.reverseMatching(this,(PObjectEqFunc)&PObject::isEqualMeta);
	pl ? pl->selfCompose(c,w_parent) : compose(c,w_parent,0);}

WObject* MmElement::compose(WControl& c,
		WObject* w_parent,const PObject* pl)const{
	const TypevsMcDict* keep = c.dict();
	c.dict_set(0);	//clear composeable Mc type dict
	WObject* wm = composeWm(c,w_parent,pl);	//compose leaf Element
	c.dict_set(keep);
	return wm;}

WObject* MmElement::composeWm(WControl&,
		WObject* w_parent,const PObject* pl)const{
	return new WmObject(w_parent,this,pl);}

// REVERSE
RObject* MmElement::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf) const {
	const MObject* m_parent = r_parent ? r_parent->meta() : 0;
	ItPosition size = length_for_reverse(c,at,buf);
	if(!check_decode_limit(m_parent,at,buf,size)){
		c.result(1); return 0;}
	PvObject* pv = reversePv(c,at,size,buf);
	RObject* r_self = reverseRm(c,r_parent,at,size,pv);
	at+=size;
	if(METH_HC_ForIPinfo_){// setting for IPinfo
		Con_IPinfo* info = c.IPinfo();
		if(r_self&&info){
			(compound()->*METH_HC_ForIPinfo_)(*info,r_self);}}
	return r_self;}

ItPosition MmElement::length_for_reverse(RControl&,ItPosition&,OCTBUF&) const{
	ItPosition size; stepPostion(size);
	return size;}

PvObject* MmElement::reversePv(RControl&,
		const ItPosition&,const ItPosition&,const OCTBUF&)const{
	return 0;}
RObject* MmElement::reverseRm(RControl&,RObject* r_parent,
		const ItPosition& at,const ItPosition& size,PvObject* pv)const{
	return new RmObject(r_parent,this,at,size,pv);}

//GENERATE
bool MmElement::generate(WControl& c,WObject* w_self,OCTBUF& buf)const{
	bool rtn = SUPER::generate(c,w_self,buf);
	if(METH_HC_ForIPinfo_){//setting for IPinfo
		Con_IPinfo* info = c.IPinfo();
		if(!c.error()&&info){
			(compound()->*METH_HC_ForIPinfo_)(*info,w_self);}}
	return rtn;}

//======================================================================
MmUint::MmUint(CSTR s,uint16_t n,const PObject* g,const PObject* e,
	const ICVoverwriter* ow,METH_HC_ForIPinfo meth):
	MmElement(s,g,e,ow,meth),width_(n),position_(0) {}
MmUint::~MmUint() {}

MmHostUint::MmHostUint(CSTR s,uint16_t n,const PObject* g,const PObject* e,
	const ICVoverwriter* ow,METH_HC_ForIPinfo meth):
	MmUint(s,n,g,e,ow,meth) {}
MmHostUint::~MmHostUint() {}


//----------------------------------------------------------------------
void MmUint::encode(uint32_t h,const ItPosition& at,OCTBUF& dst) const {
	uint16_t w=width();
	if(w>32) {abort();}
	dst.encodeUint(h,at,w);}

void MmHostUint::encode(uint32_t h,const ItPosition& at,OCTBUF& dst) const {
	uint16_t w=width();
	if(w>32) {abort();}
	dst.encodeNUint(h,at,w);}

bool MmUint::encodeNumber(WControl&,const ItPosition& at,OCTBUF& dst,const PvNumber& n) const {
	encode(n.value(),at,dst);
	return true;}

//----------------------------------------------------------------------
uint32_t MmUint::rdecode(ItPosition& at,const OCTBUF& src,bool& ok) const {
	ItPosition ml(objectLength(),width());
	if(at<ml) {ok=false; return 0;}
	ok=true;
	at-=ml;
	return decode(at,src);}
uint32_t MmUint::decode(const ItPosition& at,const OCTBUF& src) const {
	uint16_t w=width();
	if(w>32) {abort();}
	return src.decodeUint(at,w);}

uint32_t MmHostUint::decode(const ItPosition& at,const OCTBUF& src) const {
	uint16_t w=width();
	if(w>32) {abort();}
	return src.decodeNUint(at,w);}

uint32_t MmUint::value(const ItPosition& on,const OCTBUF& src) const {
	ItPosition at(on);
	ItPosition delta(offset(),position());
	at+=delta;
	return decode(at,src);}

// REVERSE
PvObject* MmUint::reversePv(RControl&,
		const ItPosition& at,const ItPosition&,const OCTBUF& buf)const{
	uint32_t val = decode(at,buf);
	return new PvNumber(val);}

//======================================================================
MmVarOctets::MmVarOctets(CSTR s,const PObject* g,const PObject* e,
	const ICVoverwriter* ow,METH_HC_ForIPinfo meth):
	MmElement(s,g,e,ow,meth){}
MmVarOctets::~MmVarOctets() {}

uint32_t MmVarOctets::objectLength(const PObject* o,const WObject* w) const {
        return o!=0?o->objectLength(w):0;}

bool MmVarOctets::encodeOctets(WControl&,
		const ItPosition& at,OCTBUF& dst,const PvOctets& elm) const {
	dst.encode(at,elm);
	return true;}
bool MmVarOctets::encodeNumber(WControl&,
		const ItPosition& at,OCTBUF& dst,const PvNumber& n)const{
	static const int bytewidth = 8;
	dst.encodeUint(n.value(),at,bytewidth);
        return true;}

ItPosition MmVarOctets::length_for_reverse(RControl&,
		ItPosition& at,OCTBUF& buf) const{
	uint32_t length = buf.remainLength(at.bytes());
	return ItPosition(length,0);}//default is remain all

PvObject* MmVarOctets::reversePv(RControl&,const ItPosition& at,
		const ItPosition& size,const OCTBUF& buf)const{
	uint32_t offset=at.bytes(),length=size.bytes();
	return new PvOctets(length,(OCTSTR)buf.string(offset));}

//======================================================================
MmOctets::MmOctets(CSTR s,uint32_t l,const PObject* g,const PObject* e,
	const ICVoverwriter* ow,METH_HC_ForIPinfo meth):
	MmElement(s,g,e,ow,meth),length_(l) {}
MmOctets::~MmOctets() {}

bool MmOctets::encodeOctets(WControl&,const ItPosition& at,OCTBUF& dst,const PvOctets& elm) const {
	dst.encode(at,elm);
	return true;}

//======================================================================
MmV4Addr::MmV4Addr(CSTR s,const PObject* g,const PObject* e,
	const ICVoverwriter* ow,METH_HC_ForIPinfo meth):
	MmOctets(s,sizeof(v4addr),g,e,ow,meth) {}
MmV4Addr::~MmV4Addr() {}

// REVERSE
PvObject* MmV4Addr::reversePv(RControl&,
		const ItPosition& at,const ItPosition&,const OCTBUF& buf)const{
	OCTSTR str = (OCTSTR)buf.string(at.bytes());
	return new PvV4Addr(str);}

//======================================================================
MmV6Addr::MmV6Addr(CSTR s,const PObject* g,const PObject* e,
	const ICVoverwriter* ow,METH_HC_ForIPinfo meth):
	MmOctets(s,sizeof(v6addr),g,e,ow,meth) {}
MmV6Addr::~MmV6Addr() {}

// REVERSE
PvObject* MmV6Addr::reversePv(RControl&,
		const ItPosition& at,const ItPosition&,const OCTBUF& buf)const{
	OCTSTR str = (OCTSTR)buf.string(at.bytes());
	return new PvV6Addr(str);}

//======================================================================
MmEther::MmEther(CSTR s,const PObject* g,const PObject* e,
	const ICVoverwriter* ow,METH_HC_ForIPinfo meth):
	MmOctets(s,sizeof(etheraddr),g,e,ow,meth) {}
MmEther::~MmEther() {}

// REVERSE
PvObject* MmEther::reversePv(RControl&,
		const ItPosition& at,const ItPosition&,const OCTBUF& buf)const{
	OCTSTR str = (OCTSTR)buf.string(at.bytes());
	return new PvEther(str);}


///////////////////////////////////////////////////////////////////////////////

bool ICVoverwriter::overwrite(ICVControl&,
		const ItPosition&,OCTBUF&,const TObject*)const{
	return true;}

bool ICV_Zero::overwrite(ICVControl&,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const{
	buf.encodeZero(at,t->size());
	return true;}

bool ICV_HC::overwrite(ICVControl& c,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const{
	return (meta_->*METH_HC_OWICV_)(c,at,buf,t);}

//////////////////////////////////////////////////////////////////////////////


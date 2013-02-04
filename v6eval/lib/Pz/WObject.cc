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
 * $TAHI: v6eval/lib/Pz/WObject.cc,v 1.68 2003/12/04 04:59:47 akisada Exp $
 */
#include "WObject.h"
#include "MmObject.h"
#include "MmChecksum.h"
#include "PObject.h"
#include "PControl.h"
#include "PvOctets.h"
#include "ItPosition.h"
#include "CmMain.h"
#include <stdio.h>
//----------------------------------------------------------------------
const PObject* WObject::rgenerate() const {return object();}
const PObject* WmObject::rgenerate() const {return rgenerate_;}
const PObject* WcObject::rgenerate() const {return rgenerate_;}
const PObject* WvSubstr::rgenerate() const {return substr_;}

//----------------------------------------------------------------------
bool WObject::encodeNumber(WControl& c,OCTBUF& b,const PvNumber& p) {
	const MObject* m=meta();	// m==0 ????????
	if(m!=0) {m->encodeNumber(c,offset(),b,p);}
	return (m!=0);}

bool WObject::encodeOctets(WControl& c,OCTBUF& b,const PvOctets& p) {
	const MObject* m=meta();	// m==0 ????????
	if(m!=0) {m->encodeOctets(c,offset(),b,p);}
	return (m!=0);}

bool WvObject::encodeNumber(WControl&,OCTBUF& b,const PvNumber& p) {
	b.encodeUint(p.value(),offset(),8);
	return true;}

bool WvObject::encodeOctets(WControl&,OCTBUF& b,const PvOctets& p) {
	b.encode(offset(),p);
	return true;}

void WObject::algorithm(const PaObject*) {}
//----------------------------------------------------------------------
inline void WObject::stepPostion(ItPosition& it) {
	ItPosition size(length(),width());
	it.add(size);}

void WObject::setSize(const ItPosition& it) {
	const ItPosition& o=offset();
	ItPosition& size=size_;
	if(o==it) {
		ItPosition s(length(),width());
		size=s;}
	else {
		size=it;
		size.subtract(o);}}

uint32_t WObject::distance() const {
	WObject* p=parent();
	if(!p)return offset().bytes();
	if(p==0 && DBGFLAGS('a')) {
		printf("=%s cannot calculate distance\n",metaString());
		return 0;}
	ItPosition at(offset());
	at-=p->offset();
	uint16_t b=at.bits();
	if(b!=0 && DBGFLAGS('a')) {
		printf("=%s bits remains %d\n",metaString(),b);}
	return at.bytes();}

//----------------------------------------------------------------------
PvObject* WObject::geneAutoValue() {
	const MObject* m=meta();
	return (m!=0)?m->geneAutoValue(this):0;}

PvObject* WObject::evalAutoValue() {
	const MObject* m=meta();
	return (m!=0)?m->evalAutoValue(this):0;}

//----------------------------------------------------------------------
bool WObject::geneAuto(WControl& c,OCTBUF& s) {
	const MObject* m=meta();
	if(m!=0) {m->geneAuto(c,this,s);}
	return c;}

bool WObject::geneFix(WControl& c,OCTBUF& s) {
	const MObject* m=meta();
	if(m!=0) {m->geneFix(c,this,s);}
	return c;}

bool WObject::generate(WControl& c,OCTBUF& s) {
	const MObject* m=meta();
	if(m!=0) {return m->generate(c,this,s);} //tranfer to Meta
	// not has a meta
	const PObject* o=object();
	return (o!=0)?o->generate(c,this,s):geneAuto(c,s);}

bool WtNode::generate(WControl& c,OCTBUF& s) {
	return WObject::generate(c,s);}

bool WmObject::generate(WControl& c,OCTBUF& s) {
	return WtNode::generate(c,s);}

bool WcObject::generate(WControl& c,OCTBUF& s) {
	uint32_t sl=s.length();
	uint32_t o=offset().bytes();
	uint32_t l=size().bytes();
	if(o+l<=sl) {rgenerate_=s.substr(o,l,rgenerate_);}
	return WtNode::generate(c,s);}

bool WvSubstr::generate(WControl& c,OCTBUF& s) {
	return WvObject::generate(c,s);}

bool WvOneof::generate(WControl& wc,OCTBUF&) {
	const PObject* o=object();
	o->generateNotAllow("oneof"); wc.result(1);
	return wc;}

bool WvComb::generate(WControl& wc,OCTBUF&) {
	const PObject* o=object();
	o->generateNotAllow("comb"); wc.result(1);
	return wc;}

//----------------------------------------------------------------------
void WObject::internalGeneratePrologue(ItPosition&) {}

void WtNode::internalGeneratePrologue(ItPosition& it) {
	CmQueue* n=0;
	for(n=nextChild();n!=0;n=nextChild(n)) {
		WObject* w=(WObject*)n;
		w->generatePrologue(it);}}

void WmObject::internalGeneratePrologue(ItPosition&) {
	const PObject* o=object();
	if(o!=0) {
		operator_=(eOperation)o->operation();
		if(operator_!=eOPEQ_) {o->generateNotAllow(operatorString());}}
	set_rgenerate(o!=0?o->generateValue(this):geneAutoValue());
	if(DBGFLAGS('k')) {
		const PObject *rg=rgenerate();
		printf("WmObject::internalGeneratePrologue() %s %s ",
			metaString(),operatorString());
		if(rg!=0)	{rg->print(); printf("\n");}
		else		{printf("NULL\n");}}}

void WvSubstr::internalGeneratePrologue(ItPosition&) {
	if(reference_==0||substr_!=0) return;
	ItPosition it;
	reference_->generatePrologue(it);
	const PObject* o=object();
	substr_=new PvOctets(it.bytes());
	o->generateOctetsWith(*substr_,reference_);}

void WObject::generatePrologue(ItPosition& at) {
	ItPosition it(at);
	offset(it);
	internalGeneratePrologue(it);
	setSize(it);
	stepPostion(at);}

void WcObject::generatePrologue(ItPosition& at) {
	WtNode::generatePrologue(at);
//Check alignment requirement
	const MObject* mc = meta();
	uint32_t X = mc->alignment_requirement();
	uint32_t H = distance();
	uint32_t S = length();
	uint32_t pad = MObject::padcount_alignment_requirement(X,H,S);
	if(pad){
		const PObject* o =object();
		const PObject* op = parent()? parent()->object() : 0;
		o->error("W %s alignment requirement(%d) need %d pad in %s",
			o->nameString(),X,pad,(op?op->nameString():""));
		}
	if(DBGFLAGS('a')) {
		printf("=%s distance %d\n",nameString(),H);}
	}
	

//----------------------------------------------------------------------
void WObject::notMatch(WControl& c,CSTR s,RObject& v) const {
	if(!c.logging()&&DBGFLAGS('e')==0) return;
	const MObject* m=meta();
	const MObject* vm=v.meta();
	TObject* p=0;
	const MObject* pm=0;
	printf("%sng %s ",logLevel>0?"log:":"",s);
	p=parent(); pm=p!=0?p->meta():0;
	if(pm!=0) {printf("%s.",pm->string());}
	printf("%s != ",m->string());
	p=v.parent(); pm=p!=0?p->meta():0;
	if(pm!=0) {printf("%s.",pm->string());}
	printf("%s\n",vm->string());}

void WObject::evaluateFailure(const PObject* r,const PObject* e,CSTR op) const {
	const MObject* m=meta();
	WObject* p=parent();
	CSTR n=p!=0?p->nameString():"";
	printf("log:ng compare %s.%s received:",n,m->string());
	if(r!=0) {r->log(0);} else {printf("NULL");}
	printf(" %s ",op!=0?op:"=");
	if(e!=0) {e->log(0);} else {printf("NULL");}
	printf("\n");}

void WObject::functionFailure(RObject* r) const {
	const MObject* m=r!=0?r->meta():0;
	const PObject* o=object();
	printf("log:ng function at %s not match with ",m!=0?m->string():"NULL");
	o->print(); printf("\n");}
	

void WObject::dbgEvaluate(const PObject* r,const PObject* e,bool ok,CSTR op) const {
	const MObject* m=meta();
	WObject* p=parent();
	CSTR n=p!=0?p->nameString():"";
	printf("%s: %s.%s received:",ok?"OK":"NG",n,m!=0?m->string():"");
	if(r!=0) {r->print();} else {printf("NULL");}
	printf(" %s ",op!=0?op:"=");
	if(e!=0) {e->print();} else {printf("NULL");}
	printf("\n");}

bool WObject::mustDefine(const PObject* m) const {
	const WObject* p=parent();
	const PObject* o=p!=0?p->object():m;
	o->error("E %s must define %s",o->nameString(),metaString());
	return false;}

bool WObject::mustDefineMem(const MObject* m) const {
	const PObject* o=object();
	o->error("E %s must define %s",o->nameString(),m->string());
	return false;}

//----------------------------------------------------------------------
bool WObject::vevaluate(const void *r) {
	if(r==0) {return false;}
	WControl c;
	evaluate(c,(RObject*)r);
	return c;}

bool WObject::metaEvaluate(WControl& c,RObject& v) {
	const MObject* m=meta();
	const MObject* vm=v.meta();
	bool comp=m->comparableMeta(vm);
	if(!comp) {c.result(1); notMatch(c,"meta",v);}
	return comp;}

bool WObject::valueEvaluate(WControl& c,const PvObject* ro,const PvObject* eo) {
	if(eo==0||ro==0) {abort(); return false;}
	int32_t x=eo->compareObjectWith(c,*ro);
	bool cmp=false;
	uint32_t op=operation();
	/**/ if(x> 0) {cmp=op&eOPLT_;}
	else if(x==0) {cmp=op&eOPEQ_;}
	else if(x< 0) {cmp=op&eOPGT_;}
	if(DBGFLAGS('e')) {dbgEvaluate(ro,eo,cmp,operatorString());}
	if(!cmp) {
		c.result(1);
		if(c.logging()) {evaluateFailure(ro,eo,operatorString());}}
	return cmp;}

//----------------------------------------------------------------------
RObject* WObject::evaluate(WControl& c,RObject* r) {
	if(r==0) return 0;
	if(metaEvaluate(c,*r)) {doEvaluate(c,*r);}
	RObject* p=r->parent();
	return (p!=0)?p->nextEvaluation(r):0;}

bool WObject::doEvaluate(WControl&,RObject&) {return false;}

bool WcObject::doEvaluate(WControl& c,RObject& r) {
	WObject* w=nextEvaluation();
	RObject* cr=r.nextEvaluation();
	for(;w!=0;w=nextEvaluation(w)) {
		if(cr==0) {break;}
		cr=w->evaluate(c,(RObject*)cr);
		if(c.stop()) {c.clearStop(); w=0; cr=0; break;}}
	if(w!=0||cr!=0) {c.result(1); notMatch(c,"count",r);}
	return c;}

bool WmObject::doEvaluate(WControl& c,RObject& r) {
	const PvObject* ro=r.pvalue();
	const PvObject* eo=revaluate();
	return valueEvaluate(c,ro,eo);}

RObject* WvOneof::evaluate(WControl& c,RObject* r) {
	WObject* ok=references_.findMatching(r,(WObjectEqFunc)&WObject::vevaluate);
	if(ok==0) {c.result(1); functionFailure(r);}
	RObject* p=r->parent();
	return p->nextEvaluation(r);}

RObject* WvComb::evaluate(WControl& c,RObject* r) {
	WObjectList tmp(references_);
	RObject* q=r;
	RObject* p=r->parent();
	for(;tmp.size();) {
		if(q==0) {q=p->nextEvaluation(r); r=q;}
		WObject* ok=tmp.findMatching(q,(WObjectEqFunc)&WObject::vevaluate);
		if(ok==0) {c.result(1); functionFailure(r); return q;}
		q=0;
		tmp.remove(ok);}
	if(tmp.size()!=0) {c.result(1); functionFailure(r);}
	return p->nextEvaluation(r);}

//----------------------------------------------------------------------
void WObject::internalEvaluatePrologue(ItPosition&) {}

void WtNode::internalEvaluatePrologue(ItPosition& it) {
	CmQueue* n=0;
	for(n=nextEvaluation();n!=0;n=nextEvaluation(n)) {
		WObject* w=(WObject*)n;
		w->evaluatePrologue(it);}}

void WmObject::internalEvaluatePrologue(ItPosition&) {
	const PObject* o=object();
	if(o!=0) {operator_=(eOperation)o->operation();}
	revaluate_=o!=0?o->evaluateValue(this):evalAutoValue();
	if(DBGFLAGS('k')) {
		PObject* re=revaluate();
		printf("WmObject::internalEvaluatePrologue() %s %s ",
			metaString(),operatorString());
		if(re!=0)	{re->print(); printf("\n");}
		else		{printf("NULL\n");}}}

void WObject::evaluatePrologue(ItPosition& it) {
	internalEvaluatePrologue(it);}

//----------------------------------------------------------------------
void WObject::icvUpdate(const PaObject&) {}

void WtNode::icvUpdate(const PaObject& sa) {
	CmQueue* n=0;
	for(n=nextEvaluation();n!=0;n=nextEvaluation(n)) {
		WObject* w=(WObject*)n;
		w->icvUpdate(sa);}}

void WcObject::icvUpdate(const PaObject& sa) {
	const MObject* m=meta();
	if(m!=0&&m->icvUpdate(sa,rgenerate_)) {return;}
	WtNode::icvUpdate(sa);}

//----------------------------------------------------------------------
const WObject* WObject::ancestor(const PObject* r) const {
	const PObject* o=object();
	if(o==r) return this;
	WObject* p=parent();
	return p!=0?p->ancestor(r):0;}

WObject* WtNode::insert(TObject* w) {
	return (WObject*)children_.insert(w);}

void WtNode::child(TObject* c) {insert(c);}
void WvSubstr::child(TObject* c) {
	if(reference_==0) {reference_=(WObject*)c;}
	WvObject::child(c);}
void WvOneof::child(TObject* c) {
	references_.add((WObject*)c); WvObject::child(c);}
void WvComb::child(TObject* c) {
	references_.add((WObject*)c); WvObject::child(c);}

//----------------------------------------------------------------------
uint16_t WObject::width() const {
	uint16_t w=0;
	const ItPosition& s=size();
	if(s.bytes()>=0) {
		w=s.bits();}
	else {
		const MObject* m=meta();
		const PObject* po=object();
		w=m!=0?m->width():(po!=0)?po->width():0;}
	return w;}

uint32_t WObject::length() const {
	const ItPosition& s=size();
	int32_t l=s.bytes();
	if(l<0) {
		const MObject* m=meta();
		const PObject* po=object();
		l=m!=0?m->objectLength(po,this):(po!=0)?po->objectLength(this):0;}
	return l;}

//======================================================================
// CONSTRUCTOR/DESCTUCTOR
WObject::WObject(WObject* p,const MObject* m,const PObject* o):TObject(p,m),
	object_(o),size_(-1,0),offset_(){}
WObject::~WObject() {object_=0;}

WtNode::WtNode(WObject* p,const MObject* m,const PObject* o):WObject(p,m,o),
	children_() {}
WtNode::~WtNode() {
	CmQueue* n=0;
	CmQueue* o=&children_;
	for(n=o->prev();n!=o;n=o->prev()) {
		n->deque(); delete n;}}

WmObject::WmObject(WObject* p,const MObject* m,const PObject* o):WtNode(p,m,o),
	operator_(eOPEQ_),rgenerate_(0),revaluate_(0) {}
WmObject::~WmObject() {
	if(rgenerate_!=0) {delete rgenerate_; rgenerate_=0;}
	if(revaluate_!=0) {delete revaluate_; revaluate_=0;}}

WcObject::WcObject(WObject* p,const MObject* m,const PObject* o):WtNode(p,m,o),
	rgenerate_(0) {}
WcObject::~WcObject() {
	if(rgenerate_!=0) {delete rgenerate_; rgenerate_=0;}}

WcPacket_IP::WcPacket_IP(WObject* p,const MObject* m,const PObject* o):
	WcObject(p,m,o),IPinfo_(new Con_IPinfo()) {}
WcPacket_IP::~WcPacket_IP() {
	if(IPinfo_!=0) {delete IPinfo_; IPinfo_=0;}}

WvObject::WvObject(WObject* p,const MObject* m,const PObject* o):WtNode(p,m,o) {}
WvObject::~WvObject() {}

WvSubstr::WvSubstr(WObject* p,const MObject* m,const PObject* o):WvObject(p,m,o),
	reference_(0),substr_(0) {}
WvSubstr::~WvSubstr() {
	reference_=0;		// reference_ is in children.
	if(substr_!=0) {delete substr_; substr_=0;}}

WvOneof::WvOneof(WObject* p,const MObject* m,const PObject* o):WvObject(p,m,o),
	references_() {}
WvOneof::~WvOneof() {}

WvComb::WvComb(WObject* p,const MObject* m,const PObject* o):WvObject(p,m,o),
	references_() {}
WvComb::~WvComb() {}

//======================================================================
// FOR DEBUGING
void WObject::printName(uint32_t t,CSTR cls) const {
	TObject::printName(t,cls?cls:"WObject");
	const PObject* o=object();
	if(o!=0) printf("='%s'",o->nameString());
	const ItPosition& it=offset();
	const ItPosition& sz=size();
	printf("\t%d %d %d %d",it.bytes(),it.bits(),sz.bytes(),sz.bits());}

void WcObject::printName(uint32_t t,CSTR cls) const {
	WObject::printName(t,cls);
	if(DBGFLAGS('k')&&rgenerate_!=0) {
		printf(" rg=%d",rgenerate_->length());}}

CSTR WObject::nameString() const {
	const PObject* o=object();
	return (o!=0)?o->nameString():"UNKOWN";}

implementCmList(WObjectList,WObject);

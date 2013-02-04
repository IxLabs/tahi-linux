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
 * $TAHI: v6eval/lib/Pz/PvObject.cc,v 1.53 2005/05/09 09:35:24 akisada Exp $
 */
#include "WObject.h"
#include "PvObject.h"
#include "PvOctets.h"
#include "MmObject.h"
#include "MvFunction.h"
#include "PControl.h"
#include <stdio.h>
#include <time.h>

PvObject::PvObject(CSTR f,int l):PObject(f,l) {}
PvObject::PvObject(const PvObject& o):PObject(o) {}
PvObject::~PvObject() {}
PvObject* PvObject::shallowCopy() const {return 0;}

// COMPOSE
WObject* PvObject::selfCompose(WControl& c,WObject* w_parent) const {
	const MObject* m=meta();
	return m!=0?m->compose(c,w_parent,this):new WvObject(w_parent,0,this);}

//======================================================================
// PvOperator
PvOperator::PvOperator(const MObject* m,CSTR f,int l):PvObject(f,l),meta_(m),rvalue_(0) {}
PvOperator::PvOperator(const PvOperator& x):PvObject(x),
	meta_(x.meta_),rvalue_(x.rvalue_){}
PvOperator::~PvOperator() {}
PvObject* PvOperator::shallowCopy() const {return new PvOperator(*this);}

bool PvOperator::generate(WControl& c,WObject* w,OCTBUF& s) const {
	PObject* r=rvalue();
	int32_t o=operation();
	if(o!=TObject::eOPEQ_) {
		error("E cannot generate with operator %s",metaString());}
	return (r!=0)?r->generate(c,w,s):w->geneAuto(c,s);}

PObject* PvOperator::rvalue(PObject* r) {
	rvalue_=r;
	return this;}

uint32_t PvOperator::operation() const {
	const MvOperator* m=(const MvOperator*)meta();
	return m!=0?m->operation():0;}

CSTR PvOperator::metaString() const {
	const MObject* m=meta();
	return m!=0?m->string():"";}

void PvOperator::print() const {
	printf("%s",metaString());
	PObject* r=rvalue();
	if(r!=0) {r->print();}}

uint32_t PvOperator::objectLength(const WObject* w) const {
	PObject* r=rvalue();
	return r!=0?r->objectLength(w):0;}

//======================================================================
// PvFunction
PFunction::PFunction(CSTR f,int l):PvObject(f,l),args_(4) {}
PFunction::PFunction(const PFunction& o):PvObject(o),args_(o.args()) {}
PFunction::~PFunction() {}

PObject* PFunction::argument(PObject* a) {
	args_.add(a);
	return this;}

void PFunction::checkArgument() const {}

const MObject* PFunction::meta() const {return 0;}
CSTR PFunction::metaString() const {
	const MObject* m=meta();
	return m!=0?m->string():"";}

void PFunction::print() const {
	printf("%s("/*)*/,metaString());
	uint32_t i,i9=args_.size();
	for(i=0;i<i9;i++) {
		PObject* o=args_[i];
		o->printArgument();
		if(i+1!=i9) {printf(",");}}
	printf(/*(*/")");}

//----------------------------------------------------------------------
// GENERATOR FUNCTION
PvFunction::PvFunction(const MvFunction* m,CSTR f,int l):PFunction(f,l),
	meta_(m) {}
PvFunction::PvFunction(const PvFunction& o):PFunction(o),meta_(o.metaClass()) {}
const MObject* PvFunction::meta() const {return meta_;}
PvObject* PvFunction::shallowCopy() const {return new PvFunction(*this);}
PvFunction::~PvFunction() {}

void PvFunction::checkArgument() const {
	const MvFunction* m=metaClass();
	m->checkArgument(*this,args());}

uint32_t PvFunction::objectLength(const WObject* w) const {
	const MvFunction* m=metaClass();
	return m!=0?m->functionLength(args(),w):0;}

bool PvFunction::generate(WControl& c,WObject* w,OCTBUF& s) const {
	const MvFunction* m=metaClass();
	m->functionGenerate(c,w,s,args());
	return c;}

bool PvFunction::generateV6Addr(PvV6Addr& v6) const {
	const MvFunction* m=metaClass();
	return m->generateV6Addr(args(),v6);}

bool PvFunction::evaluateV6Addr(PvV6Addr& v6) const {
	const MvFunction* m=metaClass();
	return m->evaluateV6Addr(args(),v6);}

bool PvFunction::isV6Addr() const {
	const MvFunction* m=metaClass();
	return m!=0?m->isV6Addr():false;}

bool PvFunction::isOctets() const {
	const MvFunction* m=metaClass();
	return m!=0?m->isOctets():false;}

bool PvFunction::generateOctetsWith(PvOctets& oct,WObject* w) const {
	const MvFunction* m=metaClass();
	return m->generateOctetsWith(args(),oct,w);}

//======================================================================
// PvNumber
PvNumber::PvNumber():PvObject(),value_(0) {}
PvNumber::PvNumber(int32_t v):PvObject(),value_(v) {}
PvNumber::PvNumber(const PvNumber& o):PvObject(o),value_(o.value()) {}
PvNumber::~PvNumber() {}
PvObject* PvNumber::shallowCopy() const {return new PvNumber(*this);}
uint32_t PvNumber::objectLength(const WObject*) const {return 1;}

bool PvNumber::generate(WControl& c,WObject* w,OCTBUF& s) const {
	w->encodeNumber(c,s,*this);
	return c;}

int32_t PvNumber::intValue(bool& ok) const {ok=true; return value_;}
void PvNumber::print() const {
	printf("%u",value_);}

int32_t PvNumber::compareNumber(int32_t r) const {
	return value()-r;}
int32_t PvNumber::compareObject(const PObject& r) const {
	return -(r.compareNumber(value()));}

//======================================================================
// PvNumbers
PvNumbers::PvNumbers():PvObject(),numbers_() {}
PvNumbers::PvNumbers(const PvNumbers& x):PvObject(x),numbers_(x.numbers_) {}
PvNumbers::~PvNumbers() {}
PvObject* PvNumbers::shallowCopy() const {return new PvNumbers(*this);}

uint32_t PvNumbers::length() const {return numbers_.size();}
void PvNumbers::add(PObject* v) {numbers_.add((PvNumber*)v);}
void PvNumbers::print() const {
	const PvNumberList& nums=numbers_;
	uint32_t i,i9=nums.size();
	printf("[%d]={",i9);
	for(i=0;i<i9;i++) {
		PvNumber* n=nums[i];
		bool ok;
		printf("%d%s",n->intValue(ok),i!=i9-1?",":"}");}}

bool PvNumbers::generate(WControl& c,WObject* w,OCTBUF& s) const {
	const PvNumberList& nums=numbers_;
	uint32_t i,i9=nums.size();
	PvOctets tmp(i9);
	ItPosition it;
	for(i=0;i<i9;i++,it.addBytes(1)) {
		PvNumber* n=nums[i];
		bool ok;
		tmp.encodeUint(n->intValue(ok),it,8);}
	w->encodeOctets(c,s,tmp);
	return c;}

PvOctets* PvNumbers::octetString() const {
	const PvNumberList& nums=numbers_;
	uint32_t i,i9=nums.size();
	PvOctets* oct=new PvOctets(i9);
	ItPosition it;
	for(i=0;i<i9;i++,it.addBytes(1)) {
		PvNumber* n=nums[i];
		bool ok;
		oct->encodeUint(n->intValue(ok),it,8);}
	return oct;}

//======================================================================
// PvString
PvString::PvString():PvObject(),value_() {}
PvString::PvString(CSTR s):PvObject(),value_(s) {}
PvString::PvString(const PvString& x):PvObject(x),value_(x.value_) {}
PvString::~PvString() {}
PvObject* PvString::shallowCopy() const {return new PvString(*this);}

void PvString::print() const {
	bool ok;
	printf("\"%s\"",strValue(ok));}

uint32_t PvString::length() const {return value_.length();}
CSTR PvString::strValue(bool& ok) const {ok=true; return value_.string();}
COCTSTR PvString::octetsValue(bool& ok) const {
	ok=true;
	return (COCTSTR)value_.string();}

PvOctets* PvString::octetString() const {
	uint32_t l=value_.length();
	OCTSTR s=(OCTSTR)value_.string();
	return new PvOctets(l,s,true);}

//======================================================================
PvObject* PvObject::generateValue(WObject*) const {
	return shallowCopy();}

PvObject* PvObject::evaluateValue(WObject*) const {
	return shallowCopy();}

PvObject* PvOperator::generateValue(WObject* w) const {
	PObject* r=rvalue();
	return  r!=0?r->generateValue(w):w->geneAutoValue();}

PvObject* PvOperator::evaluateValue(WObject* w) const {
	PObject* r=rvalue();
	return  r!=0?r->evaluateValue(w):w->evalAutoValue();}

PvObject* PvFunction::generateValue(WObject* w) const {
	const MvFunction* m=metaClass();
	PvObject* o=m->generateValue(w,args());
	return o!=0?o:PvObject::generateValue(w);}

PvObject* PvFunction::evaluateValue(WObject* w) const {
	const MvFunction* m=metaClass();
	PvObject* o=m->evaluateValue(w,args());
	return o!=0?o:PvObject::evaluateValue(w);}

int32_t PvFunction::compareObject(const PObject& r) const {
	const MvFunction* m=metaClass();
	return m->compareObject(r,args());}

PvNumber* PvNumber::unique(int32_t n) {
	PvNumber tmp(n);
	PvNumber* f=numbers_.find(&tmp);
	if(f==0) {
		f=new PvNumber(n);
		numbers_.add(f);}
	return f;}

PvTimeval::PvTimeval():PvObject() {
	value_.tv_sec=0;
	value_.tv_usec=0;}

PvTimeval::PvTimeval(const timeval& tv):PvObject() {
	value_.tv_sec=tv.tv_sec;
	value_.tv_usec=tv.tv_usec;}

PvTimeval::PvTimeval(const PvTimeval& p):PvObject() {
	const timeval& tv=p.value();
	value_.tv_sec=tv.tv_sec;
	value_.tv_usec=tv.tv_usec;}

PvObject* PvTimeval::shallowCopy() const {return new PvTimeval(*this);}
uint32_t PvTimeval::length() const {return sizeof(timeval);}
void PvTimeval::print() const {
	const timeval& tv=value();
	const time_t sec = tv.tv_sec;
	struct tm* m=localtime(&sec);
	printf("%04d/%02d/%02d %02d:%02d:%02d.%06ld",
		m->tm_year+1900,m->tm_mon+1,m->tm_mday,
		m->tm_hour,m->tm_min,m->tm_sec,tv.tv_usec);}

int32_t PvTimeval::compareTimeval(const timeval& r) const {
	const timeval& l=value();
	return (l>r)?1:(l==r)?0:-1;}

int32_t PvTimeval::compareObject(const PObject& ro) const {
	return -(ro.compareTimeval(value()));}

PvTimeval::~PvTimeval() {}
	
void PvObject::log(uint32_t t) const {print();}

PvNumberSet PvNumber::numbers_(256);
implementCmSet(PvNumberSet,PvNumber);
implementCmList(PvNumberList,PvNumber);

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
 * $TAHI: v6eval/lib/Pz/PObject.cc,v 1.50 2005/05/09 09:35:24 akisada Exp $
 */
#include "PObject.h"
#include "PvOctets.h"
#include "CmLexer.h"
#include "WObject.h"
#include "PControl.h"
#include "McObject.h"
#include <stdio.h>
PObject::PObject(CSTR f,int l):file_(f),line_(l) {}
PObject::PObject(const PObject& o):file_(o.file()),line_(o.line()) {}
PObject::~PObject() {}
bool PObject::isEqual(const PObject* v) const {return this==v;}
PObject* PObject::rvalue(PObject*) {return this;}
PObject* PObject::name(PObject*) {return this;}
PObject* PObject::member(PObject*) {return this;}
PObject* PObject::argument(PObject*) {return this;}
const PObject* PObject::rvalue()const{return 0;}
const PObjectList& PObject::members()const{static PObjectList tmp; return tmp;}
const PObjectList& PObject::args()const{static PObjectList tmp; return tmp;}
PObject* PObject::unary(PObject*) {return this;}
PObject* PObject::node(PObject*,PObject*) {return this;}
const PObject* PObject::lvalue() const {return 0;}
const PObject* PObject::unary() const {return 0;}
bool PObject::matchesWith(const PObject&) const {return false;}
const PObject* PObject::corresponding(const PObject&) const {return this;}
const PObject* PObject::correspondingMeta(const MObject*) const {return 0;}

int32_t PObject::intValue(bool& ng) const {ng=false; return 0;}
CSTR PObject::strValue(bool& ng) const {ng=false; return 0;}
COCTSTR PObject::octetsValue(bool& ng) const {ng=false; return 0;}
PvOctets* PObject::octetString() const {return 0;}
bool PObject::isV6Addr() const {return false;}
bool PObject::isOctets() const {return false;}
bool PObject::isName() const {return false;}
uint32_t PObject::operation() const {return TObject::eOPEQ_;}
const MObject* PObject::findMember(CSTR) const {return 0;}
bool PObject::isEqualObject(const PObject* p) const {
	return p!=0?compareObject(*p)==0:false;}

void PObject::error(CSTR fmt,...) const {
	va_list v;
	va_start(v,fmt);
	CmLexer::eout(file(),line(),fmt[0],&fmt[2],v);
	va_end(v);}
CSTR PObject::nameString() const {return "";}
CSTR PObject::metaString() const {
	const MObject* m=meta();
	return m!=0?m->string():"";}
void PObject::print() const {}
void PObject::printConvert() const {print();}
void PObject::printArgument() const {print();}
void PObject::log(uint32_t t) const {}
void PObject::generateNotAllow(CSTR s) const {
	error("E generate not allow for %s",s);}
int32_t PObject::compareOctets(const PvOctets&) const {return -1;}
int32_t PObject::compareNumber(int32_t) const {return -1;}
int32_t PObject::compareTimeval(const timeval&) const {return -1;}
int32_t PObject::compareCalc() const{return -1;}
int32_t PObject::compareObject(const PObject&) const {return -1;}
int32_t PObject::compareObjectWith(WControl&,const PObject& o) const {
	return compareObject(o);}

const MObject* PObject::meta() const {return 0;}
bool PObject::isEqualMeta(const MObject*) const {
	return false;}

uint16_t PObject::width() const {return 0;}
uint32_t PObject::length() const {return 0;}
uint32_t PObject::objectLength(const WObject*) const {return length();}

bool PObject::generate(WControl& c,WObject*,OCTBUF&) const {
	c.result(1);
	error("U PObject::generate has called");
	return c;}
PvObject* PObject::generateValue(WObject*) const {
	return 0;}
PvObject* PObject::evaluateValue(WObject*) const {
	return 0;}
bool PObject::generateV6Addr(PvV6Addr& into) const {
	into.zero();
	return false;}
bool PObject::evaluateV6Addr(PvV6Addr& into) const {
	into.zero();
	return false;}
bool PObject::generateOctetsWith(PvOctets& into,WObject*) const {
	into.zero();
	return false;}

// COMPOSE
WObject* PObject::selfCompose(WControl& c,WObject* w_parent) const {
	const MObject* m=meta();
	return m!=0?m->compose(c,w_parent,this):0;}

void PObject::vselfCompose(void* v,va_list ap) const {
        WControl* c = (WControl*)v;
	DCLA(WObject*,w_parent);
        selfCompose(*c,w_parent);}

void PObject::vmatchselfCompose(void* v,va_list ap) const {
        WControl* c = (WControl*)v;
	DCLA(WObject*,w_parent);
	DCLA(const MObject*,m);
	if(isEqualMeta(m)){
		selfCompose(*c,w_parent);}
	}

bool PObject::check_nameCompose(WControl&,WObject*,const PvName&) const{
	return true;}

//----------------------------------------------------------------------------
// SA MEMBER YAC INTERFACE
PObject* PObject::pad_member(PObject*){return this;}
PObject* PObject::crypt_member(PObject*){return this;}
PObject* PObject::auth_member(PObject*){return this;}
PObject* PObject::bsa_member(PObject*){return this;}
PObject* PObject::ike_p2_hash_member(PObject*){return this;}
PObject* PObject::dhcpauth_member(PObject*){return this;}

//----------------------------------------------------------------------------
PoQueue::PoQueue(PObject* o):CmQueue(),object_(o) {}
PoQueue::~PoQueue() {object_=0;}
PObject* PoQueue::pop() {
	PoQueue* p=(PoQueue*)prev();
	if(p!=this) {p->deque(); delete p;}
	p=(PoQueue*)prev();
	return p->object();}
PObject* PoQueue::push(PObject* o) {
	PoQueue* q=new PoQueue(o);
	insert(q);
	return o;}

implementCmList(PObjectList,PObject);

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
 * $TAHI: v6eval/lib/Pz/RObject.cc,v 1.26 2003/12/04 04:59:47 akisada Exp $
 */
#include "RObject.h"
#include "PControl.h"
#include "McObject.h"
#include "PvObject.h"
#include "PvOctets.h"
#include "ItPosition.h"
#include "CmMain.h"
#include <stdio.h>

TObject::TObject(TObject* p,const MObject* m):CmQueue(),
	meta_(m),parent_(p){
	if(p!=0) {p->child(this);}}
TObject::~TObject() {meta_=0; parent_=0;}

const PvObject* TObject::pvalue()const{return 0;}
ItPosition TObject::size()const{return ItPosition();}

void TObject::child(TObject*) {}
TObject* TObject::nextChild(const CmQueue* q) const {
	const CmQueue* top=children();
	const CmQueue* p=q!=0?q:top;
	CmQueue* n=p!=0?p->next():0;
	return n!=top?(TObject*)n:0;}
TObject* TObject::nextSister() const {
	TObject* p=parent();
	return p!=0?p->nextChild(this):0;}
TObject* TObject::prevChild(const CmQueue* q) const {
	const CmQueue* top=children();
	const CmQueue* p=q!=0?q:top;
	CmQueue* n=p!=0?p->prev():0;
	return n!=top?(TObject*)n:0;}
TObject* TObject::prevSister() const {
	TObject* p=parent();
	return p!=0?p->prevChild(this):0;}

bool TObject::evaluateObject() const {
	const MObject* m=meta();
	return (m!=0 && !m->disused());}
TObject* TObject::nextEvaluation(const CmQueue* q) const {
	TObject* n=nextChild(q);
	for(;n!=0;n=nextChild(n)) {
		if(n->evaluateObject()) {return n;}}
	return n;}
void TObject::adoptSisters(TObject& newparent) {
	TObject* s=0;
	for(;(s=nextSister())!=0;) {
		s->deque();
		newparent.child(s);
		s->adoptParent(&newparent);}}

// OVERWRITE ICV 
bool TObject::overwrite_ICV(ICVControl& c,ItPosition& at,OCTBUF& buf)const{
	const MObject* m = meta();
	bool rtn = m ? m->overwrite_ICV(c,at,buf,this) : true;
	at += size();
	return rtn;}
bool TObject::overwrite_ICV_child(ICVControl& c,
		const ItPosition& at,OCTBUF& buf)const{
	bool rtn = true;
	ItPosition memat = at;
	TObject* child = 0;
	for(child=nextChild(); child&&rtn; child=nextChild(child)) {
		rtn = child->overwrite_ICV(c,memat,buf);}
	return rtn;}

// POST ACTION
void TObject::post_reverse(Con_IPinfo&,RControl&,RObject*){}
void TObject::post_generate(Con_IPinfo&,WControl&,OCTBUF&,WObject*){}
void TObject::post_generateWithLength(Con_IPinfo &, WControl &, OCTBUF &, WObject *, uint32_t) {}

//======================================================================
// CONSTRUCTOR/DESCTUCTOR
RObject::RObject(RObject* p,const MObject* m,const ItPosition& o,PvObject* pv):
	TObject(p,m),pvalue_(pv),offset_(o) {}
RObject::~RObject() {
	if(pvalue_!=0) {delete pvalue_; pvalue_=0;}}

//======================================================================
RtNode::RtNode(RObject* r_parent,const MObject* m,
		const ItPosition& offset,PvObject* pv):
	RObject(r_parent,m,offset,pv),children_() {}
RtNode::~RtNode() {
	CmQueue* n=0;
	CmQueue* o=&children_;
	for(n=o->prev();n!=o;n=o->prev()) {
		n->deque(); delete n;}}

RObject* RtNode::insert(TObject* w) {
	return (RObject*)children_.insert(w);}
void RtNode::child(TObject* c) {insert(c);}

//======================================================================
RmObject::RmObject(RObject* r_parent,const MObject* m,
		const ItPosition& offset,const ItPosition& size,PvObject* pv):
	RObject(r_parent,m,offset,pv),size_(size){}
RmObject::~RmObject() {}

//======================================================================
RcObject::RcObject(RObject* r_parent,const MObject* m,
		const ItPosition& offset,PvObject* pv):
	RtNode(r_parent,m,offset,pv){}
RcObject::~RcObject() {}
ItPosition RcObject::size()const{return ItPosition(pvalue()->length(),0);}

//======================================================================

RcPacket_IP::RcPacket_IP(RObject* r_parent,const MObject* m,
		const ItPosition& offset,PvObject* pv):
	RcObject(r_parent,m,offset,pv),IPinfo_(new Con_IPinfo()){}
RcPacket_IP::~RcPacket_IP() {
	if(IPinfo_) {delete IPinfo_; IPinfo_=0;}}

//======================================================================
// CORRSPONDING
const TObject* TObject::corresponding(const MObject* c) const {
	const MObject* m=meta();
	if(c==m) return this;
	const CmQueue* n=0;
	for(n=nextChild();n!=0;n=nextChild(n)) {
		const TObject* i=(const TObject*)n;
		const TObject* f=i->corresponding(c);
		if(f!=0) return f;}
	return 0;}

//======================================================================
// FOR DEBUGING
CSTR TObject::metaString() const {
	const MObject* m=meta();
	return m!=0?m->string():"";}

void TObject::printName(uint32_t t,CSTR cls) const {
	uint32_t i;
	for(i=0;i<t;i++) {printf("| ");}
	printf("%s::print(%s)",cls!=0?cls:"TObject",metaString());}

void TObject::printChild(uint32_t t) const {
	const CmQueue* n=0;
	for(n=nextChild();n!=0;n=nextChild(n)) {
		const TObject* w=(const TObject*)n;
		w->print(t+1);}}

void TObject::print(uint32_t t) const {
	printName(t); printf("\n");
	printChild(t);}

void RObject::printName(uint32_t t,CSTR) const {
	printf("log:");
	for(uint32_t i=0;i<t;i++) {printf("| ");}
	printf("%-30s",metaString());}

void RObject::print(uint32_t t) const {
	TObject::print(t);}

void RmObject::printName(uint32_t t,CSTR) const {
	RObject::printName(t);
	const PvObject* o=pvalue();
	if(o!=0) {printf(" = "); o->print();}
	else {printf("NO OBJECT");}}

void RcObject::printName(uint32_t t,CSTR) const {
	RObject::printName(t);
	const PvObject* o=pvalue();
	if(o!=0) {printf("(length:%d)",o->length());}
	else {printf("NO BODY");}}

//======================================================================
// FOR LOGGING
void TObject::logSelf(uint32_t t,CSTR) const {
	printf("log:");
	for(uint32_t i=0;i<t;i++) {printf("| ");}
	printf("%-32s",metaString());}

void TObject::logChild(uint32_t t) const {
	const CmQueue* n=0;
	for(n=nextChild();n!=0;n=nextChild(n)) {
		const TObject* w=(const TObject*)n;
		w->log(t+1);}}

void TObject::log(uint32_t t) const {
	logSelf(t); printf("\n");
	logChild(t);}

void RmObject::logSelf(uint32_t t,CSTR cls) const {
	RObject::logSelf(t,cls);
	const PvObject* o=pvalue();
	if(o!=0) {printf(" = "); o->log(t);}}

void RcObject::logSelf(uint32_t t,CSTR cls) const {
	RObject::logSelf(t,cls);
	const PvObject* o=pvalue();
	if(o!=0) {printf("(length:%d)",o->length());}}

//======================================================================
void TObject::describeSelf(uint32_t,CSTR) const {
	const MObject* m=meta();
	if(m!=0) {m->describe(pvalue());}}

void TObject::describeChild(uint32_t t) const {
	const CmQueue* n=0;
	for(n=nextChild();n!=0;n=nextChild(n)) {
		const TObject* w=(const TObject*)n;
		w->describe(t+1);}}

void TObject::describe(uint32_t t) const {
	describeSelf(t);
	describeChild(t);}

const CSTR TObject::operSTR[8]={"-","<","=","<=",">","!=",">=","-"};

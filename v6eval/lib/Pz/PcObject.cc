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
 * $TAHI: v6eval/lib/Pz/PcObject.cc,v 1.37 2003/10/23 04:37:30 akisada Exp $
 */
#include "PcObject.h"
#include "McObject.h"
#include "McNull.h"
#include "McEther.h"
#include "PvName.h"
#include "PvOctets.h"
#include "WObject.h"
#include "ItPosition.h"
#include "PControl.h"
#include "CmMain.h"
#include <stdio.h>
//======================================================================
// PACKET CLASS
PcObject::PcObject(const MObject* m,CSTR f,int l):PObject(f,l),
	meta_(m),name_(0),members_() {}
PcObject::~PcObject() {}

PObject* PcObject::name(PObject* pn) {
	name_=(PvName*)pn;
	name_->declaration(this);
	return this;}

PObject* PcObject::member(PObject* v) {
	members_.add(v);
	return this;}

const MObject* PcObject::findMember(CSTR s) const {
	const McObject* mc=(const McObject*)meta();
	return mc->findMember(s);}

CSTR PcObject::nameString() const {
	PvName* n=nameObject();
	return n!=0?n->nameString():"";}

void PcObject::printCompound(CSTR s) const {
	printf("%s(%s) %s\n",s,meta_->string(),nameString());}

void PcObject::print() const {
	printCompound("PcObject");
	members_.elementsPerform((PObjectFunc)&PObject::print);
	}

// COMPOSE
WObject* PcObject::selfCompose(WControl& c,WObject* w_parent) const {
	const MObject* m=meta();
	return m!=0?m->compose(c,w_parent,this):0;}

bool PcObject::check_nameCompose(WControl& c,
		WObject* w_parent,const PvName& nm)const{
	const TypevsMcDict* dict = c.dict();
	const MObject* mc = meta();
	if(mc && dict && !dict->contains(mc)){
		nm.error("E unmatch reference type %s",nm.nameString());
		return false;}
	if(w_parent && w_parent->ancestor(this)!=0){
		nm.error("U recurring at %s",nm.nameString());
		return false;}
	return true;}

//======================================================================
void PcObject::prints() {
	frameSet().elementsPerform((NameDefineFunc)&NameDefine::print);}

void PcObject::frame(PObject* pn) {
	frameSet().add(((PvName*)pn)->definition());}

PcObject*  PcObject::frameName(CSTR name) {
	NameDefine tmp(name);
	NameDefine* n=frameSet().find(&tmp);
	return n!=0?n->declaration():0;}

bool PcObject::frameList(const StringList& l,PcObjectList& to) {
	bool rc=true;
	uint32_t i,i9=l.size();
	for(i=0;i<i9;i++) {
		CmString* s=l[i];
		CSTR name=s->string();
		PcObject* o=frameName(name);
		if(o!=0) {to.add((PcObject*)o);}
		else {
			rc=false;
			fprintf(stderr,"err:specified %s not found\n",name);}}
	return rc;}

#include "PAlgorithm.h"
//======================================================================
SAOctets* PcObject::generateFrame() const {
	AlgorithmList* sa=new AlgorithmList();
	WSAControl wcomp(sa);		// to correct sa information.
	WObject* w = selfCompose(wcomp,0);
	if(sa->size()==0) {delete sa; sa=0;}
	ItPosition it;
	w->generatePrologue(it);
	if(DBGFLAGS('p')) {w->print(0);}
	uint32_t l=it.bytes();
	PvOctets* buf=new PvOctets(l);
	WControl wc;
	w->generate(wc,*buf);
	delete w;
	if(!wc) {delete buf; return 0;}
	SAOctets* saoct=new SAOctets(buf,sa);
	if(DBGFLAGS('d')) {buf->dump();}
	if(logLevel==0&&DBGFLAGS('r')) {
		ItPosition rit;
		RSAControl rc(sa);	// need sa for reverse
		McFrame* mcf=McFrame_Ether::instance();
		RObject* ro=mcf->reverse(rc,0,rit,*buf);
		ro->print(0);
		delete ro;}
	return saoct;}

bool PcObject::generateFrames(const PcObjectList& frames,SAOctetsList& l) {
	bool rc=true;
	uint32_t i,i9=frames.size();
	for(i=0;i<i9;i++) {
		PcObject* f=frames[i];
		SAOctets* o=f->generateFrame();
		if(o==0) {rc=false;}
		else {l.add(o);}}
	return rc;}

bool PcObject::sendingFrames(const StringList& names,SAOctetsList& l) {
	bool rc=true;
	PcObjectList frames(names.size());
	if(!(rc=frameList(names,frames))) {return rc;}
	return generateFrames(frames,l);}

//======================================================================
bool PcObject::evaluatePrologue(AlgorithmFrames& l) const {
	AlgorithmList* sa=new AlgorithmList();
	WSAControl wcomp(sa);		// to correct sa information.
	WObject* w = selfCompose(wcomp,0);
	if(w==0||sa->size()==0) {delete sa; sa=0;}
	if(w==0) {return false;}
	ItPosition it;
	w->evaluatePrologue(it);
	if(DBGFLAGS('e')) {
		printf("==========================================\n");
		w->print(0);}
	if(sa==0)	{l.none(w);}
	else		{l.alogrithm(new WAlgorithm(w,sa));}
	return true;}

bool PcObject::evaluateComposers(const PcObjectList& frames,AlgorithmFrames& l) {
	bool rc=true;
	uint32_t i,i9=frames.size();
	for(i=0;i<i9;i++) {
		PcObject* f=frames[i];
		if(!f->evaluatePrologue(l)) {rc=false;}}
	return rc;}

bool PcObject::evaluateFrames(const StringList& names,AlgorithmFrames& l) {
	bool rc=true;
	PcObjectList frames(names.size());
	if(!(rc=frameList(names,frames))) {return rc;}
	return evaluateComposers(frames,l);}

NameSet PcObject::frameSet_(512);
implementCmList(PcObjectList,PcObject);

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
 * $TAHI: v6eval/lib/Pz/PvName.cc,v 1.19 2005/05/09 09:35:24 akisada Exp $
 */
#include "PvName.h"
#include "PcObject.h"
#include <stdio.h>

NameDefine::NameDefine(CSTR s):CmCString(s),declaration_(0) {}
NameDefine::~NameDefine() {declaration_=0;}

PvName::PvName(NameDefine* o,CSTR f,int l):PvObject(f,l),definition_(o) {}
PvName::PvName(const PvName& x):PvObject(x),definition_(x.definition_){}
PvName::~PvName() {definition_=0;}
PvObject* PvName::shallowCopy() const{return new PvName(*this);}

void PvName::declaration(PcObject* o) {
	if(declaration()!=0) {
		error("E %s duplicated declaration",nameString());
		return;}
	NameDefine* nd=definition();
	nd->declaration(o);}

bool PvName::isName() const {return true;}

// COMPOSE
WObject* PvName::selfCompose(WControl& c,WObject* w_parent) const {
	const PObject* decl = declaration();
	if(decl==0){
		error("E %s not defined",nameString()); return 0;}
	if(!decl->check_nameCompose(c,w_parent,*this)){	//ErrorHandling
		return 0;}
	return decl->selfCompose(c,w_parent);}//forward to declaration

PvName* PvName::tokenObject(CSTR s,int l,CSTR f) {
	NameSet& set=NameDefine::nameSet();
	NameDefine tmp(s);
	NameDefine* o=set.find(&tmp);
	if(o==0) {set.add(o=new NameDefine(s));}
	return new PvName(o,f,l);}

void PvName::print() const {
	printf("%s",nameString());}
void PvName::printArgument() const {printf("%s",nameString());}

void NameDefine::print() const {
	PcObject* pc=declaration();
	if(pc!=0) {pc->print();}}
void NameDefine::printUndefine() {
	PcObject* o=declaration();
	if(o==0) {
		fprintf(stderr,"err: %s not declared\n",string());}}

void NameDefine::printUndefines() {
	NameSet& set=nameSet();
	set.elementsPerform((NameDefineFunc)&NameDefine::printUndefine);}

const uint32_t defaultNameSetSize=512;
NameSet NameDefine::nameSet_(defaultNameSetSize);
implementCmSet(NameSet,NameDefine);

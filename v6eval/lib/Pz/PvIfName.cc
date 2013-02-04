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
 * $TAHI: v6eval/lib/Pz/PvIfName.cc,v 1.9 2001/10/12 04:56:17 tanaka Exp $
 */
#include "PvIfName.h"
#include <stdio.h>
#include "PvOctets.h"
#include "CmLexer.h"
PvIfName::PvIfName(CSTR n):CmCString(n),ether_(0),v6addr_(0) {}
PvIfName::PvIfName(CSTR n,PvEther* e):CmCString(n),ether_(e),v6addr_() {
	PvEUI64 eui64(*e);
	const PvV6Addr& l=linkLocal();
	l.merge(64,eui64,&v6addr_);}

const PvV6Addr& PvIfName::linkLocal() {
	return PvV6Addr::linkLocal();}

void PvIfName::nut(CSTR n,CSTR e,CSTR f,uint32_t l) {
	bool ok=false;
	PvEther* ether=new PvEther(e,ok);
	if(!ok) {CmLexer::eouts(f,l,'E',"invalid ether address %s for interface %s",e!=0?e:"",n);}
	PvIfName* r=new PvIfName(n,ether);
	PvIfName* o=(PvIfName*)nutSet_->filter(r);
	if(r!=o) {CmLexer::eouts(f,l,'E',"duplicate NUT inteface name %s %s",n,e);}}

void PvIfName::tn(CSTR n,CSTR e,CSTR f,uint32_t l) {
	bool ok=false;
	PvEther* ether=new PvEther(e,ok);
	if(!ok) {CmLexer::eouts(f,l,'E',"invalid ether address %s for interface %s",e!=0?e:"",n);}
	PvIfName* r=new PvIfName(n,ether);
	PvIfName* o=(PvIfName*)tnSet_->filter(r);
	if(r!=o) {CmLexer::eouts(f,l,'E',"duplicate TN inteface name %s %s",n,e);}}

PvIfName* PvIfName::findNut(CSTR s) {
	CSTR name=s!=0?s:defaultIF();
	PvIfName tmp(name);
	PvIfName* f=nutSet_->find(&tmp);
	if(f==0) {CmLexer::error('U',"cannot find NUT interface %s",name!=0?name:"");}
	return f;}

PvIfName* PvIfName::findTn(CSTR s) {
	CSTR name=s!=0?s:defaultIF();
	PvIfName tmp(name);
	PvIfName* f=tnSet_->find(&tmp);
	if(f==0) {CmLexer::error('U',"cannot find TN interface %s",name!=0?name:"");}
	return f;}

void PvIfName::print() const {
	printf("IF=%s ether=",string()); ether()->print();
	printf(" v6="); v6addr()->print();
	printf("\n");}

PvIfName::~PvIfName() {
	delete ether_; ether_=0;}

void PvIfName::prints() {
	printf("---TN---------------------------------------\n");
	tnSet_->elementsPerform((PvIfNameFunc)&PvIfName::print);
	printf("---NUT--------------------------------------\n");
	nutSet_->elementsPerform((PvIfNameFunc)&PvIfName::print);}

void PvIfName::swapTNvsNUT() {
	PvIfNameSet* tmp=tnSet_;
	tnSet_=nutSet_;
	nutSet_=tmp;}

PvIfNameSet PvIfName::tnSetArea_;
PvIfNameSet PvIfName::nutSetArea_;
PvIfNameSet* PvIfName::tnSet_=&PvIfName::tnSetArea_;
PvIfNameSet* PvIfName::nutSet_=&PvIfName::nutSetArea_;
CSTR PvIfName::defaultIF_="unknown";
implementCmSet(PvIfNameSet,PvIfName);

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
 * $TAHI: v6eval/lib/Cm/CmMatch.cc,v 1.5 2001/10/12 04:56:13 tanaka Exp $
 */
#include "CmMatch.h"
#include "CmToken.h"
#include <stdio.h>
#include <stdlib.h>
CmMatch::CmMatch(const CmToken* t,CSTR s):CmQueue(),
	token_(t),string_(s),org_() {}
CmMatch::~CmMatch() {}

CmMatch* CmMatch::setToken(const CmToken& t) {
	if(token()!=0) {
		ooutf("duplicate CmMatch(%s)\n",t.string());
		abort();}
	token(&t);
	return this;}

//----------------------------------------------------------------------
// SPLIT TOKEN STRING by LENGTH
// this will be the child of newly created CmMatch
CmMatch* CmMatch::split(int32_t l) {
	CSTR ms=string();
	CmCString upper(ms,l);
	CmCString lower(ms+l);
	CmMatch* ulm=new CmMatch(0,upper.string());
	CmQueue* p=prev();
	deque();
	p->insert(ulm);
	ulm->org().insert(this);
	setString(lower.string());
	return ulm;}

//----------------------------------------------------------------------
// MATCHING INITIAL LETTER
CmQueue* CmMatch::greaterEqualInitial(const CmQueue& org,char c,char& mi) {
	const CmQueue* o=&org;
	CmQueue* p=org.next();
	mi=0;
	for(;p!=o;p=p->next()) {
		CmMatch* m=(CmMatch*)p;
		mi=m->initial();
		if(c<=mi) {return p;}}
	return p;}

const CmMatch* CmMatch::equalInitial(const CmQueue& org,char c) {
	const CmQueue* o=&org;
	CmQueue* p=org.next();
	for(;p!=o;p=p->next()) {
		CmMatch* m=(CmMatch*)p;
		char mi=m->initial();
		if(c==mi) {return m;}}
	return 0;}

//----------------------------------------------------------------------
// SCAN STRING
CmMatch* CmMatch::matchTokenString(const CmToken& t,CSTR ts) {
	CSTR ms=string();
	CSTR mp=0;
	char mc,tc;
	for(mp=ms;(tc=*ts,mc=*mp);mp++,ts++) { 
		if(tc!=mc) {break;}}
	if(mc==0&&tc==0) {
		return setToken(t);}
	CmMatch* lm=mc==0?this:split(mp-ms);
	lm=(tc==0)?lm->setToken(t):lm->newTokenString(t,ts);
	return lm;}

const CmMatch* CmMatch::findMatchString(CSTR ts) const {
	CSTR ms=string();
	CSTR mp=0;
	char mc,tc=0;
	for(mp=ms;(tc=*ts,mc=*mp);mp++,ts++) {
		if(tc!=mc) {break;}}
	if(mc!=0) {return 0;}
	const CmMatch* child=findString(org_,ts);
	const CmMatch* lm=child!=0?child->findMatchString(ts):0;
	return lm!=0?lm:this;}

//----------------------------------------------------------------------
// FIND INITIAL MATCH
CmMatch* CmMatch::addTokenString(CmQueue& org,const CmToken& t,CSTR ts) {
	char ti=*ts;
	char mi=0;
	CmQueue* q=greaterEqualInitial(org,ti,mi);
	CmMatch* lm=0;
	if(ti!=mi) {
		lm=new CmMatch(&t,ts);
		q->insert(lm);}
	else {
		lm=(CmMatch*)q;
		lm=lm->matchTokenString(t,ts);}
	return lm;}

const CmMatch* CmMatch::findString(const CmQueue& org,CSTR ts) {
	const CmMatch* lm=equalInitial(org,*ts);
	return lm!=0?lm->findMatchString(ts):0;}

//----------------------------------------------------------------------
// PUBLIC INTERFACE
const CmToken* CmMatch::findToken(CSTR s,const CmQueue* o) {
	const CmQueue& org=o!=0?*o:top();
	const CmMatch* lm=findString(org,s);
	return lm!=0?lm->token():0;}

const CmToken* CmMatch::lex(CSTR& s) {
	const CmToken* lt=findToken(s);
	if(lt!=0) {s+=lt->length();}
	return lt;}

CmMatch* CmMatch::addToken(const CmToken& t,CmQueue* o) {
	CmQueue& org=o!=0?*o:top();
	return addTokenString(org,t,t.string());}

CmQueue CmMatch::top_;

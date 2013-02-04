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
 * $TAHI: v6eval/lib/pkt/RAFControl.cc,v 1.13 2003/10/23 04:37:31 akisada Exp $
 */
#include "RAFControl.h"
#include "PAlgorithm.h"
#include "CmMain.h"
#include "WObject.h"
#include "PControl.h"
#include "PvOctets.h"
#include "MfAlgorithm.h"
#include <stdio.h>

RAFControl::RAFControl(const AlgorithmFrames& c):RControl(),
	candidates_(c),algorithm_(0) {}
RAFControl::~RAFControl() {}
const PaObject*	RAFControl::pop_SA(eAlgorithm,uint32_t,const PvObject*) {
	uint32_t i=current_++;
	const WAlgorithm* wa=algorithm();
	if(wa==0) {
		const AlgorithmFrames& af=candidates();
		algorithm(wa=af.nextAlgorithm());
		if(wa==0) {return 0;}}
	const AlgorithmList* l=wa->algorithms();
	const PaObject* sa=0;
	if(l!=0) {
		uint32_t i9=l->size();
		sa=i<i9?l->index(i):0;}
	return sa;}

bool RAFControl::nextAlgorithm() {
	current_=0;
	const WAlgorithm* wa=algorithm();
	if(wa==0) {return false;}
	const AlgorithmFrames& af=candidates();
	algorithm(wa=af.nextAlgorithm(wa));
	return wa!=0;}

void RAFControl::reset() {
	RControl::reset();
	current_=0;}

//----------------------------------------------------------------------
// return bool
//    ng: false: error	
//     -: false: not match
//  name: true:	 match
CSTR RAFControl::reverseEvaluate(const McObject& reverser,PvOctets& frame,bool &found) {
	found=false;
	WObject* w=0;
	bool loop=true;
	int reversed=0;
	for(;loop;loop=nextAlgorithm()) {
		reset();
		ItPosition rit;
		RObject* ro=reverser.reverse(*this,0,rit,frame);
		if(!error()) {				// REVERSE OK
			reversed++;
			/**/ if(logLevel>0)	{ro->log(0);}
			else if(DBGFLAGS('r'))	{ro->print(0);}
			if(DBGFLAGS('s')) {printf("desc:"); ro->describe(0); printf("\n");}
			if((w=evaluate(ro))!=0) {	// FOUND PACKET
				delete ro; ro=0;
				found=true; return w->nameString();}}
		else {					// REVERSE NG
			const WAlgorithm* wa=algorithm();
			if(wa==0) {			// NO ALGORITHM DETECTED
				delete ro; ro=0;
				return "ng";}
			if(logLevel>0) {
				const WObject* w=wa!=0?wa->composed():0;
				printf("log: reverse failed with using algorithms on %s\n",
					w!=0?w->nameString():"NO FRAME");}}
		delete ro; ro=0;}
	if(reversed==0&&(logLevel>0||DBGFLAGS('r'))) {
		ItPosition rit;
		RControl rc;
		RObject* ro=reverser.reverse(rc,0,rit,frame);
		if(!rc.error()) {
			/**/ if(logLevel>0)	{ro->log(0);}
			else if(DBGFLAGS('r'))	{ro->print(0);}}
		delete ro; ro=0;}
	return "-";}

void RAFControl::applyFrame(const WObject* w) const {
	WControl ok(logLevel>0,DBGFLAGS('e'));
	CSTR name=w!=0?w->nameString():"NO FRAME";
	if(ok.msg()) {
		printf("%s===%s=================================\n",
			ok.logging()?"log:":"",name);}}

const WAlgorithm* RAFControl::appliedAlgorithm() const {
	WControl ok(logLevel>0,DBGFLAGS('e'));
	const WAlgorithm* wa=algorithm();
	const WObject* w=wa!=0?wa->composed():0;
	applyFrame(w);
	return w!=0?wa:0;}

bool RAFControl::evaluateWith(RObject* ro,WObject* w) const {
	if(w==0) return false;
	WControl ok(logLevel>0,DBGFLAGS('e'));
	w->evaluate(ok,ro);
	return ok;}

WObject* RAFControl::evaluateAlgorithm(RObject* ro) const {
	const WAlgorithm* wa=appliedAlgorithm();
	if(wa==0) {return 0;}
	WObject* w=(WObject*)wa->matchedComposed(current_,ro);
	return w!=0?evaluateWith(ro,w)?w:0:0;}
	

WObject* RAFControl::evaluate(RObject* ro) {
	if(current_>0) {return evaluateAlgorithm(ro);}
	const WObjectList& wl=noneAlgorithms();
	uint32_t i,i9=wl.size();
	for(i=0;i<i9;i++) {
		WObject* w=wl[i];
		applyFrame(w);
		if(evaluateWith(ro,w)) {return w;}}
	return 0;}

const WObjectList& RAFControl::noneAlgorithms() const {
	const AlgorithmFrames& af=candidates();
	return af.noneAlgorithms();}

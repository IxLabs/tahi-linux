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
 * $TAHI: v6eval/lib/Pz/PvAction.cc,v 1.22 2001/10/12 04:56:16 tanaka Exp $
 */
#include "PvAction.h"
#include "MmObject.h"
#include "WObject.h"
#include "PControl.h"
#include "PvOctets.h"
#include <stdio.h>

//======================================================================
PvAction::PvAction(const MObject* m,CSTR f,int l):PvObject(f,l),meta_(m) {}
PvAction::PvAction(const PvAction& x):PvObject(x),meta_(x.meta_) {}
PvAction::~PvAction() {}
void PvAction::print() const {
	CSTR s=meta()->string();
	printf(s);}

//======================================================================
PvAUTO::PvAUTO(const MObject* m,CSTR f,int l):PvAction(m,f,l){}
PvAUTO::PvAUTO(const PvAUTO& x):PvAction(x){}
PvAUTO::~PvAUTO() {}
PvObject* PvAUTO::shallowCopy() const {return new PvAUTO(*this);}

bool PvAUTO::generate(WControl& cntr,WObject* wmem,OCTBUF& buf) const {
	return wmem->geneAuto(cntr,buf);}

PvObject* PvAUTO::generateValue(WObject* w) const {
	return w->geneAutoValue();}

PvObject* PvAUTO::evaluateValue(WObject* w) const {
	return w->evalAutoValue();}

//======================================================================
PvANY::PvANY(const MObject* m,CSTR f,int l):PvAction(m,f,l){}
PvANY::PvANY(const PvANY& x):PvAction(x){}
PvANY::~PvANY() {}
PvObject* PvANY::shallowCopy() const {return new PvANY(*this);}

bool PvANY::generate(WControl& c,WObject*,OCTBUF&) const {
	generateNotAllow("any"); c.result(1);
	return c;}

int32_t PvANY::compareOctets(const PvOctets&) const {return 0;}
int32_t PvANY::compareNumber(int32_t) const {return 0;}
int32_t PvANY::compareObject(const PObject&) const {return 0;}
int32_t PvANY::compareObjectWith(WControl&,const PObject&) const {return 0;}

void PvANY::print() const {
	printf("ANY");}

//======================================================================
PvSTOP::PvSTOP(const MObject* m,CSTR f,int l):PvAction(m,f,l){}
PvSTOP::PvSTOP(const PvSTOP& x):PvAction(x){}
PvSTOP::~PvSTOP() {}
PvObject* PvSTOP::shallowCopy() const {return new PvSTOP(*this);}

bool PvSTOP::generate(WControl& c,WObject*,OCTBUF&) const {
	generateNotAllow("stop"); c.result(1);
	return c;}

int32_t PvSTOP::compareObjectWith(WControl& w,const PObject&) const {
	w.setStop();
	return 0;}

//======================================================================
PvFILL::PvFILL(const MObject* m,CSTR f,int l):PvAction(m,f,l){}
PvFILL::PvFILL(const PvFILL& x):PvAction(x){}
PvFILL::~PvFILL() {}
PvObject* PvFILL::shallowCopy() const {return new PvFILL(*this);}

bool PvFILL::generate(WControl& cntr,WObject* /*wmem*/,OCTBUF& /*buf*/) const {
#ifdef YDC_YET_IMPL
	return wmem->geneFILL(cntr,buf);
#endif
	return cntr;}

//======================================================================
PvZERO::PvZERO(const MObject* m,CSTR f,int l):PvAction(m,f,l){}
PvZERO::PvZERO(const PvZERO& x):PvAction(x){}
PvZERO::~PvZERO() {}
PvObject* PvZERO::shallowCopy() const {return new PvZERO(*this);}

bool PvZERO::generate(WControl& cntr,WObject* /*wmem*/,OCTBUF& /*buf*/) const {
#ifdef YDC_YET_IMPL
	return wmem->geneZERO(cntr,buf);
#endif
	return cntr;}

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
 * $TAHI: v6eval/lib/Pz/PvAutoItem.cc,v 1.7 2001/10/12 04:56:17 tanaka Exp $
 */
#include "PvAutoItem.h"
#include "MmObject.h"
#include "PvOctets.h"
#include "PControl.h"
#include "WObject.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////
PvHCgene::PvHCgene(McObject* meta,HCgenefunc func):PvObject(),
	meta_(meta),HCgenefunc_(func){}
PvHCgene::PvHCgene(const PvHCgene& x):PvObject(x),
	meta_(x.meta_),HCgenefunc_(x.HCgenefunc_){}
PvHCgene::~PvHCgene(){}
PvObject* PvHCgene::shallowCopy() const{return new PvHCgene(*this);}

bool PvHCgene::generate(WControl& cntr,WObject* wmem,OCTBUF& buf)const{
	return (meta_->*HCgenefunc_)(cntr,wmem,buf);}	//call hardcoding

PvObject* PvHCgene::generateValue(WObject*)const{
	return 0;}

PvObject* PvHCgene::evaluateValue(WObject*)const{
	printf("evaluate on PvHCgene!!\n");
	return 0;}

void PvHCgene::print() const{printf("HCgene");}

//////////////////////////////////////////////////////////////////////////////
PvHCeval::PvHCeval(McObject* meta,HCevalfunc func):PvObject(),
	meta_(meta),HCevalfunc_(func){}
PvHCeval::PvHCeval(const PvHCeval& x):PvObject(x),
	meta_(x.meta_),HCevalfunc_(x.HCevalfunc_){}
PvHCeval::~PvHCeval(){}
PvObject* PvHCeval::shallowCopy() const{return new PvHCeval(*this);}

bool PvHCeval::generate(WControl& cntr,WObject*,OCTBUF&)const{
	printf("generate on PvHCeval!!\n");
	abort();
	return cntr;}

PvObject* PvHCeval::evaluateValue(WObject* wmem)const{
	return (meta_->*HCevalfunc_)(wmem);}		//call hardcoding

void PvHCeval::print() const{printf("HCeval");}

//////////////////////////////////////////////////////////////////////////////
PvMUSTDEF::PvMUSTDEF():PvObject(){}
PvMUSTDEF::PvMUSTDEF(const PvMUSTDEF& x):PvObject(x){}
PvMUSTDEF::~PvMUSTDEF(){}
PvObject* PvMUSTDEF::shallowCopy() const{return new PvMUSTDEF(*this);}

bool PvMUSTDEF::generate(WControl&,WObject* w,OCTBUF&) const {
	return w->mustDefine(this);}

PvObject* PvMUSTDEF::generateValue(WObject* w)const{
	w->mustDefine(this);
	return 0;}
PvObject* PvMUSTDEF::evaluateValue(WObject* w) const {
	w->mustDefine(this);
	return 0;}

void PvMUSTDEF::print() const{ printf("MUSTDEF");}

const PvMUSTDEF* PvMUSTDEF::must() {return &must_;}
PvMUSTDEF PvMUSTDEF::must_;


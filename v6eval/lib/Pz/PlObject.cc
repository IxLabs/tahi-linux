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
 * $TAHI: v6eval/lib/Pz/PlObject.cc,v 1.32 2001/10/12 04:56:16 tanaka Exp $
 */
#include "PcObject.h"
#include "PvOctets.h"
#include "PlObject.h"
#include "McObject.h"
#include "WObject.h"
#include <stdio.h>
#include <typeinfo>
//======================================================================
// PlObject
PlObject::PlObject(const MObject* m,CSTR f,int l):PObject(f,l),
	meta_(m),rvalue_(0) {}
PlObject::~PlObject() {}
PObject* PlObject::rvalue(PObject* r) {rvalue_=r; return this;}

void PlObject::print() const {
	const PObject* r=rvalue();
	printf("%s",meta()->string());
	if(r!=0) {r->print();}
	printf("\n");}

bool PlObject::generate(WControl& c,WObject* w,OCTBUF& s) const {
	const PObject* r=rvalue();
	return (r!=0)?r->generate(c,w,s):w->geneAuto(c,s);}

PvObject* PlObject::generateValue(WObject* w) const {
	const PObject* r=rvalue();
	return (r!=0)?r->generateValue(w):w->geneAutoValue();}

PvObject* PlObject::evaluateValue(WObject* w) const {
	const PObject* r=rvalue();
	return (r!=0)?r->evaluateValue(w):w->evalAutoValue();}

uint16_t PlObject::width() const {
	const PObject* r=rvalue();
	return r!=0?r->width():0;}

uint32_t PlObject::objectLength(const WObject* w) const {
	const PObject* r=rvalue();
	return r!=0?r->objectLength(w):0;}

bool PlObject::isEqualMeta(const MObject* m) const {
	return meta()==m;}

uint32_t PlObject::operation() const {
	const PObject* r=rvalue();
	return (r!=0)?r->operation():PObject::operation();}

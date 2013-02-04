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
 * $TAHI: v6eval/lib/Pz/ItPosition.cc,v 1.5 2001/10/12 04:56:14 tanaka Exp $
 */
#include "ItPosition.h"
#include <stdio.h>
ItPosition::ItPosition():bytes_(0),bits_(0) {}
ItPosition::ItPosition(int32_t o,uint16_t b):bytes_(o),bits_(b) {}
ItPosition::ItPosition(const ItPosition& i):bytes_(i.bytes()),bits_(i.bits()) {}
void ItPosition::addBytes(int32_t o) {bytes_+=o;}
void ItPosition::addBits(uint16_t w) {
	uint16_t n=bits()+w;
	addBytes(n/8);
	bits(n%8);}
void ItPosition::add(const ItPosition& i) {
	addBits(i.bits());
	addBytes(i.bytes());}

void ItPosition::subtractBytes(int32_t o) {bytes_-=o;}
void ItPosition::subtractBits(uint16_t w) {
	uint16_t n=bits();
	if(n>=w) {n-=w;}
	else {
		uint16_t d=w-n;
		subtractBytes(d/8+1);
		n=8-d;}
	bits(n%8);}
void ItPosition::subtract(const ItPosition& i) {
	subtractBytes(i.bytes());
	subtractBits(i.bits());}

ItPosition& ItPosition::operator=(const ItPosition& i) {
	if(this!=&i) {
		bytes(i.bytes());
		bits(i.bits());}
	return *this;}

ItPosition& ItPosition::operator+=(const ItPosition& i) {
	add(i);
	return *this;}

ItPosition& ItPosition::operator-=(const ItPosition& i) {
	subtract(i);
	return *this;}

bool ItPosition::operator==(const ItPosition& y) const {
	uint16_t mi=bits();
	uint32_t my=bytes()+mi/8; mi%=8;
	uint16_t yi=y.bits();
	uint32_t yy=y.bytes()+yi/8; yi%=8;
	return (my==yy && mi==yi);}

bool ItPosition::operator<(const ItPosition& y) const {
	uint16_t mi=bits();
	uint32_t my=bytes()+mi/8; mi%=8;
	uint16_t yi=y.bits();
	uint32_t yy=y.bytes()+yi/8; yi%=8;
	return (my<yy)||(my==yy&&mi<yi);}

bool ItPosition::operator<=(const ItPosition& y) const {
	return (*this<y || *this==y);}

bool ItPosition::operator>(const ItPosition& y) const {
	uint16_t mi=bits();
	uint32_t my=bytes()+mi/8; mi%=8;
	uint16_t yi=y.bits();
	uint32_t yy=y.bytes()+yi/8; yi%=8;
	return (my>yy)||(my==yy&&mi<yi);}

bool ItPosition::operator>=(const ItPosition& y) const {
	return (*this>y || *this==y);}

bool ItPosition::operator!=(const ItPosition& y) const {
	return !(*this==y);}

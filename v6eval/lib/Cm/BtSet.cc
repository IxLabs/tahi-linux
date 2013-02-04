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
 * $TAHI: v6eval/lib/Cm/BtSet.cc,v 1.6 2001/10/12 04:56:13 tanaka Exp $
 */
#include "BtSet.h"
//IMPLEMENTATION
const u_long noObjectEntry=0xffffffff;
u_long BtSet::noOfElements() const {return noOfElements_;}
u_long BtSet::size() const {return capacity();}
BtSet::~BtSet() {noOfElements_=0;}

BtSet& BtSet::operator=(const BtSet& s) {
	return (BtSet&)BtArray::operator=(s);}

BtSet::BtSet(u_long s):BtArray(s>1?s*2:2),noOfElements_(0) {}

BtSet::BtSet(const BtSet &a):BtArray(a.capacity()),noOfElements_(0) {
	*this=a;}

BtSet::BtSet(ELEMENTS elm,...):BtArray(defaultArraySize*2),noOfElements_(0) {
	addElementsFrom(elm);}

uint32_t BtSet::hashElement(const ELEMENTS o) const {
	u_long x=(long)o;
	return x;}

bool BtSet::isSame(const ELEMENTS a,const ELEMENTS b) const {
	return a==b;}

ELEMENTS BtSet::findAt(const ELEMENTS o,u_long& s) const {
	s=noObjectEntry;
	u_long size=capacity(), u;
	if(size==0) return 0;
	u_long hash=hashElement(o);
	ELEMENTS* top=elements();
	ELEMENTS* end=top+size;
	ELEMENTS* p;
	for(p=top+(hash%size),u=size;u>0;p++,u--) {
		ELEMENTS c;
		if(p>=end) {p=top;}
		if(((c=*p)==0) || isSame(c,o)) {
			s=p-top; return c;}}
	return 0;}

void BtSet::rehash(u_long at) {
	u_long size=capacity(), u, i;
	ELEMENTS* top=elements();
	for(i=at+1,u=size-1;u>0;i++,u--) {
		if(i>=size) {i=0;}
		ELEMENTS o=top[i];
		if(o==0) return;	/* ÉÔÏ¢Â³ */
		u_long hash=hashElement(o)%size;
		if((at<i)?(hash<=at||i<hash):(i<hash&&hash<=at)) {
			top[at]=o; top[i]=0; at=i;}}}

ELEMENTS BtSet::addIfAbsent(ELEMENTS o,u_long& s) {
	u_long c=capacity();
	u_long n=noOfElements();
	if(n*3>c*2) expand();
	ELEMENTS old=findAt(o,s);
	u_long u=s;
	if(old==0&&u!=noObjectEntry) {
		noOfElements_++; atPut(u,o);
		return o;}
	return old;}

//----------------------------------------------------------------------
// ADDED:	o
// EXIST:	0
ELEMENTS BtSet::add(ELEMENTS o) {
	u_long n;
	if(o==0) return 0;
	ELEMENTS add=addIfAbsent(o,n);
	return add==o?o:0;}

//----------------------------------------------------------------------
// ADDED:	add
// EXIST:	delete o and return old
ELEMENTS BtSet::filter(ELEMENTS o) {
	u_long n;
	if(o==0) return 0;
	ELEMENTS add=addIfAbsent(o,n);
	if(add!=0 && add!=o) {
		deleteElement(o); return add;}
	return add;}

//----------------------------------------------------------------------
// REPLACED:	old
// ADDED:	0
// NOT ADDED:	0
ELEMENTS BtSet::replace(ELEMENTS o) {
	u_long n;
	if(o==0) return 0;
	ELEMENTS add=addIfAbsent(o,n);
	if(add!=0) {
		if(add!=o) {atPut(n,o);}
		return add==o?0:add;}
	return 0;}

ELEMENTS BtSet::remove(const ELEMENTS o) {
	u_long n;
	ELEMENTS old=findAt(o,n);
	if(old==0) return 0;
	removeAt(n); noOfElements_--;
	rehash(n);
	return old;}

void BtSet::expand() {
	noOfElements_=0;
	BtArray::expand();}

void BtSet::deleteContents() {
	BtArray::deleteContents();
	noOfElements_=0;}

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
 * $TAHI: v6eval/lib/Cm/BtArray.cc,v 1.7 2001/10/12 04:56:13 tanaka Exp $
 */
#include "BtArray.h"
//IMPLEMENTATION
u_long BtArray::size() const {return capacity();}
u_long BtArray::position(const u_long n) const {
	if(n>=size()) {rangeError(n);}
	return n;}

u_long BtArray::noOfElements() const {
	u_long i, i9=size(), t=0;
	for(i=0;i<i9;i++) {
		ELEMENTS c=index(i);
		if(c!=0) {t++;}}
	return t;}

ELEMENTS* BtArray::emptyYourself() {
	if(elements_!=0) {
		ELEMENTS *p, *e;
		for(p=elements_,e=p+capacity();p<e;p++) {*p=0;}}
	return elements_;}

void BtArray::removeArray() {
	delete [] elements_; elements_=0;
	capacity_=0;}

ELEMENTS* BtArray::newCapacity(const u_long s) {
	if(elements_) {removeArray();}
	elements_=new ELEMENTS[capacity_=s];
	return emptyYourself();}

void BtArray::addElements(ELEMENTS elm,va_list ap) {
	ELEMENTS p;
	for(p=elm;p!=0;p=va_arg(ap,ELEMENTS)) {add(p);}}

BtArray::~BtArray() {
	removeArray();}

BtArray& BtArray::operator=(const BtArray& a) {
	u_long s=a.capacity();
	ELEMENTS* from=a.elements();
	copyElements(s,from);
	return *this;}

BtArray::BtArray(const u_long s):capacity_(0),elements_(0) {
	if(s>0) {newCapacity(s);}}

BtArray::BtArray(const BtArray &a):capacity_(0),elements_(0) {
	*this=a;}

BtArray::BtArray(ELEMENTS elm,...):capacity_(0),elements_(0) {
	newCapacity(defaultArraySize);
	addElementsFrom(elm);}

ELEMENTS BtArray::atPut(const u_long n,ELEMENTS o) {
	u_long pos=position(n);
	ELEMENTS* array=elements();
	ELEMENTS old=array[pos];
	array[pos]=o;
	return old;}

ELEMENTS BtArray::add(ELEMENTS o) {
	u_long i=0;
	for(;;) {
		u_long i9=capacity();
		for(;i<i9;++i) {
			if(elements_[i]==0) {return elements_[i]=o;}}
		expand();}}

void BtArray::copyElements(const u_long s,ELEMENTS *from) {
	emptyYourself();
	if(from==0) {return;}			// Nothing to copy
	if(s>capacity()) {newCapacity(s);}
	for(u_long i=0;i<s;++i){
		ELEMENTS c=from[i];
		if(c) add(c);}}

//----------------------------------------------------------------------
// Elements owner changes to caller. caller must delete old
ELEMENTS* BtArray::oldElements(u_long& s) {
	s=capacity();
	ELEMENTS* old=elements(); elements_=0;
	return old;}

void BtArray::expand() {
	u_long s=0;
	ELEMENTS *old=oldElements(s);
	newCapacity((s+1)*2);
	copyElements(s,old);
	delete [] old;}

void BtArray::deleteElement(ELEMENTS) {}

void BtArray::deleteContents() {
	if(elements_) {
		ELEMENTS *p, *e;
		for(p=elements_,e=p+capacity();p<e;p++) {
			deleteElement(*p); *p=0;}}
	emptyYourself();}

ELEMENTS BtArray::removeAt(const u_long n) {
	ELEMENTS old=elements_[n];
	elements_[n]=0;
	return old;}

ELEMENTS BtArray::findAt(const ELEMENTS o,u_long& s) const {
	u_long i, i9=size();
	for(i=0;i<i9;i++) {
		ELEMENTS c=index(i);
		if(o==c) {
			s=i;
			return c;}}
	return 0;}

ELEMENTS BtArray::find(const ELEMENTS o) const {
	u_long n;
	return findAt(o,n);}

ELEMENTS BtArray::remove(const ELEMENTS o) {
	u_long n;
	ELEMENTS old=findAt(o,n);
	if(old!=0) {removeAt(n);}
	return old;}

ELEMENTS BtArray::addIfAbsent(ELEMENTS o) {
	ELEMENTS found=find(o);
	return (found==0)?add(o):found;}

#include <stdio.h>
#include <stdlib.h>
void BtArray::rangeError(const u_long n) const {
	eoutf("internal error:index %ld out of range %ld\n",n,size());
	abort();}

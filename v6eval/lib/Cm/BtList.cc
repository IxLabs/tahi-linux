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
 * $TAHI: v6eval/lib/Cm/BtList.cc,v 1.5 2001/10/12 04:56:13 tanaka Exp $
 */
#include "BtList.h"
//IMPLEMENTATION
u_long BtList::noOfElements() const {return noOfElements_;}
u_long BtList::size() const {return noOfElements();}
BtList& BtList::operator=(const BtList& l) {
	return (BtList&)BtArray::operator=(l);}

void BtList::removeArray() {
	BtArray::removeArray();
	noOfElements_=0,free_=0;}

ELEMENTS BtList::add(ELEMENTS elm) {return append(elm);}

BtList::BtList(const u_long s):BtArray(s),noOfElements_(0),free_(0) {}

BtList::BtList(const BtList &a):BtArray(a.capacity()),noOfElements_(0),free_(0) {
	*this=a;}

BtList::BtList(ELEMENTS elm,...):BtArray(defaultArraySize),noOfElements_(0),free_(0) {
	addElementsFrom(elm);}

u_long BtList::position(const u_long n) const {
	if(n>=noOfElements()) {rangeError(n);}
	return n<free_?leftPosition(n):rightPosition(n);}

void BtList::leftToRight(const u_long n) {
	ELEMENTS* array=elements();
	u_long r=rightPosition(n), l=leftPosition(n);
	array[r]=array[l]; array[l]=0;}
void BtList::leftToRight(const u_long s,const u_long e) {
	long i,i9=e;
	for(i=s;i>=i9;--i) {leftToRight(i);}}

void BtList::rightToLeft(const u_long n) {
	ELEMENTS* array=elements();
	u_long r=rightPosition(n), l=leftPosition(n);
	array[l]=array[r]; array[r]=0;}
void BtList::rightToLeft(const u_long s,const u_long e) {
	u_long i;
	for(i=s;i<e;++i) {rightToLeft(i);}}

ELEMENTS* BtList::array(const u_long s,const u_long count) {
	ELEMENTS* array=elements();
	if(s>=free_) {return array+rightPosition(s);}
	rightToLeft(free_,s+count);
	free_=s+count;
	return array+s;}

ELEMENTS BtList::insert(const u_long at,ELEMENTS o) {
	u_long n=noOfElements();
	if(at>n) {rangeError(at);}
	if(n>=capacity()) {expand();}
	ELEMENTS* array=elements();
	/**/ if(at<free_) {leftToRight(free_-1,at);}
	else if(at>free_) {rightToLeft(free_,at);}
	free_=at+1; noOfElements_+=1;
	return array[at]=o;}

ELEMENTS BtList::removeAt(const u_long at) {
	ELEMENTS* array=elements(), old=0;
	u_long pos=position(at);
	old=array[pos]; array[pos]=0;
	if(noOfElements()<capacity()) {
		/**/ if(at<free_) {leftToRight(free_-1,at+1);}
		else if(at>free_) {rightToLeft(free_,at);}}
	free_=at; noOfElements_-=1;
	return old;}

ELEMENTS* BtList::emptyYourself() {
	noOfElements_=0; free_=0;
	return BtArray::emptyYourself();}

BtList::~BtList() {
	removeArray();}

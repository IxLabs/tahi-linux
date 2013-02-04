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
 * $TAHI: v6eval/lib/pkt/Ringbuf.cc,v 1.10 2001/10/05 11:41:20 tanaka Exp $
 */

#include "Ringbuf.h"
#include <string.h>

Ringbuf::Ringbuf (uint32_t size, uint32_t len):size_(size),len_(len+1){
	start = new char *[len_];
	wp = rp = start;
	for(uint32_t i=0; i < len_; i++, wp++)
		*wp = new char [size];
	wp = start;}

Ringbuf::~Ringbuf (){
	char ** tmp = start;
	for(uint32_t i=0; i < size_; i++, tmp++)
		delete *tmp;
	delete start;}

char * Ringbuf::read(char *buf){
	if(isEmpty()) return NULL;
	char *ret = *rp;
	if(buf!=0) memcpy(buf,*rp,size_);
	rp = nextPoint(rp);
	return ret;}

char * Ringbuf::readn(uint32_t n, char *buf){
	if(isEmpty()) return NULL;
	char **ret=start + ((rp - start + n) % len_);
	if(buf!=0) memcpy(buf,*ret,size_);
	return *ret;}

char * Ringbuf::write(char *data, uint32_t size){
	char * ret = NULL;
	if(!isFull()){
		memcpy(*wp,data,size);
		ret = *wp;
		wp = nextPoint(wp);
	} else if(mode_==eWRAP){
		memcpy(*wp,data,size);
		ret = *wp;
		wp = nextPoint(wp);
		rp = nextPoint(rp);}
	return ret;}

int Ringbuf::isEmpty() const { return (wp == rp); }

int Ringbuf::isFull() const  { return (rp - wp == 1 || wp == (rp + len_ - 1));}

char ** Ringbuf::nextPoint(char ** p) const{
	if(p == start + len_ - 1) return start;
	else return p+1;}

uint32_t Ringbuf::datanum() const {
	if( wp >= rp ) return wp-rp;
	else return wp-rp+len_;}

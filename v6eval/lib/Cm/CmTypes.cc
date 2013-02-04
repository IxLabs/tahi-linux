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
 * $TAHI: v6eval/lib/Cm/CmTypes.cc,v 1.7 2001/10/12 04:56:14 tanaka Exp $
 */
#include "CmTypes.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/errno.h>
static fmtOutputFunction eoutFunc_=0;
static fmtOutputFunction ooutFunc_=0;
fmtOutputFunction regEoutFunc(fmtOutputFunction f) {
	fmtOutputFunction o=eoutFunc_;
	eoutFunc_=f;
	return o;}

fmtOutputFunction regOoutFunc(fmtOutputFunction f) {
	fmtOutputFunction o=ooutFunc_;
	ooutFunc_=f;
	return o;}

int eoutf(CSTR fmt,...) {
	int rc=0;
	va_list v;
	va_start(v,fmt);
	if(eoutFunc_==0) {
		rc=vfprintf(stderr,fmt,v);}
	else {
		rc=(*eoutFunc_)(fmt,v);}
	va_end(v);
	return rc;}

int ooutf(CSTR fmt,...) {
	int rc=0;
	va_list v;
	va_start(v,fmt);
	if(ooutFunc_==0) {
		rc=vprintf(fmt,v);}
	else {
		rc=(*ooutFunc_)(fmt,v);}
	va_end(v);
	return rc;}

void eerr(CSTR fmt) {
	CSTR e=strerror(errno);
	if(fmt==0) {
		eoutf("%s\n",e);}
	else {
		eoutf("%s: %s\n",fmt,e);}}

uint32_t basicHash(CSTR s,int32_t l) {
	if(s==0||l<0) return 0;
	uint32_t v=0;
	CSTR e=s+l;
	for(;s<e;s++) {
		char c=*s;
		v=(v<<1)^(c);}
	uint32_t t=v>>10;
	t^=(t>>10);
	return v^t;}

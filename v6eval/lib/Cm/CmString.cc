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
 * $TAHI: v6eval/lib/Cm/CmString.cc,v 1.12 2001/10/12 04:56:14 tanaka Exp $
 */
#include "CmString.h"
//IMPLEMENTATION
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "CmTypes.h"
#if !defined(__FreeBSD__) && !defined(__bsdi__)
#include <alloca.h>
#endif

void CmString::set(CSTR s,long l) {
	data_=s;
	length_=l;
}

CmString::CmString():data_(0),length_(0) {
}

CmString::CmString(CSTR s) {
	set(s,s!=0?strlen(s):0);
}

CmString::CmString(CSTR s, long n) {
	set(s,n);
}

CmString::CmString(const CmString& s) {
	set(s.string(),s.length());
}

CmString::~CmString() {}

CmString& CmString::operator=(const CmString& s) {
	if(this!=&s) {
		set(s.string(),s.length());}
	return *this;
}

CmString& CmString::operator=(CSTR s) {
	set(s,s!=0?strlen(s):0);
	return *this;
}

uint32_t CmString::hash() const {
	int32_t l=length();
	CSTR s=string();
	return basicHash(s,l<0?strlen(s):l);}

int CmString::compare(CSTR s) const {
	CSTR ms=string();
	int r=0;
	/**/ if(ms!=0&&s!=0) {r=strcmp(ms,s);}
	else if(ms==0&&s==0) {r=0;}
	else if(ms!=0&&s==0) {r=1;}
	else if(ms==0&&s!=0) {r=-1;}
	return r;}

int CmString::compare(const CmString& s) const {
	return compare(s.string());}

bool CmString::operator==(CSTR s) const {
	return compare(s)==0;}

bool CmString::operator==(const CmString& s) const {
	return operator==(s.string());}

bool CmString::like(CSTR s) const {
	CSTR ms=string();
	return (ms!=0&&s!=0)?(strncmp(ms,s,length())==0):(ms==s);}

bool CmString::case_insensitive_equal(const CmString& s) const {
	long l=length();
	if(l!=s.length()) {
		return false;}
	CSTR p = string();
	CSTR q = s.string();
	CSTR e = p+l;
	for (; p < e; p++, q++) {
		long c1 = *p;
		long c2 = *q;
		if (c1 != c2 && tolower(c1) != tolower(c2)) {
			return false;
		}
	}
	return true;
}

bool CmString::case_insensitive_equal(CSTR s) const {
	return case_insensitive_equal(CmString(s));
}

/*
* A negative value for start initializes the position at the end
* of the string before indexing.  Any negative length makes
* the substring extend to the end of the string.
*/

CmString CmString::substr(long start,long len) const {
	long l=length();
	if (start >= l || start < -l) {
		/* should raise exception */
		return *this;
	}
	long pos = (start >= 0) ? start : (l + start);
	if (pos + len > l) {
		/* should raise exception */
		return *this;
	}
	long nl=(len>=0)?len:(l-pos);
	return CmString(data_+pos,nl);
}

void CmString::set_to_substr(long start,long len) {
	long l=length();
	if (start >= l || start < -l) {
		/* should raise exception */
		return;
	}
	long pos = (start >= 0) ? start : (l + start);
	if (pos + len > l) {
		/* should raise exception */
		return;
	}
	long nl = (len >= 0) ? len : (l - pos);
	set(data_+pos,nl);
}

/*
* A negative value for start initializes the position to the end
* of the string before indexing and searches right-to-left.
*/

long CmString::search(long start, char c) const {
	long l=length();
	if (start >= l || start < -l) {
		/* should raise exception */
		return -1;
	}
	CSTR s=string();
	CSTR p=s+(0<=start?start:0);
	CSTR e=s+l+(start<0?start:0);
	long pos=-1;
	long len;
	for(; p<e; p+=len){
		len=mblen(p,2);
		if(*p==c){
			pos=p-s;
			if(start>=0) break;}}
	return pos;
}

/*
* Convert a string to binary value.
*/

bool CmString::convert(int& value) const {
	CSTR s = string();
	STR ptr;
	value = s!=0?strtol(s, &ptr, 0):0;
	return ptr != s;
}

bool CmString::convert(long& value) const {
	CSTR s = string();
	STR ptr;
	value = s!=0?strtol(s, &ptr, 0):0;
	return ptr != s;
}

bool CmString::convert(float& value) const {
	CSTR s = string();
	STR ptr;
	value = s!=0?(float)strtod(s, &ptr):0.;
	return ptr != s;
}

bool CmString::convert(double& value) const {
	CSTR s = string();
	STR ptr;
	value = s!=0?strtod(s, &ptr):0.;
	return ptr != s;
}

/* class CmCString */

CmCString::CmCString():CmString(),allocated_(0) {}

CmCString::CmCString(CSTR s):CmString(),allocated_(0) {
	set(s,s!=0?strlen(s):0);
}

CmCString::CmCString(CSTR s, long length):CmString(),allocated_(0) {
	set(s, length);
}

CmCString::CmCString(const CmString& s):CmString(),allocated_(0) {
	set(s.string(), s.length());
}

CmCString::CmCString(const CmCString& s):CmString(),allocated_(0) {
	set(s.string(), s.length());
}

CmCString::~CmCString() {
	free();
}

/*
* Guarantee null-terminated string for compatibility with prlongf et al.
*/

void CmCString::set(CSTR s,long l) {
	long nl=l+1;
	STR ns=(STR)string();
	if(allocated_<nl) {
		free();
		nl=round8(nl);
		ns=new char[allocated_=nl];}
	if(l>0) {strncpy(ns,s,l);}
	ns[l]='\0';
	CmString::set(ns,l);
}

void CmString::free() {
	delete [] data_;
	CmString::set(0,0);
}

void CmCString::free() {
	CmString::free();
	allocated_=0;
}

CmCString& CmCString::operator+=(CSTR as) {
	long ml=length();
	long al=as!=0?strlen(as):0;
	long l=ml+al;
	STR ns=(STR)alloca(l+1);
	CSTR ms=string();
	CSTR e;
	STR t=ns;
	for(e=ms+ml;ms<e;ms++,t++) {
		*t=*ms;}
	for(e=as+al;as<e;as++,t++) {
		*t=*as;}
	*t=0;
	set(ns,l);
	return *this;}

implementCmSet(StringSet, CmString);
implementCmList(StringList, CmString);
implementCmList(CStringList, CmCString);

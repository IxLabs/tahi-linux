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
 * $TAHI: v6eval/lib/Cm/CmString.h,v 1.8 2001/10/12 04:56:14 tanaka Exp $
 */
#ifndef _Cm_CmString_h_
#define _Cm_CmString_h_	1
/* Interface Definition */
#include "CmTypes.h"
#include "CmCltn.h"
//class ostream;

class CmString {
protected:
	CSTR data_;
	long length_;
public:
	CmString();
	CmString(CSTR);
	CmString(CSTR,long);
	CmString(const CmString&);
virtual ~CmString();

inline	CSTR string() const;
inline	long length() const;
	CmString& operator=(const CmString&);
	CmString& operator=(CSTR);

virtual uint32_t hash() const;
	bool isEqual(const CmString*) const;

	int compare(const CmString&) const;
virtual int compare(CSTR) const;
virtual	bool operator==(const CmString&) const;
	bool operator==(CSTR) const;
inline	bool operator!=(const CmString&) const;
inline	bool operator!=(CSTR) const;
inline	bool operator>(const CmString&) const;
inline	bool operator>=(const CmString&) const;
inline	bool operator<(const CmString&) const;
inline	bool operator<=(const CmString&) const;
inline	bool operator>(CSTR) const;
inline	bool operator>=(CSTR) const;
inline	bool operator<(CSTR) const;
inline	bool operator<=(CSTR) const;
virtual bool like(CSTR) const;

virtual bool case_insensitive_equal(const CmString&) const;
virtual bool case_insensitive_equal(CSTR) const;

	char operator[](long index) const;
virtual CmString substr(long start, long length) const;
	CmString left(long length) const;
	CmString right(long start) const;

virtual void set_to_substr(long start, long length);
	void set_to_left(long length);
	void set_to_right(long start);

virtual long search(long start, char) const;
	long index(char) const;
	long rindex(char) const;

virtual bool convert(int&) const;
virtual bool convert(long&) const;
virtual bool convert(float&) const;
virtual bool convert(double&) const;

//friend ostream& operator<<(ostream&, const CmString&);

protected:
virtual void set(CSTR,long);
virtual	void free();
};

interfaceCmSet(StringSet, CmString);
interfaceCmList(StringList, CmString);

class CmCString : public CmString {
	long allocated_;
public:
	CmCString();
	CmCString(CSTR);
	CmCString(CSTR,long);
	CmCString(const CmString&);
	CmCString(const CmCString&);
	CmCString& operator+=(CSTR s);
	CmCString& operator+=(const CmString& s);
virtual ~CmCString();

protected:
virtual void set(CSTR,long);
private:
	void free();
};

interfaceCmList(CStringList, CmCString);


inline CSTR CmString::string() const {return data_;}
inline long CmString::length() const {return length_;}
inline char CmString::operator [](long index) const {return data_[index];}

inline CmString CmString::left(long length) const {return substr(0, length);}
inline CmString CmString::right(long start) const {return substr(start, -1);}

inline void CmString::set_to_left(long length) {set_to_substr(0, length);}
inline void CmString::set_to_right(long start) {set_to_substr(start, -1);}

inline long CmString::index(char c) const {return search(0, c);}
inline long CmString::rindex(char c) const {return search(-1, c);}

inline bool CmString::operator!=(const CmString& s) const {
	return !operator==(s);}

inline bool CmString::operator!=(CSTR s) const {
	return !operator==(s);}

inline bool CmString::operator>(const CmString& s) const {
	return compare(s)>0;}

inline bool CmString::operator>(CSTR s) const {
	return compare(s)>0;}

inline bool CmString::operator>=(const CmString& s) const {
	return compare(s)>=0;}

inline bool CmString::operator>=(CSTR s) const {
	return compare(s)>=0;}

inline bool CmString::operator <(const CmString& s) const {
	return compare(s)<0;}

inline bool CmString::operator <(CSTR s) const {
	return compare(s)<0;}

inline bool CmString::operator<=(const CmString& s) const {
	return compare(s)<=0;}

inline bool CmString::operator <=(CSTR s) const {
	return compare(s)<=0;}

inline bool CmString::isEqual(const CmString* s) const {
	return operator==(*s);}

inline CmCString& CmCString::operator+=(const CmString& s) {
	return this->operator+=(s.string());}
#endif

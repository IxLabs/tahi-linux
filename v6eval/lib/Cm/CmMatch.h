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
 * $TAHI: v6eval/lib/Cm/CmMatch.h,v 1.4 2001/10/12 04:56:13 tanaka Exp $
 */
#ifndef _Cm_CmMatch_h_
#define _Cm_CmMatch_h_	1
#include "CmString.h"
#include "CmQueue.h"
class CmToken;
class CmMatch:public CmQueue {
private:
static	CmQueue top_;
	const CmToken* token_;
	CmCString string_;
	CmQueue org_;
//----------------------------------------------------------------------
// INTERFACE TO PUBLIC THRU THE STATIC
public:
static	const CmToken* lex(CSTR&);
static	const CmToken* findToken(CSTR,const CmQueue* =0);
static	CmMatch* addToken(const CmToken&,CmQueue* =0);
//----------------------------------------------------------------------
// MEMBER RETRIEVE
public:
inline	CSTR string() const;
inline	const CmToken* token() const;
inline	char initial() const;
protected:
inline	CmQueue& org();
//----------------------------------------------------------------------
// CONSTRUCTOR/DESTRUCTOR
protected:
	CmMatch(const CmToken*,CSTR);
virtual	~CmMatch();
//----------------------------------------------------------------------
// FINDING THE TOKEN MATCHES
protected:
static	const CmMatch* equalInitial(const CmQueue&,char);
static	const CmMatch* findString(const CmQueue&,CSTR);
	const CmMatch* findMatchString(CSTR) const;
//----------------------------------------------------------------------
// ADDING THE NEW TOKEN
protected:
inline	CmMatch* matchTokenString(const CmToken&,CSTR);
inline	void token(const CmToken*);
inline	void setString(CSTR);
	CmMatch* setToken(const CmToken&);
	CmMatch* split(int32_t l);
	CmMatch* newTokenString(const CmToken &,CSTR);
static	CmQueue* greaterEqualInitial(const CmQueue&,char,char&);
static	CmMatch* addTokenString(CmQueue&,const CmToken&,CSTR);
//----------------------------------------------------------------------
// TOP LEVEL OF QUEUE
protected:
static	CmQueue& top();
};
inline CSTR CmMatch::string() const {return string_.string();}
inline char CmMatch::initial() const {return *string();}
inline const CmToken* CmMatch::token() const {return token_;}
inline void CmMatch::token(const CmToken* t) {token_=t;}
inline CmQueue& CmMatch::top() {return top_;}
inline CmQueue& CmMatch::org() {return org_;}
inline void CmMatch::setString(CSTR s) {string_=s;}
inline CmMatch* CmMatch::newTokenString(const CmToken& t,CSTR ts) {
	return addTokenString(org(),t,ts);}
#endif

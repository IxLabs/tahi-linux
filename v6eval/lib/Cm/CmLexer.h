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
 * $TAHI: v6eval/lib/Cm/CmLexer.h,v 1.14 2003/12/04 04:59:47 akisada Exp $
 */
#ifndef _Cm_CmLexer_h_
#define _Cm_CmLexer_h_ 1
#include "CmTypes.h"
#include "CmString.h"
#include "CmToken.h"
#include <stdio.h>

#if !(defined(__FreeBSD__) && __FreeBSD__ >= 4)

const int32_t	YYERRCODE=256;

#endif

class PObject;
class McObject;
class MvAction;
class MvFunction;
class CmToken;
class CmLexer {
public:
	enum eNumBase {eBB_=0x1,eOO_=0x2,eDD_=0x4,eHH_=0x8};
private:
	FILE* iod_;
	STR current_;
	STR buffer_;
	unsigned int buffer_size;
	int lineNo_;
	CSTR fileName_;
static	CSTR _tag2Fmts[];
static	CSTR _tag1Fmts[];
static	uint32_t errorCount_;
static	uint32_t warningSuppress_;
static	char numTypes_[256];
static	StringSet nameSet_;
static	CmLexer* instance_;
public:
	CmLexer(CSTR);
virtual	~CmLexer();
static	void initialize();
static	CmLexer* instance();
	uint32_t errorCount() const;
//----------------------------------------------------------------------
// ERROR HANDLING
static	void eout(CSTR,int,char,CSTR,va_list);
static	void eouts(CSTR,int,char,CSTR,...);
static	void error(char,CSTR,...);
static	void uerror(CSTR,CSTR);
inline	void yaccError(CSTR s) const;
	CSTR fileName() const;
	int lineNo() const;
	void close();
	STR getLine();
protected:
//----------------------------------------------------------------------
// LEXER TOKEN HANDLING
virtual	STR nextToken();
	STR getLineTag(STR);
virtual	STR controls(STR);
	STR skipComment(STR);
virtual	CSTR nameLex(STR&,CmCString&);
virtual	int32_t digitLex(STR&);
virtual	CSTR stringLex(STR&,CmCString&);
//----------------------------------------------------------------------
// ITERATE BUFFER
	STR continueGetLine();
virtual	STR current() const;
virtual	STR current(STR);
inline	STR next(int32_t=1);
inline	STR buffer();
	STR readCompleteLine(int);
//----------------------------------------------------------------------
// TYPE OPERATION
static	bool isAlpha(int c);
static	bool isAlnum(int c);
static	void initNumTypes();
//----------------------------------------------------------------------
// FILE ATTRIBUTE
	FILE* open(CSTR);
	void fileName(CSTR);
	bool eof();
};
inline STR CmLexer::current() const {return current_;}
inline STR CmLexer::buffer() {return buffer_;}
inline STR CmLexer::current(STR s) {return (current_=s);}
inline STR CmLexer::next(int32_t n) {return (current_+=n);}
inline void CmLexer::yaccError(CSTR s) const {error('E',s);}
inline CmLexer* CmLexer::instance() {return instance_;}
inline uint32_t CmLexer::errorCount() const {return errorCount_;}
inline CSTR CmLexer::fileName() const {return fileName_;}
inline int CmLexer::lineNo() const {return lineNo_;}

extern int yydebug;
#endif

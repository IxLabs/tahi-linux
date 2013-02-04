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
 * $TAHI: v6eval/lib/Cm/CmLexer.cc,v 1.18 2008/04/02 07:35:30 akisada Exp $
 */
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <sys/param.h>
#include "CmMain.h"
#include "CmMatch.h"
#include "CmLexer.h"
#define	sharpComment	1
#define	sharpLineNotation	1
#define	slashSlashComment	1
#define	slashAsteriskComment	1
//----------------------------------------------------------------------
// controls:
// "@"	: dynamic debugging	YYDEBUG
// "#"	: comment		sharpComment
// "0"	: end of line
//				sharpLineNotation
// "/*"	: C comment		slashAsteriskComment
// "//"	: C++ comment		slashSlashComment
STR CmLexer::controls(STR s) {
	int c=*s;
	switch(c) {
#if defined(sharpComment)
		case '#': {
			return current((STR)"");}
#endif
#if defined(YYDEBUG)
		case '@': {
			STR start=(s=next());
			for(;isdigit(c=*s);s=next());
			*s=0; int n=atoi(start); *s=c;
			DBGFLAGS(n)^=1;
			return s;}
#endif
		case '/': {
#if defined(slashSlashComment)
			if(s[1]=='/') {return current((STR)"");}
#endif
#if defined(slashAsteriskComment)
			if(s[1]=='*') {return skipComment(next(2));}
#endif
			break;}
		case 0: {
			if((s=getLine())==0) {
				return 0;}
#if defined(sharpLineNotation)
			s=getLineTag(s);
#endif
			return s;}}
	return 0;}

//----------------------------------------------------------------------
// SCAN FOR ALPHA NUMERICS
CSTR CmLexer::nameLex(STR& s,CmCString& tmp) {
	STR p=s;
	int c=0;
	for(;isAlnum(c=*p);p=next());
	*p=0; tmp=s; *p=c;
	s=p;
	return tmp.string();}

//----------------------------------------------------------------------
// SCAN FOR NUMERICS
int32_t CmLexer::digitLex(STR& s) {
	int c=*s;
	int32_t v=0, base, mask;
	/**/ if(c!='0')				{base=10; mask=eDD_;}
	else if((c=*next())=='x'||c=='X')	{base=16; mask=eHH_; c=*next();}
	else if(c=='b'||c=='B')			{base=2; mask=eBB_; c=*next();}
	else					{base=8; mask=eOO_;}
	for(;numTypes_[c&0xff]&mask;c=*next()) {
		int32_t n=c<='9'?c-'0':10+(tolower(c)-'a');
		v=(v*base)+n;}
	s=current();
	return v;}

//----------------------------------------------------------------------
// SCAN FOR QUOTED STRING
CSTR CmLexer::stringLex(STR& s,CmCString& tmp) {
	STR p=0;
	int c0=*s;
	int c=0;
	int offset=s-buffer();	// Save offset in case buffer grows
	for(p=next();(c=*p)&&(c!=c0);p=next()) {
		if(c!='\\') continue;
		c=*(p=next());
		if(c==0||c=='\n') {
			p=next(-1);
			p=continueGetLine();}}
	s=buffer()+offset; // Reset offset just in case buffer grew
	*p=0; tmp=s+1; *p=c;
	if(!c) {error('E',"unterminated string");}
	else {next();}
	s=p;
	return tmp.string();}

//----------------------------------------------------------------------
// ERROR MESSAGE HANDLING
void CmLexer::eout(CSTR f,int l,char lvl,CSTR fmt,va_list v) {
	static CSTR errLevel[5]={
		"information","warning","error","sever error","unrecoverable"};
	static CSTR errFmt="\"%s\", line %d: %s: ";
	int level;
	switch(lvl) {
		case 'I':	level=0; break;
		case 'W':	level=1; break;
		case 'E':	level=2; break;
		case 'S':	level=3; break;
		case 'U':	level=4; break;
		default:	level=0; break;}
	if(warningSuppress_&&level<=1) return;
	char buf[BUFSIZ];
	int n=snprintf(buf,sizeof(buf),errFmt,f!=0?f:"",l,errLevel[level]);
	n+=vsnprintf(buf+n,sizeof(buf)-n,fmt,v);
	CmLexer* lx=CmLexer::instance();
	STR s=lx!=0?lx->current():0;
	if(s!=0) {
		s++; char x=*s; *s=0;
		n+=snprintf(buf+n,sizeof(buf)-n," near \'%s\'",lx->buffer()); *s=x;}
	snprintf(buf+n,sizeof(buf)-n,"\n");
	eoutf(buf);
	if(level>=3) exit(level);
	if(level>=2) {errorCount_++;}}

void CmLexer::eouts(CSTR f,int l,char lvl,CSTR fmt,...) {
	va_list v;
	va_start(v,fmt);
	eout(f,l,lvl,fmt,v);
	va_end(v);}

void CmLexer::error(char lvl,CSTR fmt,...) {
	CmLexer* lex=instance();
	int l=0;
	CSTR s=0;
	if(lex!=0) {
		l=lex->lineNo();
		s=lex->fileName();}
	va_list v;
	va_start(v,fmt);
	eout(s,l,lvl,fmt,v);
	va_end(v);}

void CmLexer::uerror(CSTR s,CSTR a) {
	if(!errorCount_) error(s[0],&s[2],a);
	else error('U',"cannot recover from earlier errors: good bye!");}

//----------------------------------------------------------------------
// CHARACTER CLASSIFICATION
bool CmLexer::isAlpha(int c) {return (isalpha(c)||c=='_');}
bool CmLexer::isAlnum(int c) {return (isalnum(c)||c=='_');}

void CmLexer::initNumTypes() {
	int i=0;
	for(i=0;i<256;i++) {numTypes_[i]=0;}
	for(i='0';i<='1';++i) {numTypes_[i]=eHH_|eDD_|eOO_|eBB_;}
	for(i='2';i<='7';++i) {numTypes_[i]=eHH_|eDD_|eOO_;}
	for(i='8';i<='9';++i) {numTypes_[i]=eHH_|eDD_;}
	for(i='a';i<='f';++i) {numTypes_[i]=eHH_;}
	for(i='A';i<='F';++i) {numTypes_[i]=eHH_;}}

void CmLexer::initialize() {
	static bool hasBeenHere=false;
	if(hasBeenHere) return;
	hasBeenHere=true;
	initNumTypes();}

//----------------------------------------------------------------------
// ADVANCING THE BUFFER
STR CmLexer::getLine() {
	STR s=current(readCompleteLine(0));
	if(DBGFLAGS(2)) {puts(s!=0?s:"EOF");}
	return s;}

STR CmLexer::continueGetLine() {
	STR s=current(readCompleteLine(current()-buffer()));
	if(DBGFLAGS(2)) {puts(s!=0?s:"EOF");}
	return s;}

STR CmLexer::skipComment(STR s) {
	for(;;s=next()) {
		for(;*s==0;) {
			s=getLine();
			if(s==0) {return s;}}
		if(s[0]=='*'&&s[1]=='/') {s=next(2); break;}}
	return s;}

STR CmLexer::readCompleteLine(int offset) {
	bool completeLineRead=false;
	int  newoffset=offset;
	++lineNo_;
	// Keep reading until we read a complete line
	STR s=fgets(buffer_+newoffset,buffer_size-newoffset,iod_);
	if (s) {
		do {
			if (strlen(buffer_)==(buffer_size-1)) {
				if ((buffer_[buffer_size-2]=='\n')||feof(iod_)) {
					completeLineRead=true;
				}
				else {
					STR temp=buffer_;
					int temp_size=buffer_size;
					buffer_size*=2;
					buffer_=new char[buffer_size+4];
					memcpy(buffer_,temp,temp_size);
					delete temp;
					newoffset=strlen(buffer_);
					fgets(buffer_+newoffset,buffer_size-newoffset,iod_);
				}
			}
			else {
				completeLineRead=true;
			}
		} while (!completeLineRead);
	s=buffer_+offset;}
	return s;}

//----------------------------------------------------------------------
// if controls shown then rescan from current
STR CmLexer::nextToken() {
	STR s=0;
	int c=0;
	for(;(s=current());){
		for(;isspace(c=*s);s=next());
		if(!controls(s)) break;}
	return current();}

STR CmLexer::getLineTag(STR s) {
	CSTR *ps;
	int line;
	char tmpName[MAXPATHLEN];
	for(ps=_tag2Fmts;*ps!=0;ps++)
		if(sscanf(s,*ps,&line,tmpName)==2){
			fileName(tmpName);
			lineNo_=line-1;
			return current((STR)"");}
	for(ps=_tag1Fmts;*ps!=0;ps++)
		if(sscanf(s,*ps,&line,tmpName)==1) {
			lineNo_=line-1;
			return current((STR)"");}
	return s;}

FILE* CmLexer::open(CSTR s) {
	FILE* iod=fopen(s,"r");
	if(iod==0) {eoutf(" cannot open %s\n",s);}
	return iod;}

void CmLexer::close() {
	if(iod_!=0) {fclose(iod_);}
	iod_=0; lineNo_=0; current_=0; fileName_=0;}

void CmLexer::fileName(CSTR s) {
	CmString tmp(s);
	CmString* name=nameSet_.find(&tmp);
	if(name==0) {
		name=new CmCString(s);
		nameSet_.add(name);}
	fileName_=name->string();}

CmLexer::CmLexer(CSTR s):iod_(0),current_(0),buffer_(0),buffer_size(BUFSIZ),lineNo_(0),fileName_(0) {
	fileName(s);
	buffer_=new char[buffer_size+4];
	buffer_[0]=0;
	current_=buffer_;
	iod_=s!=0?open(s):stdin;
	instance_=this;
	if(iod_==0) {exit(1);}}

CmLexer::~CmLexer() {
	delete buffer_;
	close();
	instance_=0;}

bool CmLexer::eof() {return (feof(iod_));}

CSTR CmLexer::_tag2Fmts[]={
	"# %d \"%[^\"]\"",
	"# line %d \"%s[^\"]\"",
	"#%d \"%s[^\"]\"",
	"#line %d \"%s[^\"]\"",
	0};
CSTR CmLexer::_tag1Fmts[]={
	"# %d",
	"#%d",
	"# line %d",
	"#line %d",
	0};
uint32_t CmLexer::errorCount_=0;
uint32_t CmLexer::warningSuppress_=0;
char CmLexer::numTypes_[256];
CmLexer* CmLexer::instance_=0;
StringSet CmLexer::nameSet_;

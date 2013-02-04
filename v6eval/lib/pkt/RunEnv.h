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
 * $TAHI: v6eval/lib/pkt/RunEnv.h,v 1.26 2009/08/27 00:10:04 akisada Exp $
 */
#if !defined(__RunEnv_h__)
#define	__RunEnv_h__	1
#include "CmString.h"
#include "timeval.h"
#include <stdio.h>
class IfName:public CmCString {
private:
	CmCString interface_;
	CmCString ether_;
public:
	IfName(CSTR,CSTR,CSTR);
	~IfName();
static	IfName* create(CSTR,CSTR,CSTR,CSTR,uint32_t);
inline	CSTR interface() const;
inline	CSTR ether() const;
	void print(void*,...) const;
};
inline CSTR IfName::interface() const {return interface_.string();}
inline CSTR IfName::ether() const {return ether_.string();}

#include "CmCltn.h"
interfaceCmSet(IfNameSet,IfName);

typedef IfName* (*IfNameCreator)(CSTR,CSTR,CSTR,CSTR,uint32_t);
class CmMain;
class RunEnv {
public:
static	CSTR packetDefinition(CmMain&,StringList&,bool&);
static	CSTR defineFile();
static	CSTR unixPath(CmMain&,StringList* =0);
static	CSTR defaultVirtualIF();
static	CSTR interfaceUnixPath();
static	CSTR interface();
static	uint32_t bufferSize();
static	timeval* tvtimeout();
static	CSTR filter();
static	timeval seek();
static	uint32_t count();
static	void print();
static	IfName* tester(CSTR =0);
static	IfName* target(CSTR =0);
private:
static	CmCString interfaceUnixPath_;
//----------------------------------------------------------------------
// Information from TN(TESTING NODE) Config FILE
//----------------------------------------------------------------------
static	IfNameSet tnSet_;
static	CmCString* socketpath_;
static	CmCString* filterrule_;
//----------------------------------------------------------------------
// Information from NUT(NODE UNDER TEST) Config FILE
//----------------------------------------------------------------------
static	IfNameSet nutSet_;
//----------------------------------------------------------------------
// commad			pktbuf	pktctl	pktsend	pktrecv
// option
//----------------------------------------------------------------------
// -t: (TN file)	STR	O	O	O	O
// -i: (IF name)	STR	O	O	O	O
// -n: (NUT file)	STR	-	-	O	O
// -p: (PKT def)	STR	-	-	O	O
// -e: (timeout)	STR	-	-	-	O
// -s: (seek)		STR	-	-	-	O
// -c: (count)		NUM	-	-	-	O
// -l: 			NUM	O	O	O	O
// -d: 			NUM	O	O	O	O
// -m: (buffer size)	NUM	O	-	-	-
//----------------------------------------------------------------------
static	CmCString* tnFile_;
static	CmCString* ifName_;
static	CmCString* nutFile_;
static	CmCString* pktDef_;
static	uint32_t settimeout_;
static	timeval tvtimeout_;
static	timeval seek_;
static	uint32_t count_;
static	uint32_t bufsize_;
protected:
//----------------------------------------------------------------------
static	FILE* configFile(CmString*,CSTR,CSTR,CmCString&);
static	bool doTNline(STR,IfNameCreator,CSTR,uint32_t);
static	bool doTNfile(IfNameCreator);
static	bool doNUTline(STR,IfNameCreator,CSTR,uint32_t);
static	bool doNUTfile(IfNameCreator);
static	bool doOption(STR*,StringList&);
//----------------------------------------------------------------------
// SETING MEMBER FUNCTIONS
static	void socketpath(CSTR);
static	void filterrule(CSTR);
static	bool tnFile(CSTR);
static	bool ifName(CSTR);
static	bool nutFile(CSTR);
static	bool pktDef(CSTR);
static	bool timeout(CSTR);
static	void calctvtimeout();
static	bool seek(CSTR);
static	bool count(CSTR);
static	bool log(CSTR);
static	bool dbg(CSTR);
static	bool hexdump(void);
static	bool bufsize(CSTR);
static	bool setString(CmCString*&,CSTR,CSTR =0);
static	CSTR searchPath(CSTR,CSTR,CmCString&);
static	void printString(CSTR,const CmString*);
static	const uint32_t defaultBufferSize;
static	const CSTR defaultInterfaceName_;
static	const uint32_t interfaceMatchLen;
static	const CSTR defaultSocketDir_;
static	const CSTR defaultCongigDirEnv_;
static	const CSTR defaultSocketDirEnv_;
static	const CSTR defaultTNfileName_;
static	const CSTR defaultNUTfileName_;
static	const CSTR defaultDefSearchPath_;
};
#endif

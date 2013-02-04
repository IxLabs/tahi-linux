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
 * $TAHI: v6eval/lib/Pz/PvName.h,v 1.16 2001/10/12 04:56:17 tanaka Exp $
 */
#if !defined(__PvName_h__)
#define	__PvName_h__	1
#include "CmTypes.h"
#include "CmString.h"
#include "PvObject.h"
class PcObject;
class NameSet;
class NameDefine:public CmCString {
private:
	PcObject* declaration_;
static	NameSet nameSet_;
public:
	NameDefine(CSTR s);
virtual	~NameDefine();
inline	PcObject* declaration() const;
	void declaration(PcObject*);
static	NameSet& nameSet();
virtual	void print() const;
	void printUndefine();
static	void printUndefines();
};
inline PcObject* NameDefine::declaration() const {return declaration_;}
inline void NameDefine::declaration(PcObject* o) {declaration_=o;}
inline NameSet& NameDefine::nameSet() {return nameSet_;}

class PvName:public PvObject {
private:
	NameDefine* definition_;
public:
static	PvName* tokenObject(CSTR,int,CSTR);
// COMPOSE INTERFACE ----------------------------------------------------------
virtual	WObject* selfCompose(WControl&,WObject* w_parent) const;
//----------------------------------------------------------------------
// Parse TREE interface
inline	NameDefine* definition() const;
inline	PcObject* declaration() const;
	void declaration(PcObject*);
virtual	bool isName() const;
	PvName(NameDefine*,CSTR,int);
	PvName(const PvName&);
virtual	~PvName();
virtual PvObject* shallowCopy() const;
inline	CSTR nameString() const;
virtual	void print() const;
virtual	void printArgument() const;
};
inline NameDefine* PvName::definition() const {return definition_;}
inline PcObject* PvName::declaration() const {
	const NameDefine* nd=definition();
	return nd!=0?nd->declaration():0;}
inline CSTR PvName::nameString() const {
	const NameDefine* nd=definition();
	return nd!=0?nd->string():0;}
#include "CmCltn.h"
interfaceCmSet(NameSet,NameDefine);
#endif

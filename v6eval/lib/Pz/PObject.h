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
 * $TAHI: v6eval/lib/Pz/PObject.h,v 1.48 2005/05/09 09:35:24 akisada Exp $
 */
#if !defined(__PObject_h__)
#define __PObject_h__	1
#include <stdarg.h>
#include "CmTypes.h"
#include "pzCommon.h"
struct timeval;
class MObject;
class TObject;
class WObject;
class WControl;
class RObject;
class PvObject;
class PvOctets;
class PvNumber;
class PvV6Addr;
class PvName;
class PObjectList;

class PObject {
private:
	CSTR file_;
	int line_;
public:
	PObject(CSTR=0,int=0);
	PObject(const PObject&);
virtual	~PObject();
	bool isEqual(const PObject*) const;
	bool isEqualObject(const PObject*) const;
virtual CSTR metaString() const;
virtual CSTR nameString() const;
virtual const MObject* findMember(CSTR) const;
// COMPOSE INTERFACE ----------------------------------------------------------
virtual	WObject* selfCompose(WControl&,WObject* w_parent) const;
virtual bool check_nameCompose(WControl&,WObject* w_parent,const PvName&) const;
// COMPOSE INTERFACE(for Collection elementsPerform) --------------------------
	void vselfCompose(void* v,va_list ap) const;
	void vmatchselfCompose(void* v,va_list ap) const;
//----------------------------------------------------------------------
virtual	const MObject* meta() const;
virtual	bool isEqualMeta(const MObject*) const;
virtual	int32_t compareOctets(const PvOctets&) const;
virtual	int32_t compareNumber(int32_t) const;
virtual	int32_t compareTimeval(const timeval&) const;
virtual	int32_t compareCalc() const;
virtual	int32_t compareObject(const PObject&) const;
virtual	int32_t compareObjectWith(WControl&,const PObject&) const;
//----------------------------------------------------------------------
// Parse TREE interface
public:
virtual	PObject* rvalue(PObject*);
virtual	PObject* name(PObject*);
virtual	PObject* member(PObject*);
virtual	PObject* argument(PObject*);
virtual const PObject* rvalue() const;
virtual const PObjectList& members()const;
virtual const PObjectList& args()const;
virtual	int32_t intValue(bool&) const;
virtual	CSTR strValue(bool&) const;
virtual	COCTSTR octetsValue(bool&) const;
virtual	PvOctets* octetString() const;
virtual	bool isV6Addr() const;
virtual	bool isOctets() const;
virtual	bool isName() const;
virtual	uint32_t operation() const;
//----------------------------------------------------------------------
virtual	void generateNotAllow(CSTR) const;
virtual	bool generate(WControl&,WObject*,OCTBUF&) const;
virtual	PvObject* generateValue(WObject*) const;
virtual	PvObject* evaluateValue(WObject*) const;
virtual	bool generateV6Addr(PvV6Addr&) const;
virtual	bool evaluateV6Addr(PvV6Addr&) const;
virtual	bool generateOctetsWith(PvOctets&,WObject*) const;
//----------------------------------------------------------------------
// SA MEMBER YAC INTERFACE
virtual PObject* pad_member(PObject*);
virtual PObject* crypt_member(PObject*);
virtual PObject* auth_member(PObject*);
virtual PObject* bsa_member(PObject*);
virtual PObject* ike_p2_hash_member(PObject*);
virtual PObject* dhcpauth_member(PObject*);
//----------------------------------------------------------------------
virtual	PObject* unary(PObject*);
virtual	PObject* node(PObject*,PObject*);
virtual	const PObject* lvalue() const;
virtual	const PObject* unary() const;
virtual bool matchesWith(const PObject&) const;
virtual	const PObject* corresponding(const PObject&) const;
virtual	const PObject* correspondingMeta(const MObject*) const;
public:
inline	int line() const;
inline	CSTR file() const;
virtual	void error(CSTR,...) const;
virtual	void print() const;
virtual	void printConvert() const;
virtual void printArgument() const;
virtual void log(uint32_t=0) const;
virtual	uint16_t width() const;
virtual	uint32_t length() const;
virtual	uint32_t objectLength(const WObject*) const;
};
inline	int PObject::line() const {return line_;}
inline	CSTR PObject::file() const {return file_;}

#include "CmCltn.h"
interfaceCmList(PObjectList,PObject);
#include "CmQueue.h"
class PoQueue:public CmQueue {
private:
	PObject* object_;
public:
	PoQueue(PObject* =0);
virtual	~PoQueue();
inline	PObject* object() const;
inline	void object(PObject*);
virtual	PObject* pop();
virtual	PObject* push(PObject*);
};
inline	PObject* PoQueue::object() const {return object_;}
inline	void PoQueue::object(PObject* o) {object_=o;}
#endif

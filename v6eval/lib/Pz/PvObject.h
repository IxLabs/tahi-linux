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
 * $TAHI: v6eval/lib/Pz/PvObject.h,v 1.44 2003/12/04 04:59:47 akisada Exp $
 */
#if !defined(__PvObject_h__)
#define __PvObject_h__	1
#include "PObject.h"
class PvNumber;
class PvOctets;
//======================================================================
class PvObject:public PObject {
public:
	PvObject(CSTR=0,int=0);
	PvObject(const PvObject&);
virtual	PvObject* shallowCopy() const;
virtual	~PvObject();
virtual void log(uint32_t=0) const;
virtual	PvObject* generateValue(WObject*) const;
virtual	PvObject* evaluateValue(WObject*) const;
// COMPOSE INTERFACE ----------------------------------------------------------
virtual	WObject* selfCompose(WControl&,WObject* w_parent) const;
};

class MObject;
//======================================================================
class PvOperator:public PvObject {
private:
	const MObject* meta_;
	PObject* rvalue_;
public:
	PvOperator(const MObject*,CSTR,int);
	PvOperator(const PvOperator&);//shallow
virtual	~PvOperator();
virtual	PvObject* shallowCopy() const;
//----------------------------------------------------------------------
// ENCODING/DECODING
virtual	bool generate(WControl&,WObject*,OCTBUF&) const;
virtual	PvObject* generateValue(WObject*) const;
virtual	PvObject* evaluateValue(WObject*) const;
//----------------------------------------------------------------------
// Parse TREE interface
virtual	PObject* rvalue(PObject*);
//----------------------------------------------------------------------
inline	PObject* rvalue() const;
virtual	const MObject* meta() const;
virtual	uint32_t operation() const;
virtual	uint32_t objectLength(const WObject*) const;
virtual	CSTR metaString() const;
virtual	void print() const;
};
inline PObject* PvOperator::rvalue() const {return rvalue_;}
inline const MObject* PvOperator::meta() const {return meta_;}

//======================================================================
class PFunction:public PvObject {
private:
	PObjectList args_;
public:
	PFunction(CSTR,int);
	PFunction(const PFunction&);
virtual	~PFunction();
//----------------------------------------------------------------------
// Parse TREE interface
virtual	PObject* argument(PObject*);
virtual const PObjectList& args()const;
virtual	void checkArgument() const;
//----------------------------------------------------------------------
virtual	const MObject* meta() const;
virtual	CSTR metaString() const;
virtual	void print() const;
};
inline const PObjectList& PFunction::args()const{return args_;}

class MvFunction;
//======================================================================
class PvFunction:public PFunction {
private:
	const MvFunction* meta_;
public:
	PvFunction(const MvFunction*,CSTR,int);
	PvFunction(const PvFunction&);//shallow
virtual	PvObject* shallowCopy() const;
virtual	~PvFunction();
//----------------------------------------------------------------------
// ENCODING/DECODING
virtual	bool generate(WControl&,WObject*,OCTBUF&) const;
virtual	bool generateV6Addr(PvV6Addr&) const;
virtual	bool evaluateV6Addr(PvV6Addr&) const;
virtual	bool generateOctetsWith(PvOctets&,WObject*) const;
virtual	PvObject* generateValue(WObject*) const;
virtual	PvObject* evaluateValue(WObject*) const;
virtual	int32_t compareObject(const PObject&) const;
//----------------------------------------------------------------------
// Parse TREE interface
virtual	void checkArgument() const;
virtual	bool isV6Addr() const;
virtual	bool isOctets() const;
//----------------------------------------------------------------------
virtual	const MObject* meta() const;
	const MvFunction* metaClass() const;
virtual	uint32_t objectLength(const WObject*) const;
};
inline const MvFunction* PvFunction::metaClass() const {return meta_;}

class PvNumberSet;
//======================================================================
class PvNumber:public PvObject {
private:
	int32_t value_;
static	PvNumberSet numbers_;
public:
	PvNumber();
	PvNumber(int32_t);
	PvNumber(const PvNumber&);
virtual	~PvNumber();
virtual	PvObject* shallowCopy() const;
//----------------------------------------------------------------------
// ENCODING/DECODING
virtual	bool generate(WControl&,WObject*,OCTBUF&) const;
virtual	int32_t compareNumber(int32_t) const;
virtual	int32_t compareObject(const PObject&) const;
//----------------------------------------------------------------------
virtual	uint32_t objectLength(const WObject*) const;
virtual	int32_t intValue(bool&) const;
virtual	int32_t value() const;
virtual	void print() const;
	uint32_t hash() const;
	bool isEqual(const PvNumber*) const;
static	PvNumber* unique(int32_t);
	void value_set(int32_t val);
};
inline int32_t PvNumber::value() const {return value_;}
inline uint32_t PvNumber::hash() const {return value();}
inline bool PvNumber::isEqual(const PvNumber* p) const {return value()==p->value();}
inline void PvNumber::value_set(int32_t val){value_=val;}

#include "CmCltn.h"
interfaceCmSet(PvNumberSet,PvNumber);
interfaceCmList(PvNumberList,PvNumber);

//======================================================================
class PvNumbers:public PvObject {
private:
	PvNumberList numbers_;
public:
	PvNumbers();
	PvNumbers(const PvNumbers&);//shallow
virtual	~PvNumbers();
virtual	PvObject* shallowCopy() const;
//----------------------------------------------------------------------
// ENCODING/DECODING
virtual	bool generate(WControl&,WObject*,OCTBUF&) const;
//----------------------------------------------------------------------
virtual	PvOctets* octetString() const;
virtual	uint32_t length() const;
	void add(PObject*);
virtual	void print() const;
};

#include "CmString.h"
//======================================================================
class PvString:public PvObject {
private:
	CmCString value_;
public:
	PvString();
	PvString(CSTR);
	PvString(const PvString&);
virtual	~PvString();
virtual	PvObject* shallowCopy() const;
virtual	PvOctets* octetString() const;
virtual	uint32_t length() const;
virtual	CSTR strValue(bool&) const;
virtual	COCTSTR octetsValue(bool&) const;
virtual	CSTR value() const;
virtual	void print() const;
	void value_set(CSTR val);
};
inline void PvString::value_set(CSTR val){value_=val;}
inline CSTR PvString::value() const {return value_.string();}

#include "timeval.h"
//======================================================================
class PvTimeval:public PvObject {
private:
	timeval value_;
public:
	PvTimeval();
	PvTimeval(const timeval&);
	PvTimeval(const PvTimeval&);
virtual	~PvTimeval();
virtual	PvObject* shallowCopy() const;
virtual	uint32_t length() const;
virtual	int32_t compareTimeval(const timeval&) const;
virtual	int32_t compareObject(const PObject&) const;
virtual	const timeval& value() const;
virtual	void print() const;
};
inline const timeval& PvTimeval::value() const {return value_;}
#endif

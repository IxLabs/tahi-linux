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
 * $TAHI: v6eval/lib/Pz/PcObject.h,v 1.24 2001/10/12 04:56:16 tanaka Exp $
 */
#if !defined(__PcObject_h__)
#define	__PcObject_h__	1
#include "PObject.h"
#include "CmCltn.h"
class PvName;
class MObject;
class NameSet;
class StringList;
class PcObjectList;
class SAOctets;
class SAOctetsList;
class WAlgorithm;
class AlgorithmFrames;
//======================================================================
class PcObject:public PObject {
private:
	const MObject*	meta_;
	PvName*		name_;			// compound name
	PObjectList	members_;
static	NameSet frameSet_;
public:
	PcObject(const MObject*,CSTR,int);
virtual	~PcObject();
virtual	PObject* name(PObject*);
virtual	PObject* member(PObject*);
virtual	const MObject* meta() const;
virtual const MObject* findMember(CSTR) const;
virtual const PObjectList& members()const;
//----------------------------------------------------------------------
// COMPOSER interface
virtual	WObject* selfCompose(WControl&,WObject* w_parent) const;
virtual bool check_nameCompose(WControl&,WObject* w_parent,const PvName&) const;
//----------------------------------------------------------------------
// COMMON INTERFACE
inline	PvName* nameObject() const;
virtual	CSTR nameString() const;
virtual	void printCompound(CSTR) const;
virtual	void print() const;
//----------------------------------------------------------------------
static	NameSet& frameSet();
static	bool frameList(const StringList&,PcObjectList&);
static	void prints();
static	void frame(PObject*);
static	PcObject* frameName(CSTR);
//----------------------------------------------------------------------
	SAOctets* generateFrame() const;
static	bool generateFrames(const PcObjectList&,SAOctetsList&);
static	bool sendingFrames(const StringList&,SAOctetsList&);
//----------------------------------------------------------------------
	bool evaluatePrologue(AlgorithmFrames&) const;
static	bool evaluateComposers(const PcObjectList&,AlgorithmFrames&);
static	bool evaluateFrames(const StringList&,AlgorithmFrames&);
};

inline const MObject* PcObject::meta() const {return meta_;}
inline PvName* PcObject::nameObject() const {return name_;}
inline const PObjectList& PcObject::members()const{return members_;}
inline NameSet& PcObject::frameSet() {return frameSet_;}

interfaceCmList(PcObjectList,PcObject);
#endif

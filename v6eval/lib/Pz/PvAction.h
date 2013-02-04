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
 * $TAHI: v6eval/lib/Pz/PvAction.h,v 1.17 2001/10/12 04:56:16 tanaka Exp $
 */
#if !defined(__PvAction_h__)
#define __PvAction_h__	1

#include "PvObject.h"

class MObject;
class PvAction: public PvObject {
private:
	const MObject* meta_;
protected:
	PvAction(const MObject*,CSTR,int);
	PvAction(const PvAction&);
public:
virtual	~PvAction();
virtual	const MObject* meta() const;
virtual	void print() const;
};
inline const MObject* PvAction::meta() const {return meta_;}

class PvAUTO: public PvAction {
public:
	PvAUTO(const MObject* =0,CSTR=0,int=0);
	PvAUTO(const PvAUTO&);
virtual	~PvAUTO();
virtual	PvObject* shallowCopy() const;
virtual bool generate(WControl&,WObject* wmem,OCTBUF& buf)const;
virtual	PvObject* generateValue(WObject*) const;
virtual	PvObject* evaluateValue(WObject*) const;
};

class PvANY: public PvAction {
public:
	PvANY(const MObject* =0,CSTR=0,int=0);
	PvANY(const PvANY&);
virtual	~PvANY();
virtual	PvObject* shallowCopy() const;
virtual bool generate(WControl&,WObject* wmem,OCTBUF& buf)const;
virtual	int32_t compareOctets(const PvOctets&) const;
virtual	int32_t compareNumber(int32_t) const;
virtual	int32_t compareObject(const PObject&) const;
virtual	int32_t compareObjectWith(WControl&,const PObject&) const;
virtual	void print() const;
};

class PvSTOP: public PvAction {
public:
	PvSTOP(const MObject* =0,CSTR=0,int=0);
	PvSTOP(const PvSTOP&);
virtual	~PvSTOP();
virtual	PvObject* shallowCopy() const;
virtual bool generate(WControl&,WObject* wmem,OCTBUF& buf)const;
virtual	int32_t compareObjectWith(WControl&,const PObject&) const;
};

class PvFILL: public PvAction {
public:
	PvFILL(const MObject* =0,CSTR=0,int=0);
	PvFILL(const PvFILL&);
virtual	~PvFILL();
virtual	PvObject* shallowCopy() const;
virtual bool generate(WControl&,WObject* wmem,OCTBUF& buf)const;
};

class PvZERO: public PvAction {
public:
	PvZERO(const MObject* =0,CSTR=0,int=0);
	PvZERO(const PvZERO&);
virtual	~PvZERO();
virtual	PvObject* shallowCopy() const;
virtual bool generate(WControl&,WObject* wmem,OCTBUF& buf)const;
};

#endif

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
 * $TAHI: v6eval/lib/Pz/PvAutoItem.h,v 1.5 2001/10/12 04:56:17 tanaka Exp $
 */
#if !defined(__PvAutoItem_h__)
#define __PvAutoItem_h__

#include "PvObject.h"
#include "McObject.h"
#include "WObject.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////
/////// use only MmObject create Auto Parameter Items ////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
class PvHCgene: public PvObject {//Pc hardcoding func for generate Auto
private:
	McObject*	meta_;
	HCgenefunc	HCgenefunc_;
public:
	PvHCgene(McObject*,HCgenefunc);
	PvHCgene(const PvHCgene&);
virtual ~PvHCgene();
virtual PvObject* shallowCopy() const;
virtual bool generate(WControl&,WObject*,OCTBUF&) const;
virtual PvObject* generateValue(WObject*) const;
virtual PvObject* evaluateValue(WObject*) const;	//ErrorHandling
virtual void print() const;
};

//////////////////////////////////////////////////////////////////////////////
class PvHCeval: public PvObject {//Pc hardcoding func for evaluate Auto
private:
	McObject*	meta_;
	HCevalfunc	HCevalfunc_;
public:
	PvHCeval(McObject*,HCevalfunc);
	PvHCeval(const PvHCeval&);
virtual ~PvHCeval();
virtual PvObject* shallowCopy() const;
virtual bool generate(WControl&,WObject*,OCTBUF&) const;	//ErrorHandling
virtual PvObject* evaluateValue(WObject*) const;
virtual void print() const;
};

//////////////////////////////////////////////////////////////////////////////
class PvMUSTDEF: public PvObject {//if member rvalue undefined ,must be error
static	PvMUSTDEF must_;
public:
static	const PvMUSTDEF* must();
public:
	PvMUSTDEF();
	PvMUSTDEF(const PvMUSTDEF&);
virtual ~PvMUSTDEF();
virtual PvObject* shallowCopy() const;
virtual bool generate(WControl&,WObject*,OCTBUF&) const;//ErrorHandling
virtual PvObject* generateValue(WObject*) const;	//ErrorHandling
virtual PvObject* evaluateValue(WObject*) const;	//ErrorHandling
virtual void print() const;
};

#endif

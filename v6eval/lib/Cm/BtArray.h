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
 * $TAHI: v6eval/lib/Cm/BtArray.h,v 1.4 2001/10/12 04:56:13 tanaka Exp $
 */
#ifndef _Cm_BtArray_h_
#define _Cm_BtArray_h_	1
#include <stdarg.h>
#include "BtObject.h"
typedef void* ELEMENTS;
struct BtArray:public BtObject {
private:
	u_long capacity_;
	ELEMENTS* elements_;
public:
	BtArray& operator=(const BtArray&);
	BtArray(const u_long size=0);
	BtArray(const BtArray &);
	BtArray(ELEMENTS elm,...);
virtual	~BtArray();

virtual	u_long size() const;
virtual	u_long noOfElements() const;
	ELEMENTS index(const u_long n) const;
	ELEMENTS operator[](const u_long n) const;
	ELEMENTS at(const u_long n) const;
	ELEMENTS atPut(const u_long n,ELEMENTS o);
virtual	ELEMENTS add(ELEMENTS o);
virtual	ELEMENTS removeAt(const u_long n);
	ELEMENTS remove(const ELEMENTS o);
	ELEMENTS addIfAbsent(ELEMENTS o);
//----------------------------------------------------------------------
// Abstract Element type
virtual	void deleteElement(ELEMENTS o);
//----------------------------------------------------------------------
// GROUP OPERATION
	ELEMENTS find(const ELEMENTS o) const;
virtual	ELEMENTS* emptyYourself();
virtual	void deleteContents();
protected:
	void addElements(ELEMENTS elm,va_list ap);
virtual	ELEMENTS findAt(const ELEMENTS o,u_long& s) const;
virtual	u_long position(const u_long n) const;
virtual	void copyElements(const u_long n,ELEMENTS*);
virtual	void removeArray();
	ELEMENTS* oldElements(u_long& n);
	ELEMENTS* elements() const;
	u_long capacity() const;
virtual	void expand();
	void rangeError(const u_long n) const;
	ELEMENTS* newCapacity(const u_long);
};
inline	u_long BtArray::capacity() const {return capacity_;}
inline	ELEMENTS* BtArray::elements() const {return elements_;}
inline	ELEMENTS BtArray::index(const u_long n) const {return elements_[position(n)];}
inline	ELEMENTS BtArray::operator[](const u_long n) const {return index(n);}
inline	ELEMENTS BtArray::at(const u_long n) const {return index(n);}
const	u_long defaultArraySize=8;
#define addElementsFrom(arg)\
	va_list v;\
	va_start(v,arg);\
	addElements((ELEMENTS)arg,v);\
	va_end(v);
#endif

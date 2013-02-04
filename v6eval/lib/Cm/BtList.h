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
 * $TAHI: v6eval/lib/Cm/BtList.h,v 1.4 2001/10/12 04:56:13 tanaka Exp $
 */
#ifndef _Cm_BtList_h_
#define _Cm_BtList_h_	1
#include <stdlib.h>
#include <stdarg.h>
#include "BtArray.h"
struct BtList:public BtArray {
private:
	u_long noOfElements_, free_;
public:
	BtList& operator=(const BtList&);
	BtList(const u_long n=0);
	BtList(const BtList &);
	BtList(ELEMENTS,...);
virtual	~BtList();
virtual	u_long size() const;
virtual	u_long noOfElements() const;
	ELEMENTS* array(const u_long n,const  u_long count);
	ELEMENTS insert(const u_long n,ELEMENTS);
	ELEMENTS prepend(ELEMENTS elm);
	ELEMENTS append(ELEMENTS elm);
virtual	ELEMENTS add(ELEMENTS elm);
virtual	ELEMENTS removeAt(const u_long n);
virtual	void removeArray();
virtual	ELEMENTS* emptyYourself();
protected:
virtual	u_long position(const u_long n) const;
	u_long free() const;
private:
	ELEMENTS* createList(const u_long);
	u_long leftPosition(const u_long n) const;
	u_long rightPosition(const u_long n) const;
	void leftToRight(const u_long s,const u_long e);
	void rightToLeft(const u_long s,const u_long e);
	void leftToRight(const u_long n);
	void rightToLeft(const u_long n);
};

inline	u_long BtList::leftPosition(const u_long n) const {return n;}
inline	u_long BtList::rightPosition(const u_long n) const {return n+capacity()-noOfElements();}
inline	u_long BtList::free() const {return free_;}
inline	ELEMENTS BtList::prepend(ELEMENTS elm) {return insert(0,elm);}
inline	ELEMENTS BtList::append(ELEMENTS elm) {return insert(noOfElements(),elm);}
#endif

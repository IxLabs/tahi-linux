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
 * $TAHI: v6eval/lib/Pz/ItPosition.h,v 1.6 2001/10/12 04:56:14 tanaka Exp $
 */
#if !defined(__ItPosition_h__)
#define	__ItPosition_h__	1
#include "CmTypes.h"
//======================================================================
// POSITION ITERATOR
class ItPosition {
private:
	int32_t bytes_;
	uint16_t bits_;
public:
	ItPosition();
	ItPosition(int32_t,uint16_t =0);
	ItPosition(const ItPosition&);
//----------------------------------------------------------------------
// OPERATORS
public:
	ItPosition& operator=(const ItPosition&);
	ItPosition& operator+=(const ItPosition&);
	ItPosition& operator-=(const ItPosition&);
	bool operator==(const ItPosition&) const;
	bool operator<(const ItPosition&) const;
	bool operator<=(const ItPosition&) const;
	bool operator>(const ItPosition&) const;
	bool operator>=(const ItPosition&) const;
	bool operator!=(const ItPosition&) const;
//----------------------------------------------------------------------
// MEMBER MUNIPULATIONS
public:
	int32_t bytes() const;
	uint16_t bits() const;
	void bytes(int32_t);
	void bits(uint16_t);
//----------------------------------------------------------------------
// ADDING/SUBTRACTING
public:
	void addBytes(int32_t);
	void addBits(uint16_t);
	void add(const ItPosition &);
	void subtractBytes(int32_t);
	void subtractBits(uint16_t);
	void subtract(const ItPosition &);
};
inline int32_t ItPosition::bytes() const {return bytes_+bits_/8;}
inline void ItPosition::bytes(int32_t o) {bytes_=o;}
inline uint16_t ItPosition::bits() const {return bits_%8;}
inline void ItPosition::bits(uint16_t o) {bits_=o;}
#endif

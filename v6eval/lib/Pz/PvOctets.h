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
 * $TAHI: v6eval/lib/Pz/PvOctets.h,v 1.35 2003/12/04 04:59:47 akisada Exp $
 */
#if !defined(__PvOctets_h__)
#define	__PvOctets_h__	1
#include "pzCommon.h"
#include "PvObject.h"
class ItPosition;
class WControl;
class WObject;
class RObject;
class PvIfName;
//======================================================================
class PvOctets:public PvObject {
private:
	uint32_t allocated_;
	uint32_t length_;
	OCTSTR buffer_;
public:
	uint32_t length() const;
virtual	uint16_t bitWidth() const;
	COCTSTR string(uint32_t =0) const;
	OCTSTR buffer(uint32_t =0);
	PvOctets();
	PvOctets(const PvOctets&);
	PvOctets(uint32_t,OCTSTR=0,bool=false);
virtual	PvObject* shallowCopy() const;
virtual	~PvOctets();
	PvOctets* substr(uint32_t,uint32_t,PvOctets* =0);
	PvOctets* set(uint32_t,OCTSTR=0);
//----------------------------------------------------------------------
// ENCODING INTERFACE
//	RA: stand for Right Adjust 
//	LA: stand for Left Adjust 
	void encodeLA(octet,const ItPosition&,uint16_t);
	void encodeRA(octet,const ItPosition&,uint16_t);
	void encodeLAs(COCTSTR,const ItPosition&,uint16_t);
	void encodeRAs(COCTSTR,const ItPosition&,uint16_t);
	void encodeNUint(uint32_t,const ItPosition&,uint16_t);
	void encodeUint(uint32_t,const ItPosition&,uint16_t);
	void encode(const ItPosition&,const PvOctets&);
	void encodeZero(const ItPosition& at,const ItPosition& size);
//----------------------------------------------------------------------
	octet decodeLA(const ItPosition&,uint16_t) const;
	octet decodeRA(const ItPosition&,uint16_t) const;
	void decodeLAs(OCTSTR,const ItPosition&,uint16_t) const;
	void decodeRAs(OCTSTR,const ItPosition&,uint16_t) const;
	uint32_t decodeNUint(const ItPosition&,uint16_t) const;
	uint32_t decodeUint(const ItPosition&,uint16_t) const;
//----------------------------------------------------------------------
	uint32_t sum1Octet() const;
	uint32_t sum2Octets() const;
	uint32_t sum4Octets() const;
//----------------------------------------------------------------------
virtual	int32_t compareWidth(const PvOctets&,uint32_t) const;
virtual	int32_t compareOctets(const PvOctets&) const;
virtual	int32_t compareObject(const PObject&) const;
virtual	bool generate(WControl&,WObject*,OCTBUF&) const;
//----------------------------------------------------------------------
	PvOctets& operator=(const PvOctets&);
//----------------------------------------------------------------------
	uint32_t hash() const;
	bool isEqual(const PvOctets*) const;
virtual	COCTSTR octetsValue(bool&) const;
virtual	PvOctets* octetString() const;
virtual	int32_t addressFamily() const;
//----------------------------------------------------------------------
virtual	void zero();
virtual	void fill();
virtual	void print() const;
virtual	void dump(CSTR=0) const;
virtual	void log(uint32_t=0) const;
//----------------------------------------------------------------------
inline	int32_t remainLength(uint32_t) const;
inline	PvOctets* left(uint32_t,PvOctets* =0);
inline	PvOctets* right(uint32_t,PvOctets* =0);
static	void defaultNotUsed();
virtual	bool isOctets() const;
protected:
static	PvIfName* defaultTN_;
static	PvIfName* defaultNUT_;
static	PvIfName* defaultTN();
static	PvIfName* defaultNUT();
static const CSTR linkLocalPrefixPresentation_;
};
inline uint32_t PvOctets::length() const {return length_;}
inline COCTSTR PvOctets::string(uint32_t n) const {return buffer_+n;}
inline int32_t PvOctets::remainLength(uint32_t n) const {return length()-n;}
inline PvOctets* PvOctets::left(uint32_t l,PvOctets* p) {
	return substr(0,l,p);}
inline PvOctets* PvOctets::right(uint32_t o,PvOctets* p) {
	return substr(o,remainLength(o),p);}
inline uint32_t PvOctets::hash() const {
	return basicHash((CSTR)string(),(int)length());}
inline uint16_t PvOctets::bitWidth() const {return length_*8;}

//======================================================================
class PvV4Addr:public PvOctets {
private:
public:
	PvV4Addr(OCTSTR=0,bool=false);
	PvV4Addr(CSTR,bool&,OCTSTR=0);		// PRESENTATION
virtual	PvObject* shallowCopy() const;
virtual	~PvV4Addr();
	bool pton(CSTR);
virtual	void print() const;
virtual	void log(uint32_t=0) const;
virtual	int32_t addressFamily() const;
};

//======================================================================
class PvV6Addr:public PvOctets {
private:
static	PvV6Addr* linkLocal_;
public:
	PvV6Addr(OCTSTR=0,bool=false);
	PvV6Addr(CSTR,bool&,OCTSTR=0);		// RFC2373 PRESENTATION
virtual	PvObject* shallowCopy() const;
virtual	~PvV6Addr();
static	const PvV6Addr* TN();
static	const PvV6Addr* NUT();
static	const PvV6Addr& linkLocal();
	bool pton(CSTR);
	PvV6Addr* merge(uint16_t,const PvOctets&,PvV6Addr* =0) const;
	bool netMerge(CSTR,int,const PvV6Addr&);
virtual	void print() const;
virtual	void log(uint32_t=0) const;
virtual	bool isV6Addr() const;
virtual	int32_t addressFamily() const;
};

//======================================================================
class PvEther:public PvOctets {
private:
public:
	PvEther(OCTSTR=0,bool=false);
	PvEther(CSTR,bool&,OCTSTR=0);			// PRESENTATION
	PvEther(const PvV6Addr&,OCTSTR=0);	// V6 -> ETHER MULTICAST
virtual	PvObject* shallowCopy() const;
virtual	~PvEther();
static	const PvEther* TN();
static	const PvEther* NUT();
	bool pton(CSTR);
	void multicast(const PvV6Addr&);	// V6 -> ETHER MULTICAST
virtual	void print() const;
virtual	void log(uint32_t=0) const;
};

//======================================================================
class PvEUI64:public PvOctets {
private:
public:
	PvEUI64(OCTSTR=0,bool=false);
	PvEUI64(const PvEther&,OCTSTR=0);	// IEEE802 -> EUI-64
virtual	~PvEUI64();
	void create(const PvEther&);		// IEEE802 -> EUI-64
};

#include "CmCltn.h"
interfaceCmList(PvOctetsList,PvOctets);

//======================================================================
class PvCookie64: public PvOctets {
	public:
		PvCookie64(OCTSTR = 0, bool = false);
		virtual	~PvCookie64();
		virtual	void log(uint32_t=0) const;
		virtual	void print() const;
		static	const PvCookie64 *zerocookie();
		virtual	PvObject *shallowCopy() const;
};

//======================================================================
class PvBSA96: public PvOctets {
	public:
		PvBSA96(OCTSTR = 0, bool = false);
		virtual	~PvBSA96();
		virtual	void log(uint32_t=0) const;
		virtual	void print() const;
		static	const PvBSA96 *zerobsa();
		virtual	PvObject *shallowCopy() const;
};
#endif

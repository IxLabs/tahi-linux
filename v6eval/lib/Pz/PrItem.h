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
 * $TAHI: v6eval/lib/Pz/PrItem.h,v 1.3 2003/12/04 04:59:47 akisada Exp $
 */
#if !defined(__PrItem_h__)
#define __PrItem_h__

#include "ItPosition.h"
#include "PvObject.h"
#include "PvOctets.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////
// detailed print log abstruct class
//////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
typedef ItPosition		ItP;

class PrItem {
	const char*	name_;	//Item name
public:
	PrItem(const char* name=0):name_(name){}
	const char* get_name()const{return name_;}
	bool check_remainLength(const ItP& reqlen,
			const ItP& at,const PvOctets& buf)const;
	void print_tag(uint32_t t)const;
virtual	bool print(uint32_t t,ItP& at,const PvOctets& buf)const;
};


//////////////////////////////////////////////////////////////////////////////
// element memeber printer
//////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
class PrUint :public PrItem{//Uint member printer
	uint32_t	width_;	//bits width
public:
	PrUint(uint32_t width,const char* name):PrItem(name),width_(width){}
	uint32_t get_value(const ItP& at,const PvOctets& buf)const;
	void step(ItP& at)const{at.addBits(width_);}
	bool print(uint32_t t,ItP& at,const PvOctets& buf)const;
};

//---------------------------------------------------------------------------
template <class PVADDR>
class PrAddr :public PrItem{//Address member printer template
#ifdef	__DEFV6EVAL_GPLUS_IMPL_STATIC_MEMBER_TEMPLATE__
//g++(v2.7) not implemented: static data member templates
static	const PVADDR	base_;	//PvAddr base object
static	const PVADDR&	base(){return base_;}
#else
static	const PVADDR&	base(){	//PvAddr base object
		static const PVADDR base_;
		return base_;}
#endif
public:
	PrAddr(const char* name):PrItem(name){}
	void step(ItP& at)const{at.addBytes(base().length());}
	bool print(uint32_t t,ItP& at,const PvOctets& buf)const;
};

template <class PVADDR>
bool PrAddr<PVADDR>::print(uint32_t t,ItP& at,const PvOctets& buf)const{
	if(!check_remainLength(ItP(base().length()),at,buf))return false;
	print_tag(t);
	PVADDR value((OCTSTR)buf.string(at.bytes()));//decode PvAddr
	printf(" = "); value.log(t); printf("\n");
	step(at); //forward Itp& at
	return true;}

#ifdef	__DEFV6EVAL_GPLUS_IMPL_STATIC_MEMBER_TEMPLATE__
template <class PVADDR>
const PVADDR PrAddr<PVADDR>::base_;//PvAddr base object
#endif

typedef PrAddr<PvV4Addr>	PrV4Addr;
typedef PrAddr<PvV6Addr>	PrV6Addr;
typedef PrAddr<PvEther>		PrEther;

//---------------------------------------------------------------------------
class PrData :public PrItem{//Variable OctetsData member printer
public:
	PrData(const char* name):PrItem(name){}
	bool print(uint32_t t,ItP& at,const PvOctets& buf)const;
};


//////////////////////////////////////////////////////////////////////////////
// Abstruct printer compound & PrNeedless
//////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
class PrCompound :public PrItem{
protected:
	PrCompound(const char* name):PrItem(name){}
virtual	bool print_head(uint32_t t,uint32_t length)const;
virtual	bool print_body(uint32_t t,ItP& at,const PvOctets& bodybuf)const;
static	bool print_remain_Needless(uint32_t t,ItP& at,const PvOctets& bodybuf);
public:
virtual	uint32_t length_for_pr(const ItP& at,const PvOctets& buf)const;
	bool print(uint32_t t,ItP& at,const PvOctets& buf)const;
	bool print_remain_more0(uint32_t t,ItP& at,const PvOctets& buf)const;
};

//---------------------------------------------------------------------------
class PrNeedless :public PrCompound{
private:
	PrData	data_;
public:
	PrNeedless():PrCompound("[Needless]"),data_("data"){}
	uint32_t length_for_pr(const ItP& at,const PvOctets& buf)const{
		return buf.remainLength(at.bytes());}//all remain
	bool print_body(uint32_t t,ItP& at,const PvOctets& buf)const{
		data_.	print(t,at,buf);
		return true;}
};

//////////////////////////////////////////////////////////////////////////////
#endif

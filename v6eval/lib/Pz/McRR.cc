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
 * $TAHI: v6eval/lib/Pz/McRR.cc,v 1.4 2003/10/23 04:37:30 akisada Exp $
 */
#include "McRR.h"
#include "ItPosition.h"
#include "WObject.h"
#include "RObject.h"
#include "PControl.h"
#include "PvObject.h"
#include "PvOctets.h"
#include "CmMain.h"


//////////////////////////////////////////////////////////////////////////////
// particular RcObject for ICMPv6RR MessageBody printer
class RcICMPv6RR : public RcObject{
public:
	RcICMPv6RR(RObject*,const MObject*,
		const ItPosition& offset,PvObject* =0);
virtual	~RcICMPv6RR();
//
virtual	void printChild(uint32_t t) const ;
virtual	void logChild(uint32_t t) const ;
virtual void describeChild(uint32_t t) const ;
	void print_MessageBody(uint32_t t,
		uint32_t bodytype,const RObject& r_msgbody)const;
	uint32_t get_MessageBodyType()const{
		const McUpp_ICMPv6_RouterRenumbering* mc =
			(const McUpp_ICMPv6_RouterRenumbering*)meta();
		return mc->get_MessageBodyType(this);}
};


//////////////////////////////////////////////////////////////////////////////
// class McUpp_ICMPv6_RouterRenumbering
McUpp_ICMPv6_RouterRenumbering::McUpp_ICMPv6_RouterRenumbering(CSTR key):
	McUpp_ICMPv6(key){}
McUpp_ICMPv6_RouterRenumbering::~McUpp_ICMPv6_RouterRenumbering(){}

RObject* McUpp_ICMPv6_RouterRenumbering::reverseRc(RControl&,
		RObject* r_parent,const ItPosition& at,PvObject* pv)const{
	return new RcICMPv6RR(r_parent,this,at,pv);}

uint32_t McUpp_ICMPv6_RouterRenumbering::get_MessageBodyType(
		const RObject* rc)const{
	const TObject* coder =	rc->corresponding(Code_);
	if(!coder)return ~0x0;//want errlog?
	const PvNumber* pv =	(const PvNumber*)coder->pvalue();
	if(!pv)return ~0x0;//want errlog?
	uint32_t val = pv->value();
	return val;}

//////////////////////////////////////////////////////////////////////////////
#define CLASS	"RcICMPv6RR"
#define SUPER	RcObject

RcICMPv6RR::RcICMPv6RR(RObject* r_parent,const MObject* m,
                const ItPosition& offset,PvObject* pv):
        	SUPER(r_parent,m,offset,pv){}
RcICMPv6RR::~RcICMPv6RR() {}

void RcICMPv6RR::printChild(uint32_t t) const {
if(DBGFLAGS('R')){//stop detailed log
	SUPER::printChild(t);
	}
else{
	uint32_t bodytype = get_MessageBodyType();
	const RObject* next= nextChild();
	while(next){
		const RObject* nn= nextChild(next);
		if(!nn){//Lastmember detailed MessageBody print
			print_MessageBody(t+1,bodytype,*next);}
		else{	//normal print
			next->print(t+1);}
		next=nn;}
	}
	}

void RcICMPv6RR::logChild(uint32_t t) const {
if(DBGFLAGS('R')){//stop detailed log
	SUPER::logChild(t);
	}
else{
	uint32_t bodytype = get_MessageBodyType();
	const RObject* next= nextChild();
	while(next){
		const RObject* nn= nextChild(next);
		if(!nn){//Lastmember detailed MessageBody print
			print_MessageBody(t+1,bodytype,*next);}
		else{	//normal print
			next->log(t+1);}
		next=nn;}
	}
	}

#include <stdio.h>
void RcICMPv6RR::describeChild(uint32_t t) const {
#if 1
	uint32_t bodytype = get_MessageBodyType();
	switch(bodytype){
	case TP_Code_ICMPv6RR_Command:	printf("command"); break;
	case TP_Code_ICMPv6RR_Result:	printf("result"); break;
	case TP_Code_ICMPv6RR_Reset:	printf("reset"); break;
	default:			printf("any"); break;}
#endif
	SUPER::describeChild(t);
	}

#undef SUPER
#undef CLASS


//////////////////////////////////////////////////////////////////////////////
// detailed log print for RR MessageBody 
//////////////////////////////////////////////////////////////////////////////
#include "PrItem.h"

//////////////////////////////////////////////////////////////////////////////
// RR Command Message Body items
//
// PrRR_CommandMessageBody
// --1:N- PrRR_PrefixControlOperation
//        --1:1- PrRR_MatchPrefixPart
//        --1:N- PrRR_UsePrefixPart
// N: more than0
//////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
class PrRR_MatchPrefixPart :public PrCompound{
private:
	PrUint		OpCode_;
	PrUint		OpLength_;
	PrUint		Ordinal_;
	PrUint		MatchLen_;
	PrUint		MinLen_;
	PrUint		MaxLen_;
	PrUint		Reserved_;
	PrV6Addr	MatchPrefix_;
public:
	PrRR_MatchPrefixPart():PrCompound("MatchPrefixPart"),
		OpCode_(	 8,"OpCode"),
		OpLength_(	 8,"OpLength"),
		Ordinal_(	 8,"Ordinal"),
		MatchLen_(	 8,"MatchLen"),
		MinLen_(	 8,"MinLen"),
		MaxLen_(	 8,"MaxLen"),
		Reserved_(	16,"Reserved"),
		MatchPrefix_(	   "MatchPrefix"){}
	uint32_t length_for_pr(const ItP&,const PvOctets&)const{
		return 24;}//fix length
	uint32_t OpLength_for_pr(const ItP& at,const PvOctets& buf)const{
		int32_t need = length_for_pr(at,buf);
		int32_t limit = buf.remainLength(at.bytes());
		if(need<=limit){
			ItP memat=at;
			OpCode_.step(memat); //forward memat OpLength_
			uint32_t L = OpLength_.get_value(memat,buf);
			return L*8;}	//is PrefixControlOperation length
		return need;}
	bool print_body(uint32_t t,ItP& at,const PvOctets& buf)const{
		OpCode_.	print(t,at,buf);
		OpLength_.	print(t,at,buf);
		Ordinal_.	print(t,at,buf);
		MatchLen_.	print(t,at,buf);
		MinLen_.	print(t,at,buf);
		MaxLen_.	print(t,at,buf);
		Reserved_.	print(t,at,buf);
		MatchPrefix_.	print(t,at,buf);
		return true;}
};
//---------------------------------------------------------------------------
class PrRR_UsePrefixPart :public PrCompound{
private:
	PrUint		UseLen_;
	PrUint		KeepLen_;
	PrUint		FlagMask_;
	PrUint		RAFlags_;
	PrUint		ValidLifetime_;
	PrUint		PreferredLifetime_;
	PrUint		VFlag_;
	PrUint		PFlag_;
	PrUint		Reserved_;
	PrV6Addr	UsePrefix_;
public:
	PrRR_UsePrefixPart():PrCompound("UsePrefixPart"),
		UseLen_(		 8,"UseLen"),
		KeepLen_(		 8,"KeepLen"),
		FlagMask_(		 8,"FlagMask"),
		RAFlags_(		 8,"RAFlags"),
		ValidLifetime_(		32,"ValidLifetime"),
		PreferredLifetime_(	32,"PreferredLifetime"),
		VFlag_(		 	 1,"VFlag"),
		PFlag_(		 	 1,"PFlag"),
		Reserved_(	 	30,"Reserved"),
		UsePrefix_(	 	   "UsePrefix"){}
	uint32_t length_for_pr(const ItP&,const PvOctets&)const{
		return 32;}//fix length
	bool print_body(uint32_t t,ItP& at,const PvOctets& buf)const{
		UseLen_.		print(t,at,buf);
		KeepLen_.		print(t,at,buf);
		FlagMask_.		print(t,at,buf);
		RAFlags_.		print(t,at,buf);
		ValidLifetime_.		print(t,at,buf);
		PreferredLifetime_.	print(t,at,buf);
		VFlag_.			print(t,at,buf);
		PFlag_.			print(t,at,buf);
		Reserved_.		print(t,at,buf);
		UsePrefix_.		print(t,at,buf);
		return true;}
};
//---------------------------------------------------------------------------
class PrRR_PrefixControlOperation :public PrCompound{
private:
	PrRR_MatchPrefixPart	MatchPrefixPart_;	//must1
	PrRR_UsePrefixPart	UsePrefixPart_;		//more than0
public:
	PrRR_PrefixControlOperation():PrCompound("PrefixControlOperation"),
		MatchPrefixPart_(),
		UsePrefixPart_(){}
	uint32_t length_for_pr(const ItP& at,const PvOctets& buf)const{
		return MatchPrefixPart_.OpLength_for_pr(at,buf);}
	bool print_body(uint32_t t,ItP& at,const PvOctets& buf)const{
		bool rtn = true;
		if(rtn)rtn = MatchPrefixPart_.	print(t,at,buf);
		if(rtn)rtn = UsePrefixPart_.	print_remain_more0(t,at,buf);
		return rtn;}
};
//---------------------------------------------------------------------------
class PrRR_CommandMessageBody :public PrCompound{
static	const PrRR_CommandMessageBody* instance_;
private:
	PrRR_PrefixControlOperation	Operation_;//more than0
public:
	PrRR_CommandMessageBody():PrCompound("CommandMessageBody"),
		Operation_(){}
static	const PrRR_CommandMessageBody* instance(){
		if(!instance_)instance_=new PrRR_CommandMessageBody();
		return instance_;}
	uint32_t length_for_pr(const ItP& at,const PvOctets& buf)const{
		return buf.remainLength(at.bytes());}
	bool print_body(uint32_t t,ItP& at,const PvOctets& buf)const{
		bool rtn = true;
		rtn = Operation_.print_remain_more0(t,at,buf);
		return rtn;}	
};
const PrRR_CommandMessageBody* PrRR_CommandMessageBody::instance_=0;


//////////////////////////////////////////////////////////////////////////////
// RR Result Message Body items
//
// PrRR_ResultMessageBody
// --1:N- PrRR_ResultMessage
// N: more than0
//////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
class PrRR_ResultMessage :public PrCompound{
private:
	PrUint		Reserved_;
	PrUint		BFlag_;
	PrUint		FFlag_;
	PrUint		Ordinal_;
	PrUint		MatchedLen_;
	PrUint		InterfaceIndex_;
	PrV6Addr	MatchedPrefix_;
public:
	PrRR_ResultMessage():PrCompound("ResultMessage"),
		Reserved_(	14,"Reserved"),
		BFlag_(		 1,"BFlag"),
		FFlag_(		 1,"FFlag"),
		Ordinal_(	 8,"Ordinal"),
		MatchedLen_(	 8,"MatchedLen"),
		InterfaceIndex_(32,"InterfaceIndex"),
		MatchedPrefix_(	   "MatchedPrefix"){}
	uint32_t length_for_pr(const ItP&,const PvOctets&)const{
		return 24;}//fix length
	bool print_body(uint32_t t,ItP& at,const PvOctets& buf)const{
		Reserved_.	print(t,at,buf);
		BFlag_.		print(t,at,buf);
		FFlag_.		print(t,at,buf);
		Ordinal_.	print(t,at,buf);
		MatchedLen_.	print(t,at,buf);
		InterfaceIndex_.print(t,at,buf);
		MatchedPrefix_.	print(t,at,buf);
		return true;}
};
//---------------------------------------------------------------------------
class PrRR_ResultMessageBody :public PrCompound{
static	const PrRR_ResultMessageBody* instance_;
private:
	PrRR_ResultMessage	Result_;//more than0
public:
	PrRR_ResultMessageBody():PrCompound("ResultMessageBody"),
		Result_(){}
static	const PrRR_ResultMessageBody* instance(){
		if(!instance_)instance_=new PrRR_ResultMessageBody();
		return instance_;}
	uint32_t length_for_pr(const ItP& at,const PvOctets& buf)const{
		return buf.remainLength(at.bytes());}
	bool print_body(uint32_t t,ItP& at,const PvOctets& buf)const{
		bool rtn = true;
		if(rtn)rtn = Result_.print_remain_more0(t,at,buf);
		return rtn;}
};
const PrRR_ResultMessageBody* PrRR_ResultMessageBody::instance_=0;


//////////////////////////////////////////////////////////////////////////////
// RR Reset Message Body items
//
// PrRR_ResetMessageBody
// --nomember
//
//////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
class PrRR_ResetMessageBody :public PrCompound{
static	const PrRR_ResetMessageBody* instance_;
private:
	//no member
public:
	PrRR_ResetMessageBody():PrCompound("ResetMessageBody"){}
static	const PrRR_ResetMessageBody* instance(){
		if(!instance_)instance_=new PrRR_ResetMessageBody();
		return instance_;}
	uint32_t length_for_pr(const ItP&,const PvOctets&)const{
		return 0;}
	bool print_body(uint32_t,ItP&,const PvOctets&)const{
		bool rtn = true;
		//no member
		return rtn;}
};
const PrRR_ResetMessageBody* PrRR_ResetMessageBody::instance_=0;


//////////////////////////////////////////////////////////////////////////////
// RR Any Message Body items (unknow type)
//
// PrRR_AnyMessageBody
// --anydata
//
//////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
class PrRR_AnyMessageBody :public PrCompound{
static	const PrRR_AnyMessageBody*	instance_;
private:
	PrData	data_;
public:
	PrRR_AnyMessageBody():PrCompound("AnyMessageBody"),
		data_("data"){}
static	const PrRR_AnyMessageBody* instance(){
		if(!instance_)instance_=new PrRR_AnyMessageBody();
		return instance_;}
	uint32_t length_for_pr(const ItP& at,const PvOctets& buf)const{
		return buf.remainLength(at.bytes());}
	bool print_body(uint32_t t,ItP& at,const PvOctets& buf)const{
		data_.	print(t,at,buf);
		return true;}
static const PrCompound* get_MessageBody(uint32_t bodytype){
		const PrCompound* rtn=0;
		switch(bodytype){
		case TP_Code_ICMPv6RR_Command:{
			rtn= PrRR_CommandMessageBody::instance(); break;}
		case TP_Code_ICMPv6RR_Result:{
			rtn= PrRR_ResultMessageBody::instance(); break;}
		case TP_Code_ICMPv6RR_Reset:{
			rtn= PrRR_ResetMessageBody::instance(); break;}
		default:{
			rtn= PrRR_AnyMessageBody::instance(); break;} }
		return rtn;}
};
const PrRR_AnyMessageBody* PrRR_AnyMessageBody::instance_=0;


//////////////////////////////////////////////////////////////////////////////
// class RcICMPv6RR
void RcICMPv6RR::print_MessageBody(uint32_t t,
		uint32_t bodytype,const RObject& r_msgbody)const{
	const PrCompound* pr = PrRR_AnyMessageBody::get_MessageBody(bodytype);
	ItP at;
	const PvOctets* buf = (const PvOctets*)r_msgbody.pvalue();
	pr->print(t,at,*buf);
	delete pr;}


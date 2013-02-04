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
 * $TAHI: v6eval/lib/Pz/McICMPv4.cc,v 1.4 2001/10/12 04:56:14 tanaka Exp $
 */
#include "McICMPv4.h"
#include "ItPosition.h"
#include "WObject.h"
#include "RObject.h"
#include "PControl.h"
#include "PvObject.h"
#include "PvOctets.h"

//////////////////////////////////////////////////////////////////////////////
#define SUPER	McUpper

McUpp_ICMPv4_ONE* McUpp_ICMPv4_ONE::instance_ = 0;
McUpp_ICMPv4_ONE* McUpp_ICMPv4_ONE::instance(){
	if(!instance_){instance_= new McUpp_ICMPv4_ONE("ICMPv4");}
	return instance_;}

McUpp_ICMPv4_ONE::McUpp_ICMPv4_ONE(CSTR key):SUPER(key){
        member( new MmHeader_onICMPv4( "header" ) );
        //dict
        MmUpper_onIP::add(this);	//Packet_IP::upper=
	}

McUpp_ICMPv4_ONE::~McUpp_ICMPv4_ONE(){}

// COMPOSE/REVERSE
bool McUpp_ICMPv4_ONE::containsMc(const MObject* mc) const{
	bool rtn = McUpper::containsMc(mc);
	return rtn ? rtn : members_[0]->containsMc(mc);}

uint32_t McUpp_ICMPv4_ONE::length_for_reverse(
		RControl&,ItPosition& at,OCTBUF& buf) const{
	return buf.remainLength(at.bytes());}

RObject* McUpp_ICMPv4_ONE::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	return members_[0]->reverse(c,r_parent,at,buf);}//forward MmHdr_ICMPv4
#undef SUPER

//////////////////////////////////////////////////////////////////////////////
#define SUPER	McHeader

McUpp_ICMPv4::McUpp_ICMPv4(CSTR key):SUPER(key),type_(0){
	McUpp_ICMPv4_ONE::instance();}//wake up 
McUpp_ICMPv4::~McUpp_ICMPv4(){}
int32_t McUpp_ICMPv4::token()const{return metaToken(tkn_upper_);}

// COMPOSE/REVERSE
uint32_t McUpp_ICMPv4::length_for_reverse(
		RControl&,ItPosition& at,OCTBUF& buf) const{
	return buf.remainLength(at.bytes());}

bool McUpp_ICMPv4::overwrite_DictType(
		RControl& c,ItPosition& at,OCTBUF& buf)const{
	ItPosition tmpat=at;
	RObject* rtype = type_->reverse(c,0,tmpat,buf);
	if(!rtype)return false;                 //Type field decode error
	//
	const PvNumber* pv = (const PvNumber*)rtype->pvalue();
	uint32_t typevalue = pv->value();
	c.DictType().type_Set(typevalue);       //self Type set
	delete rtype;
	return true;}

RObject* McUpp_ICMPv4::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	RObject* r_self = SUPER::reverse(c,r_parent,at,buf);
	if(!c.error()){
		Con_IPinfo* info = c.IPinfo();
		if(info)info->reverse_postUppChecksum(c,r_self);}
	return r_self;}

bool McUpp_ICMPv4::generate(WControl& c,WObject* w_self,OCTBUF& buf) const {
	bool rtn = SUPER::generate(c,w_self,buf);
	if(!c.error()){
		Con_IPinfo* info = c.IPinfo();
		if(info)info->generate_postUppChecksum(c,buf,w_self);}
	return rtn;}
#undef SUPER

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv4_ANY::McUpp_ICMPv4_ANY(CSTR key):McUpp_ICMPv4(key){}
McUpp_ICMPv4_ANY::~McUpp_ICMPv4_ANY(){}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv4_DestinationUnreachable::McUpp_ICMPv4_DestinationUnreachable(CSTR key):McUpp_ICMPv4(key){}
McUpp_ICMPv4_DestinationUnreachable::~McUpp_ICMPv4_DestinationUnreachable(){}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv4_TimeExceeded::McUpp_ICMPv4_TimeExceeded(CSTR key):McUpp_ICMPv4(key){}
McUpp_ICMPv4_TimeExceeded::~McUpp_ICMPv4_TimeExceeded(){}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv4_ParameterProblem::McUpp_ICMPv4_ParameterProblem(CSTR key):McUpp_ICMPv4(key){}
McUpp_ICMPv4_ParameterProblem::~McUpp_ICMPv4_ParameterProblem(){}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv4_Redirect::McUpp_ICMPv4_Redirect(CSTR key):McUpp_ICMPv4(key){}
McUpp_ICMPv4_Redirect::~McUpp_ICMPv4_Redirect(){}

//////////////////////////////////////////////////////////////////////////////
// particular internal class (to forward headerType(ICMPv4)->icmpv4Type)
MmHeader_onICMPv4::MmHeader_onICMPv4(CSTR key):MmReference_Must1(key,true){}
MmHeader_onICMPv4::~MmHeader_onICMPv4(){}

void MmHeader_onICMPv4::add(McUpp_ICMPv4* mc){
	dict_.add(mc->icmpv4Type(),mc);}
void MmHeader_onICMPv4::add_other(McUpp_ICMPv4* mc){dict_.add_other(mc);}
TypevsMcDict MmHeader_onICMPv4::dict_;

// REVERSE
bool MmHeader_onICMPv4::overwrite_DictType(
		RControl& c,ItPosition& at,OCTBUF& buf)const{
	McUpp_ICMPv4* any = (McUpp_ICMPv4*)dict_.find_other();
	return any->overwrite_DictType(c,at,buf);}


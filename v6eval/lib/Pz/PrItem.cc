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
 * $TAHI: v6eval/lib/Pz/PrItem.cc,v 1.3 2003/12/04 04:59:47 akisada Exp $
 */
#include "PrItem.h"
//#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////
// printer items
//////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
#define CLASS "PrItem"

bool PrItem::check_remainLength(const ItP& reqlen,
			const ItP& at,const PvOctets& buf)const{
	uint32_t l= buf.length();
	ItP len(l,0);
	len-=at; len-=reqlen;
	if(len.bytes()>=0)return true;//printable enough length
	//
	fprintf(stderr,
		"err:unable print %s size(%d,%d) ,remain at(%d,%d)/size(%d)\n",
		name_,reqlen.bytes(),reqlen.bits(),at.bytes(),at.bits(),l);
	return false;}

void PrItem::print_tag(uint32_t t)const{
	printf("log:");
	for(uint32_t i=0;i<t;i++) {printf("| ");}
	printf("%-30s",name_);}

bool PrItem::print(uint32_t,ItP&,const PvOctets&)const{return true;}

#undef CLASS

//---------------------------------------------------------------------------
#define CLASS "PrUint"

uint32_t PrUint::get_value(const ItP& at,const PvOctets& buf)const{
	uint32_t value = buf.decodeUint(at,width_);//decode hostorder
	return value;}

bool PrUint::print(uint32_t t,ItP& at,const PvOctets& buf)const{
	if(!check_remainLength(ItP(0,width_),at,buf))return false;
	print_tag(t);
	uint32_t value = get_value(at,buf);
	printf(" = %u\n",value);
	step(at); //forward Itp& at
	return true;}

#undef CLASS

//---------------------------------------------------------------------------
#define CLASS "PrData"

bool PrData::print(uint32_t t,ItP& at,const PvOctets& buf)const{
	if(!check_remainLength(ItP(0),at,buf))return false;
	print_tag(t);
	int32_t length = buf.remainLength(at.bytes());
	PvOctets value(length,(OCTSTR)buf.string(at.bytes()));
	printf(" = "); value.log(t); printf("\n");
	at.addBytes(length); //forward Itp& at
	return true;}

#undef CLASS

//////////////////////////////////////////////////////////////////////////////
// Abstruct printer compound & PrNeedless
//////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
#define CLASS "PrCompound"

uint32_t PrCompound::length_for_pr(const ItP&,const PvOctets&)const{
	return 0;}

bool PrCompound::print_head(uint32_t t,uint32_t length)const{
	print_tag(t); printf("(length:%d)",length); printf("\n");
	return true;}
bool PrCompound::print_body(uint32_t,ItP&,const PvOctets&)const{
	return true;}

bool PrCompound::print(uint32_t t,ItP& at,const PvOctets& buf)const{
	uint32_t length = length_for_pr(at,buf);
	if(!check_remainLength(ItP(length),at,buf))return false;
	ItP bodyat;
	PvOctets bodybuf(length,(OCTSTR)buf.string(at.bytes()));
	bool rtn = true;
	if(rtn)rtn = print_head(t,length);
	if(rtn)rtn = print_body(t+1,bodyat,bodybuf);
	if(bodybuf.remainLength(bodyat.bytes())>0){//remain Needless
		print_remain_Needless(t+1,bodyat,bodybuf);}
	at.addBytes(length);
	return rtn;}

bool PrCompound::print_remain_more0(uint32_t t,
		ItP& at,const PvOctets& buf)const{
	bool rtn= true;
	int32_t remain = 0;
	while(rtn && (remain=buf.remainLength(at.bytes()))>0){
		rtn = print(t,at,buf);}
	return rtn;}

bool PrCompound::print_remain_Needless(uint32_t t,ItP& at,const PvOctets& buf){
	const PrNeedless tmp;
	return tmp.print(t,at,buf);}

#undef CLASS


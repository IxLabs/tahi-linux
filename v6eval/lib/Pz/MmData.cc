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
 * $TAHI: v6eval/lib/Pz/MmData.cc,v 1.5 2001/10/12 04:56:16 tanaka Exp $
 */
#include "MmData.h"
#include "PControl.h"
#include "PvOctets.h"
#include "WObject.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////
MmData::MmData(CSTR key,bool evalskip,const ICVoverwriter* ow):MmObject(key),
	evalskip_(evalskip),ICVoverwriter_(ow){}
MmData::~MmData() {}

int32_t MmData::token() const{return metaToken(tkn_data_);}

uint32_t MmData::objectLength(const PObject* o,const WObject* w) const {
	return o!=0?o->objectLength(w):0;}

bool MmData::encodeOctets(WControl&,const ItPosition& at,
				OCTBUF& dst,const PvOctets& elm)const{
	dst.encode(at,elm);
	return true;}
bool MmData::encodeNumber(WControl&,const ItPosition& at,
				OCTBUF& dst,const PvNumber& n)const{
	dst.encodeUint(n.value(),at,8);
	return true;}

// COMPOSE(more than 0)
void MmData::composeList(WControl& c,
		WObject* w_parent,const PObjectList& pls)const{
	const PObject* pl =
		pls.reverseMatching(this,(PObjectEqFunc)&PObject::isEqualMeta);
	if(pl==0) {
		compose(c,w_parent,0);} 
	else {	pls.elementsPerformWith(
			(PObjectFunc)&PObject::vmatchselfCompose,
			&c,w_parent,this);
		}
	}

WObject* MmData::compose(WControl& c,
		WObject* w_parent,const PObject* pl)const{
	WObject* w_self = (WObject*)w_parent->corresponding(this);
	if(!w_self){
		w_self = composeWm(c,w_parent,pl);}
	if(pl){	const PObject* right = pl->rvalue();
		if(!right){w_parent->mustDefineMem(this); return 0;}
		const TypevsMcDict* keep = c.dict(); c.dict_set(0);
		WObject* wm = right->selfCompose(c,w_self); wm=wm;
		c.dict_set(keep);}
	return w_self;}

WObject* MmData::composeWm(WControl&,
		WObject* w_parent,const PObject* pl)const{
	return new WmData(w_parent,this,pl);}

// REVERSE(must 1)
uint32_t MmData::length_for_reverse(
		RControl&,ItPosition& at,OCTBUF& buf) const {
	return buf.remainLength(at.bytes());}

RObject* MmData::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	const MObject* m_parent = r_parent ? r_parent->meta() : 0;
	uint32_t length = length_for_reverse(c,at,buf);
	if(evalskip_&&!length){
		return 0;}//useless reverse
	ItPosition size(length,0);
	if(!check_decode_limit(m_parent,at,buf,size)){
		c.set_error(1); return 0;}
	PvObject* pv = buf.substr(at.bytes(),length);
	RObject* r_self = reverseRm(c,r_parent,at,size,pv);
	at+=size;
	return r_self;}

RObject* MmData::reverseRm(RControl&,RObject* r_parent,
		const ItPosition& at,const ItPosition& size,PvObject* pv)const{
	return new RmObject(r_parent,this,at,size,pv);}

bool MmData::generate(WControl& c,WObject* w_self,OCTBUF& buf) const{
	return generate_child(c,w_self,buf);}

#include "McAH.h"
bool MmData::overwrite_ICV(ICVControl& c,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const{
	return ICVoverwriter_? ICVoverwriter_->overwrite(c,at,buf,t) : true;}

///////////////////////////////////////////////////////////////////////////////
WmData::WmData(WObject* p,const MObject* m,const PObject* o):WtNode(p,m,o),
	data_(0) {}
WmData::~WmData() {
	if(data_){delete data_; data_=0;}
	}

void WmData::internalEvaluatePrologue(ItPosition&) {
	ItPosition mine;
	generatePrologue(mine);
	PvOctets* po=new PvOctets(mine.bytes());
	if(data_)delete data_;
	data_=po;
	WControl tc;
	generate(tc,*po);}

bool WmData::doEvaluate(WControl& c,RObject& r) {
	const PvObject* ro = r.pvalue();
	return valueEvaluate(c,ro,data_);}


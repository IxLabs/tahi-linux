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
 * $TAHI: v6eval/lib/Pz/McObject.cc,v 1.46 2002/12/02 12:31:30 akisada Exp $
 */
#include "McObject.h"
#include "MmObject.h"
#include "MmHeader.h"
#include "MmData.h"
#include "WObject.h"
#include "RObject.h"
#include "PControl.h"
#include "PcObject.h"
#include "PvOctets.h"
#include "ItPosition.h"
#include "DmObject.h"
#include <stdio.h>

MObject::MObject(CSTR s):CmToken(s),describer_(0) {}
MObject::~MObject() {delete describer_; describer_=0;}
uint32_t MObject::objectLength(const PObject*,const WObject*) const {return 0;}
uint16_t MObject::width() const {return 0;}
void MObject::postProcess(void*) {}
void MObject::stepPostion(ItPosition&) const{}
void MObject::compound(MObject*) {}
MvKeyword::MvKeyword(CSTR s,int32_t t):MObject(s),token_(t) {}
MvKeyword::~MvKeyword() {}
bool MObject::icvUpdate(const PaObject&,PvOctets*) const {return false;}

PObject* MObject::tokenObject(int,CSTR) const {return 0;}

//must return error *Control 
bool MObject::generate(WControl& c,WObject* w_self,OCTBUF& buf) const {
	if(w_self){
		const PObject* p = w_self->rgenerate();
		return p ? p->generate(c,w_self,buf) : geneAuto(c,w_self,buf);}
	return c;}
bool MObject::generate_child(WControl& c,WObject* w_self,OCTBUF& buf) const {
	if(w_self){
		WObject* n=0;
		for(n=w_self->nextChild();n!=0;n=w_self->nextChild(n)) {
			WObject* w=(WObject*)n;
			w->generate(c,buf);}
		}
	return c;}

bool MObject::geneAuto(WControl& c,WObject*,OCTBUF&) const {return c;}
bool MObject::geneFix(WControl& c,WObject*,OCTBUF&) const {return c;}
bool MObject::encodeNumber(WControl&,const ItPosition&,OCTBUF&,const PvNumber&) const {
	return false;}
bool MObject::encodeOctets(WControl&,const ItPosition&,OCTBUF&,const PvOctets&) const {
	return false;}
PvObject* MObject::geneAutoValue(WObject*) const {return 0;}
PvObject* MObject::evalAutoValue(WObject*) const {return 0;}

// COMPOSE/REVERSE
WObject* MObject::compose(WControl&,WObject*,const PObject*) const {return 0;}
RObject* MObject::reverse(RControl&,RObject*,ItPosition&,OCTBUF&) const {return 0;}
RObject* MObject::rreverse(RControl& c,RObject* rp,ItPosition& at,OCTBUF& s) const {
	ItPosition ml(objectLength(),width());
	if(at<ml) {c.set_error(1); return 0;}
	at-=ml;
	return reverse(c,rp,at,s);}
void MObject::composeList(WControl&,WObject*,const PObjectList&) const {}

bool MObject::check_decode_limit(const MObject* m_parent,
		const ItPosition& at,OCTBUF& buf,const ItPosition& size)const{
	uint32_t l=buf.length();
	ItPosition len(l,0);
	len-=at; len-=size;
	if(len.bytes()>=0) {return true;}//decodeable enough length
	fprintf(stderr,
		"err:unable decode %s size(%d,%d) ,in %s at(%d,%d)/size(%d)\n",
		string(),size.bytes(),size.bits(),
		(m_parent?m_parent->string():""),at.bytes(),at.bits(),l);
	return false;}

bool MObject::check_decode_alignment_requirement(const MObject* m_parent,
		const ItPosition& offset,const ItPosition& size)const{
	uint32_t X = alignment_requirement();
	return check_decode_alignment_requirement(m_parent,offset,size,X);}

bool MObject::check_decode_alignment_requirement(const MObject* m_parent,
	const ItPosition& offset,const ItPosition& size,uint32_t ali)const{
	uint32_t X = ali;
	uint32_t H = offset.bytes();	//offset from ParentTop
	uint32_t S = size.bytes();	//this size
	uint32_t pad = padcount_alignment_requirement(X,H,S);
	if(pad==0) {return true;}	//ok alignment requirement no need pad
	fprintf(stderr,
		"err:%s alignment requirement(%d) need %d pad in %s\n",
		string(),X,pad,(m_parent?m_parent->string():""));
	return false;}

//OVERWRITE ICV
bool MObject::overwrite_ICV(ICVControl&,
		const ItPosition&,OCTBUF&,const TObject*)const{
	return true;}//useless overwrite
		
///////////////////////////////////////////////////////////////////////////////
McObject::McObject(CSTR s):MObject(s),size_(),members_(16) {}
McObject::~McObject() {}

PObject* McObject::tokenObject(int l,CSTR f) const {
	return new PcObject(this,f,l);}

void McObject::member(MObject* m) {
	members_.add(m);
	m->compound(this);}

MObject* McObject::findMember(CSTR s) const {
	MObject* rtn=0;
	for(int i=0,imax=members_.size();i<imax&&!rtn;i++){
		MObject* mem = members_[i];
		rtn=mem->findMember(s);}
	return rtn;}

McObject* McObject::find(CSTR s) {
	McObject tmp(s);
	return (McObject*)set_.find(&tmp);}

MObject* McObject::findClassMember(CSTR c,CSTR m) {
	const McObject* cls=find(c);
	return cls!=0?cls->findMember(m):0;}

McObject* McObject::confirmed() {
	ItPosition i;
	members_.elementsPerformWith(
		(MObjectFunc)&MObject::postProcess,(void*)&i);
	size_=i;
	return this;}

void McObject::print() const {
	members_.elementsPerform((MObjectFunc)&MObject::print);}

// COMPOSE
WObject* McObject::compose(WControl& c,
		WObject* w_parent,const PObject* pc) const {
	WObject* w_self = composeWc(c,w_parent,pc);
	const PObjectList& pls = pc->members();
	members_compose(c,w_self,pls);
	return w_self;}

WObject* McObject::composeWc(WControl&,
		WObject* w_parent,const PObject* pc) const {
	return new WcObject(w_parent,this,pc);}

void McObject::members_compose(WControl& c,
		WObject* w_self,const PObjectList& pls)const{
	for(int i=0,imax=members_.size();i<imax;i++){
		MObject* mem = members_[i];
		mem->composeList(c,w_self,pls);
		if(!c)break;}
	}

// REVERSE
RObject* McObject::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	//
	const MObject* m_parent = r_parent ? r_parent->meta() : 0;
	uint32_t length = length_for_reverse(c,at,buf);
	ItPosition size(length,0);
	if(!check_decode_limit(m_parent,at,buf,size)){
		c.set_error(1); return 0;}
	if(!check_decode_alignment_requirement(m_parent,at,size)){
		c.set_warning(3); /*return 0;*/}
	//
	OCTBUF* buf_self = buf.substr(at.bytes(),length);
	RObject* r_self = reverseRc(c,r_parent,at,buf_self);
	ItPosition memat; //new offset from buf_self
	members_reverse(c,r_self,memat,*buf_self);
	at.addBytes(length);
	return r_self;}

uint32_t McObject::length_for_reverse(RControl&,ItPosition&,OCTBUF&) const {
	return size_.bytes();}
uint32_t McObject::Layerlength_for_reverse(RControl&,ItPosition&,OCTBUF&) const{
	return 0;}

RObject* McObject::reverseRc(RControl&,
		RObject* r_parent,const ItPosition& at,PvObject* pv)const{
	return new RcObject(r_parent,this,at,pv);}

void McObject::members_reverse(RControl& c,
		RObject* r_self,ItPosition& memat,OCTBUF& buf_self)const{
	for(int i=0,imax=members_.size();i<imax;i++){
		MObject* mem = members_[i];
		mem->reverse(c,r_self,memat,buf_self);
		if(!c)break;}
	int32_t remainlen = buf_self.remainLength(memat.bytes());
	if(remainlen>0){//reverse length Needless data
		McNeedless* mless = McNeedless::instance();
		mless->reverse(c,r_self,memat,buf_self);
		}
	}

void MObject::describe(const PObject* o) const {
	DmObject* dm=describer();
	if(dm!=0) {dm->describe(this,o);}}

// GENERATE
bool McObject::generate(WControl& c,WObject* w_self,OCTBUF& buf) const {
	return generate_child(c,w_self,buf);}

//////////////////////////////////////////////////////////////////////////////
// Needless data (no token ,can not describe on PacketDefineFile)
McNeedless* McNeedless::instance_ = 0;
McNeedless* McNeedless::instance(){
	if(!instance_){instance_= new McNeedless("[Needless]");}
	return instance_;}

McNeedless::McNeedless(CSTR s):McObject(s) {
	member( new MmData( "data" ) );
	}
McNeedless::~McNeedless() {}

// COMPOSE/REVERSE
uint32_t McNeedless::length_for_reverse(
		RControl&,ItPosition& at,OCTBUF& buf) const{
	return buf.remainLength(at.bytes());}

RObject* McNeedless::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	uint32_t remainlen = length_for_reverse(c,at,buf);
	RObject* rc = remainlen ? McObject::reverse(c,r_parent,at,buf) : 0;
	return rc;}//if remainlen==0 do not reverse


//////////////////////////////////////////////////////////////////////////////
// DictType VS McObject dict

TypevsMc::TypevsMc():type_(-1),Mc_(0){}
TypevsMc::TypevsMc(int32_t type,MObject* mc):type_(type),Mc_(mc){}

TypevsMcDict::TypevsMcDict():list_(),other_(0){}
TypevsMcDict::~TypevsMcDict(){
	for(int i=0,imax=list_.size();i<imax;i++){
		delete list_[i];}
	}
MObject* TypevsMcDict::find(const Con_DictType& DictType)const{
	/**/ if(DictType.uneffect())	return 0;
	else if(DictType.finish())	return McNeedless::instance();
	else if(DictType.other())	return other_;
	int32_t type = DictType.type();
	for(int i=0,imax=list_.size();i<imax;i++){
		TypevsMc* rec = list_[i];
		if(type==rec->type())	return rec->Mc();}
	return other_;}
MObject* TypevsMcDict::find_other()const{return other_;}
bool TypevsMcDict::contains(const MObject* mc)const{
	if(other_ && other_->containsMc(mc))return true;
	for(int i=0,imax=list_.size();i<imax;i++){
		TypevsMc* rec = list_[i];
		if(rec->Mc()->containsMc(mc))return true;}
	return false;}

bool TypevsMcDict::check(const Con_DictType &DictType) const {
	if(DictType.effect()) {
		int32_t type = DictType.type();

		for(int i = 0, imax = list_.size(); i< imax; i++) {
			TypevsMc *rec = list_[i];

			if(type == rec->type()) {
				return(true);
			}
		}
	}

	return(false);
}

void TypevsMcDict::add(int32_t type,MObject* mc){
	list_.append(new TypevsMc(type,mc));}
void TypevsMcDict::add_other(MObject* mc){
	other_=mc;}

implementCmList(TypevsMcList,TypevsMc);
implementCmSet(MObjectSet,MObject);
implementCmList(MObjectList,MObject);
int32_t MObject::tokenArray_[MObject::maxMetaToken_];
MObjectSet McObject::set_(64);

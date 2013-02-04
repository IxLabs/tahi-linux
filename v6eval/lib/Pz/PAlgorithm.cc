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
 * $TAHI: v6eval/lib/Pz/PAlgorithm.cc,v 1.13 2003/12/04 04:59:47 akisada Exp $
 */
#include "PAlgorithm.h"
#include "CmMain.h"
#include "WObject.h"
#include "PControl.h"
#include "PvOctets.h"
#include "MfAlgorithm.h"
#include <stdio.h>

//----------------------------------------------------------------------
uint32_t PaAH::ICV_Length() const {
	return auth_!=0?auth_->ICV_Length():0;}

OCTBUF* PaAH::ICV_Calculate(const OCTBUF& s) const {
	if(!auth_) return 0;
	auth_->init();
	auth_->update(s);
	PvOctets* icv=auth_->result();
	return icv;}

//======================================================================
// ESP
// ciphertxt:
//  | spi   | seq   | (IV)  | payload       |(pad)|PL|NH| (ICV)    |
//  |       |       |       |               |     |  |  |          |
//  +-------+-------+--...--+---........----+-...-+--+--+---....---+
//
//  PL=paddingLength/ NH=nextHeadr
//
//                           <--Decrypted-------------->
//
//                   <---Crypted----------------------->
//----------------------------------------------------------------------
// in:	sizeof(IV)+sizeof(payload)+sizeof(pad)+sizeof(PL)+sizeof(NH)
// out:	           sizeof(payload)+sizeof(pad)+sizeof(PL)+sizeof(NH)
//----------------------------------------------------------------------
uint32_t PaESP::ivecLength() const {
	return crypt_!=0?crypt_->ivecLength():0;}
	
uint32_t PaESP::Decrypted_Length(const OCTBUF& buf) const {
	if(crypt_==0) return 0;
	uint32_t l=buf.length();
	uint32_t ivlen=ivecLength();
	return l-ivlen;}

OCTBUF* PaESP::Decrypted_Octets(const OCTBUF& buf,OCTBUF*& ivec) const {
	if(ivec!=0) {ivec->set(0);}
	return crypt_!=0?crypt_->decrypt(buf,ivec):new PvOctets(buf);}
	
//----------------------------------------------------------------------
// in:	           sizeof(payload)+sizeof(pad)+sizeof(PL)+sizeof(NH)
// out:	sizeof(IV)+sizeof(payload)+sizeof(pad)+sizeof(PL)+sizeof(NH)
//----------------------------------------------------------------------
uint32_t PaESP::Crypted_Length(uint32_t len) const {
	uint32_t ivlen=ivecLength();
	return len+ivlen;}

OCTBUF* PaESP::Crypted_Octets(const OCTBUF& buf) const {
	return crypt_!=0?crypt_->encrypt(buf):new PvOctets(buf);}

//----------------------------------------------------------------------
// in:	sizeof(payload)+sizeof(PL)+sizeof(NH)
// out:	sizeof(pad)
//----------------------------------------------------------------------
uint32_t PaESP::Padding_Length(uint32_t decrypinglen) const {
	return crypt_!=0?crypt_->Padding_Length(decrypinglen,auth()):0;}

OCTBUF* PaESP::Padding_Octets(uint32_t len) const {
	return pad_!=0?pad_->Padding_Octets(len):MfESPPad::defaultOctets(len);}

//----------------------------------------------------------------------
PaAH::PaAH(const MObject* m,CSTR st,int l):PaObject(m,st,l),auth_(0) {}
PaAH::~PaAH() {
	if(auth_!=0) {delete auth_; auth_=0;}}
PObject* PaAH::auth_member(PObject* p) {
	return auth_=(PfAuth*)(p);}

void PaAH::icvInit() const {
	if(auth_!=0) {auth_->init();}}
void PaAH::icvUpdate(OCTBUF* buf) const {
	if(auth_!=0&&buf!=0) {auth_->update(*buf);}}
OCTBUF* PaAH::icvResult() const {
	PvOctets* icv=auth_!=0?auth_->result():0;
	return icv;}
//----------------------------------------------------------------------
PaESP::PaESP(const MObject* m,CSTR st,int l):PaAH(m,st,l),
	pad_(0),crypt_(0) {}
PaESP::~PaESP() {
	if(pad_!=0) {delete pad_; pad_=0;}
	if(crypt_!=0) {delete crypt_; crypt_=0;}}
//----------------------------------------------------------------------
PObject* PaESP::pad_member(PObject* p) {
	return pad_=(PfESPPad*)(p);}
PObject* PaESP::crypt_member(PObject* p) {
	return crypt_=(PfCrypt*)(p);}

//----------------------------------------------------------------------
PaObject::PaObject(const MObject* m,CSTR st,int l):PcObject(m,st,l) {}
PaObject::~PaObject() {}
uint32_t PaObject::ICV_Length() const {return 0;}
OCTBUF* PaObject::ICV_Calculate(const OCTBUF&) const {return 0;}
uint32_t PaObject::Decrypted_Length(const OCTBUF&) const {return 0;}
OCTBUF* PaObject::Decrypted_Octets(const OCTBUF&,OCTBUF*& ivec) const {
	ivec=0; return 0;}
uint32_t PaObject::Crypted_Length(uint32_t) const {return 0;}
OCTBUF* PaObject::Crypted_Octets(const OCTBUF&) const {return 0;}
uint32_t PaObject::Padding_Length(uint32_t) const {return 0;}
OCTBUF* PaObject::Padding_Octets(uint32_t) const {return 0;}
WObject* PaObject::selfCompose(WControl& c,WObject* w_parent) const {
	w_parent->algorithm(this);
	c.set_push_SA(this);
	return 0;}
void PaObject::icvInit() const {}
void PaObject::icvUpdate(OCTBUF*) const {}
OCTBUF* PaObject::icvResult() const {return 0;}
////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------
OCTBUF* PfESPPad::Padding_Octets(uint32_t len) const {
	const MfESPPad* m=metaClass();
	return m->Padding_Octets(len);}

//----------------------------------------------------------------------
uint32_t PfAuth::alignment() const {
	const MfAuth* m=metaClass();
	return m->alignment(args());}

void PfAuth::init() {
	const MfAuth* m=metaClass();
	context_=m->init(context_,args());}

void PfAuth::update(const OCTBUF& s) {
	const MfAuth* m=metaClass();
	m->update(context_,args(),s);}

PvOctets* PfAuth::result() {
	const MfAuth* m=metaClass();
	return m->result(context_,args());}

uint32_t PfAuth::ICV_Length() const {
	const MfAuth* m=metaClass();
	return m->icvLength();}

inline uint32_t roundN(uint32_t num,uint32_t align) {
	return ((num+align-1)/align)*align;}
//----------------------------------------------------------------------
uint32_t PfCrypt::ivecLength() const {
	const MfCrypt* m=metaClass();
	return m->ivecLength();}

uint32_t PfCrypt::Padding_Length(uint32_t len,const PfAuth* ah) const {
	const MfCrypt* m=metaClass();
	uint32_t nah=ah!=0?ah->alignment():1;
	uint32_t nalign=m->alignment(args());
	if(nah>1) {nalign=roundN(nalign,nah);}
	uint32_t total=nalign>1?roundN(len,nalign):len;
	return total-len;}

OCTBUF* PfCrypt::decrypt(const OCTBUF& buf,OCTBUF*& ivec) const {
	const MfCrypt* m=metaClass();
	return m->decryptOctets(buf,args(),ivec);}

OCTBUF* PfCrypt::encrypt(const OCTBUF& buf) const {
	const MfCrypt* m=metaClass();
	PvOctets ivec;
	return m->encryptOctets(buf,args(),&ivec);}

//----------------------------------------------------------------------
void PfESPPad::checkArgument() const {}
void PfAuth::checkArgument() const {
	const MfAuth* m=metaClass();
	m->checkArgument(*this,args());}

void PfCrypt::checkArgument() const {
	const MfCrypt* m=metaClass();
	m->checkArgument(*this,args());}

//----------------------------------------------------------------------
PfESPPad::PfESPPad(const MfESPPad* m,CSTR f,int l):PFunction(f,l),meta_(m) {}
PfESPPad::~PfESPPad() {}
const MObject* PfESPPad::meta() const {return metaClass();}
PfCrypt::PfCrypt(const MfCrypt* m,CSTR f,int l):PFunction(f,l),meta_(m) {}
PfCrypt::~PfCrypt() {}
const MObject* PfCrypt::meta() const {return metaClass();}
PfAuth::PfAuth(const MfAuth* m,CSTR f,int l):PFunction(f,l),meta_(m),context_(0) {}
PfAuth::~PfAuth() {}
const MObject* PfAuth::meta() const {return metaClass();}

//======================================================================
SAOctets::SAOctets(PvOctets* f,AlgorithmList* l):frame_(f),algorithms_(l) {}
SAOctets::~SAOctets() {
	if(frame_!=0) {delete frame_; frame_=0;}
	if(algorithms_!=0) {delete algorithms_; algorithms_=0;}};
bool SAOctets::isEqual(const SAOctets*) const {return false;}

//======================================================================
WAlgorithm::WAlgorithm(WObject* w,AlgorithmList* l):CmQueue(),
	composed_(w),algorithms_(l) {}
WAlgorithm::~WAlgorithm() {}
AlgorithmFrames::AlgorithmFrames():algorithms_(),noneAlgorithms_() {}
AlgorithmFrames::~AlgorithmFrames() {}
void AlgorithmFrames::alogrithm(WAlgorithm* p) {algorithms_.insert(p);}
void AlgorithmFrames::none(WObject* w) {
	noneAlgorithms_.add(w);}
const WAlgorithm* AlgorithmFrames::nextAlgorithm(const CmQueue* o) const {
	const CmQueue* top=algorithms();
	const CmQueue* p=o!=0?o:top;
	CmQueue* n=p!=0?p->next():0;
	return n!=top?(WAlgorithm*)n:0;}

const WObject* WAlgorithm::matchedComposed(uint32_t cnt,RObject* ro) const {
	AlgorithmList* al=algorithms();
	const WObject* w=composed();
	uint32_t sas=al!=0?al->size():0;
	WControl wc(logLevel>0,DBGFLAGS('e'));
	if(sas!=cnt) {
		if(w!=0) {
			w->notMatch(wc,"algorithm count",*ro);}
		else {
			printf("%sng algorithm count not match %d != %d\n",
			wc.logging()?"log:":"",cnt,sas);}
		return 0;}
	if(al!=0&&wc.msg()) {
		AlgorithmList& ral=*al;
		printf("%sapplied algorithms={",wc.logging()?"log:":"");
		for(uint32_t i=0;i<sas;i++) {
			PaObject* pa=ral[i];
			printf("%s%s",i!=0?",":"",pa->nameString());}
		printf("}\n");}
	return w;}

//----------------------------------------------------------------------
void PaAH::print() const {
	if(auth_!=0) {printf("auth="); auth_->print(); printf("\n");}}
void PaESP::print() const {
	if(pad_!=0) {printf("pad="); pad_->print(); printf("\n");}
	if(crypt_!=0) {printf("crypt="); crypt_->print(); printf("\n");}
	PaAH::print();}

//----------------------------------------------------------------------
void PaAH::log(uint32_t t) const {
	printf("log: AH=%s\n",nameString());
	logSelf();}
void PaAH::logSelf() const {
	if(auth_!=0) {printf("log: auth="); auth_->print(); printf("\n");}}
void PaESP::log(uint32_t t) const {
	printf("log: ESP=%s\n",nameString());
	logSelf();}
void PaESP::logSelf() const {
	if(pad_!=0) {printf("log: pad="); pad_->print(); printf("\n");}
	if(crypt_!=0) {printf("log: crypt="); crypt_->print(); printf("\n");}
	PaAH::logSelf();}
void WAlgorithm::log(uint32_t t) const {
	AlgorithmList* al=algorithms();
	uint32_t sas=al!=0?al->size():0;
	if(sas==0) return;
	const WObject* w=composed();
	printf("log:--------------------------------------------\n");
	printf("log: frame=%s\n",w!=0?w->nameString():"NO FRAME");
	AlgorithmList& ral=*al;
	for(uint32_t i=0;i<sas;i++) {
		PaObject* pa=ral[i];
		if(pa!=0) {pa->log(t);}}
	printf("log:--------------------------------------------\n");}
void AlgorithmFrames::log(uint32_t t) const {
	const WAlgorithm* wa=nextAlgorithm(0);
	if(wa==0) return;
	printf("log:===ALGORITHM LISTS BEGIN====================\n");
	for(;wa!=0;wa=nextAlgorithm(wa)) {wa->log(t);}
	printf("log:===ALGORITHM LISTS END======================\n");}

implementCmList(AlgorithmList,PaObject);
implementCmList(SAOctetsList,SAOctets);

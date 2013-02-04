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
 * $TAHI: v6eval/lib/Pz/McESP.cc,v 1.21 2005/05/09 09:35:23 akisada Exp $
 */
#include "McESP.h"
#include "ItPosition.h"
#include "WObject.h"
#include "RObject.h"
#include "PControl.h"
#include "PvObject.h"
#include "PvOctets.h"
#include "PvAutoItem.h"
#include "PvAction.h"
#include "McAlgorithm.h"
#include "PAlgorithm.h"
#include "CmMain.h"
#include <stdio.h>

WcESP::WcESP(WObject* w,const MObject* m,const PObject* p):WcObject(w,m,p),
	algorithm_(0) {}
WcESP::~WcESP() {}

void WcESP::algorithm(const PaObject* sa) {algorithm_=sa;}
void WcESP::printChild(uint32_t t) const {
	if(DBGFLAGS('S')){
		if(algorithm_){printf("== ESP SA ==\n"); algorithm_->print();}}
	WcObject::printChild(t);}

RcESP::RcESP(RObject* r,const MObject* m,const ItPosition& o,PvObject* p):
	RcObject(r,m,o,p),algorithm_(0) {}
RcESP::~RcESP() {}

void RcESP::algorithm(const PaObject* sa) {algorithm_=sa;}
void RcESP::printChild(uint32_t t) const {
	if(DBGFLAGS('S')){
		if(algorithm_){printf("== ESP SA ==\n"); algorithm_->print();}}
	RcObject::printChild(t);}

//////////////////////////////////////////////////////////////////////////////
#define SUPER	McHdr_Ext

McHdr_Ext_ESP::McHdr_Ext_ESP(CSTR key):SUPER(key){}
McHdr_Ext_ESP::~McHdr_Ext_ESP(){}

uint32_t McHdr_Ext_ESP::alignment_requirement() const{
	return 0;}//exclude ESP

// COMPOSE/REVERSE
uint32_t McHdr_Ext_ESP::length_for_reverse(
		RControl&,ItPosition& at,OCTBUF& buf)const{
	return buf.remainLength(at.bytes());}

RObject* McHdr_Ext_ESP::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	const PaObject* keepSA = c.poping_SA(); c.poping_SA(0);
	Con_ESPinfo keep = c.ESPinfo(); c.ESPinfo().reset();
	RObject* r_self = SUPER::reverse(c,r_parent,at,buf);
	c.ESPinfo() = keep;
	c.poping_SA(keepSA);
	c.DictType().finish_Set(); // nextType must overewrite Needless
	return r_self;}
RObject* McHdr_Ext_ESP::reverseRc(RControl&,
		RObject* r_parent,const ItPosition& at,PvObject* pv) const {
	return new RcESP(r_parent,this,at,pv);}
WObject* McHdr_Ext_ESP::composeWc(WControl&,
		WObject* w_parent,const PObject* pc) const {
	return new WcESP(w_parent,this,pc);}

WObject* McHdr_Ext_ESP::compose(WControl& c,
		WObject* w_parent,const PObject* pc) const {
	const PaObject* keepSA = c.pushing_SA(); c.pushing_SA(0);
	WObject* w_self = SUPER::compose(c,w_parent,pc);
	c.pushing_SA(keepSA);
	return w_self;}

bool McHdr_Ext_ESP::overwrite_ICV(ICVControl&,
                const ItPosition&,OCTBUF&,const TObject*)const{
	return true;}//ICV CONST undef ESP

#define	UN(n) PvNumber::unique(n)
#define	MUST()PvMUSTDEF::must()

McHdr_Ext_ESP* McHdr_Ext_ESP::create(CSTR key){
	McHdr_Ext_ESP* mc = new McHdr_Ext_ESP(key);
	mc->member( new MmSPI( "SPI",		32,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "SequenceNumber",32,	UN(0),	UN(0) ) );
	mc->member( new MmAlgorithm_onESP("algorithm")); //reference
/*nolex*/mc->member( new MmESP_Crypted("Crypted"));
/*nolex*/mc->member( new MmESP_ICV("ICV"));
	// dict
	MmExtent_onIP::add(mc);			//Packet_IP::exthdr=
	return mc;}
#undef SUPER

//////////////////////////////////////////////////////////////////////////////
#define SUPER	MmReference_Must1
#include "PvName.h"
MmAlgorithm_onESP::MmAlgorithm_onESP(CSTR key):SUPER(key) {}
MmAlgorithm_onESP::~MmAlgorithm_onESP() {}
RObject* MmAlgorithm_onESP::reverse(
		RControl& c,RObject* r_parent,ItPosition& at,OCTBUF&)const{
	RObject* r_self=0;
	c.set_pop_SA(eESP_);//select&set SA
	const PaObject* sa = c.poping_SA();
	if(sa){ RcESP* rcesp = (RcESP*)r_parent;
		if(rcesp)rcesp->algorithm(sa);
		PvObject* pv = sa->nameObject()->shallowCopy();
		ItPosition size(0);
		r_self = new RmObject(r_parent,this,at,size,pv); }
	return r_self;}// not reverse

bool MmAlgorithm_onESP::disused() const{return true;}
#undef SUPER


//////////////////////////////////////////////////////////////////////////////
#define SUPER	MmVarOctets

MmESP_Crypted::MmESP_Crypted(CSTR str):SUPER(str){
	mDecrypted_	= McESP_Decrypted::create("Decrypted");
	mIVEC_		= new MmESP_IVEC("IVEC"); //for log
	}
MmESP_Crypted::~MmESP_Crypted(){
	delete mDecrypted_;	mDecrypted_=0;
	delete mIVEC_;		mIVEC_=0;}

MObject* MmESP_Crypted::findMember(CSTR str) const{
	MObject* rtn = SUPER::findMember(str);
	if(!rtn)rtn = mDecrypted_->findMember(str); //forward lex find
	return rtn;}

ItPosition MmESP_Crypted::length_for_reverse(
		RControl& c,ItPosition& at,OCTBUF& buf)const{
	const PaObject* sa = c.poping_SA();
	uint32_t length = buf.remainLength(at.bytes());
	if(sa){	uint32_t icvlen = sa->ICV_Length();
		length -= icvlen;}
	return ItPosition(length);}

RObject* MmESP_Crypted::reverseRm(RControl& c,RObject* r_parent,
		const ItPosition& at,const ItPosition& size,PvObject* pv)const{
	RmESP_Crypted* r_self = new RmESP_Crypted(r_parent,this,at,size,pv);
	const PaObject* sa = c.poping_SA();
	if(!sa)return r_self;
	OCTBUF& cryptedbuf = (OCTBUF&)*pv;
	if(cryptedbuf.length()==0){
		fprintf(stderr,"err: ESP Crypted buffer length is zero\n");
		return r_self;}
	// try decrypt
	OCTBUF *decryptedbuf=0,*ivecbuf=0;
/*SA*/	decryptedbuf = sa->Decrypted_Octets(cryptedbuf,ivecbuf);
	if(!decryptedbuf){
		fprintf(stderr,"err: SA(%s) doesn't return Decrypted buffer\n",
			sa->nameString());
		c.set_error(1); return r_self;}
	// Decypted self life management
	r_self->set_rDecrypted( mDecrypted_->Self_reverse(c,decryptedbuf) );
	if(ivecbuf){ //ivec self life management
		r_self->set_rIVEC( mIVEC_->Self_reverse(c,ivecbuf) );}
	return r_self;}

void MmESP_Crypted::composeList(WControl& c,
		WObject* w_parent,const PObjectList& pls)const{
	const PaObject* sa = c.pushing_SA();
	WmESP_Crypted* w_self = new WmESP_Crypted(w_parent,this,sa);
	w_self->set_wDecrypted(
		mDecrypted_->Self_composeList(c,pls) ); //forward pls
	//IVEC no use
	}

#undef SUPER

//-----------------------------------------------------------------------------
#define SUPER	RmObject
RmESP_Crypted::RmESP_Crypted(RObject* r_parent,const MObject* m,
		const ItPosition& offset,const ItPosition& size,PvObject* pv):
		SUPER(r_parent,m,offset,size,pv),rDecrypted_(0),rIVEC_(0){}
RmESP_Crypted::~RmESP_Crypted(){
	if(rDecrypted_){delete rDecrypted_; rDecrypted_=0;}
	if(rIVEC_){delete rIVEC_; rIVEC_=0;}
	}
void RmESP_Crypted::print(uint32_t t) const {
	if(DBGFLAGS('c')||rDecrypted_==0)	{SUPER::print(t);}
	else					{printChild(t);} }
void RmESP_Crypted::log(uint32_t t) const {
	if(DBGFLAGS('c')||rDecrypted_==0)	{SUPER::log(t);}
	else					{logChild(t);} }
void RmESP_Crypted::printChild(uint32_t t) const {
	SUPER::printChild(t);
	if(rIVEC_)	rIVEC_->print(t);
	if(rDecrypted_) rDecrypted_->print(t);}
void RmESP_Crypted::logChild(uint32_t t) const {
	SUPER::logChild(t);
	if(rIVEC_)	rIVEC_->log(t);
	if(rDecrypted_) rDecrypted_->log(t);}
void RmESP_Crypted::describeChild(uint32_t t) const {
	SUPER::describeChild(t);
	if(rIVEC_)	rIVEC_->describeChild(t);
	if(rDecrypted_)	rDecrypted_->describeChild(t);}
const TObject* RmESP_Crypted::corresponding(const MObject* m) const {
	const TObject* t=SUPER::corresponding(m);
	if(t==0&&rDecrypted_!=0) {t=rDecrypted_->corresponding(m);}
	return t;}
#undef SUPER

//-----------------------------------------------------------------------------
#define SUPER	WmObject
WmESP_Crypted::WmESP_Crypted(
	WObject* p,const MObject* m,const PaObject* sa):
		SUPER(p,m,0),sa_(sa),wDecrypted_(0){}
WmESP_Crypted::~WmESP_Crypted(){
	if(wDecrypted_){delete wDecrypted_; wDecrypted_=0;}
	}

void WmESP_Crypted::do_adopt_afterESP(){
	WObject* wesp		= parent();			//Hdr_Ext_ESP
	WObject* newparent	= wDecrypted_->nextChild();	//ESP_Payload
	wesp->adoptSisters(*newparent);
	}
void WmESP_Crypted::internalGeneratePrologue(ItPosition& it){
	do_adopt_afterESP();
	if(!sa_)return;
	ItPosition decit;
	wDecrypted_->generatePrologue(decit);
	uint32_t decryptedlen = decit.bytes();
/*SA*/	uint32_t cryptedlen = sa_->Crypted_Length(decryptedlen);
	it.addBytes(cryptedlen);
	//Now Can't generate wDecrypted_ (need WControl has Pseudoinfo)
	}
bool WmESP_Crypted::generate(WControl& c,OCTBUF& s) {
	if(!sa_)return false;
	uint32_t decryptedlen = wDecrypted_->length();
	OCTBUF decryptedbuf(decryptedlen);
	wDecrypted_->generate(c,decryptedbuf);
/*SA*/	OCTBUF* cryptedbuf = sa_->Crypted_Octets(decryptedbuf);
	if(!cryptedbuf){
		fprintf(stderr,
			"err: SA(%s) doesn't return Crypted buffer\n",
			sa_->nameString());
		c.set_error(1);}
	set_rgenerate(cryptedbuf); //generater self life management
	return SUPER::generate(c,s);
	//Now Can generate wDecrypted_ (WControl has Pseudoinfo)
	}
void WmESP_Crypted::internalEvaluatePrologue(ItPosition&) {
	do_adopt_afterESP();
	ItPosition decit;
	wDecrypted_->evaluatePrologue(decit);
	}
bool WmESP_Crypted::doEvaluate(WControl& c,RObject& r) {
	//typecheck be finished by metaEvaluate()
	RmESP_Crypted& rm = (RmESP_Crypted&)r;
	RObject* rDecrypt = rm.rDecrypted();
	if(!rDecrypt){ c.result(1); notMatch(c,"can't decrypt",r); return c; }
	wDecrypted_->evaluate(c,rDecrypt);
	return c;}
void WmESP_Crypted::printChild(uint32_t t) const{
	SUPER::printChild(t);
	if(wDecrypted_)wDecrypted_->print(t);}
void WmESP_Crypted::describeChild(uint32_t t) const {
	SUPER::describeChild(t);
	if(wDecrypted_)wDecrypted_->describeChild(t);}
const TObject* WmESP_Crypted::corresponding(const MObject* m) const {
	const TObject* t=SUPER::corresponding(m);
	if(t==0&&wDecrypted_!=0) {t=wDecrypted_->corresponding(m);}
	return t;}
#undef SUPER

	
//////////////////////////////////////////////////////////////////////////////
#define SUPER	MmVarOctets

MmESP_ICV::MmESP_ICV(CSTR str):SUPER(str){}
MmESP_ICV::~MmESP_ICV(){}

ItPosition MmESP_ICV::length_for_reverse(
		RControl& c,ItPosition&,OCTBUF& ) const {
	const PaObject* sa = c.poping_SA();
	uint32_t length =0;
/*SA*/	if(sa){	length= sa->ICV_Length();}
	return ItPosition(length);}

RObject* MmESP_ICV::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	RObject* r_self = SUPER::reverse(c,r_parent,at,buf);
	if(!c.error()){// useless post reverse 
		Con_IPinfo tmp;
		r_self->post_reverse(tmp,c,r_self);}
	return r_self;}

bool MmESP_ICV::geneAuto(WControl& c,WObject* w_self,OCTBUF& buf)const{
	if(!c.error()){
		Con_IPinfo tmp;
		w_self->post_generate(tmp,c,buf,w_self);}
	return c;}
#undef SUPER


//////////////////////////////////////////////////////////////////////////////
class RmESP_ICV : public RmObject{
	const PaObject* sa_;
	PvObject*	calc_pvalue_;
public:
	RmESP_ICV(RObject* r_parent,const MObject* m,
		const ItPosition& offset,const ItPosition& size,PvObject* pv,
		const PaObject* sa);
virtual ~RmESP_ICV();
	void set_calc_pvalue(PvObject* calc);
	PvObject* calc_pvalue()const{return calc_pvalue_;}
virtual void post_reverse(Con_IPinfo&,RControl& c,RObject*);
virtual void printName(uint32_t,CSTR =0) const;
virtual void logSelf(uint32_t,CSTR =0) const;
};

RObject* MmESP_ICV::reverseRm(RControl& c,RObject* r_parent,
		const ItPosition& at,const ItPosition& size,PvObject* pv)const{
	const PaObject* sa = c.poping_SA();
	RmESP_ICV* r_self = new RmESP_ICV(r_parent,this,at,size,pv,sa);
	return r_self;}


#define SUPER	RmObject
RmESP_ICV::RmESP_ICV(RObject* r_parent,const MObject* m,
		const ItPosition& offset,const ItPosition& size,PvObject* pv,
		const PaObject* sa):SUPER(r_parent,m,offset,size,pv),
		sa_(sa),calc_pvalue_(new OCTBUF()){}
RmESP_ICV::~RmESP_ICV(){
	if(calc_pvalue_){delete calc_pvalue_; calc_pvalue_=0;}
	}
void RmESP_ICV::set_calc_pvalue(PvObject* calc){
	if(calc_pvalue_){delete calc_pvalue_;}
	calc_pvalue_=calc;}

void RmESP_ICV::post_reverse(Con_IPinfo&,RControl& c,RObject*){
	RObject* esp = parent();
	OCTBUF* espbuf = (OCTBUF*)esp->pvalue();
	uint32_t ahsize = offset().bytes();
	PvOctets ahbuf; espbuf->left(ahsize,&ahbuf);
	//
	OCTBUF* calc = 0;
/*SA*/	if(sa_)calc = sa_->ICV_Calculate(ahbuf);
	if(!calc)calc=new OCTBUF();
	set_calc_pvalue(calc);
	//
	const PvObject* rcv=pvalue();
	if(calc->compareObject(*rcv)!=0){
		c.unmatchMessage(meta()->string(),rcv,calc);
		c.set_warning(0);}
	if(DBGFLAGS('C')) {
		printf("== ESPICV overwrite buffer ==\n");
		ahbuf.print(); printf("\n");
		printf("== ESPICV calculate ==\n");
		calc->print(); printf("\n");}
	}

void RmESP_ICV::printName(uint32_t t,CSTR cls) const {
	SUPER::printName(t,cls);
	printf(" calc("); if(calc_pvalue_)calc_pvalue_->print(); printf(")");}
void RmESP_ICV::logSelf(uint32_t t,CSTR cls) const {
	SUPER::logSelf(t,cls);
	printf(" calc("); if(calc_pvalue_)calc_pvalue_->print(); printf(")");}
#undef SUPER

//////////////////////////////////////////////////////////////////////////////
class WmESP_ICV : public WmObject{
	const PaObject* sa_;
public:
	WmESP_ICV(WObject* p,const MObject* m,const PaObject* sa);
virtual	~WmESP_ICV();
// GENERATE/EVALUATE INTERFACE ------------------------------------------------
virtual	void internalGeneratePrologue(ItPosition& it);
virtual void post_generate(Con_IPinfo&,WControl& c,OCTBUF& buf,WObject*);
virtual	bool doEvaluate(WControl& c,RObject& r);
};

WObject* MmESP_ICV::composeWm(WControl& c,
		WObject* w_parent,const PObject*)const{
	const PaObject* sa = c.pushing_SA();
	WmESP_ICV* w_self = new WmESP_ICV(w_parent,this,sa);
	return w_self;}


#define SUPER	WmObject
WmESP_ICV::WmESP_ICV(WObject* p,const MObject* m,const PaObject* sa):
		SUPER(p,m,0),sa_(sa){}
WmESP_ICV::~WmESP_ICV(){
	}
void WmESP_ICV::internalGeneratePrologue(ItPosition& it){
	if(!sa_)return;
/*SA*/	uint32_t icvlen	= sa_->ICV_Length();
	it.addBytes(icvlen);
	}

void WmESP_ICV::post_generate(Con_IPinfo&,WControl& c,OCTBUF& buf,WObject*){
	WObject* esp = parent();
	OCTBUF* espbuf = (OCTBUF*)esp->pvalue();
	uint32_t ahsize = distance();
	PvOctets ahbuf; espbuf->left(ahsize,&ahbuf);
	//
	OCTBUF* calc = 0;
/*SA*/	if(sa_)calc = sa_->ICV_Calculate(ahbuf);
	if(!calc)calc=new OCTBUF();
	set_rgenerate(calc); //generater self life management
	SUPER::generate(c,buf);
	//
	if(DBGFLAGS('C')) {
		printf("== ESPICV overwrite buffer ==\n");
		ahbuf.print(); printf("\n");
		printf("== ESPICV calculate ==\n");
		calc->print(); printf("\n");}
	}

bool WmESP_ICV::doEvaluate(WControl& c,RObject& r) {
	//typecheck be finished by metaEvaluate()
	RmESP_ICV& rm = (RmESP_ICV&)r;
	const PvObject* ro	= rm.pvalue();
	const PvObject* eo	= revaluate();
	if(!eo)eo		= rm.calc_pvalue();
	return valueEvaluate(c,ro,eo);}
#undef SUPER


//////////////////////////////////////////////////////////////////////////////
RObject* MmESP_IVEC::Self_reverse(RControl& c,OCTBUF* selficv)const{
	ItPosition at,size(selficv->length());
	RObject* r_self = reverseRm(c,0,at,size,selficv);
	return r_self;}


//////////////////////////////////////////////////////////////////////////////
// class McESP_Decrypted: public McObject ////////////////////////////////////
#define SUPER	McObject

McESP_Decrypted::McESP_Decrypted(CSTR str):SUPER(str),
	NextHeader_(0),PadLength_(0){}
McESP_Decrypted::~McESP_Decrypted(){}

RObject* McESP_Decrypted::Self_reverse(RControl& c,OCTBUF* selfdecrypted)const{
//Right Adjustment ////////////////////////////////////////////
	bool ok=false;
	ItPosition RA_at(selfdecrypted->length());
//NextHeader
	uint32_t nexttype = NextHeader_->rdecode(RA_at,*selfdecrypted,ok);
	if(!ok){fprintf(stderr,
		"err: ESP NextHeader field decode over remain size(%d)",
			RA_at.bytes());
		c.set_error(1); return 0;}
//Padlen
	uint32_t padlen = PadLength_->rdecode(RA_at,*selfdecrypted,ok);
	if(!ok){fprintf(stderr,
		"err: ESP PadLength field decode over remain size(%d)",
			RA_at.bytes());
		c.set_error(1); return 0;}
//Pad
	ItPosition PDsize(padlen);
	if(RA_at<PDsize) {
		fprintf(stderr,
		"err: ESP Padding size(%d) over remain size(%d)\n",
			padlen,RA_at.bytes());
		c.set_error(1); return 0;}
	RA_at-=PDsize;
//Payload
	uint32_t paylen = RA_at.bytes();
//
	c.DictType().type_Set(nexttype);
	c.ESPinfo().set_PadLength(padlen);
	c.ESPinfo().set_PayloadLength(paylen);
//
	ItPosition at;
	RObject* r_self = reverseRc(c,0,at,selfdecrypted);
	ItPosition memat;
	members_reverse(c,r_self,memat,*selfdecrypted);
	return r_self;}

WObject* McESP_Decrypted::Self_composeList(
		WControl& c,const PObjectList& pls)const{
	WObject* w_self = composeWc(c,0,0);
	members_compose(c,w_self,pls);
	return w_self;}

bool McESP_Decrypted::HCGENE(PadLength)(
		WControl& cntr,WObject* wmem,OCTBUF& buf) const{
	WObject* wm		= wmem->prevSister();	//ESP_Padding
	uint32_t reallen	= wm->size().bytes();
	uint32_t valulen	= reallen;
	PvNumber def(valulen);
	return def.generate(cntr,wmem,buf);}

int32_t McESP_Decrypted::get_next_headerType(WObject* wmem)const{
	WObject* wc	= wmem->parent();	//ESP_Decrypted
	WObject* payload= wc->nextChild();	//ESP_Payload
	WObject* next	= payload->nextChild();	//extent,upper
	int32_t rtn	= next? next->meta()->headerType() : -1;
        if(rtn==-1)wmem->mustDefine(0);		//Payload is error
        return rtn;}

bool McESP_Decrypted::HCGENE(NextHeader)(
		WControl& cntr,WObject* wmem,OCTBUF& buf) const{
/**/	int32_t val	= get_next_headerType(wmem); //next child
	if(val==-1)return false;
	PvNumber def(val);
	return def.generate(cntr,wmem,buf);}

PObject* McESP_Decrypted::HCEVAL(NextHeader)(WObject* wmem) const{
/**/	int32_t val	= get_next_headerType(wmem); //next child
	return new PvNumber(val);}
	
McESP_Decrypted* McESP_Decrypted::create(CSTR key){
	McESP_Decrypted* mc = new McESP_Decrypted(key);
/*nolex*/mc->member( McESP_Payload::create(	"ESPPayload") );
/*nolex*/mc->member( new MmESP_Padding(		"Padding" ) );
	mc->member( mc->PadLength_=
		    new MmUint(	"PadLength",		8,
			new PvHCgene(mc,(HCgenefunc)&McESP_Decrypted::HCGENE(PadLength)),
			new PvANY() ) );
	mc->member( mc->NextHeader_=
		    new MmUint( "NextHeader",	8,
			new PvHCgene(mc,(HCgenefunc)&McESP_Decrypted::HCGENE(NextHeader)),
			new PvHCeval(mc,(HCevalfunc)&McESP_Decrypted::HCEVAL(NextHeader)) ) );
	//
	return mc;}


//////////////////////////////////////////////////////////////////////////////
#define SUPER	McObject

McESP_Payload::McESP_Payload(CSTR str):SUPER(str){}
McESP_Payload::~McESP_Payload(){}

uint32_t McESP_Payload::length_for_reverse(
		RControl& c,ItPosition& ,OCTBUF& )const{
	uint32_t length = c.ESPinfo().get_PayloadLength();
	return length;}

RObject* McESP_Payload::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
//Sorry only IPv6 ESP
	return SUPER::reverse(c,r_parent,at,buf);}

void McESP_Payload::composeList(WControl& c,
		WObject* w_parent,const PObjectList& /*pls*/) const {
	WObject* w_self = composeWc(c,w_parent,0); w_self=w_self;
	//Now can not members_compose,
	//after compose geneProloge WmESP_Crypted::do_adopt_afterESP()
	}

McESP_Payload* McESP_Payload::create(CSTR str){
	McESP_Payload* mc = new McESP_Payload(str);
/*nolex*/mc->member( new MmExtent_onIP(	"exthdr") );
/*nolex*/mc->member( new MmUpper_onIP(	"upper" ) );
	//
	return mc;}

#undef SUPER

//////////////////////////////////////////////////////////////////////////////
#define SUPER	MmVarOctets
MmESP_Padding::MmESP_Padding(CSTR key):SUPER(key){}
MmESP_Padding::~MmESP_Padding(){}

ItPosition MmESP_Padding::length_for_reverse(
		RControl& c,ItPosition&,OCTBUF&)const{
	uint32_t length = c.ESPinfo().get_PadLength();
	return ItPosition(length);}

WObject* MmESP_Padding::composeWm(WControl& c,
		WObject* w_parent,const PObject*)const{
	const PaObject* sa = c.pushing_SA();
	WmESP_Padding* w_self = new WmESP_Padding(w_parent,this,sa);
	return w_self;}
#undef SUPER

//-----------------------------------------------------------------------------
#define SUPER	WmObject
WmESP_Padding::WmESP_Padding(WObject* p,const MObject* m,const PaObject* sa):
		SUPER(p,m,0),sa_(sa){}
WmESP_Padding::~WmESP_Padding(){
	}
void WmESP_Padding::internalGeneratePrologue(ItPosition& it) {
	if(!sa_)return;
	WObject* payload = prevSister();	//ESP_Payload
	uint32_t paylen = payload->length();
/*SA*/	uint32_t paddinglen = sa_->Padding_Length(paylen+2);
/*SA*/	OCTBUF* padbuf = sa_->Padding_Octets(paddinglen);
	set_rgenerate(padbuf); //generater self life management
	it.addBytes(paddinglen);
	}
#undef SUPER

//////////////////////////////////////////////////////////////////////////////


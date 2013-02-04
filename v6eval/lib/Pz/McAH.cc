/*
 * Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011
 * Yokogawa Electric Corporation, 
 * INTAP (Interoperability Technology Association for Information Processing, Japan),
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
 * $TAHI: v6eval/lib/Pz/McAH.cc,v 1.15 2003/10/23 04:37:30 akisada Exp $
 */
#include "McAH.h"
#include "MmData.h"
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


//////////////////////////////////////////////////////////////////////////////
#define DEF_LENGTH_ELEM_AH	4
#define DEF_LENGTH_OFFSET_AH	8

#define SUPER	McHdr_Ext

McHdr_Ext_AH::McHdr_Ext_AH(CSTR key):SUPER(key){}
McHdr_Ext_AH::~McHdr_Ext_AH(){}

uint32_t McHdr_Ext_AH::alignment_requirement() const{
	if(provisional_AH_alignment_v4_v6_)return provisional_AH_alignment_v4_v6_;
	return SUPER::alignment_requirement();}//only IPv6 alignment(8)

// COMPOSE/REVERSE
uint32_t McHdr_Ext_AH::length_for_reverse(
		RControl& c,ItPosition& at,OCTBUF& buf)const{
	if(!length_)return SUPER::length_for_reverse(c,at,buf);
	uint32_t valulen= length_->value(at,buf);
	uint32_t length = (valulen*DEF_LENGTH_ELEM_AH)+DEF_LENGTH_OFFSET_AH;
	return length;}

RObject* McHdr_Ext_AH::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	const PaObject* keepSA = c.poping_SA(); c.poping_SA(0);
	RObject* r_self = SUPER::reverse(c,r_parent,at,buf);
	c.poping_SA(keepSA);
	return r_self;}
WObject* McHdr_Ext_AH::compose(WControl& c,
		WObject* w_parent,const PObject* pc) const {
	const PaObject* keepSA = c.pushing_SA(); c.pushing_SA(0);
	WObject* w_self = SUPER::compose(c,w_parent,pc);
	c.pushing_SA(keepSA);
	return w_self;}

bool McHdr_Ext_AH::HCGENE(HeaderExtLength)(
			WControl& cntr,WObject* wmem,OCTBUF& buf) const{
	WObject* wc	= wmem->parent();       //Hdr_Ext_AH
	uint32_t reallen= wc->size().bytes();
	uint32_t valulen= (reallen-DEF_LENGTH_OFFSET_AH)/DEF_LENGTH_ELEM_AH;
	PvNumber def(valulen);
	return def.generate(cntr,wmem,buf);}

bool McHdr_Ext_AH::overwrite_ICV(ICVControl& c,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const{
	return t->overwrite_ICV_child(c,at,buf);}//forward to member

#define	UN(n) PvNumber::unique(n)
#define	MUST()PvMUSTDEF::must()
#define EVALANY()	new PvANY()
#define GENEHC(mc,cls,mem)	new PvHCgene(mc,(HCgenefunc)&cls::HCGENE(mem))
#define EVALHC(mc,cls,mem)	new PvHCeval(mc,(HCevalfunc)&cls::HCEVAL(mem))
#define ICVCONST()	0
#define ICVZERO()	new ICV_Zero()
#define DEF_EVALSKIP	true

McHdr_Ext_AH* McHdr_Ext_AH::create(CSTR key){
	McHdr_Ext_AH* mc = new McHdr_Ext_AH(key);
	mc->nextType_member(
		    new MmUint( "NextHeader",	8,
			GENEHC(mc,McHdr_Ext_AH,NextHeader),
			EVALHC(mc,McHdr_Ext_AH,NextHeader),	ICVCONST() ) );
	mc->length_member(
		    new MmUint( "PayloadLength",8,
			GENEHC(mc,McHdr_Ext_AH,HeaderExtLength),
			EVALANY(),		ICVCONST() ) );
	mc->member( new MmUint(	"Reserved",	16,
			UN(0),	UN(0),		ICVCONST() ) );
	mc->member( new MmSPI(	"SPI",		32,
			UN(0),	UN(0),		ICVCONST() ) );
	mc->member( new MmUint( "SequenceNumber",32,
			UN(0),	UN(0),		ICVCONST() ) );
	mc->member( new MmAlgorithm_onAH("algorithm")); //reference
	mc->member( new MmAH_ICV("ICV",		ICVZERO() ));
	mc->member( new MmData( "Padding",DEF_EVALSKIP, ICVCONST() ) );
	// dict
	MmExtent_onIP::add(mc);			//Packet_IP::exthdr=
	return mc;}
#undef SUPER

//////////////////////////////////////////////////////////////////////////////
class RcAH:public RcObject {
private:
	const PaObject* algorithm_;
public:
	RcAH(RObject*,const MObject*,const ItPosition& o,PvObject* =0);
virtual	~RcAH();
virtual	void algorithm(const PaObject* sa){algorithm_=sa;}
virtual	void printChild(uint32_t) const;
};

RObject* McHdr_Ext_AH::reverseRc(RControl&,
		RObject* r_parent,const ItPosition& at,PvObject* pv) const {
	return new RcAH(r_parent,this,at,pv);}

RcAH::RcAH(RObject* r,const MObject* m,const ItPosition& o,PvObject* p):
	RcObject(r,m,o,p),algorithm_(0) {}
RcAH::~RcAH() {}
void RcAH::printChild(uint32_t t) const {
	if(DBGFLAGS('S')){
		if(algorithm_){printf("== AH SA ==\n"); algorithm_->print();}}
	RcObject::printChild(t);}


//////////////////////////////////////////////////////////////////////////////
class WcAH:public WcObject {
private:
	const PaObject* algorithm_;
public:
	WcAH(WObject*,const MObject*,const PObject* =0);
virtual	~WcAH();
virtual	void algorithm(const PaObject* sa){algorithm_=sa;}
virtual	void printChild(uint32_t) const;
};

WObject* McHdr_Ext_AH::composeWc(WControl&,
		WObject* w_parent,const PObject* pc) const {
	return new WcAH(w_parent,this,pc);}

WcAH::WcAH(WObject* w,const MObject* m,const PObject* p):WcObject(w,m,p),
	algorithm_(0) {}
WcAH::~WcAH() {}
void WcAH::printChild(uint32_t t) const {
	if(DBGFLAGS('S')){
		if(algorithm_){printf("== AH SA ==\n"); algorithm_->print();}}
	WcObject::printChild(t);}


//////////////////////////////////////////////////////////////////////////////
#define SUPER	MmReference_Must1
#include "PvName.h"
MmAlgorithm_onAH::MmAlgorithm_onAH(CSTR key):SUPER(key) {}
MmAlgorithm_onAH::~MmAlgorithm_onAH() {}
RObject* MmAlgorithm_onAH::reverse(
		RControl& c,RObject* r_parent,ItPosition& at,OCTBUF&)const{
	RObject* r_self=0;
	c.set_pop_SA(eAH_);//select&set SA
	const PaObject* sa = c.poping_SA();
	if(sa){ RcAH* rcah = (RcAH*)r_parent;
		if(rcah)rcah->algorithm(sa);
		PvObject* pv = sa->nameObject()->shallowCopy();
		ItPosition size(0);
		r_self = new RmObject(r_parent,this,at,size,pv); }
	return r_self;}// not reverse

bool MmAlgorithm_onAH::disused() const{return true;}
#undef SUPER


//////////////////////////////////////////////////////////////////////////////
#define SUPER	MmVarOctets

MmAH_ICV::MmAH_ICV(CSTR str,const ICVoverwriter* ow):SUPER(str,0,0,ow){}
MmAH_ICV::~MmAH_ICV(){}

ItPosition MmAH_ICV::length_for_reverse(
		RControl& c,ItPosition& at,OCTBUF& buf) const {
	const PaObject* sa = c.poping_SA();
	uint32_t length =0;
if(DBGFLAGS('V')){
	//Request Spec (for v6eval)
	//if ICV can not find SA , ICV Length = remainLength onAH
	length = buf.remainLength(at.bytes());
}
/*SA*/	if(sa){	length= sa->ICV_Length();}
	return ItPosition(length);}

//post add
RObject* MmAH_ICV::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	RObject* r_self = SUPER::reverse(c,r_parent,at,buf);
	if(!c.error()){
		Con_IPinfo* info = c.IPinfo();
		if(info)info->postAHICV(r_self);}//post reverse calcICV
	return r_self;}

bool MmAH_ICV::geneAuto(WControl& c,WObject* w_self,OCTBUF&) const{
	if(!c.error()){
		Con_IPinfo* info = c.IPinfo();
		if(info)info->postAHICV(w_self);}//post generate calcICV
	return c;}
#undef SUPER


///////////////////////////////////////////////////////////////////////////////
class RmAH_ICV : public RmObject{
	const PaObject*	sa_;
	PvObject*	calc_pvalue_;
public:
	RmAH_ICV(RObject* r_parent,const MObject* m,
		const ItPosition& offset,const ItPosition& size,PvObject* pv,
		const PaObject* sa);
virtual ~RmAH_ICV();
	void set_calc_pvalue(PvObject* icvbuf);
	PvObject* calc_pvalue()const{return calc_pvalue_;}
virtual void post_reverse(Con_IPinfo& info,RControl&,RObject* pkt);
virtual void printName(uint32_t,CSTR =0) const;
virtual void logSelf(uint32_t,CSTR =0) const;
};

RObject* MmAH_ICV::reverseRm(RControl& c,RObject* r_parent,
		const ItPosition& at,const ItPosition& size,PvObject* pv)const{
	const PaObject* sa = c.poping_SA();
	RmAH_ICV* r_self = new RmAH_ICV(r_parent,this,at,size,pv,sa);
	return r_self;}


#define SUPER	RmObject
RmAH_ICV::RmAH_ICV(RObject* r_parent,const MObject* m,
		const ItPosition& offset,const ItPosition& size,PvObject* pv,
		const PaObject* sa):SUPER(r_parent,m,offset,size,pv),
		sa_(sa),calc_pvalue_(new OCTBUF()){}
RmAH_ICV::~RmAH_ICV(){
	if(calc_pvalue_){delete calc_pvalue_; calc_pvalue_=0;}
	}
void RmAH_ICV::set_calc_pvalue(PvObject* calc){
	if(calc_pvalue_){delete calc_pvalue_;}
	calc_pvalue_=calc;}

void RmAH_ICV::post_reverse(Con_IPinfo& info,RControl& c,RObject* pkt){
	OCTBUF* pktbuf = (OCTBUF*)pkt->pvalue();
	OCTBUF ahbuf(pktbuf->length(),(OCTSTR)pktbuf->string(),true);
	ICVControl icvc;
	icvc.LastDstAddr(info.LastDstAddr());
	icvc.HomeAddr(info.SrcAddr());	// MIP6AH

	ItPosition at;
	pkt->overwrite_ICV_child(icvc,at,ahbuf);
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
		info.print(); printf("\n");
		printf("== AHICV overwrite buffer ==\n");
		ahbuf.print(); printf("\n");
		printf("== AHICV calculate ==\n");
		calc->print(); printf("\n");}
	}

void RmAH_ICV::printName(uint32_t t,CSTR cls) const {
	SUPER::printName(t,cls);
	printf(" calc("); if(calc_pvalue_)calc_pvalue_->print(); printf(")");}
void RmAH_ICV::logSelf(uint32_t t,CSTR cls) const {
	SUPER::logSelf(t,cls);
	printf(" calc("); if(calc_pvalue_)calc_pvalue_->print(); printf(")");}
#undef SUPER


///////////////////////////////////////////////////////////////////////////////
class WmAH_ICV : public WmObject{
	const PaObject*	sa_;
public:
	WmAH_ICV(WObject* p,const MObject* m,const PaObject* sa);
virtual ~WmAH_ICV();
// GENERATE/EVALUATE INTERFACE ------------------------------------------------
virtual void internalGeneratePrologue(ItPosition& it);
virtual void post_generate(Con_IPinfo& info,WControl& c,OCTBUF& buf,
				WObject* pkt);
virtual bool doEvaluate(WControl& c,RObject& r);
};

WObject* MmAH_ICV::composeWm(WControl& c,
		WObject* w_parent,const PObject*)const{
	const PaObject* sa = c.pushing_SA();
	WmAH_ICV* w_self = new WmAH_ICV(w_parent,this,sa);
	return w_self;}


#define SUPER	WmObject
WmAH_ICV::WmAH_ICV(WObject* p,const MObject* m,const PaObject* sa):
		SUPER(p,m,0),sa_(sa){}
WmAH_ICV::~WmAH_ICV(){}

void WmAH_ICV::internalGeneratePrologue(ItPosition& it){
	if(!sa_)return;
/*SA*/	uint32_t icvlen	= sa_->ICV_Length();
	it.addBytes(icvlen);
	}
void WmAH_ICV::post_generate(Con_IPinfo& info,WControl& c,OCTBUF& buf,
		WObject* pkt){
	OCTBUF* pktbuf = (OCTBUF*)pkt->pvalue();
	OCTBUF ahbuf(pktbuf->length(),(OCTSTR)pktbuf->string(),true);
	ICVControl icvc;
	icvc.LastDstAddr(info.LastDstAddr());
	icvc.HomeAddr(info.SrcAddr());	// MIP6AH

	ItPosition at;
	pkt->overwrite_ICV_child(icvc,at,ahbuf);
	//
	OCTBUF* calc = 0;
/*SA*/	if(sa_)calc = sa_->ICV_Calculate(ahbuf);
	if(!calc)calc=new OCTBUF();
	set_rgenerate(calc); //generater self life management
	SUPER::generate(c,buf);
	//
	if(DBGFLAGS('C')) {
		info.print(); printf("\n");
		printf("== AHICV overwrite buffer ==\n");
		ahbuf.print(); printf("\n");
		printf("== AHICV calculate ==\n");
		calc->print(); printf("\n");}
	}

bool WmAH_ICV::doEvaluate(WControl& c,RObject& r) {
	//typecheck be finished by metaEvaluate()
	RmAH_ICV& rm = (RmAH_ICV&)r;
	const PvObject* ro	= rm.pvalue();
	const PvObject* eo	= revaluate();
	if(!eo)eo		= rm.calc_pvalue(); //auto
	return valueEvaluate(c,ro,eo);}
#undef SUPER


//////////////////////////////////////////////////////////////////////////////


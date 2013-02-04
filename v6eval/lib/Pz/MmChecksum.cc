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
 * $TAHI: v6eval/lib/Pz/MmChecksum.cc,v 1.26 2003/10/23 04:37:30 akisada Exp $
 */
#include "MmChecksum.h"
#include "PControl.h"
#include "RObject.h"
#include "WObject.h"
#include "PvObject.h"
#include "PvOctets.h"
#include "CmMain.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////
#define SUPER	MmUint

MmChecksum::MmChecksum(CSTR s,uint16_t w,
	bool optionable,bool usepseudo,const ICVoverwriter* ow):
	SUPER(s,w,0,0,ow),optionable_(optionable),usepseudo_(usepseudo){}
MmChecksum::~MmChecksum(){}

RObject* MmChecksum::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	RObject* r_self = SUPER::reverse(c,r_parent,at,buf);
	if(!c.error()){
		add_post(c.IPinfo(),r_self);}
	return r_self;}

bool MmChecksum::geneAuto(WControl& c,WObject* w_self,OCTBUF&)const{
	if(!c.error()){
		add_post(c.IPinfo(),w_self);}
	return c;}

void MmChecksum::add_post(Con_IPinfo*,TObject*)const{}
#undef SUPER

//////////////////////////////////////////////////////////////////////////////
#define SUPER	MmChecksum

MmUppChecksum::MmUppChecksum(CSTR s,uint16_t w,
	bool optionable,bool usepseudo,const ICVoverwriter* ow):
	SUPER(s,w,optionable,usepseudo,ow){}
MmUppChecksum::~MmUppChecksum(){}
void MmUppChecksum::add_post(Con_IPinfo* info,TObject* self)const{
	if(info)info->postUppChecksum(self);}//post generate/reverse

MmIPChecksum::MmIPChecksum(CSTR s,uint16_t w,
	bool optionable,const ICVoverwriter* ow):
	SUPER(s,w,optionable,false,ow){}
MmIPChecksum::~MmIPChecksum(){}
void MmIPChecksum::add_post(Con_IPinfo* info,TObject* self)const{
	if(info)info->postIPChecksum(self);}//post generate/reverse

#undef SUPER

/////////////////////////////////////////////////////////////////////////////
class ChecksumCalculater{
public:
	ChecksumCalculater(){}
	uint32_t calculate(Con_IPinfo& info,int32_t upptype,
		const PvOctets& uppbuf,bool opt)const;
	uint32_t calculateWithLength(Con_IPinfo &, int32_t, const PvOctets &, bool, uint32_t) const;
	uint32_t calculate(const PvOctets& uppbuf,bool opt)const;
};
uint32_t ChecksumCalculater::calculate(Con_IPinfo& info,
		int32_t upptype,const PvOctets& uppbuf,bool optionable)const{
	if(!info.SrcAddr()||!info.LastDstAddr()||upptype==-1)return 0;
	const PvOctets& src=(const PvOctets&)*info.SrcAddr();
	const PvOctets& dst=(const PvOctets&)*info.LastDstAddr();
	uint32_t upplen = uppbuf.length();
	uint32_t sum=src.sum2Octets()+dst.sum2Octets()
		+upplen+upptype+uppbuf.sum2Octets();
	uint32_t checksum = ::xSum(sum);
	if(optionable && !checksum){
		//If the computed checksum is zero,it is transmitted as all ones
		checksum = ~checksum;}
	return checksum;}

uint32_t ChecksumCalculater::calculateWithLength(Con_IPinfo &info,
	int32_t upptype, const PvOctets &uppbuf, bool optionable, uint32_t targetlength) const {

	if((!info.SrcAddr()) || (!info.LastDstAddr()) || (upptype == -1)) {
		return(0);
	}

	const PvOctets &src = (const PvOctets &)*info.SrcAddr();
	const PvOctets &dst = (const PvOctets &)*info.LastDstAddr();

	uint32_t upplen = targetlength;
	uint32_t sum = src.sum2Octets() + dst.sum2Octets() + upplen + upptype + uppbuf.sum2Octets();
	uint32_t checksum = ::xSum(sum);

	if((optionable) && (!checksum)){
		//If the computed checksum is zero,it is transmitted as all ones
		checksum = ~checksum;
	}

	return(checksum);
}

uint32_t ChecksumCalculater::calculate(
		const PvOctets& buf,bool optionable)const{
	uint32_t sum = buf.sum2Octets();
	uint32_t checksum = ::xSum(sum);
	if(optionable && !checksum){
		//If the computed checksum is zero,it is transmitted as all ones
		checksum = ~checksum;}
	return checksum;}

/////////////////////////////////////////////////////////////////////////////

class RmChecksum : public RmObject{
	bool		optionable_;
	bool		usepseudo_;
	PvObject*	calc_pvalue_;
public:
	RmChecksum(RObject* r_parent,const MObject* m,
		const ItPosition& offset,const ItPosition& size,PvObject* pv,
		bool optionable,bool usepseudo);
virtual	~RmChecksum();
	void set_calc_pvalue(PvObject* calc);
	PvObject* calc_pvalue()const{return calc_pvalue_;}
virtual void post_reverse(Con_IPinfo& info,RControl&,RObject* base);
virtual void printName(uint32_t t,CSTR cls) const ;
virtual void logSelf(uint32_t t,CSTR cls) const ;
};

RObject* MmChecksum::reverseRm(RControl&,RObject* r_parent,
		const ItPosition& at,const ItPosition& size,PvObject* pv)const{
	RmChecksum* r_self = new RmChecksum(r_parent,this,at,size,pv,
					optionable_,usepseudo_);
	return r_self;}


#define SUPER	RmObject
RmChecksum::RmChecksum(RObject* r_parent,const MObject* m,
		const ItPosition& offset,const ItPosition& size,PvObject* pv,
		bool optionable,bool usepseudo):
		SUPER(r_parent,m,offset,size,pv),
		optionable_(optionable),usepseudo_(usepseudo),
		calc_pvalue_(new OCTBUF()){}
RmChecksum::~RmChecksum(){
	if(calc_pvalue_){delete calc_pvalue_; calc_pvalue_=0;}
	}

void RmChecksum::set_calc_pvalue(PvObject* calc){
	if(calc_pvalue_){delete calc_pvalue_;}
	calc_pvalue_=calc;}

void RmChecksum::post_reverse(Con_IPinfo& info,RControl& c,RObject* base){
	OCTBUF* basebuf = (OCTBUF*)base->pvalue();
	OCTBUF checkbuf(basebuf->length(),(OCTSTR)basebuf->string(),true);
#if 1
	ItPosition at;
	RObject* r = this;
	while(r&&r!=base){
		at += r->offset(); r=r->parent();}
	checkbuf.encodeZero(at,size());
#else
//	base->overwrite_Checksum_child(c,ItPosition(),checkbuf);
#endif
	ChecksumCalculater cal;
	uint32_t calc = 0;
	int32_t basetype = -1;
	if(usepseudo_){
		basetype = base->meta()->headerType();
		calc = cal.calculate(info,basetype,checkbuf,optionable_);}
	else{	calc = cal.calculate(checkbuf,optionable_);}
	PvNumber* pvcalc=new PvNumber(calc);
	set_calc_pvalue(pvcalc);
	//
	PvNumber* decpv = (PvNumber*)pvalue();
	uint32_t dec = decpv->value();
	if(dec!=calc){
		c.unmatchMessage(meta()->string(),decpv,pvcalc);
		c.set_warning(0);}// wcode 0 ok?
	if(DBGFLAGS('C')) {
		if(usepseudo_){
			info.print(); printf("\n");
			printf("UppType = %d\n",basetype);}
		printf("== Checksum overwrite buffer ==\n");
		checkbuf.print(); printf("\n");
		}
	}

void RmChecksum::printName(uint32_t t,CSTR cls) const {
	SUPER::printName(t,cls);
	printf(" calc("); if(calc_pvalue_)calc_pvalue_->print(); printf(")");}
void RmChecksum::logSelf(uint32_t t,CSTR cls) const {
	SUPER::logSelf(t,cls);
	printf(" calc("); if(calc_pvalue_)calc_pvalue_->print(); printf(")");}

#undef SUPER


///////////////////////////////////////////////////////////////////////////////
class WmChecksum : public WmObject{
	bool optionable_;
	bool usepseudo_;
public:
	WmChecksum(WObject* p,const MObject* m,const PObject* po,
		bool optionable=false,bool usepseudo=true);
virtual ~WmChecksum();
virtual void post_generate(Con_IPinfo&,WControl&,OCTBUF& buf,WObject* from);
virtual void post_generateWithLength(Con_IPinfo &, WControl &, OCTBUF &, WObject *, uint32_t);
virtual bool doEvaluate(WControl& c,RObject& r);
};

WObject* MmChecksum::composeWm(WControl&,
		WObject* w_parent,const PObject* po)const{
	WmChecksum* w_self = new WmChecksum(w_parent,this,po,
					optionable_,usepseudo_);
	return w_self;}


#define SUPER	WmObject
WmChecksum::WmChecksum(WObject* p,const MObject* m,const PObject* po,
		bool optionable,bool usepseudo):
	SUPER(p,m,po),optionable_(optionable),usepseudo_(usepseudo){}
WmChecksum::~WmChecksum(){}

void WmChecksum::post_generate(Con_IPinfo& info,WControl& c,OCTBUF& buf,
		WObject* base){
	const OCTBUF* basebuf = (const OCTBUF*)base->pvalue();
	OCTBUF checkbuf(basebuf->length(),(OCTSTR)basebuf->string(),true);
#if 1
	ItPosition at = offset(); //frameoffset
	at -= base->offset();//distance
	checkbuf.encodeZero(at,size());
#else
//	base->overwrite_Checksum_child(c,ItPosition(),checkbuf);
#endif
	ChecksumCalculater cal;
	uint32_t calc = 0;
	int32_t basetype = -1;
	if(usepseudo_){
		basetype = base->meta()->headerType();
		calc = cal.calculate(info,basetype,checkbuf,optionable_);}
	else{	calc = cal.calculate(checkbuf,optionable_);}
	set_rgenerate(new PvNumber(calc));
	SUPER::generate(c,buf);
	//
	if(DBGFLAGS('C')) {
		if(usepseudo_){
			info.print(); printf("\n");
			printf("UppType = %d\n",basetype);}
		printf("== Checksum overwrite buffer ==\n");
		checkbuf.print(); printf("\n");
		}
	}

void WmChecksum::post_generateWithLength(Con_IPinfo &info, WControl &c, OCTBUF &buf, WObject *base, uint32_t Xlength) {
	const OCTBUF *basebuf = (const OCTBUF *)base->pvalue();

	uint32_t targetlength = basebuf->length();

	if(Xlength < targetlength) {
		targetlength = Xlength;
	}

	OCTBUF checkbuf(targetlength, (OCTSTR)basebuf->string(), true);

	//
	// XXX
	//
	// (1) target is the last payload
	//	len: 16 byte, buf: 16 byte	-> OK
	//	len: 08 byte, buf: 16 byte	-> OK
	//	len: 24 byte, buf: 16 byte	-> OK
	//
	// (2) target is intermediate payload (like piggyback w/ MH)
	//	len: 16 byte, buf: 16 byte	-> OK
	//	len: 08 byte, buf: 16 byte	-> OK
	//	len: 24 byte, buf: 16 byte	-> NG	How shold I get extra payload buffer?
	//

	ItPosition at = offset();	//frameoffset
	at -= base->offset();	//distance
	checkbuf.encodeZero(at, size());

	ChecksumCalculater cal;
	uint32_t calc = 0;
	int32_t basetype = -1;

	if(usepseudo_){
		basetype = base->meta()->headerType();
		calc = cal.calculateWithLength(info, basetype, checkbuf, optionable_, Xlength);
	} else {
		calc = cal.calculate(checkbuf, optionable_);
	}

	set_rgenerate(new PvNumber(calc));
	SUPER::generate(c,buf);

	if(DBGFLAGS('C')) {
		if(usepseudo_){
			info.print();
			printf("\n");
			printf("UppType = %d\n", basetype);
		}

		printf("== Checksum overwrite buffer ==\n");
		checkbuf.print();
		printf("\n");
	}

	return;
}

bool WmChecksum::doEvaluate(WControl& c,RObject& r) {
	//typecheck be finished by metaEvaluate()
	RmChecksum& rm = (RmChecksum&)r;
	const PvObject* ro	= rm.pvalue();
	const PvObject* eo	= revaluate();
	if(!eo)eo		= rm.calc_pvalue(); //auto
	return valueEvaluate(c,ro,eo);}

#undef SUPER

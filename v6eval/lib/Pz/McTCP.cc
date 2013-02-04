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
 * $TAHI: v6eval/lib/Pz/McTCP.cc,v 1.7 2001/10/12 04:56:15 tanaka Exp $
 */
#include "McTCP.h"
#include "MmData.h"
#include "ItPosition.h"
#include "WObject.h"
#include "RObject.h"
#include "PControl.h"
#include "PvObject.h"
#include "PvOctets.h"

#define DEF_ALIGNMENT_TCP	4
#define DEF_LENGTH_ELEM_TCP	4

//////////////////////////////////////////////////////////////////////////////
#define SUPER	McUpper
McUpp_TCP* McUpp_TCP::instance_=0;
McTopHdr_TCP* McUpp_TCP::tophdr_=0;
McUpp_TCP::McUpp_TCP(CSTR key):SUPER(key) {instance_=this;}
McUpp_TCP::~McUpp_TCP() {if(instance_==this)instance_=0;}

// COMPOSE/REVERSE
uint32_t McUpp_TCP::length_for_reverse(
		RControl& c,ItPosition& at,OCTBUF& buf) const{
	uint32_t length = tophdr_->Layerlength_for_reverse(c,at,buf);
	return length;}

RObject* McUpp_TCP::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	RObject* r_self = SUPER::reverse(c,r_parent,at,buf);
	if(!c.error()){
		Con_IPinfo* info = c.IPinfo();
		if(info)info->reverse_postUppChecksum(c,r_self);}
	return r_self;}

bool McUpp_TCP::generate(WControl& c,WObject* w_self,OCTBUF& buf) const {
	bool rtn = SUPER::generate(c,w_self,buf);
	if(!c.error()){
		Con_IPinfo* info = c.IPinfo();
		if(info)info->generate_postUppChecksum(c,buf,w_self);}
	return rtn;}
#undef SUPER

//----------------------------------------------------------------------------
#define SUPER	McHeader
McTopHdr_TCP::McTopHdr_TCP(CSTR key):SUPER(key),length_(0){}
McTopHdr_TCP::~McTopHdr_TCP(){}
uint32_t McTopHdr_TCP::alignment_requirement() const{
	return DEF_ALIGNMENT_TCP;}

// COMPOSE/REVERSE
uint32_t McTopHdr_TCP::length_for_reverse(
		RControl& c,ItPosition& at,OCTBUF& buf) const{
	if(!length_)return SUPER::length_for_reverse(c,at,buf);
	uint32_t valulen	= length_->value(at,buf);
	uint32_t length		= valulen*DEF_LENGTH_ELEM_TCP;
	return length;}

uint32_t McTopHdr_TCP::Layerlength_for_reverse(
			RControl&,ItPosition& at,OCTBUF& buf) const{
	return buf.remainLength(at.bytes());}

bool McTopHdr_TCP::HCGENE(DataOffset)(
			WControl& cntr,WObject* wmem,OCTBUF& buf) const{
	WObject* wc		= wmem->parent();	//TopHdr_TCP
	uint32_t reallen	= wc->size().bytes();
	uint32_t valulen	= reallen/DEF_LENGTH_ELEM_TCP;
	PvNumber def(valulen);
	return def.generate(cntr,wmem,buf);}
#undef SUPER


///////////////////////////////////////////////////////////////////////////////
#define SUPER	McOption
McOpt_TCP::McOpt_TCP(CSTR key):SUPER(key),kind_(0),length_(0){}
McOpt_TCP::~McOpt_TCP(){}

// COMPOSE/REVERSE
uint32_t McOpt_TCP::length_for_reverse(RControl& c,
		ItPosition& at,OCTBUF& buf) const{
	if(!length_)return SUPER::length_for_reverse(c,at,buf);
	uint32_t valulen	= length_->value(at,buf);
	uint32_t length		= valulen;
	return length;}

bool McOpt_TCP::HCGENE(Length)(WControl& cntr,WObject* wmem,OCTBUF& buf)const{
	WObject* wc		= wmem->parent();	//Opt_TCP
	uint32_t reallen	= wc->size().bytes();
	uint32_t valulen	= reallen;
	PvNumber def(valulen);
	return def.generate(cntr,wmem,buf);}

bool McOpt_TCP::overwrite_DictType(
		RControl& c,ItPosition& at,OCTBUF& buf)const{
	if(c.DictType().finish())return false;	//End of OptionList
	uint32_t limit = buf.remainLength(at.bytes());
	if(limit==0)return false;		//End of TCP Header
	//
	ItPosition tmpat=at;
	RObject* rkind = kind_->reverse(c,0,tmpat,buf);
	if(!rkind)return false;			//Type field decode error
	//
	const PvNumber* pv = (const PvNumber*)rkind->pvalue();
	uint32_t kindvalue = pv->value();
	c.DictType().type_Set(kindvalue);	//self Type set
	delete rkind;
	return true;}
#undef SUPER

///////////////////////////////////////////////////////////////////////////////
#define SUPER	McOpt_TCP

//----------------------------------------------------------------------------
McOpt_TCP_ANY::McOpt_TCP_ANY(CSTR key):SUPER(key){}
McOpt_TCP_ANY::~McOpt_TCP_ANY(){}

//----------------------------------------------------------------------------
McOpt_TCP_EndofOptionList::McOpt_TCP_EndofOptionList(CSTR key):SUPER(key){}
McOpt_TCP_EndofOptionList::~McOpt_TCP_EndofOptionList(){}

RObject* McOpt_TCP_EndofOptionList::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	RObject* r_self = SUPER::reverse(c,r_parent,at,buf);
	c.DictType().finish_Set(); // nextType must finish
	return r_self;}

//----------------------------------------------------------------------------
McOpt_TCP_NoOperation::McOpt_TCP_NoOperation(CSTR key):SUPER(key){}
McOpt_TCP_NoOperation::~McOpt_TCP_NoOperation(){}

//----------------------------------------------------------------------------
#define DEF_ALIGNMENT_MaximumSegmentSize	2
McOpt_TCP_MaximumSegmentSize::McOpt_TCP_MaximumSegmentSize(CSTR key):
	SUPER(key){}
McOpt_TCP_MaximumSegmentSize::~McOpt_TCP_MaximumSegmentSize(){}

uint32_t McOpt_TCP_MaximumSegmentSize::alignment_requirement() const{
	return DEF_ALIGNMENT_MaximumSegmentSize;}

/////////////////////////////////////////////////////////////////////////////// // create options
#include "PvAutoItem.h"
#include "PvAction.h"

#define UN(n)		PvNumber::unique(n)
#define MUST()		PvMUSTDEF::must()
#define EVALANY()	new PvANY()
#define GENEHC(mc,cls,mem)	new PvHCgene(mc,(HCgenefunc)&cls::HCGENE(mem))
#define EVALHC(mc,cls,mem)	new PvHCeval(mc,(HCevalfunc)&cls::HCEVAL(mem))

#define LEXADD(classname,lexname) addCompound(classname::create(lexname))
void McOpt_TCP::create_options(){
	LEXADD(McOpt_TCP_ANY,			"Opt_TCP_ANY" );
	LEXADD(McOpt_TCP_EndofOptionList,	"Opt_TCP_EndofOptionList" );
	LEXADD(McOpt_TCP_NoOperation,		"Opt_TCP_NoOperation" );
	LEXADD(McOpt_TCP_MaximumSegmentSize,	"Opt_TCP_MaximumSegmentSize" );
	}

void McOpt_TCP::common_member(){
        int32_t Kind = optionType();
        kind_member(	new MmUint( "Kind",	8,	UN(Kind),UN(Kind) ) );
        length_member(	new MmUint( "Length",	8,
				GENEHC(this,McOpt_TCP,Length),	EVALANY() ) );
        }

McOpt_TCP_ANY* McOpt_TCP_ANY::create(CSTR key){
	McOpt_TCP_ANY* mc = new McOpt_TCP_ANY(key);
	mc->common_member();
	mc->member( new MmData( "data" ) );
	// dict
	MmOption_onTCP::add_other(mc);		//Hdr_TCP::option=
	return mc;}
#if 0
ANY(Unknown Type)
        +--------+--------+--------//--------+
        |Kind    | Length | data             |
        +--------+--------+--------//--------+
#endif

McOpt_TCP_EndofOptionList* McOpt_TCP_EndofOptionList::create(CSTR key){
	McOpt_TCP_EndofOptionList* mc = new McOpt_TCP_EndofOptionList(key);
        int32_t Kind = mc->optionType();
        mc->kind_member( new MmUint( "Kind",	8,	UN(Kind),UN(Kind) ) );
	// dict
	MmOption_onTCP::add(mc);		//Hdr_TCP::option=
	return mc;}
#if 0
End of Option List
        +--------+
        |00000000|
        +--------+
#endif

McOpt_TCP_NoOperation* McOpt_TCP_NoOperation::create(CSTR key){
	McOpt_TCP_NoOperation* mc = new McOpt_TCP_NoOperation(key);
        int32_t Kind = mc->optionType();
        mc->kind_member( new MmUint( "Kind",	8,	UN(Kind),UN(Kind) ) );
	// dict
	MmOption_onTCP::add(mc);		//Hdr_TCP::option=
	return mc;}
#if 0
No Operation
        +--------+
        |00000001|
        +--------+
#endif

McOpt_TCP_MaximumSegmentSize* McOpt_TCP_MaximumSegmentSize::create(CSTR key){
	McOpt_TCP_MaximumSegmentSize* mc =
		new McOpt_TCP_MaximumSegmentSize(key);
	mc->common_member();
	mc->member( new MmUint( "MaxSegSize",	16,	UN(0),UN(0) ) );
	// dict
	MmOption_onTCP::add(mc);		//Hdr_TCP::option=
	return mc;}
#if 0
Maximum Segment Size
	+--------+--------+---------+--------+
        |00000010|00000100|   max seg size   |
        +--------+--------+---------+--------+
#endif


///////////////////////////////////////////////////////////////////////////////
MmOption_onTCP::MmOption_onTCP(CSTR key):MmReference_More0(key,true) {}
MmOption_onTCP::~MmOption_onTCP() {}

void MmOption_onTCP::add(McOpt_TCP* mc){
	dict_.add(mc->optionType(),mc);}
void MmOption_onTCP::add_other(McOpt_TCP* mc){dict_.add_other(mc);}
TypevsMcDict MmOption_onTCP::dict_;

// REVERSE
bool MmOption_onTCP::overwrite_DictType(
		RControl& c,ItPosition& at,OCTBUF& buf)const{
	McOpt_TCP* any = (McOpt_TCP*)dict_.find_other();
	return any->overwrite_DictType(c,at,buf);}

///////////////////////////////////////////////////////////////////////////////

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
 * $TAHI: v6eval/lib/Pz/McIPv4.cc,v 1.10 2006/01/10 09:49:37 akisada Exp $
 */
#include "McIPv4.h"
#include "MmData.h"
#include "ItPosition.h"
#include "WObject.h"
#include "RObject.h"
#include "PControl.h"
#include "PvObject.h"
#include "PvOctets.h"

#define DEF_ALIGNMENT_IPv4	4
#define DEF_LENGTH_ELEM_IPv4	4

//////////////////////////////////////////////////////////////////////////////
#define SUPER	McPacket
McPacket_IPv4* McPacket_IPv4::instance_=0;
McTopHdr_IPv4* McPacket_IPv4::tophdr_=0;
McPacket_IPv4::McPacket_IPv4(CSTR s):SUPER(s) {}
McPacket_IPv4::~McPacket_IPv4() {}

// COMPOSE/REVERSE
uint32_t McPacket_IPv4::length_for_reverse(
		RControl& c,ItPosition& at,OCTBUF& buf) const{
	uint32_t length = tophdr_->Layerlength_for_reverse(c,at,buf);
	return length;}

RObject* McPacket_IPv4::reverseRc(RControl& c,
		RObject* r_parent,const ItPosition& at,PvObject* pv)const{
	RcPacket_IP* rc = new RcPacket_IP(r_parent,this,at,pv);
	c.set_IPinfo(rc->IPinfo());
	return rc;}

WObject* McPacket_IPv4::composeWc(WControl& c,
		WObject* w_parent,const PObject* pc) const {
	WcPacket_IP* wc = new WcPacket_IP(w_parent,this,pc);
	c.set_IPinfo(wc->IPinfo());
	return wc;}

RObject* McPacket_IPv4::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	Con_IPinfo* keep = c.IPinfo();
	RcPacket_IP* rc = (RcPacket_IP*)SUPER::reverse(c,r_parent,at,buf);
	if(!c.error()){
		Con_IPinfo* info = rc->IPinfo();
		info->reverse_postAHICV(c,rc);}
	c.set_IPinfo(keep);
	return rc;}

WObject* McPacket_IPv4::compose(WControl& c,
		WObject* w_parent,const PObject* pc) const{
	Con_IPinfo* keep = c.IPinfo();
	WObject* rtn = SUPER::compose(c,w_parent,pc);
	c.set_IPinfo(keep);
	return rtn;}

bool McPacket_IPv4::generate(WControl& c,WObject* w_self,OCTBUF& buf) const {
	Con_IPinfo* keep = c.IPinfo();
	WcPacket_IP* wc = (WcPacket_IP*)w_self;
	c.set_IPinfo(wc->IPinfo());
	bool rtn = SUPER::generate(c,w_self,buf);
	if(!c.error()){
		Con_IPinfo* info = wc->IPinfo();
		info->generate_postAHICV(c,buf,wc);}
	c.set_IPinfo(keep);
	return rtn;}

bool McPacket_IPv4::overwrite_ICV(ICVControl&,
		const ItPosition&,OCTBUF&,const TObject*)const{
	return true;}//useless overwrite tunnelPacket

#undef SUPER

//----------------------------------------------------------------------------
#define SUPER	McHeader
McTopHdr_IPv4::McTopHdr_IPv4(CSTR key):SUPER(key),
	length_(0),Layerlength_(0),Flags_(0),fragoffset_(0){}
McTopHdr_IPv4::~McTopHdr_IPv4(){}
uint32_t McTopHdr_IPv4::alignment_requirement() const{
	McHeader::provisional_AH_alignment_v4_v6_=DEF_ALIGNMENT_IPv4;
	return DEF_ALIGNMENT_IPv4;}

// COMPOSE/REVERSE
uint32_t McTopHdr_IPv4::length_for_reverse(
		RControl& c,ItPosition& at,OCTBUF& buf) const{
	if(!length_)return SUPER::length_for_reverse(c,at,buf);
	uint32_t valulen	= length_->value(at,buf);
	uint32_t length		= valulen*DEF_LENGTH_ELEM_IPv4;
	return length;}

uint32_t McTopHdr_IPv4::Layerlength_for_reverse(
			RControl& c,ItPosition& at,OCTBUF& buf) const {
	if(!Layerlength_)return SUPER::Layerlength_for_reverse(c,at,buf);
	uint32_t valulen	= Layerlength_->value(at,buf);
	uint32_t Layerlength	= valulen;
	return Layerlength;}


void McTopHdr_IPv4::nextType_set_Control(RControl& c,RObject* r_self)const{
	//if it is FragmentPacket then nextType overwrite other(McPayload)
	if(!Flags_||!fragoffset_)return;
	const TObject* tflags = r_self->corresponding(Flags_);
	const PvNumber* pvflags =  (const PvNumber*)tflags->pvalue();
	uint32_t flags = pvflags->value();
	const TObject* toffset = r_self->corresponding(fragoffset_);
	const PvNumber* pvoffset =  (const PvNumber*)toffset->pvalue();
	uint32_t offset = pvoffset->value();
	bool isMF = !!(flags&0x00000001); //MF is LSB on Flag(3bitfield)
	if(isMF || offset!=0){//Fragment Packet
		c.DictType().other_Set();}
	}

RObject* McTopHdr_IPv4::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	RObject* r_self = SUPER::reverse(c,r_parent,at,buf);
	if(!c.error()){
		Con_IPinfo* info = c.IPinfo();
		if(info)info->reverse_postIPChecksum(c,r_self);
		nextType_set_Control(c,r_self);}
	return r_self;}

bool McTopHdr_IPv4::generate(WControl& c,WObject* w_self,OCTBUF& buf) const {
	bool rtn = SUPER::generate(c,w_self,buf);
	if(!c.error()){
		Con_IPinfo* info = c.IPinfo();
		if(info)info->generate_postIPChecksum(c,buf,w_self);}
	return rtn;}

bool McTopHdr_IPv4::HCGENE(IHL)(WControl& cntr,WObject* wmem,OCTBUF& buf) const{
	WObject* wc		= wmem->parent();	//TopHdr_IPv4
	uint32_t reallen	= wc->size().bytes();
	uint32_t valulen	= reallen/DEF_LENGTH_ELEM_IPv4;
	PvNumber def(valulen);
	return def.generate(cntr,wmem,buf);}

bool McTopHdr_IPv4::HCGENE(TotalLength)(
		WControl& cntr,WObject* wmem,OCTBUF& buf) const{
	WObject* wc		= wmem->parent();	//TopHdr_IPv4
	WObject* packet		= wc->parent();		//Packet_IPv4
	uint32_t reallen	= packet->size().bytes();
	uint32_t valulen	= reallen;
	PvNumber def(valulen);
	return def.generate(cntr,wmem,buf);}

bool McTopHdr_IPv4::HCGENE(Protocol)(WControl& cntr,WObject* wmem,OCTBUF& buf) const{
	int32_t val		= get_next_headerType(wmem);
	if(val==-1)return false;
	PvNumber def(val);
	return def.generate(cntr,wmem,buf);}
PObject* McTopHdr_IPv4::HCEVAL(Protocol)(WObject* wmem) const{
	int32_t val		= get_next_headerType(wmem);
	return new PvNumber(val);}

void McTopHdr_IPv4::HC_ForIPinfo(SourceAddress)(
		Con_IPinfo& info,TObject* tmem) const {
	const PvObject* pv = tmem->pvalue();
	info.SrcAddr(pv);}
void McTopHdr_IPv4::HC_ForIPinfo(DestinationAddress)(
		Con_IPinfo& info,TObject* tmem) const {
	const PvObject* pv = tmem->pvalue();
	info.DstAddr(pv);}

bool McTopHdr_IPv4::overwrite_ICV(ICVControl& c,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const{
	return t->overwrite_ICV_child(c,at,buf);}

bool McTopHdr_IPv4::HC_OWICV(DestinationAddress)(ICVControl& c,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const{
	const OCTBUF* last = (const OCTBUF*)c.LastDstAddr();
	if(last)buf.encode(at,*last);
	const PvObject* shift = t->pvalue();
	c.ShiftRoutAddr(shift);
	return true;}

#undef SUPER

///////////////////////////////////////////////////////////////////////////////
#define SUPER	McOption
McOpt_IPv4::McOpt_IPv4(CSTR key,bool isZeroICV):SUPER(key),
	type_(0),length_(0),isZeroICV_(isZeroICV){}
McOpt_IPv4::~McOpt_IPv4(){}

// COMPOSE/REVERSE
uint32_t McOpt_IPv4::length_for_reverse(RControl& c,
		ItPosition& at,OCTBUF& buf) const{
	if(!length_)return SUPER::length_for_reverse(c,at,buf);
	uint32_t valulen	= length_->value(at,buf);
	uint32_t length		= valulen;
	return length;}

bool McOpt_IPv4::HCGENE(Length)(WControl& cntr,WObject* wmem,OCTBUF& buf)const{
	WObject* wc		= wmem->parent();	//Opt_IPv4
	uint32_t reallen	= wc->size().bytes();
	uint32_t valulen	= reallen;
	PvNumber def(valulen);
	return def.generate(cntr,wmem,buf);}

bool McOpt_IPv4::overwrite_DictType(
		RControl& c,ItPosition& at,OCTBUF& buf)const{
	if(c.DictType().finish())return false;	//End of OptionList
	uint32_t limit = buf.remainLength(at.bytes());
	if(limit==0)return false;		//End of IPv4 Header
	//
	ItPosition tmpat=at;
	RObject* rtype = type_->reverse(c,0,tmpat,buf);
	if(!rtype)return false;			//Type field decode error
	//
	const PvNumber* pv = (const PvNumber*)rtype->pvalue();
	uint32_t typevalue = pv->value();
	c.DictType().type_Set(typevalue);	//self Type set
	delete rtype;
	return true;}

bool McOpt_IPv4::overwrite_ICV(ICVControl& c,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const{
	bool rtn = true;
	if(isZeroICV_){
		ICV_Zero ow;
		rtn = ow.overwrite(c,at,buf,t);}
	return rtn;}
#undef SUPER

///////////////////////////////////////////////////////////////////////////////
#define SUPER	McOpt_IPv4

//----------------------------------------------------------------------------
McOpt_IPv4_ANY::McOpt_IPv4_ANY(CSTR key,bool isZeroICV):
	SUPER(key,isZeroICV){}
McOpt_IPv4_ANY::~McOpt_IPv4_ANY(){}

//----------------------------------------------------------------------------
McOpt_IPv4_EndofOptionList::McOpt_IPv4_EndofOptionList(CSTR key,bool isZeroICV):
	SUPER(key,isZeroICV){}
McOpt_IPv4_EndofOptionList::~McOpt_IPv4_EndofOptionList(){}

RObject* McOpt_IPv4_EndofOptionList::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	RObject* r_self = SUPER::reverse(c,r_parent,at,buf);
	c.DictType().finish_Set(); // nextType must finish
	return r_self;}

//----------------------------------------------------------------------------
McOpt_IPv4_NoOperation::McOpt_IPv4_NoOperation(CSTR key,bool isZeroICV):
	SUPER(key,isZeroICV){}
McOpt_IPv4_NoOperation::~McOpt_IPv4_NoOperation(){}

//----------------------------------------------------------------------------
#define DEF_ALIGNMENT_SourceRoute	4
McOpt_IPv4_SourceRouteCom::McOpt_IPv4_SourceRouteCom(
	CSTR key,bool isZeroICV):SUPER(key,isZeroICV){}
McOpt_IPv4_SourceRouteCom::~McOpt_IPv4_SourceRouteCom(){}
uint32_t McOpt_IPv4_SourceRouteCom::alignment_requirement() const{
	return DEF_ALIGNMENT_SourceRoute;}

uint32_t McOpt_IPv4_SourceRouteCom::HC_MLC(RouteData)(
		const ItPosition& at,OCTBUF& buf)const{
	uint32_t length	= length_->value(at,buf);
	return (length-3)/DEF_ALIGNMENT_SourceRoute;}

void McOpt_IPv4_SourceRouteCom::HC_ForIPinfo(Pointer)(
		Con_IPinfo& info,TObject* tmem) const {
	TObject* tropt	= tmem->parent();
	uint32_t length = tropt->size().bytes();
	const PvNumber* pv = (const PvNumber*)tmem->pvalue();
	uint32_t pointer = pv->value();
	bool isLeft	= pointer<=length;
	info.Route_isLeft(isLeft);}
void McOpt_IPv4_SourceRouteCom::HC_ForIPinfo(RouteData)(
		Con_IPinfo& info,TObject* tmem) const {
	const PvObject* pv = tmem->pvalue();
	if(info.Route_isLeft()){
		info.LastDstAddr(pv);}
	}


//----------------------------------------------------------------------------
#define DEF_ALIGNMENT_RecordRoute	4
McOpt_IPv4_RecordRoute::McOpt_IPv4_RecordRoute(CSTR key,bool isZeroICV):
	SUPER(key,isZeroICV){}
McOpt_IPv4_RecordRoute::~McOpt_IPv4_RecordRoute(){}
uint32_t McOpt_IPv4_RecordRoute::alignment_requirement() const{
	return DEF_ALIGNMENT_RecordRoute;}

uint32_t McOpt_IPv4_RecordRoute::HC_MLC(RouteData)(
		const ItPosition& at,OCTBUF& buf)const{
	uint32_t length	= length_->value(at,buf);
	return (length-3)/DEF_ALIGNMENT_RecordRoute;}

//----------------------------------------------------------------------------
#define DEF_ALIGNMENT_Timestamp	4
McOpt_IPv4_Timestamp::McOpt_IPv4_Timestamp(CSTR key,bool isZeroICV):
	SUPER(key,isZeroICV){}
McOpt_IPv4_Timestamp::~McOpt_IPv4_Timestamp(){}
uint32_t McOpt_IPv4_Timestamp::alignment_requirement() const{
	return DEF_ALIGNMENT_Timestamp;}

uint32_t McOpt_IPv4_Timestamp::HC_MLC(Timestamp)(
		const ItPosition& at,OCTBUF& buf)const{
	uint32_t length	= length_->value(at,buf);
	return (length-4)/DEF_ALIGNMENT_Timestamp;}
//----------------------------------------------------------------------------
McOpt_IPv4_RouterAlert::McOpt_IPv4_RouterAlert(CSTR key,bool isZeroICV):
	SUPER(key,isZeroICV){}
McOpt_IPv4_RouterAlert::~McOpt_IPv4_RouterAlert(){}


///////////////////////////////////////////////////////////////////////////////
// create options
#include "PvAutoItem.h"
#include "PvAction.h"

#define OPT_ICVCONST()	false
#define OPT_ICVZERO()	true

#define UN(n)		PvNumber::unique(n)
#define MUST()		PvMUSTDEF::must()
#define EVALANY()	new PvANY()
#define GENEHC(mc,cls,mem)	new PvHCgene(mc,(HCgenefunc)&cls::HCGENE(mem))
#define EVALHC(mc,cls,mem)	new PvHCeval(mc,(HCevalfunc)&cls::HCEVAL(mem))

#define LEXADD(classname,lexname) addCompound(classname::create(lexname))
void McOpt_IPv4::create_options(){
	LEXADD(McOpt_IPv4_ANY,"Opt_IPv4_ANY" );
	LEXADD(McOpt_IPv4_EndofOptionList,	"Opt_IPv4_EndofOptionList" );
	LEXADD(McOpt_IPv4_NoOperation,		"Opt_IPv4_NoOperation" );
	LEXADD(McOpt_IPv4_LooseSourceRoute,	"Opt_IPv4_LooseSourceRoute" );
	LEXADD(McOpt_IPv4_StrictSourceRoute,	"Opt_IPv4_StrictSourceRoute" );
	LEXADD(McOpt_IPv4_RecordRoute,		"Opt_IPv4_RecordRoute" );
	LEXADD(McOpt_IPv4_Timestamp,		"Opt_IPv4_Timestamp" );
	LEXADD(McOpt_IPv4_RouterAlert,		"Opt_IPv4_RouterAlert" );
	}

void McOpt_IPv4::common_member(){
        int32_t Type = optionType();
        type_member(
                new MmUint( "Type",	8,	UN(Type),UN(Type) ) );
        length_member(
                new MmUint( "Length",	8,
			GENEHC(this,McOpt_IPv4,Length), EVALANY() ) );
        }

McOpt_IPv4_ANY* McOpt_IPv4_ANY::create(CSTR key){
	McOpt_IPv4_ANY* mc = new McOpt_IPv4_ANY(key,	OPT_ICVZERO() );
	mc->common_member();
	mc->member( new MmData( "data" ) );
	// dict
	MmOption_onIPv4::add_other(mc);		//Hdr_IPv4::option=
	return mc;}
#if 0
ANY(Unknown Type)
        +--------+--------+--------//--------+
        |Type    | length | data             |
        +--------+--------+--------//--------+
#endif

McOpt_IPv4_EndofOptionList* McOpt_IPv4_EndofOptionList::create(CSTR key){
	McOpt_IPv4_EndofOptionList* mc =
		new McOpt_IPv4_EndofOptionList(key,	OPT_ICVCONST() );
        int32_t Type = mc->optionType();
        mc->type_member(
                new MmUint( "Type",	8,	UN(Type),UN(Type) ) );
	// dict
	MmOption_onIPv4::add(mc);		//Hdr_IPv4::option=
	return mc;}
#if 0
End of Option List
        +--------+
        |00000000|
        +--------+
#endif

McOpt_IPv4_NoOperation* McOpt_IPv4_NoOperation::create(CSTR key){
	McOpt_IPv4_NoOperation* mc =
		new McOpt_IPv4_NoOperation(key,	OPT_ICVCONST() );
        int32_t Type = mc->optionType();
        mc->type_member(
                new MmUint( "Type",	8,	UN(Type),UN(Type) ) );
	// dict
	MmOption_onIPv4::add(mc);		//Hdr_IPv4::option=
	return mc;}
#if 0
No Operation
        +--------+
        |00000001|
        +--------+
#endif

template<int32_t TYPE>
McOpt_IPv4_SourceRoute<TYPE>* McOpt_IPv4_SourceRoute<TYPE>::create(CSTR key){
	McOpt_IPv4_SourceRoute<TYPE>* mc =
		new McOpt_IPv4_SourceRoute<TYPE>(key,	OPT_ICVZERO() );
	mc->common_member();
	mc->member( new MmUint( "Pointer",	8,
				UN(4), EVALANY(),0,
				(METH_HC_ForIPinfo)&McOpt_IPv4_SourceRoute<TYPE>::HC_ForIPinfo(Pointer) ) );
	mc->member( new MmMultiple(
			new MmV4Addr( "RouteData",
				MUST(),MUST(),0,
				(METH_HC_ForIPinfo)&McOpt_IPv4_SourceRoute<TYPE>::HC_ForIPinfo(RouteData) ),
			(METH_HC_MLC)&McOpt_IPv4_SourceRoute<TYPE>::HC_MLC(RouteData) ) );
	// dict
	MmOption_onIPv4::add(mc);		//Hdr_IPv4::option=
	return mc;}
#if 0
Loose Source and Record Route
        +--------+--------+--------+---------//--------+
        |10000011| length | pointer|     route data    |
        +--------+--------+--------+---------//--------+
#endif

McOpt_IPv4_RecordRoute* McOpt_IPv4_RecordRoute::create(CSTR key){
	McOpt_IPv4_RecordRoute* mc =
		new McOpt_IPv4_RecordRoute(key,	OPT_ICVZERO() );
	mc->common_member();
	mc->member( new MmUint( "Pointer",	8,	UN(4), EVALANY() ) );
	mc->member( new MmMultiple(
			new MmV4Addr( "RouteData",
				MUST(),EVALANY() ),
			(METH_HC_MLC)&McOpt_IPv4_RecordRoute::HC_MLC(RouteData) ) );
	// dict
	MmOption_onIPv4::add(mc);	       //Hdr_IPv4::option=
	return mc;}
#if 0
Record Route
        +--------+--------+--------+---------//--------+
        |00000111| length | pointer|     route data    |
        +--------+--------+--------+---------//--------+
#endif

McOpt_IPv4_Timestamp* McOpt_IPv4_Timestamp::create(CSTR key){
	McOpt_IPv4_Timestamp* mc =
		new McOpt_IPv4_Timestamp(key,	OPT_ICVZERO() );
	mc->common_member();
	mc->member( new MmUint( "Pointer",	8,	UN(5), EVALANY() ) );
	mc->member( new MmUint( "Overflow",	4,	UN(0), EVALANY() ) );
	mc->member( new MmUint( "Flag",		4,	UN(0), EVALANY() ) );
	mc->member( new MmMultiple(
			new MmUint( "Timestamp",32,	UN(0), EVALANY() ),
			(METH_HC_MLC)&McOpt_IPv4_Timestamp::HC_MLC(Timestamp) ) );
	// dict
	MmOption_onIPv4::add(mc);		//Hdr_IPv4::option=
	return mc;}
// Timestamp is ( V4Address or Uint32 )
// if there is an Address field ,can not describe  Timestamp = v4("..");

#if 0
Internet Timestamp
        +--------+--------+--------+--------+
        |01000100| length | pointer|oflw|flg|
        +--------+--------+--------+--------+
        |         internet address          |
        +--------+--------+--------+--------+
        |             timestamp             |
        +--------+--------+--------+--------+
        |                 .                 |
                          .
                          .
#endif

McOpt_IPv4_RouterAlert* McOpt_IPv4_RouterAlert::create(CSTR key){
	McOpt_IPv4_RouterAlert* mc =
		new McOpt_IPv4_RouterAlert(key,	OPT_ICVCONST() );
	mc->common_member();
	mc->member( new MmUint( "Value",	16,	UN(0), UN(0) ) );
	// dict
	MmOption_onIPv4::add(mc);		//Hdr_IPv4::option=
	return mc;}
#if 0
http://www.ietf.org/rfc/rfc2113.txt
Router Alert
        +--------+--------+--------+--------+
        |10010100|00000100|  2 octet value  |
        +--------+--------+--------+--------+
#endif

///////////////////////////////////////////////////////////////////////////////
MmOption_onIPv4::MmOption_onIPv4(CSTR key):MmReference_More0(key,true) {}
MmOption_onIPv4::~MmOption_onIPv4() {}

void MmOption_onIPv4::add(McOpt_IPv4* mc){
	dict_.add(mc->optionType(),mc);}
void MmOption_onIPv4::add_other(McOpt_IPv4* mc){dict_.add_other(mc);}
TypevsMcDict MmOption_onIPv4::dict_;

// REVERSE
bool MmOption_onIPv4::overwrite_DictType(
		RControl& c,ItPosition& at,OCTBUF& buf)const{
	McOpt_IPv4* any = (McOpt_IPv4*)dict_.find_other();
	return any->overwrite_DictType(c,at,buf);}

///////////////////////////////////////////////////////////////////////////////

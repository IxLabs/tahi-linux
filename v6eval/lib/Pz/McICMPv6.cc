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
 * $TAHI: v6eval/lib/Pz/McICMPv6.cc,v 1.17 2009/11/25 07:22:17 akisada Exp $
 */
#include "McICMPv6.h"
#include "ItPosition.h"
#include "WObject.h"
#include "RObject.h"
#include "PControl.h"
#include "PvObject.h"
#include "PvOctets.h"
#include "McMLDv2.h"

//////////////////////////////////////////////////////////////////////////////
#define SUPER	McUpper
McUpp_ICMPv6_ONE* McUpp_ICMPv6_ONE::instance_ = 0;
McUpp_ICMPv6_ONE* McUpp_ICMPv6_ONE::instance(){
	if(!instance_){instance_= new McUpp_ICMPv6_ONE("ICMPv6");}
	return instance_;}

McUpp_ICMPv6_ONE::McUpp_ICMPv6_ONE(CSTR key):SUPER(key){
        member( new MmHeader_onICMPv6( "header" ) );
        //dict
        MmUpper_onIP::add(this);	//Packet_IP::upper=
	}

McUpp_ICMPv6_ONE::~McUpp_ICMPv6_ONE(){}

// COMPOSE/REVERSE
bool McUpp_ICMPv6_ONE::containsMc(const MObject* mc) const{
	bool rtn = McUpper::containsMc(mc);
	return rtn ? rtn : members_[0]->containsMc(mc);}

uint32_t McUpp_ICMPv6_ONE::length_for_reverse(
		RControl&,ItPosition& at,OCTBUF& buf) const{
	return buf.remainLength(at.bytes());}

RObject* McUpp_ICMPv6_ONE::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	return members_[0]->reverse(c,r_parent,at,buf);}//forward MmHdr_ICMPv6
#undef SUPER

//////////////////////////////////////////////////////////////////////////////
#define SUPER	McHeader

McUpp_ICMPv6::McUpp_ICMPv6(CSTR key):SUPER(key),type_(0){
	McUpp_ICMPv6_ONE::instance();}//wake up 
McUpp_ICMPv6::~McUpp_ICMPv6(){}
int32_t McUpp_ICMPv6::token()const{return metaToken(tkn_upper_);}

// COMPOSE/REVERSE
uint32_t McUpp_ICMPv6::length_for_reverse(
		RControl&,ItPosition& at,OCTBUF& buf) const{
	return buf.remainLength(at.bytes());}

bool McUpp_ICMPv6::overwrite_DictType(
		RControl& c,ItPosition& at,OCTBUF& buf)const{
	ItPosition tmpat=at;
	RObject* rtype = type_->reverse(c,0,tmpat,buf);
	if(!rtype)return false;			//Type field decode error
	//
	const PvNumber* pv = (const PvNumber*)rtype->pvalue();
	uint32_t typevalue = pv->value();
#ifndef NOT_USE_MLDV2_QUERY
	if (typevalue == (uint32_t)TP_ICMPv6_MLDQuery) { // MLD Query
		uint32_t limit = buf.remainLength(at.bytes());
		if (limit >= 28) { // MLDv2
			typevalue = TP_ICMPv6_MLDv2Query;
		}
	}
#endif	// NOT_USE_MLDV2_QUERY
	c.DictType().type_Set(typevalue);	//self Type set
	delete rtype;
	return true;}

RObject* McUpp_ICMPv6::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	RObject* r_self = SUPER::reverse(c,r_parent,at,buf);
	if(!c.error()){
		Con_IPinfo* info = c.IPinfo();
		if(info)info->reverse_postUppChecksum(c,r_self);}
	return r_self;}

bool McUpp_ICMPv6::generate(WControl& c,WObject* w_self,OCTBUF& buf) const {
	bool rtn = SUPER::generate(c,w_self,buf);
	if(!c.error()){
		Con_IPinfo* info = c.IPinfo();
		if(info)info->generate_postUppChecksum(c,buf,w_self);}
	return rtn;}
#undef SUPER

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_ANY::McUpp_ICMPv6_ANY(CSTR key):McUpp_ICMPv6(key){}
McUpp_ICMPv6_ANY::~McUpp_ICMPv6_ANY(){}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_RS::McUpp_ICMPv6_RS(CSTR key):McUpp_ICMPv6(key){}
McUpp_ICMPv6_RS::~McUpp_ICMPv6_RS(){}

McUpp_ICMPv6_RA::McUpp_ICMPv6_RA(CSTR key):McUpp_ICMPv6(key){}
McUpp_ICMPv6_RA::~McUpp_ICMPv6_RA(){}

McUpp_ICMPv6_NS::McUpp_ICMPv6_NS(CSTR key):McUpp_ICMPv6(key){}
McUpp_ICMPv6_NS::~McUpp_ICMPv6_NS(){}

McUpp_ICMPv6_NA::McUpp_ICMPv6_NA(CSTR key):McUpp_ICMPv6(key){}
McUpp_ICMPv6_NA::~McUpp_ICMPv6_NA(){}

McUpp_ICMPv6_Redirect::McUpp_ICMPv6_Redirect(CSTR key):McUpp_ICMPv6(key){}
McUpp_ICMPv6_Redirect::~McUpp_ICMPv6_Redirect(){}

McUpp_ICMPv6_PacketTooBig::McUpp_ICMPv6_PacketTooBig(CSTR key):McUpp_ICMPv6(key){}
McUpp_ICMPv6_PacketTooBig::~McUpp_ICMPv6_PacketTooBig(){}

McUpp_ICMPv6_DestinationUnreachable::McUpp_ICMPv6_DestinationUnreachable(CSTR key):McUpp_ICMPv6(key){}
McUpp_ICMPv6_DestinationUnreachable::~McUpp_ICMPv6_DestinationUnreachable(){}

McUpp_ICMPv6_TimeExceeded::McUpp_ICMPv6_TimeExceeded(CSTR key):McUpp_ICMPv6(key){}
McUpp_ICMPv6_TimeExceeded::~McUpp_ICMPv6_TimeExceeded(){}

McUpp_ICMPv6_ParameterProblem::McUpp_ICMPv6_ParameterProblem(CSTR key):McUpp_ICMPv6(key){}
McUpp_ICMPv6_ParameterProblem::~McUpp_ICMPv6_ParameterProblem(){}

McUpp_ICMPv6_HAADRequest::McUpp_ICMPv6_HAADRequest(CSTR key):McUpp_ICMPv6(key) {}
McUpp_ICMPv6_HAADRequest::~McUpp_ICMPv6_HAADRequest() {}

////////////////////////////////////////////////////////////////////////////////
#define HAADREPOFFSET	8
#define HAADDRLEN	16

McUpp_ICMPv6_HAADReply::McUpp_ICMPv6_HAADReply(CSTR key):McUpp_ICMPv6(key) {}
McUpp_ICMPv6_HAADReply::~McUpp_ICMPv6_HAADReply() {}

uint32_t McUpp_ICMPv6_HAADReply::HC_MLC(Address)(const ItPosition &at, OCTBUF &buf) const {
	uint32_t length = (buf.remainLength(at.bytes()) - HAADREPOFFSET) / HAADDRLEN;
	return(length);
} 
#undef HAADDRLEN
#undef HAADREPOFFSET

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_MobilePrefixSolicit::McUpp_ICMPv6_MobilePrefixSolicit(CSTR key):McUpp_ICMPv6(key) {}
McUpp_ICMPv6_MobilePrefixSolicit::~McUpp_ICMPv6_MobilePrefixSolicit() {}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_MobilePrefixAdvertise::McUpp_ICMPv6_MobilePrefixAdvertise(CSTR key):McUpp_ICMPv6(key) {}
McUpp_ICMPv6_MobilePrefixAdvertise::~McUpp_ICMPv6_MobilePrefixAdvertise() {}

//////////////////////////////////////////////////////////////////////////////
// particular internal class (to forward headerType(ICMPv6)->icmpv6Type)
MmHeader_onICMPv6::MmHeader_onICMPv6(CSTR key):MmReference_Must1(key,true){}
MmHeader_onICMPv6::~MmHeader_onICMPv6(){}

void MmHeader_onICMPv6::add(McUpp_ICMPv6* mc){
	dict_.add(mc->icmpv6Type(),mc);}
void MmHeader_onICMPv6::add_other(McUpp_ICMPv6* mc){dict_.add_other(mc);}
TypevsMcDict MmHeader_onICMPv6::dict_;

// REVERSE
bool MmHeader_onICMPv6::overwrite_DictType(
		RControl& c,ItPosition& at,OCTBUF& buf)const{
	McUpp_ICMPv6* any = (McUpp_ICMPv6*)dict_.find_other();
	return any->overwrite_DictType(c,at,buf);}


//////////////////////////////////////////////////////////////////////////////
#define DEF_ALIGNMENT_OptICMPv6		8
#define DEF_LENGTH_ELEM_OptICMPv6	8

McOpt_ICMPv6::McOpt_ICMPv6(CSTR key):McOption(key),type_(0),length_(0){}
McOpt_ICMPv6::~McOpt_ICMPv6(){}

// COMPOSE/REVERSE
uint32_t McOpt_ICMPv6::length_for_reverse(
		RControl& c,ItPosition& at,OCTBUF& buf) const{
	if(!length_)return McOption::length_for_reverse(c,at,buf);
	uint32_t valulen	= length_->value(at,buf);
	uint32_t length		= valulen*DEF_LENGTH_ELEM_OptICMPv6;
	return length;}

bool McOpt_ICMPv6::overwrite_DictType(
		RControl& c,ItPosition& at,OCTBUF& buf)const{
	uint32_t limit = buf.remainLength(at.bytes());
	if(limit==0)return false;		//End of ICMPv6 Header
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

bool McOpt_ICMPv6::HCGENE(Length)(
		WControl& cntr,WObject* wmem,OCTBUF& buf) const{
	WObject* wc = wmem->parent();
	uint32_t reallen	= wc->size().bytes();
	uint32_t valulen	= reallen/DEF_LENGTH_ELEM_OptICMPv6;
	PvNumber def(valulen);
	return def.generate(cntr,wmem,buf);}

//////////////////////////////////////////////////////////////////////////////
McOpt_ICMPv6_ANY::McOpt_ICMPv6_ANY(CSTR key):McOpt_ICMPv6(key){}
McOpt_ICMPv6_ANY::~McOpt_ICMPv6_ANY(){}

//////////////////////////////////////////////////////////////////////////////
McOpt_ICMPv6_SLL::McOpt_ICMPv6_SLL(CSTR key):McOpt_ICMPv6(key){}
McOpt_ICMPv6_SLL::~McOpt_ICMPv6_SLL(){}

McOpt_ICMPv6_TLL::McOpt_ICMPv6_TLL(CSTR key):McOpt_ICMPv6(key){}
McOpt_ICMPv6_TLL::~McOpt_ICMPv6_TLL(){}

McOpt_ICMPv6_Prefix::McOpt_ICMPv6_Prefix(CSTR key):McOpt_ICMPv6(key){}
McOpt_ICMPv6_Prefix::~McOpt_ICMPv6_Prefix(){}

McOpt_ICMPv6_MTU::McOpt_ICMPv6_MTU(CSTR key):McOpt_ICMPv6(key){}
McOpt_ICMPv6_MTU::~McOpt_ICMPv6_MTU(){}

McOpt_ICMPv6_Redirected::McOpt_ICMPv6_Redirected(CSTR key):McOpt_ICMPv6(key){}
McOpt_ICMPv6_Redirected::~McOpt_ICMPv6_Redirected(){}

McOpt_ICMPv6_AdvInterval::McOpt_ICMPv6_AdvInterval(CSTR key):McOpt_ICMPv6(key) {}
McOpt_ICMPv6_AdvInterval::~McOpt_ICMPv6_AdvInterval() {}

McOpt_ICMPv6_HomeAgentInfo::McOpt_ICMPv6_HomeAgentInfo(CSTR key):McOpt_ICMPv6(key) {}
McOpt_ICMPv6_HomeAgentInfo::~McOpt_ICMPv6_HomeAgentInfo() {}

McOpt_ICMPv6_RouteInfo::McOpt_ICMPv6_RouteInfo(CSTR key): McOpt_ICMPv6(key) {}
McOpt_ICMPv6_RouteInfo::~McOpt_ICMPv6_RouteInfo() {}

//////////////////////////////////////////////////////////////////////////////
MmOption_onICMPv6::MmOption_onICMPv6(CSTR key):MmReference_More0(key,true) {}
MmOption_onICMPv6::~MmOption_onICMPv6() {}

void MmOption_onICMPv6::add(McOpt_ICMPv6* mc){
	dict_.add(mc->optionType(),mc);}
void MmOption_onICMPv6::add_other(McOpt_ICMPv6* mc){dict_.add_other(mc);}
TypevsMcDict MmOption_onICMPv6::dict_;

// REVERSE
bool MmOption_onICMPv6::overwrite_DictType(
		RControl& c,ItPosition& at,OCTBUF& buf)const{
	McOpt_ICMPv6* any = (McOpt_ICMPv6*)dict_.find_other();
	return any->overwrite_DictType(c,at,buf);}

// ICMPv6 MLDv2 Report
////////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_MLDv2Report::McUpp_ICMPv6_MLDv2Report(CSTR key):McUpp_ICMPv6(key) {}
McUpp_ICMPv6_MLDv2Report::~McUpp_ICMPv6_MLDv2Report() {}
bool McUpp_ICMPv6_MLDv2Report::HCGENE(NumOfMcastAddrRecords)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	WObject *wc = wmem->parent();
	CmQueue* n=0;
	uint32_t valulen = 0;
	/* counting childs */
	for(n=wc->nextChild();n!=0;n=wc->nextChild(n))
		valulen++;
	PvNumber def(valulen - 5); // subtract (Type, Code, Checksum, Reserved, NumOfMcastAddrRecords)
	return(def.generate(cntr, wmem, buf));
}
// MLDv2 Multicast Address Record
//////////////////////////////////////////////////////////////////////////////
McMLDv2_AddrRecord::McMLDv2_AddrRecord(CSTR key): McUpper(key), type_(0), auxdatalen_(0), numofsrc_(0)
{ MmMLDv2AddrRecord_onICMPv6::add_other(this);}
McMLDv2_AddrRecord::~McMLDv2_AddrRecord() {}

// return source address record count
uint32_t McMLDv2_AddrRecord::HC_MLC(SourceAddress)(const ItPosition &at, OCTBUF &buf) const {
	if (numofsrc_)
		return(numofsrc_->value(at, buf));
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
MmMLDv2AddrRecord_onICMPv6::MmMLDv2AddrRecord_onICMPv6(CSTR key):MmReference_More0(key,true) {}
MmMLDv2AddrRecord_onICMPv6::~MmMLDv2AddrRecord_onICMPv6() {}

void MmMLDv2AddrRecord_onICMPv6::add(McMLDv2_AddrRecord* mc){
	dict_.add(mc->optionType(),mc);}
void MmMLDv2AddrRecord_onICMPv6::add_other(McMLDv2_AddrRecord* mc){dict_.add_other(mc);}
TypevsMcDict MmMLDv2AddrRecord_onICMPv6::dict_;

// REVERSE
bool MmMLDv2AddrRecord_onICMPv6::overwrite_DictType(
		RControl& c,ItPosition& at,OCTBUF& buf)const{
	McOpt_ICMPv6* any = (McOpt_ICMPv6*)dict_.find_other();
	return any->overwrite_DictType(c,at,buf);}

// ICMPv6 MLD/MLDv2 Query
////////////////////////////////////////////////////////////////////////////////
#define MLDV2OFFSET	28 // (Type Code Checksum MaxResponseCode Resv McastAddr ... NumOfSources ) = 28 Byte
#define MLDV2ADDRLEN	16 //address record length

// MLDv1
McUpp_ICMPv6_MLDQuery::McUpp_ICMPv6_MLDQuery(CSTR key):McUpp_ICMPv6(key) {}
McUpp_ICMPv6_MLDQuery::~McUpp_ICMPv6_MLDQuery() {}

#ifdef NOT_USE_MLDV2_QUERY
bool McUpp_ICMPv6_MLDQuery::HCGENE(NumOfSources)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	WObject *wc = wmem->parent();
	uint32_t reallen = wc->size().bytes();
	uint32_t valulen = (reallen - MLDV2OFFSET)/MLDV2ADDRLEN;
	PvNumber def(valulen);

	return(def.generate(cntr, wmem, buf));
}
uint32_t McUpp_ICMPv6_MLDQuery::HC_MLC(SourceAddress)(const ItPosition &at, OCTBUF &buf) const {
	uint32_t length = (buf.remainLength(at.bytes()) - MLDV2OFFSET) / MLDV2ADDRLEN;
	return(length);
} 
#else	// NOT_USE_MLDV2_QUERY
// MLDv2
McUpp_ICMPv6_MLDv2Query::McUpp_ICMPv6_MLDv2Query(CSTR key):McUpp_ICMPv6(key) {}
McUpp_ICMPv6_MLDv2Query::~McUpp_ICMPv6_MLDv2Query() {}

bool McUpp_ICMPv6_MLDv2Query::HCGENE(NumOfSources)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	WObject *wc = wmem->parent();
	uint32_t reallen = wc->size().bytes();
	uint32_t valulen = (reallen - MLDV2OFFSET)/MLDV2ADDRLEN;
	PvNumber def(valulen);

	return(def.generate(cntr, wmem, buf));
}

uint32_t McUpp_ICMPv6_MLDv2Query::HC_MLC(SourceAddress)(const ItPosition &at, OCTBUF &buf) const {
	uint32_t length = (buf.remainLength(at.bytes()) - MLDV2OFFSET) / MLDV2ADDRLEN;
	return(length);
} 
#endif	// NOT_USE_MLDV2_QUERY

#undef MLDV2OFFSET
#undef MLDV2ADDRLEN

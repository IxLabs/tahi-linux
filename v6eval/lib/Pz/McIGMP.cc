/*
 * Copyright (C) 2003 Yokogawa Electric Corporation,
 * INTAP(Interoperability Technology Association
 * for Information Processing, Japan). 
 * Copyright (C) IBM Corporation 2003.
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
 *    Author: Kazuo Hiekata <hiekata@yamato.ibm.com>
 */

#include "McIGMP.h"
#include "ItPosition.h"
#include "WObject.h"
#include "RObject.h"
#include "PControl.h"
#include "PvObject.h"
#include "PvOctets.h"

//////////////////////////////////////////////////////////////////////////////
#define SUPER	McUpper

McUpp_IGMP_ONE* McUpp_IGMP_ONE::instance_ = 0;
McUpp_IGMP_ONE* McUpp_IGMP_ONE::instance(){
	if(!instance_){instance_= new McUpp_IGMP_ONE("IGMP");}
	return instance_;}

McUpp_IGMP_ONE::McUpp_IGMP_ONE(CSTR key):SUPER(key){
        member( new MmHeader_onIGMP( "header" ) );
        //dict
        MmUpper_onIP::add(this);	//Packet_IP::upper=
	}

McUpp_IGMP_ONE::~McUpp_IGMP_ONE(){}

// COMPOSE/REVERSE
bool McUpp_IGMP_ONE::containsMc(const MObject* mc) const{
	bool rtn = McUpper::containsMc(mc);
	return rtn ? rtn : members_[0]->containsMc(mc);}

uint32_t McUpp_IGMP_ONE::length_for_reverse(
		RControl&,ItPosition& at,OCTBUF& buf) const{
	return buf.remainLength(at.bytes());}

RObject* McUpp_IGMP_ONE::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	return members_[0]->reverse(c,r_parent,at,buf);}//forward MmHdr_IGMP
#undef SUPER

//////////////////////////////////////////////////////////////////////////////
#define SUPER	McHeader

McUpp_IGMP::McUpp_IGMP(CSTR key):SUPER(key),type_(0){
	McUpp_IGMP_ONE::instance();}//wake up 
McUpp_IGMP::~McUpp_IGMP(){}
int32_t McUpp_IGMP::token()const{return metaToken(tkn_upper_);}

// COMPOSE/REVERSE
uint32_t McUpp_IGMP::length_for_reverse(
		RControl&,ItPosition& at,OCTBUF& buf) const{
	return buf.remainLength(at.bytes());}

bool McUpp_IGMP::overwrite_DictType(
		RControl& c,ItPosition& at,OCTBUF& buf)const{
	ItPosition tmpat=at;
	RObject* rtype = type_->reverse(c,0,tmpat,buf);
	if(!rtype)return false;                 //Type field decode error
	//
	const PvNumber* pv = (const PvNumber*)rtype->pvalue();
	uint32_t typevalue = pv->value();
	c.DictType().type_Set(typevalue);       //self Type set
	delete rtype;
	return true;}

RObject* McUpp_IGMP::reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const{
	RObject* r_self = SUPER::reverse(c,r_parent,at,buf);
	if(!c.error()){
		Con_IPinfo* info = c.IPinfo();
		if(info)info->reverse_postUppChecksum(c,r_self);}
	return r_self;}

bool McUpp_IGMP::generate(WControl& c,WObject* w_self,OCTBUF& buf) const {
	bool rtn = SUPER::generate(c,w_self,buf);
	if(!c.error()){
		Con_IPinfo* info = c.IPinfo();
		if(info)info->generate_postUppChecksum(c,buf,w_self);}
	return rtn;}
#undef SUPER

//////////////////////////////////////////////////////////////////////////////
McUpp_IGMP_ANY::McUpp_IGMP_ANY(CSTR key):McUpp_IGMP(key){}
McUpp_IGMP_ANY::~McUpp_IGMP_ANY(){}

//////////////////////////////////////////////////////////////////////////////
#define IGMPV3OFFSET	12 // (Type Code Checksum MaxResponseCode Resv McastAddr ... NumOfSources ) = 28 Byte
#define IGMPV3ADDRLEN	4 //address length

McUpp_IGMP_IGMPQuery::McUpp_IGMP_IGMPQuery(CSTR key):McUpp_IGMP(key) {}
McUpp_IGMP_IGMPQuery::~McUpp_IGMP_IGMPQuery() {}

bool McUpp_IGMP_IGMPQuery::HCGENE(NumOfSources)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	WObject *wc = wmem->parent();
	uint32_t reallen = wc->size().bytes();
	uint32_t valulen = (reallen - IGMPV3OFFSET)/IGMPV3ADDRLEN;
	PvNumber def(valulen);

	return(def.generate(cntr, wmem, buf));
}
uint32_t McUpp_IGMP_IGMPQuery::HC_MLC(SourceAddress)(const ItPosition &at, OCTBUF &buf) const {
	uint32_t length = (buf.remainLength(at.bytes()) - IGMPV3OFFSET) / IGMPV3ADDRLEN;
	return(length);
} 
#undef IGMPV3OFFSET
#undef IGMPV3ADDRLEN

//////////////////////////////////////////////////////////////////////////////
McUpp_IGMP_IGMPv3Report::McUpp_IGMP_IGMPv3Report(CSTR key):McUpp_IGMP(key) {}
McUpp_IGMP_IGMPv3Report::~McUpp_IGMP_IGMPv3Report() {}

bool McUpp_IGMP_IGMPv3Report::HCGENE(NumOfGroupAddrRecords)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	WObject *wc = wmem->parent();
	CmQueue* n=0;
	uint32_t valulen = 0;
	/* counting childs */
	for(n=wc->nextChild();n!=0;n=wc->nextChild(n))
		valulen++;
	PvNumber def(valulen - 5); // subtract (Type, Code, Checksum, Reserved, NumOfGroupAddrRecords)
	return(def.generate(cntr, wmem, buf));
}

//////////////////////////////////////////////////////////////////////////////
McUpp_IGMP_IGMPv2Report::McUpp_IGMP_IGMPv2Report(CSTR key):McUpp_IGMP(key) {}
McUpp_IGMP_IGMPv2Report::~McUpp_IGMP_IGMPv2Report() {}

//////////////////////////////////////////////////////////////////////////////
McUpp_IGMP_IGMPv2LeaveGroup::McUpp_IGMP_IGMPv2LeaveGroup(CSTR key):McUpp_IGMP(key) {}
McUpp_IGMP_IGMPv2LeaveGroup::~McUpp_IGMP_IGMPv2LeaveGroup() {}

//////////////////////////////////////////////////////////////////////////////
McUpp_IGMP_IGMPv1Report::McUpp_IGMP_IGMPv1Report(CSTR key):McUpp_IGMP(key) {}
McUpp_IGMP_IGMPv1Report::~McUpp_IGMP_IGMPv1Report() {}

//////////////////////////////////////////////////////////////////////////////
// particular internal class (to forward headerType(IGMP)->igmpType)
MmHeader_onIGMP::MmHeader_onIGMP(CSTR key):MmReference_Must1(key,true){}
MmHeader_onIGMP::~MmHeader_onIGMP(){}

void MmHeader_onIGMP::add(McUpp_IGMP* mc){
	dict_.add(mc->igmpType(),mc);}
void MmHeader_onIGMP::add_other(McUpp_IGMP* mc){dict_.add_other(mc);}
TypevsMcDict MmHeader_onIGMP::dict_;

// REVERSE
bool MmHeader_onIGMP::overwrite_DictType(
		RControl& c,ItPosition& at,OCTBUF& buf)const{
	McUpp_IGMP* any = (McUpp_IGMP*)dict_.find_other();
	return any->overwrite_DictType(c,at,buf);}

//////////////////////////////////////////////////////////////////////////////
#define IGMPV3_ADDRRECORD_OFFSET 8
#define IGMPV3_ADDRLEN	4 //address record length

McIGMPv3_AddrRecord::McIGMPv3_AddrRecord(CSTR key): McUpper(key), type_(0), auxdatalen_(0), numofsrc_(0)
{ MmIGMPv3AddrRecord_onIGMP::add_other(this);}
McIGMPv3_AddrRecord::~McIGMPv3_AddrRecord() {}

// return source address record count
uint32_t McIGMPv3_AddrRecord::HC_MLC(SourceAddress)(const ItPosition &at, OCTBUF &buf) const {
	if (numofsrc_)
		return(numofsrc_->value(at, buf));
	return 0;
}
bool McIGMPv3_AddrRecord::overwrite_DictType(
		RControl& c,ItPosition& at,OCTBUF& buf)const{
	uint32_t limit = buf.remainLength(at.bytes());
	if(limit==0)return false;		//End of IGMP Message
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

uint32_t McIGMPv3_AddrRecord::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const {
	uint32_t valulen = auxdatalen_->value(at, buf) * 4 + (numofsrc_->value(at, buf) * IGMPV3_ADDRLEN);
	uint32_t length = valulen + IGMPV3_ADDRRECORD_OFFSET;
	return(length);
}
bool McIGMPv3_AddrRecord::HCGENE(NumOfSources)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	WObject *wc = wmem->parent();
	CmQueue* n=0;
	uint32_t valulen = 0;
	/* counting childs */
	for(n=wc->nextChild();n!=0;n=wc->nextChild(n))
		valulen++;
	PvNumber def(valulen - 5); // subtract (Type AuxDataLen NumOfSources GroupAddress AuxData)
	return(def.generate(cntr, wmem, buf));
}
// REVERSE
RObject* McIGMPv3_AddrRecord::reverse(RControl& c,
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
#undef IGMPV3_ADDRRECORD_OFFSET
#undef IGMPV3_ADDRLEN
//////////////////////////////////////////////////////////////////////////////
MmIGMPv3AddrRecord_onIGMP::MmIGMPv3AddrRecord_onIGMP(CSTR key):MmReference_More0(key,true) {}
MmIGMPv3AddrRecord_onIGMP::~MmIGMPv3AddrRecord_onIGMP() {}

void MmIGMPv3AddrRecord_onIGMP::add(McIGMPv3_AddrRecord* mc){
	dict_.add(mc->optionType(),mc);}
void MmIGMPv3AddrRecord_onIGMP::add_other(McIGMPv3_AddrRecord* mc){dict_.add_other(mc);}
TypevsMcDict MmIGMPv3AddrRecord_onIGMP::dict_;

// REVERSE
bool MmIGMPv3AddrRecord_onIGMP::overwrite_DictType(
		RControl& c,ItPosition& at,OCTBUF& buf)const{
	McIGMPv3_AddrRecord* any = (McIGMPv3_AddrRecord*)dict_.find_other();
	return any->overwrite_DictType(c,at,buf);
}

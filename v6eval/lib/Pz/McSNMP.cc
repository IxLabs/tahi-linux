/*
 * Copyright (C) IBM Corporation 2004.
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
 *    Author: Kazuo Hiekata <e30948@jp.ibm.com>
 *
 */

#if !defined(NOT_USE_SNMP)

#include "McSub.h"
#include "MmHeader.h"
#include "ItPosition.h"
#include "WObject.h"
#include "RObject.h"
#include "PControl.h"
#include "PvObject.h"
#include "PvOctets.h"
#include "McSNMP.h"
#include <stdio.h>
#include <string.h>

////////////////////////////////////////////////////////////////
#define SUPER McHeader
McUdp_SNMP::McUdp_SNMP(CSTR s): McHeader(s) {}
McUdp_SNMP::~McUdp_SNMP() {}
uint32_t McUdp_SNMP::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const {
	return(buf.remainLength(at.bytes()));
}
RObject *McUdp_SNMP::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	RObject *r_self = SUPER::reverse(c, r_parent, at, buf);
	return(r_self);
}
McUdp_SNMPTrap::McUdp_SNMPTrap(CSTR s): McHeader(s) {}
McUdp_SNMPTrap::~McUdp_SNMPTrap() {}
uint32_t McUdp_SNMPTrap::length_for_reverse(RControl &, ItPosition &at, OCTBUF &buf) const {
	return(buf.remainLength(at.bytes()));
}
RObject *McUdp_SNMPTrap::reverse(RControl &c, RObject *r_parent, ItPosition &at, OCTBUF &buf) const {
	RObject *r_self = SUPER::reverse(c, r_parent, at, buf);
	return(r_self);
}
#undef SUPER
////////////////////////////////////////////////////////////////
#define PDU_HDR_LEN 2
McPdu_SNMP::McPdu_SNMP(CSTR key): McOption(key), type_(0), length_(0) {}
McPdu_SNMP::~McPdu_SNMP() {}
uint32_t McPdu_SNMP::length_for_reverse(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(!length_) {
		return(McOption::length_for_reverse(c, at, buf));
	}
	uint32_t length = length_->value(at, buf) + PDU_HDR_LEN;
	return(length);
}
bool McPdu_SNMP::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		ItPosition tmpat = at;
		RObject *rcode = type_->reverse(c, 0, tmpat,buf);
		if(!rcode) {
			return(false);
		}
		const PvNumber *pv = (const PvNumber *)rcode->pvalue();
		uint32_t codevalue = pv->value();
		c.DictType().type_Set(codevalue);
		delete(rcode);
		return(true);
	}
	return(false);
}
bool McPdu_SNMP::HCGENE(Length)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	WObject *wc = wmem->parent();
	uint32_t valulen = wc->size().bytes() - PDU_HDR_LEN;
	PvNumber def(valulen);
	return(def.generate(cntr, wmem, buf));
}
bool McPdu_SNMP::HCGENE(Type)(WControl &cntr, WObject *wmem, OCTBUF &buf) const {
	int32_t val = get_pduType(wmem);
	if(val < 0) {
		return(false);
	}
	PvNumber def(val);
	return(def.generate(cntr, wmem, buf));
}
PObject *McPdu_SNMP::HCEVAL(Type)(WObject *wmem) const {
	int32_t val = get_pduType(wmem);
	return(new PvNumber(val));
}
int32_t McPdu_SNMP::get_pduType(WObject *wmem) const {
	WObject *wc = wmem->parent();
	int32_t rtn = wc ? wc->meta()->pduType() : -1;
	if(rtn < 0) {
		wmem->mustDefine(0);
	}
	return(rtn);
}
#undef PDU_HDR_LEN
////////////////////////////////////////////////////////////////
McPdu_SNMP_ANY::McPdu_SNMP_ANY(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_ANY::~McPdu_SNMP_ANY() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_Message::McPdu_SNMP_Message(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_Message::~McPdu_SNMP_Message() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_Version::McPdu_SNMP_Version(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_Version::~McPdu_SNMP_Version() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_Community::McPdu_SNMP_Community(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_Community::~McPdu_SNMP_Community() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_GetRequest::McPdu_SNMP_GetRequest(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_GetRequest::~McPdu_SNMP_GetRequest() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_GetNextRequest::McPdu_SNMP_GetNextRequest(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_GetNextRequest::~McPdu_SNMP_GetNextRequest() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_SetRequest::McPdu_SNMP_SetRequest(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_SetRequest::~McPdu_SNMP_SetRequest() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_GetResponse::McPdu_SNMP_GetResponse(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_GetResponse::~McPdu_SNMP_GetResponse() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_Trap::McPdu_SNMP_Trap(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_Trap::~McPdu_SNMP_Trap() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_GetBulk::McPdu_SNMP_GetBulk(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_GetBulk::~McPdu_SNMP_GetBulk() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_Inform::McPdu_SNMP_Inform(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_Inform::~McPdu_SNMP_Inform() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_Trap2::McPdu_SNMP_Trap2(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_Trap2::~McPdu_SNMP_Trap2() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_Report::McPdu_SNMP_Report(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_Report::~McPdu_SNMP_Report() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_RequestID::McPdu_SNMP_RequestID(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_RequestID::~McPdu_SNMP_RequestID() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_ErrorStatus::McPdu_SNMP_ErrorStatus(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_ErrorStatus::~McPdu_SNMP_ErrorStatus() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_ErrorIndex::McPdu_SNMP_ErrorIndex(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_ErrorIndex::~McPdu_SNMP_ErrorIndex() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_GenericTrap::McPdu_SNMP_GenericTrap(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_GenericTrap::~McPdu_SNMP_GenericTrap() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_SpecificTrap::McPdu_SNMP_SpecificTrap(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_SpecificTrap::~McPdu_SNMP_SpecificTrap() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_VarBindList::McPdu_SNMP_VarBindList(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_VarBindList::~McPdu_SNMP_VarBindList() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_VarBind::McPdu_SNMP_VarBind(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_VarBind::~McPdu_SNMP_VarBind() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_Boolean::McPdu_SNMP_Boolean(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_Boolean::~McPdu_SNMP_Boolean() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_Integer::McPdu_SNMP_Integer(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_Integer::~McPdu_SNMP_Integer() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_BitString::McPdu_SNMP_BitString(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_BitString::~McPdu_SNMP_BitString() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_OctString::McPdu_SNMP_OctString(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_OctString::~McPdu_SNMP_OctString() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_Null::McPdu_SNMP_Null(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_Null::~McPdu_SNMP_Null() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_ObjectID::McPdu_SNMP_ObjectID(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_ObjectID::~McPdu_SNMP_ObjectID() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_Sequence::McPdu_SNMP_Sequence(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_Sequence::~McPdu_SNMP_Sequence() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_Set::McPdu_SNMP_Set(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_Set::~McPdu_SNMP_Set() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_Opaque::McPdu_SNMP_Opaque(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_Opaque::~McPdu_SNMP_Opaque() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_IPAddress::McPdu_SNMP_IPAddress(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_IPAddress::~McPdu_SNMP_IPAddress() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_Counter::McPdu_SNMP_Counter(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_Counter::~McPdu_SNMP_Counter() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_Gauge::McPdu_SNMP_Gauge(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_Gauge::~McPdu_SNMP_Gauge() {}

////////////////////////////////////////////////////////////////
McPdu_SNMP_TimeTicks::McPdu_SNMP_TimeTicks(CSTR key): McPdu_SNMP(key) {}
McPdu_SNMP_TimeTicks::~McPdu_SNMP_TimeTicks() {}


////////////////////////////////////////////////////////////////
MmPdu_SNMP_onSNMP::MmPdu_SNMP_onSNMP(CSTR key): MmReference_More0(key, true) {}
MmPdu_SNMP_onSNMP::~MmPdu_SNMP_onSNMP() {}
void MmPdu_SNMP_onSNMP::add(McPdu_SNMP *mc) {
	dict_.add(mc->pduType(), mc);
}
void MmPdu_SNMP_onSNMP::add_other(McPdu_SNMP *mc) {
	dict_.add_other(mc);
}
TypevsMcDict MmPdu_SNMP_onSNMP::dict_;
bool MmPdu_SNMP_onSNMP::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McPdu_SNMP *any = (McPdu_SNMP *)dict_.find_other();
		return(any->overwrite_DictType(c, at, buf));
	}
	return(false);
}
////////////////////////////////////////////////////////////////
MmPdu_SNMP_onPDU::MmPdu_SNMP_onPDU(CSTR key): MmReference_More0(key, true) {}
MmPdu_SNMP_onPDU::~MmPdu_SNMP_onPDU() {}
void MmPdu_SNMP_onPDU::add(McPdu_SNMP *mc) {
	dict_.add(mc->pduType(), mc);
}
void MmPdu_SNMP_onPDU::add_other(McPdu_SNMP *mc) {
	dict_.add_other(mc);
}
TypevsMcDict MmPdu_SNMP_onPDU::dict_;
bool MmPdu_SNMP_onPDU::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McPdu_SNMP *any = (McPdu_SNMP *)dict_.find_other();
		return(any->overwrite_DictType(c, at, buf));
	}
	return(false);
}
////////////////////////////////////////////////////////////////
MmSNMP_Version_onSNMP_Message::MmSNMP_Version_onSNMP_Message(CSTR key): MmReference_Must1(key, true) {}
MmSNMP_Version_onSNMP_Message::~MmSNMP_Version_onSNMP_Message() {}
void MmSNMP_Version_onSNMP_Message::add(McPdu_SNMP *mc) {
	dict_.add(mc->pduType(), mc);
}
void MmSNMP_Version_onSNMP_Message::add_other(McPdu_SNMP *mc) {
	dict_.add_other(mc);
}
TypevsMcDict MmSNMP_Version_onSNMP_Message::dict_;
bool MmSNMP_Version_onSNMP_Message::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McPdu_SNMP *any = (McPdu_SNMP *)dict_.find_other();
		return(any->overwrite_DictType(c, at, buf));
	}
	return(false);
}
////////////////////////////////////////////////////////////////
MmSNMP_Community_onSNMP_Message::MmSNMP_Community_onSNMP_Message(CSTR key): MmReference_Must1(key, true) {}
MmSNMP_Community_onSNMP_Message::~MmSNMP_Community_onSNMP_Message() {}
void MmSNMP_Community_onSNMP_Message::add(McPdu_SNMP *mc) {
	dict_.add(mc->pduType(), mc);
}
void MmSNMP_Community_onSNMP_Message::add_other(McPdu_SNMP *mc) {
	dict_.add_other(mc);
}
TypevsMcDict MmSNMP_Community_onSNMP_Message::dict_;
bool MmSNMP_Community_onSNMP_Message::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McPdu_SNMP *any = (McPdu_SNMP *)dict_.find_other();
		return(any->overwrite_DictType(c, at, buf));
	}
	return(false);
}
////////////////////////////////////////////////////////////////
MmSNMP_RequestID_onSNMP_Message::MmSNMP_RequestID_onSNMP_Message(CSTR key): MmReference_Must1(key, true) {}
MmSNMP_RequestID_onSNMP_Message::~MmSNMP_RequestID_onSNMP_Message() {}
void MmSNMP_RequestID_onSNMP_Message::add(McPdu_SNMP *mc) {
	dict_.add(mc->pduType(), mc);
}
void MmSNMP_RequestID_onSNMP_Message::add_other(McPdu_SNMP *mc) {
	dict_.add_other(mc);
}
TypevsMcDict MmSNMP_RequestID_onSNMP_Message::dict_;
bool MmSNMP_RequestID_onSNMP_Message::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McPdu_SNMP *any = (McPdu_SNMP *)dict_.find_other();
		return(any->overwrite_DictType(c, at, buf));
	}
	return(false);
}
////////////////////////////////////////////////////////////////
MmSNMP_ErrorStatus_onSNMP_Message::MmSNMP_ErrorStatus_onSNMP_Message(CSTR key): MmReference_Must1(key, true) {}
MmSNMP_ErrorStatus_onSNMP_Message::~MmSNMP_ErrorStatus_onSNMP_Message() {}
void MmSNMP_ErrorStatus_onSNMP_Message::add(McPdu_SNMP *mc) {
	dict_.add(mc->pduType(), mc);
}
void MmSNMP_ErrorStatus_onSNMP_Message::add_other(McPdu_SNMP *mc) {
	dict_.add_other(mc);
}
TypevsMcDict MmSNMP_ErrorStatus_onSNMP_Message::dict_;
bool MmSNMP_ErrorStatus_onSNMP_Message::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McPdu_SNMP *any = (McPdu_SNMP *)dict_.find_other();
		return(any->overwrite_DictType(c, at, buf));
	}
	return(false);
}
////////////////////////////////////////////////////////////////
MmSNMP_ErrorIndex_onSNMP_Message::MmSNMP_ErrorIndex_onSNMP_Message(CSTR key): MmReference_Must1(key, true) {}
MmSNMP_ErrorIndex_onSNMP_Message::~MmSNMP_ErrorIndex_onSNMP_Message() {}
void MmSNMP_ErrorIndex_onSNMP_Message::add(McPdu_SNMP *mc) {
	dict_.add(mc->pduType(), mc);
}
void MmSNMP_ErrorIndex_onSNMP_Message::add_other(McPdu_SNMP *mc) {
	dict_.add_other(mc);
}
TypevsMcDict MmSNMP_ErrorIndex_onSNMP_Message::dict_;
bool MmSNMP_ErrorIndex_onSNMP_Message::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McPdu_SNMP *any = (McPdu_SNMP *)dict_.find_other();
		return(any->overwrite_DictType(c, at, buf));
	}
	return(false);
}
////////////////////////////////////////////////////////////////
MmSNMP_ObjectID_onTrap_Message::MmSNMP_ObjectID_onTrap_Message(CSTR key): MmReference_Must1(key, true) {}
MmSNMP_ObjectID_onTrap_Message::~MmSNMP_ObjectID_onTrap_Message() {}
void MmSNMP_ObjectID_onTrap_Message::add(McPdu_SNMP *mc) {
	dict_.add(mc->pduType(), mc);
}
void MmSNMP_ObjectID_onTrap_Message::add_other(McPdu_SNMP *mc) {
	dict_.add_other(mc);
}
TypevsMcDict MmSNMP_ObjectID_onTrap_Message::dict_;
bool MmSNMP_ObjectID_onTrap_Message::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McPdu_SNMP *any = (McPdu_SNMP *)dict_.find_other();
		return(any->overwrite_DictType(c, at, buf));
	}
	return(false);
}
////////////////////////////////////////////////////////////////
MmSNMP_IPAddress_onTrap_Message::MmSNMP_IPAddress_onTrap_Message(CSTR key): MmReference_Must1(key, true) {}
MmSNMP_IPAddress_onTrap_Message::~MmSNMP_IPAddress_onTrap_Message() {}
void MmSNMP_IPAddress_onTrap_Message::add(McPdu_SNMP *mc) {
	dict_.add(mc->pduType(), mc);
}
void MmSNMP_IPAddress_onTrap_Message::add_other(McPdu_SNMP *mc) {
	dict_.add_other(mc);
}
TypevsMcDict MmSNMP_IPAddress_onTrap_Message::dict_;
bool MmSNMP_IPAddress_onTrap_Message::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McPdu_SNMP *any = (McPdu_SNMP *)dict_.find_other();
		return(any->overwrite_DictType(c, at, buf));
	}
	return(false);
}
////////////////////////////////////////////////////////////////
MmSNMP_GenericTrap_onTrap_Message::MmSNMP_GenericTrap_onTrap_Message(CSTR key): MmReference_Must1(key, true) {}
MmSNMP_GenericTrap_onTrap_Message::~MmSNMP_GenericTrap_onTrap_Message() {}
void MmSNMP_GenericTrap_onTrap_Message::add(McPdu_SNMP *mc) {
	dict_.add(mc->pduType(), mc);
}
void MmSNMP_GenericTrap_onTrap_Message::add_other(McPdu_SNMP *mc) {
	dict_.add_other(mc);
}
TypevsMcDict MmSNMP_GenericTrap_onTrap_Message::dict_;
bool MmSNMP_GenericTrap_onTrap_Message::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McPdu_SNMP *any = (McPdu_SNMP *)dict_.find_other();
		return(any->overwrite_DictType(c, at, buf));
	}
	return(false);
}
////////////////////////////////////////////////////////////////
MmSNMP_SpecificTrap_onTrap_Message::MmSNMP_SpecificTrap_onTrap_Message(CSTR key): MmReference_Must1(key, true) {}
MmSNMP_SpecificTrap_onTrap_Message::~MmSNMP_SpecificTrap_onTrap_Message() {}
void MmSNMP_SpecificTrap_onTrap_Message::add(McPdu_SNMP *mc) {
	dict_.add(mc->pduType(), mc);
}
void MmSNMP_SpecificTrap_onTrap_Message::add_other(McPdu_SNMP *mc) {
	dict_.add_other(mc);
}
TypevsMcDict MmSNMP_SpecificTrap_onTrap_Message::dict_;
bool MmSNMP_SpecificTrap_onTrap_Message::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McPdu_SNMP *any = (McPdu_SNMP *)dict_.find_other();
		return(any->overwrite_DictType(c, at, buf));
	}
	return(false);
}
////////////////////////////////////////////////////////////////
MmSNMP_TimeStamp_onTrap_Message::MmSNMP_TimeStamp_onTrap_Message(CSTR key): MmReference_Must1(key, true) {}
MmSNMP_TimeStamp_onTrap_Message::~MmSNMP_TimeStamp_onTrap_Message() {}
void MmSNMP_TimeStamp_onTrap_Message::add(McPdu_SNMP *mc) {
	dict_.add(mc->pduType(), mc);
}
void MmSNMP_TimeStamp_onTrap_Message::add_other(McPdu_SNMP *mc) {
	dict_.add_other(mc);
}
TypevsMcDict MmSNMP_TimeStamp_onTrap_Message::dict_;
bool MmSNMP_TimeStamp_onTrap_Message::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McPdu_SNMP *any = (McPdu_SNMP *)dict_.find_other();
		return(any->overwrite_DictType(c, at, buf));
	}
	return(false);
}
////////////////////////////////////////////////////////////////
MmSNMP_VarBindList_onPDU::MmSNMP_VarBindList_onPDU(CSTR key): MmReference_More0(key, true) {}
MmSNMP_VarBindList_onPDU::~MmSNMP_VarBindList_onPDU() {}
void MmSNMP_VarBindList_onPDU::add(McPdu_SNMP *mc) {
	dict_.add(mc->pduType(), mc);
}
void MmSNMP_VarBindList_onPDU::add_other(McPdu_SNMP *mc) {
	dict_.add_other(mc);
}
TypevsMcDict MmSNMP_VarBindList_onPDU::dict_;
bool MmSNMP_VarBindList_onPDU::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McPdu_SNMP *any = (McPdu_SNMP *)dict_.find_other();
		return(any->overwrite_DictType(c, at, buf));
	}
	return(false);
}
////////////////////////////////////////////////////////////////
MmSNMP_VarBind_onPDU::MmSNMP_VarBind_onPDU(CSTR key): MmReference_More0(key, true) {}
MmSNMP_VarBind_onPDU::~MmSNMP_VarBind_onPDU() {}
void MmSNMP_VarBind_onPDU::add(McPdu_SNMP *mc) {
	dict_.add(mc->pduType(), mc);
}
void MmSNMP_VarBind_onPDU::add_other(McPdu_SNMP *mc) {
	dict_.add_other(mc);
}
TypevsMcDict MmSNMP_VarBind_onPDU::dict_;
bool MmSNMP_VarBind_onPDU::overwrite_DictType(RControl &c, ItPosition &at, OCTBUF &buf) const {
	if(buf.remainLength(at.bytes())) {
		McPdu_SNMP *any = (McPdu_SNMP *)dict_.find_other();
		return(any->overwrite_DictType(c, at, buf));
	}
	return(false);
}
////////////////////////////////////////////////////////////////
#define SNMPVERSION_LEN 1
MmSNMPVersion::MmSNMPVersion(CSTR s,uint16_t n,const PObject* g,const PObject* e,
	const ICVoverwriter* ow,METH_HC_ForIPinfo meth):
	MmUint(s,n,g,e,ow,meth) {}
MmSNMPVersion::~MmSNMPVersion() {}

PvObject* MmSNMPVersion::reversePv(RControl&,
		const ItPosition& at,const ItPosition&,const OCTBUF& buf)const{
	uint32_t val = decode(at,buf);
	return new PvSNMPVersion(val);}
////////////////////////////////////////////////////////////////
PvSNMPVersion::~PvSNMPVersion() {}
PvSNMPVersion::PvSNMPVersion():PvNumber() {}
PvSNMPVersion::PvSNMPVersion(int32_t x):PvNumber(x) {}
PvSNMPVersion::PvSNMPVersion(const PvNumber& x):PvNumber(x) {}
void PvSNMPVersion::print() const {
	printf("%u", (value()&0xff));
	switch(value()&0xff) {
		case 0:	printf(" (Version 1)"); break;
		case 1:	printf(" (Version 2C)"); break;
		case 2:	printf(" (Version 3)"); break;
		default: printf(" (Unknown Version)"); break;
	}
	return;
}
#undef SNMPVERSION_LEN
////////////////////////////////////////////////////////////////
MmObjectID::MmObjectID(CSTR s, const PObject *g, const PObject *e): MmVarOctets(s, g, e, 0, 0) {}
MmObjectID::~MmObjectID() {}

PvObject *MmObjectID::reversePv(RControl &, const ItPosition &at, const ItPosition &size, const OCTBUF &buf) const {
	uint32_t offset = at.bytes();
	uint32_t length = size.bytes();
	return(new PvObjectID(length, (OCTSTR)buf.string(offset)));
}

////////////////////////////////////////////////////////////////
PvObjectID::PvObjectID(): PvOctets() {}
PvObjectID::PvObjectID(uint32_t l, OCTSTR o, bool b): PvOctets(l, o, b) {}
PvObjectID::~PvObjectID() {}
PvObject *PvObjectID::shallowCopy() const {
	return(new PvObjectID(length(), (OCTSTR)string()));
}
void PvObjectID::log(uint32_t t) const {
	CmCString s("\nlog:");
	for(uint32_t i=0;i<t;i++) {s += "| ";}
		s += "  ";
	dump(s.string());
	return;
}
void PvObjectID::print() const {
	dump(length()>16?0:"");
	return;
}
void PvObjectID::dump(CSTR tag) const {
	int i, j, k, i9=length();
	char tmp[256];
	char tmp2[64];
	COCTSTR s=string();
	for(i=0;i<i9;i++)
		printf("%02x",s[i]&0xff);
	printf((tag!=0&&length()>16)?tag:"");
	int a=(int)(s[0]&0xff);
	sprintf(tmp, "%d.%d.",(int)(a/40), (int)(a%40));
	/* ASN.1 decode */	
	for(i=1;i<i9;i++){
		j=0;
		a=(int)(s[i]&0xff);
		if (a > 0x7F)
			for(j=1;j<i9-i;j++)
				if (s[i+j] < 0x80) break;
		
		if (j!=0) {
			a=0;
			for (k=i; k<=i+j; k++)
				a+=(int)((s[k]&0x7f)<<(7*(i+j-k)));
		}
		if(i+j<i9-1)
			sprintf(tmp2, "%d.",a);
		else
			sprintf(tmp2, "%d", a);
		strcat(tmp, tmp2);
		i+=j;
	}
	printf(" (%s)", tmp);
	return;
}
////////////////////////////////////////////////////////////////
MmSNMPString::MmSNMPString(CSTR s, const PObject *g, const PObject *e): MmVarOctets(s, g, e, 0, 0) {}
MmSNMPString::~MmSNMPString() {}

PvObject *MmSNMPString::reversePv(RControl &, const ItPosition &at, const ItPosition &size, const OCTBUF &buf) const {
	uint32_t offset = at.bytes();
	uint32_t length = size.bytes();
	return(new PvSNMPString(length, (OCTSTR)buf.string(offset)));
}
////////////////////////////////////////////////////////////////
PvSNMPString::PvSNMPString(): PvOctets() {}
PvSNMPString::PvSNMPString(uint32_t l, OCTSTR o, bool b): PvOctets(l, o, b) {}
PvSNMPString::~PvSNMPString() {}
PvObject *PvSNMPString::shallowCopy() const {
	return(new PvSNMPString(length(), (OCTSTR)string()));
}
void PvSNMPString::log(uint32_t t) const {
	dump("\nlog:  ");
	return;
}
void PvSNMPString::print() const {
	dump();
	return;
}
void PvSNMPString::dump(CSTR tag) const {
	int i, i9=length();
	COCTSTR s=string();
//	CSTR nl=tag!=0?tag:"\n";
	CSTR pad=(tag!=0&&length()>16)?tag:"";
	for(i=0;i<i9;i++) {
		printf("%s%02x",pad,s[i]&0xff);
		switch((i%32)+1) {
			case 4: case 12: case 20: case 28:	pad=""; break;
			case 8:	case 16: case 24:		pad=""; break;
			case 32:				pad=""; break;
			default:				pad=""; break;}}
//			case 4: case 12: case 20: case 28:	pad=" "; break;
//			case 8:	case 16: case 24:		pad="  "; break;
//			case 32:				pad=nl; break;
//			default:				pad=""; break;}}
	printf(" ascii(");
	for(i=0;i<i9;i++)
		printf("%.01s",&s[i]);
	printf(")");
	return;
}
////////////////////////////////////////////////////////////////
#define ERRORSTATUS_LEN 1
MmErrorStatus::MmErrorStatus(CSTR s,uint16_t n,const PObject* g,const PObject* e,
	const ICVoverwriter* ow,METH_HC_ForIPinfo meth):
	MmUint(s,n,g,e,ow,meth) {}
MmErrorStatus::~MmErrorStatus() {}

PvObject* MmErrorStatus::reversePv(RControl&,
		const ItPosition& at,const ItPosition&,const OCTBUF& buf)const{
	uint32_t val = decode(at,buf);
	return (new PvErrorStatus(val));
}
////////////////////////////////////////////////////////////////
PvErrorStatus::~PvErrorStatus() {}
PvErrorStatus::PvErrorStatus():PvNumber() {}
PvErrorStatus::PvErrorStatus(int32_t x):PvNumber(x) {}
void PvErrorStatus::print() const {
	printf("%u", (value()&0xff));
	switch(value()&0xff) {
		/* Errors defined in rfc1157.txt	*/
		case 0: printf(" (noError)"); break;
		case 1: printf(" (tooBig)"); break;
		case 2: printf(" (noSuchName)"); break;
		case 3: printf(" (badValue)"); break;
		case 4: printf(" (readOnly)"); break;
		case 5: printf(" (genErr)"); break;
		/* Errors defined in rfc1905.txt	*/
		case 6: printf(" (noAccess)"); break;
		case 7: printf(" (wrongType)"); break;
		case 8: printf(" (wrongLength)"); break;
		case 9: printf(" (wrongEncoding)"); break;
		case 10: printf(" (wrongValue)"); break;
		case 11: printf(" (noCreation)"); break;
		case 12: printf(" (inconsistentValue)"); break;
		case 13: printf(" (resourceUnavailable)"); break;
		case 14: printf(" (commitFailed)"); break;
		case 15: printf(" (undoFailed)"); break;
		case 16: printf(" (authorizationError)"); break;
		case 17: printf(" (notWritable)"); break;
		case 18: printf(" (inconsistentName)"); break;
		/* Unknown */
		default: printf(" (Unknown Error)"); break;
	}
	return;
}
#undef ERRORSTATUS_LEN
////////////////////////////////////////////////////////////////

#endif //NOT_USE_SNMP

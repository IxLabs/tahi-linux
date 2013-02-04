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

#if !defined(__McSNMP_h__)
#define	__McSNMP_h__	1

#include "PvOctets.h"

#include "McSub.h"
#include "McUDP.h"

// ASN.1 type
const int32_t TP_SNMP_ASN_Boolean	= 1;
const int32_t TP_SNMP_ASN_Integer	= 2;
const int32_t TP_SNMP_ASN_BitString	= 3;
const int32_t TP_SNMP_ASN_OctString	= 4;
const int32_t TP_SNMP_ASN_Null	= 5;
const int32_t TP_SNMP_ASN_ObjectID	= 6;
const int32_t TP_SNMP_ASN_Sequence	= 16;	// 0x10
const int32_t TP_SNMP_ASN_Set	= 17;	// 0x11
const int32_t TP_SNMP_ASN_Opaque	= 48;	// 0x30
const int32_t TP_SNMP_ASN_IPAddress	= 64;	// 0x40
const int32_t TP_SNMP_ASN_Counter	= 65;	// 0x41
const int32_t TP_SNMP_ASN_Gauge	= 66;	// 0x42
const int32_t TP_SNMP_ASN_TimeTicks	= 67;	// 0x43

// RFC 1157: A Simple Network Management Protocol (SNMP)
// RFC 1905: Protocol Operations for Version 2 of the Simple
//          Network Management Protocol (SNMPv2)
const int32_t TP_SNMP_GetRequest		= 160;	// 0xa0 (RFC 1157 & 1905) 
const int32_t TP_SNMP_GetNextRequest		= 161;	// 0xa1 (RFC 1157 & 1905)
const int32_t TP_SNMP_GetResponse		= 162;	// 0xa2 (RFC 1157 & 1905)
const int32_t TP_SNMP_SetRequest		= 163;	// 0xa3 (RFC 1157 & 1905)
const int32_t TP_SNMP_Trap			= 164;	// 0xa4 (RFC 1157)
const int32_t TP_SNMP_GetBulk		= 165;	// 0xa5 (RFC 1905)
const int32_t TP_SNMP_Inform		= 166;	// 0xa6 (RFC 1905)
const int32_t TP_SNMP_Trap2			= 167;	// 0xa7 (RFC 1905)
const int32_t TP_SNMP_Report		= 168;	// 0xa8 (RFC 1905)

// SNMP type (aliases)
const int32_t TP_SNMP_Message		= 48;	// TP_SNMP_ASN_Opaque
const int32_t TP_SNMP_Version		= 2;	// TP_SNMP_ASN_Integer
const int32_t TP_SNMP_Community		= 4;	// TP_SNMP_ASN_OctString
const int32_t TP_SNMP_RequestID		= 2;	// TP_SNMP_ASN_Integer
const int32_t TP_SNMP_ErrorStatus		= 2;	// TP_SNMP_ASN_Integer
const int32_t TP_SNMP_ErrorIndex		= 2;	// TP_SNMP_ASN_Integer
const int32_t TP_SNMP_GenericTrap		= 2;	// TP_SNMP_ASN_Integer
const int32_t TP_SNMP_SpecificTrap		= 2;	// TP_SNMP_ASN_Integer
const int32_t TP_SNMP_VarBindList		= 48;	// TP_SNMP_ASN_Opaque
const int32_t TP_SNMP_VarBind		= 48;	// TP_SNMP_ASN_Opaque

////////////////////////////////////////////////////////////////
class McUdp_SNMP: public McHeader {
	protected:
		McUdp_SNMP(CSTR);
	public:
		virtual ~McUdp_SNMP();
		static McUdp_SNMP *create(CSTR);
		virtual uint32_t length_for_reverse(RControl &, ItPosition &,
			OCTBUF &) const;
		int32_t upperPort() const {
			return(TP_Udp_SNMP);
		}
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
};
////////////////////////////////////////////////////////////////
class McUdp_SNMPTrap: public McHeader {
	protected:
		McUdp_SNMPTrap(CSTR);
	public:
		virtual ~McUdp_SNMPTrap();
		static McUdp_SNMPTrap *create(CSTR);
		virtual uint32_t length_for_reverse(RControl &, ItPosition &,
			OCTBUF &) const;
		int32_t upperPort() const {
			return(TP_Udp_SNMPTRAP);
		}
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP: public McOption {
	protected:
		MmUint *type_;
		void type_member(MmUint *meta) {
			type_ = meta;
			member(meta);
		}
		MmUint *length_;
		void length_member(MmUint *meta) {
			length_ = meta;
			member(meta);
		}
		void common_member();
		McPdu_SNMP(CSTR);
	public:
		virtual ~McPdu_SNMP();
		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
		//HardCording action method
		DEC_HCGENE(Length);
		DEC_HCGENE(Type);
		DEC_HCEVAL(Type);
		int32_t get_pduType(WObject *) const;
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_ANY: public McPdu_SNMP {
	public:
		McPdu_SNMP_ANY(CSTR);
		virtual ~McPdu_SNMP_ANY();
		static McPdu_SNMP_ANY *create(CSTR);
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_Message: public McPdu_SNMP {
	public:
		McPdu_SNMP_Message(CSTR);
		virtual ~McPdu_SNMP_Message();
		static McPdu_SNMP_Message *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_Message);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_Version: public McPdu_SNMP {
	public:
		McPdu_SNMP_Version(CSTR);
		virtual ~McPdu_SNMP_Version();
		static McPdu_SNMP_Version *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_Version);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_Community: public McPdu_SNMP {
	public:
		McPdu_SNMP_Community(CSTR);
		virtual ~McPdu_SNMP_Community();
		static McPdu_SNMP_Community *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_Community);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_GetRequest: public McPdu_SNMP {
	public:
		McPdu_SNMP_GetRequest(CSTR);
		virtual ~McPdu_SNMP_GetRequest();
		static McPdu_SNMP_GetRequest *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_GetRequest);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_GetNextRequest: public McPdu_SNMP {
	public:
		McPdu_SNMP_GetNextRequest(CSTR);
		virtual ~McPdu_SNMP_GetNextRequest();
		static McPdu_SNMP_GetNextRequest *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_GetNextRequest);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_SetRequest: public McPdu_SNMP {
	public:
		McPdu_SNMP_SetRequest(CSTR);
		virtual ~McPdu_SNMP_SetRequest();
		static McPdu_SNMP_SetRequest *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_SetRequest);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_GetResponse: public McPdu_SNMP {
	public:
		McPdu_SNMP_GetResponse(CSTR);
		virtual ~McPdu_SNMP_GetResponse();
		static McPdu_SNMP_GetResponse *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_GetResponse);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_Trap: public McPdu_SNMP {
	public:
		McPdu_SNMP_Trap(CSTR);
		virtual ~McPdu_SNMP_Trap();
		static McPdu_SNMP_Trap *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_Trap);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_GetBulk: public McPdu_SNMP {
	public:
		McPdu_SNMP_GetBulk(CSTR);
		virtual ~McPdu_SNMP_GetBulk();
		static McPdu_SNMP_GetBulk *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_GetBulk);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_Inform: public McPdu_SNMP {
	public:
		McPdu_SNMP_Inform(CSTR);
		virtual ~McPdu_SNMP_Inform();
		static McPdu_SNMP_Inform *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_Inform);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_Trap2: public McPdu_SNMP {
	public:
		McPdu_SNMP_Trap2(CSTR);
		virtual ~McPdu_SNMP_Trap2();
		static McPdu_SNMP_Trap2 *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_Trap2);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_Report: public McPdu_SNMP {
	public:
		McPdu_SNMP_Report(CSTR);
		virtual ~McPdu_SNMP_Report();
		static McPdu_SNMP_Report *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_Report);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_RequestID: public McPdu_SNMP {
	public:
		McPdu_SNMP_RequestID(CSTR);
		virtual ~McPdu_SNMP_RequestID();
		static McPdu_SNMP_RequestID *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_RequestID);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_ErrorStatus: public McPdu_SNMP {
	public:
		McPdu_SNMP_ErrorStatus(CSTR);
		virtual ~McPdu_SNMP_ErrorStatus();
		static McPdu_SNMP_ErrorStatus *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_ErrorStatus);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_ErrorIndex: public McPdu_SNMP {
	public:
		McPdu_SNMP_ErrorIndex(CSTR);
		virtual ~McPdu_SNMP_ErrorIndex();
		static McPdu_SNMP_ErrorIndex *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_ErrorIndex);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_GenericTrap: public McPdu_SNMP {
	public:
		McPdu_SNMP_GenericTrap(CSTR);
		virtual ~McPdu_SNMP_GenericTrap();
		static McPdu_SNMP_GenericTrap *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_GenericTrap);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_SpecificTrap: public McPdu_SNMP {
	public:
		McPdu_SNMP_SpecificTrap(CSTR);
		virtual ~McPdu_SNMP_SpecificTrap();
		static McPdu_SNMP_SpecificTrap *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_SpecificTrap);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_VarBindList: public McPdu_SNMP {
	public:
		McPdu_SNMP_VarBindList(CSTR);
		virtual ~McPdu_SNMP_VarBindList();
		static McPdu_SNMP_VarBindList *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_VarBindList);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_VarBind: public McPdu_SNMP {
	public:
		McPdu_SNMP_VarBind(CSTR);
		virtual ~McPdu_SNMP_VarBind();
		static McPdu_SNMP_VarBind *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_VarBind);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_Boolean: public McPdu_SNMP {
	public:
		McPdu_SNMP_Boolean(CSTR);
		virtual ~McPdu_SNMP_Boolean();
		static McPdu_SNMP_Boolean *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_ASN_Boolean);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_Integer: public McPdu_SNMP {
	public:
		McPdu_SNMP_Integer(CSTR);
		virtual ~McPdu_SNMP_Integer();
		static McPdu_SNMP_Integer *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_ASN_Integer);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_BitString: public McPdu_SNMP {
	public:
		McPdu_SNMP_BitString(CSTR);
		virtual ~McPdu_SNMP_BitString();
		static McPdu_SNMP_BitString *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_ASN_BitString);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_OctString: public McPdu_SNMP {
	public:
		McPdu_SNMP_OctString(CSTR);
		virtual ~McPdu_SNMP_OctString();
		static McPdu_SNMP_OctString *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_ASN_OctString);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_Null: public McPdu_SNMP {
	public:
		McPdu_SNMP_Null(CSTR);
		virtual ~McPdu_SNMP_Null();
		static McPdu_SNMP_Null *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_ASN_Null);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_ObjectID: public McPdu_SNMP {
	public:
		McPdu_SNMP_ObjectID(CSTR);
		virtual ~McPdu_SNMP_ObjectID();
		static McPdu_SNMP_ObjectID *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_ASN_ObjectID);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_Sequence: public McPdu_SNMP {
	public:
		McPdu_SNMP_Sequence(CSTR);
		virtual ~McPdu_SNMP_Sequence();
		static McPdu_SNMP_Sequence *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_ASN_Sequence);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_Set: public McPdu_SNMP {
	public:
		McPdu_SNMP_Set(CSTR);
		virtual ~McPdu_SNMP_Set();
		static McPdu_SNMP_Set *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_ASN_Set);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_Opaque: public McPdu_SNMP {
	public:
		McPdu_SNMP_Opaque(CSTR);
		virtual ~McPdu_SNMP_Opaque();
		static McPdu_SNMP_Opaque *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_ASN_Opaque);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_IPAddress: public McPdu_SNMP {
	public:
		McPdu_SNMP_IPAddress(CSTR);
		virtual ~McPdu_SNMP_IPAddress();
		static McPdu_SNMP_IPAddress *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_ASN_IPAddress);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_Counter: public McPdu_SNMP {
	public:
		McPdu_SNMP_Counter(CSTR);
		virtual ~McPdu_SNMP_Counter();
		static McPdu_SNMP_Counter *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_ASN_Counter);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_Gauge: public McPdu_SNMP {
	public:
		McPdu_SNMP_Gauge(CSTR);
		virtual ~McPdu_SNMP_Gauge();
		static McPdu_SNMP_Gauge *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_ASN_Gauge);
		}
};
////////////////////////////////////////////////////////////////
class McPdu_SNMP_TimeTicks: public McPdu_SNMP {
	public:
		McPdu_SNMP_TimeTicks(CSTR);
		virtual ~McPdu_SNMP_TimeTicks();
		static McPdu_SNMP_TimeTicks *create(CSTR);
		int32_t pduType() const {
			return(TP_SNMP_ASN_TimeTicks);
		}
};
////////////////////////////////////////////////////////////////
class MmPdu_SNMP_onSNMP: public MmReference_More0 {
	static TypevsMcDict dict_;
	public:
		MmPdu_SNMP_onSNMP(CSTR);
		virtual ~MmPdu_SNMP_onSNMP();
		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}
		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}
		static void add(McPdu_SNMP *mc);
		static void add_other(McPdu_SNMP *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};
////////////////////////////////////////////////////////////////
class MmSNMP_Version_onSNMP_Message: public MmReference_Must1 {
	static TypevsMcDict dict_;
	public:
		MmSNMP_Version_onSNMP_Message(CSTR);
		virtual ~MmSNMP_Version_onSNMP_Message();
		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}
		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}
		static void add(McPdu_SNMP *mc);
		static void add_other(McPdu_SNMP *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};
////////////////////////////////////////////////////////////////
class MmSNMP_Community_onSNMP_Message: public MmReference_Must1 {
	static TypevsMcDict dict_;
	public:
		MmSNMP_Community_onSNMP_Message(CSTR);
		virtual ~MmSNMP_Community_onSNMP_Message();
		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}
		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}
		static void add(McPdu_SNMP *mc);
		static void add_other(McPdu_SNMP *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};
////////////////////////////////////////////////////////////////
class MmSNMP_RequestID_onSNMP_Message: public MmReference_Must1 {
	static TypevsMcDict dict_;
	public:
		MmSNMP_RequestID_onSNMP_Message(CSTR);
		virtual ~MmSNMP_RequestID_onSNMP_Message();
		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}
		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}
		static void add(McPdu_SNMP *mc);
		static void add_other(McPdu_SNMP *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};
////////////////////////////////////////////////////////////////
class MmSNMP_ErrorStatus_onSNMP_Message: public MmReference_Must1 {
	static TypevsMcDict dict_;
	public:
		MmSNMP_ErrorStatus_onSNMP_Message(CSTR);
		virtual ~MmSNMP_ErrorStatus_onSNMP_Message();
		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}
		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}
		static void add(McPdu_SNMP *mc);
		static void add_other(McPdu_SNMP *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};
////////////////////////////////////////////////////////////////
class MmSNMP_ErrorIndex_onSNMP_Message: public MmReference_Must1 {
	static TypevsMcDict dict_;
	public:
		MmSNMP_ErrorIndex_onSNMP_Message(CSTR);
		virtual ~MmSNMP_ErrorIndex_onSNMP_Message();
		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}
		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}
		static void add(McPdu_SNMP *mc);
		static void add_other(McPdu_SNMP *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};
////////////////////////////////////////////////////////////////
class MmSNMP_ObjectID_onTrap_Message: public MmReference_Must1 {
	static TypevsMcDict dict_;
	public:
		MmSNMP_ObjectID_onTrap_Message(CSTR);
		virtual ~MmSNMP_ObjectID_onTrap_Message();
		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}
		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}
		static void add(McPdu_SNMP *mc);
		static void add_other(McPdu_SNMP *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};
////////////////////////////////////////////////////////////////
class MmSNMP_IPAddress_onTrap_Message: public MmReference_Must1 {
	static TypevsMcDict dict_;
	public:
		MmSNMP_IPAddress_onTrap_Message(CSTR);
		virtual ~MmSNMP_IPAddress_onTrap_Message();
		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}
		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}
		static void add(McPdu_SNMP *mc);
		static void add_other(McPdu_SNMP *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};
////////////////////////////////////////////////////////////////
class MmSNMP_GenericTrap_onTrap_Message: public MmReference_Must1 {
	static TypevsMcDict dict_;
	public:
		MmSNMP_GenericTrap_onTrap_Message(CSTR);
		virtual ~MmSNMP_GenericTrap_onTrap_Message();
		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}
		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}
		static void add(McPdu_SNMP *mc);
		static void add_other(McPdu_SNMP *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};
////////////////////////////////////////////////////////////////
class MmSNMP_SpecificTrap_onTrap_Message: public MmReference_Must1 {
	static TypevsMcDict dict_;
	public:
		MmSNMP_SpecificTrap_onTrap_Message(CSTR);
		virtual ~MmSNMP_SpecificTrap_onTrap_Message();
		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}
		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}
		static void add(McPdu_SNMP *mc);
		static void add_other(McPdu_SNMP *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};
////////////////////////////////////////////////////////////////
class MmSNMP_TimeStamp_onTrap_Message: public MmReference_Must1 {
	static TypevsMcDict dict_;
	public:
		MmSNMP_TimeStamp_onTrap_Message(CSTR);
		virtual ~MmSNMP_TimeStamp_onTrap_Message();
		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}
		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}
		static void add(McPdu_SNMP *mc);
		static void add_other(McPdu_SNMP *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};
////////////////////////////////////////////////////////////////
class MmSNMP_VarBindList_onPDU: public MmReference_More0 {
	static TypevsMcDict dict_;
	public:
		MmSNMP_VarBindList_onPDU(CSTR);
		virtual ~MmSNMP_VarBindList_onPDU();
		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}
		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}
		static void add(McPdu_SNMP *mc);
		static void add_other(McPdu_SNMP *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};
////////////////////////////////////////////////////////////////
class MmSNMP_VarBind_onPDU: public MmReference_More0 {
	static TypevsMcDict dict_;
	public:
		MmSNMP_VarBind_onPDU(CSTR);
		virtual ~MmSNMP_VarBind_onPDU();
		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}
		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}
		static void add(McPdu_SNMP *mc);
		static void add_other(McPdu_SNMP *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class MmPdu_SNMP_onPDU: public MmReference_More0 {
	static TypevsMcDict dict_;
	public:
		MmPdu_SNMP_onPDU(CSTR);
		virtual ~MmPdu_SNMP_onPDU();
		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}
		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}
		static void add(McPdu_SNMP *mc);
		static void add_other(McPdu_SNMP *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class MmSNMPVersion: public MmUint {
public:
	MmSNMPVersion(CSTR,uint16_t,const PObject* =0,const PObject* =0,
		const ICVoverwriter* ow=0,METH_HC_ForIPinfo meth=0);
	virtual	~MmSNMPVersion();
	virtual PvObject* reversePv(RControl&,
		const ItPosition& at,const ItPosition& size,const OCTBUF&)const;
};
////////////////////////////////////////////////////////////////
class MmObjectID: public MmVarOctets {
	public:
		MmObjectID(CSTR, const PObject *, const PObject *);
		virtual ~MmObjectID();
		virtual PvObject *reversePv(RControl &, const ItPosition &,
			const ItPosition &, const OCTBUF &) const;
};
////////////////////////////////////////////////////////////////
class MmSNMPString: public MmVarOctets {
	public:
		MmSNMPString(CSTR, const PObject *, const PObject *);
		virtual ~MmSNMPString();
		virtual PvObject *reversePv(RControl &, const ItPosition &,
			const ItPosition &, const OCTBUF &) const;
};
////////////////////////////////////////////////////////////////
class MmErrorStatus: public MmUint {
public:
	MmErrorStatus(CSTR,uint16_t,const PObject* =0,const PObject* =0,
		const ICVoverwriter* ow=0,METH_HC_ForIPinfo meth=0);
	virtual	~MmErrorStatus();
	virtual PvObject* reversePv(RControl&,
		const ItPosition& at,const ItPosition& size,const OCTBUF&)const;
};
////////////////////////////////////////////////////////////////
class PvSNMPVersion:public PvNumber {
public:
	PvSNMPVersion();
	PvSNMPVersion(int32_t);
	PvSNMPVersion(const PvNumber&);
virtual	~PvSNMPVersion();
virtual	void print() const;
};
////////////////////////////////////////////////////////////////
class PvObjectID: public PvOctets {
public:
	PvObjectID();
	PvObjectID(uint32_t, OCTSTR = 0, bool = false);
	virtual ~PvObjectID();
	virtual PvObject *shallowCopy() const;
	virtual void print() const;
	virtual void log(uint32_t=0) const;
	virtual void dump(CSTR = 0) const;
};
////////////////////////////////////////////////////////////////
class PvSNMPString: public PvOctets {
public:
	PvSNMPString();
	PvSNMPString(uint32_t, OCTSTR = 0, bool = false);
	virtual ~PvSNMPString();
	virtual PvObject *shallowCopy() const;
	virtual void print() const;
	virtual void log(uint32_t=0) const;
	virtual void dump(CSTR = 0) const;
};
////////////////////////////////////////////////////////////////
class PvErrorStatus:public PvNumber {
public:
	PvErrorStatus();
	PvErrorStatus(int32_t);
	virtual	~PvErrorStatus();
	virtual	void print() const;
};
#endif

#endif //NOT_USE_SNMP

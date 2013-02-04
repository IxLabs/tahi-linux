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

#if !defined(__McIGMP_h__)
#define	__McIGMP_h__	1

#include "McSub.h"
#include "MmHeader.h"

// for IGMPv3 Address Record
#include "MmObject.h"
#include "PvOctets.h"
#include "McObject.h"

//////////////////////////////////////////////////////////////////////////////
//	Upper IGMPv1	[RFC-1112]
//        IGMPv2	[RFC-2236]
//        IGMPv3	[RFC-3376]

const int32_t TP_IGMP_IGMPQuery	=17; // 0x11 Membership Query
const int32_t TP_IGMP_IGMPv1Report	=18; // 0x12 Version 1 Membership Report
const int32_t TP_IGMP_IGMPv2Report =22; // 0x16 Version 2 Membership Report
const int32_t TP_IGMP_IGMPv2LeaveGroup =23; // 0x17 Version 2 Leave Group
const int32_t TP_IGMP_IGMPv3Report	=34; // 0x22 Version 3 Membership Report

const int32_t TP_IGMPV3_MODE_IS_INCLUDE			= 1;
const int32_t TP_IGMPV3_MODE_IS_EXCLUDE			= 2;
const int32_t TP_IGMPV3_CHANGE_TO_INCLUDE_MODE	= 3;
const int32_t TP_IGMPV3_CHANGE_TO_EXCLUDE_MODE	= 4;
const int32_t TP_IGMPV3_ALLOW_NEW_SOURCES		= 5;
const int32_t TP_IGMPV3_BLOCK_OLD_SOURCES		= 6;

// meta IGMP only one (reverse upptype(one)->igmptype(any) McUpp_IGMP_*}
class McUpp_IGMP_ONE :public McUpper{
private:
static	McUpp_IGMP_ONE* instance_;
	McUpp_IGMP_ONE(CSTR);
public:
virtual	~McUpp_IGMP_ONE();
static	McUpp_IGMP_ONE* instance();
	int32_t headerType()const{return TP_Upp_IGMP;}
	bool containsMc(const MObject* mc) const;
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t length_for_reverse(RControl&,ItPosition& at,OCTBUF& buf) const;
virtual RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
};

//////////////////////////////////////////////////////////////////////////////
//	Header IGMP (AS Upper)
class McUpp_IGMP :public McHeader{
protected:
	MmUint*	type_;
	void	type_member(MmUint* meta){type_=meta; member(meta);}
	void	common_member();
	McUpp_IGMP(CSTR);
public:
virtual	~McUpp_IGMP();
virtual	int32_t token() const;
	int32_t headerType()const{return TP_Upp_IGMP;}
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t length_for_reverse(RControl&,ItPosition& at,OCTBUF& buf) const;
	bool overwrite_DictType(RControl&,ItPosition& at,OCTBUF& buf) const;
virtual	RObject* reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const;
virtual bool generate(WControl& c,WObject* w_self,OCTBUF& buf)const;
};

class McUpp_IGMP_ANY :public McUpp_IGMP{
public:
	McUpp_IGMP_ANY(CSTR);
virtual	~McUpp_IGMP_ANY();
static	McUpp_IGMP_ANY* create(CSTR);
};

class McUpp_IGMP_IGMPQuery :public McUpp_IGMP{
public:
	McUpp_IGMP_IGMPQuery(CSTR);
virtual	~McUpp_IGMP_IGMPQuery();
static	McUpp_IGMP_IGMPQuery* create(CSTR);
	int32_t igmpType()const{return TP_IGMP_IGMPQuery;}

	//HardCording action method
	DEC_HCGENE(NumOfSources);
	DEC_HC_MLC(SourceAddress);
};

class McUpp_IGMP_IGMPv3Report :public McUpp_IGMP{
public:
	McUpp_IGMP_IGMPv3Report(CSTR);
virtual	~McUpp_IGMP_IGMPv3Report();
static	McUpp_IGMP_IGMPv3Report* create(CSTR);
	int32_t igmpType()const{return TP_IGMP_IGMPv3Report;}
	
	//HardCording action method
	DEC_HCGENE(NumOfGroupAddrRecords);
};

class McUpp_IGMP_IGMPv1Report :public McUpp_IGMP{
public:
	McUpp_IGMP_IGMPv1Report(CSTR);
virtual	~McUpp_IGMP_IGMPv1Report();
static	McUpp_IGMP_IGMPv1Report* create(CSTR);
	int32_t igmpType()const{return TP_IGMP_IGMPv1Report;}
};

class McUpp_IGMP_IGMPv2Report :public McUpp_IGMP{
public:
	McUpp_IGMP_IGMPv2Report(CSTR);
virtual	~McUpp_IGMP_IGMPv2Report();
static	McUpp_IGMP_IGMPv2Report* create(CSTR);
	int32_t igmpType()const{return TP_IGMP_IGMPv2Report;}
};

class McUpp_IGMP_IGMPv2LeaveGroup :public McUpp_IGMP{
public:
	McUpp_IGMP_IGMPv2LeaveGroup(CSTR);
virtual	~McUpp_IGMP_IGMPv2LeaveGroup();
static	McUpp_IGMP_IGMPv2LeaveGroup* create(CSTR);
	int32_t igmpType()const{return TP_IGMP_IGMPv2LeaveGroup;}
};

//////////////////////////////////////////////////////////////////////////////
// header = xx (reference header on the McUpp_IGMP_ONE)
// reference particular tophdr (on McUpp_IGMP_ONE)
class MmHeader_onIGMP :public MmReference_Must1 {
static	TypevsMcDict	dict_; //igmpType vs McUpp_IGMP_XX
public:
	MmHeader_onIGMP(CSTR);
virtual ~MmHeader_onIGMP();
	int32_t token()const{return metaToken(tkn_header_ref_);}
	const TypevsMcDict* get_dict()const{return &dict_;}
static	void add(McUpp_IGMP* mc);
static	void add_other(McUpp_IGMP* mc);
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
	bool overwrite_DictType(RControl&,ItPosition& at,OCTBUF& buf)const;
};

//////////////////////////////////////////////////////////////////////////////
//	IGMPv3 Address Record
class McIGMPv3_AddrRecord: public McUpper {
	protected:
		MmUint *type_;
		void type_member(MmUint *meta) {
			type_ = meta;
			member(meta);
		}

		MmUint *auxdatalen_;
		void auxdatalen_member(MmUint *meta) {
			auxdatalen_ = meta;
			member(meta);
		}

		MmUint *numofsrc_;
		void numofsrc_member(MmUint *meta) {
			numofsrc_ = meta;
			member(meta);
		}
		
		MmV4Addr *mcastaddr_;
		void mcastaddr_member(MmV4Addr *meta) {
			mcastaddr_ = meta;
			member(meta);
		}
	public:
		McIGMPv3_AddrRecord(CSTR);
		~McIGMPv3_AddrRecord();

		uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
		
		//HardCording action method
		DEC_HCGENE(NumOfSources);
		DEC_HC_MLC(SourceAddress);
		
		RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
		
		static McIGMPv3_AddrRecord *create(CSTR);
};
//////////////////////////////////////////////////////////////////////////////
class MmIGMPv3AddrRecord_onIGMP:public MmReference_More0 {
static	TypevsMcDict	dict_;
public:
	MmIGMPv3AddrRecord_onIGMP(CSTR);
	~MmIGMPv3AddrRecord_onIGMP();
	int32_t token()const{return metaToken(tkn_option_ref_);}
	const TypevsMcDict* get_dict()const{return &dict_;}
static	void add(McIGMPv3_AddrRecord* mc);
static	void add_other(McIGMPv3_AddrRecord* mc);
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
	bool overwrite_DictType(RControl&,ItPosition& at,OCTBUF& buf)const;
};

#endif

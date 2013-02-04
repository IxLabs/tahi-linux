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
 * $TAHI: v6eval/lib/Pz/McIPv6.h,v 1.16 2002/12/02 12:31:29 akisada Exp $
 */
#if !defined(__McIPv6_h__)
#define	__McIPv6_h__	1

#include "McSub.h"
#include "MmHeader.h"

//////////////////////////////////////////////////////////////////////////////
// Network Layer IPv6	RFC2460

class McPacket_IPv6:public McPacket{
static	McPacket_IPv6*		instance_;
static	class McTopHdr_IPv6*	tophdr_;
	McPacket_IPv6(CSTR);
public:
virtual	~McPacket_IPv6();
static	McPacket_IPv6* create(CSTR key,CSTR tophdrkey);
static	McPacket_IPv6* instance(){return instance_;}
	int32_t protocolFamily()const{return PF_NL_IPv6;}
	int32_t protocolType()const{return TP_NL_IPv6;}
	int32_t headerType()const{return TP_Tunnel_IPv6;}
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t length_for_reverse(RControl&,ItPosition& at,OCTBUF& buf) const;
virtual RObject* reverseRc(RControl&,
		RObject* r_parent,const ItPosition& at,PvObject* pv) const;
virtual	WObject* composeWc(WControl&,WObject* w_parent,const PObject* pc) const;
virtual RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&)const;
virtual WObject* compose(WControl&,WObject* w_parent,const PObject* pc) const;
virtual bool generate(WControl& c,WObject* w_self,OCTBUF& buf) const;
// OVERWRITE ICV
virtual bool overwrite_ICV(ICVControl&,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const;
};

class McTopHdr_IPv6 :public McHeader{
friend class McPacket_IPv6;
	MmUint*		Layerlength_;
	void Layerlength_member(MmUint* meta){
			Layerlength_=meta; member(meta);}
	McTopHdr_IPv6(CSTR);
virtual	~McTopHdr_IPv6();
static	McTopHdr_IPv6* create(CSTR);
	int32_t protocolType()const{return TP_NL_IPv6;}
	int32_t headerType()const{return TP_Tunnel_IPv6;}
	uint32_t alignment_requirement() const;
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t Layerlength_for_reverse(RControl&,ItPosition&,OCTBUF&) const;
//HardCording action method
	DEC_HCGENE(PayloadLength);
	DEC_HCGENE(NextHeader);
	DEC_HCEVAL(NextHeader);
//HardCording for IPinfo setting
	DEC_HC_ForIPinfo(SourceAddress);
	DEC_HC_ForIPinfo(DestinationAddress);
// OVERWRITE ICV
virtual bool overwrite_ICV(ICVControl&,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const;
	DEC_HC_OWICV(SourceAddress);	// MIP6AH
	DEC_HC_OWICV(DestinationAddress);
};


//////////////////////////////////////////////////////////////////////////////
//	IPv6 Extention Header

class McHdr_Ext :public McHeader{
protected:
	MmUint*		length_;
	void length_member(MmUint* meta){
			length_=meta; member(meta);}
	McHdr_Ext(CSTR);
public:
virtual ~McHdr_Ext();
	uint32_t alignment_requirement() const;
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t length_for_reverse(RControl&,ItPosition& at,OCTBUF& buf) const;
//HardCording action method
	DEC_HCGENE(NextHeader);
	DEC_HCEVAL(NextHeader);
	DEC_HCGENE(HeaderExtLength);
// OVERWRITE ICV
virtual bool overwrite_ICV(ICVControl&,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const;
};

class McHdr_Ext_HopByHop :public McHdr_Ext{
	McHdr_Ext_HopByHop(CSTR);
public:
virtual ~McHdr_Ext_HopByHop();
static	McHdr_Ext_HopByHop* create(CSTR);
	int32_t headerType()const{return TP_Ext_HopByHop;}
};

class McHdr_Ext_Destination :public McHdr_Ext{
	McHdr_Ext_Destination(CSTR);
public:
virtual ~McHdr_Ext_Destination();
static	McHdr_Ext_Destination* create(CSTR);
	int32_t headerType()const{return TP_Ext_Destination;}
};

class McHdr_Ext_Routing :public McHdr_Ext{
	McHdr_Ext_Routing(CSTR);
public:
virtual ~McHdr_Ext_Routing();
static	McHdr_Ext_Routing* create(CSTR);
	int32_t headerType()const{return TP_Ext_Routing;}
//HardCording multiple count method
	DEC_HC_MLC(Address);
//HardCording for IPinfo setting
	DEC_HC_ForIPinfo(SegmentsLeft);
	DEC_HC_ForIPinfo(Address);
// OVERWRITE ICV
virtual bool overwrite_ICV(ICVControl&,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const;
	DEC_HC_OWICV(SegmentsLeft);
	DEC_HC_OWICV(Address);
};

class McHdr_Ext_Fragment :public McHdr_Ext{
	McHdr_Ext_Fragment(CSTR);
public:
virtual	~McHdr_Ext_Fragment();
static	McHdr_Ext_Fragment* create(CSTR);
	int32_t headerType()const{return TP_Ext_Fragment;}
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
// OVERWRITE ICV
virtual bool overwrite_ICV(ICVControl&,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const; //error
};

//////////////////////////////////////////////////////////////////////////////
//	Option for IPv6 Extention Header

//Option Type (Ext)
const int32_t TP_Opt_Ext_Pad1			=0;
const int32_t TP_Opt_Ext_PadN			=1;
const int32_t TP_Opt_Ext_TunnelEncap		=0x04;
const int32_t TP_Opt_Ext_RouterAlert		=0x05;
const int32_t TP_Opt_Ext_JumboPayload		=0xc2;

const int32_t TP_Opt_Ext_HomeAddress = 0xc9;	// 201


class McOpt_Ext :public McOption{
protected:
	MmUint*	type_;
	void	type_member(MmUint* meta){type_=meta; member(meta);}
	MmUint*	length_;
	void	length_member(MmUint* meta){length_=meta; member(meta);}
	void	common_member();
	McOpt_Ext(CSTR);
public:
virtual	~McOpt_Ext();
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t length_for_reverse(RControl&,ItPosition& at,OCTBUF& buf) const;
	bool overwrite_DictType(RControl&,ItPosition& at,OCTBUF& buf) const;
//HardCording action method
	DEC_HCGENE(OptDataLength);
// OVERWRITE ICV
virtual bool overwrite_ICV(ICVControl&,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const;
	DEC_HC_OWICV(OptExtData);
};

//any optionType Format (for unknown option type)
class McOpt_Ext_ANY :public McOpt_Ext{
	McOpt_Ext_ANY(CSTR);
public:
virtual	~McOpt_Ext_ANY();
static	McOpt_Ext_ANY* create(CSTR);
};

class McOpt_Ext_Pad1 :public McOpt_Ext{
	McOpt_Ext_Pad1(CSTR);
public:
virtual	~McOpt_Ext_Pad1();
static	McOpt_Ext_Pad1* create(CSTR);
	int32_t optionType()const{return TP_Opt_Ext_Pad1;}
virtual bool disused() const{return true;}	//disuse evaluate
};

class McOpt_Ext_PadN :public McOpt_Ext{
	McOpt_Ext_PadN(CSTR);
public:
virtual	~McOpt_Ext_PadN();
static	McOpt_Ext_PadN* create(CSTR);
	int32_t optionType()const{return TP_Opt_Ext_PadN;}
virtual bool disused() const{return true;}	//disuse evaluate
};

class McOpt_Ext_JumboPayload :public McOpt_Ext{
	McOpt_Ext_JumboPayload(CSTR);
public:
virtual	~McOpt_Ext_JumboPayload();
static	McOpt_Ext_JumboPayload* create(CSTR);
	int32_t optionType()const{return TP_Opt_Ext_JumboPayload;}
	uint32_t alignment_requirement() const;
//HardCording action method
	DEC_HCGENE(JumboPayloadLength);
};

class McOpt_Ext_RouterAlert :public McOpt_Ext{
	McOpt_Ext_RouterAlert(CSTR);
public:
virtual	~McOpt_Ext_RouterAlert();
static	McOpt_Ext_RouterAlert* create(CSTR);
	int32_t optionType()const{return TP_Opt_Ext_RouterAlert;}
	uint32_t alignment_requirement() const;
};

class McOpt_Ext_TunnelEncap :public McOpt_Ext{
	McOpt_Ext_TunnelEncap(CSTR);
public:
virtual	~McOpt_Ext_TunnelEncap();
static	McOpt_Ext_TunnelEncap* create(CSTR);
	int32_t optionType()const{return TP_Opt_Ext_TunnelEncap;}
	uint32_t alignment_requirement() const;
};

class McOpt_Ext_HomeAddress: public McOpt_Ext {
	McOpt_Ext_HomeAddress(CSTR);

	public:
		virtual	~McOpt_Ext_HomeAddress();
		static	McOpt_Ext_HomeAddress *create(CSTR);

		int32_t optionType() const {
			return(TP_Opt_Ext_HomeAddress);
		}
#if 0
		uint32_t alignment_requirement() const;
#endif
		DEC_HC_ForIPinfo(HomeAddress);
		virtual bool overwrite_ICV( ICVControl&, const ItPosition& at,OCTBUF& buf,const TObject* t) const; // MIP6AH
		DEC_HC_OWICV(HomeAddress);	// MIP6AH
};


//////////////////////////////////////////////////////////////////////////////
// option = xx (reference option(Ext) on the McHdr_Ext_XX)
class MmOption_onExt:public MmReference_More0 {
static	TypevsMcDict	dict_;	//optionType(Ext) vs McOption_ICMPv6_XX
public:
	MmOption_onExt(CSTR);
virtual	~MmOption_onExt();
	int32_t token()const{return metaToken(tkn_option_ref_);}
	const TypevsMcDict* get_dict()const{return &dict_;}
static	void add(McOpt_Ext* mc);
static	void add_other(McOpt_Ext* mc);
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
	bool overwrite_DictType(RControl&,ItPosition& at,OCTBUF& buf)const;
};

//////////////////////////////////////////////////////////////////////////////
#endif

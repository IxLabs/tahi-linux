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
 * $TAHI: v6eval/lib/Pz/McIPv4.h,v 1.7 2003/10/20 05:54:21 akisada Exp $
 */
#if !defined(__McIPv4_h__)
#define	__McIPv4_h__	1

#include "McSub.h"

//////////////////////////////////////////////////////////////////////////////
// Network Layer IPv4	RFC791

class McPacket_IPv4:public McPacket{
static	McPacket_IPv4*		instance_;
static	class McTopHdr_IPv4*	tophdr_;
	McPacket_IPv4(CSTR);
public:
virtual	~McPacket_IPv4();
static	McPacket_IPv4* create(CSTR key,CSTR tophdrkey);
static	McPacket_IPv4* instance(){return instance_;}
	int32_t protocolFamily()const{return PF_NL_IPv4;}
	int32_t protocolType()const{return TP_NL_IPv4;}
	int32_t headerType()const{return TP_Tunnel_IPv4;}
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

class McTopHdr_IPv4 :public McHeader{
friend class McPacket_IPv4;
	MmUint*		length_;
	void length_member(MmUint* meta){length_=meta; member(meta);}
	MmUint*		Layerlength_;
	void Layerlength_member(MmUint* meta){Layerlength_=meta; member(meta);}
	MmUint*		Flags_;
	void Flags_member(MmUint* meta){Flags_=meta; member(meta);}
	MmUint*		fragoffset_;
	void fragoffset_member(MmUint* meta){fragoffset_=meta; member(meta);}
	void nextType_set_Control(RControl& c,RObject* r_self)const;
	McTopHdr_IPv4(CSTR);
virtual	~McTopHdr_IPv4();
static	McTopHdr_IPv4* create(CSTR);
	int32_t protocolType()const{return TP_NL_IPv4;}
	int32_t headerType()const{return TP_Tunnel_IPv4;}
	uint32_t alignment_requirement() const;
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t length_for_reverse(RControl&,ItPosition& at,OCTBUF& buf) const;
virtual uint32_t Layerlength_for_reverse(RControl&,ItPosition&,OCTBUF&) const;
virtual RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&)const;
virtual bool generate(WControl& c,WObject* w_self,OCTBUF& buf) const;
//HardCording action method
	DEC_HCGENE(IHL);
	DEC_HCGENE(TotalLength);
	DEC_HCGENE(Protocol);
	DEC_HCEVAL(Protocol);
//HardCording for IPinfo setting
	DEC_HC_ForIPinfo(SourceAddress);
	DEC_HC_ForIPinfo(DestinationAddress);
// OVERWRITE ICV
virtual bool overwrite_ICV(ICVControl&,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const;
	DEC_HC_OWICV(DestinationAddress);
};


//////////////////////////////////////////////////////////////////////////////
//	Option for IPv4 Header

const int32_t TP_Opt_IPv4_EndofOptionList	= 0;
const int32_t TP_Opt_IPv4_NoOperation		= 1;
const int32_t TP_Opt_IPv4_LooseSourceRoute	= 131;
const int32_t TP_Opt_IPv4_StrictSourceRoute	= 137;
const int32_t TP_Opt_IPv4_RecordRoute		= 7;
const int32_t TP_Opt_IPv4_Timestamp		= 68;
const int32_t TP_Opt_IPv4_RouterAlert		= 148;


class McOpt_IPv4 : public McOption{
protected:
	MmUint*	type_;
	void	type_member(MmUint* meta){type_=meta; member(meta);}
	MmUint*	length_;
	void	length_member(MmUint* meta){length_=meta; member(meta);}
	void	common_member();
	bool	isZeroICV_;
	McOpt_IPv4(CSTR,bool isZeroICV);
public:
virtual	~McOpt_IPv4();
static	void	create_options();
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t length_for_reverse(RControl&,ItPosition& at,OCTBUF& buf) const;
	bool overwrite_DictType(RControl&,ItPosition& at,OCTBUF& buf) const;
//HardCording action method
	DEC_HCGENE(Length);
// OVERWRITE ICV
virtual bool overwrite_ICV(ICVControl&,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const;
};


//any optionType Format (for unknown option type)
class McOpt_IPv4_ANY :public McOpt_IPv4{
	McOpt_IPv4_ANY(CSTR,bool isZeroICV);
public:
virtual	~McOpt_IPv4_ANY();
static	McOpt_IPv4_ANY* create(CSTR);
};

class McOpt_IPv4_EndofOptionList :public McOpt_IPv4{
	McOpt_IPv4_EndofOptionList(CSTR,bool isZeroICV);
public:
virtual	~McOpt_IPv4_EndofOptionList();
static	McOpt_IPv4_EndofOptionList* create(CSTR);
	int32_t optionType()const{return TP_Opt_IPv4_EndofOptionList;}
virtual	bool disused() const{return true;}      //disuse evaluate
virtual RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&)const;
};

class McOpt_IPv4_NoOperation :public McOpt_IPv4{
	McOpt_IPv4_NoOperation(CSTR,bool isZeroICV);
public:
virtual	~McOpt_IPv4_NoOperation();
static	McOpt_IPv4_NoOperation* create(CSTR);
	int32_t optionType()const{return TP_Opt_IPv4_NoOperation;}
virtual	bool disused() const{return true;}      //disuse evaluate
};

class McOpt_IPv4_SourceRouteCom :public McOpt_IPv4{
protected:
	McOpt_IPv4_SourceRouteCom(CSTR key,bool isZeroICV);
public:
virtual	~McOpt_IPv4_SourceRouteCom();
	uint32_t alignment_requirement() const;
//HardCording multiple count method
        DEC_HC_MLC(RouteData);
//HardCording for IPinfo setting
	DEC_HC_ForIPinfo(Pointer);
	DEC_HC_ForIPinfo(RouteData);
};

template<int32_t TYPE>
class McOpt_IPv4_SourceRoute :public McOpt_IPv4_SourceRouteCom{
	McOpt_IPv4_SourceRoute(CSTR key,bool isZeroICV):
		McOpt_IPv4_SourceRouteCom(key,isZeroICV){}
public:
virtual	~McOpt_IPv4_SourceRoute(){}
	int32_t optionType()const{return TYPE;}
static	McOpt_IPv4_SourceRoute* create(CSTR);
};
typedef McOpt_IPv4_SourceRoute<TP_Opt_IPv4_LooseSourceRoute>
	McOpt_IPv4_LooseSourceRoute;
typedef McOpt_IPv4_SourceRoute<TP_Opt_IPv4_StrictSourceRoute>
	McOpt_IPv4_StrictSourceRoute;


class McOpt_IPv4_RecordRoute :public McOpt_IPv4{
	McOpt_IPv4_RecordRoute(CSTR,bool isZeroICV);
public:
virtual	~McOpt_IPv4_RecordRoute();
static	McOpt_IPv4_RecordRoute* create(CSTR);
	int32_t optionType()const{return TP_Opt_IPv4_RecordRoute;}
	uint32_t alignment_requirement() const;
//HardCording multiple count method
        DEC_HC_MLC(RouteData);
};

class McOpt_IPv4_Timestamp :public McOpt_IPv4{
	McOpt_IPv4_Timestamp(CSTR,bool isZeroICV);
public:
virtual	~McOpt_IPv4_Timestamp();
static	McOpt_IPv4_Timestamp* create(CSTR);
	int32_t optionType()const{return TP_Opt_IPv4_Timestamp;}
	uint32_t alignment_requirement() const;
//HardCording multiple count method
        DEC_HC_MLC(Timestamp);
};

class McOpt_IPv4_RouterAlert :public McOpt_IPv4{
	McOpt_IPv4_RouterAlert(CSTR,bool isZeroICV);
public:
virtual	~McOpt_IPv4_RouterAlert();
static	McOpt_IPv4_RouterAlert* create(CSTR);
	int32_t optionType()const{return TP_Opt_IPv4_RouterAlert;}
};

#include "MmHeader.h"
//////////////////////////////////////////////////////////////////////////////
// option = xx (reference option(IPv4) on the McTopHdr_IPv4)
class MmOption_onIPv4:public MmReference_More0 {
static	TypevsMcDict	dict_;  //optionType(IPv4) vs McOpt_IPv4_*
public:
	MmOption_onIPv4(CSTR);
virtual	~MmOption_onIPv4();
	int32_t token()const{return metaToken(tkn_option_ref_);}
	const TypevsMcDict* get_dict()const{return &dict_;}
static	void add(McOpt_IPv4* mc);
static	void add_other(McOpt_IPv4* mc);
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
	bool overwrite_DictType(RControl&,ItPosition& at,OCTBUF& buf) const;
};

#endif

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
 * $TAHI: v6eval/lib/Pz/McTCP.h,v 1.6 2001/10/12 04:56:15 tanaka Exp $
 */
#if !defined(__McTCP_h__)
#define	__McTCP_h__	1

#include "McSub.h"

//////////////////////////////////////////////////////////////////////////////
//	Upper TCP	RFC793

class McUpp_TCP :public McUpper{
static	McUpp_TCP*		instance_;
static	class McTopHdr_TCP*	tophdr_;
	McUpp_TCP(CSTR);
public:
virtual ~McUpp_TCP();
static	McUpp_TCP* create(CSTR key,CSTR tophdrkey);
static	McUpp_TCP* instance(){return instance_;}
	int32_t headerType()const{return TP_Upp_TCP;}
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t length_for_reverse(RControl&,ItPosition& at,OCTBUF& buf) const;
virtual RObject* reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const;
virtual bool generate(WControl& c,WObject* w_self,OCTBUF& buf)const;
};

class McTopHdr_TCP :public McHeader{
friend	class McUpp_TCP;
	MmUint*	length_;
	void	length_member(MmUint* meta){length_=meta; member(meta);}
	McTopHdr_TCP(CSTR);
virtual ~McTopHdr_TCP();
static	McTopHdr_TCP* create(CSTR);
	int32_t headerType()const{return TP_Upp_TCP;}
	uint32_t alignment_requirement() const;
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t length_for_reverse(RControl&,ItPosition& at,OCTBUF& buf) const;
virtual uint32_t Layerlength_for_reverse(RControl&,ItPosition&,OCTBUF&) const;
//HardCording action method
	DEC_HCGENE(DataOffset);
};


//////////////////////////////////////////////////////////////////////////////
//	Option for TCP Header

const int32_t TP_Opt_TCP_EndofOptionList	= 0;
const int32_t TP_Opt_TCP_NoOperation		= 1;
const int32_t TP_Opt_TCP_MaximumSegmentSize	= 2;


class McOpt_TCP : public McOption{
protected:
	MmUint*	kind_;
	void	kind_member(MmUint* meta){kind_=meta; member(meta);}
	MmUint*	length_;
	void	length_member(MmUint* meta){length_=meta; member(meta);}
	void	common_member();
	McOpt_TCP(CSTR);
public:
virtual	~McOpt_TCP();
static	void	create_options();
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t length_for_reverse(RControl&,ItPosition& at,OCTBUF& buf) const;
	bool overwrite_DictType(RControl&,ItPosition& at,OCTBUF& buf) const;
//HardCording action method
	DEC_HCGENE(Length);
};


//any optionType Format (for unknown option type)
class McOpt_TCP_ANY :public McOpt_TCP{
	McOpt_TCP_ANY(CSTR);
public:
virtual	~McOpt_TCP_ANY();
static	McOpt_TCP_ANY* create(CSTR);
};

class McOpt_TCP_EndofOptionList :public McOpt_TCP{
	McOpt_TCP_EndofOptionList(CSTR);
public:
virtual	~McOpt_TCP_EndofOptionList();
static	McOpt_TCP_EndofOptionList* create(CSTR);
	int32_t optionType()const{return TP_Opt_TCP_EndofOptionList;}
virtual	bool disused() const{return true;}      //disuse evaluate
virtual RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&)const;
};

class McOpt_TCP_NoOperation :public McOpt_TCP{
	McOpt_TCP_NoOperation(CSTR);
public:
virtual	~McOpt_TCP_NoOperation();
static	McOpt_TCP_NoOperation* create(CSTR);
	int32_t optionType()const{return TP_Opt_TCP_NoOperation;}
virtual	bool disused() const{return true;}      //disuse evaluate
};

class McOpt_TCP_MaximumSegmentSize :public McOpt_TCP{
	McOpt_TCP_MaximumSegmentSize(CSTR);
public:
virtual	~McOpt_TCP_MaximumSegmentSize();
static	McOpt_TCP_MaximumSegmentSize* create(CSTR);
	int32_t optionType()const{return TP_Opt_TCP_MaximumSegmentSize;}
	uint32_t alignment_requirement() const;
};

#include "MmHeader.h"
//////////////////////////////////////////////////////////////////////////////
// option = xx (reference option(TCP) on the McTopHdr_TCP)
class MmOption_onTCP:public MmReference_More0 {
static	TypevsMcDict	dict_;  //optionType(TCP) vs McOpt_TCP_*
public:
	MmOption_onTCP(CSTR);
virtual	~MmOption_onTCP();
	int32_t token()const{return metaToken(tkn_option_ref_);}
	const TypevsMcDict* get_dict()const{return &dict_;}
static	void add(McOpt_TCP* mc);
static	void add_other(McOpt_TCP* mc);
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
	bool overwrite_DictType(RControl&,ItPosition& at,OCTBUF& buf) const;
};

//////////////////////////////////////////////////////////////////////////////
#endif

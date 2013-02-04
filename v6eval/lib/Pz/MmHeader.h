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
 * $TAHI: v6eval/lib/Pz/MmHeader.h,v 1.28 2002/12/02 12:31:30 akisada Exp $
 */
#if !defined(__MmHeader_h__)
#define	__MmHeader_h__	1
#include "MmObject.h"

//////////////////////////////////////////////////////////////////////////////
//abstruct class for Reference Member must 1
class MmReference_Must1 : public MmObject{
protected:
	bool	keepType_;	//keep nexttype from overwrite_DictType
	MmReference_Must1(CSTR key,bool keepType=false):
			MmObject(key),keepType_(keepType){}
public:
virtual ~MmReference_Must1(){}
	bool containsMc(const MObject* mc) const;
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual WObject* compose(WControl&,WObject* w_parent,const PObject* pl) const;
virtual RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
// COMPOSE/REVERSE PROTECTED MmReference_Must1* -------------------------------
protected:
virtual void composeList(WControl&,
		WObject* w_parent,const PObjectList& pls) const;
virtual bool overwrite_DictType(RControl&,ItPosition&,OCTBUF&) const{
			return true;}
	void find_error(RControl& c)const;
};

//////////////////////////////////////////////////////////////////////////////
//abstruct class for Reference Members more than 0
class MmReference_More0 : public MmObject{
protected:
	bool	keepType_;	//keep nexttype from overwrite_DictType
	MmReference_More0(CSTR key,bool keepType=false):
			MmObject(key),keepType_(keepType){}
public:
virtual ~MmReference_More0(){}
	bool containsMc(const MObject* mc) const;
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual WObject* compose(WControl&,WObject* w_parent,const PObject* pl) const;
virtual RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
// COMPOSE/REVERSE PROTECTED MmReference_More0* -------------------------------
protected:
virtual void composeList(WControl&,
		WObject* w_parent,const PObjectList& pls) const;
virtual bool overwrite_DictType(RControl&,ItPosition&,OCTBUF&) const{
			return true;}
};


//////////////////////////////////////////////////////////////////////////////
// frame = xx (reference frame on the TOP Layer)   *** now no use **
class MmFrame:public MmReference_Must1 {
static	TypevsMcDict	dict_;	//DataLinkLayerType vs McFrame_XX
public:				//other vs McPayload
	MmFrame(CSTR);
virtual	~MmFrame();
	int32_t token()const{return metaToken(tkn_packet_ref_);}
	const TypevsMcDict* get_dict()const{return &dict_;}
static	void add(McObject* mc);
static	void add_other(McObject* mc);
};

//////////////////////////////////////////////////////////////////////////////
// packet = xx (reference packet on the Layer McFrame_Ether)
class MmPacket_onEther:public MmReference_Must1 {
static	TypevsMcDict	dict_;	//protocolType vs McPacket_XX
public:				//other vs McPayload
	MmPacket_onEther(CSTR);
virtual	~MmPacket_onEther();
	int32_t token()const{return metaToken(tkn_packet_ref_);}
	const TypevsMcDict* get_dict()const{return &dict_;}
static	void add(McObject* mc);
static	void add_other(McObject* mc);
};
//////////////////////////////////////////////////////////////////////////////
// packet = xx (reference packet on the Layer McFrame_Null)
class MmPacket_onNull:public MmReference_Must1 {
static	TypevsMcDict	dict_;	//protocolFamily vs McPacket_XX
public:				//other vs McPayload
	MmPacket_onNull(CSTR);
virtual	~MmPacket_onNull();
	int32_t token()const{return metaToken(tkn_packet_ref_);}
	const TypevsMcDict* get_dict()const{return &dict_;}
static	void add(McObject* mc);
static	void add_other(McObject* mc);
};

//////////////////////////////////////////////////////////////////////////////
// header = xx (one McTopHdr_XX on the Layer Mc{Frame,Packet,Upp}_XX)
class MmTopHdr: public MmReference_Must1 {
	TypevsMcDict	dict_; // only one McTopHdr_XX
public:
	MmTopHdr(CSTR key,McObject* tophdr);
virtual	~MmTopHdr();
	int32_t token()const{return metaToken(tkn_header_ref_);}
	const TypevsMcDict* get_dict()const{return &dict_;}
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
	bool overwrite_DictType(RControl&,ItPosition& at,OCTBUF& buf)const;
};

//////////////////////////////////////////////////////////////////////////////
// exthdr = xx (reference exthdr on the McPacket_IP
class MmExtent_onIP :public MmReference_More0 {
static	TypevsMcDict	dict_;	//headerType vs McHdr_Ext_XX
public:
	MmExtent_onIP(CSTR);
virtual	~MmExtent_onIP();
	int32_t token()const{return metaToken(tkn_extent_ref_);}
	const TypevsMcDict* get_dict()const{return &dict_;}
static	void add(McObject* mc);
static	void add_other(McObject* mc);
};

//////////////////////////////////////////////////////////////////////////////
// upper = xx (reference upper on the Layer McPacket_IP)
class MmUpper_onIP:public MmReference_Must1 {
static	TypevsMcDict	dict_;	//headerType vs McUpp_XX,McPacket_XX(tunnel)
public:				//other vs McPayload
	MmUpper_onIP(CSTR);
virtual	~MmUpper_onIP();
	int32_t token()const{return metaToken(tkn_upper_ref_);}
	const TypevsMcDict* get_dict()const{return &dict_;}
static	void add(McObject* mc);
static	void add_other(McObject* mc);
};

////////////////////////////////////////////////////////////////
class MmUpper_onUpper: public MmReference_Must1 {
	static TypevsMcDict dict_;
	const McObject *tophdr_;

	public:	
		MmUpper_onUpper(CSTR, McObject *);
		virtual	~MmUpper_onUpper();

		int32_t token() const{
			return(metaToken(tkn_payload_ref_));	// for forward compatibility
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add(McObject *mc);
		static void add_dhcp(McObject *mc);
		static void add_other(McObject *mc);

		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
		const McObject *get_tophdr() const;
};

inline const McObject *MmUpper_onUpper::get_tophdr() const {
	return(tophdr_);
}

//////////////////////////////////////////////////////////////////////////////
// payload = xx (reference payload on the any McXX)
class MmPayload:public MmReference_Must1 {
static TypevsMcDict	dict_;	//only McPayload
public:
	MmPayload(CSTR);
virtual	~MmPayload();
	int32_t token() const{return metaToken(tkn_payload_ref_);}
	const TypevsMcDict* get_dict()const{return &dict_;}
static	void add_other(McObject* mc);
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
	bool overwrite_DictType(RControl&,ItPosition& at,OCTBUF& buf)const;
};

////////////////////////////////////////////////////////////////
class MmReference_Less1: public MmReference_Must1 {
	public:
		MmReference_Less1(CSTR key, bool keepType = false): MmReference_Must1(key, keepType) {};
		virtual ~MmReference_Less1() {};
		virtual WObject *compose(WControl &, WObject *, const PObject *) const;
};
#endif

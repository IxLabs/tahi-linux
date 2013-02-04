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
 * $TAHI: v6eval/lib/Pz/McObject.h,v 1.55 2005/05/09 09:35:24 akisada Exp $
 */
#if !defined(__McObject_h__)
#define	__McObject_h__	1
#include "CmToken.h"
#include "pzCommon.h"

class RControl;
class WControl;
class TObject;
class RObject;
class WObject;
class PObject;
class PvNumber;
class PvOctets;
class PvObject;
class PaObject;
class McObject;
class DmObject;
class ItPosition;
class PObjectList;
class TypevsMcDict;
class Con_IPinfo;
class ICVControl;
//======================================================================
class MObject:public CmToken {
private:
	DmObject* describer_;
public:
	MObject(CSTR);
virtual	~MObject();
virtual	void postProcess(void*);
virtual void stepPostion(ItPosition&) const;
virtual	McObject* compound() const;
virtual	void compound(MObject*);
virtual	uint32_t objectLength(const PObject* =0,const WObject* =0) const;
virtual	uint16_t width() const;
virtual	MObject* findMember(CSTR) const;
//----------------------------------------------------------------------
virtual PObject* tokenObject(int=0,CSTR=0) const;
//----------------------------------------------------------------------
virtual	bool generate(WControl&,WObject*,OCTBUF&) const;
	bool generate_child(WControl&,WObject*,OCTBUF&) const;
virtual	bool geneAuto(WControl&,WObject*,OCTBUF&) const;
virtual	bool geneFix(WControl&,WObject*,OCTBUF&) const;
virtual	bool encodeNumber(WControl&,const ItPosition&,OCTBUF&,const PvNumber&) const;
virtual	bool encodeOctets(WControl&,const ItPosition&,OCTBUF&,const PvOctets&) const;
//----------------------------------------------------------------------
virtual PvObject* geneAutoValue(WObject*) const;
virtual PvObject* evalAutoValue(WObject*) const;
virtual	bool disused() const;	//disuse evaluate
virtual	bool isEqual(const MObject*) const;
virtual bool containsMc(const MObject*) const;
virtual	bool comparableMeta(const MObject*) const;
//for local ------------------------------------------------------------
virtual	int32_t DataLinkLayer_Type() const;
virtual	int32_t protocolFamily() const;
virtual	int32_t protocolType() const;
virtual	int32_t headerType() const;
virtual	int32_t optionType() const;
virtual	int32_t optionCode() const;
virtual	int32_t icmpv6Type() const;
virtual	int32_t mobilityType() const;
virtual	int32_t vrrpVersion() const;
virtual	bool ISAKMP_encryption() const;
virtual	int32_t rrType() const;
virtual	int32_t hardwareType() const;
virtual	int32_t authProto() const;
virtual	int32_t icmpv4Type() const;
virtual	int32_t igmpType() const;
virtual	int32_t upperPort() const;
virtual	int32_t dhcpv6Type() const;
virtual	int32_t DOIvalue() const;
virtual	int32_t notify_value() const;
virtual	int32_t ProtocolID() const;
virtual	int32_t IDtype() const;
virtual	int32_t SITvalue() const;
virtual	int32_t duidType() const;
virtual	int32_t pduType() const;
virtual const TypevsMcDict* get_dict() const;
virtual uint32_t alignment_requirement() const;//1,2,4,8,0(no alignment)
static	uint32_t padcount_alignment_requirement(
		uint32_t X/*alignment*/,uint32_t H/*offset*/,uint32_t S/*size*/);
virtual	bool icvUpdate(const PaObject&,PvOctets*) const;
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual	WObject* compose(WControl&,WObject* w_parent,const PObject* po) const;
virtual	RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
virtual	RObject* rreverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
// COMPOSE/REVERSE INTERFACE(for MmObject) ------------------------------------
protected:
	friend class McObject;
	friend class MmDecorator;
virtual	void composeList(WControl&,
		WObject* w_parent,const PObjectList& pls) const;
//----------------------------------------------------------------------
public:
	bool check_decode_limit(const MObject* m_parent,
		const ItPosition& at,OCTBUF& buf,const ItPosition& size) const;
	bool check_decode_alignment_requirement(const MObject* m_parent,
		const ItPosition& offset,const ItPosition& size) const;
	bool check_decode_alignment_requirement(const MObject* m_parent,
	const ItPosition& offset,const ItPosition& size,uint32_t ali) const;
//----------------------------------------------------------------------
// OVERWRITE ICV INTERFACE
virtual	bool overwrite_ICV(ICVControl&,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const;
// describe interface
public:
inline	DmObject* describer() const;
inline	void describer(DmObject*);
virtual	void describe(const PObject*) const;
protected:
friend class ApplicationLexer;
enum metaToken_ {
	tkn_frame_,
	tkn_packet_,
	tkn_header_,
	tkn_upper_,
	tkn_isakmp_upper_,
	tkn_option_,
	tkn_payload_,
	tkn_data_,
	tkn_member_,
	tkn_function_,
	tkn_padfunc_,
	tkn_cryptfunc_,
	tkn_ike_cryptfunc_,
	tkn_authfunc_,
	tkn_packet_ref_,
	tkn_header_ref_,
	tkn_extent_ref_,
	tkn_upper_ref_,
	tkn_option_ref_,
	tkn_isakmp_encryption_ref_,
	tkn_payload_ref_,
	tkn_dummy_,
//
	tkn_algorithm_,
	tkn_algomem_pad_,
	tkn_algomem_crypt_,
	tkn_algomem_auth_,
	tkn_algofunc_,
	tkn_algorithm_ref_,
	tkn_bsafunc_,
	tkn_ike_p2_hash_func_,
	tkn_dhcpauthfunc_,
	tkn_dns_header_,
	tkn_dns_qestion_ref_,
	tkn_dns_answer_ref_,
	tkn_dns_authority_ref_,
	tkn_dns_additional_ref_,
	maxMetaToken_	// maxMetaToken_ MUST be the last
};
static	int32_t tokenArray_[maxMetaToken_];
static	int32_t metaToken(metaToken_);
};

inline McObject* MObject::compound() const {return 0;}
inline bool MObject::disused() const {return false;}
inline MObject* MObject::findMember(CSTR str) const{
	MObject tmp(str);
	return isEqual(&tmp)?(MObject*)this:0;}
inline bool MObject::isEqual(const MObject* v) const {
	return CmToken::isEqual(v);}
inline bool MObject::containsMc(const MObject*) const {return false;}
inline bool MObject::comparableMeta(const MObject* m) const {
	return this==m;}
inline int32_t MObject::DataLinkLayer_Type() const {return -1;}
inline int32_t MObject::protocolFamily() const {return -1;}
inline int32_t MObject::protocolType() const {return -1;}
inline int32_t MObject::headerType() const {return -1;}
inline int32_t MObject::optionType() const {return -1;}
inline int32_t MObject::optionCode() const {return -1;}
inline int32_t MObject::icmpv6Type() const {return -1;}
inline int32_t MObject::mobilityType() const {return -1;}
inline int32_t MObject::vrrpVersion() const {return -1;}
inline bool MObject::ISAKMP_encryption() const {return false;}
inline int32_t MObject::rrType() const {return -1;}
inline int32_t MObject::hardwareType() const {return -1;}
inline int32_t MObject::authProto() const {return -1;}
inline int32_t MObject::icmpv4Type() const {return -1;}
inline int32_t MObject::igmpType() const {return -1;}
inline int32_t MObject::upperPort() const {return -1;}
inline int32_t MObject::dhcpv6Type() const {return -1;}
inline int32_t MObject::DOIvalue() const {return -1;}
inline int32_t MObject::notify_value() const {return -1;}
inline int32_t MObject::ProtocolID() const {return -1;}
inline int32_t MObject::IDtype() const {return -1;}
inline int32_t MObject::SITvalue() const {return -1;}
inline int32_t MObject::duidType() const {return -1;}
inline int32_t MObject::pduType() const {return -1;}
inline const TypevsMcDict* MObject::get_dict() const {return 0;}
inline uint32_t MObject::alignment_requirement() const {return 0;}
inline uint32_t MObject::padcount_alignment_requirement(
				uint32_t X,uint32_t H,uint32_t S){
	return !X ? 0 : (X-(H+S)%X)%X ;}
inline int32_t MObject::metaToken(metaToken_ v) {return tokenArray_[v];}
inline DmObject* MObject::describer() const {return describer_;}
inline void MObject::describer(DmObject* dm) {describer_=dm;}

#include "CmCltn.h"
interfaceCmSet(MObjectSet,MObject);
interfaceCmList(MObjectList,MObject);

//======================================================================
class MvKeyword:public MObject {
private:
	int32_t token_;
public:
	MvKeyword(CSTR,int32_t);
virtual	~MvKeyword();
virtual	int32_t token() const;
};
inline int32_t MvKeyword::token() const {return token_;}

#include "ItPosition.h"
class WObject;
class PObjectList;
//======================================================================
class McObject:public MObject {
private:
static	MObjectSet set_;
protected:
	ItPosition size_;
	MObjectList members_;
public:
static	McObject* initialize();
	McObject(CSTR);
virtual	~McObject();
virtual PObject* tokenObject(int,CSTR) const;
virtual	void member(MObject*);
virtual	MObject* findMember(CSTR) const;
static	McObject* find(CSTR);
static	MObject* findClassMember(CSTR,CSTR);
virtual	McObject* confirmed();
virtual bool containsMc(const MObject* mc) const{return mc==this;}
virtual void print() const;
static	McObject* addCompound(McObject* mc);
// GENERATE INTERFACE ---------------------------------------------------------
virtual	bool generate(WControl&,WObject*,OCTBUF&) const;
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual	WObject* compose(WControl&,WObject* w_parent,const PObject* pc) const;
virtual	RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
// COMPOSE/REVERSE PROTECTED Mc* ----------------------------------------------
protected:
virtual	WObject* composeWc(WControl&,WObject* w_parent,const PObject* pc) const;
virtual RObject* reverseRc(RControl&,
		RObject* r_parent,const ItPosition& at,PvObject* pv) const;
	void members_compose(WControl&,
		WObject* w_self,const PObjectList& pls)const;
	void members_reverse(RControl&,
		RObject* r_self,ItPosition& memat,OCTBUF& buf_self) const;
virtual	uint32_t length_for_reverse(RControl&,ItPosition&,OCTBUF&) const;
virtual	uint32_t Layerlength_for_reverse(RControl&,ItPosition&,OCTBUF&) const;
};
//////////////////////////////////////////////////////////////////////////////
//define macro declare Hard Cording Gene/Eval for auto

// Mc HardCording auto generater
#define HCGENE(membername)	name2(HCgene_,membername)
#define DEC_HCGENE(membername)	\
bool HCGENE(membername)(WControl& cntr,WObject* wmem,OCTBUF& buf)const
typedef bool (McObject::*HCgenefunc)(WControl&,WObject*,OCTBUF&) const;

// Mc HardCording auto evaluater
#define HCEVAL(membername)	name2(HCeval_,membername)
#define DEC_HCEVAL(membername)	\
PObject* HCEVAL(membername)(WObject* wmem)const
typedef PvObject* (McObject::*HCevalfunc)(WObject*) const;

// Mc HardCording Multiple count
#define HC_MLC(membername)	name2(HCmlc_,membername)
#define DEC_HC_MLC(membername)	\
uint32_t HC_MLC(membername)(const ItPosition& at,OCTBUF& buf)const
typedef uint32_t (McObject::*METH_HC_MLC)(const ItPosition& at,OCTBUF& buf)const;

// Mc HardCording for IPinfo
#define HC_ForIPinfo(membername)	name2(HCForIPinfo_,membername)
#define DEC_HC_ForIPinfo(membername)	\
void HC_ForIPinfo(membername)(Con_IPinfo& info,TObject* tmem) const
typedef void (McObject::*METH_HC_ForIPinfo)(Con_IPinfo&,TObject*) const;

// Mc HardCording overwrite ICV
#define HC_OWICV(membername)	name2(HCoverwriteICV_,membername)
#define DEC_HC_OWICV(membername)	\
bool HC_OWICV(membername)(ICVControl&,const ItPosition&,OCTBUF& buf,const TObject*)const
typedef bool (McObject::*METH_HC_OWICV)(ICVControl&,
		const ItPosition&,OCTBUF& buf,const TObject*)const;

//======================================================================
// Needless data ( receive size is more than need size)
class McNeedless:public McObject {
private:
static	McNeedless* instance_;
	McNeedless(CSTR);
public:
virtual	~McNeedless();
static	McNeedless* instance();
virtual	bool disused() const{return true;}	//disuse evaluate
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t length_for_reverse(RControl&,ItPosition&,OCTBUF&) const;
virtual	RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
};

///////////////////////////////////////////////////////////////////////////
// DictType VS McObject

struct TypevsMc{
	int32_t		type_;
	MObject*	Mc_;
	TypevsMc();
	TypevsMc(int32_t type,MObject* mc);
	bool	isEqual(const TypevsMc* e)const{return this==e;}
	int32_t	type()const{return type_;}
	MObject* Mc()const{return Mc_;}
};
interfaceCmList(TypevsMcList,TypevsMc);

class Con_DictType;
class TypevsMcDict{
	TypevsMcList	list_;
	MObject*	other_;
public:
	TypevsMcDict();
	~TypevsMcDict();
	bool isEqual(const TypevsMcDict* e) const;
	//
	MObject*	find(const Con_DictType& DictType)const;
	MObject*	find_other()const;
	bool		contains(const MObject* mc)const;
	bool check(const Con_DictType &) const;
	//
	void		add(int32_t type,MObject* mc);
	void		add_other(MObject* mc);
};
#endif

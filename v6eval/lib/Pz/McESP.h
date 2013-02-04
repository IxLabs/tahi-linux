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
 * $TAHI: v6eval/lib/Pz/McESP.h,v 1.12 2005/05/09 09:35:23 akisada Exp $
 */
#if !defined(__McESP_h__)
#define	__McESP_h__	1

#include "McIPv6.h"

//////////////////////////////////////////////////////////////////////////////
// IPsec ESP(Encapsulating Security Payload) Extention Header	RFC2406

class McHdr_Ext_ESP : public McHdr_Ext{
protected:
	McHdr_Ext_ESP(CSTR);
public:
static	McHdr_Ext_ESP* create(CSTR);
virtual ~McHdr_Ext_ESP();
	int32_t headerType()const{return TP_Ext_ESP;}
	uint32_t alignment_requirement() const;
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t length_for_reverse(RControl&,ItPosition& at,OCTBUF& buf) const;
virtual RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
virtual RObject* reverseRc(RControl&,
		RObject* r_parent,const ItPosition& at,PvObject* pv) const;
virtual WObject* compose(WControl& c,WObject* w_parent,const PObject* pc) const;
virtual WObject* composeWc(WControl&,WObject* w_parent,const PObject* pc) const;
// OVERWRITE ICV
virtual bool overwrite_ICV(ICVControl&,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const;
};

#include "MmObject.h"
#include "WObject.h"
#include "RObject.h"

//////////////////////////////////////////////////////////////////////////////
#include "MmHeader.h"
class MmAlgorithm_onESP : public MmReference_Must1{
public:
	MmAlgorithm_onESP(CSTR);
virtual ~MmAlgorithm_onESP();
	int32_t token()const{return metaToken(tkn_algorithm_ref_);}
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual	RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
virtual bool disused() const;
};

//////////////////////////////////////////////////////////////////////////////
class MmESP_Crypted : public MmVarOctets{ //unlex,variable octets member
	class McESP_Decrypted*	mDecrypted_;
	class MmESP_IVEC*	mIVEC_;
public:
	MmESP_Crypted(CSTR str);
virtual ~MmESP_Crypted();
virtual MObject* findMember(CSTR str) const;
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual ItPosition length_for_reverse(RControl&,ItPosition&,OCTBUF& buf)const;
virtual RObject* reverseRm(RControl& c,RObject* r_parent,
                const ItPosition& at,const ItPosition& size,PvObject* pv)const;
virtual void	composeList(WControl& c,
			WObject* w_parent,const PObjectList& pls)const;
};

class PaObject;
//-----------------------------------------------------------------------------
class WmESP_Crypted : public WmObject{
	const PaObject*	sa_;
	WObject*	wDecrypted_;
public:
	WmESP_Crypted(WObject* p,const MObject* m,const PaObject* sa);
virtual ~WmESP_Crypted();
	void set_wDecrypted(WObject* w){wDecrypted_=w;}
	WObject* wDecrypted()const{return wDecrypted_;}
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
	void do_adopt_afterESP();
// GENERATE/EVALUATE INTERFACE ------------------------------------------------
virtual void internalGeneratePrologue(ItPosition& it);
virtual bool generate(WControl& c,OCTBUF& s);
virtual	void internalEvaluatePrologue(ItPosition& it);
virtual bool doEvaluate(WControl& c,RObject& r);
virtual void printChild(uint32_t) const;
virtual	void describeChild(uint32_t) const;
virtual	const TObject* corresponding(const MObject*) const;
};
//-----------------------------------------------------------------------------
class RmESP_Crypted : public RmObject{
	RObject*	rDecrypted_;
	RObject*	rIVEC_; //for log
public:
	RmESP_Crypted(RObject* r_parent,const MObject* m,
		const ItPosition& offset,const ItPosition& size,PvObject* pv);
virtual ~RmESP_Crypted();
// LOG PRINT
virtual void print(uint32_t) const;
virtual void log(uint32_t) const;
virtual void printChild(uint32_t t) const;
virtual void logChild(uint32_t t) const;
virtual	void describeChild(uint32_t) const;
	void set_rDecrypted(RObject* r){rDecrypted_=r;}
	RObject* rDecrypted()const{return rDecrypted_;}
	void set_rIVEC(RObject* r){rIVEC_=r;}
	RObject* rIVEC()const{return rIVEC_;}
virtual	const TObject* corresponding(const MObject*) const;
};


//////////////////////////////////////////////////////////////////////////////
class MmESP_ICV : public MmVarOctets{ //unlex,variable octets member
public:
	MmESP_ICV(CSTR str);
virtual ~MmESP_ICV();
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual ItPosition length_for_reverse(RControl&,ItPosition&,OCTBUF& buf)const;
virtual RObject* reverseRm(RControl& c,RObject* r_parent,
		const ItPosition& at,const ItPosition& size,PvObject* pv)const;
virtual WObject* composeWm(WControl& c,
		WObject* w_parent,const PObject* pl)const;
virtual	RObject* reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const;
virtual bool geneAuto(WControl& c,WObject* w_self,OCTBUF& buf)const;
};


//////////////////////////////////////////////////////////////////////////////
class MmESP_IVEC : public MmVarOctets{ //unlex,variable octets member for log
public:
	MmESP_IVEC(CSTR key):MmVarOctets(key){}
virtual	~MmESP_IVEC(){}
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
	RObject* Self_reverse(RControl& c,OCTBUF* selfivec)const;
};


//////////////////////////////////////////////////////////////////////////////
class McESP_Decrypted : public McObject{//Decrypted field compound
friend class MmESP_Crypted;
	MmUint*		NextHeader_;
	MmUint*		PadLength_;
	McESP_Decrypted(CSTR str);
static	McESP_Decrypted* create(CSTR);
virtual ~McESP_Decrypted();
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
	RObject* Self_reverse(RControl& c,OCTBUF* pv)const;
	WObject* Self_composeList(WControl& c,const PObjectList& pls)const;
//HardCording action method
	DEC_HCGENE(PadLength);
	DEC_HCGENE(NextHeader);
	DEC_HCEVAL(NextHeader);
	int32_t get_next_headerType(WObject* wmem)const;
};


//////////////////////////////////////////////////////////////////////////////
class McESP_Payload : public McObject{//Decrypted ESP Payload field compound
friend class McESP_Decrypted;
	McESP_Payload(CSTR str);
static	McESP_Payload* create(CSTR);
virtual ~McESP_Payload();
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual uint32_t length_for_reverse(RControl& c,ItPosition& at,OCTBUF&)const;
virtual RObject* reverse(RControl& c,
		RObject* r_parent,ItPosition& at,OCTBUF& buf)const;
virtual void composeList(WControl& c,
		WObject* w_parent,const PObjectList& pls) const;
};


//////////////////////////////////////////////////////////////////////////////
class MmESP_Padding : public MmVarOctets{//unlex,variable octets member
public:
	MmESP_Padding(CSTR key);
virtual ~MmESP_Padding();
virtual bool disused() const{return true;}
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual ItPosition length_for_reverse(RControl& c,ItPosition& at,OCTBUF&)const;
virtual WObject* composeWm(WControl& c,
		WObject* w_parent,const PObject* pl)const;
};

//-----------------------------------------------------------------------------
class WmESP_Padding : public WmObject{
	const PaObject*	sa_;
public:
	WmESP_Padding(WObject* p,const MObject* m,const PaObject* sa);
virtual ~WmESP_Padding();
// GENERATE/EVALUATE INTERFACE ------------------------------------------------
virtual void internalGeneratePrologue(ItPosition& it);
};

//////////////////////////////////////////////////////////////////////////////

class WcESP:public WcObject {
private:
        const PaObject* algorithm_;
public:
        WcESP(WObject*,const MObject*,const PObject* =0);
virtual ~WcESP();
virtual void algorithm(const PaObject*);
virtual void printChild(uint32_t) const;
};

class RcESP:public RcObject {
private:
        const PaObject* algorithm_;
public:
        RcESP(RObject*,const MObject*,const ItPosition& o,PvObject* =0);
virtual ~RcESP();
virtual void algorithm(const PaObject*);
virtual void printChild(uint32_t) const;
};
#endif

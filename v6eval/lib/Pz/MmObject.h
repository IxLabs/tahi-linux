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
 * $TAHI: v6eval/lib/Pz/MmObject.h,v 1.52 2005/05/09 09:35:24 akisada Exp $
 */
#if !defined(__MmObject_h__)
#define	__MmObject_h__	1
#include "pzCommon.h"
#include "McObject.h"
class ItPosition;
class WObject;
class PObject;
class WControl;
class PvOctets;
class ICVoverwriter;
//======================================================================
// META MEMBER OBJECT
class MmObject:public MObject {
private:
	McObject* compound_;
public:
	MmObject(CSTR);
virtual	~MmObject();
virtual	int32_t token() const;
virtual	PObject* tokenObject(int,CSTR) const;
virtual	void compound(MObject*);
virtual	McObject* compound() const;
};

//======================================================================
// META MEMBER DECORATOR OBJECT (message sending decorator MmObject)
class MmDecorator:public MmObject {
private:
	MmObject* body_;
protected:
	MmObject* body()const{return body_;}
public:
	MmDecorator(MmObject* body);
virtual	~MmDecorator();
	void set_body(MmObject* body);
	const MmObject* get_body()const{return body_;}
//----------------------------------------------------------------------
virtual void print() const;
virtual int32_t token() const;
virtual PObject* tokenObject(int=0,CSTR=0) const;
//----------------------------------------------------------------------
virtual	void postProcess(void*);
virtual void stepPostion(ItPosition&) const;
virtual	McObject* compound() const;
virtual	void compound(MObject*);
virtual	uint32_t objectLength(const PObject* =0,const WObject* =0) const;
virtual	uint16_t width() const;
virtual	MObject* findMember(CSTR) const;
//----------------------------------------------------------------------
virtual	bool generate(WControl&,WObject*,OCTBUF&) const;
virtual	bool geneAuto(WControl&,WObject*,OCTBUF&) const;
virtual	bool geneFix(WControl&,WObject*,OCTBUF&) const;
virtual	bool encodeNumber(WControl&,const ItPosition&,OCTBUF&,const PvNumber&) const;
virtual	bool encodeOctets(WControl&,const ItPosition&,OCTBUF&,const PvOctets&) const;
virtual PvObject* geneAutoValue(WObject*) const;
virtual PvObject* evalAutoValue(WObject*) const;
virtual bool overwrite_ICV(ICVControl& c,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const;
//----------------------------------------------------------------------
virtual	bool disused() const;	//disuse evaluate
virtual	bool isEqual(const MObject*) const;
virtual bool containsMc(const MObject*) const;
virtual	bool comparableMeta(const MObject*) const;
virtual int32_t DataLinkLayer_Type() const;
virtual int32_t protocolFamily() const;
virtual int32_t protocolType() const;
virtual int32_t headerType() const;
virtual int32_t optionType() const;
virtual int32_t optionCode() const;
virtual int32_t icmpv6Type() const;
virtual int32_t mobilityType() const;
virtual int32_t vrrpVersion() const;
virtual bool ISAKMP_encryption() const;
virtual int32_t rrType() const;
virtual int32_t hardwareType() const;
virtual int32_t authProto() const;
virtual int32_t icmpv4Type() const;
virtual int32_t igmpType() const;
virtual int32_t upperPort() const;
virtual int32_t dhcpv6Type() const;
virtual int32_t DOIvalue() const;
virtual int32_t notify_value() const;
virtual int32_t ProtocolID() const;
virtual int32_t IDtype() const;
virtual int32_t SITvalue() const;
virtual int32_t duidType() const;
#if !defined(NOT_USE_SNMP)
virtual int32_t pduType() const;
#endif //NOT_USE_SNMP
virtual const TypevsMcDict* get_dict() const;
virtual uint32_t alignment_requirement() const;//1,2,4,8,0(no alignment)
virtual	bool icvUpdate(const PaObject&,PvOctets*) const;
virtual	WObject* compose(WControl&,WObject* w_parent,const PObject* po) const;
virtual	RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
virtual	RObject* rreverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
virtual	void composeList(WControl&,
		WObject* w_parent,const PObjectList& pls) const;
virtual	void describe(const PObject*) const;
};

//======================================================================
// DECORATOR MULTIPLE MEMBERS
class MmMultiple:public MmDecorator { //more than 0
private:
	METH_HC_MLC	METH_HC_MLC_;
public:
	MmMultiple(MmObject*,METH_HC_MLC meth);
virtual	~MmMultiple();
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual	void composeList(WControl&,
		WObject* w_parent,const PObjectList& pls) const;
virtual RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
};

//======================================================================
// ELEMENT MEMBERS
class MmElement:public MmObject {
private:
	uint32_t type_;
	uint32_t offset_;
	const PObject*		generater_;
	const PObject*		evaluater_;
	const ICVoverwriter*	ICVoverwriter_;
	METH_HC_ForIPinfo	METH_HC_ForIPinfo_;
public:
	MmElement(CSTR,
		const PObject* =0,const PObject* =0,const ICVoverwriter* ow=0,
		METH_HC_ForIPinfo meth=0);
virtual	~MmElement();
inline	const PObject* generater() const;
inline	const PObject* evaluater() const;
inline	void generater(const PObject*);
inline	void evaluater(const PObject*);
// AUTO FUNCTION
virtual	bool geneAuto(WControl&,WObject*,OCTBUF&) const;
virtual	PvObject* geneAutoValue(WObject*) const;
virtual	PvObject* evalAutoValue(WObject*) const;
//----------------------------------------------------------------------
// LENGTH AND WIDTH/OFFSET
virtual	void stepPostion(ItPosition&) const;	
virtual	void postProcess(void*);
	uint32_t offset() const;
	void offset(uint32_t);
virtual	uint16_t position() const;
virtual	void position(uint16_t);
virtual void print() const;
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual	WObject* compose(WControl&,WObject* w_parent,const PObject* pl) const;
virtual RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
// COMPOSE/REVERSE PROTECTED MmElement* ---------------------------------------
protected:
virtual	void composeList(WControl&,
		WObject* w_parent,const PObjectList& pls) const;
virtual	WObject* composeWm(WControl&,WObject* w_parent,const PObject* pl) const;
virtual ItPosition length_for_reverse(RControl&,ItPosition& at,OCTBUF& buf) const;
virtual RObject* reverseRm(RControl&,RObject* r_parent,
		const ItPosition& at,const ItPosition& size,PvObject* pv) const;
virtual PvObject* reversePv(RControl&,
		const ItPosition& at,const ItPosition& size,const OCTBUF&)const;
// GENERATE INTERFACE ---------------------------------------------------------
virtual bool generate(WControl& c,WObject* w_self,OCTBUF& buf)const;
// OVERWRITE ICV
virtual bool overwrite_ICV(ICVControl& c,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const;
};

inline uint32_t MmElement::offset() const {return offset_;}
inline void MmElement::offset(uint32_t o) {offset_=o;}
inline uint16_t MmElement::position() const {return 0;}
inline void MmElement::position(uint16_t) {}
inline const PObject* MmElement::generater() const {return generater_;}
inline const PObject* MmElement::evaluater() const {return evaluater_;}
inline void MmElement::generater(const PObject* p) {generater_=p;}
inline void MmElement::evaluater(const PObject* p) {evaluater_=p;}


//======================================================================
// META MEMBER UINT(Number)
class MmUint:public MmElement {
private:
	uint16_t width_;
	uint16_t position_;
public:
	MmUint(CSTR,uint16_t,const PObject* =0,const PObject* =0,
		const ICVoverwriter* ow=0,METH_HC_ForIPinfo meth=0);
virtual	~MmUint();
//----------------------------------------------------------------------
virtual	void encode(uint32_t,const ItPosition&,OCTBUF&) const;
virtual	bool encodeNumber(WControl&,const ItPosition&,OCTBUF&,const PvNumber&) const;
//----------------------------------------------------------------------
virtual	uint32_t rdecode(ItPosition&,const OCTBUF&,bool&) const;
virtual	uint32_t decode(const ItPosition&,const OCTBUF&) const;
virtual	uint32_t value(const ItPosition&,const OCTBUF&) const;
//----------------------------------------------------------------------
inline	uint16_t width() const;
virtual	uint16_t position() const;
virtual	void position(uint16_t);
// COMPOSE/REVERSE PROTECTED MmUint -------------------------------------------
virtual PvObject* reversePv(RControl&,
		const ItPosition& at,const ItPosition& size,const OCTBUF&)const;
};

inline uint16_t MmUint::width() const {return width_;}
inline uint16_t MmUint::position() const {return position_;}
inline void MmUint::position(uint16_t o) {position_=o;}

//-----------------------------------------------------------------------
class MmHostUint:public MmUint {
public:
	MmHostUint(CSTR,uint16_t,const PObject* =0,const PObject* =0,
		const ICVoverwriter* ow=0,METH_HC_ForIPinfo meth=0);
virtual	~MmHostUint();
virtual	void encode(uint32_t,const ItPosition&,OCTBUF&) const;
virtual	uint32_t decode(const ItPosition&,const OCTBUF&) const;
};

//======================================================================
// META MEMBER VARIABLE OCTSTR
class MmVarOctets:public MmElement {
public:
	MmVarOctets(CSTR,const PObject* =0,const PObject* =0,
		const ICVoverwriter* ow=0,METH_HC_ForIPinfo meth=0);
virtual	~MmVarOctets();
//----------------------------------------------------------------------
virtual	uint32_t objectLength(const PObject* =0,const WObject* =0) const;
virtual	bool encodeOctets(WControl&,
		const ItPosition& at,OCTBUF& dst,const PvOctets&) const;
virtual bool encodeNumber(WControl&,
		const ItPosition& at,OCTBUF& dst,const PvNumber& n)const;
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual ItPosition length_for_reverse(RControl&,ItPosition&,OCTBUF&) const;
virtual PvObject* reversePv(RControl&,
		const ItPosition& at,const ItPosition& size,const OCTBUF&)const;
};

//======================================================================
// META MEMBER OCTSTR HAS FIX LENGTH
class MmOctets:public MmElement {
private:
	uint32_t length_;
public:
	MmOctets(CSTR,uint32_t,const PObject* =0,const PObject* =0,
		const ICVoverwriter* ow=0,METH_HC_ForIPinfo meth=0);
virtual	~MmOctets();
//----------------------------------------------------------------------
virtual	bool encodeOctets(WControl&,const ItPosition&,OCTBUF&,const PvOctets&) const;
virtual	uint32_t objectLength(const PObject* =0,const WObject* =0) const;
};
inline	uint32_t MmOctets::objectLength(const PObject*,const WObject*) const {return length_;}

//======================================================================
// IPv4 ADDRESS
class MmV4Addr:public MmOctets {
public:
	MmV4Addr(CSTR,const PObject* =0,const PObject* =0,
		const ICVoverwriter* ow=0,METH_HC_ForIPinfo meth=0);
virtual	~MmV4Addr();
// COMPOSE/REVERSE PROTECTED MmUint -------------------------------------------
virtual PvObject* reversePv(RControl&,
		const ItPosition& at,const ItPosition& size,const OCTBUF&)const;
};

//======================================================================
// IPv6 ADDRESS
class MmV6Addr:public MmOctets {
public:
	MmV6Addr(CSTR,const PObject* =0,const PObject* =0,
		const ICVoverwriter* ow=0,METH_HC_ForIPinfo meth=0);
virtual	~MmV6Addr();
// COMPOSE/REVERSE PROTECTED MmUint -------------------------------------------
virtual PvObject* reversePv(RControl&,
		const ItPosition& at,const ItPosition& size,const OCTBUF&)const;
};

//======================================================================
// ETHER ADDRESS
class MmEther:public MmOctets {
public:
	MmEther(CSTR,const PObject* =0,const PObject* =0,
		const ICVoverwriter* ow=0,METH_HC_ForIPinfo meth=0);
virtual	~MmEther();
// COMPOSE/REVERSE PROTECTED MmUint -------------------------------------------
virtual PvObject* reversePv(RControl&,
		const ItPosition& at,const ItPosition& size,const OCTBUF&)const;
};

//////////////////////////////////////////////////////////////////////////////
// ICV overwrite Items

class ICVoverwriter {
public:
	ICVoverwriter(){}
virtual	bool overwrite(ICVControl&,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const;
};

class ICV_Zero : public ICVoverwriter{
public:
	ICV_Zero():ICVoverwriter(){}
virtual	bool overwrite(ICVControl&,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const;
};
class ICV_HC : public ICVoverwriter{
	const McObject*	meta_;
	METH_HC_OWICV	METH_HC_OWICV_;
public:
	ICV_HC(const McObject* m,METH_HC_OWICV hc):ICVoverwriter(),
		meta_(m),METH_HC_OWICV_(hc){}
virtual	bool overwrite(ICVControl&,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const;
};
//////////////////////////////////////////////////////////////////////////////

#endif

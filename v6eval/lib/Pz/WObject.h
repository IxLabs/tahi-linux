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
 * $TAHI: v6eval/lib/Pz/WObject.h,v 1.49 2005/05/09 09:35:24 akisada Exp $
 */
#if !defined(__WObject_h__)
#define	__WObject_h__	1
#include "CmTypes.h"
#include "pzCommon.h"
#include "RObject.h"
#include "ItPosition.h"
class PObject;
class MObject;
class WControl;
class RControl;
class PvNumber;
class PvOctets;
class PvObject;
class PaObject;
//======================================================================
// WORK TREE COMPOSER
class WObject:public TObject {
private:
	const PObject* object_; //Pc,Pl,Pv
	ItPosition size_;	//this size
	ItPosition offset_;	//from FrameTop
public:
	WObject(WObject*,const MObject*,const PObject* =0);
virtual	~WObject();
	virtual WObject *wDecrypted() const { return(0); }
//----------------------------------------------------------------------
virtual	void generatePrologue(ItPosition&);
virtual	void evaluatePrologue(ItPosition&);
//----------------------------------------------------------------------
// MEMBER MANUPULATION
virtual const PvObject* pvalue() const{return (const PvObject*)rgenerate();}
virtual ItPosition size() const{return size_;}//pvalue size
//
inline	const PObject* object() const;
inline	const ItPosition& offset() const;
virtual	uint32_t length() const;
virtual uint32_t distance() const;	//from ParentTop
virtual uint32_t operation() const{return eOPEQ_;}
virtual	const PObject* rgenerate() const;
	CSTR operatorString() const {return operSTR[operation()];}
//----------------------------------------------------------------------
// HIERARCY MANUPULATION
inline	WObject* nextChild(const CmQueue* =0) const;
inline	WObject* nextSister() const;
inline	WObject* nextEvaluation(const CmQueue* =0) const;
inline	WObject* prevChild(const CmQueue* =0) const;
inline	WObject* prevSister() const;
inline	WObject* parent() const;
//----------------------------------------------------------------------
// GENERATE/ENCODE
virtual	bool generate(WControl&,OCTBUF&);
virtual	bool geneAuto(WControl&,OCTBUF&);
virtual	bool geneFix(WControl&,OCTBUF&);
//----------------------------------------------------------------------
virtual	bool encodeNumber(WControl&,OCTBUF&,const PvNumber&);
virtual	bool encodeOctets(WControl&,OCTBUF&,const PvOctets&);
//----------------------------------------------------------------------
virtual	void notMatch(WControl&,CSTR,RObject&) const;
virtual	void evaluateFailure(const PObject*,const PObject*,CSTR=0) const;
virtual	void functionFailure(RObject* r) const;
virtual	void dbgEvaluate(const PObject*,const PObject*,bool,CSTR=0) const;
virtual	bool metaEvaluate(WControl&,RObject&);
	bool valueEvaluate(WControl&,const PvObject* ro,const PvObject* eo);
virtual	PvObject* evalAutoValue();
virtual	PvObject* geneAutoValue();
virtual	bool doEvaluate(WControl&,RObject&);
virtual	RObject* evaluate(WControl&,RObject*);
virtual	bool vevaluate(const void*);
//----------------------------------------------------------------------
	const WObject* ancestor(const PObject* r) const;
	CSTR nameString() const;
	bool mustDefine(const PObject*) const;
	bool mustDefineMem(const MObject*) const;
//----------------------------------------------------------------------
// ALGORITHM INTERFACE
virtual	void algorithm(const PaObject*);
virtual	void icvUpdate(const PaObject&);
protected:
inline	void offset(const ItPosition&);
virtual	void printName(uint32_t,CSTR =0) const;
virtual	const CmQueue* children() const;
virtual	uint16_t width() const;
virtual	void internalGeneratePrologue(ItPosition&);
virtual	void internalEvaluatePrologue(ItPosition&);
virtual	void stepPostion(ItPosition&);
	void setSize(const ItPosition&);
inline	void set_object(const PObject* po);
};
inline const PObject* WObject::object() const {return object_;}
inline void WObject::set_object(const PObject* po){object_=po;}
inline const ItPosition& WObject::offset() const {return offset_;}
inline void WObject::offset(const ItPosition& o) {offset_=o;}
inline const CmQueue* WObject::children() const {return 0;}
inline WObject* WObject::parent() const {
	return (WObject*)TObject::parent();}
inline WObject* WObject::nextChild(const CmQueue* q) const {
	return (WObject*)TObject::nextChild(q);}
inline WObject* WObject::nextSister() const {
	return (WObject*)TObject::nextSister();}
inline WObject* WObject::nextEvaluation(const CmQueue* q) const {
	return (WObject*)TObject::nextEvaluation(q);}
inline WObject* WObject::prevChild(const CmQueue* q) const {
	return (WObject*)TObject::prevChild(q);}
inline WObject* WObject::prevSister() const{
	return (WObject*)TObject::prevSister();}

#include "CmCltn.h"
interfaceCmList(WObjectList,WObject);

//======================================================================
// has CHILDREN COMPOSER
class WtNode:public WObject {
private:
	CmQueue children_;
public:
	WtNode(WObject*,const MObject*,const PObject* =0);
virtual	~WtNode();
virtual	WObject* insert(TObject*);
virtual	void child(TObject*);
protected:
virtual	const CmQueue* children() const;
virtual	bool generate(WControl&,OCTBUF&);
virtual	void internalGeneratePrologue(ItPosition&);
virtual	void internalEvaluatePrologue(ItPosition&);
virtual	void icvUpdate(const PaObject&);
};
inline const CmQueue* WtNode::children() const {return &children_;}

//======================================================================
// MEMBER COMPOSER
class WmObject:public WtNode {
private:
	eOperation operator_;
	PObject* rgenerate_;
	PvObject* revaluate_;
public:
	WmObject(WObject*,const MObject*,const PObject* =0);
virtual	~WmObject();
virtual	bool generate(WControl&,OCTBUF&);
virtual	void internalGeneratePrologue(ItPosition&);
virtual	bool doEvaluate(WControl&,RObject&);
virtual	void internalEvaluatePrologue(ItPosition&);
virtual	uint32_t operation() const;
	PvObject* revaluate() const;
virtual	const PObject* rgenerate() const;
protected:
inline	void set_rgenerate(PObject*);
};
inline uint32_t WmObject::operation() const {return operator_;}
inline PvObject* WmObject::revaluate() const {return revaluate_;}
inline void WmObject::set_rgenerate(PObject* g) {rgenerate_=g;}

//======================================================================
// COMPOUND COMPOSER
class WcObject:public WtNode {
private:
	PvOctets* rgenerate_;
public:
	WcObject(WObject*,const MObject*,const PObject* =0);
virtual	~WcObject();
virtual	const PObject* rgenerate() const;
virtual	bool generate(WControl&,OCTBUF&);
virtual	void generatePrologue(ItPosition&);
virtual	bool doEvaluate(WControl&,RObject&);
virtual	void printName(uint32_t,CSTR =0) const;
virtual	void icvUpdate(const PaObject&);
};

//======================================================================
class WcPacket_IP:public WcObject {// has a Pseudo_Header ,and keep it
private:
	class Con_IPinfo*	IPinfo_;
public:
	WcPacket_IP(WObject*,const MObject*,const PObject* pc);
virtual	~WcPacket_IP();
	Con_IPinfo* IPinfo()const{return IPinfo_;}
};

//======================================================================
// DATA COMPOSER
class WvObject:public WtNode {
private:
public:
	WvObject(WObject*,const MObject*,const PObject* =0);
virtual	~WvObject();
virtual	bool encodeNumber(WControl&,OCTBUF&,const PvNumber&);
virtual	bool encodeOctets(WControl&,OCTBUF&,const PvOctets&);
};

//======================================================================
// SUBSTR FUNCTION COMPOSER
class WvSubstr:public WvObject {
private:
	WObject* reference_;
	PvOctets* substr_;
public:
	WvSubstr(WObject*,const MObject*,const PObject* =0);
virtual	~WvSubstr();
virtual	void internalGeneratePrologue(ItPosition&);
virtual	bool generate(WControl&,OCTBUF&);
virtual	const PObject* rgenerate() const;
protected:
virtual	void child(TObject*);
};

//======================================================================
// ONEOF FUNCTION COMPOSER
class WvOneof:public WvObject {
private:
	WObjectList references_;
public:
	WvOneof(WObject*,const MObject*,const PObject* =0);
virtual	~WvOneof();
virtual	RObject* evaluate(WControl&,RObject*);
virtual	bool generate(WControl&,OCTBUF&);
protected:
virtual	void child(TObject*);
};

//======================================================================
// COMB FUNCTION COMPOSER
class WvComb:public WvObject {
private:
	WObjectList references_;
public:
	WvComb(WObject*,const MObject*,const PObject* =0);
virtual	~WvComb();
virtual	RObject* evaluate(WControl&,RObject*);
virtual	bool generate(WControl&,OCTBUF&);
protected:
virtual	void child(TObject*);
};
#endif

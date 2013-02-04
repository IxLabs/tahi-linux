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
 * $TAHI: v6eval/lib/Pz/RObject.h,v 1.29 2003/07/15 09:10:01 akisada Exp $
 */
#if !defined(__RObject_h__)
#define	__RObject_h__	1
#include "CmTypes.h"
#include "pzCommon.h"
#include "CmQueue.h"
#include "ItPosition.h"
class PvObject;
class MObject;
class Con_IPinfo;
class ICVControl;
class RControl;
class WControl;
class RObject;
class WObject;

//======================================================================
// TIE TREE COMPOSER(tie meta and lvalue ro rvalue
class TObject:public CmQueue {
public:
	enum eOperation {eOPLT_=0x01, eOPEQ_=0x02, eOPGT_=0x04};
static	const CSTR operSTR[8];
private:
	const MObject* meta_;
	TObject* parent_;
public:
	TObject(TObject*,const MObject*);
virtual	~TObject();
inline	const MObject* meta() const;
inline	TObject* parent() const;
virtual const PvObject* pvalue() const;	//R:reversedPv W:generatedPv
virtual	ItPosition size() const;	//pvalue size
	TObject* nextChild(const CmQueue* =0) const;
	TObject* nextSister() const;
	TObject* nextEvaluation(const CmQueue* =0) const;
	TObject* prevChild(const CmQueue* =0) const;
	TObject* prevSister() const;
	bool evaluateObject() const;
	CSTR metaString() const;
	void adoptSisters(TObject&);
//----------------------------------------------------------------------
virtual	const TObject* corresponding(const MObject*) const;
//----------------------------------------------------------------------
virtual	void logSelf(uint32_t,CSTR =0) const;
virtual	void logChild(uint32_t) const;
virtual	void log(uint32_t) const;
//----------------------------------------------------------------------
virtual	void describeSelf(uint32_t,CSTR=0) const;
virtual	void describeChild(uint32_t) const;
virtual	void describe(uint32_t) const;
//----------------------------------------------------------------------
virtual	void printName(uint32_t,CSTR =0) const;
virtual	void printChild(uint32_t) const;
virtual	void print(uint32_t) const;
inline	bool isEqual(const TObject*) const;
protected:
virtual	const CmQueue* children() const;
virtual	void child(TObject*);
virtual	void adoptParent(TObject*);
//----------------------------------------------------------------------
public:
	bool overwrite_ICV(ICVControl&,ItPosition& at,OCTBUF& buf)const;
	bool overwrite_ICV_child(ICVControl&,
			const ItPosition& at,OCTBUF& buf)const;
// POST ACTION
virtual	void post_reverse(Con_IPinfo&,RControl&,RObject* from);
virtual	void post_generate(Con_IPinfo&,WControl& c,OCTBUF& buf,WObject* from);
virtual	void post_generateWithLength(Con_IPinfo &, WControl &, OCTBUF &, WObject *, uint32_t);
};

inline const MObject* TObject::meta() const {return meta_;}
inline TObject* TObject::parent() const {return parent_;}
inline void TObject::adoptParent(TObject* p) {parent_=p;}
inline const CmQueue* TObject::children() const {return 0;}
inline bool TObject::isEqual(const TObject* p) const {
	return meta()==p->meta();}

class PControl;
class WControl;
//======================================================================
// REVERSE TREE COMPOSER
class RObject:public TObject {
private:
	PvObject* pvalue_;		//Pv(only decodeable realvalue)
	ItPosition offset_;		//from ParentTop
public:
	RObject(RObject*,const MObject*,const ItPosition& offset,PvObject* =0);
virtual	~RObject();
virtual const PvObject* pvalue() const{return pvalue_;}
//
inline	RObject* parent() const;
inline	const ItPosition& offset() const{return offset_;}
inline	RObject* nextChild(const CmQueue* =0) const;
inline	RObject* nextSister() const;
inline	RObject* nextEvaluation(const CmQueue* =0) const;
inline	RObject* prevChild(const CmQueue* =0) const;
inline	RObject* prevSister() const;
virtual	void print(uint32_t) const;
protected:
virtual	void printName(uint32_t,CSTR =0) const;
};
inline RObject* RObject::parent() const {
	return (RObject*)TObject::parent();}
inline RObject* RObject::nextChild(const CmQueue* q) const {
	return (RObject*)TObject::nextChild(q);}
inline RObject* RObject::nextSister() const {
	return (RObject*)TObject::nextSister();}
inline RObject* RObject::nextEvaluation(const CmQueue* q) const {
	return (RObject*)TObject::nextEvaluation(q);}
inline RObject* RObject::prevChild(const CmQueue* q) const {
	return (RObject*)TObject::prevChild(q);}
inline RObject* RObject::prevSister() const {
	return (RObject*)TObject::prevSister();}

//======================================================================
// has CHILDREN COMPOSER
class RtNode:public RObject {
private:
	CmQueue children_;
public:
	RtNode(RObject*,const MObject*,const ItPosition&,PvObject* =0);
virtual	~RtNode();
virtual	RObject* insert(TObject*);
virtual	void child(TObject*);
protected:
virtual	const CmQueue* children() const;
};
inline const CmQueue* RtNode::children() const {return &children_;}

//======================================================================
// MEMBER COMPOSER
class RmObject:public RObject {
private:
	ItPosition size_;		//this size
public:
	RmObject(RObject*,const MObject*,const ItPosition& offset,
		const ItPosition& size,PvObject* =0);
virtual	~RmObject();
virtual	ItPosition size() const{return size_;}
virtual	void printName(uint32_t,CSTR =0) const;
virtual	void logSelf(uint32_t,CSTR =0) const;
};

//======================================================================
// COMPOUND COMPOSER
class RcObject:public RtNode {
public:
	RcObject(RObject*,const MObject*,const ItPosition& offset,PvObject* =0);
virtual	~RcObject();
virtual	ItPosition size() const;	//pvalue size
virtual	void printName(uint32_t,CSTR =0) const;
virtual	void logSelf(uint32_t,CSTR =0) const;
};

class RcPacket_IP:public RcObject {
private:
	class Con_IPinfo*	IPinfo_;
public:
	RcPacket_IP(RObject*,const MObject*,
		const ItPosition& offset,PvObject* pv);
virtual	~RcPacket_IP();
	Con_IPinfo* IPinfo()const{return IPinfo_;}
};
#endif

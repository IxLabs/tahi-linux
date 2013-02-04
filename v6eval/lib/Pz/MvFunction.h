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
 * $TAHI: v6eval/lib/Pz/MvFunction.h,v 1.45 2006/08/31 01:06:40 akisada Exp $
 */
#if !defined(__MvFunction_h__)
#define	__MvFunction_h__	1
#include "McObject.h"
#include <openssl/md5.h>
class PObject;
//======================================================================
class MvAction:public MvKeyword {
public:
	MvAction(CSTR,int32_t);
virtual ~MvAction();
virtual	WObject* compose(WControl&,WObject*,const PObject*) const;
};

//----------------------------------------------------------------------
// ANY TOKEN CAN BE RIGHT VALUE OR COMPOUND COMPARISON INDICATOR
class MvANY:public MvAction {
public:
	MvANY(CSTR,int32_t);
virtual ~MvANY();
virtual PObject* tokenObject(int,CSTR) const;
virtual	WObject* compose(WControl&,WObject*,const PObject*) const;
virtual bool comparableMeta(const MObject*) const;
};
inline bool MvANY::comparableMeta(const MObject*) const {return true;}

//----------------------------------------------------------------------
// STOP TOKEN CAN BE COMPOUND COMPARISON INDICATOR
class MvSTOP:public MvAction {
public:
	MvSTOP(CSTR,int32_t);
virtual ~MvSTOP();
virtual PObject* tokenObject(int,CSTR) const;
virtual	WObject* compose(WControl&,WObject*,const PObject*) const;
virtual bool comparableMeta(const MObject*) const;
};
inline bool MvSTOP::comparableMeta(const MObject*) const {return true;}

//----------------------------------------------------------------------
class MvAUTO:public MvAction {
public:
	MvAUTO(CSTR,int32_t);
virtual ~MvAUTO();
virtual PObject* tokenObject(int,CSTR) const;
};

//----------------------------------------------------------------------
class MvFILL:public MvAction {
public:
	MvFILL(CSTR,int32_t);
virtual ~MvFILL();
virtual PObject* tokenObject(int,CSTR) const;
};

//----------------------------------------------------------------------
class MvZERO:public MvAction {
public:
	MvZERO(CSTR,int32_t);
virtual ~MvZERO();
virtual PObject* tokenObject(int,CSTR) const;
};

//======================================================================
class MvOperator:public MvKeyword {
private:
	uint32_t operation_;
public:
	MvOperator(CSTR,int32_t,int32_t);
virtual	~MvOperator();
virtual PObject* tokenObject(int,CSTR) const;
virtual	uint32_t operation() const;
};
inline uint32_t MvOperator::operation() const {return operation_;}

class TObject;
class PvEther;
class PvV4Addr;
class PvV6Addr;
class PFunction;
class PvObjectID;
//======================================================================
class MvFunction:public MObject {
private:
public:
	MvFunction(CSTR);
virtual	~MvFunction();
virtual	int32_t token() const;
virtual	PObject* tokenObject(int,CSTR) const;
virtual	uint32_t objectLength(const PObject* =0,const WObject* =0) const;
virtual	uint32_t functionLength(const PObjectList&,const WObject*) const;
virtual	bool functionGenerate(WControl&,WObject*,OCTBUF&,const PObjectList&) const;
virtual	bool generateV6Addr(const PObjectList&,PvV6Addr&) const;
virtual	bool evaluateV6Addr(const PObjectList&,PvV6Addr&) const;
virtual	bool generateOctetsWith(const PObjectList&,PvOctets&,WObject*) const;
virtual	PvObject* generateValue(WObject*,const PObjectList&) const;
virtual	PvObject* evaluateValue(WObject*,const PObjectList&) const;
virtual	int32_t compareObject(const PObject&,const PObjectList&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
virtual	bool isV6Addr() const;
virtual	bool isOctets() const;
// COMPOSE INTERFACE ----------------------------------------------------------
virtual	WObject* compose(WControl&,WObject* w_parent,const PObject* pv) const;
// COMPOSE INTERFACE PROTECTED MvFunction* ------------------------------------
protected:
virtual	WObject* composeWv(WControl&,WObject* w_parent,const PObject* pv) const;
virtual void args_compose(WControl&,
		WObject* w_self,const PObjectList& pas) const;
};

inline uint32_t MvFunction::functionLength(const PObjectList&,const WObject*) const {return 0;}

//======================================================================
class MvWithin:public MvFunction {
public:
	MvWithin(CSTR);
virtual	~MvWithin();
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
virtual	int32_t compareObject(const PObject&,const PObjectList&) const;
};

//======================================================================
class MvOneof:public MvFunction {
public:
	MvOneof(CSTR);
virtual	~MvOneof();
virtual	int32_t compareObject(const PObject&,const PObjectList&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
// COMPOSE INTERFACE ----------------------------------------------------------
virtual	WObject* composeWv(WControl&,WObject* w_parent,const PObject* pv) const;
virtual void args_compose(WControl&,
		WObject* w_self,const PObjectList& pas) const;
};

//======================================================================
class MvComb:public MvFunction {
public:
	MvComb(CSTR);
virtual	~MvComb();
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
// COMPOSE INTERFACE ----------------------------------------------------------
virtual	WObject* composeWv(WControl&,WObject* w_parent,const PObject* pv) const;
virtual void args_compose(WControl&,
		WObject* w_self,const PObjectList& pas) const;
};

//======================================================================
class MvOctets:public MvFunction {
public:
	MvOctets(CSTR);
virtual	~MvOctets();
virtual	bool functionGenerate(WControl&,WObject*,OCTBUF&,const PObjectList&) const;
virtual	PvObject* generateValue(WObject*,const PObjectList&) const;
virtual	PvObject* evaluateValue(WObject*,const PObjectList&) const;
virtual	bool isOctets() const;
};

//======================================================================
class MvRepeat:public MvOctets {
public:
	MvRepeat(CSTR);
virtual	~MvRepeat();
virtual	bool generateOctetsWith(const PObjectList&,PvOctets&,WObject*) const;
virtual	uint32_t functionLength(const PObjectList&,const WObject*) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
};

//======================================================================
class MvHexStr: public MvOctets {
	public:
		MvHexStr(CSTR);
		virtual	~MvHexStr();

		virtual	bool generateOctetsWith(const PObjectList &, PvOctets &, WObject *) const;
		virtual	uint32_t functionLength(const PObjectList &, const WObject *) const;
		virtual	bool checkArgument(const PFunction &, const PObjectList &) const;
		virtual bool hex_pton(OCTSTR, uint32_t, CSTR, uint32_t, bool) const;
		virtual bool isHexStr(CSTR, uint32_t) const;
};

//======================================================================
class MvInt2Hex: public MvOctets {
	public:
		MvInt2Hex(CSTR);
		virtual	~MvInt2Hex();

		virtual	bool generateOctetsWith(const PObjectList &,
			PvOctets &, WObject *) const;

		virtual	uint32_t functionLength(const PObjectList &,
			const WObject *) const;

		virtual	bool checkArgument(const PFunction &,
			const PObjectList &) const;
};

//======================================================================
class MvFile: public MvOctets {
	public:
		MvFile(CSTR);
		virtual	~MvFile();

		virtual	bool checkArgument(const PFunction &, const PObjectList &) const;
		virtual	uint32_t functionLength(const PObjectList &, const WObject *) const;
		virtual	bool generateOctetsWith(const PObjectList &, PvOctets &, WObject *) const;
};

//======================================================================
class MvSipTxt: public MvFile {
	public:
		MvSipTxt(CSTR);
		virtual	~MvSipTxt();
		virtual	PvObject *generateValue(WObject *, const PObjectList &) const;
		virtual	PvObject *evaluateValue(WObject *, const PObjectList &) const;
};

//======================================================================
class MvAscii: public MvOctets {
	public:
		MvAscii(CSTR);
		virtual	~MvAscii();

		virtual	bool generateOctetsWith(const PObjectList &,
			PvOctets &, WObject *) const;

		virtual	uint32_t functionLength(const PObjectList &,
			const WObject *) const;

		virtual	bool checkArgument(const PFunction &,
			const PObjectList &) const;

		virtual PvObject *generateValue(WObject *,
			const PObjectList &) const;

		virtual PvObject *evaluateValue(WObject *,
			const PObjectList &) const;
};

//======================================================================
class MvDnsStr: public MvOctets {
	public:
		MvDnsStr(CSTR);
		virtual	~MvDnsStr();

		virtual	bool generateOctetsWith(const PObjectList &, PvOctets &, WObject *) const;
		virtual	uint32_t functionLength(const PObjectList &, const WObject *) const;
		virtual	bool checkArgument(const PFunction &, const PObjectList &) const;
		virtual	PvObject *generateValue(WObject *, const PObjectList &) const;
		virtual	PvObject *evaluateValue(WObject *, const PObjectList &) const;
};

//======================================================================
class MvDnsName: public MvOctets {
	public:
		MvDnsName(CSTR);
		virtual	~MvDnsName();

		virtual	bool generateOctetsWith(const PObjectList &, PvOctets &, WObject *) const;
		virtual	uint32_t functionLength(const PObjectList &, const WObject *) const;
		virtual	bool checkArgument(const PFunction &, const PObjectList &) const;
		virtual	PvObject *generateValue(WObject *, const PObjectList &) const;
		virtual	PvObject *evaluateValue(WObject *, const PObjectList &) const;
		virtual bool isDnsName(CSTR, uint32_t) const;
};

//======================================================================
class MvCookie64: public MvOctets {
	public:
		MvCookie64(CSTR);
		virtual	~MvCookie64();
		virtual	bool generateOctetsWith(const PObjectList &,
			PvOctets &, WObject *) const;

		virtual	uint32_t functionLength(const PObjectList &,
			const WObject *) const;

		virtual bool checkArgument(const PFunction &,
			const PObjectList &) const;

		virtual PvObject *generateValue(WObject *,
			const PObjectList &) const;

		virtual PvObject *evaluateValue(WObject *,
			const PObjectList &) const;
};

//======================================================================
class MvKbu: public MvOctets {
	public:
		MvKbu(CSTR);
		virtual	~MvKbu();

		virtual	bool generateOctetsWith(const PObjectList &, PvOctets &, WObject *) const;
		virtual	uint32_t functionLength(const PObjectList &, const WObject *) const;
		virtual bool checkArgument(const PFunction &, const PObjectList &) const;
};

//======================================================================
class MfBSA: public MvOctets {
	public:
		MfBSA(CSTR);
		virtual	~MfBSA();

		virtual bool checkArgument(const PFunction &, const PObjectList &) const;
		virtual	uint32_t functionLength(const PObjectList &, const WObject *) const;
		virtual	bool generateOctetsWith(const PObjectList &, PvOctets &, WObject *) const;

		virtual OCTSTR init(OCTSTR, const PObjectList &) const;
		virtual void update(OCTSTR, const PObjectList &, const OCTBUF &) const;
		virtual PvOctets *result(OCTSTR, const PObjectList &) const;

		virtual int32_t token() const;
		virtual PObject *tokenObject(int, CSTR) const;
};

inline int32_t MfBSA::token() const {return metaToken(tkn_bsafunc_);}

//======================================================================
class MfDHCPAuth: public MvOctets {
	public:
		MfDHCPAuth(CSTR);
		virtual	~MfDHCPAuth();

		virtual	bool generateOctetsWith(const PObjectList &, PvOctets &, WObject *) const;
		virtual	uint32_t functionLength(const PObjectList &, const WObject *) const;
		virtual bool checkArgument(const PFunction &, const PObjectList &) const;

		virtual OCTSTR init(OCTSTR, const PObjectList &) const;
		virtual void update(OCTSTR, const PObjectList &, const OCTBUF &) const;
		virtual PvOctets *result(OCTSTR, const PObjectList &) const;

		virtual int32_t token() const;
		virtual PObject *tokenObject(int, CSTR) const;
};

inline int32_t MfDHCPAuth::token() const {return metaToken(tkn_dhcpauthfunc_);}

//======================================================================
class MvSubstr:public MvOctets {
public:
	MvSubstr(CSTR);
virtual	~MvSubstr();
virtual	uint32_t functionLength(const PObjectList&,const WObject*) const;
virtual	bool generateSubstr(WObject*,PObject*,PObject*,PvOctets&) const;
virtual	bool generateOctetsWith(const PObjectList&,PvOctets&,WObject*) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
// COMPOSE INTERFACE ----------------------------------------------------------
virtual	WObject* composeWv(WControl&,WObject* w_parent,const PObject* pv) const;
virtual void args_compose(WControl&,
		WObject* w_self,const PObjectList& pas) const;
};

//======================================================================
class MvPatch:public MvSubstr {
public:
	MvPatch(CSTR);
virtual	~MvPatch();
virtual	uint32_t functionLength(const PObjectList&,const WObject*) const;
virtual	bool generateOctetsWith(const PObjectList&,PvOctets&,WObject*) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
};

//======================================================================
class MvLeft:public MvSubstr {
public:
	MvLeft(CSTR);
virtual	~MvLeft();
virtual	uint32_t functionLength(const PObjectList&,const WObject*) const;
virtual	bool generateOctetsWith(const PObjectList&,PvOctets&,WObject*) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
};

//======================================================================
class MvRight:public MvSubstr {
public:
	MvRight(CSTR);
virtual	~MvRight();
virtual	uint32_t functionLength(const PObjectList&,const WObject*) const;
virtual	bool generateOctetsWith(const PObjectList&,PvOctets&,WObject*) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
};

//======================================================================
class MvV4:public MvFunction {
public:
	MvV4(CSTR);
virtual	~MvV4();
virtual	uint32_t functionLength(const PObjectList&,const WObject*) const;
virtual	bool generateV4Addr(const PObjectList&,PvV4Addr&) const;
virtual	bool functionGenerate(WControl&,WObject*,OCTBUF&,const PObjectList&) const;
virtual	PvObject* generateValue(WObject*,const PObjectList&) const;
virtual	PvObject* evaluateValue(WObject*,const PObjectList&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
};
inline uint32_t MvV4::functionLength(const PObjectList&,const WObject*) const {return sizeof(v4addr);}

//======================================================================
class MvEther:public MvFunction {
public:
	MvEther(CSTR);
virtual	~MvEther();
virtual	uint32_t functionLength(const PObjectList&,const WObject*) const;
virtual	bool generateEther(const PObjectList&,PvEther&) const;
virtual	bool evaluateEther(const PObjectList&,PvEther&) const;
virtual	bool generateTN(const PObject*,PvEther&) const;
virtual	bool generateNUT(const PObject*,PvEther&) const;
virtual	bool functionGenerate(WControl&,WObject*,OCTBUF&,const PObjectList&) const;
virtual	PvObject* generateValue(WObject*,const PObjectList&) const;
virtual	PvObject* evaluateValue(WObject*,const PObjectList&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
};
inline uint32_t MvEther::functionLength(const PObjectList&,const WObject*) const {return sizeof(etheraddr);}

//======================================================================
class MvEtherSRC:public MvEther {
public:
	MvEtherSRC(CSTR);
virtual	~MvEtherSRC();
virtual	bool generateEther(const PObjectList&,PvEther&) const;
virtual	bool evaluateEther(const PObjectList&,PvEther&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
};

//======================================================================
class MvEtherDST:public MvEther {
public:
	MvEtherDST(CSTR);
virtual	~MvEtherDST();
virtual	bool generateEther(const PObjectList&,PvEther&) const;
virtual	bool evaluateEther(const PObjectList&,PvEther&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
};

//======================================================================
class MvEtherNUT:public MvEther {
public:
	MvEtherNUT(CSTR);
virtual	~MvEtherNUT();
virtual	bool generateEther(const PObjectList&,PvEther&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
};

//======================================================================
class MvEtherTN:public MvEther {
public:
	MvEtherTN(CSTR);
virtual	~MvEtherTN();
virtual	bool generateEther(const PObjectList&,PvEther&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
};

//======================================================================
class MvEtherMulti:public MvEther {
public:
	MvEtherMulti(CSTR);
virtual	~MvEtherMulti();
virtual	bool generateEther(const PObjectList&,PvEther&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
virtual	PvObject* generateValue(WObject*,const PObjectList&) const;
virtual	PvObject* evaluateValue(WObject*,const PObjectList&) const;
};

//======================================================================
class MvV6:public MvFunction {
public:
	MvV6(CSTR);
virtual	~MvV6();
virtual	uint32_t functionLength(const PObjectList&,const WObject*) const;
virtual	bool generateV6Addr(const PObjectList&,PvV6Addr&) const;
virtual	bool evaluateV6Addr(const PObjectList&,PvV6Addr&) const;
virtual	bool generateTN(const PObject*,PvV6Addr&) const;
virtual	bool generateNUT(const PObject*,PvV6Addr&) const;
virtual	bool functionGenerate(WControl&,WObject*,OCTBUF&,const PObjectList&) const;
virtual	PvObject* generateValue(WObject*,const PObjectList&) const;
virtual	PvObject* evaluateValue(WObject*,const PObjectList&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
virtual	bool isV6Addr() const;
};
inline uint32_t MvV6::functionLength(const PObjectList&,const WObject*) const {return sizeof(v6addr);}

//======================================================================
class MvV6SRC:public MvV6 {
public:
	MvV6SRC(CSTR);
virtual	~MvV6SRC();
virtual	bool generateV6Addr(const PObjectList&,PvV6Addr&) const;
virtual	bool evaluateV6Addr(const PObjectList&,PvV6Addr&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
};

//======================================================================
class MvV6DST:public MvV6 {
public:
	MvV6DST(CSTR);
virtual	~MvV6DST();
virtual	bool generateV6Addr(const PObjectList&,PvV6Addr&) const;
virtual	bool evaluateV6Addr(const PObjectList&,PvV6Addr&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
};

//======================================================================
class MvV6NUT:public MvV6 {
public:
	MvV6NUT(CSTR);
virtual	~MvV6NUT();
virtual	bool generateV6Addr(const PObjectList&,PvV6Addr&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
};

//======================================================================
class MvV6TN:public MvV6 {
public:
	MvV6TN(CSTR);
virtual	~MvV6TN();
virtual	bool generateV6Addr(const PObjectList&,PvV6Addr&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
};

//======================================================================
class MvV6PNUT:public MvV6 {
public:
	MvV6PNUT(CSTR);
virtual	~MvV6PNUT();
virtual	bool generateV6Addr(const PObjectList&,PvV6Addr&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
};

//======================================================================
class MvV6PTN:public MvV6 {
public:
	MvV6PTN(CSTR);
virtual	~MvV6PTN();
virtual	bool generateV6Addr(const PObjectList&,PvV6Addr&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
};

//======================================================================
class MvV6Merge:public MvV6 {
public:
	MvV6Merge(CSTR);
virtual	~MvV6Merge();
virtual	bool generateV6Addr(const PObjectList&,PvV6Addr&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
virtual	PvObject* generateValue(WObject*,const PObjectList&) const;
virtual	PvObject* evaluateValue(WObject*,const PObjectList&) const;
};

//======================================================================
class MvV6V6:public MvV6 {
public:
	MvV6V6(CSTR);
virtual	~MvV6V6();
virtual	bool generateV6Addr(const PObjectList&,PvV6Addr&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
};

//======================================================================
class MvV6Ether:public MvV6 {
public:
	MvV6Ether(CSTR);
virtual	~MvV6Ether();
virtual	bool generateV6Addr(const PObjectList&,PvV6Addr&) const;
virtual	bool checkArgument(const PFunction&,const PObjectList&) const;
};

//======================================================================
class MvObjectID: public MvOctets {
	public:
		MvObjectID(CSTR);
		virtual	~MvObjectID();

		virtual	bool generateOctetsWith(const PObjectList &, PvOctets &, WObject *) const;
		virtual	uint32_t functionLength(const PObjectList &, const WObject *) const;
		virtual	bool checkArgument(const PFunction &, const PObjectList &) const;
		virtual bool asn1encode(char *, uint32_t *, CSTR, uint32_t) const;
		virtual bool isOidStr(CSTR, uint32_t) const;
};

////////////////////////////////////////////////////////////////
// HASH_I = prf(SKEYID,                                       //
//	g^xi | g^xr | CKY-I | CKY-R | SAi_b | IDii_b )        //
// HASH_R = prf(SKEYID,                                       //
//	g^xr | g^xi | CKY-R | CKY-I | SAi_b | IDir_b )        //
////////////////////////////////////////////////////////////////
class MvP1_HASH: public MvOctets {
	public:
		MvP1_HASH(CSTR);
		virtual	~MvP1_HASH();

		virtual	bool generateOctetsWith(const PObjectList &,
			PvOctets &, WObject *) const;

		virtual	uint32_t functionLength(const PObjectList &,
			const WObject *) const;

		virtual	bool checkArgument(const PFunction &,
			const PObjectList &) const;
};

////////////////////////////////////////////////////////////////
// phase 1 IV = hash( g^xi | g^xr )                           //
////////////////////////////////////////////////////////////////
class MvP1_IV: public MvOctets {
	public:
		MvP1_IV(CSTR);
		virtual	~MvP1_IV();

		virtual	bool generateOctetsWith(const PObjectList &,
			PvOctets &, WObject *) const;

		virtual	uint32_t functionLength(const PObjectList &,
			const WObject *) const;

		virtual	bool checkArgument(const PFunction &,
			const PObjectList &) const;
};

////////////////////////////////////////////////////////////////////////////////
// phase 2 IV =                                                               //
//     hash( the last phase 1 CBC output block | the phase 2 message id )     //
////////////////////////////////////////////////////////////////////////////////
class MvP2_IV: public MvOctets {
	public:
		MvP2_IV(CSTR);
		virtual	~MvP2_IV();

		virtual	bool generateOctetsWith(const PObjectList &,
			PvOctets &, WObject *) const;

		virtual	uint32_t functionLength(const PObjectList &,
			const WObject *) const;

		virtual	bool checkArgument(const PFunction &,
			const PObjectList &) const;
};

////////////////////////////////////////////////////////////////////////////////
// HASH(2) = prf(SKEYID_a, M-ID | Ni_b | SA | Nr [ | KE ] [ | IDci | IDcr )   //
////////////////////////////////////////////////////////////////////////////////
class MvP2_HASH_2: public MvOctets {
	public:
		MvP2_HASH_2(CSTR);
		virtual	~MvP2_HASH_2();

		virtual	bool generateOctetsWith(const PObjectList &,
			PvOctets &, WObject *) const;

		virtual	uint32_t functionLength(const PObjectList &,
			const WObject *) const;

		virtual	bool checkArgument(const PFunction &,
			const PObjectList &) const;

		virtual OCTSTR init(OCTSTR, const PObjectList &) const;
		virtual void update(OCTSTR, const PObjectList &,
			const OCTBUF &) const;
		virtual PvOctets *result(OCTSTR, const PObjectList &) const;

		virtual int32_t token() const;
		virtual PObject *tokenObject(int, CSTR) const;
};

inline int32_t MvP2_HASH_2::token() const {
	return(metaToken(tkn_ike_p2_hash_func_)); }

////////////////////////////////////////////////////////////////////////////////
// HASH(1) = prf(SKEYID_a, M-ID | SA | Ni [ | KE ] [ | IDci | IDcr )          //
////////////////////////////////////////////////////////////////////////////////
class MvP2_HASH_1: public MvP2_HASH_2 {
	public:
		MvP2_HASH_1(CSTR key): MvP2_HASH_2(key) {}
		virtual	~MvP2_HASH_1() {};

		virtual	bool checkArgument(const PFunction &,
			const PObjectList &) const;

		virtual void update(OCTSTR, const PObjectList &,
			const OCTBUF &) const;
};

////////////////////////////////////////////////////////
// HASH(3) = prf(SKEYID_a, 0 | M-ID | Ni_b | Nr_b)    //
////////////////////////////////////////////////////////
class MvP2_HASH_3: public MvP2_HASH_2 {
	public:
		MvP2_HASH_3(CSTR key): MvP2_HASH_2(key) {}
		virtual	~MvP2_HASH_3() {};

		virtual	bool checkArgument(const PFunction &,
			const PObjectList &) const;

		virtual void update(OCTSTR, const PObjectList &,
			const OCTBUF &) const;
};
#endif

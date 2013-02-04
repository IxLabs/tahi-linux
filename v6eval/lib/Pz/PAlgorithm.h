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
 * $TAHI: v6eval/lib/Pz/PAlgorithm.h,v 1.8 2005/05/09 09:35:24 akisada Exp $
 */
#if !defined(__PAlgorithm_h__)
#define	__PAlgorithm_h__	1
#include "PcObject.h"

//======================================================================
// SA value
class PaObject:public PcObject{
protected:
	PaObject(const MObject*,CSTR,int);
public:
virtual	~PaObject();
virtual	WObject* selfCompose(WControl&,WObject* w_parent) const;
//----------------------------------------------------------------------
// SA FUNCTION MEMBER INTERFACE (must need as SA scope)
virtual	uint32_t ICV_Length() const;
virtual	OCTBUF*	 ICV_Calculate(const OCTBUF& ahbuf) const;
virtual	uint32_t Decrypted_Length(const OCTBUF& cryptedbuf) const;
virtual	OCTBUF*	 Decrypted_Octets(const OCTBUF& cryptedbuf,OCTBUF*& ivec) const;
virtual	uint32_t Crypted_Length(uint32_t decryptedlen) const;
virtual	OCTBUF*	 Crypted_Octets(const OCTBUF& decryptedbuf) const;
virtual	uint32_t Padding_Length(uint32_t decryptinglen) const;
virtual	OCTBUF*	 Padding_Octets(uint32_t paddinglen) const;
virtual	void icvInit() const;
virtual	void icvUpdate(OCTBUF*) const;
virtual	OCTBUF* icvResult() const;
};

class PfAuth;
//======================================================================
class PaAH:public PaObject{
private:
	PfAuth*	auth_;
public:
	PaAH(const MObject*,CSTR,int);
virtual	~PaAH();
//----------------------------------------------------------------------
// YAC INTERFACE
virtual	PObject* auth_member(PObject* p);
	PfAuth* auth() const;
//----------------------------------------------------------------------
// SA FUNCTION MEMBER INTERFACE
virtual	uint32_t ICV_Length() const;
virtual	OCTBUF*	 ICV_Calculate(const OCTBUF& ahbuf) const;
//----------------------------------------------------------------------
virtual	void print() const;
virtual	void icvInit() const;
virtual	void icvUpdate(OCTBUF*) const;
virtual	OCTBUF* icvResult() const;
virtual	void log(uint32_t=0) const;
virtual	void logSelf() const;
};
inline PfAuth* PaAH::auth() const {return auth_;}

class PfESPPad;
class PfCrypt;
//======================================================================
class PaESP:public PaAH{
protected:
	PfESPPad* pad_;
	PfCrypt* crypt_;
public:
	PaESP(const MObject*,CSTR,int);
virtual	~PaESP();
//----------------------------------------------------------------------
// YACC INTERFACE
virtual	PObject* pad_member(PObject* p);
virtual	PObject* crypt_member(PObject* p);
	PfESPPad* pad() const;
	PfCrypt* crypt() const;
//----------------------------------------------------------------------
// SA FUNCTION MEMBER INTERFACE
virtual	uint32_t ivecLength() const;
virtual	uint32_t Decrypted_Length(const OCTBUF& cryptedbuf) const;
virtual	OCTBUF*	 Decrypted_Octets(const OCTBUF& cryptedbuf,OCTBUF*& ivec) const;
virtual	uint32_t Crypted_Length(uint32_t decryptedlen) const;
virtual	OCTBUF*	 Crypted_Octets(const OCTBUF& decryptedbuf) const;
virtual	uint32_t Padding_Length(uint32_t decryptinglen) const;
virtual	OCTBUF*	 Padding_Octets(uint32_t Paddinglen) const;
//----------------------------------------------------------------------
virtual	void print() const;
virtual	void log(uint32_t=0) const;
virtual	void logSelf() const;
};
inline PfESPPad* PaESP::pad() const {return pad_;}
inline PfCrypt* PaESP::crypt() const {return crypt_;}

#include "PvObject.h"
class MfESPPad;
//======================================================================
class PfESPPad:public PFunction {
private:
	const MfESPPad* meta_;
public:
	PfESPPad(const MfESPPad*,CSTR,int);
virtual	~PfESPPad();
	const MfESPPad* metaClass() const;
//----------------------------------------------------------------------
	OCTBUF* Padding_Octets(uint32_t) const;
//----------------------------------------------------------------------
virtual	void checkArgument() const;
virtual	const MObject* meta() const;
};
inline const MfESPPad* PfESPPad::metaClass() const {return meta_;}

class MfCrypt;
//======================================================================
class PfCrypt:public PFunction {
private:
	const MfCrypt* meta_;
public:
	PfCrypt(const MfCrypt*,CSTR,int);
virtual	~PfCrypt();
	const MfCrypt* metaClass() const;
//----------------------------------------------------------------------
	uint32_t ivecLength() const;
	uint32_t Padding_Length(uint32_t,const PfAuth*) const;
	uint32_t alignment() const;
	OCTBUF* encrypt(const OCTBUF&) const;
	OCTBUF* decrypt(const OCTBUF&,OCTBUF*&) const;
//----------------------------------------------------------------------
virtual	void checkArgument() const;
virtual	const MObject* meta() const;
};
inline const MfCrypt* PfCrypt::metaClass() const {return meta_;}

class MfAuth;
//======================================================================
class PfAuth:public PFunction {
private:
	const MfAuth* meta_;
	OCTSTR context_;
public:
	PfAuth(const MfAuth*,CSTR,int);
virtual	~PfAuth();
	const MfAuth* metaClass() const;
//----------------------------------------------------------------------
	uint32_t alignment() const;
	uint32_t ICV_Length() const;
	void init();
	void update(const OCTBUF&);
	PvOctets* result();
//----------------------------------------------------------------------
// Parse TREE interface
virtual	void checkArgument() const;
virtual	const MObject* meta() const;
};
inline const MfAuth* PfAuth::metaClass() const {return meta_;}

class AlgorithmList;
//======================================================================
class SAOctets {
private:
	PvOctets* frame_;
	AlgorithmList* algorithms_;
public:
	SAOctets(PvOctets*,AlgorithmList*);
	~SAOctets();
	PvOctets* frame() const;
	AlgorithmList* algorithms() const;
	bool isEqual(const SAOctets*) const;
};
inline PvOctets* SAOctets::frame() const {return frame_;}
inline AlgorithmList* SAOctets::algorithms() const {return algorithms_;}
interfaceCmList(SAOctetsList,SAOctets);

//======================================================================
class WAlgorithm:public CmQueue {
private:
	WObject* composed_;
	AlgorithmList* algorithms_;
public:
	WAlgorithm(WObject*,AlgorithmList*);
virtual	~WAlgorithm();
	const WObject* composed() const;
	AlgorithmList* algorithms() const;
	const WObject* matchedComposed(uint32_t,RObject*) const;
	void log(uint32_t=0) const;
};
inline const WObject* WAlgorithm::composed() const {return composed_;}
inline AlgorithmList* WAlgorithm::algorithms() const {return algorithms_;}

#include "WObject.h"
//======================================================================
class AlgorithmFrames {
private:
	CmQueue algorithms_;
	WObjectList noneAlgorithms_;
public:
	AlgorithmFrames();
	~AlgorithmFrames();
	const CmQueue* algorithms() const;
	void none(WObject*);
	void alogrithm(WAlgorithm*);
	const WObjectList& noneAlgorithms() const;
	const WAlgorithm* nextAlgorithm(const CmQueue* =0) const;
	void log(uint32_t=0) const;
};
inline const CmQueue* AlgorithmFrames::algorithms() const {
	return &algorithms_;}
inline const WObjectList& AlgorithmFrames::noneAlgorithms() const {
	return noneAlgorithms_;}

#include "CmCltn.h"
interfaceCmList(AlgorithmList,PaObject);
#endif

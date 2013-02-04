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
 * $TAHI: v6eval/lib/Pz/MfAlgorithm.h,v 1.30 2009/07/31 04:41:07 akisada Exp $
 */
#if !defined(__MfAlgorithm_h__)
#define	__MfAlgorithm_h__	1
#include "MvFunction.h"
#include <openssl/des.h>
#include <openssl/evp.h>
#include <openssl/blowfish.h>
#ifndef OPENSSL_NO_RC5
#include <openssl/rc5.h>
#endif	// OPENSSL_NO_RC5
#include <openssl/cast.h>
#include <openssl/aes.h>

class PFunction;
class PObjectList;
//======================================================================
class MfESPPad:public MvFunction {
private:
static	MfESPPad* defaultPadding_;
static	MfESPPad* IKEdefaultPadding_;
	PvOctets* padding_;
public:
	MfESPPad(CSTR,PvOctets*);
virtual ~MfESPPad();
virtual int32_t token() const;
virtual PObject* tokenObject(int l,CSTR f) const;
virtual bool checkArgument(const PFunction&,const PObjectList&) const;
//----------------------------------------------------------------------
virtual	OCTBUF* Padding_Octets(uint32_t) const;
static	MfESPPad* defaultPadding();
static	MfESPPad* defaultPadding(MfESPPad*);
static	MfESPPad *IKEdefaultPadding();
static	MfESPPad *IKEdefaultPadding(MfESPPad *);
static	OCTBUF* defaultOctets(uint32_t);
static	OCTBUF* IKEdefaultOctets(uint32_t);
};
inline int32_t MfESPPad::token() const {return metaToken(tkn_padfunc_);}
inline MfESPPad* MfESPPad::defaultPadding() {return defaultPadding_;}
inline MfESPPad* MfESPPad::defaultPadding(MfESPPad* m) {
	MfESPPad* o=defaultPadding_; defaultPadding_=m; return o;}
inline MfESPPad *MfESPPad::IKEdefaultPadding() { return(IKEdefaultPadding_); }
inline MfESPPad *MfESPPad::IKEdefaultPadding(MfESPPad *m) {
	MfESPPad *o = IKEdefaultPadding_; IKEdefaultPadding_ = m; return(o); }

class PFunction;
//======================================================================
class MfAuth:public MvFunction {
private:
	uint8_t icvLength_;
	uint8_t alignUnit_;
	uint16_t dummy_;
public:
	MfAuth(CSTR,uint8_t,uint8_t);
virtual	~MfAuth();
inline	uint8_t icvLength() const;
inline	uint8_t alignUnit() const;
virtual int32_t token() const;
virtual PObject* tokenObject(int l,CSTR f) const;
virtual bool checkArgument(const PFunction&,const PObjectList&) const;
//----------------------------------------------------------------------
virtual	OCTSTR init(OCTSTR,const PObjectList&) const;
virtual	void update(OCTSTR,const PObjectList&,const OCTBUF&) const;
virtual	PvOctets* result(OCTSTR,const PObjectList&) const;
virtual	uint32_t alignment(const PObjectList&) const;
};
inline int32_t MfAuth::token() const {return metaToken(tkn_authfunc_);}
inline uint8_t MfAuth::icvLength() const {return icvLength_;}
inline uint8_t MfAuth::alignUnit() const {return alignUnit_;}

//----------------------------------------------------------------------
class MfHMAC:public MfAuth {
public:
	MfHMAC(CSTR,uint8_t,uint8_t);
virtual	~MfHMAC();
virtual bool checkArgument(const PFunction&,const PObjectList&) const;
virtual	const EVP_MD* evp() const=0;
virtual	OCTSTR init(OCTSTR,const PObjectList&) const;
virtual	void update(OCTSTR,const PObjectList&,const OCTBUF&) const;
virtual	PvOctets* result(OCTSTR,const PObjectList&) const;
};

//----------------------------------------------------------------------
class MfHMACMD5:public MfHMAC {
public:
	MfHMACMD5(CSTR,uint8_t,uint8_t);
virtual	~MfHMACMD5();
virtual	const EVP_MD* evp() const;
};

class MfHMACMD5_2: public MfHMACMD5 {
	public:
		MfHMACMD5_2(CSTR, uint8_t, uint8_t);
		virtual	~MfHMACMD5_2();
		virtual bool checkArgument(const PFunction &,
			const PObjectList &) const;
		virtual OCTSTR init(OCTSTR, const PObjectList &) const;
};

//----------------------------------------------------------------------
class MfHMACSHA1:public MfHMAC {
public:
	MfHMACSHA1(CSTR,uint8_t,uint8_t);
virtual	~MfHMACSHA1();
virtual	const EVP_MD* evp() const;
};

class MfHMACSHA1_2: public MfHMACSHA1 {
	public:
		MfHMACSHA1_2(CSTR, uint8_t, uint8_t);
		virtual	~MfHMACSHA1_2();
//		virtual	const EVP_MD *evp() const;
		virtual bool checkArgument(const PFunction &,
			const PObjectList &) const;
		virtual OCTSTR init(OCTSTR, const PObjectList &) const;
};



#if defined(OPENSSL_VERSION_NUMBER) && (OPENSSL_VERSION_NUMBER >= 0x0090800fL)
//----------------------------------------------------------------------
class MfHMACSHA2_256:public MfHMAC {
public:
	MfHMACSHA2_256(CSTR,uint8_t,uint8_t);
virtual ~MfHMACSHA2_256();
virtual const EVP_MD* evp() const;
};

//----------------------------------------------------------------------
class MfHMACSHA2_384:public MfHMAC {
public:
	MfHMACSHA2_384(CSTR,uint8_t,uint8_t);
virtual ~MfHMACSHA2_384();
virtual const EVP_MD* evp() const;
};

//----------------------------------------------------------------------
class MfHMACSHA2_512:public MfHMAC {
public:
	MfHMACSHA2_512(CSTR,uint8_t,uint8_t);
virtual ~MfHMACSHA2_512();
virtual const EVP_MD* evp() const;
};
#endif	// OPENSSL_VERSION_NUMBER



class PFunction;
//======================================================================
class MfCrypt:public MvFunction {
private:
	uint8_t keyLength_;
	uint8_t ivecLength_;
	uint8_t alignUnit_;
	uint8_t dummy_;
public:
	MfCrypt(CSTR,uint8_t,uint8_t,uint8_t);
virtual	~MfCrypt();
virtual int32_t token() const;
virtual PObject* tokenObject(int l,CSTR f) const;
virtual bool checkArgument(const PFunction&,const PObjectList&) const;
//----------------------------------------------------------------------
inline	uint8_t keyLength() const;
inline	uint8_t ivecLength() const;
inline	uint8_t alignUnit() const;
virtual	const PObject* key(const PObjectList&) const;
virtual	uint32_t alignment(const PObjectList&) const;
virtual	uint32_t alignTimes(const PObjectList&) const;
virtual	bool alignmentCheck(uint32_t,const PObjectList&) const;
virtual	OCTBUF* encryptOctets(const OCTBUF&,const PObjectList&,OCTBUF* =0) const;
virtual	OCTBUF* decryptOctets(const OCTBUF&,const PObjectList&,OCTBUF*&) const;
virtual	void encrypt(OCTSTR,OCTSTR,uint32_t,const PObject*,OCTSTR) const;
virtual	void decrypt(OCTSTR,OCTSTR,uint32_t,const PObject*,OCTSTR) const;
};
inline int32_t MfCrypt::token() const {return metaToken(tkn_cryptfunc_);}
inline uint8_t MfCrypt::keyLength() const {return keyLength_;};
inline uint8_t MfCrypt::ivecLength() const {return ivecLength_;};
inline uint8_t MfCrypt::alignUnit() const {return alignUnit_;};

//----------------------------------------------------------------------
class MfCryptKey:public MfCrypt {
public:
	MfCryptKey(CSTR,uint8_t,uint8_t,uint8_t);
virtual	~MfCryptKey();
virtual bool checkArgument(const PFunction&,const PObjectList&) const;
virtual	const PObject* key(const PObjectList&) const;
virtual	uint32_t alignTimes(const PObjectList&) const;
};

//----------------------------------------------------------------------
class MfDESCBC:public MfCryptKey {
public:
	MfDESCBC(CSTR,uint8_t,uint8_t,uint8_t);
virtual	~MfDESCBC();
	void scheduleKeys(const PObject*,des_key_schedule&) const;
virtual	void encrypt(OCTSTR,OCTSTR,uint32_t,const PObject*,OCTSTR) const;
virtual	void decrypt(OCTSTR,OCTSTR,uint32_t,const PObject*,OCTSTR) const;
};

//----------------------------------------------------------------------
class MfBLOWFISH:public MfCryptKey {
public:
	MfBLOWFISH(CSTR,uint8_t,uint8_t,uint8_t);
virtual	~MfBLOWFISH();
	void scheduleKeys(const PObject*,BF_KEY&) const;
virtual	void encrypt(OCTSTR,OCTSTR,uint32_t,const PObject*,OCTSTR) const;
virtual	void decrypt(OCTSTR,OCTSTR,uint32_t,const PObject*,OCTSTR) const;
};

//----------------------------------------------------------------------
#ifndef OPENSSL_NO_RC5
class MfRC5:public MfCryptKey {
public:
	MfRC5(CSTR,uint8_t,uint8_t,uint8_t);
virtual	~MfRC5();
	void scheduleKeys(const PObject*,RC5_32_KEY&) const;
virtual	void encrypt(OCTSTR,OCTSTR,uint32_t,const PObject*,OCTSTR) const;
virtual	void decrypt(OCTSTR,OCTSTR,uint32_t,const PObject*,OCTSTR) const;
};
#endif	// OPENSSL_NO_RC5

//----------------------------------------------------------------------
class MfCAST128:public MfCryptKey {
public:
	MfCAST128(CSTR,uint8_t,uint8_t,uint8_t);
virtual	~MfCAST128();
	void scheduleKeys(const PObject*,CAST_KEY&) const;
virtual	void encrypt(OCTSTR,OCTSTR,uint32_t,const PObject*,OCTSTR) const;
virtual	void decrypt(OCTSTR,OCTSTR,uint32_t,const PObject*,OCTSTR) const;
};

//----------------------------------------------------------------------
class MfDES3CBC:public MfCryptKey {
public:
	MfDES3CBC(CSTR,uint8_t,uint8_t,uint8_t);
virtual	~MfDES3CBC();
	void scheduleKeys(const PObject*,
		des_key_schedule&,des_key_schedule&,des_key_schedule&) const;
virtual	void encrypt(OCTSTR,OCTSTR,uint32_t,const PObject*,OCTSTR) const;
virtual	void decrypt(OCTSTR,OCTSTR,uint32_t,const PObject*,OCTSTR) const;
};



class MfIKE_DESCBC: public MfDESCBC {
	public:
		MfIKE_DESCBC(CSTR, uint8_t, uint8_t, uint8_t);
		virtual	~MfIKE_DESCBC();
		virtual bool checkArgument(const PFunction &,
			const PObjectList &) const;
		virtual int32_t token() const;

		virtual OCTBUF *encryptOctets(const OCTBUF &,
			const PObjectList &, OCTBUF * = 0) const;
		virtual OCTBUF *decryptOctets(const OCTBUF &,
			const PObjectList &, OCTBUF *&) const;
};

inline int32_t MfIKE_DESCBC::token() const {
	return(metaToken(tkn_ike_cryptfunc_)); }



class MfDESCBC_2: public MfIKE_DESCBC {
	public:
		MfDESCBC_2(CSTR, uint8_t, uint8_t, uint8_t);
		virtual	~MfDESCBC_2();
		virtual int32_t token() const;

		virtual OCTBUF *encryptOctets(const OCTBUF &,
			const PObjectList &, OCTBUF * = 0) const;
		virtual OCTBUF *decryptOctets(const OCTBUF &,
			const PObjectList &, OCTBUF *&) const;
};

inline int32_t MfDESCBC_2::token() const {
	return(metaToken(tkn_cryptfunc_)); }



class MfIKE_DES3CBC: public MfDES3CBC {
	public:
		MfIKE_DES3CBC(CSTR, uint8_t, uint8_t, uint8_t);
		virtual	~MfIKE_DES3CBC();
		virtual bool checkArgument(const PFunction &,
			const PObjectList &) const;
		virtual int32_t token() const;

		virtual OCTBUF *encryptOctets(const OCTBUF &,
			const PObjectList &, OCTBUF * = 0) const;
		virtual OCTBUF *decryptOctets(const OCTBUF &,
			const PObjectList &, OCTBUF *&) const;
};

inline int32_t MfIKE_DES3CBC::token() const {
	return(metaToken(tkn_ike_cryptfunc_)); }



class MfDES3CBC_2: public MfIKE_DES3CBC {
	public:
		MfDES3CBC_2(CSTR, uint8_t, uint8_t, uint8_t);
		virtual	~MfDES3CBC_2();
//		virtual bool checkArgument(const PFunction &,
//			const PObjectList &) const;
		virtual int32_t token() const;

		virtual OCTBUF *encryptOctets(const OCTBUF &,
			const PObjectList &, OCTBUF * = 0) const;
		virtual OCTBUF *decryptOctets(const OCTBUF &,
			const PObjectList &, OCTBUF *&) const;
};

inline int32_t MfDES3CBC_2::token() const {
	return(metaToken(tkn_cryptfunc_)); }

//----------------------------------------------------------------------
class MfRIJNDAEL:public MfCryptKey {
public:
	MfRIJNDAEL(CSTR,uint8_t,uint8_t,uint8_t);
virtual	~MfRIJNDAEL();
virtual	void encrypt(OCTSTR,OCTSTR,uint32_t,const PObject*,OCTSTR) const;
virtual	void decrypt(OCTSTR,OCTSTR,uint32_t,const PObject*,OCTSTR) const;
};

class MfRIJNDAEL_2: public MfRIJNDAEL {
	public:
		MfRIJNDAEL_2(CSTR,uint8_t,uint8_t,uint8_t);
		virtual	~MfRIJNDAEL_2();
		virtual bool checkArgument(const PFunction&, const PObjectList&) const;
		virtual OCTBUF *encryptOctets(const OCTBUF &,
			const PObjectList &, OCTBUF * = 0) const;
		virtual OCTBUF *decryptOctets(const OCTBUF &,
			const PObjectList &, OCTBUF *&) const;
};

//----------------------------------------------------------------------
extern "C" {
#include "crypto/camellia/camellia.h"
}
class MfCAMELLIA:public MfCryptKey {
public:
	MfCAMELLIA(CSTR,uint8_t,uint8_t,uint8_t);
virtual	~MfCAMELLIA();
	void scheduleKeys(const PObject*,camellia_ctx*) const;
virtual	void encrypt(OCTSTR,OCTSTR,uint32_t,const PObject*,OCTSTR) const;
virtual	void decrypt(OCTSTR,OCTSTR,uint32_t,const PObject*,OCTSTR) const;
};

//--------------------------------------------------------------------//
class MfAES_CTR: public MfCryptKey {
	public:
		MfAES_CTR(CSTR, uint8_t, uint8_t, uint8_t);
		virtual	~MfAES_CTR();
		virtual	void encrypt(OCTSTR,
			OCTSTR, uint32_t, const PObject *, OCTSTR) const;
		virtual	void decrypt(OCTSTR,
			OCTSTR, uint32_t, const PObject *, OCTSTR) const;
};

class MfAES_CTR_2: public MfAES_CTR {
	public:
		MfAES_CTR_2(CSTR, uint8_t, uint8_t, uint8_t);
		virtual	~MfAES_CTR_2();
		virtual bool checkArgument(const PFunction&, const PObjectList&) const;
		virtual OCTBUF *encryptOctets(const OCTBUF &,
			const PObjectList &, OCTBUF * = 0) const;
		virtual OCTBUF *decryptOctets(const OCTBUF &,
			const PObjectList &, OCTBUF *&) const;
};

//----------------------------------------------------------------------
class MfESPPadAny:public MfESPPad {
public:
	MfESPPadAny(CSTR,PvOctets*);
	~MfESPPadAny();
};

//----------------------------------------------------------------------
#define AES_BLOCK_SIZE 16

struct xcbc_ops {
	unsigned int len;
	u_int8_t *prev;
};

struct aes_ctx {
	AES_KEY key;
	u_int8_t iv[AES_BLOCK_SIZE];
	struct xcbc_ops *ops;
};



class MfAES_XCBC: public MfAuth {
	public:
		MfAES_XCBC(CSTR, uint8_t, uint8_t);
		virtual	~MfAES_XCBC();
		virtual bool checkArgument(const PFunction &,
			const PObjectList &) const;
		virtual	OCTSTR init(OCTSTR, const PObjectList &) const;
		virtual	void update(OCTSTR, const PObjectList &,
			const OCTBUF &) const;

		virtual	PvOctets* result(OCTSTR, const PObjectList &) const;
		struct aes_ctx *crypto_alloc_xcbc_block(void) const;
		void crypto_free_xcbc_block(struct aes_ctx *) const;
		void _crypto_xcbc_init(struct aes_ctx *,
			const u_int8_t *, const int) const;
		void xor_128(u_int8_t *, const u_int8_t *) const;
};

inline void
MfAES_XCBC::xor_128(u_int8_t *a, const u_int8_t *b) const
{
	((u_int32_t *)a)[0] ^= ((u_int32_t *)b)[0];
	((u_int32_t *)a)[1] ^= ((u_int32_t *)b)[1];
	((u_int32_t *)a)[2] ^= ((u_int32_t *)b)[2];
	((u_int32_t *)a)[3] ^= ((u_int32_t *)b)[3];

	return;
}



class MfAES_XCBC_2: public MfAES_XCBC {
	public:
		MfAES_XCBC_2(CSTR, uint8_t, uint8_t);
		virtual ~MfAES_XCBC_2();
		virtual bool checkArgument(const PFunction &,
			const PObjectList &) const;
		virtual OCTSTR init(OCTSTR, const PObjectList &) const;
		virtual	PvOctets* result(OCTSTR, const PObjectList &) const;
};



#endif

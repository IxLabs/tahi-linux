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
 * $TAHI: v6eval/lib/Pz/MfAlgorithm.cc,v 1.49 2009/07/31 04:41:07 akisada Exp $
 */
#include "MfAlgorithm.h"
#include "PAlgorithm.h"
#include "PvOctets.h"
#include "CmMain.h"
#include "sslconfig.h"
#include <string.h>
#include <netinet/in.h>

#include "debug.h"

//----------------------------------------------------------------------
OCTBUF* MfESPPad::Padding_Octets(uint32_t len) const {
	return padding_->substr(0,len);}

OCTBUF* MfESPPad::defaultOctets(uint32_t len) {
	MfESPPad* m=defaultPadding();
	return m!=0?m->Padding_Octets(len):new PvOctets(len);}

OCTBUF *
MfESPPad::IKEdefaultOctets(uint32_t len)
{
	MfESPPad *m = IKEdefaultPadding();
	return(m? m->Padding_Octets(len): new PvOctets(len));
}

//----------------------------------------------------------------------
// NULL AUTHENTIFICATION
OCTSTR MfAuth::init(OCTSTR,const PObjectList&) const {return 0;}
void MfAuth::update(OCTSTR,const PObjectList&,const OCTBUF&) const {}
PvOctets* MfAuth::result(OCTSTR,const PObjectList&) const {return 0;}
uint32_t MfAuth::alignment(const PObjectList&) const {return alignUnit();}

//----------------------------------------------------------------------
// NULL CRYPT ALGORITHM
uint32_t MfCrypt::alignment(const PObjectList& args) const {
	uint32_t n=alignTimes(args);
	uint8_t u=alignUnit();
	return n>1?n*u:u;}

bool MfCrypt::alignmentCheck(uint32_t l,const PObjectList& args) const {
	uint8_t reqAlign=alignUnit();
	if(l%reqAlign!=0) {
		printf("log: length %d not match with required alignment %d on %s\n",
			l,reqAlign,string());
		return false;}
	uint32_t funcAlign=alignment(args);
	if(l%funcAlign!=0) {
		printf("log: length %d not match with function alignment %d on %s\n",
			l,funcAlign,string());}
	return true;}

//----------------------------------------------------------------------
uint32_t MfCrypt::alignTimes(const PObjectList& args) const {
	uint32_t n=1;
	if(args.size()>0) {
		bool ok=true;
		n=args[0]->intValue(ok);
		if(!ok) n=1;}
	return n>1?n:1;}

uint32_t MfCryptKey::alignTimes(const PObjectList& args) const {
	uint32_t n=1;
	if(args.size()>2) {
		bool ok=true;
		n=args[2]->intValue(ok);
		if(!ok) n=1;}
	return n>1?n:1;}

//----------------------------------------------------------------------
const PObject* MfCrypt::key(const PObjectList&) const {
	return 0;}

const PObject* MfCryptKey::key(const PObjectList& args) const {
	return (args.size()>0)?args[0]:0;}

//----------------------------------------------------------------------
void MfCrypt::encrypt(OCTSTR os,OCTSTR is,uint32_t l,const PObject*,OCTSTR) const {
	memcpy(os,is,l);}
void MfCrypt::decrypt(OCTSTR os,OCTSTR is,uint32_t l,const PObject*,OCTSTR) const {
	memcpy(os,is,l);}

//----------------------------------------------------------------------
OCTBUF* MfCrypt::encryptOctets(const OCTBUF& text,const PObjectList& a,OCTBUF* ivec) const {
	if(DBGFLAGS('E')) {printf("===text\n"); text.dump(); printf("\n");}
	//--------------------------------------------------------------
	// length check
	uint32_t lenc=text.length();
	//--------------------------------------------------------------
	// initial vector
	// if no ivec specified in function, it's depend on malloc
	// how can i return random value?
	// should i make it error?
	OCTSTR ivp=0;
	if(a.size()>1) {
		bool ok=true;
		ivp=(OCTSTR)a[1]->octetsValue(ok);}
	uint32_t ivlen=ivecLength();
	if(ivec!=0) {ivec->set(ivlen,ivp);}
	PvOctets ivwork(ivlen,ivp,true);	// IV field changes
	PvOctets* enc=new PvOctets(ivlen+lenc);
	OCTSTR os=enc->buffer();
	OCTSTR is=(OCTSTR)text.string();
	const PObject* k=key(a);
	OCTSTR wp=ivwork.buffer();
	memcpy(os,wp,ivlen); os+=ivlen;		// initial vector
	if(!alignmentCheck(lenc,a)) {
		MfCrypt::encrypt(os,is,lenc,k,wp);}
	else {
		encrypt(os,is,lenc,k,wp);}
	if(DBGFLAGS('E')) {printf("===encrypt\n"); enc->dump(); printf("\n");}
	return enc;}

OCTBUF* MfCrypt::decryptOctets(const OCTBUF& cipher,const PObjectList& a,
		OCTBUF*& ivec) const {
	if(DBGFLAGS('D')) {printf("===cipher\n"); cipher.dump(); printf("\n");}
	//--------------------------------------------------------------
	// length check: use super class decrypt
	uint32_t lcipher=cipher.length();
	uint32_t ivlen=ivecLength();
	uint32_t ldec=lcipher-ivlen;
	//--------------------------------------------------------------
	// initial vector
	OCTSTR cp=(OCTSTR)cipher.string();
	if(ivlen>0) {
		if(ivec==0) {ivec=new PvOctets(0);}
		ivec->set(ivlen,cp);}
	PvOctets ivwork(ivlen,cp,true);		// IV field changes
	PvOctets* dec=new PvOctets(ldec);
	OCTSTR os=dec->buffer();
	OCTSTR is=cp+ivlen;
	const PObject* k=key(a);
	OCTSTR wp=ivwork.buffer();
	if(!alignmentCheck(ldec,a)) {
		MfCrypt::decrypt(os,is,ldec,k,wp);}
	else {
		decrypt(os,is,ldec,k,wp);}
	if(DBGFLAGS('D')) {printf("===decrypt\n"); dec->dump(); printf("\n");}
	return dec;}

#include <openssl/des.h>
//----------------------------------------------------------------------
// DESCBC CRYPT ALGORITHM
void MfDESCBC::scheduleKeys(const PObject* key,des_key_schedule& schd) const {
	bool ok=true;
	COCTSTR keys=key->octetsValue(ok);
	des_key_sched((DES_CBLOCK_IN)keys,schd);}

void MfDESCBC::encrypt(OCTSTR os,OCTSTR is,uint32_t l,const PObject* key,OCTSTR iv) const {
	des_key_schedule schd;
	scheduleKeys(key,schd);
	des_cbc_encrypt(is,os,l,schd,(DES_CBLOCK_IO)iv,DES_ENCRYPT);}

void MfDESCBC::decrypt(OCTSTR os,OCTSTR is,uint32_t l,const PObject* key,OCTSTR iv) const {
	des_key_schedule schd;
	scheduleKeys(key,schd);
	des_cbc_encrypt(is,os,l,schd,(DES_CBLOCK_IO)iv,DES_DECRYPT);}

#include <openssl/blowfish.h>
//----------------------------------------------------------------------
// BLOWFISH CRYPT ALGORITHM
void MfBLOWFISH::scheduleKeys(const PObject* key,BF_KEY& schd) const {
	bool ok=true;
	COCTSTR keys=key->octetsValue(ok);
	BF_set_key(&schd,key->length(),(OCTSTR)keys);}

void MfBLOWFISH::encrypt(OCTSTR os,OCTSTR is,uint32_t l,const PObject* key,OCTSTR iv) const {
	BF_KEY schd;
	scheduleKeys(key,schd);
	BF_cbc_encrypt(is,os,l,&schd,iv,DES_ENCRYPT);}

void MfBLOWFISH::decrypt(OCTSTR os,OCTSTR is,uint32_t l,const PObject* key,OCTSTR iv) const {
	BF_KEY schd;
	scheduleKeys(key,schd);
	BF_cbc_encrypt(is,os,l,&schd,iv,DES_DECRYPT);}

#ifndef OPENSSL_NO_RC5
#include <openssl/rc5.h>
//----------------------------------------------------------------------
// RC5 CRYPT ALGORITHM
void MfRC5::scheduleKeys(const PObject* key,RC5_32_KEY& schd) const {
	bool ok=true;
	COCTSTR keys=key->octetsValue(ok);
	RC5_32_set_key(&schd,key->length(),(OCTSTR)keys,RC5_16_ROUNDS);}

void MfRC5::encrypt(OCTSTR os,OCTSTR is,uint32_t l,const PObject* key,OCTSTR iv) const {
	RC5_32_KEY schd;
	scheduleKeys(key,schd);
	RC5_32_cbc_encrypt(is,os,l,&schd,iv,DES_ENCRYPT);}

void MfRC5::decrypt(OCTSTR os,OCTSTR is,uint32_t l,const PObject* key,OCTSTR iv) const {
	RC5_32_KEY schd;
	scheduleKeys(key,schd);
	RC5_32_cbc_encrypt(is,os,l,&schd,iv,DES_DECRYPT);}
#endif	// OPENSSL_NO_RC5

#include <openssl/cast.h>
//----------------------------------------------------------------------
// CAST128 CRYPT ALGORITHM
void MfCAST128::scheduleKeys(const PObject* key,CAST_KEY& schd) const {
	bool ok=true;
	COCTSTR keys=key->octetsValue(ok);
	CAST_set_key(&schd,key->length(),(OCTSTR)keys);}

void MfCAST128::encrypt(OCTSTR os,OCTSTR is,uint32_t l,const PObject* key,OCTSTR iv) const {
	CAST_KEY schd;
	scheduleKeys(key,schd);
	CAST_cbc_encrypt(is,os,l,&schd,iv,DES_ENCRYPT);}

void MfCAST128::decrypt(OCTSTR os,OCTSTR is,uint32_t l,const PObject* key,OCTSTR iv) const {
	CAST_KEY schd;
	scheduleKeys(key,schd);
	CAST_cbc_encrypt(is,os,l,&schd,iv,DES_DECRYPT);}

//----------------------------------------------------------------------
// DES3CBC CRYPT ALGORITHM
void MfDES3CBC::scheduleKeys(const PObject* key,des_key_schedule& schd0,
		des_key_schedule& schd1,des_key_schedule& schd2) const{
	bool ok=true;
	COCTSTR keys=key->octetsValue(ok);
	des_key_sched((DES_CBLOCK_IN)keys,schd0);
	des_key_sched((DES_CBLOCK_IN)(keys+8),schd1);
	des_key_sched((DES_CBLOCK_IN)(keys+16),schd2);}

void MfDES3CBC::encrypt(OCTSTR os,OCTSTR is,uint32_t l,const PObject* key,OCTSTR iv) const {
	des_key_schedule schd0,schd1,schd2;
	scheduleKeys(key,schd0,schd1,schd2);
	des_ede3_cbc_encrypt(is,os,l,schd0,schd1,schd2,(DES_CBLOCK_IO)iv,DES_ENCRYPT);}

void MfDES3CBC::decrypt(OCTSTR os,OCTSTR is,uint32_t l,const PObject* key,OCTSTR iv) const {
	des_key_schedule schd0,schd1,schd2;
	scheduleKeys(key,schd0,schd1,schd2);
	des_ede3_cbc_encrypt(is,os,l,schd0,schd1,schd2,(DES_CBLOCK_IO)iv,DES_DECRYPT);}

extern "C" {
#include <crypto/rijndael/rijndael.h>
}
//----------------------------------------------------------------------
// RIJNDAEL/AES CBC CRYPT ALGORITHM
void MfRIJNDAEL::encrypt(OCTSTR os,OCTSTR is,uint32_t l,const PObject* key,OCTSTR iv) const {
	keyInstance k;
	cipherInstance c;
	bool ok=true;
	COCTSTR keys=key->octetsValue(ok);

	rijndael_makeKey(&k, DIR_ENCRYPT, key->length() << 3, (char *)keys);
	rijndael_cipherInit(&c, MODE_CBC, (char *)iv);
	rijndael_blockEncrypt(&c, &k, is, l << 3, os);}

void MfRIJNDAEL::decrypt(OCTSTR os,OCTSTR is,uint32_t l,const PObject* key,OCTSTR iv) const {
	keyInstance k;
	cipherInstance c;
	bool ok=true;
	COCTSTR keys=key->octetsValue(ok);

	rijndael_makeKey(&k, DIR_DECRYPT, key->length() << 3, (char *)keys);
	rijndael_cipherInit(&c, MODE_CBC, (char *)iv);
	rijndael_blockDecrypt(&c, &k, is, l << 3, os);}


extern "C" {
#include "crypto/camellia/camellia.h"
}
//----------------------------------------------------------------------
//  CAMELLIA CBC CRYPT ALGORITHM
void MfCAMELLIA::scheduleKeys(const PObject* key,camellia_ctx* schd) const {
	bool ok=true;
	COCTSTR keys=key->octetsValue(ok);
	camellia_set_key(schd, (OCTSTR)keys, key->length() << 3);}

void MfCAMELLIA::encrypt(OCTSTR os,OCTSTR is,uint32_t l,const PObject* key,OCTSTR iv) const {
	camellia_ctx schd;
	scheduleKeys(key,&schd);
	camellia_cbc_encrypt(is,os,l,&schd,iv,DES_ENCRYPT);}

void MfCAMELLIA::decrypt(OCTSTR os,OCTSTR is,uint32_t l,const PObject* key,OCTSTR iv) const {
	camellia_ctx schd;
	scheduleKeys(key,&schd);
	camellia_cbc_encrypt(is,os,l,&schd,iv,DES_DECRYPT);}


//--------------------------------------------------------------------//
#ifdef AES_CTR_NONCE_SIZE
#undef AES_CTR_NONCE_SIZE
#endif	/* AES_CTR_NONCE_SIZE */

#ifdef AES_CTR_IV_SIZE
#undef AES_CTR_IV_SIZE
#endif	/* AES_CTR_IV_SIZE */

#ifndef AES_CTR_NONCE_SIZE
#define AES_CTR_NONCE_SIZE	4
#endif	/* AES_CTR_NONCE_SIZE */

#ifndef AES_CTR_IV_SIZE
#define AES_CTR_IV_SIZE		8
#endif	/* AES_CTR_IV_SIZE */



MfAES_CTR::MfAES_CTR(CSTR s, uint8_t k, uint8_t i, uint8_t a):
	MfCryptKey(s, k, i, a) {}



MfAES_CTR::~MfAES_CTR() {}



void
MfAES_CTR::encrypt(OCTSTR os,
	OCTSTR is, uint32_t l, const PObject *key, OCTSTR iv) const
{
	unsigned long length    = l;
	int bits = (key->length() - AES_CTR_NONCE_SIZE) << 3;
	unsigned int num = 0;
	AES_KEY k;
	unsigned char ecount_buf[AES_BLOCK_SIZE];



	union {
		u_int8_t bytes[AES_BLOCK_SIZE];
		struct aes_ctrblk {
			u_int32_t nonce;
			u_int8_t iv[AES_CTR_IV_SIZE];
			u_int32_t block_counter;
		} fields;
	} ctrblk;



	memset(&k, 0, sizeof(k));
	memset(&ecount_buf, 0, sizeof(ecount_buf));
	memset(&ctrblk, 0, sizeof(ctrblk));



	bool ok = true;
	COCTSTR keys = key->octetsValue(ok);

	memcpy((void *)&ctrblk.fields.nonce,
		keys + key->length() - AES_CTR_NONCE_SIZE,
		sizeof(ctrblk.fields.nonce));
	memcpy((void *)ctrblk.fields.iv, iv, sizeof(ctrblk.fields.iv));
	ctrblk.fields.block_counter = htonl(1);

	AES_set_encrypt_key(keys, bits, (AES_KEY *)&k);

	AES_ctr128_encrypt(is, os, length, &k, &ctrblk.bytes[0], ecount_buf, &num);

#ifdef AESCTR_DBG
xdbg("/tmp/aesctr_dbg.txt", "MfIKE_DES3CBC", "l: %ld\n", l);
xdmp("/tmp/aesctr_dbg.txt", "MfAES_CTR", "keys", keys, key->length());
xdmp("/tmp/aesctr_dbg.txt", "MfAES_CTR", "is", is, l);
xdmp("/tmp/aesctr_dbg.txt", "MfAES_CTR", "os", os, l);
#endif	// AESCTR_DBG

	return;
}



void
MfAES_CTR::decrypt(OCTSTR os,
	OCTSTR is, uint32_t l, const PObject *key, OCTSTR iv) const
{
	return(encrypt(os, is, l, key, iv));
}



MfAES_CTR_2::MfAES_CTR_2(CSTR s,uint8_t k,uint8_t i,uint8_t a):MfAES_CTR(s,k,i,a) {}
MfAES_CTR_2::~MfAES_CTR_2() {}

bool
MfAES_CTR_2::checkArgument(const PFunction& o, const PObjectList& a) const
{
	bool ok	= true;
	bool rc	= true;
	uint32_t n	= a.size();
	CSTR name	= o.metaString();

	if((n!=1) && (n!=2) && (n!=3)) {
		o.error("E %s mast have 1-3 arguments, not %d", name, n);
		return(false);
	}

	if(!a[0]->isOctets()) {
		o.error("E %s first argument has to be octet stream", name);
		rc = false;
	}

	if(n > 1) {
		if(!a[1]->isOctets()) {
			o.error("E %s second argument has to be octet stream", name);
			rc = false;
		}
	}

	if(n == 3) {
		uint32_t t	= a[2]->intValue(ok);

		if(!ok) {
			o.error("E %s third argument has to be int",name);
			rc	= false;
		}

		uint32_t l	= t * alignUnit();
		if(l > 256) {
			o.error("E %s alignment %d is too big", name, t);
			rc = false;
		}
	}

	return(rc);
}

OCTBUF *
MfAES_CTR_2::encryptOctets(const OCTBUF &text,
	const PObjectList &a, OCTBUF *ivec) const
{
	if(DBGFLAGS('E')) {
		printf("===text\n");
		text.dump();
		printf("\n");
	}

	//--------------------------------------------------------------
	// length check
	uint32_t lenc	= text.length();

	//--------------------------------------------------------------
	// initial vector
	// if no ivec specified in function, it's depend on malloc
	// how can i return random value?
	// should i make it error?
	OCTSTR ivp	= 0;

	if(a.size() > 1) {
		PObject *p1 = a[1];
		PvOctets tmpivec;

		if(p1->generateOctetsWith(tmpivec, 0)) {
			ivp	= tmpivec.buffer();
		}
	}

	uint32_t ivlen	= ivecLength();
	if(ivec!=0) {
		ivec->set(ivlen, ivp);
	}

	PvOctets ivwork(ivlen, ivp, true);	// IV field changes
	PvOctets *enc	= new PvOctets(ivlen + lenc);

	OCTSTR os	= enc->buffer();
	OCTSTR is	= (OCTSTR)text.string();

	PObject *p0	= a[0];
	PvOctets tmpkey;
	p0->generateOctetsWith(tmpkey, 0);
	const PObject *k	= &tmpkey;

	OCTSTR wp	= ivwork.buffer();

	memcpy(os, wp, ivlen);
	os += ivlen;	// initial vector

	if(!alignmentCheck(lenc, a)) {
		MfCrypt::encrypt(os, is, lenc, k, wp);
	} else {
		encrypt(os, is, lenc, k, wp);
	}

	if(DBGFLAGS('E')) {
		printf("===encrypt\n");
		enc->dump();
		printf("\n");
	}

	return(enc);
}



OCTBUF *
MfAES_CTR_2::decryptOctets(const OCTBUF &cipher,
	const PObjectList &a, OCTBUF *&ivec) const
{
	if(DBGFLAGS('D')) {
		printf("===cipher\n");
		cipher.dump();
		printf("\n");
	}

	//--------------------------------------------------------------
	// length check: use super class decrypt
	uint32_t lcipher	= cipher.length();
	uint32_t ivlen	= ivecLength();
	uint32_t ldec	= lcipher-ivlen;

	//--------------------------------------------------------------
	// initial vector
	OCTSTR cp	= (OCTSTR)cipher.string();

	if(ivlen>0) {
		if(ivec == 0) {
			ivec	= new PvOctets(0);
		}

		ivec->set(ivlen, cp);
	}

	PvOctets ivwork(ivlen, cp, true);		// IV field changes
	PvOctets *dec	= new PvOctets(ldec);
	OCTSTR os	= dec->buffer();
	OCTSTR is	= cp + ivlen;

	PObject *p0 = a[0];
	PvOctets tmpkey;
	p0->generateOctetsWith(tmpkey, 0);
	const PObject *k	= &tmpkey;

	OCTSTR wp	= ivwork.buffer();

	if(!alignmentCheck(ldec,a)) {
		MfCrypt::decrypt(os, is, ldec, k, wp);
	} else {
		decrypt(os, is, ldec, k, wp);
	}

	if(DBGFLAGS('D')) {
		printf("===decrypt\n");
		dec->dump();
		printf("\n");
	}

	return(dec);
}



#ifdef AES_CTR_NONCE_SIZE
#undef AES_CTR_NONCE_SIZE
#endif	/* AES_CTR_NONCE_SIZE */

#ifdef AES_CTR_IV_SIZE
#undef AES_CTR_IV_SIZE
#endif	/* AES_CTR_IV_SIZE */



#include <openssl/hmac.h>
//----------------------------------------------------------------------
// HMAC AUTHENTIFICATION ALGORITHM
OCTSTR MfHMAC::init(OCTSTR cp,const PObjectList& a) const {
	HMAC_CTX* ctx=cp!=0?(HMAC_CTX*)cp:new HMAC_CTX;
	bool ok=true;
	COCTSTR key=0;
	uint32_t keylen=0;
	if(a.size()>0) {
		key=a[0]->octetsValue(ok);
		keylen=a[0]->length();}
	HMAC_Init(ctx,(OCTSTR)key,keylen,evp());
	return (OCTSTR)ctx;}

void
MfHMAC::update(OCTSTR cp, const PObjectList &a, const OCTBUF &s) const
{
	HMAC_CTX *ctx = (HMAC_CTX *)cp;
	HMAC_Update(ctx, (OCTSTR)s.string(), s.length());

	if(a.size() > 1) {
		bool ok = true;
		uint32_t high = 0;

		high = a[1]->intValue(ok);
#ifdef ESN_DBG
xdbg("/tmp/esn_dbg.txt", "MfHMAC", "high: %ld\n", high);
xdbg("/tmp/esn_dbg.txt", "MfHMAC", "sizeof(high): %ld\n", sizeof(high));
xdmp("/tmp/esn_dbg.txt", "MfHMAC", "high", &high, sizeof(high));
#endif	// ESN_DBG
		high = htonl(high);
#ifdef ESN_DBG
xdbg("/tmp/esn_dbg.txt", "MfHMAC", "high: %ld\n", high);
xdbg("/tmp/esn_dbg.txt", "MfHMAC", "sizeof(high): %ld\n", sizeof(high));
xdmp("/tmp/esn_dbg.txt", "MfHMAC", "high", &high, sizeof(high));
#endif	// ESN_DBG
		HMAC_Update(ctx, (OCTSTR)&high, sizeof(high));
	}

	return;
}

PvOctets* MfHMAC::result(OCTSTR cp,const PObjectList&) const {
	HMAC_CTX* ctx=(HMAC_CTX*)cp;
	uint32_t len=HMAC_MAX_MD_CBLOCK;
	octet m[HMAC_MAX_MD_CBLOCK];
	HMAC_Final(ctx,m,&len);
	uint32_t icvlen=icvLength();
	PvOctets* rc=new PvOctets(icvlen);
	OCTSTR os=rc->buffer();
	memcpy(os,m,icvlen);
	HMAC_cleanup(ctx);
	return rc;}

//----------------------------------------------------------------------
// EVP ALGORITHM
const EVP_MD* MfHMAC::evp() const {return 0;}
const EVP_MD* MfHMACMD5::evp() const {return EVP_md5();}
const EVP_MD* MfHMACSHA1::evp() const {return EVP_sha1();}
#if defined(OPENSSL_VERSION_NUMBER) && (OPENSSL_VERSION_NUMBER >= 0x0090800fL)
const EVP_MD* MfHMACSHA2_256::evp() const { return EVP_sha256(); }
const EVP_MD* MfHMACSHA2_384::evp() const { return EVP_sha384(); }
const EVP_MD* MfHMACSHA2_512::evp() const { return EVP_sha512(); }
#endif

//----------------------------------------------------------------------
bool MfESPPad::checkArgument(const PFunction&,const PObjectList&) const {
	return true;}
bool MfAuth::checkArgument(const PFunction&,const PObjectList&) const {
	return true;}

//----------------------------------------------------------------------
// xxx([key, Seq# (high-order bits)])
bool
MfHMAC::checkArgument(const PFunction &o, const PObjectList &a) const
{
	bool ok = true;
	bool rc = true;
	uint32_t n = a.size();
	CSTR name = o.metaString();

	if(n != 0 && n != 1 && n != 2) {
		o.error("E %s must have 0, 1 or 2 argument, not %d", name, n);
		return(false);
	}

	if(n == 0) {
		return(true);
	}

	a[0]->octetsValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be octets",name);
		rc = false;
	}

	if(n == 2) {
		a[1]->intValue(ok);
		if(!ok) {
			o.error("E %s second argument has to be int", name);
			rc = false;
		}
	}

	return(rc);
}

//----------------------------------------------------------------------
// null([alignment])
bool MfCrypt::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	uint32_t n=a.size();
	CSTR name=o.metaString();
	if(n!=0&&n!=1) {
		o.error("E %s must have 0 or 1 argument, not %d",name,n);
		return false;}
	if(n==0) {return true;}
	a[0]->intValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be int",name);
		rc=false;}
	return rc;}

//----------------------------------------------------------------------
// xxx(key[,ivec[,alignment]])
bool MfCryptKey::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	uint32_t n=a.size();
	CSTR name=o.metaString();
	if(n!=1&&n!=2&&n!=3) {
		o.error("E %s mast have 1-3 arguments, not %d",name,n);
		return false;}
	a[0]->octetsValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be octets",name);
		rc=false;}
	else {
		uint32_t l=a[0]->length();
		uint32_t kl=keyLength();
		if(l!=kl) {
			o.error("E %s key length has to be %d, not %d",name,kl,l);
			rc=false;}}
	if(n>1) {
		a[1]->octetsValue(ok);
		if(!ok) {
			o.error("E %s second argument has to be octets",name);
			rc=false;}
		else {
			uint32_t l=a[1]->length();
			uint32_t il=ivecLength();
			if(l!=il) {
				o.error("E %s ivec length has to be %d, not %d",name,il,l);
				rc=false;}}}
	if(n==3) {
		uint32_t t=a[2]->intValue(ok);
		if(!ok) {
			o.error("E %s third argument has to be int",name);
			rc=false;}
		uint32_t l=t*alignUnit();
		if(l>256) {
			o.error("E %s alignment %d is too big",name,t);
			rc=false;}}
	return rc;}

//----------------------------------------------------------------------
PObject* MfESPPad::tokenObject(int l,CSTR f) const {
	return new PfESPPad(this,f,l);}
PObject* MfCrypt::tokenObject(int l,CSTR f) const {
	return new PfCrypt(this,f,l);}
PObject* MfAuth::tokenObject(int l,CSTR f) const {
	return new PfAuth(this,f,l);}

//----------------------------------------------------------------------
// CONSTRUCTOR/DESTRUCTOR
MfESPPad::MfESPPad(CSTR s,PvOctets* o):MvFunction(s),padding_(o) {}
MfESPPad::~MfESPPad() {}
MfCrypt::MfCrypt(CSTR s,uint8_t k,uint8_t i,uint8_t a):MvFunction(s),
	keyLength_(k),ivecLength_(i),alignUnit_(a),dummy_(0) {}
MfCrypt::~MfCrypt() {}
MfESPPadAny::MfESPPadAny(CSTR s,PvOctets* o):MfESPPad(s,o) {}
MfESPPadAny::~MfESPPadAny() {}
MfCryptKey::MfCryptKey(CSTR s,uint8_t k,uint8_t i,uint8_t a):MfCrypt(s,k,i,a) {}
MfCryptKey::~MfCryptKey() {}
MfDESCBC::MfDESCBC(CSTR s,uint8_t k,uint8_t i,uint8_t a):MfCryptKey(s,k,i,a) {}
MfDESCBC::~MfDESCBC() {}
MfBLOWFISH::MfBLOWFISH(CSTR s,uint8_t k,uint8_t i,uint8_t a):MfCryptKey(s,k,i,a) {}
MfBLOWFISH::~MfBLOWFISH() {}
#ifndef OPENSSL_NO_RC5
MfRC5::MfRC5(CSTR s,uint8_t k,uint8_t i,uint8_t a):MfCryptKey(s,k,i,a) {}
MfRC5::~MfRC5() {}
#endif	// OPENSSL_NO_RC5
MfCAST128::MfCAST128(CSTR s,uint8_t k,uint8_t i,uint8_t a):MfCryptKey(s,k,i,a) {}
MfCAST128::~MfCAST128() {}
MfDES3CBC::MfDES3CBC(CSTR s,uint8_t k,uint8_t i,uint8_t a):MfCryptKey(s,k,i,a) {}
MfDES3CBC::~MfDES3CBC() {}

MfIKE_DESCBC::MfIKE_DESCBC(CSTR s, uint8_t k, uint8_t i, uint8_t a):
	MfDESCBC(s, k, i, a) {}
MfIKE_DESCBC::~MfIKE_DESCBC() {}

MfIKE_DES3CBC::MfIKE_DES3CBC(CSTR s, uint8_t k, uint8_t i, uint8_t a):
	MfDES3CBC(s, k, i, a) {}
MfIKE_DES3CBC::~MfIKE_DES3CBC() {}

MfDESCBC_2::MfDESCBC_2(CSTR s, uint8_t k, uint8_t i, uint8_t a):
	MfIKE_DESCBC(s, k, i, a) {}
MfDESCBC_2::~MfDESCBC_2() {}

MfDES3CBC_2::MfDES3CBC_2(CSTR s, uint8_t k, uint8_t i, uint8_t a):
	MfIKE_DES3CBC(s, k, i, a) {}
MfDES3CBC_2::~MfDES3CBC_2() {}

bool
MfIKE_DESCBC::checkArgument(const PFunction &o, const PObjectList &a) const
{
	bool ok = true;
	bool rc = true;
	uint32_t n = a.size();
	CSTR name = o.metaString();

	if((n != 1) && (n != 2) && (n != 3)) {
		o.error("E %s mast have 1-3 arguments, not %d", name, n);
		return(false);
	}

	if(!a[0]->isOctets()) {
		o.error("E %s first argument has to be octet stream", name);
		rc = false;
	}

	if(n > 1) {
		if(!a[1]->isOctets()) {
			o.error("E %s second argument has to be octet stream",
				name);

			rc = false;
		}
	}

	if(n == 3) {
		uint32_t t = a[2]->intValue(ok);

		if(!ok) {
			o.error("E %s third argument has to be int", name);
			rc = false;
		}

		uint32_t l = t * alignUnit();

		if(l > 256) {
			o.error("E %s alignment %d is too big", name, t);
			rc = false;
		}
	}

	return(rc);
}

bool
MfIKE_DES3CBC::checkArgument(const PFunction &o, const PObjectList &a) const
{
	bool ok = true;
	bool rc = true;
	uint32_t n = a.size();
	CSTR name = o.metaString();

	if((n != 1) && (n != 2) && (n != 3)) {
		o.error("E %s mast have 1-3 arguments, not %d", name, n);
		return(false);
	}

	if(!a[0]->isOctets()) {
		o.error("E %s first argument has to be octet stream", name);
		rc = false;
	}

	if(n > 1) {
		if(!a[1]->isOctets()) {
			o.error("E %s second argument has to be octet stream",
				name);

			rc = false;
		}
	}

	if(n == 3) {
		uint32_t t = a[2]->intValue(ok);

		if(!ok) {
			o.error("E %s third argument has to be int", name);
			rc = false;
		}

		uint32_t l = t * alignUnit();

		if(l > 256) {
			o.error("E %s alignment %d is too big", name, t);
			rc = false;
		}
	}

	return(rc);
}

OCTBUF *
MfIKE_DESCBC::encryptOctets(const OCTBUF &text,
	const PObjectList &a, OCTBUF *ivec) const
{
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfIKE_DESCBC", "plaintext",
	text.string(), text.length());
#endif	// ISAKMP_DBG
	if(DBGFLAGS('E')) {
		printf("===text\n");
		text.dump();
		printf("\n");
	}

	uint32_t lenc = text.length();

	OCTSTR ivp = 0;
	uint32_t ivlen = ivecLength();

	PvOctets isakmp_ivec;

	if(a.size() > 1) {
		PObject *p1 = a[1];

		if(p1->generateOctetsWith(isakmp_ivec, 0)) {
			ivp = isakmp_ivec.buffer();
			ivlen = isakmp_ivec.length();
		}
	}

	if(ivec != 0) {
		ivec->set(ivlen, ivp);
	}

	PvOctets ivwork(ivlen, ivp, true);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfIKE_DESCBC", "ivec",
	ivwork.buffer(), ivwork.length());
#endif	// ISAKMP_DBG
	PvOctets *enc = new PvOctets(lenc);

	OCTSTR os = enc->buffer();
	OCTSTR is = (OCTSTR)text.string();

	PObject *p0 = a[0];
	PvOctets isakmp_key;
	p0->generateOctetsWith(isakmp_key, 0);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfIKE_DESCBC", "key",
	isakmp_key.buffer(), isakmp_key.length());
#endif	// ISAKMP_DBG
	const PObject *k = &isakmp_key;

	OCTSTR wp = ivwork.buffer();

	if(!alignmentCheck(lenc, a)) {
		MfCrypt::encrypt(os, is, lenc, k, wp);
	} else {
		encrypt(os, is, lenc, k, wp);
	}

	if(DBGFLAGS('E')) {
		printf("===encrypt\n");
		enc->dump();
		printf("\n");
	}

	return(enc);
}

OCTBUF *
MfIKE_DESCBC::decryptOctets(const OCTBUF &cipher,
	const PObjectList &a, OCTBUF *&ivec) const
{
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfIKE_DESCBC", "ciphertext",
	cipher.string(), cipher.length());
#endif	// ISAKMP_DBG
	if(DBGFLAGS('D')) {
		printf("===cipher\n");
		cipher.dump();
		printf("\n");
	}

	uint32_t lcipher	= cipher.length();
	uint32_t ivlen		= ivecLength();
	uint32_t ldec		= lcipher;

	OCTSTR cp		= (OCTSTR)cipher.string();

	OCTSTR ivp = 0;
	PvOctets isakmp_ivec;

	if(a.size() > 1) {
		PObject *p1 = a[1];

		if(p1->generateOctetsWith(isakmp_ivec, 0)) {
			ivp = isakmp_ivec.buffer();
			ivlen = isakmp_ivec.length();
		}
	}

	if(ivlen > 0) {
		if(!ivec) {
			ivec = new PvOctets(0);
		}

		ivec->set(ivlen, ivp);
	}

	PvOctets ivwork(ivlen, ivp, true);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfIKE_DESCBC", "ivec",
	ivwork.buffer(), ivwork.length());
#endif	// ISAKMP_DBG
	PvOctets *dec = new PvOctets(ldec);

	OCTSTR os = dec->buffer();
	OCTSTR is = cp;

	PObject *p0 = a[0];
	PvOctets isakmp_key;
	p0->generateOctetsWith(isakmp_key, 0);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfIKE_DESCBC", "key",
	isakmp_key.buffer(), isakmp_key.length());
#endif	// ISAKMP_DBG
	const PObject *k = &isakmp_key;
	OCTSTR wp = ivwork.buffer();

	if(!alignmentCheck(ldec,a)) {
		MfCrypt::decrypt(os, is, ldec, k, wp);
	} else {
		decrypt(os, is, ldec, k, wp);
	}

	if(DBGFLAGS('D')) {
		printf("===decrypt\n");
		dec->dump();
		printf("\n");
	}

	return(dec);
}

OCTBUF *
MfIKE_DES3CBC::encryptOctets(const OCTBUF &text,
	const PObjectList &a, OCTBUF *ivec) const
{
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfIKE_DES3CBC", "plaintext",
	text.string(), text.length());
#endif	// ISAKMP_DBG
	if(DBGFLAGS('E')) {
		printf("===text\n");
		text.dump();
		printf("\n");
	}

	uint32_t lenc = text.length();

	OCTSTR ivp = 0;
	uint32_t ivlen = ivecLength();
#ifdef ISAKMP_DBG
	xdbg("/tmp/isakmp_dbg.txt", "MfIKE_DES3CBC",
		"a.size(): %ld\n", a.size());
#endif  // ISAKMP_DBG
	PvOctets isakmp_ivec;

	if(a.size() > 1) {
		PObject *p1 = a[1];

		if(p1->generateOctetsWith(isakmp_ivec, 0)) {
			ivp = isakmp_ivec.buffer();
			ivlen = isakmp_ivec.length();
		}

#ifdef ISAKMP_DBG
	xdmp("/tmp/isakmp_dbg.txt", "MfIKE_DES3CBC", "isakmp_ivec[right]",
		isakmp_ivec.buffer(), isakmp_ivec.length());
	xdmp("/tmp/isakmp_dbg.txt", "MfIKE_DES3CBC", "isakmp_ivec[left]",
		ivp, ivlen);
#endif	// ISAKMP_DBG
	}
#ifdef ISAKMP_DBG
	xdmp("/tmp/isakmp_dbg.txt", "MfIKE_DES3CBC", "isakmp_ivec[out:0]",
		ivp, ivlen);
#endif	// ISAKMP_DBG
	if(ivec) {
		ivec->set(ivlen, ivp);
	}
#ifdef ISAKMP_DBG
	xdmp("/tmp/isakmp_dbg.txt", "MfIKE_DES3CBC", "isakmp_ivec[out:1]",
		ivp, ivlen);
#endif	// ISAKMP_DBG
	PvOctets ivwork(ivlen, ivp, true);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfIKE_DES3CBC", "ivec",
	ivwork.buffer(), ivwork.length());
#endif	// ISAKMP_DBG
	PvOctets *enc = new PvOctets(lenc);

	OCTSTR os = enc->buffer();
	OCTSTR is = (OCTSTR)text.string();

	PObject *p0 = a[0];
	PvOctets isakmp_key;
	p0->generateOctetsWith(isakmp_key, 0);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfIKE_DES3CBC", "key",
	isakmp_key.buffer(), isakmp_key.length());
#endif	// ISAKMP_DBG
	const PObject *k = &isakmp_key;

	OCTSTR wp = ivwork.buffer();

	if(!alignmentCheck(lenc, a)) {
		MfCrypt::encrypt(os, is, lenc, k, wp);
	} else {
		encrypt(os, is, lenc, k, wp);
	}

	if(DBGFLAGS('E')) {
		printf("===encrypt\n");
		enc->dump();
		printf("\n");
	}

	return(enc);
}

OCTBUF *
MfIKE_DES3CBC::decryptOctets(const OCTBUF &cipher,
	const PObjectList &a, OCTBUF *&ivec) const
{
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfIKE_DES3CBC", "ciphertext",
	cipher.string(), cipher.length());
#endif	// ISAKMP_DBG
	if(DBGFLAGS('D')) {
		printf("===cipher\n");
		cipher.dump();
		printf("\n");
	}

	uint32_t lcipher	= cipher.length();
	uint32_t ivlen		= ivecLength();
	uint32_t ldec		= lcipher;

	OCTSTR cp		= (OCTSTR)cipher.string();

	OCTSTR ivp = 0;
	PvOctets isakmp_ivec;

	if(a.size() > 1) {
		PObject *p1 = a[1];

		if(p1->generateOctetsWith(isakmp_ivec, 0)) {
			ivp = isakmp_ivec.buffer();
			ivlen = isakmp_ivec.length();
		}
	}

	if(ivlen > 0) {
		if(!ivec) {
			ivec = new PvOctets(0);
		}

		ivec->set(ivlen, ivp);
	}

	PvOctets ivwork(ivlen, ivp, true);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfIKE_DES3CBC", "ivec",
	ivwork.buffer(), ivwork.length());
#endif	// ISAKMP_DBG
	PvOctets *dec = new PvOctets(ldec);

	OCTSTR os = dec->buffer();
	OCTSTR is = cp;

	PObject *p0 = a[0];
	PvOctets isakmp_key;
	p0->generateOctetsWith(isakmp_key, 0);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfIKE_DES3CBC", "key",
	isakmp_key.buffer(), isakmp_key.length());
#endif	// ISAKMP_DBG
	const PObject *k = &isakmp_key;

	OCTSTR wp = ivwork.buffer();

	if(!alignmentCheck(ldec,a)) {
		MfCrypt::decrypt(os, is, ldec, k, wp);
	} else {
		decrypt(os, is, ldec, k, wp);
	}

	if(DBGFLAGS('D')) {
		printf("===decrypt\n");
		dec->dump();
		printf("\n");
	}

	return(dec);
}

OCTBUF *
MfDESCBC_2::encryptOctets(const OCTBUF &text,
	const PObjectList &a, OCTBUF *ivec) const
{
	if(DBGFLAGS('E')) {
		printf("===text\n");
		text.dump();
		printf("\n");
	}

        uint32_t lenc = text.length();

	OCTSTR ivp = 0;
	PvOctets ipsec_ivec;

        if(a.size() > 1) {
		PObject *p1 = a[1];

		if(p1->generateOctetsWith(ipsec_ivec, 0)) {
			ivp = ipsec_ivec.buffer();
		}
	}

	uint32_t ivlen = ivecLength();
	if(ivec) {
		ivec->set(ivlen, ivp);
	}

	PvOctets ivwork(ivlen, ivp, true);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfDESCBC_2", "ivec",
	ivwork.buffer(), ivwork.length());
#endif	// ISAKMP_DBG
	PvOctets *enc = new PvOctets(ivlen + lenc);
	OCTSTR os = enc->buffer();
	OCTSTR is = (OCTSTR)text.string();

	PObject *p0 = a[0];
	PvOctets ipsec_key;
	p0->generateOctetsWith(ipsec_key, 0);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfDESCBC_2", "key",
	ipsec_key.buffer(), ipsec_key.length());
#endif	// ISAKMP_DBG
	const PObject *k = &ipsec_key;

	OCTSTR wp = ivwork.buffer();
	memcpy(os, wp, ivlen);
	os += ivlen;

	if(!alignmentCheck(lenc, a)) {
		MfCrypt::encrypt(os, is, lenc, k, wp);
	} else {
		encrypt(os, is, lenc, k, wp);
	}

	if(DBGFLAGS('E')) {
		printf("===encrypt\n");
		enc->dump();
		printf("\n");
	}

	return(enc);
}

OCTBUF *
MfDESCBC_2::decryptOctets(const OCTBUF &cipher, const PObjectList &a,
	OCTBUF *&ivec) const
{
	if(DBGFLAGS('D')) {
		printf("===cipher\n");
		cipher.dump();
		printf("\n");
	}

        uint32_t lcipher	= cipher.length();
        uint32_t ivlen		= ivecLength();
        uint32_t ldec		= lcipher - ivlen;

	OCTSTR cp = (OCTSTR)cipher.string();

	if(ivlen > 0) {
		if(!ivec) {
			ivec = new PvOctets(0);
		}

		ivec->set(ivlen, cp);
	}

	PvOctets ivwork(ivlen, cp, true);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfDESCBC_2", "ivec",
	ivwork.buffer(), ivwork.length());
#endif	// ISAKMP_DBG
	PvOctets *dec = new PvOctets(ldec);
	OCTSTR os = dec->buffer();
	OCTSTR is = cp + ivlen;

	PObject *p0 = a[0];
	PvOctets ipsec_key;
	p0->generateOctetsWith(ipsec_key, 0);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfDESCBC_2", "key",
	ipsec_key.buffer(), ipsec_key.length());
#endif	// ISAKMP_DBG
	const PObject *k = &ipsec_key;

	OCTSTR wp = ivwork.buffer();

	if(!alignmentCheck(ldec, a)) {
		MfCrypt::decrypt(os,is,ldec,k,wp);
	} else {
		decrypt(os,is,ldec,k,wp);
	}

	if(DBGFLAGS('D')) {
		printf("===decrypt\n");
		dec->dump();
		printf("\n");
	}

	return(dec);
}

OCTBUF *
MfDES3CBC_2::encryptOctets(const OCTBUF &text,
	const PObjectList &a, OCTBUF *ivec) const
{
	if(DBGFLAGS('E')) {
		printf("===text\n");
		text.dump();
		printf("\n");
	}

        uint32_t lenc = text.length();

	OCTSTR ivp = 0;
	PvOctets ipsec_ivec;

        if(a.size() > 1) {
		PObject *p1 = a[1];

		if(p1->generateOctetsWith(ipsec_ivec, 0)) {
			ivp = ipsec_ivec.buffer();
		}
	}

	uint32_t ivlen = ivecLength();
	if(ivec) {
		ivec->set(ivlen, ivp);
	}

	PvOctets ivwork(ivlen, ivp, true);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfDES3CBC_2", "ivec",
	ivwork.buffer(), ivwork.length());
#endif	// ISAKMP_DBG
	PvOctets *enc = new PvOctets(ivlen + lenc);
	OCTSTR os = enc->buffer();
	OCTSTR is = (OCTSTR)text.string();

	PObject *p0 = a[0];
	PvOctets ipsec_key;
	p0->generateOctetsWith(ipsec_key, 0);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfDES3CBC_2", "key",
	ipsec_key.buffer(), ipsec_key.length());
#endif	// ISAKMP_DBG
	const PObject *k = &ipsec_key;

	OCTSTR wp = ivwork.buffer();
	memcpy(os, wp, ivlen);
	os += ivlen;

	if(!alignmentCheck(lenc, a)) {
		MfCrypt::encrypt(os, is, lenc, k, wp);
	} else {
		encrypt(os, is, lenc, k, wp);
	}

	if(DBGFLAGS('E')) {
		printf("===encrypt\n");
		enc->dump();
		printf("\n");
	}

	return(enc);
}

OCTBUF *
MfDES3CBC_2::decryptOctets(const OCTBUF &cipher, const PObjectList &a,
	OCTBUF *&ivec) const
{
	if(DBGFLAGS('D')) {
		printf("===cipher\n");
		cipher.dump();
		printf("\n");
	}

        uint32_t lcipher	= cipher.length();
        uint32_t ivlen		= ivecLength();
        uint32_t ldec		= lcipher - ivlen;

	OCTSTR cp = (OCTSTR)cipher.string();

	if(ivlen > 0) {
		if(!ivec) {
			ivec = new PvOctets(0);
		}

		ivec->set(ivlen, cp);
	}

	PvOctets ivwork(ivlen, cp, true);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfDES3CBC_2", "ivec",
	ivwork.buffer(), ivwork.length());
#endif	// ISAKMP_DBG
	PvOctets *dec = new PvOctets(ldec);
	OCTSTR os = dec->buffer();
	OCTSTR is = cp + ivlen;

	PObject *p0 = a[0];
	PvOctets ipsec_key;
	p0->generateOctetsWith(ipsec_key, 0);
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfDES3CBC_2", "key",
	ipsec_key.buffer(), ipsec_key.length());
#endif	// ISAKMP_DBG
	const PObject *k = &ipsec_key;

	OCTSTR wp = ivwork.buffer();
	if(!alignmentCheck(ldec, a)) {
		MfCrypt::decrypt(os,is,ldec,k,wp);
	} else {
		decrypt(os,is,ldec,k,wp);
	}

	if(DBGFLAGS('D')) {
		printf("===decrypt\n");
		dec->dump();
		printf("\n");
	}

	return(dec);
}

MfRIJNDAEL::MfRIJNDAEL(CSTR s,uint8_t k,uint8_t i,uint8_t a):MfCryptKey(s,k,i,a) {}
MfRIJNDAEL::~MfRIJNDAEL() {}

MfRIJNDAEL_2::MfRIJNDAEL_2(CSTR s,uint8_t k,uint8_t i,uint8_t a):MfRIJNDAEL(s,k,i,a) {}
MfRIJNDAEL_2::~MfRIJNDAEL_2() {}

bool
MfRIJNDAEL_2::checkArgument(const PFunction& o, const PObjectList& a) const
{
	bool ok	= true;
	bool rc	= true;
	uint32_t n	= a.size();
	CSTR name	= o.metaString();

	if((n!=1) && (n!=2) && (n!=3)) {
		o.error("E %s mast have 1-3 arguments, not %d", name, n);
		return(false);
	}

	if(!a[0]->isOctets()) {
		o.error("E %s first argument has to be octet stream", name);
		rc = false;
	}

	if(n > 1) {
		if(!a[1]->isOctets()) {
			o.error("E %s second argument has to be octet stream", name);
			rc = false;
		}
	}

	if(n == 3) {
		uint32_t t	= a[2]->intValue(ok);

		if(!ok) {
			o.error("E %s third argument has to be int",name);
			rc	= false;
		}

		uint32_t l	= t * alignUnit();
		if(l > 256) {
			o.error("E %s alignment %d is too big", name, t);
			rc = false;
		}
	}

	return(rc);
}

OCTBUF *
MfRIJNDAEL_2::encryptOctets(const OCTBUF &text,
	const PObjectList &a, OCTBUF *ivec) const
{
	if(DBGFLAGS('E')) {
		printf("===text\n");
		text.dump();
		printf("\n");
	}

	//--------------------------------------------------------------
	// length check
	uint32_t lenc	= text.length();

	//--------------------------------------------------------------
	// initial vector
	// if no ivec specified in function, it's depend on malloc
	// how can i return random value?
	// should i make it error?
	OCTSTR ivp	= 0;

	if(a.size() > 1) {
		PObject *p1 = a[1];
		PvOctets tmpivec;

		if(p1->generateOctetsWith(tmpivec, 0)) {
			ivp	= tmpivec.buffer();
		}
	}

	uint32_t ivlen	= ivecLength();
	if(ivec!=0) {
		ivec->set(ivlen, ivp);
	}

	PvOctets ivwork(ivlen, ivp, true);	// IV field changes
	PvOctets *enc	= new PvOctets(ivlen + lenc);

	OCTSTR os	= enc->buffer();
	OCTSTR is	= (OCTSTR)text.string();

	PObject *p0	= a[0];
	PvOctets tmpkey;
	p0->generateOctetsWith(tmpkey, 0);
	const PObject *k	= &tmpkey;

	OCTSTR wp	= ivwork.buffer();

	memcpy(os, wp, ivlen);
	os += ivlen;	// initial vector

	if(!alignmentCheck(lenc, a)) {
		MfCrypt::encrypt(os, is, lenc, k, wp);
	} else {
		encrypt(os, is, lenc, k, wp);
	}

	if(DBGFLAGS('E')) {
		printf("===encrypt\n");
		enc->dump();
		printf("\n");
	}

	return(enc);
}



OCTBUF *
MfRIJNDAEL_2::decryptOctets(const OCTBUF &cipher,
	const PObjectList &a, OCTBUF *&ivec) const
{
	if(DBGFLAGS('D')) {
		printf("===cipher\n");
		cipher.dump();
		printf("\n");
	}

	//--------------------------------------------------------------
	// length check: use super class decrypt
	uint32_t lcipher	= cipher.length();
	uint32_t ivlen	= ivecLength();
	uint32_t ldec	= lcipher-ivlen;

	//--------------------------------------------------------------
	// initial vector
	OCTSTR cp	= (OCTSTR)cipher.string();

	if(ivlen>0) {
		if(ivec == 0) {
			ivec	= new PvOctets(0);
		}

		ivec->set(ivlen, cp);
	}

	PvOctets ivwork(ivlen, cp, true);		// IV field changes
	PvOctets *dec	= new PvOctets(ldec);
	OCTSTR os	= dec->buffer();
	OCTSTR is	= cp + ivlen;

	PObject *p0 = a[0];
	PvOctets tmpkey;
	p0->generateOctetsWith(tmpkey, 0);
	const PObject *k	= &tmpkey;

	OCTSTR wp	= ivwork.buffer();

	if(!alignmentCheck(ldec,a)) {
		MfCrypt::decrypt(os, is, ldec, k, wp);
	} else {
		decrypt(os, is, ldec, k, wp);
	}

	if(DBGFLAGS('D')) {
		printf("===decrypt\n");
		dec->dump();
		printf("\n");
	}

	return(dec);
}

MfCAMELLIA::MfCAMELLIA(CSTR s,uint8_t k,uint8_t i,uint8_t a):MfCryptKey(s,k,i,a) {}
MfCAMELLIA::~MfCAMELLIA() {}
MfAuth::MfAuth(CSTR s,uint8_t i,uint8_t a):MvFunction(s),
	icvLength_(i),alignUnit_(a),dummy_(0) {}
MfAuth::~MfAuth() {}
MfHMAC::MfHMAC(CSTR s,uint8_t i,uint8_t a):MfAuth(s,i,a) {}
MfHMAC::~MfHMAC() {}
MfHMACMD5::MfHMACMD5(CSTR s,uint8_t i,uint8_t a):MfHMAC(s,i,a) {}
MfHMACMD5::~MfHMACMD5() {}

MfHMACMD5_2::MfHMACMD5_2(CSTR s, uint8_t i, uint8_t a): MfHMACMD5(s, i, a) {}
MfHMACMD5_2::~MfHMACMD5_2() {}

bool
MfHMACMD5_2::checkArgument(const PFunction &o,const PObjectList &a) const
{
	bool ok = true;
	bool rc = true;
	uint32_t n = a.size();
	CSTR name = o.metaString();

	if(n != 0 && n != 1 && n != 2) {
		o.error("E %s must have 0, 1 or 2 argument, not %d",name,n);
		return(false);
	}

	if(n == 0) {
		return(true);
	}

	if(!a[0]->isOctets()) {
		o.error("E %s first argument has to be octet stream", name);
		rc = false;
	}

	if(n == 2) {
		a[1]->intValue(ok);
		if(!ok) {
			o.error("E %s second argument has to be int", name);
			rc = false;
		}
	}

	return(rc);
}

OCTSTR
MfHMACMD5_2::init(OCTSTR cp, const PObjectList &a) const
{
	HMAC_CTX *ctx = cp != 0? (HMAC_CTX*)cp: new HMAC_CTX;
	COCTSTR key = 0;
	uint32_t keylen = 0;
	PvOctets hash_key;

	if(a.size() > 0) {
		PObject *p0 = a[0];

		if(p0->generateOctetsWith(hash_key, 0)) {
			key	= hash_key.buffer();
			keylen	= hash_key.length();
		}
	}

	HMAC_Init(ctx, (OCTSTR)key, keylen, evp());
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfHMACMD5_2", "HMAC_Init(key)", key, keylen);
#endif	// ISAKMP_DBG
	return((OCTSTR)ctx);
}



MfHMACSHA1::MfHMACSHA1(CSTR s,uint8_t i,uint8_t a):MfHMAC(s,i,a) {}
MfHMACSHA1::~MfHMACSHA1() {}



MfHMACSHA1_2::MfHMACSHA1_2(CSTR s, uint8_t i, uint8_t a): MfHMACSHA1(s, i, a) {}
MfHMACSHA1_2::~MfHMACSHA1_2() {}

bool
MfHMACSHA1_2::checkArgument(const PFunction &o,const PObjectList &a) const
{
	bool ok = true;
	bool rc = true;
	uint32_t n = a.size();
	CSTR name = o.metaString();

	if(n != 0 && n != 1 && n != 2) {
		o.error("E %s must have 0, 1 or 2 argument, not %d",name,n);
		return(false);
	}

	if(n == 0) {
		return(true);
	}

	if(!a[0]->isOctets()) {
		o.error("E %s first argument has to be octet stream", name);
		rc = false;
	}

	if(n == 2) {
		a[1]->intValue(ok);
		if(!ok) {
			o.error("E %s second argument has to be int", name);
			rc = false;
		}
	}

	return(rc);
}

OCTSTR
MfHMACSHA1_2::init(OCTSTR cp, const PObjectList &a) const
{
	HMAC_CTX *ctx = cp != 0? (HMAC_CTX*)cp: new HMAC_CTX;

	COCTSTR key = 0;
	uint32_t keylen = 0;
	PvOctets hash_key;

	if(a.size() > 0) {
		PObject *p0 = a[0];

		if(p0->generateOctetsWith(hash_key, 0)) {
			key	= hash_key.buffer();
			keylen	= hash_key.length();
		}
	}

	HMAC_Init(ctx, (OCTSTR)key, keylen, evp());
#ifdef ISAKMP_DBG
xdmp("/tmp/isakmp_dbg.txt", "MfHMACSHA1_2", "HMAC_Init(key)", key, keylen);
#endif	// ISAKMP_DBG
	return((OCTSTR)ctx);
}



#if defined(OPENSSL_VERSION_NUMBER) && (OPENSSL_VERSION_NUMBER >= 0x0090800fL)
MfHMACSHA2_256::MfHMACSHA2_256(CSTR s,uint8_t i,uint8_t a):MfHMAC(s,i,a) {}
MfHMACSHA2_256::~MfHMACSHA2_256() {}
MfHMACSHA2_384::MfHMACSHA2_384(CSTR s,uint8_t i,uint8_t a):MfHMAC(s,i,a) {}
MfHMACSHA2_384::~MfHMACSHA2_384() {}
MfHMACSHA2_512::MfHMACSHA2_512(CSTR s,uint8_t i,uint8_t a):MfHMAC(s,i,a) {}
MfHMACSHA2_512::~MfHMACSHA2_512() {}
#endif	// OPENSSL_VERSION_NUMBER



MfESPPad* MfESPPad::defaultPadding_=0;
MfESPPad* MfESPPad::IKEdefaultPadding_=0;

//--------------------------------------------------------------------//
const u_int32_t k1[4] = {0x01010101, 0x01010101, 0x01010101, 0x01010101};
const u_int32_t k2[4] = {0x02020202, 0x02020202, 0x02020202, 0x02020202};
const u_int32_t k3[4] = {0x03030303, 0x03030303, 0x03030303, 0x03030303};

MfAES_XCBC::MfAES_XCBC(CSTR s, uint8_t i, uint8_t a): MfAuth(s, i, a) {}
MfAES_XCBC::~MfAES_XCBC() {}

OCTSTR
MfAES_XCBC::init(OCTSTR cp, const PObjectList &a) const
{
	struct aes_ctx *ctx = 0;

	if(!(ctx = crypto_alloc_xcbc_block())) {
		fprintf(stderr, "crypto_alloc_xcbc_block err\n");
		return(0);
	}
#ifdef AESXCBC_DBG
xdbg("/tmp/aesxcbc_dbg.txt", "MfAES_XCBC", "ctx: %p\n", ctx);
#endif	// AESXCBC_DBG
	bool ok = true;
	COCTSTR key = 0;
	uint32_t keylen = 0;
	if(a.size() > 0) {
		key    = a[0]->octetsValue(ok);
		keylen = a[0]->length();
	}
#ifdef AESXCBC_DBG
xdmp("/tmp/aesxcbc_dbg.txt", "MfAES_XCBC", "key", key, keylen);
#endif	// AESXCBC_DBG
	_crypto_xcbc_init(ctx, key, keylen);
	memset(ctx->iv, 0, sizeof(ctx->iv));

	return((OCTSTR)ctx);
}

void
MfAES_XCBC::update(OCTSTR cp, const PObjectList &a, const OCTBUF &s) const
{
	struct aes_ctx *ctx  = (struct aes_ctx *)cp;
	struct xcbc_ops *ops = ctx->ops;
#ifdef AESXCBC_DBG
xdbg("/tmp/aesxcbc_dbg.txt", "MfAES_XCBC", "ctx: %p\n", ctx);
#endif	// AESXCBC_DBG
	u_int8_t *p      = (OCTSTR)s.string();
	unsigned int len = s.length();
	OCTSTR tmp = 0;
	OCTSTR bkup = 0;
	uint32_t high = 0;
	bool ok = true;

	if(a.size() > 1) {
		len += sizeof(high);
		high = a[1]->intValue(ok);
		high = htonl(high);
	}

	tmp = new octet[len];
	bkup = tmp;

	memcpy(tmp, p, s.length());

	if(a.size() > 1) {
		memcpy(tmp + s.length(), &high, sizeof(high));
	}

#ifdef AESXCBC_DBG
xdmp("/tmp/aesxcbc_dbg.txt", "MfAES_XCBC", "s", p, s.length());
xdmp("/tmp/aesxcbc_dbg.txt", "MfAES_XCBC", "tmp", tmp, len);
#endif	// AESXCBC_DBG
	unsigned int bsize = AES_BLOCK_SIZE;

	if((ops->len + len) <= bsize) {
		memcpy(ops->prev + ops->len, tmp, len);
		ops->len += len;

		if(bkup) {
			delete [] bkup; bkup = 0; tmp = 0;
		}

		return;
	}

	memcpy(ops->prev + ops->len, tmp, bsize - ops->len);
	len -= bsize - ops->len;
	tmp += bsize - ops->len;
	xor_128(ctx->iv, ops->prev);
	AES_encrypt(ctx->iv, ctx->iv, &ctx->key);

	ops->len = 0;

	while(len > bsize) {
		xor_128(ctx->iv, tmp);
		AES_encrypt(ctx->iv, ctx->iv, &ctx->key);
		tmp += bsize;
		len -= bsize;
	}

	memcpy(ops->prev, tmp, len);
	ops->len = len;

	if(bkup) {
		delete [] bkup; bkup = 0; tmp = 0;
	}

	return;
}

PvOctets *
MfAES_XCBC::result(OCTSTR cp, const PObjectList &a) const
{
	struct aes_ctx *ctx  = (struct aes_ctx *)cp;
	struct xcbc_ops *ops = ctx->ops;
	unsigned int bsize = AES_BLOCK_SIZE;
	octet out[AES_BLOCK_SIZE];

	uint32_t icvlen = icvLength();
	PvOctets *rc = new PvOctets(icvlen);
	OCTSTR os = rc->buffer();
	bool ok = true;
#ifdef AESXCBC_DBG
xdbg("/tmp/aesxcbc_dbg.txt", "MfAES_XCBC", "ctx: %p\n", ctx);
#endif	// AESXCBC_DBG
	COCTSTR key = 0;
	uint32_t keylen = 0;
	if(a.size() > 0) {
		key    = a[0]->octetsValue(ok);
		keylen = a[0]->length();
	}
#ifdef AESXCBC_DBG
xdmp("/tmp/aesxcbc_dbg.txt", "MfAES_XCBC", "key", key, keylen);
#endif	// AESXCBC_DBG
	if(ops->len == bsize) {
		u_int8_t key2[bsize];
#ifdef AESXCBC_DBG
xdbg("/tmp/aesxcbc_dbg.txt", "MfAES_XCBC", "ops->len: %d -- same\n", ops->len);
#endif	// AESXCBC_DBG
		if(AES_set_encrypt_key(key, keylen * 8, &ctx->key)) {
			return(0);
		}

		AES_encrypt((const u_int8_t *)k2, key2, &ctx->key);

		xor_128(ctx->iv, ops->prev);
		xor_128(ctx->iv, key2);

		_crypto_xcbc_init(ctx, key, keylen);

		AES_encrypt(ctx->iv, out, &ctx->key);
	} else {
		u_int8_t key3[bsize];
		unsigned int rlen;
		u_int8_t *p = ops->prev + ops->len;
#ifdef AESXCBC_DBG
xdbg("/tmp/aesxcbc_dbg.txt", "MfAES_XCBC", "ops->len: %d -- diff\n", ops->len);
#endif	// AESXCBC_DBG
		*p = 0x80;
		p++;

		rlen = bsize - ops->len - 1;
		if(rlen) {
			memset(p, 0, rlen);
		}

		if(AES_set_encrypt_key(key, keylen * 8, &ctx->key)) {
			return(0);
		}

		AES_encrypt((const u_int8_t *)k3, key3, &ctx->key);

		xor_128(ctx->iv, ops->prev);

		xor_128(ctx->iv, key3);

		_crypto_xcbc_init(ctx, key, keylen);

		AES_encrypt(ctx->iv, out, &ctx->key);
	}

	memcpy(os, out, icvlen);

	crypto_free_xcbc_block(ctx);

	return(rc);
}

struct aes_ctx *
MfAES_XCBC::crypto_alloc_xcbc_block(void) const
{
	struct aes_ctx  *ctx = 0;
	struct xcbc_ops *ops = 0;

	if(!(ctx = (struct aes_ctx *)malloc(sizeof(*ctx)))) {
		return(0);
	}

	if(!(ops = (struct xcbc_ops *)malloc(sizeof(*ops) + AES_BLOCK_SIZE))) {
		delete ctx; ctx = 0;
		return(0);
	}

	ops->len = 0;
	ops->prev = (u_int8_t *)(ops + 1);

	ctx->ops = ops;

	return(ctx);
}

void
MfAES_XCBC::crypto_free_xcbc_block(struct aes_ctx *ctx) const
{
	if(ctx) {
		if(ctx->ops) {
			delete ctx->ops; ctx->ops = 0;
		}

		delete ctx; ctx = 0;
	}

	return;
}

void
MfAES_XCBC::_crypto_xcbc_init(struct aes_ctx *ctx, const u_int8_t *key,
	const int keylen) const
{
	u_int8_t key1[16];

	if(AES_set_encrypt_key(key, keylen * 8, &ctx->key)) {
		return;
	}

	AES_encrypt((const u_int8_t *)k1, key1, &ctx->key);

	if(AES_set_encrypt_key(key1, sizeof(key1) * 8, &ctx->key)) {
		return;
	}
};

bool
MfAES_XCBC::checkArgument(const PFunction &o, const PObjectList &a) const
{
	bool ok = true;
	bool rc = true;
	uint32_t n = a.size();
	CSTR name = o.metaString();

	if(n != 0 && n != 1 && n != 2) {
		o.error("E %s must have 0, 1 or 2 argument, not %d", name, n);
		return(false);
	}

	if(n == 0) {
		return(true);
	}

	a[0]->octetsValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be octets", name);
		rc = false;
	}

	if(n == 2) {
		a[1]->intValue(ok);
		if(!ok) {
			o.error("E %s second argument has to be int", name);
			rc = false;
		}
	}

	return(rc);
}



MfAES_XCBC_2::MfAES_XCBC_2(CSTR s, uint8_t i, uint8_t a): MfAES_XCBC(s, i, a) {}
MfAES_XCBC_2::~MfAES_XCBC_2() {}

OCTSTR
MfAES_XCBC_2::init(OCTSTR cp, const PObjectList &a) const
{
	struct aes_ctx *ctx = 0;

	if(!(ctx = crypto_alloc_xcbc_block())) {
		fprintf(stderr, "crypto_alloc_xcbc_block err\n");
		return(0);
	}
#ifdef AESXCBC_DBG
xdbg("/tmp/aesxcbc_dbg.txt", "MfAES_XCBC_2", "ctx: %p\n", ctx);
#endif	// AESXCBC_DBG
#if 0
	bool ok = true;
#endif
	COCTSTR key = 0;
	uint32_t keylen = 0;
	PvOctets mac_key;

	if(a.size() > 0) {
#if 0
		key    = a[0]->octetsValue(ok);
		keylen = a[0]->length();
#else
		PObject *p0 = a[0];

		if(p0->generateOctetsWith(mac_key, 0)) {
			key	= mac_key.buffer();
			keylen	= mac_key.length();
		}
#endif
	}
#ifdef AESXCBC_DBG
xdmp("/tmp/aesxcbc_dbg.txt", "MfAES_XCBC_2", "key", key, keylen);
#endif	// AESXCBC_DBG
	_crypto_xcbc_init(ctx, key, keylen);
	memset(ctx->iv, 0, sizeof(ctx->iv));

	return((OCTSTR)ctx);
}



PvOctets *
MfAES_XCBC_2::result(OCTSTR cp, const PObjectList &a) const
{
	struct aes_ctx *ctx  = (struct aes_ctx *)cp;
	struct xcbc_ops *ops = ctx->ops;
	unsigned int bsize = AES_BLOCK_SIZE;
	octet out[AES_BLOCK_SIZE];

	uint32_t icvlen = icvLength();
	PvOctets *rc = new PvOctets(icvlen);
	OCTSTR os = rc->buffer();
#if 0
	bool ok = true;
#endif
#ifdef AESXCBC_DBG
xdbg("/tmp/aesxcbc_dbg.txt", "MfAES_XCBC_2", "ctx: %p\n", ctx);
#endif	// AESXCBC_DBG
	COCTSTR key = 0;
	uint32_t keylen = 0;
	PvOctets mac_key;

	if(a.size() > 0) {
#if 0
		key    = a[0]->octetsValue(ok);
		keylen = a[0]->length();
#else
		PObject *p0 = a[0];

		if(p0->generateOctetsWith(mac_key, 0)) {
			key	= mac_key.buffer();
			keylen	= mac_key.length();
		}
#endif
	}
#ifdef AESXCBC_DBG
xdmp("/tmp/aesxcbc_dbg.txt", "MfAES_XCBC_2", "key", key, keylen);
#endif	// AESXCBC_DBG
	if(ops->len == bsize) {
		u_int8_t key2[bsize];
#ifdef AESXCBC_DBG
xdbg("/tmp/aesxcbc_dbg.txt",
	"MfAES_XCBC_2", "ops->len: %d -- same\n", ops->len);
#endif	// AESXCBC_DBG
		if(AES_set_encrypt_key(key, keylen * 8, &ctx->key)) {
			return(0);
		}

		AES_encrypt((const u_int8_t *)k2, key2, &ctx->key);

		xor_128(ctx->iv, ops->prev);
		xor_128(ctx->iv, key2);

		_crypto_xcbc_init(ctx, key, keylen);

		AES_encrypt(ctx->iv, out, &ctx->key);
	} else {
		u_int8_t key3[bsize];
		unsigned int rlen;
		u_int8_t *p = ops->prev + ops->len;
#ifdef AESXCBC_DBG
xdbg("/tmp/aesxcbc_dbg.txt",
	"MfAES_XCBC_2", "ops->len: %d -- diff\n", ops->len);
#endif	// AESXCBC_DBG
		*p = 0x80;
		p++;

		rlen = bsize - ops->len - 1;
		if(rlen) {
			memset(p, 0, rlen);
		}

		if(AES_set_encrypt_key(key, keylen * 8, &ctx->key)) {
			return(0);
		}

		AES_encrypt((const u_int8_t *)k3, key3, &ctx->key);

		xor_128(ctx->iv, ops->prev);

		xor_128(ctx->iv, key3);

		_crypto_xcbc_init(ctx, key, keylen);

		AES_encrypt(ctx->iv, out, &ctx->key);
	}

	memcpy(os, out, icvlen);

	crypto_free_xcbc_block(ctx);

	return(rc);
}



bool
MfAES_XCBC_2::checkArgument(const PFunction &o, const PObjectList &a) const
{
	bool ok = true;
	bool rc = true;
	uint32_t n = a.size();
	CSTR name = o.metaString();

	if(n != 0 && n != 1 && n != 2) {
		o.error("E %s must have 0, 1 or 2 argument, not %d", name, n);
		return(false);
	}

	if(n == 0) {
		return(true);
	}

	if(!a[0]->isOctets()) {
		o.error("E %s first argument has to be octet stream", name);
		rc = false;
	}

	if(n == 2) {
		a[1]->intValue(ok);
		if(!ok) {
			o.error("E %s second argument has to be int", name);
			rc = false;
		}
	}

	return(rc);
}

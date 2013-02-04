/*
 *
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
 * $TAHI: v6eval/bin/sigcomp/sigcomp.c,v 1.20 2008/04/03 00:34:43 akisada Exp $
 *
 */

#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include <openssl/err.h>
#include <openssl/evp.h>

#include <openssl/rsa.h>


#ifdef DEBUG
#ifndef dbg
#define dbg(stream, fmt, args...) \
{ \
if(stream) { \
fprintf(stream, "dbg[%d]: %s: %d: %s(): " fmt, \
getpid(), __FILE__, __LINE__, __FUNCTION__, ##args); \
} \
}
#endif	// dbg



#ifndef dmp
#define dmp(stream, name, buf, buflen) \
{ \
if(stream) { \
unsigned char *buffer = (unsigned char *)buf; \
unsigned int length = (unsigned int)buflen; \
unsigned int d = 0; \
fprintf(stream, "dmp[%d]: %s: %d: %s(): %s: buffer: ", \
getpid(), __FILE__, __LINE__, __FUNCTION__, name); \
for(d = 0; d < length; d ++) { \
fprintf(stream, "%02x", buffer[d]); \
} \
fputc('\n', stream); \
fprintf(stream, "dmp[%d]: %s: %d: %s(): %s: length: %d\n", \
getpid(), __FILE__, __LINE__, __FUNCTION__, name, length); \
} \
}
#endif	// dmp
#endif	// DEBUG



typedef unsigned char bool;
static const bool false	= 0;
static const bool true	= 1;

static const char *prog	= NULL;
static bool xxxflag;



static bool sigcomp_main(const char *const, const char *const,
	const char *const, const char *const, const char *const);
static bool sigcomp_return(const bool, BIGNUM *, BIGNUM *, BIGNUM *);

static bool sigcomp_sign(const EVP_MD *const, const BIGNUM *const,
	const BIGNUM *const, const char *const);
static void sigcomp_sign_dump(const unsigned char *const, int,
	const unsigned char *const, int, const unsigned char *const, int);
static bool sigcomp_sign_return(const bool,
	unsigned char *, unsigned char *, unsigned char *, EVP_PKEY *);

static bool sigcomp_vrfy(const EVP_MD *const, const BIGNUM *const,
	const BIGNUM *const, const BIGNUM *const, const char *const);
static void sigcomp_vrfy_dump(const bool, const unsigned char *const, int,
	const unsigned char *const, int, const unsigned char *const, int,
	const unsigned char *const, int);
static bool sigcomp_vrfy_return(const bool,
	unsigned char *, unsigned char *, unsigned char *, unsigned char *,
	EVP_PKEY *);

static const EVP_MD *evp_md_with_algorithm(const char *const,
	const char *const);

static void openssl_strerror(const char *const);
static void usage(void);



int
main(int argc, char **argv, char **envp)
{
#if 0
	const char *algorithm	= NULL;
#else
	const char *algorithm	= "md5";
#endif
	const char *key		= NULL;
	const char *msg		= NULL;
	const char *signature	= NULL;
#if 0
	const char *type	= NULL;
#else
	const char *type	= "rsa";
#endif

	xxxflag	= false;

	if(!(prog = basename(argv[0]))) {
		fprintf(stderr, "err: basename: %s -- %s\n",
			strerror(errno), argv[0]);

		return(-1);
		/* NOTREACHED */
	}

	{
		int ch = 0;
#if 0
		while((ch = getopt(argc, argv, "a:k:m:s:t:@")) != -1) {
#else
		while((ch = getopt(argc, argv, "k:m:s:@")) != -1) {
#endif
			switch (ch) {
#if 0
				case 'a':
					algorithm	= optarg;
					break;
#endif
				case 'k':
					key		= optarg;
					break;
				case 'm':
					msg		= optarg;
					break;
				case 's':
					signature	= optarg;
					break;
#if 0
				case 't':
					type		= optarg;
					break;
#endif
				case '@':
					xxxflag		= true;
					break;
				default:
					usage();
					/* NOTREACHED */

					break;
			}
		}

		argc -= optind;
		argv += optind;
	}

	if(argc || !algorithm || !key || !msg || !type) {
		usage();
		/* NOTREACHED */
	}

	return(sigcomp_main(algorithm, key, msg, signature, type)? 0: -1);
	/* NOTREACHED */
}



static bool
sigcomp_main(const char *const algorithm, const char *const key,
	const char *const msg, const char *const signature,
	const char *const type)
{
	const EVP_MD *evp_md	= NULL;
	BIGNUM *bn_key		= NULL;
	BIGNUM *bn_msg		= NULL;
	BIGNUM *bn_signature	= NULL;

	ERR_load_crypto_strings();

	if(!(evp_md = evp_md_with_algorithm(type, algorithm))) {
		fprintf(stderr, "err: algorithm: %s -- "
			"unknown algorithm for %s\n", algorithm, type);

		return(sigcomp_return(false, bn_key, bn_msg, bn_signature));
	}

	/* key -> bn_key */
	if(!(bn_key = BN_new())) {
		openssl_strerror("BN_new");
		return(sigcomp_return(false, bn_key, bn_msg, bn_signature));
	}

	if(!BN_hex2bn(&bn_key, (const char *)key)) {
		openssl_strerror("BN_hex2bn");
		return(sigcomp_return(false, bn_key, bn_msg, bn_signature));
	}

	/* msg -> bn_msg */
	if(!(bn_msg = BN_new())) {
		openssl_strerror("BN_new");
		return(sigcomp_return(false, bn_key, bn_msg, bn_signature));
	}

	if(!BN_hex2bn(&bn_msg, (const char *)msg)) {
		openssl_strerror("BN_hex2bn");
		return(sigcomp_return(false, bn_key, bn_msg, bn_signature));
	}

	if(signature) {
		/* signature -> bn_signature */
		if(!(bn_signature = BN_new())) {
			openssl_strerror("BN_new");
			return(sigcomp_return(false,
				bn_key, bn_msg, bn_signature));
		}

		if(!BN_hex2bn(&bn_signature, (const char *)signature)) {
			openssl_strerror("BN_hex2bn");
			return(sigcomp_return(false,
				bn_key, bn_msg, bn_signature));
		}

		if(!sigcomp_vrfy(evp_md, bn_key, bn_msg, bn_signature, type)) {
			return(sigcomp_return(false,
				bn_key, bn_msg, bn_signature));
		}
	} else {
		if(!sigcomp_sign(evp_md, bn_key, bn_msg, type)) {
			return(sigcomp_return(false,
				bn_key, bn_msg, bn_signature));
		}
	}

	return(sigcomp_return(true, bn_key, bn_msg, bn_signature));
}



static bool
sigcomp_return(const bool code,
	BIGNUM *bn_key, BIGNUM *bn_msg, BIGNUM *bn_signature)
{
	if(bn_signature) {
		BN_free(bn_signature); bn_signature = NULL;
	}

	if(bn_msg) {
		BN_free(bn_msg); bn_msg = NULL;
	}

	if(bn_key) {
		BN_free(bn_key); bn_key = NULL;
	}

	return(code);
}



static bool
sigcomp_sign(const EVP_MD *const evp_md, const BIGNUM *const bn_key,
	const BIGNUM *const bn_msg, const char *const type)
{
	EVP_PKEY *pkey			= NULL;
	unsigned char *key		= NULL;
	unsigned char *msg		= NULL;
	unsigned char *signature	= NULL;
	unsigned char *ptr = NULL;

	int keylen		= 0;
	int msglen		= 0;
	int signaturelen	= 0;
	int padding		= 0;

	/* bn_key -> key */
	if(!(keylen = BN_num_bytes(bn_key))) {
		openssl_strerror("BN_num_bytes");
		return(sigcomp_sign_return(false, key, msg, signature, pkey));
	}

	if(!(key = (unsigned char *)malloc(keylen))) {
		fprintf(stderr, "err: malloc: %s\n", strerror(errno));
		return(sigcomp_sign_return(false, key, msg, signature, pkey));
	}

	if(BN_bn2bin(bn_key, key) != keylen) {
		openssl_strerror("BN_bn2bin");
		return(sigcomp_sign_return(false, key, msg, signature, pkey));
	}
#ifdef DEBUG
	dmp(stderr, "key", key, keylen);
#endif	// DEBUG
	/* bn_msg -> msg */
	if(!(msglen = BN_num_bytes(bn_msg))) {
		openssl_strerror("BN_num_bytes");
		return(sigcomp_sign_return(false, key, msg, signature, pkey));
	}

	if(!(msg = (unsigned char *)malloc(msglen))) {
		fprintf(stderr, "err: malloc: %s\n", strerror(errno));
		return(sigcomp_sign_return(false, key, msg, signature, pkey));
	}

	if(BN_bn2bin(bn_msg, msg) != msglen) {
		openssl_strerror("BN_bn2bin");
		return(sigcomp_sign_return(false, key, msg, signature, pkey));
	}
#ifdef DEBUG
	dmp(stderr, "msg", msg, msglen);
#endif	// DEBUG
	ptr = key;

	for( ; ; ) {
		if(!strcmp(type, "rsa")) {
#if defined(OPENSSL_VERSION_NUMBER) && (OPENSSL_VERSION_NUMBER >= 0x0090800fL)
			if(!(pkey = d2i_PrivateKey(EVP_PKEY_RSA,
				NULL, (const unsigned char **)&ptr, keylen))) {
#else
			if(!(pkey = d2i_PrivateKey(EVP_PKEY_RSA,
				NULL, &ptr, keylen))) {
#endif
				openssl_strerror("d2i_PrivateKey");
				return(sigcomp_sign_return(false,
					key, msg, signature, pkey));
			}

			signaturelen	= RSA_size(pkey->pkey.rsa);
			padding		= RSA_PKCS1_PADDING;

			break;
		}

		if(!strcmp(type, "dsa")) {
#if defined(OPENSSL_VERSION_NUMBER) && (OPENSSL_VERSION_NUMBER >= 0x0090800fL)
			if(!(pkey = d2i_PrivateKey(EVP_PKEY_DSA,
				NULL, (const unsigned char **)&ptr, keylen))) {
#else
			if(!(pkey = d2i_PrivateKey(EVP_PKEY_DSA,
				NULL, &ptr, keylen))) {
#endif

				openssl_strerror("d2i_PrivateKey");
				return(sigcomp_sign_return(false,
					key, msg, signature, pkey));
			}

			fprintf(stderr, "err: type: %s -- not implemented\n",
				type);

			return(sigcomp_sign_return(false,
				key, msg, signature, pkey));

			break;
		}

		fprintf(stderr, "err: type: %s -- unknown type\n",
			type);

		return(sigcomp_sign_return(false, key, msg, signature, pkey));
		/* NOTREACHED */
	}

	if(!(signature = (unsigned char *)malloc(signaturelen))) {
		fprintf(stderr, "err: malloc: %s\n", strerror(errno));
		return(sigcomp_sign_return(false, key, msg, signature, pkey));
	}

	RSA_private_encrypt(msglen, msg, signature, pkey->pkey.rsa, padding);
#ifdef DEBUG
	dmp(stderr, "signature", signature, signaturelen);
#endif	// DEBUG
	sigcomp_sign_dump(key, keylen, msg, msglen, signature, signaturelen);

	return(sigcomp_sign_return(true, key, msg, signature, pkey));
}



static void
sigcomp_sign_dump(const unsigned char *const key, int keylen,
	const unsigned char *const msg, int msglen,
	const unsigned char *const signature, int signaturelen)
{
	int d = 0;

	if(xxxflag) {
#if 0	/* needless */
		for(d = 0; d < msglen; d ++) {
			printf("%02x", msg[d]);
		}
#endif
		for(d = 0; d < signaturelen; d ++) {
			printf("%02x", signature[d]);
		}

		printf("\n");

		return;
	}

	printf("log:SigCompSign_Results             (length:%d)\n",
		keylen + msglen + signaturelen);

	printf("log:| PrivateKey                      (length:%d)\n", keylen);
	printf("log:| | data                             = ");

	for(d = 0; d < keylen; d ++) {
		printf("%02x", key[d]);
	}

	printf("\n");

	printf("log:| Message                         (length:%d)\n", msglen);
	printf("log:| | data                             = ");

	for(d = 0; d < msglen; d ++) {
		printf("%02x", msg[d]);
	}

	printf("\n");

	printf("log:| Signature                       (length:%d)\n",
		signaturelen);

	printf("log:| | data                             = ");

	for(d = 0; d < signaturelen; d ++) {
		printf("%02x", signature[d]);
	}

	printf("\n");

	return;
}



static bool
sigcomp_sign_return(const bool code,
	unsigned char *key, unsigned char *msg, unsigned char *signature,
	EVP_PKEY *pkey)
{
	if(pkey) {
		EVP_PKEY_free(pkey); pkey = NULL;
	}

	if(signature) {
		free(signature); signature = NULL;
	}

	if(msg) {
		free(msg); msg = NULL;
	}

	if(key) {
		free(key); key = NULL;
	}

	return(code);
}



static bool
sigcomp_vrfy(const EVP_MD *const evp_md, const BIGNUM *const bn_key,
	const BIGNUM *const bn_msg,
	const BIGNUM *const bn_signature, const char *const type)
{
	EVP_PKEY *pkey  = NULL;

	unsigned char *key		= NULL;
	unsigned char *msg		= NULL;
	unsigned char *signature	= NULL;
	unsigned char *to		= NULL;
	unsigned char *ptr		= NULL;

	int keylen		= 0;
	int msglen		= 0;
	int signaturelen	= 0;
	int tolen		= 0;
	int padding		= 0;

	bool compare		= true;

	/* bn_key -> key */
	if(!(keylen = BN_num_bytes(bn_key))) {
		openssl_strerror("BN_num_bytes");
		return(sigcomp_vrfy_return(false,
			key, msg, signature, to, pkey));
	}

	if(!(key = (unsigned char *)malloc(keylen))) {
		fprintf(stderr, "err: malloc: %s\n", strerror(errno));
		return(sigcomp_vrfy_return(false,
			key, msg, signature, to, pkey));
	}

	if(BN_bn2bin(bn_key, key) != keylen) {
		openssl_strerror("BN_bn2bin");
		return(sigcomp_vrfy_return(false,
			key, msg, signature, to, pkey));
	}
#ifdef DEBUG
	dmp(stderr, "key", key, keylen);
#endif  // DEBUG
	/* bn_msg -> msg */
        if(!(msglen = BN_num_bytes(bn_msg))) {
		openssl_strerror("BN_num_bytes");
		return(sigcomp_vrfy_return(false,
			key, msg, signature, to, pkey));
        }

	if(!(msg = (unsigned char *)malloc(msglen))) {
		fprintf(stderr, "err: malloc: %s\n", strerror(errno));
		return(sigcomp_vrfy_return(false,
			key, msg, signature, to, pkey));
	}

	if(BN_bn2bin(bn_msg, msg) != msglen) {
		openssl_strerror("BN_bn2bin");
		return(sigcomp_vrfy_return(false,
			key, msg, signature, to, pkey));
	}
#ifdef DEBUG
	dmp(stderr, "msg", msg, msglen);
#endif  // DEBUG
	/* bn_signature -> signature */
        if(!(signaturelen = BN_num_bytes(bn_signature))) {
		openssl_strerror("BN_num_bytes");
		return(sigcomp_vrfy_return(false,
			key, msg, signature, to, pkey));
        }

	if(!(signature = (unsigned char *)malloc(signaturelen))) {
		fprintf(stderr, "err: malloc: %s\n", strerror(errno));
		return(sigcomp_vrfy_return(false,
			key, msg, signature, to, pkey));
	}

	if(BN_bn2bin(bn_signature, signature) != signaturelen) {
		openssl_strerror("BN_bn2bin");
		return(sigcomp_vrfy_return(false,
			key, msg, signature, to, pkey));
	}
#ifdef DEBUG
	dmp(stderr, "signature", signature, signaturelen);
#endif  // DEBUG
        ptr = key;

	for( ; ; ) {
		if(!strcmp(type, "rsa")) {
#if defined(OPENSSL_VERSION_NUMBER) && (OPENSSL_VERSION_NUMBER >= 0x0090800fL)
			if(!(pkey = d2i_PublicKey(EVP_PKEY_RSA,
				NULL, (const unsigned char **)&ptr, keylen))) {
#else
			if(!(pkey = d2i_PublicKey(EVP_PKEY_RSA,
				NULL, &ptr, keylen))) {
#endif

				openssl_strerror("d2i_PublicKey");
				return(sigcomp_vrfy_return(false,
					key, msg, signature, to, pkey));
			}

			tolen		= RSA_size(pkey->pkey.rsa);
			padding		= RSA_PKCS1_PADDING;

			break;
		}

		if(!strcmp(type, "dsa")) {
#if defined(OPENSSL_VERSION_NUMBER) && (OPENSSL_VERSION_NUMBER >= 0x0090800fL)
			if(!(pkey = d2i_PublicKey(EVP_PKEY_DSA,
				NULL, (const unsigned char **)&ptr, keylen))) {
#else
			if(!(pkey = d2i_PublicKey(EVP_PKEY_DSA,
				NULL, &ptr, keylen))) {
#endif

				openssl_strerror("d2i_PublicKey");
				return(sigcomp_vrfy_return(false,
					key, msg, signature, to, pkey));
			}

			fprintf(stderr, "err: type: %s -- not implemented\n",
				type);

			return(sigcomp_vrfy_return(false,
				key, msg, signature, to, pkey));
			/* NOTREACHED */

			break;
		}

		fprintf(stderr, "err: type: %s -- unknown type\n",
			type);

		return(sigcomp_vrfy_return(false,
			key, msg, signature, to, pkey));
		/* NOTREACHED */
	}

	if(!(to = (unsigned char *)malloc(tolen))) {
		fprintf(stderr, "err: malloc: %s\n", strerror(errno));
		return(sigcomp_vrfy_return(false,
			key, msg, signature, to, pkey));
	}

	if(RSA_public_decrypt(signaturelen,
		signature, to, pkey->pkey.rsa, padding) < 0) {

		openssl_strerror("RSA_public_decrypt");
		return(sigcomp_vrfy_return(true,
			key, msg, signature, to, pkey));
	}

	if(memcmp(msg, to, msglen)) {
		compare	= false;
	}

	sigcomp_vrfy_dump(compare, key, keylen,
		msg, msglen, signature, signaturelen, to, tolen);

	return(sigcomp_vrfy_return(true, key, msg, signature, to, pkey));
}



static void
sigcomp_vrfy_dump(const bool code, const unsigned char *const key, int keylen,
	const unsigned char *const msg, int msglen,
	const unsigned char *const signature, int signaturelen,
	const unsigned char *const to, int tolen)
{
	int d = 0;

	printf("log:SigCompVrfy_Results             (length:%d)\n",
		keylen + msglen + signaturelen + tolen);

	printf("log:| PublicKey                       (length:%d)\n", keylen);
	printf("log:| | data                             = ");

	for(d = 0; d < keylen; d ++) {
		printf("%02x", key[d]);
	}

	printf("\n");

	printf("log:| Message                         (length:%d)\n", msglen);
	printf("log:| | data                             = ");

	for(d = 0; d < msglen; d ++) {
		printf("%02x", msg[d]);
	}

	printf("\n");

	printf("log:| Signature                       (length:%d)\n",
		signaturelen + tolen);

	printf("log:| | status                           = %s\n",
		code? "true": "false");

	printf("log:| | Encrypted                       (length:%d)\n",
		signaturelen);

	printf("log:| | | data                             = ");

	for(d = 0; d < signaturelen; d ++) {
		printf("%02x", signature[d]);
	}

	printf("\n");

	printf("log:| | Decrypted                       (length:%d)\n", tolen);
	printf("log:| | | data                             = ");

	for(d = 0; d < tolen; d ++) {
		printf("%02x", to[d]);
	}

	printf("\n");

	return;
}



static bool
sigcomp_vrfy_return(const bool code,
	unsigned char *key, unsigned char *msg, unsigned char *signature,
	unsigned char *to, EVP_PKEY *pkey)
{
	if(pkey) {
		EVP_PKEY_free(pkey); pkey = NULL;
	}

	if(to) {
		free(to); to = NULL;
	}

	if(signature) {
		free(signature); signature = NULL;
	}

	if(msg) {
		free(msg); msg = NULL;
	}

	if(key) {
		free(key); key = NULL;
	}

	return(code);
}



static const EVP_MD *
evp_md_with_algorithm(const char *const type, const char *const algorithm)
{
	const EVP_MD *evp_md	= NULL;
	bool dsa	= false;
	bool rsa	= false;

	for( ; ; ) {
		if(!strcmp(type, "dsa")) {
			dsa = true;
			break;
		}

		if(!strcmp(type, "rsa")) {
			rsa = true;
			break;
		}

		break;
	}

	for( ; ; ) {
		if(dsa && !strcmp(algorithm, "dss1")) {
			evp_md = EVP_dss1();
			break;
		}

		if(rsa && !strcmp(algorithm, "md2")) {
			evp_md = EVP_md2();
			break;
		}

		if(rsa && !strcmp(algorithm, "md4")) {
			evp_md = EVP_md4();
			break;
		}

		if(rsa && !strcmp(algorithm, "md5")) {
			evp_md = EVP_md5();
			break;
		}

		if(rsa && !strcmp(algorithm, "mdc2")) {
			evp_md = EVP_mdc2();
			break;
		}

		if(rsa && !strcmp(algorithm, "ripemd160")) {
			evp_md = EVP_ripemd160();
			break;
		}

		if(rsa && !strcmp(algorithm, "sha1")) {
			evp_md = EVP_sha1();
			break;
		}

		break;
	}

	return(evp_md);
}



static void
openssl_strerror(const char *const label)
{
	unsigned long code = 0;

	while((code = ERR_get_error())) {
		fprintf(stderr, "err: %s: %s\n",
			label, ERR_error_string(code, NULL));
	}

	return;
}



static void
usage(void)
{
#if 0
	fprintf(stderr,
		"err: usage: %s -a algorithm -k key -m msg -t type\n", prog);
	fprintf(stderr,
		"err:        %s -a algorithm -k key -m msg -s signature "
		"-t type\n", prog);

	fprintf(stderr, "err: \n");
	fprintf(stderr, "err:            algorithm: dss1\n");
	fprintf(stderr, "err:            type:      dsa\n");
	fprintf(stderr, "err: \n");
	fprintf(stderr, "err:            algorithm: "
		"md2, md4, md5, sha1, mdc2, ripemd160\n");
	fprintf(stderr, "err:            type:      rsa\n");
	fprintf(stderr, "err: \n");
#else
	fprintf(stderr,
		"err: usage: %s -k privatekey -m msg\n", prog);

	fprintf(stderr,
		"err:        %s -k publickey  -m msg -s signature\n", prog);
#endif

	exit(-1);
	/* NOTREACHED */

	return;
}

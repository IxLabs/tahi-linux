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
 * $TAHI: v6eval/bin/x509dec/x509dec.c,v 1.15 2008/04/03 01:09:48 akisada Exp $
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <errno.h>
#include <unistd.h>

#include <sys/queue.h>

#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include "x509dec.h"


typedef unsigned char bool;
static const bool false	= 0;
static const bool true	= 1;

static const char *prog = NULL;



static bool x509dec_main(const char *const);
static bool x509dec_main_return(const bool, BIGNUM *);

static bool x509dec_on_x509(const BIGNUM *const);
static bool x509dec_on_x509_return(const bool, X509 *, unsigned char *);

static bool x509dec_on_evp(const X509 *const);
static void x509dec_dump(const unsigned char *const, const int,
	const unsigned char *const, const int, struct tailq *);
static bool x509dec_on_evp_return(const bool,
	unsigned char *, unsigned char *);

static unsigned char *X509dec_get_pubkey(const X509 *const, int *);
static unsigned char *X509dec_get_pubkey_return(unsigned char *const pubkey,
	unsigned char *, EVP_PKEY *);

static unsigned char *X509dec_get_subject_name(const X509 *const, int *);
static const unsigned char *X509dec_get_subject_alt_name(const X509 *const,
struct tailq *);

static void openssl_strerror(const char *const);
static void usage(void);



int
main(int argc, char **argv, char **envp)
{
	const char *dertxt = NULL;

	if(!(prog = basename(argv[0]))) {
		fprintf(stderr, "err: basename: %s -- %s\n",
			strerror(errno), argv[0]);

		return(-1);
		/* NOTREACHED */
	}

	{
		int ch = 0;

		while((ch = getopt(argc, argv, "d:")) != -1) {
			switch (ch) {
				case 'd':
					dertxt	= optarg;
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

	if(argc || !dertxt) {
		usage();
		/* NOTREACHED */
	}

	return(x509dec_main(dertxt)? 0: -1);
	/* NOTREACHED */
}



static bool
x509dec_main(const char *const dertxt)
{
	BIGNUM *bignum = NULL;

	ERR_load_crypto_strings();

	if(!(bignum = BN_new())) {
		openssl_strerror("BN_new");
		return(x509dec_main_return(false, bignum));
	}

	if(!BN_hex2bn(&bignum, (const char *)dertxt)) {
		openssl_strerror("BN_hex2bn");
		return(x509dec_main_return(false, bignum));
	}

	if(!x509dec_on_x509(bignum)) {
		return(x509dec_main_return(false, bignum));
	}

	return(x509dec_main_return(true, bignum));
}



static bool
x509dec_main_return(const bool code, BIGNUM *bignum)
{
	if(bignum) {
		BN_free(bignum); bignum = 0;
	}

	return(code);
}



static bool
x509dec_on_x509(const BIGNUM *const bignum)
{
	int buflen = 0;
	unsigned char *buf = NULL, *ptr = NULL;
	X509 *x509 = NULL;

	if(!(buflen = BN_num_bytes(bignum))) {
		openssl_strerror("BN_num_bytes");
		return(x509dec_on_x509_return(false, x509, buf));
	}

	if(!(buf = (unsigned char *)malloc(buflen))) {
		fprintf(stderr, "err: malloc: %s\n", strerror(errno));
		return(x509dec_on_x509_return(false, x509, buf));
	}

	if(BN_bn2bin(bignum, buf) != buflen) {
		openssl_strerror("BN_bn2bin");
		return(x509dec_on_x509_return(false, x509, buf));
	}

	ptr = buf;

#if defined(OPENSSL_VERSION_NUMBER) && (OPENSSL_VERSION_NUMBER >= 0x0090800fL)
	if(!(x509 = d2i_X509(NULL, (const unsigned char **)&ptr, buflen))) {
#else
	if(!(x509 = d2i_X509(NULL, &ptr, buflen))) {
#endif
		openssl_strerror("d2i_X509");
		return(x509dec_on_x509_return(false, x509, buf));
	}

	if(!x509dec_on_evp(x509)) {
		return(x509dec_on_x509_return(false, x509, buf));
	}

	return(x509dec_on_x509_return(true, x509, buf));
}



static bool
x509dec_on_x509_return(const bool code, X509 *x509, unsigned char *buf)
{
	if(x509) {
		X509_free(x509); x509 = 0;
	}

	if(buf) {
		free(buf); buf = 0;
	}

	return(code);
}



static bool
x509dec_on_evp(const X509 *const x509)
{
	unsigned char *pubkey = NULL;
	int pubkeylen = 0;

	unsigned char *subject_name = NULL;
	int subject_namelen = 0;

	struct tailq head = TAILQ_HEAD_INITIALIZER(head);

	TAILQ_INIT(&head);

	if(!(pubkey =
		X509dec_get_pubkey(x509, &pubkeylen))) {

		return(x509dec_on_evp_return(false, pubkey, subject_name));
	}

	if(!(subject_name =
		X509dec_get_subject_name(x509, (int *)&subject_namelen))) {

		return(x509dec_on_evp_return(false, pubkey, subject_name));
	}

	X509dec_get_subject_alt_name(x509, (struct tailq *)&head);

	x509dec_dump(pubkey, pubkeylen,
		subject_name, subject_namelen, (struct tailq *)&head);

	return(x509dec_on_evp_return(true, pubkey, subject_name));
}



static void
x509dec_dump(const unsigned char *const pubkey, const int pubkeylen,
	const unsigned char *const subject_name, const int subject_namelen,
	struct tailq *head)
{
	int d = 0;
	int total = 0;
	int extlen = 0;
	struct name *name = NULL;

	total = pubkeylen + subject_namelen;

	TAILQ_FOREACH(name, head, names) {
		extlen += name->name_len;
	}

	total += extlen;

	printf("log:X509dec_Results                 (length:%d)\n", total);

	printf("log:| SubjectName                     (length:%d)\n",
		subject_namelen);
	printf("log:| | data                             = ");

	for(d = 0; d < subject_namelen; d ++) {
		printf("%02x", subject_name[d]);
	}

	printf("\n");

	printf("log:| PublicKey                       (length:%d)\n",
		pubkeylen);
	printf("log:| | data                             = ");

	for(d = 0; d < pubkeylen; d ++) {
		printf("%02x", pubkey[d]);
	}

	printf("\n");

	if(extlen) {
		printf("log:| Extensions                      (length:%d)\n",
			extlen);

		TAILQ_FOREACH(name, head, names) {
			printf("log:| | SubjectAltName                  (length:%d)\n",
				name->name_len);
			printf("log:| | | Type                             = %d\n", name->name_type);
			printf("log:| | | Data                            (length:%d)\n", name->name_len);
			printf("log:| | | | data                             = ");

			for(d = 0; d < name->name_len; d ++) {
				printf("%02x", name->name_buf[d]);
			}

			printf("\n");
		}
	}
}



static bool
x509dec_on_evp_return(const bool code,
	unsigned char *pubkey, unsigned char *subject_name)
{
	if(subject_name) {
		free(subject_name); subject_name = NULL;
	}

	if(pubkey) {
		free(pubkey); pubkey = NULL;
	}

	return(code);
}



static unsigned char *
X509dec_get_pubkey(const X509 *const x509, int *buflen)
{
	EVP_PKEY *evp = NULL;
	unsigned char *buf = NULL, *ptr = NULL;
#if 0
	int evp_pkey_type = 0;
	const BIGNUM *bignum = NULL;
#endif
	if(!(evp = X509_get_pubkey((X509 *)x509))) {
		openssl_strerror("X509_get_pubkey");
		return(X509dec_get_pubkey_return(NULL, buf, evp));
	}
#if 0
	evp_pkey_type = EVP_PKEY_type(evp->type);

	switch(evp_pkey_type) {
		case EVP_PKEY_RSA:
			*buflen = BN_num_bytes(evp->pkey.rsa->n);
			bignum = evp->pkey.rsa->n;

			break;
		case EVP_PKEY_DSA:
			*buflen = BN_num_bytes(evp->pkey.dsa->pub_key);
			bignum = evp->pkey.dsa->pub_key;

			break;
		default:
			fprintf(stderr,
				"err: EVP_PKEY_type: unknown type -- %d\n",
				evp_pkey_type);

			return(X509dec_get_pubkey_return(NULL, buf, evp));

			break;
	}

	if(!(buf = (unsigned char *)malloc(*buflen))) {
		fprintf(stderr, "err: malloc: %s\n", strerror(errno));
		return(X509dec_get_pubkey_return(NULL, buf, evp));
	}

	if(BN_bn2bin(bignum, buf) != *buflen) {
		openssl_strerror("BN_bn2bin");
		return(X509dec_get_pubkey_return(NULL, buf, evp));
	}
#else
	if(!(*buflen = i2d_PublicKey(evp, NULL))) {
		openssl_strerror("i2d_PublicKey");
		return(X509dec_get_pubkey_return(NULL, buf, evp));
	}

	if(!(buf = (unsigned char *)malloc(*buflen))) {
		fprintf(stderr, "err: malloc: %s\n", strerror(errno));
		return(X509dec_get_pubkey_return(NULL, buf, evp));
	}

	ptr = buf;

	if(i2d_PublicKey(evp, &ptr) != *buflen) {
		openssl_strerror("i2d_PublicKey");
		return(X509dec_get_pubkey_return(NULL, buf, evp));
	}
#endif
	return(X509dec_get_pubkey_return(buf, buf, evp));
}



static unsigned char *
X509dec_get_pubkey_return(unsigned char *const pubkey,
	unsigned char *buf, EVP_PKEY *evp)
{
	if(!pubkey && buf) {
		free(buf); buf = NULL;
	}

	if(evp) {
		EVP_PKEY_free(evp); evp = NULL;
	}

	return(pubkey);
}



static unsigned char *
X509dec_get_subject_name(const X509 *const x509, int *buflen)
{
	X509_NAME *subject_name = NULL;
	unsigned char *buf = NULL, *ptr = NULL;

	if(!(subject_name = X509_get_subject_name((X509 *)x509))) {
		openssl_strerror("X509_get_pubkey");
		return(NULL);
	}

	if(!(*buflen = i2d_X509_NAME(subject_name, NULL))) {
		openssl_strerror("i2d_X509_NAME");
		return(NULL);
	}

	if(!(buf = (unsigned char *)malloc(*buflen))) {
		fprintf(stderr, "err: malloc: %s\n", strerror(errno));
		return(NULL);
	}

	ptr = buf;

	if(i2d_X509_NAME(subject_name, &ptr) != *buflen) {
		openssl_strerror("i2d_X509_NAME");
		return(NULL);
	}

	return(buf);
}



static const unsigned char *
X509dec_get_subject_alt_name(const X509 *const x509, struct tailq *head)
{
	GENERAL_NAMES	*extensions	= NULL;
	GENERAL_NAME	*extension	= NULL;
	int subjAltName_pos		= 0;

	if(!(extensions = X509_get_ext_d2i((X509 *)x509,
		NID_subject_alt_name, NULL, NULL))) {

		openssl_strerror("X509_get_ext_d2i");
		return(NULL);
	}

	for(subjAltName_pos = 0;
		subjAltName_pos < sk_GENERAL_NAME_num(extensions);
		subjAltName_pos ++) {

		struct name *name = NULL;

		extension = sk_GENERAL_NAME_value(extensions, subjAltName_pos);

		if(!(name = (struct name *)malloc(sizeof(struct name)))) {
			fprintf(stderr, "err: malloc: %s\n", strerror(errno));
			return(NULL);
		}

		if(extension->d.ia5->data[extension->d.ia5->length] != '\0') {
			fprintf(stderr,
				"err: data is not terminated by '\\0'.\n");

			return(NULL);
		}

		name->name_len = extension->d.ia5->length + 1;

		if(!(name->name_buf =
			(unsigned char *)malloc(name->name_len))) {

			fprintf(stderr, "err: malloc: %s\n", strerror(errno));
			return(NULL);
		}

		strlcpy((char *)name->name_buf, (const char *)extension->d.ia5->data,
			name->name_len);
		name->name_type = extension->type;

		TAILQ_INSERT_TAIL(head, name, names);
	}

	return(NULL);
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
	fprintf(stderr, "err: usage: %s -d der\n", prog);
	exit(-1);
	/* NOTREACHED */

	return;
}

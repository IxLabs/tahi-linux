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
 * $TAHI: v6eval/bin/ivcomp/ivcomp.c,v 1.3 2008/04/02 06:09:02 akisada Exp $
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <libgen.h>
#include <unistd.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

typedef unsigned char bool;
const bool false	= 0;
const bool true		= 1;

static const unsigned char *prog = NULL;

int ivcomp_main(const unsigned char *const, const unsigned char *const);
int md5_main(const unsigned char *const, unsigned int);
int sha1_main(const unsigned char *const, unsigned int);
bool hex_pton(const unsigned char *const, const unsigned int,
	unsigned char *const, const unsigned int);
void ivcomp_dump(const unsigned char *const,
	const unsigned char *const, const unsigned int,
	const unsigned char *const, const unsigned int);
void usage(void);

int
main(int argc, char **argv, char **envp)
{
	const unsigned char *algorithm  = NULL;
	const unsigned char *msg        = NULL;

	if(!(prog = (unsigned char *)basename(argv[0]))) {
		fprintf(stderr, "err: basename: %s -- %s\n",
			strerror(errno), argv[0]);

		return(-1);
		/* NOTREACHED */
	}

	{
		int ch = 0;

		while((ch = getopt(argc, argv, "a:k:m:")) != -1) {
			switch (ch) {
				case 'a':
					algorithm	= (unsigned char *)optarg;
					break;
				case 'm':
					msg		= (unsigned char *)optarg;
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

	if(argc || (!(algorithm && msg))) {
		usage();
		/* NOTREACHED */
	}

	return(ivcomp_main(algorithm, msg));
	/* NOTREACHED */
}

int
ivcomp_main(const unsigned char *const algorithm,
	const unsigned char *const msg)
{
	int ret			= 0;
	unsigned int msglen	= strlen((const char *)msg);
	unsigned int len	= (msglen + 1) / 2;
	unsigned char *data	= NULL;

	if(!(data = malloc(len))) {
		fprintf(stderr, "err: malloc: data: %s\n", strerror(errno));
		return(-1);
	}

	if(!hex_pton(msg, msglen, data, len)) {
		fprintf(stderr, "err: HMAC: msg: conversion failure -- %s\n",
			msg);
		return(-1);
	}

	for( ; ; ) {
		if(!strcmp((const char *)algorithm, "md5")) {
			ret = md5_main(data, len);
			break;
		}

		if(!strcmp((const char *)algorithm, "sha1")) {
			ret = sha1_main(data, len);
			break;
		}

		ret = -1;
		break;
	}

	free(data);

	return(ret);
}

int
md5_main(const unsigned char *const data, unsigned int len)
{
	unsigned char digest[MD5_DIGEST_LENGTH];

	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx, data, len);
	MD5_Final(digest, &ctx);

	ivcomp_dump((unsigned char *)"md5", data, len, digest, MD5_DIGEST_LENGTH);

	return(0);
}

int
sha1_main(const unsigned char *const data, unsigned int len)
{
	unsigned char digest[SHA_DIGEST_LENGTH];

	SHA_CTX ctx;
	SHA1_Init(&ctx);
	SHA1_Update(&ctx, data, len);
	SHA1_Final(digest, &ctx);

	ivcomp_dump((unsigned char *)"sha1", data, len, digest, SHA_DIGEST_LENGTH);

	return(0);
}

bool
hex_pton(const unsigned char *const src, const unsigned int srclen,
	unsigned char *const dst, const unsigned int dstlen)
{
	bool upper = true;
	unsigned int x = 0;
	unsigned int y = 0;
	const char xdigits_l[] = "0123456789abcdef";
	const char xdigits_u[] = "0123456789ABCDEF";
	const char *xdigits = 0;

	memset(dst, 0, dstlen);

	for( ; (x < srclen) && (y < dstlen); x ++) {
		const char *cptr = 0;

		if(((cptr = strchr((xdigits = xdigits_l), src[x])) == 0) &&
			((cptr = strchr((xdigits = xdigits_u), src[x])) == 0)) {
				return(false);
		}

		if(upper) {
			dst[y] = (cptr - xdigits) << 4;
			upper = false;
		} else {
			dst[y] |= (cptr - xdigits);
			upper = true;
			y ++;
		}
	}

	return(true);
}

void
ivcomp_dump(const unsigned char *const algorithm,
	const unsigned char *const d, const unsigned int n,
	const unsigned char *const md, const unsigned int md_len)
{
	int x = 0;

	printf("log:IVComp_Results                  (length:%d)\n",
		n + md_len);

	printf("log:| algorithm                        = %s\n", algorithm);

	printf("log:| Message                         (length:%d)\n", n);
	printf("log:| | data                             = ");
	for(x = 0; x < n; x ++) {
		printf("%02x", d[x]);
	}
	printf("\n");

	printf("log:| Digest                          (length:%d)\n", md_len);
	printf("log:| | data                             = ");
	for(x = 0; x < md_len; x ++) {
		printf("%02x", md[x]);
	}
	printf("\n");

	return;
}

void
usage(void)
{
	fprintf(stderr, "err: usage: %s -a algorithm -m msg\n", prog);
	fprintf(stderr, "err: \n");
	fprintf(stderr, "err:            algorithm: md5, sha1\n");
	fprintf(stderr, "err: \n");
	exit(-1);
	/* NOTREACHED */

	return;
}

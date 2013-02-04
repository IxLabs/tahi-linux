/*
 *
 * Copyright (c) 2006,2008
 * NTT (Nippon Telegraph and Telephone Corporation) . All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer as
 *   the first lines of this file unmodified.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NTT ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL NTT BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD: src/sys/crypto/camellia/camellia-api.c,v 1.1 2007/05/09 19:37:01 gnn Exp $
 */

#include <sys/cdefs.h>

#include <sys/types.h>
#ifdef _KERNEL
#include <sys/systm.h>
#endif
#include "crypto/camellia/camellia.h"

void
camellia_set_key(camellia_ctx *ctx, const u_char *key, int bits)
{

    Camellia_Ekeygen(bits, key, ctx->subkey);
    ctx->bits = bits;
}

void
camellia_cbc_encrypt(const u_char *src, u_char *dst, uint32_t srcbytes,
			camellia_ctx *ctx, u_char *iv, const int enc)
{
	/* iv must be 16bytes */
	/* enc: 1 is encrypt, 0 is decrypt */

	u_char block[16];
	u_char ivec[4][4];
	int i, numb;

	numb = srcbytes/16;

	if (enc) {
		((uint32_t *)block)[0] =
			 ((uint32_t *)iv)[0] ^ ((uint32_t *)src)[0];
		((uint32_t *)block)[1] =
			 ((uint32_t *)iv)[1] ^ ((uint32_t *)src)[1];
		((uint32_t *)block)[2] =
			 ((uint32_t *)iv)[2] ^ ((uint32_t *)src)[2];
		((uint32_t *)block)[3] =
			 ((uint32_t *)iv)[3] ^ ((uint32_t *)src)[3];

		camellia_encrypt(ctx, block, dst);
		src += 16;

		for(i = numb - 1; i > 0; i--) {
			((uint32_t *)block)[0] =
				 ((uint32_t *)dst)[0] ^ ((uint32_t *)src)[0];
			((uint32_t *)block)[1] =
				 ((uint32_t *)dst)[1] ^ ((uint32_t *)src)[1];
			((uint32_t *)block)[2] =
				 ((uint32_t *)dst)[2] ^ ((uint32_t *)src)[2];
			((uint32_t *)block)[3] =
				 ((uint32_t *)dst)[3] ^ ((uint32_t *)src)[3];
			dst += 16;
			camellia_encrypt(ctx, block, dst);
			src += 16;
		}
	} else {
		*((uint32_t *)ivec[0]) = *((uint32_t *)iv    );
		*((uint32_t *)ivec[1]) = *((uint32_t *)(iv + 4));
		*((uint32_t *)ivec[2]) = *((uint32_t *)(iv + 8));
		*((uint32_t *)ivec[3]) = *((uint32_t *)(iv +12));

		for(i = numb; i > 0; i--) {
			camellia_decrypt(ctx, src, block);
			((uint32_t *)block)[0] ^= *((uint32_t *)ivec[0]);
			((uint32_t *)block)[1] ^= *((uint32_t *)ivec[1]);
			((uint32_t *)block)[2] ^= *((uint32_t *)ivec[2]);
			((uint32_t *)block)[3] ^= *((uint32_t *)ivec[3]);

			*((uint32_t *)ivec[0]) = ((uint32_t *)src)[0];
			*((uint32_t *)ivec[1]) = ((uint32_t *)src)[1];
			*((uint32_t *)ivec[2]) = ((uint32_t *)src)[2];
			*((uint32_t *)ivec[3]) = ((uint32_t *)src)[3];

			((uint32_t *)dst)[0] = ((uint32_t *)block)[0];
			((uint32_t *)dst)[1] = ((uint32_t *)block)[1];
			((uint32_t *)dst)[2] = ((uint32_t *)block)[2];
			((uint32_t *)dst)[3] = ((uint32_t *)block)[3];
			src += 16;
			dst += 16;
		}
	}
}


void
camellia_decrypt(const camellia_ctx *ctx, const u_char *src, u_char *dst)
{

    Camellia_DecryptBlock(ctx->bits, src, ctx->subkey, dst);
}

void
camellia_encrypt(const camellia_ctx *ctx, const u_char *src, u_char *dst)
{

    Camellia_EncryptBlock(ctx->bits, src, ctx->subkey, dst);
}

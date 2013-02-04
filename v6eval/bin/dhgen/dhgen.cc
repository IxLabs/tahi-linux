/*
 * The Initial Developer of the Original Code is International Business
 * Machines Corporation.  Portions created by IBM Corporation are
 * Copyright (C) 2003 International Business Machines Corporation.
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
 * $TAHI: v6eval/bin/dhgen/dhgen.cc,v 1.3 2005/05/09 09:35:21 akisada Exp $
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <openssl/dh.h>
#include <openssl/bn.h>
#include <openssl/rand.h>
#include <openssl/err.h>


DH *pDH = 0;

char *pPrime = 0,
     *pGenerator = 0;

char *pPubKey = 0,
     *pPrivKey = 0;

#ifdef TAHI
bool xflag = false;
#endif	// TAHI

bool ParseCommand(int argc, char *argv[]) {

	int iOption;

#ifdef TAHI
	while ((iOption = getopt(argc, argv, "p:g:@")) != -1) {
#else	// TAHI
	while ((iOption = getopt(argc, argv, "p:g:")) != -1) {
#endif	// TAHI
		switch (iOption) {
			case 'p':
				// Make sure '-p' is specified only once
				if (pPrime)
					return false;

				pPrime = new char [strlen(optarg) + 1];
				if (!pPrime)
					return false;
				strcpy(pPrime, optarg);
				break;

			case 'g':
				// Make sure '-g' is specified only once
				if (pGenerator)
					return false;

				pGenerator = new char[strlen(optarg) + 1];
				if (!pGenerator)
					return false;
				strcpy(pGenerator, optarg);
				break;
#ifdef TAHI
			case '@':
				xflag = true;
				break;
#endif	// TAHI

			default:
				return false;
		}
	}

	if (!pPrime || !pGenerator)
		return false;

	return true;
}

void Usage() {

	fprintf(stderr, "err:Usage: dhgen -p prime -g generator\n");
	fprintf(stderr, "err:Generate a Diffie-Hellman public/private key pair.\n");
	fprintf(stderr, "err:\n");
	fprintf(stderr, "err:Options:\n");
	fprintf(stderr, "err:\t-p prime         Prime number to be used, specified in hex (F34D, etc.)\n");
	fprintf(stderr, "err:\t-g generator     Generator number to be used, specified in hex (F34D, etc.)\n");
	fprintf(stderr, "err:\n");
}

void PrintError(const char *function) {

	int error = ERR_get_error();

	fprintf(stderr, "err:dhgen: %s error: %08X (%i)\n", function, error, error);
	fprintf(stderr, "err:\tLib  = %08X (%i)\n", ERR_GET_LIB(error), ERR_GET_LIB(error));
	fprintf(stderr, "err:\tFunc = %08X (%i)\n", ERR_GET_FUNC(error), ERR_GET_FUNC(error));
	fprintf(stderr, "err:\tReason = %08X (%i)\n", ERR_GET_REASON(error), ERR_GET_REASON(error));
}

void PrintResult(char *data) {

	int len = strlen(data);

	printf("log:| | data                             =");
	for (int i = 0; i < len; i++) {
		if ((i % 64) == 0)
			printf("\nlog:| |   ");
		else if ((i % 16) == 0)
			printf("  ");
		else if ((i % 8) == 0)
			printf(" ");

		printf("%c", data[i]);
	}
	printf("\n");
}
#ifdef TAHI
void xPrintResult(char *data) {

	int len = strlen(data);

	printf("log:| | data                             = ");
	for (int i = 0; i < len; i++) {
		printf("%c", data[i]);
	}
	printf("\n");
}
#endif	// TAHI

void Cleanup() {

	if (pDH)
		DH_free(pDH);

	if (pPubKey)
		OPENSSL_free(pPubKey);

	if (pPrivKey)
		OPENSSL_free(pPrivKey);
}

int main(int argc, char *argv[]) {

	if (!ParseCommand(argc, argv)) {
		Usage();
		return 1;
	}

	pDH = DH_new();
	if (!pDH) {
		PrintError("DH_new");
		return 1;
	}

	pDH->p = BN_new();
	if (!pDH->p) {
		PrintError("BN_new (prime)");
		Cleanup();
		return 1;
	}
	if (!BN_hex2bn(&pDH->p, pPrime)) {
		PrintError("BN_hex2bn (prime)");
		Cleanup();
		return 1;
	}

	pDH->g = BN_new();
	if (!pDH->g) {
		PrintError("BN_new (generator)");
		Cleanup();
		return 1;
	}
	if (!BN_hex2bn(&pDH->g, pGenerator)) {
		PrintError("BN_hex2bn (generator)");
		Cleanup();
		return 1;
	}

	if (!DH_generate_key(pDH)) {
		PrintError("DH_generate_key");
		Cleanup();
		return 1;
	}

	pPubKey = BN_bn2hex(pDH->pub_key);
	if (!pPubKey) {
		PrintError("BN_bn2hex (public key)");
		Cleanup();
		return 1;
	}
	pPrivKey = BN_bn2hex(pDH->priv_key);
	if (!pPrivKey) {
		PrintError("BN_bn2hex (private key)");
		Cleanup();
		return 1;
	}

#ifdef TAHI
	printf("log:DHGen_Results                   (length:%i)\n",
		xflag?
			BN_num_bytes(pDH->pub_key) +
				BN_num_bytes(pDH->priv_key):
			strlen(pPubKey) + strlen(pPrivKey));
#else	// TAHI
	printf("log:DHGen_Results                   (length:%i)\n", strlen(pPubKey) + strlen(pPrivKey));
#endif	// TAHI
#ifdef TAHI
	printf("log:| Public_Key                      (length:%i)\n",
		xflag?
			BN_num_bytes(pDH->pub_key):
			strlen(pPubKey));
#else	// TAHI
	printf("log:| Public_Key                      (length:%i)\n", strlen(pPubKey));
#endif	// TAHI
#ifdef TAHI
	xflag? xPrintResult(pPubKey): PrintResult(pPubKey);
#else	// TAHI
	PrintResult(pPubKey);
#endif	// TAHI
#ifdef TAHI
	printf("log:| Private_Key                     (length:%i)\n",
		xflag?
			BN_num_bytes(pDH->priv_key):
			strlen(pPrivKey));
#else	// TAHI
	printf("log:| Private_Key                     (length:%i)\n", strlen(pPrivKey));
#endif	// TAHI
#ifdef TAHI
	xflag? xPrintResult(pPrivKey): PrintResult(pPrivKey);
#else	// TAHI
	PrintResult(pPrivKey);
#endif	// TAHI

	Cleanup();

	return 0;
}

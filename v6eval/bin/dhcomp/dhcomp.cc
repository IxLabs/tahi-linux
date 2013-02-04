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
 * $TAHI: v6eval/bin/dhcomp/dhcomp.cc,v 1.4 2006/03/09 07:54:53 akisada Exp $
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
     *pGenerator = 0,
     *pPubKey = 0,
     *pPrivKey = 0,
     *pRecvPubKey = 0;

BIGNUM *pBNRecvPubKey = 0;

int iSharedKeyLen = 0;
char *pSharedKey = 0;
char *pSharedKeyStock = 0;

#ifdef TAHI
bool xflag = false;
#endif	// TAHI

bool ParseCommand(int argc, char *argv[]) {

	int iOption;

#ifdef TAHI
	while ((iOption = getopt(argc, argv, "p:g:x:y:z:@")) != -1) {
#else	// TAHI
	while ((iOption = getopt(argc, argv, "p:g:x:y:z:")) != -1) {
#endif	// TAHI
		switch (iOption) {
			case 'p':
				// Make sure '-p' is specified only once
				if (pPrime)
					return false;

				pPrime = new char[strlen(optarg) + 1];
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

			case 'x':
				// Make sure '-x' is specified only once
				if (pPubKey)
					return false;

				pPubKey = new char[strlen(optarg) + 1];
				if (!pPubKey)
					return false;
				strcpy(pPubKey, optarg);
				break;

			case 'y':
				// Make sure '-y' is specified only once
				if (pPrivKey)
					return false;

				pPrivKey = new char[strlen(optarg) + 1];
				if (!pPrivKey)
					return false;
				strcpy(pPrivKey, optarg);
				break;

			case 'z':
				// Make sure '-z' is specified only once
				if (pRecvPubKey)
					return false;

				pRecvPubKey = new char[strlen(optarg) + 1];
				if (!pRecvPubKey)
					return false;
				strcpy(pRecvPubKey, optarg);
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

	if (!pPrime || !pGenerator || !pPubKey || !pPrivKey || !pRecvPubKey)
		return false;

	return true;
}

void Usage() {

	fprintf(stderr, "err:Usage: dhgen -p prime -g generator -x pubkey -y privkey -z pubkey\n");
	fprintf(stderr, "err:Compute a Diffie-Hellman shared key.\n");
	fprintf(stderr, "err:\n");
	fprintf(stderr, "err:Options:\n");
	fprintf(stderr, "err:\t-p prime         Prime number to be used, specified in hex (F34D, etc.)\n");
	fprintf(stderr, "err:\t-g generator     Generator number to be used, specified in hex (F34D, etc.)\n");
	fprintf(stderr, "err:\t-x pubkey        Previously generated public key, specified in hex (4D8C, etc.)\n");
	fprintf(stderr, "err:\t-y privkey       Previously generated private key, specified in hex (4D8C, etc.)\n");
	fprintf(stderr, "err:\t-z pubkey        Public key received from DH exchange partner, specified in hex (4D8C, etc.)\n");
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

	unsigned char *pData = (unsigned char *)data;

	printf("log:| | data                             =");
	for (int i = 0; i < iSharedKeyLen; i++) {
		if ((i % 32) == 0)
			printf("\nlog:| |   ");
		else if ((i % 8) == 0)
			printf("  ");
		else if ((i % 4) == 0)
			printf(" ");

		printf("%02X", pData[i]);
	}
	printf("\n");
}
#ifdef TAHI
void xPrintResult(char *data) {

	unsigned char *pData = (unsigned char *)data;

	printf("log:| | data                             = ");
	for (int i = 0; i < iSharedKeyLen; i++) {
		printf("%02X", pData[i]);
	}
	printf("\n");
}
#endif	// TAHI

void Cleanup() {

	if (pDH)
		DH_free(pDH);

	if (pBNRecvPubKey)
		BN_free(pBNRecvPubKey);

	delete pSharedKey;
}

int main(int argc, char *argv[]) {

	int l;

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

	pDH->pub_key = BN_new();
	if (!pDH->pub_key) {
		PrintError("BN_new (public key)");
		Cleanup();
		return 1;
	}
	if (!BN_hex2bn(&pDH->pub_key, pPubKey)) {
		PrintError("BN_hex2bn (public key)");
		Cleanup();
		return 1;
	}

	pDH->priv_key = BN_new();
	if (!pDH->priv_key) {
		PrintError("BN_new (private key)");
		Cleanup();
		return 1;
	}
	if (!BN_hex2bn(&pDH->priv_key, pPrivKey)) {
		PrintError("BN_hex2bn (private key)");
		Cleanup();
		return 1;
	}

	pBNRecvPubKey = BN_new();
	if (!pBNRecvPubKey) {
		PrintError("BN_new (received public key)");
		Cleanup();
		return 1;
	}
	if (!BN_hex2bn(&pBNRecvPubKey, pRecvPubKey)) {
		PrintError("BN_hex2bn (received public key)");
		Cleanup();
		return 1;
	}

	iSharedKeyLen = DH_size(pDH);
	pSharedKeyStock = new char[iSharedKeyLen + 1];
	pSharedKey = new char[iSharedKeyLen + 1];
	if (!pSharedKey) {
		PrintError("new char (shared key)");
		Cleanup();
		return 1;
	}

//	if (!DH_compute_key((unsigned char *)pSharedKey, pBNRecvPubKey, pDH)) {
//		PrintError("DH_compute_key");
//		Cleanup();
//		return 1;
//	}
	l = DH_compute_key((unsigned char *)pSharedKeyStock, pBNRecvPubKey, pDH);
	if (l <= 0) {
		PrintError("DH_compute_key");
		Cleanup();
		return 1;
	}
	memcpy(&pSharedKey[iSharedKeyLen - l], pSharedKeyStock, l);

#ifdef TAHI
	printf("log:DHComp_Results                  (length:%i)\n",
		xflag? iSharedKeyLen: strlen(pSharedKey));
	printf("log:| Shared_Key                      (length:%i)\n",
		xflag? iSharedKeyLen: strlen(pSharedKey));
	xflag? xPrintResult(pSharedKey): PrintResult(pSharedKey);
#else	// TAHI
	printf("log:DHComp_Results                  (length:%i)\n", strlen(pSharedKey));
	printf("log:| Shared_Key                      (length:%i)\n", strlen(pSharedKey));
	PrintResult(pSharedKey);
#endif	// TAHI

	Cleanup();

	delete pSharedKeyStock;

	return 0;
}

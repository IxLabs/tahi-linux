/*
 * Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011
 * Yokogawa Electric Corporation, 
 * INTAP (Interoperability Technology Association for Information Processing, Japan),
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
 * $TAHI: v6eval/lib/pkt/LxLexer.cc,v 1.34 2010/05/31 06:38:37 doo Exp $
 */
#define ApplicationLexer LxLexer 
//----------------------------------------------------------------------
// LEX MAIN ROUTINE
#include "LxLexer.h"
#include "LxToken.h"
#include "PzParse.h"
#include "PObject.h"
#include "PvObject.h"
#include "PvName.h"
#include "MmObject.h"
#include "McObject.h"
#include "McAlgorithm.h"
#include "MvFunction.h"
#include "PvAction.h"
#include "PvOctets.h"
#include "RObject.h"
#include "MfAlgorithm.h"
#include "CmMain.h"
#include <time.h>
extern PObject* yylval;
STR LxLexer::controls(STR s) {
	DBGFLAGS(0)=yydebug;
	s=CmLexer::controls(s);
	yydebug=DBGFLAGS(0);
	return s;}
LxLexer::LxLexer(CSTR s):CmLexer(s) {initialize();}
LxLexer::~LxLexer() {}
int32_t LxLexer::lex(PObject* compound) {
	yylval=0;
	STR s=nextToken();
	if(s==0) {return 0;}
	int c=*s;
	const MObject* t;
	/**/ if(isAlpha(c)) {
		CmCString tmp;
		CSTR str=nameLex(s,tmp);
		t=compound!=0?compound->findMember(str):0;/* MEMBER	*/
		if(t==0) {t=LxToken::find(str);}	/* KEYWORD	*/
		if(t!=0) {
			yylval=t->tokenObject(lineNo(),fileName());
			return t->token();}
		PvName* name=PvName::tokenObject(str,lineNo(),fileName());	/* NAME		*/
		yylval=name;
		return NAME;}
	else if(isdigit(c)) {				/* NUMBER	*/
		int32_t n=digitLex(s);
		yylval=PvNumber::unique(n);
		return NUMBER;}
	else if(c=='"'||c=='\''){			/* QUOTE	*/
		CmCString tmp;
		CSTR str=stringLex(s,tmp);
		yylval=new PvString(str);
		return STRING;}
	else {						/* DELIMITER	*/
		t=LxMatch::findToken(s);
		if(t!=0) {
			next(t->length());
			yylval=t->tokenObject(lineNo(),fileName());
			return t->token();}
		return YYERRCODE;}}

//----------------------------------------------------------------------
// DELIMITERS(NO TOKEN OBJECTS)
void LxLexer::addMatch(const MObject& t) {
	LxMatch::addToken(t);}

void LxLexer::delimiter(CSTR s,int32_t n) {
	MvKeyword* t=new MvKeyword(s,n);
	addMatch(*t);}

void LxLexer::operation(CSTR s,int32_t n,int32_t o) {
	MvOperator* t=new MvOperator(s,n,o);
	addMatch(*t);}

//----------------------------------------------------------------------
// DICTINARY MANUPULATION
inline void LxLexer::action(MObject* a) {
	LxToken::addToken(a);}

inline void LxLexer::function(MObject* f) {
	LxToken::addToken(f);}

//----------------------------------------------------------------------
// LEXER INITIALIZATION
void LxLexer::setMetaTokens() {
	MObject::tokenArray_[MObject::tkn_frame_]=FRAME;
	MObject::tokenArray_[MObject::tkn_packet_]=PACKET;
	MObject::tokenArray_[MObject::tkn_header_]=HEADER;
	MObject::tokenArray_[MObject::tkn_upper_]=UPPER;
	MObject::tokenArray_[MObject::tkn_isakmp_upper_]=ISAKMP_UPPER;
	MObject::tokenArray_[MObject::tkn_option_]=OPTION;
	MObject::tokenArray_[MObject::tkn_payload_]=PAYLOAD;
	MObject::tokenArray_[MObject::tkn_data_]=DATA;
	MObject::tokenArray_[MObject::tkn_member_]=MEMBER;
	MObject::tokenArray_[MObject::tkn_function_]=FUNCTION;
	MObject::tokenArray_[MObject::tkn_padfunc_]=PADFUNC;
	MObject::tokenArray_[MObject::tkn_cryptfunc_]=CRYPTFUNC;
	MObject::tokenArray_[MObject::tkn_ike_cryptfunc_]=IKE_CRYPTFUNC;
	MObject::tokenArray_[MObject::tkn_authfunc_]=AUTHFUNC;
	MObject::tokenArray_[MObject::tkn_packet_ref_]=PACKET_REF;
	MObject::tokenArray_[MObject::tkn_header_ref_]=HEADER_REF;
	MObject::tokenArray_[MObject::tkn_extent_ref_]=EXTENT_REF;
	MObject::tokenArray_[MObject::tkn_upper_ref_]=UPPER_REF;
	MObject::tokenArray_[MObject::tkn_option_ref_]=OPTION_REF;
	MObject::tokenArray_[MObject::tkn_isakmp_encryption_ref_]=ISAKMP_ENCRYPTION_REF;
	MObject::tokenArray_[MObject::tkn_payload_ref_]=PAYLOAD_REF;
	//
	MObject::tokenArray_[MObject::tkn_algorithm_]=ALGORITHM;
	MObject::tokenArray_[MObject::tkn_algomem_pad_]=ALGOMEM_PAD;
	MObject::tokenArray_[MObject::tkn_algomem_crypt_]=ALGOMEM_CRYPT;
	MObject::tokenArray_[MObject::tkn_algomem_auth_]=ALGOMEM_AUTH;
	MObject::tokenArray_[MObject::tkn_algofunc_]=ALGOFUNC;
	MObject::tokenArray_[MObject::tkn_algorithm_ref_]=ALGORITHM_REF;
	MObject::tokenArray_[MObject::tkn_bsafunc_]=BSAFUNC;
	MObject::tokenArray_[MObject::tkn_ike_p2_hash_func_]=IKE_P2_HASHFUNC;
	MObject::tokenArray_[MObject::tkn_dhcpauthfunc_]=DHCPAUTHFUNC;
	MObject::tokenArray_[MObject::tkn_dns_header_]=DNSHDR;
	MObject::tokenArray_[MObject::tkn_dns_qestion_ref_]=DNSQD_REF;
	MObject::tokenArray_[MObject::tkn_dns_answer_ref_]=DNSAN_REF;
	MObject::tokenArray_[MObject::tkn_dns_authority_ref_]=DNSNS_REF;
	MObject::tokenArray_[MObject::tkn_dns_additional_ref_]=DNSAR_REF;
	MObject::tokenArray_[MObject::tkn_dummy_]=DUMMY;
}

void LxLexer::initialize() {
	static bool hasBeenHere=false;
	if(hasBeenHere) return;
	hasBeenHere=true;
	CmLexer::initialize();
	setMetaTokens();
	operation("=",EQ,TObject::eOPEQ_);
	operation("!=",NE,TObject::eOPLT_|TObject::eOPGT_);
	operation("<",LT,TObject::eOPLT_);
	operation(">",GT,TObject::eOPGT_);
	operation("<=",LE,TObject::eOPLT_|TObject::eOPEQ_);
	operation(">=",GE,TObject::eOPGT_|TObject::eOPEQ_);
	//--------------------------------------------------------------
	delimiter("(",LP);
	delimiter(")",RP);
	delimiter("{",LC);
	delimiter("}",RC);
	delimiter("[",LB);
	delimiter("]",RB);
	delimiter(",",CM);
	delimiter(";",SM);
	delimiter("&",AND);
	delimiter("|",OR);
	delimiter("^",XOR);
	//--------------------------------------------------------------
	// INSIDE FUNCTION
//	function(new MvFunction("sizeof"));
	//--------------------------------------------------------------
	// OCTET GENERATION(length depend on argument)
	function(new MvRepeat("repeat"));
	function(new MvSubstr("substr"));
	function(new MvPatch("patch"));
	function(new MvLeft("left"));
	function(new MvRight("right"));
//	function("file",OCTETS,"s");
	function(new MvFile("file"));
	function(new MvSipTxt("siptxt"));
	function(new MvHexStr("hexstr"));
	function(new MvAscii("ascii"));
	function(new MvCookie64("keygentoken"));
	function(new MvKbu("kbm"));
	function(new MvDnsStr("dnsstr"));
	function(new MvDnsName("dnsname"));
#if !defined(NOT_USE_SNMP)
	function(new MvObjectID("objectid"));
#endif //NOT_USE_SNMP
	function(new MvInt2Hex("int2hex"));
	//--------------------------------------------------------------
	// binding security association
	function(new MfBSA("bsa"));
	function(new MfDHCPAuth("dhcpauth"));
	//--------------------------------------------------------------
	// ADDRESS GENERATION(length depend on function)
	// call functions to resolve presentation to network
	function(new MvV4("v4"));
	function(new MvEther("ether"));
	function(new MvEtherSRC("ethersrc"));
	function(new MvEtherDST("etherdst"));
	function(new MvEtherNUT("nutether"));
	function(new MvEtherTN("tnether"));
	function(new MvEtherMulti("v62ethermulti"));
	function(new MvV6("v6"));
	function(new MvV6SRC("v6src"));
	function(new MvV6DST("v6dst"));
	function(new MvV6NUT("nutv6"));
	function(new MvV6TN("tnv6"));
	function(new MvV6PNUT("nut2v6"));
	function(new MvV6PTN("tn2v6"));
	function(new MvV6V6("ipv62v6"));
	function(new MvV6Merge("v6merge"));
	function(new MvV6Ether("v6ether"));
	//--------------------------------------------------------------
	// OPTIONAL SELECTION using for matching
	function(new MvWithin("within"));
	function(new MvOneof("oneof"));
	function(new MvComb("comb"));
	//--------------------------------------------------------------
	// OPTIONAL CONTROL using for matching
	action(new MvAUTO("auto",AUTO));	// auto generation
	action(new MvANY("any",ANY));		// OPTIONAL CONTROL
	action(new MvZERO("zero",ZERO));	// 
	action(new MvFILL("fill",FILL));	// 
	action(new MvSTOP("stop",STOP));	// 

	//--------------------------------------------------------------
	// PAD ALGORITHM
{	PvOctets* oct=new PvOctets(256);
	oct->zero();
	MfESPPad* mfpad = new MfESPPad("allzero",oct);
	function(mfpad);
	MfESPPad::IKEdefaultPadding(mfpad);}
{	PvOctets* oct=new PvOctets(256);
	OCTSTR s=oct->buffer();
	for(uint32_t i=0;i<256;i++) {s[i]=i+1;}
	MfESPPad* mfpad=new MfESPPad("sequential",oct);
	function(mfpad);
	MfESPPad::defaultPadding(mfpad);}
{	PvOctets* oct=new PvOctets(256);
	OCTSTR s=oct->buffer();
	srand(time(0));
	for(uint32_t i=0;i<256;i++) {s[i]=rand();}
	function(new MfESPPadAny("random",oct));}

	//--------------------------------------------------------------
	// IKE functions
	function(new MvP1_HASH("p1_hash"));
	function(new MvP1_IV("p1_iv"));
	function(new MvP2_IV("p2_iv"));
	function(new MvP2_HASH_1("p2_hash_1"));
	function(new MvP2_HASH_2("p2_hash_2"));
	function(new MvP2_HASH_3("p2_hash_3"));

	//--------------------------------------------------------------
	// CRYPT ALGORITHM
	function(new MfCrypt("null_crypt",0,0,1));
	function(new MfDESCBC("descbc",8,8,8));
	function(new MfDESCBC_2("descbc_2", 8, 8, 8));		// binary args
	function(new MfBLOWFISH("blowfish",8,8,8));
#ifndef OPENSSL_NO_RC5
	function(new MfRC5("rc5",8,8,8));
#endif	// OPENSSL_NO_RC5
	function(new MfCAST128("cast128",8,8,8));
	function(new MfDES3CBC("des3cbc",8*3,8,8));
	function(new MfDES3CBC_2("des3cbc_2", 8 * 3, 8, 8));	// binary args
	function(new MfRIJNDAEL("rijndael_128",16,16,16));
	function(new MfRIJNDAEL_2("rijndael_128_2",16,16,16));	// binary args
	function(new MfRIJNDAEL("rijndael_192",24,16,16));
	function(new MfRIJNDAEL_2("rijndael_192_2",24,16,16));	// binary args
	function(new MfRIJNDAEL("rijndael_256",32,16,16));
	function(new MfRIJNDAEL_2("rijndael_256_2",32,16,16));	// binary args
	function(new MfCAMELLIA("camellia_128",16,16,16));
	function(new MfCAMELLIA("camellia_192",24,16,16));
	function(new MfCAMELLIA("camellia_256",32,16,16));
	// key sizes: 128 bits, nonce: 32 bits
	function(
		new MfAES_CTR(
			"aesctr_160",
			20,	// MfCrypt::keyLength_	// 128 bit + 32 bit
			8,	// MfCrypt::ivecLength_
			4	// MfCrypt::alignUnit_
		)
	);
	function(new MfAES_CTR_2("aesctr_160_2", 20, 8, 4));

	//--------------------------------------------------------------
	// AUTH ALGORITHM
	function(new MfAuth("null_auth",0,1));
	function(new MfHMACMD5("hmacmd5",12,4));
	function(new MfHMACMD5_2("hmacmd5_2", 12, 4));		// binary args
	function(new MfHMACSHA1("hmacsha1",12,4));
	function(new MfHMACSHA1_2("hmacsha1_2", 12, 4));	// binary args
#if defined(OPENSSL_VERSION_NUMBER) && (OPENSSL_VERSION_NUMBER >= 0x0090800fL)
	function(new MfHMACSHA2_256("hmacsha2_256", 12, 4));
	function(new MfHMACSHA2_384("hmacsha2_384", 12, 4));
	function(new MfHMACSHA2_512("hmacsha2_512", 12, 4));
#endif	// OPENSSL_VERSION_NUMBER
	function(new MfAES_XCBC("aesxcbc", 12, 4));
	function(new MfAES_XCBC_2("aesxcbc_2", 12, 4));		// binary args

	//--------------------------------------------------------------
	// IKE CRYPT ALGORITHM
	function(new MfIKE_DESCBC("ike_descbc", 8, 8, 8));
	function(new MfIKE_DES3CBC("ike_des3cbc", 8 * 3, 8, 8));
}

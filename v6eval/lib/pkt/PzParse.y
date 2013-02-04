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
 * $TAHI: v6eval/lib/pkt/PzParse.y,v 1.17 2005/05/09 09:35:25 akisada Exp $
 */
/* operator */
%term NAME
%term STRING
%term NUMBER
%term ADD
%term SUB
%term MUL
%term AND
%term OR
%term XOR
%term QUEST
%term COLON
%term NOT
%term INC
%term DEC

/* operators */
%term ASOP
%term RELOP
%term EQUOP
%term DIVOP
%term SHIFTL
%term SHIFTR
%term ANDAND
%term XORXOR
%term OROR

/* function */
%term FUNCTION
%term GENFUNC
%term PADFUNC
%term AUTHFUNC
%term CRYPTFUNC
%term IKE_CRYPTFUNC
%term BSAFUNC
%term IKE_P2_HASHFUNC
%term DHCPAUTHFUNC

/* compound words */
%term FRAME
%term PACKET
%term HEADER
%term UPPER
%term ISAKMP_UPPER
%term OPTION
%term PAYLOAD
%term DNSHDR

/* reference words */
%term PACKET_REF
%term HEADER_REF
%term EXTENT_REF
%term UPPER_REF
%term OPTION_REF
%term ISAKMP_ENCRYPTION_REF
%term PAYLOAD_REF
%term DNSQD_REF
%term DNSAN_REF
%term DNSNS_REF
%term DNSAR_REF

/* algorithm */
%term ALGORITHM
%term ALGOMEM_PAD
%term ALGOMEM_CRYPT
%term ALGOMEM_AUTH
%term ALGOFUNC
%term ALGORITHM_REF

/* member words */
%term MEMBER
%term DATA
%term PAD

/* value words */
%term OCTETS
%term ANY
%term AUTO
%term ZERO
%term FILL
%term STOP
%term BOOL
%term VOID
%term V4ADDR
%term V6ADDR
%term ETHER

%term DUMMY

/* delimiter */
%term LP
%term RP
%term LC
%term RC
%term LB
%term RB
%term CM
%term SM
%term EQ
%term NE
%term LT
%term GT
%term LE
%term GE
%term SH
%term SQ

/* Precedence and Order of Evaluation */
%left CM
%right ASOP EQ
%left OROR
%left XORXOR
%left ANDAND
%left OR
%left XOR
%left AND
%left EQUOP
%left RELOP
%left SHIFTL SHIFTR
%left ADD SUB
%left MUL DIVOP
%right NOT INC DEC
%right QUEST
%left LB LP

%{
#include "CmTypes.h"
#include "LxLexer.h"
#include "PObject.h"
#include "PcObject.h"
#include "PvObject.h"
#include "PvOctets.h"
#include "McObject.h"
#define YYSTYPE	PObject*

#if defined(__FreeBSD__) && __FreeBSD__ >= 4
#define YYPARSE_PARAM lexer
#define YYPARSE_PARAM_TYPE LxLexer&
#define yyparse(lex)parse(lex)
#else
#define	yyparse()parse(LxLexer& lexer)
#endif
#define	yylex()lexer.lex(compound_)
#define yyerror lexer.yaccError

static PObject* compound_=0;
static PoQueue funcQue_;
static PvFunction* function_=0;
static PvNumbers* numbers_=0;
%}
%%

file:		define.l.;
define.l.:	{$$=0;}
|		define.l;
define.l:	define
|		define.l define;
define:		frame.def
|		packet.def
|		header.def
|		dnshdr.def
|		option.def
|		upper.def
|		isakmp_upper.def
|		payload.def
|		algorithm.def;

/*----------------------------------------------------------------------*/
/* BASIC DEFINATIONS							*/
/*----------------------------------------------------------------------*/
frame.def:		frame.prefix frame.body RC	{compound_=0;};
frame.prefix:		FRAME NAME LC			{
				compound_=$1->name($2); PcObject::frame($2);};
frame.body:		header.ref packet.ref.;

packet.def:		packet.prefix packet.body RC	{compound_=0;};
packet.prefix:		PACKET NAME LC			{compound_=$1->name($2);};
packet.body:		header.ref extent.ref.l. upper.ref.;

header.def:		header.prefix header.body RC	{compound_=0;};
header.prefix:		HEADER NAME LC			{compound_=$1->name($2);};
header.body:		member.def.l. payload.ref.;

option.def:		option.prefix option.body RC	{compound_=0;};
option.prefix:		OPTION NAME LC			{compound_=$1->name($2);};
option.body:		member.def.l. payload.ref.;

upper.def:		upper.prefix upper.body RC	{compound_=0;};
upper.prefix:		UPPER NAME LC			{compound_=$1->name($2);};
upper.body:		header.ref. member.def.l. payload.ref.;

isakmp_upper.def:	isakmp_upper.prefix isakmp_upper.body RC	{compound_=0;};
isakmp_upper.prefix:	ISAKMP_UPPER NAME LC				{compound_=$1->name($2);};
isakmp_upper.body:	header.ref isakmp_encryption.ref. member.def.l. payload.ref.;

payload.def:		payload.prefix payload.body RC	{compound_=0;};
payload.prefix:		PAYLOAD NAME LC			{compound_=$1->name($2);};
payload.body:		data.def.l;

algorithm.def:		algorithm.prefix algorithm.body RC{compound_=0;};
algorithm.prefix:	ALGORITHM NAME LC		{compound_=$1->name($2);};
algorithm.body:		algomem.def.l.;

dnshdr.def	:	dnshdr.prefix dnshdr.body RC	{compound_ = 0;};
dnshdr.prefix	:	DNSHDR NAME LC			{compound_ = $1->name($2);};
dnshdr.body	:	member.def.l. dnsqd.ref. dnsan.ref. dnsns.ref. dnsar.ref.;

/*----------------------------------------------------------------------*/
/* LIST AND OPTIONALS							*/
/*----------------------------------------------------------------------*/
extent.ref.l.:						{$$=0;}
|			extent.ref.l;
extent.ref.l:		extent.ref
|			extent.ref.l extent.ref;
/* option.ref.l.:						{$$=0;} */
/* |			option.ref.l; */
/* option.ref.l:		option.ref */
/* |			option.ref.l option.ref; */
member.def.l.:						{$$=0;}
|			member.def.l;
member.def.l:		member.def
|			member.def.l member.def;
data.def.l:		data.def
|			data.def.l data.def;
algomem.def.l.:						{$$=0;}
|			algomem.def.l;
algomem.def.l:		algomem.def
|			algomem.def.l algomem.def;

/*----------------------------------------------------------------------*/
payload.ref.:						{$$=0;}
|			payload.ref;
upper.ref.:						{$$=0;}	
|			upper.ref;
header.ref.:						{$$=0;}	
|			header.ref;
packet.ref.:						{$$=0;}	
|			packet.ref;

isakmp_encryption.ref.	:				{$$ = 0;}	
			|	isakmp_encryption.ref
			;
dnsqd.ref.	:			{$$ = 0;}	
		|	dnsqd.ref
		;
dnsan.ref.	:			{$$ = 0;}	
		|	dnsan.ref
		;
dnsns.ref.	:			{$$ = 0;}	
		|	dnsns.ref
		;
dnsar.ref.	:			{$$ = 0;}	
		|	dnsar.ref
		;

/*----------------------------------------------------------------------*/
/* LEFT VALUES  MEMBER AND REFERENCE					*/
/*----------------------------------------------------------------------*/
packet.ref:		PACKET_REF eq.name SM		{
					compound_->member($1->rvalue($2));};
header.ref:		HEADER_REF eq.name SM		{
					compound_->member($1->rvalue($2));};
isakmp_encryption.ref:	ISAKMP_ENCRYPTION_REF eq.name SM		{
					compound_->member($1->rvalue($2));};
extent.ref:		EXTENT_REF eq.name SM		{
					compound_->member($1->rvalue($2));};
upper.ref:		UPPER_REF eq.upper.term SM		{
					compound_->member($1->rvalue($2));};
/* option.ref:		OPTION_REF eq.opt.term SM	{ */
/* 					compound_->member($1->rvalue($2));}; */
payload.ref:		PAYLOAD_REF eq.anyname SM		{
					compound_->member($1->rvalue($2));};
algorithm.ref:		ALGORITHM_REF eq.name SM	{
					compound_->member($1->rvalue($2));};
dnsqd.ref	:	DNSQD_REF eq.opt.term SM	{compound_->member($1->rvalue($2));};
dnsan.ref	:	DNSAN_REF eq.opt.term SM	{compound_->member($1->rvalue($2));};
dnsns.ref	:	DNSNS_REF eq.opt.term SM	{compound_->member($1->rvalue($2));};
dnsar.ref	:	DNSAR_REF eq.opt.term SM	{compound_->member($1->rvalue($2));};

/*----------------------------------------------------------------------*/
data.def:		DATA eq.data.term SM		{
					compound_->member($1->rvalue($2));};
/*----------------------------------------------------------------------*/
member.def	:	MEMBER op.mem.term SM		{ compound_->member($1->rvalue($2));}
		|	DATA eq.data.term SM		{ compound_->member($1->rvalue($2));}
		|	OPTION_REF eq.opt.term SM	{ compound_->member($1->rvalue($2));}
		|	ALGORITHM_REF eq.name SM	{ compound_->member($1->rvalue($2));}
		|	MEMBER EQ bsafunc SM		{ compound_->bsa_member($3);}
		|	MEMBER EQ ike_p2_hashfunc SM	{
				compound_->member($1->rvalue($3));
				compound_->ike_p2_hash_member($3); }
		|	MEMBER EQ dhcpauthfunc SM	{ compound_->dhcpauth_member($3);}
		;

/*----------------------------------------------------------------------*/
/* ALGORITHM COMPOUND AND REFERENCE					*/
/*----------------------------------------------------------------------*/
algomem.def:		ALGOMEM_PAD EQ padfunc SM	{
					compound_->pad_member($3);}
|			ALGOMEM_CRYPT EQ crpytfunc SM{
					compound_->crypt_member($3);}
|			ALGOMEM_CRYPT EQ ike_crpytfunc SM{
					compound_->crypt_member($3);}
|			ALGOMEM_AUTH EQ authfunc SM	{
					compound_->auth_member($3);};

/*----------------------------------------------------------------------*/
/* OPERATION AND TERMINALS						*/
/*----------------------------------------------------------------------*/
cmp.term:		NUMBER				{$$=$1;};
data.term:		NUMBER				{$$=$1;}
|			values				{$$=$1;}
|			function			{$$=$1;};
upper.term:		NAME				{$$=$1;}
|			action				{$$=$1;};
opt.term:		NAME				{$$=$1;}
|			action				{$$=$1;}
|			function			{$$=$1;};
mem.term:		NUMBER				{$$=$1;}
|			action				{$$=$1;}
|			function			{$$=$1;};
/*----------------------------------------------------------------------*/
eq.name:		EQ NAME				{$$=$2;};
eq.anyname:		EQ anyname			{$$=$2;};
eq.upper.term:		eq upper.term			{$$=$2;};
eq.opt.term:		eq opt.term			{$$=$2;};
eq.data.term:		eq data.term			{$$=$2;};
eq.mem.term:		eq mem.term			{$$=$1->rvalue($2);};
cmp.value:		cmp cmp.term			{$$=$1->rvalue($2);};
op.mem.term:		eq.mem.term			{$$=$1;}
|			cmp.value			{$$=$1;};
/*----------------------------------------------------------------------*/
values.prefix:		LC				{numbers_=new PvNumbers;};
values:			values.prefix number.l. RC 	{
				PvNumbers* ns=numbers_; numbers_=0;
				PvOctets* oct=ns->octetString();
				delete ns;
				$$=oct;};
number.l.:						{$$=0;}
|			number.l			{$$=$1;};
number.l:		NUMBER				{numbers_->add($1);}
|			number.l CM NUMBER		{numbers_->add($3);};
/*----------------------------------------------------------------------*/
anyname:		ANY				{$$=$1;}
|			NAME				{$$=$1;};
arg.term:		NUMBER				{$$=$1;}
|			STRING				{$$=$1;}
|			NAME				{$$=$1;}
|			function			{$$=$1;};
arg.l.:							{$$=0;}
|			arg.l				{$$=$1;}; 
arg.l:			arg.term			{function_->argument($1);}
|			arg.l CM arg.term		{function_->argument($3);};
func_call:		FUNCTION LP			{
				function_=(PvFunction*)funcQue_.push($1); $$=$1;};
function:		func_call arg.l. RP		{
				function_->checkArgument();
				$$=$1; function_=(PvFunction*)funcQue_.pop();};
bsa_call	:	BSAFUNC LP		{function_ = (PvFunction*)funcQue_.push($1); $$=$1;};
		;
bsafunc		:	bsa_call arg.l. RP	{function_->checkArgument(); $$=$1; function_ = (PvFunction*)funcQue_.pop();};
		;
ike_p2_hash_call	:	IKE_P2_HASHFUNC LP		{function_ = (PvFunction*)funcQue_.push($1); $$=$1;};
			;
ike_p2_hashfunc		:	ike_p2_hash_call arg.l. RP	{function_->checkArgument(); $$=$1; function_ = (PvFunction*)funcQue_.pop();};
			;
dhcpauth_call	:	DHCPAUTHFUNC LP			{function_ = (PvFunction*)funcQue_.push($1); $$=$1;};
		;
dhcpauthfunc	:	dhcpauth_call arg.l. RP	{function_->checkArgument(); $$=$1; function_ = (PvFunction*)funcQue_.pop();};
		;
/* sarg.term:		NUMBER				{$$=$1;} */
/* |			STRING				{ */
/* 				PObject* p=$1; */
/* 				PvOctets* pv=p->octetString(); */
/* 				if(p!=pv) {delete p;} */
/* 				$$=pv;} */
/* |			values				{$$=$1;}; */
/* sarg.l.:						{$$=0;} */
/* |			sarg.l				{$$=$1;}; */
/* sarg.l:			sarg.term			{function_->argument($1);} */
/* |			sarg.l CM sarg.term		{function_->argument($3);}; */
padfunc:		PADFUNC LP RP			{$$=$1;};
crpyt_call:		CRYPTFUNC LP			{
				function_=(PvFunction*)funcQue_.push($1); $$=$1;};
crpytfunc:		crpyt_call arg.l. RP		{
				function_->checkArgument();
				$$=$1; function_=(PvFunction*)funcQue_.pop();};
ike_crpyt_call:		IKE_CRYPTFUNC LP			{
				function_=(PvFunction*)funcQue_.push($1); $$=$1;};
ike_crpytfunc:		ike_crpyt_call arg.l. RP		{
				function_->checkArgument();
				$$=$1; function_=(PvFunction*)funcQue_.pop();};
auth_call:		AUTHFUNC LP			{
				function_=(PvFunction*)funcQue_.push($1); $$=$1;};
authfunc:		auth_call arg.l. RP		{
				function_->checkArgument();
				$$=$1; function_=(PvFunction*)funcQue_.pop();};
/*----------------------------------------------------------------------*/
action:			ANY				{$$=$1;}
|			AUTO				{$$=$1;}
|			ZERO				{$$=$1;}
|			FILL				{$$=$1;}
|			STOP				{$$=$1;};

eq:			EQ				{$$=$1;};
cmp:			NE				{$$=$1;}
|			LT				{$$=$1;}
|			GT				{$$=$1;}
|			LE				{$$=$1;}
|			GE				{$$=$1;};
%%

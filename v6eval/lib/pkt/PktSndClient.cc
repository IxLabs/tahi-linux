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
 * $TAHI: v6eval/lib/pkt/PktSndClient.cc,v 1.18 2003/10/23 04:37:31 akisada Exp $
 */
#include "PktSndClient.h"
#include "timeval.h"
#include "PvOctets.h"
#include "CmMain.h"
#include "McObject.h"
#include "RObject.h"
#include "PControl.h"
#include "PAlgorithm.h"
#include <stdio.h>
PktSndClient::PktSndClient(const McObject* m,SAOctetsList& l):PktClient(),
	reverser_(m),current_(0),list_(l) {}
PktSndClient::~PktSndClient() {}

int PktSndClient::writeIndication(STR s,uint32_t l) {
	tcpHead h(eWrite_);
	return sendPacket(h,l,s);}

int PktSndClient::sendConnect(CSTR) {
	return PktClient::sendConnect("pktsend");}

int PktSndClient::sendGeneratePacket() {
	uint32_t i9=list_.size();
	if(i9<=current_) {
		terminate(); exit(0);}
	SAOctets* saoct=list_[current_];
	PvOctets* o=saoct->frame();
	return writeIndication((STR)o->buffer(),o->length());}

int PktSndClient::writeConfirmation() {
	int rc=result();
	if(rc!=0) {
		printf("err:pktbuf result=%d\n",rc);
		terminate(); exit(1);}
	if(logLevel>0) {
		SAOctets* saoct=list_[current_];
		PvOctets* o=saoct->frame();
		ItPosition rit;
		RSAControl rc(saoct->algorithms());
		RObject* ro=reverser_->reverse(rc,0,rit,*o);
		ro->log(0);
		if(DBGFLAGS('s')) {printf("desc:"); ro->describe(0); printf("\n");}
		delete ro;}
	timeval& tv=*(timeval*)buffer();
	printf("std: %ld.%06ld\n",(u_long)ntohl(tv.tv_sec),(u_long)ntohl(tv.tv_usec));
	current_++;
	return sendGeneratePacket();}

int PktSndClient::connectConfirmation() {
	return sendGeneratePacket();}

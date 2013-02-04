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
 * $TAHI: v6eval/lib/pkt/PktRcvClient.cc,v 1.23 2009/11/25 07:26:56 akisada Exp $
 */
#include "PktRcvClient.h"
#include "CmMain.h"
#include "PcObject.h"
#include "McObject.h"
#include "PControl.h"
#include "PvOctets.h"
#include "ItPosition.h"
#include "timeval.h"
#include "RunEnv.h"
#include "PAlgorithm.h"
#include "RAFControl.h"
#include <stdio.h>
#include <net/bpf.h>

PktRcvClient::PktRcvClient(const McObject* m,AlgorithmFrames& l):PktClient(),
	reverser_(m),candidates_(l),evalc(1) {}
PktRcvClient::~PktRcvClient() {}

int PktRcvClient::readIndication() {
	tcpHead h(eRead_);
	timeval tv;
	timeval *timeout=RunEnv::tvtimeout();
	uint32_t size=0;	
	if(timeout!=0){
		tv.tv_sec=htonl(timeout->tv_sec);
		tv.tv_usec=htonl(timeout->tv_usec);
		size=sizeof(tv);}
	return sendPacket(h,size,(char*)&tv);}

int PktRcvClient::sendConnect(CSTR) {
	return PktClient::sendConnect("pktrecv");}

bool PktRcvClient::evaluate(const bpf_hdr& h,STR s) {
	time_t sec=h.bh_tstamp.tv_sec;
	uint32_t usec=h.bh_tstamp.tv_usec;
	uint32_t hlen=h.bh_hdrlen;
	uint32_t caplen=h.bh_caplen;
	PvOctets rcv(caplen,(OCTSTR)s+hlen);
	RAFControl rc(candidates_);
	bool okng=false;
	CSTR result=rc.reverseEvaluate(*reverser_,rcv,okng);

	if(DoHexDump) {
		uint32_t d	= 0;
		COCTSTR buffer	= rcv.string();
		uint32_t length	= rcv.length();

		printf("hex:");

		for(d = 0; d < length; d ++) {
			printf("%02x", buffer[d]);
		}

		printf("\n");
	}

	printf("std: %ld.%06d %s\n",(long)sec,usec,result);
	return okng;}

int PktRcvClient::readConfirmation() {
	int rc=result();
	STR s=buffer();
	uint32_t l=bodyLength();
	if(rc==2) {
		printf("log:pktbuf timeout\n");
		terminate(); exit(1);}
	if(rc!=0||s==0||l==0) {
		printf("err:pktbuf result=%d length=%d\n",rc,l);
		terminate(); exit(3);}
	bpf_hdr* hdr=(bpf_hdr*)s;
	bpf_hdr host;
	host.bh_tstamp.tv_sec=ntohl(hdr->bh_tstamp.tv_sec);
	host.bh_tstamp.tv_usec=ntohl(hdr->bh_tstamp.tv_usec);
	host.bh_hdrlen=ntohs(hdr->bh_hdrlen);
	host.bh_caplen=ntohl(hdr->bh_caplen);
	host.bh_datalen=ntohl(hdr->bh_datalen);
	if(host.bh_tstamp<RunEnv::seek()) {
		printf("log: skip evaluate\n"); }
	else {
		if(evaluate(host,s)) {terminate(); exit(0);}
		if(RunEnv::count()!=0 && ++evalc>RunEnv::count()) {
			printf("log:pktbuf countout\n");
			terminate(); exit(2);}}
	return readIndication();}

int PktRcvClient::connectConfirmation() {
	return readIndication();}


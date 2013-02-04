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
 * $TAHI: v6eval/lib/pkt/PktCtlClient.cc,v 1.16 2008/04/02 06:39:45 akisada Exp $
 */
#include "PktCtlClient.h"
#include "timeval.h"
#include "RunEnv.h"
#include "bufStat.h"
#include "PvOctets.h"
#include "ItPosition.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <net/bpf.h>

PktCtlClient::PktCtlClient(StringList pos):PktClient(),
	command_(pos),parsed_(0) {}
PktCtlClient::~PktCtlClient() {}

int PktCtlClient::readIndication() {
	tcpHead h(eRead_);
	timeval tv;
	timeval *timeout=RunEnv::tvtimeout();
	uint32_t size=0;	
	if(timeout!=0){
		tv.tv_sec=htonl(timeout->tv_sec);
		tv.tv_usec=htonl(timeout->tv_usec);
		size=sizeof(tv);}
	if(sendPacket(h,size,(char*)&tv)==sizeof(tv))
		return 0;
	else
		return 1;}

int PktCtlClient::writeIndication(STR s,uint32_t l) {
	tcpHead h(eWrite_);
	sendPacket(h,l,s); /* todo: error check */
	return 0;}

int PktCtlClient::dumpIndication(uint32_t l) {
	uint32_t dp=htonl(l);
	tcpHead h(eDump_);
	sendPacket(h,sizeof(dp),(char *)&dp);  /* todo: error check */
	return 0;}

int PktCtlClient::Indication(uint32_t req){
	tcpHead con(req);
	if(sendPacket(con,0,0)==sizeof(con))
		return 0;
	else
		return 1;}

int PktCtlClient::sendConnect(CSTR) {
	return PktClient::sendConnect("pktctl");}

int PktCtlClient::writeConfirmation() {
	int rc=result();
	if(rc!=0) {
		printf("err:pktbuf result=%d\n",rc);
		terminate(); exit(1);}
	timeval& tv=*(timeval*)buffer();
	printf("std: %ld.%06ld\n",(u_long)ntohl(tv.tv_sec),(u_long)ntohl(tv.tv_usec));
	return command();}

int PktCtlClient::statusConfirmation() {
	int rc=result();
	if(rc!=0) {
		printf("err:pktbuf result=%d\n",rc);
		terminate(); exit(1);}
	bufStat& stat=*(bufStat*)buffer();
	stat.print();
	return 	command();}

int PktCtlClient::Confirmation(STR msg) {
	int rc=result();
	if(rc!=0) {
		printf("err:pktbuf %s result=%d\n",msg,rc);
		terminate(); exit(1);}
	return command();}

int PktCtlClient::captureConfirmation() {
	return Confirmation((STR)"capture start");}

int PktCtlClient::clearConfirmation() {
	return Confirmation((STR)"clear buffer");}

int PktCtlClient::stopConfirmation() {
	return Confirmation((STR)"capture stop");}

int PktCtlClient::wrapConfirmation() {
	return Confirmation((STR)"wrap mode");}

int PktCtlClient::nowrapConfirmation() {
	return Confirmation((STR)"nowrap mode");}

int PktCtlClient::connectConfirmation() {
	return Confirmation((STR)"Connected");}

int PktCtlClient::readConfirmation() {
	int rc=result();
	STR s=buffer();
	uint32_t l=bodyLength();
	if(rc==2) {
		printf("log:pktbuf timeout\n");
		exit(1);}
	if(rc!=0||s==0||l==0) {
		printf("err:pktbuf result=%d length=%d\n",rc,l);
		terminate(); exit(3);}
	bpf_hdr* hdr=(bpf_hdr*)s;
	hdr->bh_tstamp.tv_sec=ntohl(hdr->bh_tstamp.tv_sec);
	hdr->bh_tstamp.tv_usec=ntohl(hdr->bh_tstamp.tv_usec);
	hdr->bh_hdrlen=ntohs(hdr->bh_hdrlen);
	hdr->bh_caplen=ntohl(hdr->bh_caplen);
	hdr->bh_datalen=ntohl(hdr->bh_datalen);
	timeval *tv=&(hdr->bh_tstamp);

	if(*tv<RunEnv::seek())
		printf("-- skip --\n");
	else
		printf("-- eval --\n");
	printf("std: %ld.%06ld\n",(long int)tv->tv_sec,tv->tv_usec);

	uint32_t hlen=hdr->bh_hdrlen;
	uint32_t caplen=hdr->bh_caplen;
	PvOctets rcv(caplen,(OCTSTR)s+hlen);
	rcv.print();

	return command();}

int PktCtlClient::dumpConfirmation() {
	int rc=result();
	STR s=buffer();
	uint32_t l=bodyLength();
	if(rc==0&&l==0) {
		printf("std:buffer empty\n");
		terminate(); exit(0);}
	if(s==0||l==0) {
		printf("err:pktctl result=%d length=%d\n",rc,l);
		terminate(); exit(3);}
	bpf_hdr* hdr=(bpf_hdr*)s;
	hdr->bh_tstamp.tv_sec=ntohl(hdr->bh_tstamp.tv_sec);
	hdr->bh_tstamp.tv_usec=ntohl(hdr->bh_tstamp.tv_usec);
	timeval *tv=&(hdr->bh_tstamp);
	printf("std: %ld.%06ld\n",(long int)tv->tv_sec,tv->tv_usec);
	uint32_t hlen=ntohs(hdr->bh_hdrlen);
	uint32_t caplen=ntohl(hdr->bh_caplen);
	PvOctets rcv(caplen,(OCTSTR)s+hlen);
	rcv.print();
	printf("\n");
	if(rc)	return dumpIndication(rc);
	else	return command();}

int PktCtlClient::command(int) {
	int rc=0;
	if(parsed_ >= command_.size())
		exit (0); // all command has paesed.

	CmString & cmdstr=*command_[parsed_++];
	if(cmdstr=="send") {
		if(parsed_ >= command_.size()){
			fprintf(stderr,"err: less argument\n");
			exit (1);}
		CSTR filename=(*command_[parsed_++]).string();
		const uint32_t MAX_PKTSIZE=1500;
		char buf[MAX_PKTSIZE];
		int fd;
		ssize_t size;

		fd = open(filename,O_RDONLY,0);
		if(fd == -1) {
		  	printf("err:can't open send data file\n");
			rc=1;}
		else {	size = read(fd,buf,MAX_PKTSIZE);
			if (size <= 0)	rc=1;
			else		rc=writeIndication(buf,size);}}
	else if(cmdstr=="recv") {
	  	rc=readIndication();}
	else if(cmdstr=="dump") {
	  	rc=dumpIndication(0);}
	else if(cmdstr=="capture") {
	  	rc=Indication(eCapture_);}
	else if(cmdstr=="clear") {
	  	rc=Indication(eClear_);}
	else if(cmdstr=="dump") {
	  	rc=Indication(eDump_);}
	else if(cmdstr=="stop") {
	  	rc=Indication(eStop_);}
	else if(cmdstr=="wrap") {
	  	rc=Indication(eWrap_);}
	else if(cmdstr=="nowrap") {
	  	rc=Indication(eNowrap_);}
	else if(cmdstr=="status") {
	  	rc=Indication(eStatus_);}
	else {	printf("err:unknown command \"%s\"\n",cmdstr.string());
		usage();
		rc=1;}
	if(rc){	exit (rc);}
	return 0;}

void PktCtlClient::usage() {
	printf("usage:\n");
	printf("  status:  report current status of pakcet buffer.\n");
	printf("  capture: flush and start receive packet from bpf.\n");
	printf("  clear:   set pointer at the top of buffer.\n");
	printf("  wrap:    use buffer as wrap buffer.\n");
	printf("  nowrap:  stop receive packet from bpf pointer comes end.\n");
	printf("  stop:    stop receiving packet from bpf.\n");
	printf("  dump:    dump packet buffer.\n");
	printf("  send:    send packet.\n");
	printf("  recv:    recv packet.\n");}

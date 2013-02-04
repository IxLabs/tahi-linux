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
 * $TAHI: v6eval/lib/pkt/PktServer.cc,v 1.20 2005/07/21 01:53:22 akisada Exp $
 */
#include <stdio.h>
#include <time.h>
#include "PktServer.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <net/bpf.h>
#include <unistd.h>
#include "CmDispatch.h"
#include "CmMain.h"
#include "timeval.h"
#include "debug.h"

PktAccept::PktAccept(PktBuf *p):PktAgent(),accepts_(new PktAgentSet(defaultPktClient)), bpf_(p){}
PktAccept::~PktAccept() {}

void PktAccept::addAgent(PktAgent* a) {
	accepts_->add(a);}

void PktAccept::removeAgent(PktAgent* a) {
	accepts_->remove(a);}

//======================================================================
// STARTING SERVICE
int PktAccept::listen(){
	CmSocket* s=socket();
	int rc=s->listen();
	if(rc<0) return rc;
	s->setReuseAddrOpt();
	if(DBGFLAGS('c')) {printf("PktAccept::listen"); s->print();}
	readAction((agentFunc)&PktAccept::accept);
	return rc;}

//======================================================================
// ESTABLISH THE CONNECTION
int PktAccept::accept() {
	CmSocket* sock=socket()->accept();
	if(sock!=0) {
		PktServer* agent=new PktServer(sock,this,bpf_);
		addAgent(agent);
		agent->receiver();}
	return 0;}

int PktAccept::terminate() {
	int rc=PktAgent::terminate();
	return rc;}

//======================================================================
PktServer::PktServer(CmSocket* s,PktAccept* a,PktBuf *bpf):PktReceiver(s),accepter_(a),bpfbuf_(bpf),pktbuf_(bpf) {
        timerAction((timerFunc)&PktServer::timeout);}

PktServer::~PktServer() {
	if(pktbuf_->readwait_==this)
		pktbuf_->readwait(0);
	dispatch()->stopTimer(this);
	accepter_->removeAgent(this);}

int PktServer::terminate() {
	accepter_->removeAgent(this);
	int rc=PktReceiver::terminate();
	delete this;
	return rc;}
	
//======================================================================
// Connect Request
int PktServer::connectRequest() {
	return response(0,0,NULL);}
//======================================================================
// Write Request
int PktServer::writeRequest() {
	timeval sendtime;
#ifdef VCLEAR_DBG
xdbg("/tmp/vclear_dbg.txt", "PktServer", "this: %p\n", this);
#endif  // VCLEAR_DBG
	bpfbuf_->write(buffer(0),header().length());
	gettimeofday(&sendtime,NULL);
	sendtime.tv_sec=htonl(sendtime.tv_sec);
	sendtime.tv_usec=htonl(sendtime.tv_usec);
	return (response(0,sizeof(sendtime),(STR)&sendtime));}
//======================================================================
// Read Request
int PktServer::readRequest() {
	// get timeout time */
	timeval tv;
	if(header().length()!=0){
		timeval &tvreq = *(timeval*)buffer(0);
		tv.tv_sec = ntohl(tvreq.tv_sec);
		tv.tv_usec = ntohl(tvreq.tv_usec);}
	// get recieve time of oldest data.
	timeval *tvdata =bpfbuf_->ReceiveTimeOfOldestData();
	if(tvdata==0){  // Buffer is Empty.
		pktbuf_->readwait(this);
		if(header().length()!=0){
			// set timeout time
			dispatch()->startTimerByAbsoluteTime(tv,this);}
		return 0;}

	if(header().length()!=0 && *tvdata > tv){
		// timeout
		return(response(2,0,0));}

	char tmp[bpfbuf_->maxdatasize()];
	int len=bpfbuf_->read(tmp,sizeof(tmp));
	if(len==0) { abort(); }
	return(readResponse(tmp,len));}

int PktServer::readResponse(char *p, int32_t size) {
	struct bpf_hdr* hdr=(bpf_hdr*)p;
	const struct bpf_hdr hdrorg=*hdr;
	hdr->bh_tstamp.tv_sec=htonl(hdrorg.bh_tstamp.tv_sec);
	hdr->bh_tstamp.tv_usec=htonl(hdrorg.bh_tstamp.tv_usec);
	hdr->bh_caplen=htonl(hdrorg.bh_caplen);
	hdr->bh_datalen=htonl(hdrorg.bh_datalen);
	hdr->bh_hdrlen=htons(hdrorg.bh_hdrlen);
	dispatch()->stopTimer(this);
	return(response(0,size,p));}

int PktServer::timeout(time_t ,uint32_t) {
	pktbuf_->readwait(0);
	return(response(2,0,0));}

//======================================================================
// Status Request
int PktServer::statusRequest() {
	struct bufStat stat = bpfbuf_->stat();
	return(response(0,sizeof(stat),(char *)(&stat)));}
// Wrap Request
int PktServer::wrapRequest() {
	bpfbuf_->setWrapMode();
	return(response(0,0,0));}

// noWrap Request
int PktServer::nowrapRequest() {
	bpfbuf_->setNowrapMode();
	return(response(0,0,0));}

// clear Request
int PktServer::clearRequest() {
	bpfbuf_->clear();
	return(response(0,0,0));}

// capture Request
int PktServer::captureRequest() {
	bpfbuf_->capture();
	return(response(0,0,0));}

// stop Request
int PktServer::stopRequest() {
	bpfbuf_->stop();
	return(response(0,0,0));}

// dump Request
int PktServer::dumpRequest() {
	uint32_t dp=ntohl(*(uint32_t *)buffer(0));
	uint32_t datanum = (bpfbuf_->stat()).datanum();
	char tmp[bpfbuf_->maxdatasize()];
	int size=bpfbuf_->readn(tmp,dp);
	if(size!=0){
		// convert network byte order
		struct bpf_hdr* hdr=(bpf_hdr*)tmp;
		const struct bpf_hdr hdrorg=*hdr;
		hdr->bh_tstamp.tv_sec=htonl(hdrorg.bh_tstamp.tv_sec);
		hdr->bh_tstamp.tv_usec=htonl(hdrorg.bh_tstamp.tv_usec);
		hdr->bh_caplen=htonl(hdrorg.bh_caplen);
		hdr->bh_datalen=htonl(hdrorg.bh_datalen);
		hdr->bh_hdrlen=htons(hdrorg.bh_hdrlen);
	}
	if(size==0||++dp >= datanum) dp=0; // buffer has more data
	return(response(dp,size,tmp));}

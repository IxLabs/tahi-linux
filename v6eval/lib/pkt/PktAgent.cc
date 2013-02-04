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
 * $TAHI: v6eval/lib/pkt/PktAgent.cc,v 1.15 2005/07/21 01:53:22 akisada Exp $
 */
#include "PktAgent.h"
#include "RunEnv.h"
#include "CmMain.h"
#include <sys/errno.h>
#include <sys/param.h>
#include "debug.h"
//======================================================================
CSTR PktAgent::unixDomainName() {
	return RunEnv::interfaceUnixPath();}
CmSocket* PktAgent::unixDomain() {
	CSTR serv=unixDomainName();
	return UnixSocket::stream(serv);}

tcpHead::tcpHead(int16_t r):request_(htons(r)),length_(0),result_(0) {}

PktAgent::PktAgent(CmSocket* s):CmAgent(),socket_(s) {
	if(socket_==0) {socket_=unixDomain();}}

PktAgent::~PktAgent() {
	terminate();
	delete socket_, socket_=0;}

int PktAgent::terminate() {
	CmSocket* s=socket();
	if(s!=0) {
		unlink();
		s->close();}
	return -1;}

void PktAgent::vsend(va_list ap) {
	DCLA(CSTR,s); DCLA(int,l);
	send(s,l);}

//======================================================================
// SEND/RECEIVE
int PktAgent::send(CSTR s,int32_t len) {
#ifdef VCLEAR_DBG
xdbg("/tmp/vclear_dbg.txt", "PktAgent", "this: %p\n", this);
#endif  // VCLEAR_DBG
	int32_t rem, l; CSTR p;
	for(rem=len,p=s;rem>0;rem-=l,p+=l) {
		l=socket()->send(p,rem);
		if(l<0&&errno==EINTR) {l=0; continue;}
		if(l<=0) return -1;}
	return len;}

int PktAgent::sendPacket(tcpHead& h,int32_t l,CSTR s) {
#ifdef VCLEAR_DBG
xdbg("/tmp/vclear_dbg.txt", "PktAgent", "this: %p\n", this);
#endif  // VCLEAR_DBG
	h.length(l);
	if(DBGFLAGS('o')) {
		uint16_t r=h.request()&0xffff;
		printf("PktAgent::sendPacket  request=%4.4x len=%d\n",r,l);}
	int rc=send((CSTR)&h,sizeof(h));
	if(rc>=0 && l>0) {rc=send(s,l);}
	return rc;}

implementCmSet(PktAgentSet,PktAgent);

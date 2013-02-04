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
 * $TAHI: v6eval/lib/pkt/BpfAgent.cc,v 1.29 2005/07/21 02:42:52 akisada Exp $
 */
#include "BpfAgent.h"
#include "RunEnv.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <net/bpf.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"

BpfAgent::BpfAgent(CSTR n,int32_t rbsize):filter_(n),buffer_(0),runStat_(eStop_) {
	if(fileDesc()<0) return;
	const Bpfilter& f=filter();
	if(f.promiscuous()<0||f.immediate(1)<0) {
		return;}

	if(RunEnv::filter()){ /* xxx: check TN def entry */
		struct bpf_program v6filter;
		struct bpf_insn insns[] = {
			BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 12),
			BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x86dd, 0, 1),
			BPF_STMT(BPF_RET+BPF_K, (u_int)-1),  // accept
			BPF_STMT(BPF_RET+BPF_K, 0),          // ignore
		};
		v6filter.bf_len = 4;
		v6filter.bf_insns = insns;
		if(f.setfilter(&v6filter)<0) return;}
	uint32_t l=f.bufferSize();
	buffer_=(caddr_t)malloc(l+1);
	rbuf = new Ringbuf(2048,rbsize); /* xxx Ethernet Depend */
	readAction((agentFunc)&BpfAgent::receive);}

int BpfAgent::receive(int) {
	int rc=filter().receive(buffer_);
#ifdef VCLEAR_DBG
xdbg("/tmp/vclear_dbg.txt", "BpfAgent", "rc: %d\n", rc);
#endif	// VCLEAR_DBG
	for(int i=0;i < rc; ){
#ifdef VCLEAR_DBG
xdbg("/tmp/vclear_dbg.txt", "BpfAgent", "i: %d\n", i);
#endif	// VCLEAR_DBG
		char *next = buffer_+i;
		struct bpf_hdr *hdr = (struct bpf_hdr *)next;
		int datasize = hdr->bh_caplen + hdr->bh_hdrlen;
		if(!fifo.IsEmpty() &&
		   fifo.firstdatalen()==(int)hdr->bh_caplen &&
		   memcmp(fifo.firstdata(),next+hdr->bh_hdrlen,hdr->bh_caplen)==0){
			fifo.del();}
		else if(runStat() == eRun_)
			rbuf->write(next,datasize);
		if(datasize > rc)
			abort();
		i += BPF_WORDALIGN(datasize);}
	if (!rbuf->isEmpty()){
		struct bpf_hdr *hdr = (struct bpf_hdr *)buffer_;
		int datasize = hdr->bh_caplen + hdr->bh_hdrlen;
		readydata(buffer_,datasize);}
	return 0;}

int
BpfAgent::nonblock_receive(int)
{
#ifdef VCLEAR_DBG
xdbg("/tmp/vclear_dbg.txt", "BpfAgent", "this: %p\n", this);
#endif	// VCLEAR_DBG
	int rc = filter().nonblock_receive(buffer_);
#ifdef VCLEAR_DBG
xdbg("/tmp/vclear_dbg.txt", "BpfAgent", "rc: %d\n", rc);
#endif	// VCLEAR_DBG

	//
	// XXX
	//
	// This is just a quick hack
	// for a problem between vSend() and filter rule.
	//
	// If filter is implemented on userland,
	// it can be fix more cool !!
	//
	// The problem is ...
	// FIFO is on userland, but filter is on the kernel.
	// When the tool sends packet which is matched to filter rule,
	// it causes mismatch in FIFO,
	// and falls in the infinite block in recv().
	//
	if(!rc && !fifo.IsEmpty()) {
		fifo.del();
	}

	for(int i=0; i < rc; ) {
#ifdef VCLEAR_DBG
xdbg("/tmp/vclear_dbg.txt", "BpfAgent", "i: %d\n", i);
#endif	// VCLEAR_DBG
		char *next = buffer_ + i;
		struct bpf_hdr *hdr = (struct bpf_hdr *)next;
		int datasize = hdr->bh_caplen + hdr->bh_hdrlen;

		if(!fifo.IsEmpty() &&
		    (fifo.firstdatalen() == (int)hdr->bh_caplen) &&
		    (memcmp(
			fifo.firstdata(),
			next+hdr->bh_hdrlen,
			hdr->bh_caplen
		    ) == 0)
		) {
			fifo.del();
		} else if(runStat() == eRun_) {
			rbuf->write(next, datasize);
		}

		if(datasize > rc) {
			abort();
		}

		i += BPF_WORDALIGN(datasize);
	}

	if(!rbuf->isEmpty()) {
		struct bpf_hdr *hdr = (struct bpf_hdr *)buffer_;
		int datasize = hdr->bh_caplen + hdr->bh_hdrlen;
		readydata(buffer_, datasize);
	}

	return(0);
}

int BpfAgent::readydata(char *,int32_t) {return 0;}

int BpfAgent::read(caddr_t buf, uint32_t len){
	int rc;
	if(len < rbuf->size()|| rbuf->isEmpty())
	  return 0;
	char *rb=rbuf->read(buf);
	if(rb==NULL)
	  return 0;
	struct bpf_hdr *hdr = (struct bpf_hdr *)rb;
	rc=hdr->bh_caplen + hdr->bh_hdrlen;
	return rc;}

int BpfAgent::readn(caddr_t buf, uint32_t n){
	int rc;
	if(rbuf->readn(n,buf)==0) rc=0;  
	else {
		struct bpf_hdr *hdr = (struct bpf_hdr *)buf;
		rc=hdr->bh_caplen + hdr->bh_hdrlen;}
	return rc;}

timeval* BpfAgent::ReceiveTimeOfOldestData() const{
	if(rbuf->isEmpty())
		return 0;
	return (timeval*)rbuf->readn(0,0);}

int BpfAgent::write(caddr_t p, uint32_t l) {
#ifdef VCLEAR_DBG
xdbg("/tmp/vclear_dbg.txt", "BpfAgent", "this: %p\n", this);
#endif	// VCLEAR_DBG
	fifo.add(p,l);
	return filter().send(p,l);}

bufStat BpfAgent::stat() const{
	struct bufStat rc;
	rc.datanum(rbuf->datanum());
	rc.size(rbuf->size());
	uint32_t recv,drop;
	filter().statistics(recv,drop);
	rc.recv(recv);
	rc.drop(drop);
	return rc;}

void BpfAgent::clear() {
#if 1
	/*
	  In rare case, clear() function is called before filterd out
	  all sent packets. So need to wait all sent packets filtered
	  out.

	  XXX : This routine is not enough tested.
	  */
	while(!fifo.IsEmpty()) {
#ifdef VCLEAR_DBG
xdbg("/tmp/vclear_dbg.txt", "BpfAgent", "this: %p\n", this);
#endif	// VCLEAR_DBG
		nonblock_receive(0);
	}
#endif
	rbuf->clear();
	filter_.flush();}

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
 * $TAHI: v6eval/lib/pkt/Bpfilter.cc,v 1.17 2005/07/21 01:53:22 akisada Exp $
 */
#include "Bpfilter.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <net/bpf.h>
#include <net/if.h>

#include <sys/types.h>
#include "debug.h"

Bpfilter::Bpfilter(CSTR n):fd_(-1),bufsize_(0) {
	int i, fd=-1;
	char dev[32];
	struct ifreq ifr;
	for(i=0;;i++) {
		snprintf(dev,sizeof(dev),"/dev/bpf%d",i);
		fd=::open(dev,O_RDWR);
		if(fd<0) {
			if(errno==EBUSY) continue;
			else {perror("err:open");}}
		break;}
	if(fd<0) return;

	bufsize_=BPF_MAXBUFSIZE;
	if(ioctl(fd,BIOCSBLEN,(caddr_t)&bufsize_)<0){
		perror("err:ioctl(BIOCSBLEN)");
		::close(fd);
		return;}

#ifdef HAVE_BPF_HDRCMPLT
	uint32_t flag = 1;
	if(ioctl(fd,BIOCSHDRCMPLT,&flag)<0){
		perror("err:ioctl(BIOCSHDRCMPLT)");
		::close(fd);
		return;}
#endif

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name,n);
	if(ioctl(fd,BIOCSETIF,(caddr_t)&ifr)<0) {
		perror("err:ioctl(BIOCSETIF)");
		::close(fd);
		return;}
	if(ioctl(fd,BIOCGBLEN,(caddr_t)&bufsize_)<0){
		perror("err:ioctl(BIOCGBLEN)");
		::close(fd);
		return;}
	fd_=fd;}

int Bpfilter::setfilter(struct bpf_program *filter) const {
	int fd=fileDesc();
	if(fd<0) return -1;
	int rc = ioctl(fd,BIOCSETF,filter);
	if(rc<0){perror("err:ioctl(BIOCSETF)");}
	return rc;
}

int Bpfilter::promiscuous() const {
	int fd=fileDesc();
	if(fd<0) return -1;
	int rc=ioctl(fd,BIOCPROMISC,NULL);
	if(rc<0) {perror("err:ioctl(BIOCPROMISC)");}
	return rc;}

int Bpfilter::flush() const {
	int fd=fileDesc();
	if(fd<0) return -1;
	int rc=ioctl(fd,BIOCFLUSH,NULL);
	if(rc<0) {perror("err:ioctl(BIOCFLUSH)");}
	return rc;}

int Bpfilter::immediate(uint32_t n) const {
	int fd=fileDesc();
	if(fd<0) return -1;
	int rc=ioctl(fd,BIOCIMMEDIATE,&n);
	if(rc<0) {perror("err:ioctl(BIOCIMMEDIATE)");}
	return rc;}

int Bpfilter::statistics(uint32_t& recv,uint32_t& drop) const {
	struct bpf_stat stat;
	int fd=fileDesc();
	recv=0; drop=0;
	if(fd<0) return -1;
	int rc=ioctl(fd,BIOCGSTATS,(caddr_t)&stat);
	if(rc<0) {perror("err:ioctl(BIOCGSTATS)");}
	else {recv=stat.bs_recv; drop=stat.bs_drop;}
	return rc;}

int Bpfilter::receive(caddr_t p) const {
	int fd=fileDesc();
	if(fd<0) return -1;
	uint32_t l=bufferSize();
#ifdef VCLEAR_DBG
xdbg("/tmp/vclear_dbg.txt", "Bpfilter", "l: %d\n", l);
#endif	// VCLEAR_DBG
	int rc=::read(fd,p,l);
#ifdef VCLEAR_DBG
xdbg("/tmp/vclear_dbg.txt", "Bpfilter", "l: %d, rc: %d\n", l, rc);
#endif	// VCLEAR_DBG
	if(rc<0) {perror("err:read");}
	return rc;}

int
Bpfilter::nonblock_receive(caddr_t p) const
{
	int rc = 0;
	int fd = fileDesc();

	if(fd<0) {
		return(-1);
	}

	uint32_t l = bufferSize();

#ifdef VCLEAR_DBG
xdbg("/tmp/vclear_dbg.txt", "Bpfilter", "l: %d\n", l);
#endif	// VCLEAR_DBG

	int nfds = fd + 1;
	fd_set readfds;
	struct timeval timeout;

	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);

	timeout.tv_sec	= 0;
	timeout.tv_usec	= 0;

	rc = ::select(nfds, &readfds, NULL, NULL, &timeout);
	if(rc < 0) {
		perror("err:select");
	}

#ifdef VCLEAR_DBG
xdbg("/tmp/vclear_dbg.txt", "Bpfilter", "l: %d, rc: %d\n", l, rc);
#endif	// VCLEAR_DBG

	if(rc && FD_ISSET(fd, &readfds)) {
		FD_CLR(fd, &readfds);

		rc = ::read(fd, p, l);

#ifdef VCLEAR_DBG
xdbg("/tmp/vclear_dbg.txt", "Bpfilter", "l: %d, rc: %d\n", l, rc);
#endif	// VCLEAR_DBG

		if(rc < 0) {
			perror("err:read");
		}
	}

	return(rc);
}

int Bpfilter::send(caddr_t p, uint32_t l) const {
	int fd=fileDesc();
	if(fd<0) return -1;
	if(l>bufferSize()){perror("err:send packet too long");return -1;};
#ifdef VCLEAR_DBG
xdbg("/tmp/vclear_dbg.txt", "Bpfilter", "l: %d\n", l);
#endif	// VCLEAR_DBG
	int rc=::write(fd,p,l);
#ifdef VCLEAR_DBG
xdbg("/tmp/vclear_dbg.txt", "Bpfilter", "l: %d, rc: %d\n", l, rc);
#endif	// VCLEAR_DBG
	if(rc<0) {perror("err:write");}
	return rc;}

uint32_t Bpfilter::getDLT() const {
	int fd=fileDesc();
	if(fd<0) return 0xffff; /* xxx */
	uint32_t dlt;
	int rc=ioctl(fd,BIOCGDLT,&dlt);
	if(rc<0) {perror("err:ioctl(BIOCGSTATS)");}
	return dlt;}


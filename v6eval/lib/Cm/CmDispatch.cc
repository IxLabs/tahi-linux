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
 * $TAHI: v6eval/lib/Cm/CmDispatch.cc,v 1.8 2001/10/12 04:56:13 tanaka Exp $
 */
#include "CmDispatch.h"
//IMPLEMENTATION
// CmDispatch provides an interface to the "select" system call.

#include "CmReceiver.h"
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/param.h>
#include "CmFdSet.h"
#include "Timer.h"
#include "CmFdMasks.h"
#include "PerfCollect.h"

CmDispatch::CmDispatch():nfds_(0),
	onRead_(new CmFdMasks(NOFILE)),
	onWrite_(new CmFdMasks(NOFILE)),
	onExcept_(new CmFdMasks(NOFILE)),
	queue_(new TimerQueue),
	collect_(new PerformanceCollector(-1,NOFILE)) {
	}

CmDispatch::~CmDispatch() {
	delete collect_, collect_=0;
	delete onRead_, onRead_=0;
	delete onWrite_, onWrite_=0;
	delete onExcept_, onExcept_=0;
	delete queue_, queue_=0;}

CmDispatch& CmDispatch::instance() {
	if(instance_ == 0) {
		instance_ = new CmDispatch;}
	return *instance_;}

void CmDispatch::instance(CmDispatch* d) {
	if(instance_) delete instance_;
	instance_=d;}
CmFdMasks* CmDispatch::maskHandler(CmDispatchMask mask) const {
	CmFdMasks* m=0;
	switch(mask) {
		case ReadMask:	{m=onRead_; break;}
		case WriteMask:	{m=onWrite_; break;}
		case ExceptMask:{m=onExcept_; break;}
		default: abort();}
	return m;}

CmReceiver* CmDispatch::handler(int fd, CmDispatchMask mask) const {
	if(fd<0||fd>=NOFILE) {abort();}
	return maskHandler(mask)->handler(fd);}

void CmDispatch::attach(int fd, CmDispatchMask mask, CmReceiver* handler) {
	maskHandler(mask)->attach(fd,handler);
	if(nfds_<fd+1) {nfds_=fd+1;}}

void CmDispatch::link(int fd, CmDispatchMask mask, CmReceiver* handler) {
	if(fd<0||fd>=NOFILE) {abort();}
	attach(fd, mask, handler);}

void	CmDispatch::reduce_nfds(int fd){
	if(nfds_!=fd+1) {return;}
	for(;nfds_>0;nfds_--) {
		int i=nfds_-1;
		if(	onRead_->handler(i)!=0||
			onWrite_->handler(i)!=0||
			onWrite_->handler(i)!=0) {
			break;}}}

void CmDispatch::detach(int fd, CmDispatchMask mask){
	maskHandler(mask)->detach(fd);}

void	CmDispatch::detach(int fd){ //rewote
	detach(fd, ReadMask);
	detach(fd, WriteMask);
	detach(fd, ExceptMask);
	reduce_nfds(fd);}

void CmDispatch::unlink(int fd, CmDispatchMask mask) {
	if(fd<0||fd>=NOFILE) {abort();}
 	detach(fd, mask);
	reduce_nfds(fd);}

void CmDispatch::unlink(int fd) {
	if(fd<0||fd>=NOFILE) {abort();}
	detach(fd);}

void CmDispatch::startTimer(time_t sec, uint32_t usec, CmReceiver* handler) {
	timeval deltaTime;
	deltaTime.tv_sec = sec; deltaTime.tv_usec = usec;
	queue_->deltaInsert(deltaTime,handler);}

void	CmDispatch::startTimerByAbsoluteTime(time_t sec, CmReceiver* handler) {
	timeval unixTime;
	unixTime.tv_sec=sec; unixTime.tv_usec=0;
	queue_->insert(unixTime, handler);}

void	CmDispatch::startTimerByAbsoluteTime(timeval unixTime, CmReceiver* handler) {
	queue_->insert(unixTime, handler);}


void CmDispatch::stopTimer(CmReceiver* handler) {
	queue_->remove(handler);}

void CmDispatch::expire(CmReceiver *h,timeval& t) {
	if(h==0) return;
	startTransaction();
	h->timerExpired(t.tv_sec,t.tv_usec);
	recordTransaction(-1);}

int CmDispatch::notify(int n,CmReceiver *h,ioNotify func) {
	if(h==0) return 0;
	startTransaction();
	int status=(h->*func)(n);
	recordTransaction(n);
	return status;}
	
void CmDispatch::notify(int n,CmFdMasks *m,ioNotify func) {
	int status=notify(n,m->notify(n),func);
	/**/ if(status<0 ) {detach(n);}
	else if(status==0) {m->clearReady(n);}
	else if(status> 0) {m->setReady(n);}}

void CmDispatch::notify() {
	for(int i=0;i<nfds_;i++){
		notify(i,onRead_,&CmReceiver::inputReady);
		notify(i,onWrite_,&CmReceiver::outputReady);
		notify(i,onExcept_,&CmReceiver::exceptionRaised);}
	timeval current=TimerQueue::currentTime();
	CmReceiver* h;
	for(;(h=queue_->expire(current));) {
		expire(h,current);}}

int CmDispatch::select(CmFdSet& r,CmFdSet& w,CmFdSet& e,timeval* howlong) {
	return ::select(nfds_,r,w,e,howlong);}

int CmDispatch::waitFor(timeval* howlong) {
	int nfound=-1;
	for(;nfound<0;) {
		CmFdSet& r=onRead_->onSelects();
		CmFdSet& w=onWrite_->onSelects();
		CmFdSet& e=onExcept_->onSelects();
		howlong = queue_->calculateTimeout(howlong);
		nfound=select(r,w,e,howlong);
		if(nfound<0) {handleError();}}
	return nfound;}

bool CmDispatch::dispatch(timeval* howlong) {
	int nfound;
	static timeval timeout;
	if(anyReady()){howlong=&timeout;}
	nfound=waitFor(howlong);
	notify();
	return nfound>0;}

bool CmDispatch::dispatch(time_t& sec, uint32_t& usec) {
	timeval howlong;
	timeval prevTime;
	howlong.tv_sec = sec; howlong.tv_usec = usec;
	prevTime = TimerQueue::currentTime();
	bool success = dispatch(&howlong);
	howlong = TimerQueue::spendTime(prevTime,howlong);
	sec = howlong.tv_sec; usec = howlong.tv_usec;
	return success;}

void CmDispatch::dispatch() {
	dispatch(0);}

bool CmDispatch::anyReady() const {
	bool r=onRead_->anySet(), w=onWrite_->anySet(),e=onExcept_->anySet();
	return r||w||e;}

void CmDispatch::handleError() {
	if(errno == EINTR) {return;}
	if(errno == EBADF) {
		checkConnections(); return;}
	eerr("CmDispatch: select");
	abort();}

void CmDispatch::checkConnections() {
	timeval zero=TimerQueue::zeroTime();
	onRead_->checkConnections(nfds_,zero);}

void CmDispatch::startTransaction() {
	collect_->startTransaction();}
void CmDispatch::recordTransaction(int fd) {
	collect_->recordTransaction(fd);
	if(transactionVector_) (*transactionVector_)(fd);}
void CmDispatch::reportPerformance() {
	collect_->report();}
void CmDispatch::clearPerformance() {
	collect_->clear();}

CmDispatch* CmDispatch::instance_=0;
CmDispatch::vector CmDispatch::transactionVector_=0;

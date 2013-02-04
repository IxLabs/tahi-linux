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
 * $TAHI: v6eval/lib/Cm/Timer.cc,v 1.4 2001/10/12 04:56:14 tanaka Exp $
 */
#include "Timer.h"
//IMPLEMENTATION
#include "timeval.h"
#include "CmReceiver.h"
struct Timer {
	timeval value_;
	CmReceiver* receiver_;
	Timer* next_;
	Timer(const timeval t, CmReceiver* h, Timer* n);
	};
Timer::Timer(timeval t, CmReceiver* h, Timer* n):value_(t),receiver_(h),next_(n) {}

inline bool TimerQueue::isEmpty() const {
	return first_==0;}

inline timeval TimerQueue::earliestTime() const {
	return first_->value_;}

void TimerQueue::deltaInsert(timeval delta,CmReceiver *h) {
	insert(currentTime()+delta,h);}
timeval TimerQueue::spendTime(timeval p,timeval howlong) {
	timeval elapsed=currentTime()-p;
	return (howlong>elapsed)?howlong-elapsed:zeroTime();}

TimerQueue::TimerQueue():first_(0) {}
TimerQueue::~TimerQueue() {
	Timer* doomed=first_;
	while(doomed!=0) {
		Timer* next=doomed->next_;
		delete doomed;
		doomed=next;}
	first_=0;}

timeval TimerQueue::currentTime() {
	timeval curTime;
	struct timezone curZone;
	gettimeofday(&curTime, &curZone);
	return curTime;}

void TimerQueue::insert(const timeval futureTime, CmReceiver* r) {
	if(isEmpty() || futureTime < earliestTime()) {
		first_=new Timer(futureTime, r, first_);}
	else {
		Timer* before=first_;
		Timer* after=first_->next_;
		while(after!=0 && futureTime > after->value_) {
			before=after;
			after=after->next_;}
		before->next_=new Timer(futureTime, r, after);}}

void TimerQueue::remove(CmReceiver* r) {
	Timer* before=0;
	Timer* doomed=first_;
	while(doomed!=0 && doomed->receiver_!=r) {
		before=doomed;
		doomed=doomed->next_;}
	if(doomed!=0) {
		if(before==0) {
			first_=doomed->next_;}
		else {
			before->next_=doomed->next_;}
	delete doomed;}}

timeval *TimerQueue::calculateTimeout(timeval* howlong) const {
	static timeval timeout;
	if(isEmpty()) return howlong;
	timeval curTime=currentTime();
	timeval ealiest=earliestTime();
	if(ealiest>curTime) {
		timeout=ealiest-curTime;
		if(howlong==0|| *howlong>timeout) {
			howlong=&timeout;}}
	else {
		timeout=TimerQueue::zeroTime();
		howlong=&timeout;}
	return howlong;}

CmReceiver* TimerQueue::expire(const timeval curTime) {
	if(isEmpty()) return 0;
	timeval ealiest=earliestTime();
	if(ealiest>curTime) return 0;
	Timer* expired=first_;
	first_=first_->next_;
	CmReceiver *h=expired->receiver_;
	delete expired;
	return h;}

timeval TimerQueue::zeroTime_={0,0};

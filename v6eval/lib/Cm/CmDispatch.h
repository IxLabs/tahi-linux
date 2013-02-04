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
 * $TAHI: v6eval/lib/Cm/CmDispatch.h,v 1.6 2001/10/12 04:56:13 tanaka Exp $
 */
#ifndef _Cm_CmDispatch_h_
#define _Cm_CmDispatch_h_	1
/*
 * Wait on multiple file descriptors until a condition occurs.
 */
#include "CmTypes.h"
class CmFdSet;
class CmReceiver;
class TimerQueue;
class CmFdMasks;
class PerformanceCollector;
#include <sys/time.h>

typedef int (CmReceiver::*ioNotify)(int);
class CmDispatch {
public:
	typedef void (*vector)(int);
private:
	int	nfds_;
	CmFdMasks* onRead_;
	CmFdMasks* onWrite_;
	CmFdMasks* onExcept_;
	TimerQueue* queue_;
	PerformanceCollector* collect_;
static	CmDispatch* instance_;
static	vector transactionVector_;
public:
	enum CmDispatchMask {ReadMask,WriteMask,ExceptMask};
	CmDispatch();
virtual ~CmDispatch();
virtual void link(int fd, CmDispatchMask, CmReceiver*);
virtual	void unlink(int fd, CmDispatchMask);
virtual void unlink(int fd);
virtual void startTimerByAbsoluteTime(time_t sec, CmReceiver*);
virtual void startTimerByAbsoluteTime(timeval unixTime, CmReceiver*);
virtual void startTimer(time_t sec, uint32_t usec, CmReceiver*);
virtual void stopTimer(CmReceiver*);
virtual bool dispatch(time_t& sec, uint32_t& usec);
virtual void dispatch();
virtual CmReceiver* handler(int fd,CmDispatchMask) const;
virtual	int select(CmFdSet& r,CmFdSet& w,CmFdSet& e,timeval* howlong);
static CmDispatch& instance();
	int count();
protected:
static void instance(CmDispatch*);
virtual void attach(int fd, CmDispatchMask, CmReceiver*);
virtual void detach(int fd);
virtual bool dispatch(timeval*);
virtual bool anyReady() const;
virtual int waitFor(timeval*);
virtual void expire(CmReceiver *h,timeval& t);
virtual int notify(int i,CmReceiver *h,ioNotify func);
virtual void notify(int i,CmFdMasks *m,ioNotify func);
virtual void notify();
virtual void handleError();
virtual void checkConnections();
private:
	CmFdMasks* maskHandler(CmDispatchMask mask) const;
	void detach(int fd, CmDispatchMask);
	void reduce_nfds(int fd);
	void startTransaction();
	void recordTransaction(int fd);
public:
	void reportPerformance();
	void clearPerformance();
static	vector setTransactionVector(vector);
};
inline int CmDispatch::count() {return nfds_;}
#endif

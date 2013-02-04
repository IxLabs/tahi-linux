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
 * $TAHI: v6eval/lib/Cm/PerfCollect.cc,v 1.10 2001/10/12 04:56:14 tanaka Exp $
 */
#include "PerfCollect.h"
//IMPLEMENTATION
#include <stdio.h>
#include <stdarg.h>
#include "timeval.h"
inline const int PerformanceCollector::origin() const {
	return origin_;}
inline const int PerformanceCollector::offset(int i) const {
	return i-origin();}
inline const int PerformanceCollector::origin(const int i) const {
	return origin()+i;}
inline const u_long PerformanceCollector::noOfRecorder() const {
	return noOfRecorder_;}

class PerformanceRecorder {
	TV maximum, total;
	long count;
public:
	void clear();
	PerformanceRecorder();
//	~PerformanceRecorder();
	void recordTransaction(TV&);
	void report(int i) const;
};

void PerformanceRecorder::clear() {
	timerclear(&maximum); timerclear(&total); count=0;}
PerformanceRecorder::PerformanceRecorder() {clear();}
void PerformanceRecorder::recordTransaction(TV &d) {
	total=total+d;
	if(maximum<d) maximum=d;
	count++;}
void PerformanceRecorder::report(int i) const {
	if(count==0) return;
	double av=((double)total.tv_sec+((double)total.tv_usec)/ONE_SECOND)/count;
	ooutf("NO=%d\ttotal=%6d.%06d\tcount=%4d\tav=%9.6f\tmax=%2d.%06d\n",
		i,total.tv_sec, total.tv_usec,count, av,
		maximum.tv_sec, maximum.tv_usec);}

PerformanceCollector::PerformanceCollector(u_long n):
	origin_(0),noOfRecorder_(n),
	recorder_(new PerformanceRecorder[n+1]) {}
PerformanceCollector::PerformanceCollector(int from,int to):
	origin_(from),noOfRecorder_(to-from),
	recorder_(new PerformanceRecorder[to-from+1]) {}

void PerformanceCollector::report() const {
	for(u_long i=0;i<noOfRecorder();i++) {recorder_[i].report(origin(i));}}
void PerformanceCollector::clear() {
	report();
	for(u_long i=0;i<noOfRecorder();i++) {recorder_[i].clear();}}
PerformanceCollector::~PerformanceCollector() {
	report();
	delete [] recorder_;}
void PerformanceCollector::recordTransaction(int n,TV &d) {
	recorder_[n].recordTransaction(d);}

TV	PerformanceCollector::transactionLimit_={10,0};
void PerformanceCollector::startTransaction() {TZ z;
	gettimeofday(&startTransactionTime_,&z);}
TV &PerformanceCollector::elapsedTransaction() {static TV zero,d; TV e; TZ z;
	gettimeofday(&e,&z);
	d=(e>startTransactionTime_)?e-startTransactionTime_:zero;
	return d;}
void PerformanceCollector::recordTransaction(int n) {
	recordTransaction(offset(n),elapsedTransaction());}
void PerformanceCollector::overTransactionTimeBreak() {}
void PerformanceCollector::checkTransactionTime() {
	if(elapsedTransaction()>PerformanceCollector::transactionLimit_) {
		overTransactionTimeBreak();}}

#include <stdlib.h>
#if 0

static struct mallinfo x[2];
static int mallocCurrent=0;
static char sep[]="-----------------------------------------------------------";
void putMemoryInfo() {
	int Old=mallocCurrent, New=(mallocCurrent^=1);
	x[New]=mallinfo();
	ooutf(sep);
	ooutf("%8d %8d  total space in arena",
		x[New].arena,x[New].arena-x[Old].arena);
	ooutf("%8d %8d  number of ordinary blocks",
		x[New].ordblks,x[New].ordblks-x[Old].ordblks);
	ooutf("%8d %8d  number of small blocks",
		x[New].smblks,x[New].smblks-x[Old].smblks);
	ooutf("%8d %8d  number of holding blocks",
		x[New].hblks,x[New].hblks-x[Old].hblks);
	ooutf("%8d %8d  space in holding block headers",
		x[New].hblkhd,x[New].hblkhd-x[Old].hblkhd);
	ooutf("%8d %8d  space in small blocks in use",
		x[New].usmblks,x[New].usmblks-x[Old].usmblks);
	ooutf("%8d %8d  space in free small blocks",
		x[New].fsmblks,x[New].fsmblks-x[Old].fsmblks);
	ooutf("%8d %8d  space in ordinary blocks in use",
		x[New].uordblks,x[New].uordblks-x[Old].uordblks);
	ooutf("%8d %8d  space in free ordinary blocks",
		x[New].fordblks,x[New].fordblks-x[Old].fordblks);
	ooutf("%8d %8d  cost of enabling keep option",
		x[New].keepcost,x[New].keepcost-x[Old].keepcost);
#if !defined(__hpux)
	ooutf("%8d %8d  max size of small blocks",
		x[New].mxfast,x[New].mxfast-x[Old].mxfast);
	ooutf("%8d %8d  number of small blocks in a holding block",
		x[New].nlblks,x[New].nlblks-x[Old].nlblks);
	ooutf("%8d %8d  small block rounding factor",
		x[New].grain,x[New].grain-x[Old].grain);
	ooutf("%8d %8d  space(including overhead) allocated in ord. blks",
		x[New].uordbytes,x[New].uordbytes-x[Old].uordbytes);
	ooutf("%8d %8d  number of ordinary blocks allocated",
		x[New].allocated,x[New].allocated-x[Old].allocated);
	ooutf("%8d %8d  bytes used in maintaining the free tree",
		x[New].treeoverhead,x[New].treeoverhead-x[Old].treeoverhead);
#endif
	ooutf(sep);}
#endif

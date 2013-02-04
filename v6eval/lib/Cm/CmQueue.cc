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
 * $TAHI: v6eval/lib/Cm/CmQueue.cc,v 1.8 2008/04/02 07:35:30 akisada Exp $
 */
#include "CmQueue.h"
//IMPLEMENTATION
#include "CmTypes.h"
#include <stdlib.h>
CmQueue::CmQueue() {initialize();}
CmQueue* CmQueue::deque()	{
	CmQueue *p=prev(), *r=next();
	p->next(r); r->prev(p);
	return initialize();}

CmQueue *CmQueue::enque(CmQueue *q) {
	if(q==0) {abort();}
	CmQueue *r=next();
	q->next(r); q->prev(this);
	next(q); r->prev(q);
	return this;}

void CmQueue::print(STR s) {
	ooutf("%s,this=%#x,next=%#x,prev=%#x\n",s,this,next_,prev_);}
void CmQueue::print() {print((STR)"\t");}

CmQueue::~CmQueue() {
	initialize();}

#if defined(TEST)
typedef CmQueue *id;
main(int argc,STR argv[]) {
	static id start;
	int i; id p;
	start=new CmQueue;
	start->print();
	for(i=0;i<10;i++) {
		start->insert(new CmQueue);}
	ooutf("---next chain----------------------\n");
	start->print("start");
	for(p=start;(p=p->next())!=start;) {
		p->print();}
	ooutf("---prev chain----------------------\n");
	start->print("start");
	for(p=start;(p=p->prev())!=start;) {
		p->print();}
	ooutf("---prev chain----------------------\n");
	for(;(p=start->next())!=start;) {
		delete (p->deque())->print();}
	(p->deque())->print("end");
	start->print("start");
}
#endif

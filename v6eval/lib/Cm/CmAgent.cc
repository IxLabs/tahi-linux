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
 * $TAHI: v6eval/lib/Cm/CmAgent.cc,v 1.6 2001/10/12 04:56:13 tanaka Exp $
 */
#include "CmAgent.h"
//IMPLEMENTATION
#include "CmDispatch.h"
int CmAgent::fileDesc() const {return -1;}
int CmAgent::inputReady(int fd) {return (this->*readAction_)(fd);}
int CmAgent::outputReady(int fd) {return (this->*writeAction_)(fd);}
int CmAgent::exceptionRaised(int fd) {return (this->*exceptAction_)(fd);}
void CmAgent::timerExpired(time_t s,uint32_t m) {(this->*timerAction_)(s,m);}
void CmAgent::timerAction(timerFunc f) {timerAction_=f;}

CmAgent::~CmAgent() {}
CmAgent::CmAgent():CmReceiver(),
	readAction_(0),writeAction_(0),exceptAction_(0),timerAction_(0) {}

void CmAgent::readAction(agentFunc f) {
	CmDispatch* d=dispatch();
	readAction_=f;
	if(f!=0) {
		d->link(fileDesc(),CmDispatch::ReadMask,this);}
	else {
		d->unlink(fileDesc(),CmDispatch::ReadMask);}}

void CmAgent::writeAction(agentFunc f) {
	CmDispatch* d=dispatch();
	writeAction_=f;
	if(f!=0) {
		d->link(fileDesc(),CmDispatch::WriteMask,this);}
	else {
		d->unlink(fileDesc(),CmDispatch::WriteMask);}}

void CmAgent::exceptAction(agentFunc f) {
	CmDispatch* d=dispatch();
	exceptAction_=f;
	if(f!=0) {
		d->link(fileDesc(),CmDispatch::ExceptMask,this);}
	else {
		d->unlink(fileDesc(),CmDispatch::ExceptMask);}}

void CmAgent::unlink() {
	int fd=fileDesc();
	if(fd>=0) {
		dispatch()->unlink(fd);
		readAction_=0; writeAction_=0; exceptAction_=0;}}

CmDispatch* CmAgent::dispatch() {
	if(dispatch_==0) {
		CmDispatch& d=CmDispatch::instance();
		dispatch_=&d;}
	return dispatch_;}

#include <stdio.h>
void CmAgent::dump(int32_t l,CSTR s) const {
	CSTR pad="\n";
	for(int i=0;i<l;i++) {
		printf("%s%02x",pad,s[i]&0xff);
		switch((i%32)+1) {
		case 4: case 12: case 20: case 28:	pad=" "; break;
		case 8: case 16: case 24:		pad="  "; break;
		case 32:				pad="\n"; break;
		default:				pad=""; break;}}}

CmDispatch* CmAgent::dispatch_=0;

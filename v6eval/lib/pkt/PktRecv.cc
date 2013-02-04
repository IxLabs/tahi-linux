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
 * $TAHI: v6eval/lib/pkt/PktRecv.cc,v 1.12 2005/07/21 01:53:22 akisada Exp $
 */
#include "PktRecv.h"
#include "CmMain.h"
#include "PvOctets.h"
#include <net/ethernet.h>
#include <sys/errno.h>
#include <stdio.h>
#include "debug.h"
PktReceiver::PktReceiver(CmSocket* s):PktAgent(s),
	header_(),allocated_(0),buffer_(0) {}
PktReceiver::~PktReceiver() {
	if(buffer_) {delete [] buffer_; buffer_=0;}}

//======================================================================
// BUFFER MANAGEMENT
STR PktReceiver::buffer(int32_t l) {
	if(allocated_<l) {
		delete [] buffer_; buffer_=0;
		int32_t n=ETHER_MAX_LEN<l?l:ETHER_MAX_LEN;
		buffer_=new char[(allocated_=round8(n))];}
	return buffer_;}

int PktReceiver::receiveBuffer(STR s,int32_t len) {
	int32_t rem, l; STR p;
	for(rem=len,p=s;rem>0;rem-=l,p+=l) {
		l=socket()->receive(s,rem);
		if(l<0&&errno==EINTR) {l=0; continue;}
		if(l<=0) return -1;}
	return len;}

//======================================================================
// RESPONSE
int PktReceiver::response(int32_t r,int32_t l,CSTR s) {
#ifdef VCLEAR_DBG
xdbg("/tmp/vclear_dbg.txt", "PktReceiver", "this: %p\n", this);
#endif  // VCLEAR_DBG
	tcpHead h=header();
	h.request(h.request()|eResponse_);
	h.result(r);
	return sendPacket(h,l,s)>=0?0:-1;}

//======================================================================
// DISTRIBUTE INCOMING PACKET TO FUNCTION
int PktReceiver::analizeRequest(int32_t r) {
	int rc=0;
	switch(r) {
		case eConnect_:	rc=connectRequest();		break;
		case eClose_:	rc=closeRequest();		break;
		case eWrite_:	rc=writeRequest();		break;
		case eRead_:	rc=readRequest();		break;
		case eCapture_:	rc=captureRequest();		break;
		case eStop_:	rc=stopRequest();		break;
		case eWrap_:	rc=wrapRequest();		break;
		case eNowrap_:	rc=nowrapRequest();		break;
		case eClear_:   rc=clearRequest();		break;
		case eDump_:	rc=dumpRequest();		break;
		case eData_:	rc=dataIndication();		break;
		case eStatus_:	rc=statusRequest();		break;}
	return rc;}

int PktReceiver::analizeConfirmation(int32_t r) {
	int	rc=0;
	switch(r) {
		case eConnect_:	rc=connectConfirmation();	break;
		case eClose_:	rc=closeConfirmation();		break;
		case eWrite_:	rc=writeConfirmation();		break;
		case eRead_:	rc=readConfirmation();		break;
		case eCapture_:	rc=captureConfirmation();	break;
		case eStop_:	rc=stopConfirmation();		break;
		case eWrap_:	rc=wrapConfirmation();		break;
		case eNowrap_:	rc=nowrapConfirmation();	break;
		case eClear_:	rc=clearConfirmation();		break;
		case eDump_:	rc=dumpConfirmation();		break;
		case eData_:	rc=dataResponse();		break;
		case eStatus_:	rc=statusConfirmation();	break;}
	return rc;}

//======================================================================
// ANALYZE INCOMING PACKET
int PktReceiver::analizePacket() {
	int rc=0;
	eRequest r=(eRequest)request();
	if(DBGFLAGS('i')) {
		printf("PktReceiver::analizePacket  request=%4.4x\n",r&0xffff);}
	if(DBGFLAGS('x')) {
		printf("PktReceiver::analizePacket\n");
		printf("    request=%4.4x length=%d, result=%d\n",
		       r&0xffff,header().length(),header().result());
		PvOctets body(header().length(),(OCTSTR)buffer(0));
		body.print();
		printf("\n");}
	if(r&eResponse_)	{rc=analizeConfirmation(r&eMask_);}
	else			{rc=analizeRequest(r);}
	return rc;}

//======================================================================
// RECEIVING PACKET
int PktReceiver::receiveHeader() {
	return receiveBuffer((STR)&header_,sizeof(header_));}
	
int PktReceiver::receiveBody() {
	int l=bodyLength();
	if(l==0) {return 0;}
	return receiveBuffer(buffer(l),l);}

int PktReceiver::receivePacket(int) {
	if(receiveHeader()<0||receiveBody()<0) {
		if(DBGFLAGS('c')) {printf("connection lost "); socket()->print();}
		return terminate();}
	return analizePacket();}

//======================================================================
// REQUEST
int PktReceiver::connectRequest() {return 0;}
int PktReceiver::closeRequest() { response(0,0,0); return terminate();}
int PktReceiver::writeRequest() {return 0;}
int PktReceiver::readRequest() {return 0;}
int PktReceiver::captureRequest() {return 0;}
int PktReceiver::stopRequest() {return 0;}
int PktReceiver::wrapRequest() {return 0;}
int PktReceiver::nowrapRequest() {return 0;}
int PktReceiver::clearRequest() {return 0;}
int PktReceiver::dumpRequest() {return 0;}
int PktReceiver::dataIndication() {return 0;}
int PktReceiver::statusRequest() {return 0;}
int PktReceiver::dataRequest() {return 0;}

//======================================================================
// CONFIRMATION
int PktReceiver::connectConfirmation() {return 0;}
int PktReceiver::closeConfirmation() {return 0;}
int PktReceiver::writeConfirmation() {return 0;}
int PktReceiver::readConfirmation() {return 0;}
int PktReceiver::captureConfirmation() {return 0;}
int PktReceiver::stopConfirmation() {return 0;}
int PktReceiver::wrapConfirmation() {return 0;}
int PktReceiver::nowrapConfirmation() {return 0;}
int PktReceiver::clearConfirmation() {return 0;}
int PktReceiver::dumpConfirmation() {return 0;}
int PktReceiver::dataResponse() {return 0;}
int PktReceiver::statusConfirmation() {return 0;}
int PktReceiver::dataConfirmation() {return 0;}


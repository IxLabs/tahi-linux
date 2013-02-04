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
 * $TAHI: v6eval/lib/pkt/PktAgent.h,v 1.6 2001/10/05 11:41:19 tanaka Exp $
 */
#if !defined(__PktAgent_h__)
#define __PktAgent_h__	1
#include "CmSocket.h"
#include "CmAgent.h"
#include "pktDefine.h"

struct tcpHead {
private:
	int16_t request_;
	int16_t length_;
	int32_t result_;
public:
	int16_t request() const;
	int16_t length() const;
	int32_t result() const;
	void request(int16_t);
	void length(int16_t);
	void result(int32_t);
	tcpHead(int16_t =0);
};
inline int16_t tcpHead::request() const {return ntohs(request_);}
inline int16_t tcpHead::length() const {return ntohs(length_);}
inline int32_t tcpHead::result() const {return ntohl(result_);}
inline void tcpHead::request(int16_t v) {request_=htons(v);}
inline void tcpHead::length(int16_t v) {length_=htons(v);}
inline void tcpHead::result(int32_t v) {result_=htonl(v);}

struct PktAgent:public CmAgent {
public:
enum eRequest {
	eConnect_	=0x0100,	// result=pid
	eClose_		=0x0200,	// result=code
	eWrite_		=0x0001,	// write packet to bpf:with data packet
	eRead_		=0x0002,	// request for read packet
					// response's payload is the packet
	eCapture_	=0x0003,	// start capture request to pktbuf
	eStop_		=0x0004,	// stop capture request to pktbuf
	eWrap_		=0x0005,	// request to use wrap buffer to pktbuf
	eNowrap_	=0x0006,	// request to stop capture 
					// when it's end of buffer
	eClear_		=0x0007,	// request to set pointer to the top of buffer
	eDump_		=0x0008,	// read request without changing the pointer
	eData_		=0x0009,	// data indication for dump request
	eStatus_	=0x000a,	// asking current status of pktbuf process
	eResponse_	=0x8000,	// Response indicator
	eMask_		=0x7fff};	// Request Mask
private:
	CmSocket *socket_;
public:
	PktAgent(CmSocket* =0);
virtual	~PktAgent();
virtual	int fileDesc() const;
	CmSocket* socket() const;
	int send(CSTR,int32_t);
virtual	int sendPacket(tcpHead&,int32_t =0,CSTR =0);
	void vsend(va_list);
	u_long hash() const;
	bool isEqual(const PktAgent* o) const;
	int compare(const PktAgent* o) const;
virtual	int terminate();	// it might delete self !!
static	CSTR unixDomainName();
static	CmSocket* unixDomain();
};
inline CmSocket* PktAgent::socket() const {return socket_;}
inline int PktAgent::fileDesc() const {return socket_->fileDesc();}
inline u_long PktAgent::hash() const {return socket_->hash();}
inline bool PktAgent::isEqual(const PktAgent* o) const {
	return socket_->isEqual(o->socket());}
inline int PktAgent::compare(const PktAgent* o) const {
	return socket_->compare(o->socket());}
interfaceCmSet(PktAgentSet,PktAgent);
#endif

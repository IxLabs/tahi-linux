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
 * $TAHI: v6eval/lib/pkt/PktRecv.h,v 1.5 2001/10/05 11:41:20 tanaka Exp $
 */
#if !defined(__PktRecv_h__)
#define __PktRecv_h__	1
#include "PktAgent.h"
struct PktReceiver:public PktAgent {
private:
	tcpHead header_;
	int32_t allocated_;
	STR buffer_;
//----------------------------------------------------------------------
// member function of receiving Request
public: 
virtual	int connectRequest();
virtual	int closeRequest();
virtual	int writeRequest();
virtual	int readRequest();
virtual	int captureRequest();
virtual	int stopRequest();
virtual	int wrapRequest();
virtual	int clearRequest();
virtual	int nowrapRequest();
virtual	int dumpRequest();
virtual	int dataRequest();
virtual	int statusRequest(); 
//----------------------------------------------------------------------
// member function of receiving Confirmation
public:
virtual	int connectConfirmation();
virtual	int closeConfirmation();
virtual	int writeConfirmation();
virtual	int readConfirmation();
virtual	int captureConfirmation();
virtual	int stopConfirmation();
virtual	int wrapConfirmation();
virtual	int nowrapConfirmation();
virtual	int clearConfirmation();
virtual	int dumpConfirmation();
virtual	int dataConfirmation();
virtual	int statusConfirmation();
//----------------------------------------------------------------------
virtual	int dataIndication();
virtual	int dataResponse();
//----------------------------------------------------------------------
public:
	int receivePacket(int);
	PktReceiver(CmSocket* =0);
virtual	~PktReceiver();
	void receiver();
protected:
	int receiveBuffer(STR,int32_t);
virtual	int receiveHeader();
virtual	int receiveBody();
	int request() const;
	int result() const;
	STR buffer(int32_t =0);
	int bodyLength() const;
	const tcpHead& header() const;
	int response(int32_t,int32_t =0,CSTR =0);
	int analizeRequest(int32_t);
	int analizeConfirmation(int32_t);
	int analizePacket();
};
inline void PktReceiver::receiver() {
	readAction((agentFunc)&PktReceiver::receivePacket);}
inline const tcpHead& PktReceiver::header() const {return header_;}
inline int PktReceiver::request() const {return header_.request();}
inline int PktReceiver::bodyLength() const {return header_.length();}
inline int PktReceiver::result() const {return header_.result();}
#endif

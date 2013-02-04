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
 * $TAHI: v6eval/lib/pkt/PktServer.h,v 1.10 2001/10/05 11:41:20 tanaka Exp $
 */
#if !defined(__PktServer_h__)
#define	__PktServer_h__	1
#include "pktDefine.h"
#include "PktRecv.h"
#include "BpfAgent.h"
#include "bufStat.h"
#include "PktBuf.h"

struct PktServer;
struct PktBuf;
struct PktAccept:public PktAgent {
private:
	PktAgentSet* accepts_;
	PktBuf *bpf_;
public:
	PktAccept(PktBuf *);
virtual	~PktAccept();
	int listen();
virtual	int accept();
	void addAgent(PktAgent*);
	void removeAgent(PktAgent*);
virtual	int terminate();
//----------------------------------------------------------------------
protected:
};

struct PktServer:public PktReceiver {
	PktAccept* accepter_;
	BpfAgent *bpfbuf_;
	PktBuf *pktbuf_;
public:
	PktServer(CmSocket*,PktAccept* a,PktBuf *);
virtual	~PktServer();
virtual	int terminate();	// it might delete self !!
//----------------------------------------------------------------------
// member function of receiving Request
public: 
	PktAccept* accepter() const;
virtual	int connectRequest();
virtual	int writeRequest();
virtual	int readRequest();
	int readResponse(char *,int32_t);
	int timeout(time_t ,uint32_t);
virtual	int captureRequest();
virtual	int stopRequest();
virtual	int wrapRequest();
virtual	int nowrapRequest();
virtual	int clearRequest();
virtual	int dumpRequest();
virtual	int statusRequest(); 
//----------------------------------------------------------------------
// member function of receiving Confirmation
public:
//virtual	int dataConfirmation();
//----------------------------------------------------------------------
protected:
};
inline PktAccept* PktServer::accepter() const {return accepter_;}
#endif

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
 * $TAHI: v6eval/lib/Cm/CmSocket.cc,v 1.29 2001/10/29 01:41:33 akisada Exp $
 */
#include "CmSocket.h"
//IMPLEMENTATION
#if defined(__FreeBSD__)
#define	setSALEN(s,l)s->sa_len=l
#else
#define	setSALEN(s,l)
#endif

#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <rpc/types.h>
#include <net/if.h>
//---PURE VIRTUAL FUNCTIONS---------------------------------------------
int CmSocket::socket() {return -1;}
int CmSocket::receive(STR ,int ) {return -1;}
int CmSocket::send(CSTR ,int ) {return -1;}
int CmSocket::sendAll(CSTR s,int l) {return send(s,l);}
int CmSocket::recvAll(STR s,int l) {return send(s,l);}
CmStream::CmStream(int l,const sock* s,int fd):CmSocket(l,s,fd),
	self_(0),peer_(0) {}
CmStream::CmStream(const inSock& in,int fd):
	CmSocket(sizeof(in),reinterpret_cast<const sock*>(&in),fd),
	self_(0),peer_(0) {}
CmStream::CmStream(const in6Sock& in,int fd):
	CmSocket(sizeof(in),reinterpret_cast<const sock*>(&in),fd),
	self_(0),peer_(0) {}
CmDgram::CmDgram(int l,const sock* s,int fd):CmSocket(l,s,fd) {}
CmDgram::CmDgram(const inSock& in,int fd):
	CmSocket(sizeof(in),reinterpret_cast<const sock*>(&in),fd) {}
CmDgram::CmDgram(const in6Sock& in,int fd):
	CmSocket(sizeof(in),reinterpret_cast<const sock*>(&in),fd) {}
CmRaw::CmRaw(int l,const sock* s,int fd):CmDgram(l,s,fd) {}
int CmStream::socket()	{return socketType(SOCK_STREAM);}
int CmDgram::socket()	{return socketType(SOCK_DGRAM);}
int CmRaw::socket()	{return socketType(SOCK_RAW);}

//---ATTRIBUTE MANUPULATION---------------------------------------------
u_short CmSockAddr::family() const {
	sock* s=sockAddr();
	return s?s->sa_family:0;}

bool CmSockAddr::family(u_short u) {
	sock* s=sockAddr();
	if(s) s->sa_family=u;
	return this;}

u_short CmSockAddr::service() const {
	u_short serv=0;
	u_short f=family();
	switch(f) {
	case AF_INET: {
		inSock* in=inetAddr();
		if(in) serv=ntohs(in->sin_port);}
		break;
	case AF_INET6: {
		in6Sock* in6=inet6Addr();
		if(in6) serv=ntohs(in6->sin6_port);}
		break;}
	return serv;}

bool CmSockAddr::service(u_short u) {
	u_short f=family();
	switch(f) {
	case AF_INET: {
		inSock* in=inetAddr();
		if(in!=0) {in->sin_port=htons(u);}}
		break;
	case AF_INET6: {
		in6Sock* in6=inet6Addr();
		if(in6!=0) {in6->sin6_port=htons(u);}}
		break;}
	return this;}

const in_addr CmSockAddr::address() const {
	in_addr anAddr={0};
	inSock* in=inetAddr();
	if(in!=0) {
		anAddr=in->sin_addr;}
	return anAddr;}

bool CmSockAddr::address(const in_addr& anAddr) {
	inSock* in=inetAddr();
	if(in!=0) {
		in->sin_addr=anAddr;}
	return this;}

inSock* CmSockAddr::inetAddr() const {
	u_short f=family();
	return f==AF_INET?reinterpret_cast<inSock*>(addr_):0;}
in6Sock* CmSockAddr::inet6Addr() const {
	u_short f=family();
	return f==AF_INET6?reinterpret_cast<in6Sock*>(addr_):0;}
unSock* CmSockAddr::unixAddr() const {
	u_short f=family();
	return f==AF_UNIX?reinterpret_cast<unSock*>(addr_):0;}

//---Dictionary Interface-----------------------------------------------
uint32_t CmSockAddr::hash() const {
	return basicHash((CSTR)sockAddr(),length());}

bool CmSockAddr::isEqual(const CmSockAddr* o) const {
	int l=length();
	int ol=o->length();
	if(l!=ol) return false;
	u_short f=family();
	switch(f) {
	case AF_UNIX: case AF_INET: {
		return memcmp(sockAddr(),o->sockAddr(),l)==0;}
		break;
	case AF_INET6: {
		if(memcmp(sockAddr(),o->sockAddr(),l)==0) {return true;}
		in6Sock* mi=inet6Addr();
		in6_addr* m6=&mi->sin6_addr;
		if(!IN6_IS_ADDR_LINKLOCAL(m6)) {return false;}
		in6Sock* oi=o->inet6Addr();
		if(oi==0) {return false;}
		if(	mi->sin6_family!=oi->sin6_family ||
			mi->sin6_port!=oi->sin6_port ||
			mi->sin6_flowinfo!=oi->sin6_flowinfo) {return false;}
		in6_addr* o6=&oi->sin6_addr;
		in6_addr wm6=*m6, wo6=*o6;
		SET_IN6_LINKLOCAL_IFINDEX(wm6,0); /* XXX */
		SET_IN6_LINKLOCAL_IFINDEX(wo6,0); /* XXX */
		return IN6_ARE_ADDR_EQUAL(&wm6,&wo6);}
		break;}
	return false;}

int CmSockAddr::compare(const CmSockAddr*) const {return 0;}

//---ERROR HANDLING-----------------------------------------------------
bool CmSocket::retryError() const {
	int e=syserrno();
	return (e==EINTR||e==ENOBUFS);}
int CmSocket::sysError(CSTR aFunc) {
	syserrno_=errno;
	if(!retryError()) {eerr(aFunc);}
	return -1;}

//---Instance Constructin/Destruction-----------------------------------
CmSockAddr::CmSockAddr(int l,const sock* s):alloc_(0),length_(0),addr_(0) {
	socketArea(l,s);}
CmSockAddr::~CmSockAddr() {
	free(addr_); addr_=0; alloc_=0; length_=0;}

int CmSockAddr::initSocket(sock* s,size_t l,int f) {
	memset(s, 0, l);
	setSALEN(s,l);
	s->sa_family=f;
	return l;}

CmSockAddr* CmSockAddr::socketArea(int l,const sock* s) {
	if(alloc_<l) {
		if(addr_) {free(addr_),addr_=0,alloc_=0;}
		addr_=(sock*)malloc(alloc_=l);}
	if(l>0&&s!=0) {
		memcpy(addr_,s,length_=l);}
	return this;}

CmSocket::CmSocket(int l,const sock* s,int fd):CmSockAddr(l,s),fileDesc_(fd) {}
CmSocket::~CmSocket() {
	close();}

//---Socket Creation----------------------------------------------------
int CmSocket::socketType(int aType,int aProtocol) {
	int fd=myFD();
	if(fd>=0) return fd;
	int domain=family();
	if((fd=::socket(domain,aType,aProtocol))<0) {
		sysError("socket");}
	fileDesc(fd);
	return fd;}

//---PURE VIRTUAL ACCEPT------------------------------------------------
CmSocket* CmSocket::create(int,const sock*,int) {return 0;}
CmSocket* CmStream::create(int l,const sock* s,int fd) {
	return new CmStream(l,s,fd);}
CmSocket* CmDgram::create(int l,const sock* s,int fd) {
	return new CmDgram(l,s,fd);}
CmSocket* CmRaw::create(int l,const sock* s,int fd) {
	return new CmRaw(l,s,fd);}

CmSocket* CmSocket::accept(int,const sock*,int) {return 0;}
CmSocket* CmStream::accept(int l,const sock* s,int fd) {
	CmStream* from=(CmStream*)create(l,s,fd);
	if(from!=0) {
		from->self(this); from->peer(from);}
	return from;}

//---BASIC OPERATION----------------------------------------------------
CmSocket* CmSocket::close() {
	if(fileDesc_>=0) {
		::close(fileDesc_),fileDesc_=-1;
		SocketSet* set=bindedSockets();
		CmSocket* f=set->find(this);
		if(f!=0) {clearBind();}
		set->remove(this);}
	return this;}

CmSocket* CmSocket::accept() {
	int fd;
	char buf[BUFSIZ];
	socklen_t len=BUFSIZ;
	sock* p=(sock*)buf;
	if((fd=::accept(myFD(),p,&len))<0) {
		sysError("accept");
		return 0;}
	return accept(len,p,fd);}

int CmSocket::bind(bool reuse) {
	int fd=socket();
	if(reuse) setReuseAddrOpt();
	if(fd>=0&&(::bind(fd,sockAddr(),length()))<0) {
		return sysError("bind");}
	SocketSet* set=bindedSockets();
	set->add(this);
	return fd;}

int CmSocket::listen(int aBacklog) {
	int fd=myFD();
	if(fd<0) fd=bind();
	if(fd>=0&&(fd=::listen(fd,aBacklog))<0) {
		sysError("listen");}
	return fd;}

int CmSocket::connect(CmSockAddr* to) {
	if(to==0) to=this;
	int fd=socket();
	if(fd>=0&&::connect(fd,to->sockAddr(),to->length())<0) {
		return sysError("connect");}
	return fd;}

int CmStream::connect(CmSockAddr* to) {
	CmSockAddr* cn=to!=0?to:this;
	peer(cn);
	int fd=CmSocket::connect(cn);
	if(fd>=0) {self(to!=0?(CmSockAddr*)this:getsockname());}
	return fd;}

int CmSocket::recvfrom(STR s,int l,CmSockAddr*) {return receive(s,l);}
int CmSocket::sendto(CSTR s,int l,CmSockAddr*) {return send(s,l);}
//---OPTIONAL OPERATIONS------------------------------------------------
CmSockAddr* CmSocket::getsockname() {
	int fd=myFD();
	char buf[BUFSIZ];
	socklen_t len=BUFSIZ;
	sock* p=(sock*)buf;
	if(fd>=0&&::getsockname(fd,p,&len)<0) {
		sysError("getsockname");
		return 0;}
	return new CmSockAddr(len,p);}

CmSockAddr* CmSocket::getpeername() {
	int fd;
	char buf[BUFSIZ];
	socklen_t len=BUFSIZ;
	sock* p=(sock*)buf;
	if((fd=::getpeername(myFD(),p,&len))<0) {
		sysError("getpeername");
		return 0;}
	return new CmSockAddr(len,p);}

bool CmSocket::nonBlocking(int) {
	int fd=myFD();
	if(fd>=0&&::fcntl(fd,F_SETFL,O_NDELAY)<0) {
		sysError("fcntl"); return false;}
	return (fd>=0);}

inSock* CmSocket::broadcast(struct ifreq* p) {
	int s=myFD();
	if(s<0) return 0;
	if(ioctl(s,SIOCGIFFLAGS,(STR)p)<0) {
		eoutf("ioctl(get interface flags)\n");
		return 0;}
	if(!(p->ifr_flags&IFF_BROADCAST)) {return 0;}
	if(ioctl(s,SIOCGIFBRDADDR,(STR)p)<0) {
		eoutf("ioctl(get broadaddr)\n");
		return 0;}
	return reinterpret_cast<inSock*>(&p->ifr_broadaddr);}

int CmSocket::broadcasts(inSock** b) {
	char buf[BUFSIZ];
	struct ifconf ifc;
	int s=myFD();
	if(s<0) return 0;
	ifc.ifc_len=sizeof(buf); ifc.ifc_buf=buf;
	if(ioctl(s,SIOCGIFCONF,(STR)&ifc)<0) {
		eoutf("ioctl(get interface configuration)\n");
		return 0;}
	struct ifreq* rp,* re;
	int n,m=ifc.ifc_len/sizeof(struct ifreq);
	inSock* a=new inSock[m],* r=a;
	for(rp=ifc.ifc_req,re=rp+m;rp<re;rp++) {
		inSock* in=broadcast(rp);
		if(in) {*r=*in; r++;}}
	if((n=r-a)>0) {*b=a; return n;}
	delete [] a;
	return 0;}

bool CmSocket::setReuseAddrOpt(bool bl) {
	int on=bl?1:0;
	if(setsockopt(myFD(),SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))<0) {
		sysError("setsockopt(SO_REUSEADDR)"); return false;}
	return true;}
bool CmSocket::setReusePortOpt(bool bl) {
	int on=bl?1:0;
	if(setsockopt(myFD(),SOL_SOCKET,SO_REUSEPORT,&on,sizeof(on))<0) {
		sysError("setsockopt(SO_REUSEPORT)"); return false;}
	return true;}
bool CmSocket::setDebugOpt(bool bl) {
	int on=bl?1:0;
	if(setsockopt(myFD(),SOL_SOCKET,SO_DEBUG,&on,sizeof(on))<0) {
		sysError("setsockopt(SO_DEBUG)"); return false;}
	return true;}

//---send/receiv--------------------------------------------------------
int CmStream::send(CSTR s,int l) {
	int fd=myFD();
	int rc=::write(fd,s,l);
	if(rc<0) {sysError("write");}
	return rc;}
int CmStream::sendAll(CSTR s,int len) {
	int32_t rem, l; CSTR p;
	for(rem=len,p=s;rem>0;rem-=l,p+=l) {
		l=send(p,rem);
		if(l<0 && !retryError()) {l=0; continue;}
		if(l<=0) return -1;}
	return len;}

int CmStream::receive(STR s,int l) {
	int fd=myFD();
	int rc=::read(fd,s,l);
	if(rc<0) {sysError("read");}
	return rc;}
int CmStream::recvAll(STR s,int len) {
	int32_t rem, l; STR p;
	for(rem=len,p=s;rem>0;rem-=l,p+=l) {
		l=receive(p,rem);
		if(l<0 && !retryError()) {l=0; continue;}
		if(l<=0) return -1;}
	return len;}

int CmDgram::send(CSTR s,int l) {
	int fd=myFD();
	int rc=::send(fd,s,l,0);
	if(rc<0) {sysError("send");}
	return rc;}
int CmDgram::receive(STR s,int l) {
	int fd=myFD();
	int rc=::recv(fd,s,l,0);
	if(rc<0) {sysError("recv");}
	return rc;}
int CmDgram::sendto(CSTR s,int l,CmSockAddr* to)	{
	int fd=myFD();
	int rc=::sendto(fd,s,l,0,to->sockAddr(),to->length());
	if(rc<0) {sysError("sendto");}
	return rc;}
int CmDgram::recvfrom(STR s,int l,CmSockAddr* from) {
	char buf[BUFSIZ];
	socklen_t len=BUFSIZ;
	sock* p=(sock*)buf;
	int fd=myFD();
	int rc=::recvfrom(fd,s,l,0,p,&len);
	if(rc<0) {sysError("recvfrom");}
	else	 {from->socketArea(len,(sock*)p);}
	return rc;}
int CmDgram::sendfrom(CSTR s,int l,CmSocket* from) {
	int fd=from->fileDesc();
	int rc=::sendto(fd,s,l,0,sockAddr(),length());
	if(rc<0) {sysError("sendto");}
	return rc;}

//---Debug Print--------------------------------------------------------
void CmSockAddr::print() const {
	u_short f=family();
	ooutf("len=%d family=%d ",length(),f);
	switch(f) {
	case AF_UNIX: {
		unSock* u=unixAddr();
		ooutf("AF_UNIX %s\n",u->sun_path);}
		break;
	case AF_INET: {
		inSock* i=inetAddr();
		ooutf("AF_INET port=%05d %s\n",
			ntohs(i->sin_port),inet_ntoa(i->sin_addr));}
		break;
	case AF_INET6: {
		char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"];
		in6Sock* i=inet6Addr();
		inet_ntop(AF_INET6,&i->sin6_addr,tmp,sizeof(tmp));
		ooutf("AF_INET6 port=%05d %s %d\n",
			ntohs(i->sin6_port),tmp,i->sin6_scope_id);}
		break;}}

void CmStream::print() const {
	ooutf("CmStream(fd=%2d) ",myFD());
	CmSockAddr::print();}
void CmDgram::print() const {
	ooutf("CmDgram(fd=%2d) ",myFD());
	CmSockAddr::print();}
void CmRaw::print() const {
	ooutf("CmRaw(fd=%2d) ",myFD());
	CmSockAddr::print();}

void CmSocket::printPeers() const {print();}
void CmStream::printPeers() const {
	ooutf("fd=%2d\n",myFD());
	CmSockAddr* s=self();
	CmSockAddr* p=peer();
	if(s!=0) {ooutf("\tself="); s->print();}
	if(p!=0) {ooutf("\tpeer="); p->print();}}

//---AF_UNIX Socket Creation--------------------------------------------
int UnixSocket::initSocket(unSock& un) {
	sock* s=reinterpret_cast<sock*>(&un);
	return CmSocket::initSocket(s,sizeof(un),AF_UNIX);}
int UnixSocket::service(unSock& un,CSTR serv) {
	initSocket(un);
	size_t l=strlen(serv);
	if(l>=sizeof(un.sun_path)) {
		eoutf("serv name \'%s\' too big %d\n",serv,l);}
	strncpy(un.sun_path,serv,sizeof(un.sun_path));
	un.sun_path[sizeof(un.sun_path)-1]='\0';
	return un.sun_path-(STR)&un+strlen(un.sun_path)+1;}

CmSocket* UnixSocket::stream() 	{unSock un;
	sock* s=reinterpret_cast<sock*>(&un);
	return new CmStream(initSocket(un),s);}
CmSocket* UnixSocket::stream(CSTR serv) {unSock un;
	sock* s=reinterpret_cast<sock*>(&un);
	return new CmStream(service(un,serv),s);}
CmDgram* UnixSocket::datagram() 	{unSock un;
	sock* s=reinterpret_cast<sock*>(&un);
	return new CmDgram(initSocket(un),s);}
CmDgram* UnixSocket::datagram(CSTR serv) {unSock un;
	sock* s=reinterpret_cast<sock*>(&un);
	return new CmDgram(service(un,serv),s);}

//---AF_INET Socket Creation--------------------------------------------
int InetSocket::initSocket(inSock& in) {
	sock* s=reinterpret_cast<sock*>(&in);
	return CmSocket::initSocket(s,sizeof(in),AF_INET);}
int InetSocket::initSocket(in6Sock& in) {
	sock* s=reinterpret_cast<sock*>(&in);
	return CmSocket::initSocket(s,sizeof(in),AF_INET6);}

int InetSocket::service(inSock& in,int serv) {
	initSocket(in);
	in.sin_port=htons(serv);
	return sizeof(in);}
int InetSocket::service(in6Sock& in,int serv) {
	initSocket(in);
	in.sin6_port=htons(serv);
	return sizeof(in);}

int InetSocket::serviceAddress(inSock& in,int serv,const in_addr& anAddr) {
	int len=service(in,serv);
	in.sin_addr=anAddr;
	return len;}
int InetSocket::serviceAddress(in6Sock& in,int serv,const in6_addr& anAddr) {
	int len=service(in,serv);
	in.sin6_addr=anAddr;
	if(IN6_IS_ADDR_LINKLOCAL(&anAddr)) {
		int scope=IN6_LINKLOCAL_IFINDEX(anAddr);
		if(scope!=0) {
			in.sin6_scope_id=scope;
			SET_IN6_LINKLOCAL_IFINDEX(in.sin6_addr,0);}}
	return len;}

//----------------------------------------------------------------------
// HOST DEPEND SOCKET
inSock* InetSocket::serviceHost4(int serv,CSTR aHost,int& l) {
	l=-1;
	struct hostent* h=gethostbyname(aHost);
	if(h==0) {return 0;}
	static inSock in;
	in_addr addr;
	memcpy(&addr,h->h_addr,h->h_length);
	l=serviceAddress(in,serv,addr);
	return &in;}

in6Sock* InetSocket::serviceHost6(int serv,CSTR aHost,int& l) {
	l=-1;
	int err=0;
	struct hostent* h=getipnodebyname(aHost,AF_INET6,0,&err);
	if(h==0) {return 0;}
	static in6Sock in6;
	in6_addr addr;
	memcpy(&addr,h->h_addr,h->h_length);
	l=serviceAddress(in6,serv,addr);
	return &in6;}

sock* InetSocket::serviceHost(int serv,CSTR aHost,int& l) {
	l=-1;
	sock* in=reinterpret_cast<sock*>(serviceHost6(serv,aHost,l));
	if(in==0) {in=reinterpret_cast<sock*>(serviceHost4(serv,aHost,l));}
	if(in==0) {eoutf("unknown hostname %s\n",aHost);}
	return in;}

CmSocket* InetSocket::stream(int serv,CSTR aHost) {
	int l=0;
	sock* in=serviceHost(serv,aHost,l);
	return (in!=0)?(new CmStream(l,in)):0;}
CmDgram* InetSocket::datagram(int serv,CSTR aHost) {
	int l=0;
	sock* in=serviceHost(serv,aHost,l);
	return (in!=0)?(new CmDgram(l,in)):0;}

//----------------------------------------------------------------------
// INET DATABASE SERVICE
int InetSocket::getService(CSTR serv) {
	struct  servent* s=getservbyname(serv,0);
	return (s)?ntohl(s->s_port):0;}
int InetSocket::defaultService(CSTR serv,CSTR debug,int def) {
	STR e=getenv(debug);
	int p=e?atoi(e):0;
	p=p?p:getService(serv);
	return p?p:def;}

CSTR InetSocket::myHostname() {
	static STR myHostNamePtr=0;
	static char myHostNameArea[MAXHOSTNAMELEN];
	if(myHostNamePtr) return myHostNamePtr;
	if(gethostname(myHostNameArea,MAXHOSTNAMELEN)<0) {
		eoutf("cannot gethostname\n");
		strcpy(myHostNameArea,"localhost");}
	return myHostNamePtr=myHostNameArea;}

const in_addr InetSocket::myIpAddress() {
	static in_addr myAddress={0};
	struct hostent* h;
	if(myAddress.s_addr) return myAddress;
	CSTR s=myHostname();
	if((h=gethostbyname(s))!=0) {
		memcpy(&myAddress,h->h_addr,h->h_length);}
	else {
		eoutf("unknown hostname %s\n",s);
		myAddress.s_addr=INADDR_LOOPBACK;}
	return myAddress;}

//----------------------------------------------------------------------
CmSocket* InetSocket::stream() {inSock in;
	sock* s=reinterpret_cast<sock*>(&in);
	return new CmStream(initSocket(in),s);}
CmSocket* InetSocket::stream6() {in6Sock in;
	sock* s=reinterpret_cast<sock*>(&in);
	return new CmStream(initSocket(in),s);}
CmDgram* InetSocket::datagram() {inSock in;
	sock* s=reinterpret_cast<sock*>(&in);
	return new CmDgram(initSocket(in),s);}
CmDgram* InetSocket::datagram6() {in6Sock in;
	sock* s=reinterpret_cast<sock*>(&in);
	return new CmDgram(initSocket(in),s);}

//----------------------------------------------------------------------
CmSocket* InetSocket::stream(int serv) {inSock in;
	sock* s=reinterpret_cast<sock*>(&in);
	return new CmStream(service(in,serv),s);}
CmSocket* InetSocket::stream6(int serv) {in6Sock in;
	sock* s=reinterpret_cast<sock*>(&in);
	return new CmStream(service(in,serv),s);}
CmDgram* InetSocket::datagram(int serv) {inSock in;
	sock* s=reinterpret_cast<sock*>(&in);
	return new CmDgram(service(in,serv),s);}
CmDgram* InetSocket::datagram6(int serv) {in6Sock in;
	sock* s=reinterpret_cast<sock*>(&in);
	return new CmDgram(service(in,serv),s);}

//----------------------------------------------------------------------
CmSocket* InetSocket::stream(int serv,const in_addr& anAddr) {inSock in;
	sock* s=reinterpret_cast<sock*>(&in);
	return new CmStream(serviceAddress(in,serv,anAddr),s);}
CmDgram* InetSocket::datagram(int serv,const in_addr& anAddr) {inSock in;
	sock* s=reinterpret_cast<sock*>(&in);
	return new CmDgram(serviceAddress(in,serv,anAddr),s);}
CmSocket* InetSocket::stream(int serv,const in6_addr& anAddr) {in6Sock in;
	sock* s=reinterpret_cast<sock*>(&in);
	return new CmStream(serviceAddress(in,serv,anAddr),s);}
CmDgram* InetSocket::datagram(int serv,const in6_addr& anAddr) {in6Sock in;
	sock* s=reinterpret_cast<sock*>(&in);
	return new CmDgram(serviceAddress(in,serv,anAddr),s);}

//---Binded Socket Control----------------------------------------------
SocketSet* CmSocket::bindedSockets() {
	if(bindedSockets_==0) {
		bindedSockets_=new SocketSet;
		atexit(&CmSocket::clearBinds);}
	return bindedSockets_;}

void CmSocket::clearBind() {
	unSock* u=unixAddr();
	if(u!=0) {::unlink(u->sun_path);}}

void CmSocket::clearBinds() {
	SocketSet* set=bindedSockets();
	set->elementsPerform((CmSocketFunc)&CmSocket::clearBind);}

SocketSet* CmSocket::bindedSockets_=0;
implementCmSet(SocketSet,CmSocket);

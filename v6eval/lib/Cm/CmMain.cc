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
 * $TAHI: v6eval/lib/Cm/CmMain.cc,v 1.19 2009/08/27 00:10:04 akisada Exp $
 */
#include "CmMain.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <utmp.h>
#include <time.h>
#include <pwd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/param.h>
extern int causeStopSignals[];
static int stopSingals[]={SIGINT,SIGQUIT,SIGTERM,0};
extern void applicationMain(CmMain*);

//----------------------------------------------------------------------
// シグナルは捕捉処理
static void sigCatch(int sig,int code,struct sigcontext*,STR) {
static char fmt[]=
"%02d:%02d:%02d.%06d\tsigCatch(sig=%d,code=%d) pid=%d,application=%s\n";
	static struct timeval t[1]; struct timezone z[1]; struct tm *m;
	CmMain *cm=CmMain::instance();
	gettimeofday(t,z);
	m=localtime((const time_t *)&t[0].tv_sec);
	STR name=cm->applicationName();
	eoutf(fmt,m->tm_hour,m->tm_min,m->tm_sec,t[0].tv_usec,
		sig,code,getpid(),name?name:"");
	int *p, s;
	for(p=stopSingals;(s=*p);p++) {
		if(s==sig) {exit(sig);}}
	if(sig!=SIGIOT) {signal(sig,(exitHandler)sigCatch);}
	::abort();}

//----------------------------------------------------------------------
// 再起動処理
void CmMain::save(int argc) {
	STR *argv=orgArgs_, *p, *q, s;
	applicationName(argv[0]);
	saveArgs_=(STR*)new STR[argc+2];
	for(p=argv,q=saveArgs_;(s=*p);p++,q++) {*q=s;}
	*q=0;}
void CmMain::restart() {
	int i, max=getdtablesize();
	pid_t pid=vfork();
	if(pid< 0) {eoutf("cannot restart %s\n",command()); exit(1);}
	if(pid!=0) {exit(0);}
	SETPGRP(0,0);
	for(i=3;i<max;i++) close(i);
	execve(command(),saveArgs(),saveEnvp());
	eoutf("cannot execute %s\n",command());
	exit(1);}

//----------------------------------------------------------------------
// プログラムメイン
int main(int argc,STR *argv,STR *envp) {
	int *p, s;
	for(p=causeStopSignals;(s=*p);p++) {signal(s,(exitHandler)sigCatch);}
	signal(SIGPIPE,SIG_IGN);
	setvbuf(stdout,(STR)NULL,_IOLBF,0);
	CmMain* cm=new CmMain(argc,argv,envp);
	applicationMain(cm);
	exit(0);
	return 0;}

//----------------------------------------------------------------------
// システム情報
const STR CmMain::myHostname() {
	static STR myHostNamePtr=0;
	static char myHostNameArea[MAXHOSTNAMELEN];
	if(myHostNamePtr) return myHostNamePtr;
	if(gethostname(myHostNameArea,MAXHOSTNAMELEN)<0) {
		eoutf("cannot gethostname\n");
		strcpy(myHostNameArea,"localhost");}
	return myHostNamePtr=myHostNameArea;}

const int CmMain::getdtablesize() {
	struct rlimit rlp;
	getrlimit(RLIMIT_NOFILE,&rlp);
	return rlp.rlim_cur;}

void CmMain::setDbgFlags(CSTR s) {
	if(s) {for(;*s;++s) {DBGFLAGS(*s&0xff)++;}}}

//----------------------------------------------------------------------
// 障害解析情報作成
static struct utmp *myUtmpEnt(FILE *in,struct utmp *u) {
	int s=ttyslot();
	if(s<0||fseek(in,sizeof(struct utmp)*s,0)<0||
		fread(u,sizeof(struct utmp),1,in)==0) {return 0;}
	return u;}
void CmMain::makeCatch2Eye(STR p) {
static char catch2[]=" on %*.*s:%-*.*s from %*.*s";
	struct utmp ux[1], *u; FILE *in;
	if((in=fopen("/etc/utmp","r"))==NULL) {return;}
	u=myUtmpEnt(in,ux); fclose(in);
	if(!u) {return;}
#define A(a)sizeof(a),sizeof(a),a
	sprintf(p,catch2,A(u->ut_line),A(u->ut_name),A(u->ut_host));
#undef A
	return;}
void CmMain::makeCatchEye(const STR pgmName) {
static char catch1[]="@(#) %02d/%02d/%02d %02d:%02d:%02d %s STARTED by %s@%s";
	time_t now=time(0); struct tm *m=localtime(&now);
	struct passwd *pw=getpwuid(getuid());
	CSTR p=pw?pw->pw_name:"unknown";
	sprintf(catchStart_,catch1,m->tm_year,m->tm_mon+1,m->tm_mday,
		m->tm_hour,m->tm_min,m->tm_sec,pgmName,p,myHostname());
	makeCatch2Eye(catchStart_+strlen(catchStart_));}

//----------------------------------------------------------------------
// 起動時情報
STR CmMain::applicationName(STR pgm) {
	STR s;
	if(applicationName_) return applicationName_;
	makeCatchEye(applicationName_=(s=strrchr(pgm,'/'))?s+1:pgm);
	return applicationName_;}

//----------------------------------------------------------------------
// 生成/消滅
CmMain::CmMain(int argc,STR *argv,STR *envp):
	argCount_(argc),orgArgs_(argv),saveEnvp_(envp) {
	save(argc);
	instance_=this;}

CmMain::~CmMain() {
	delete [] saveArgs_;}

//----------------------------------------------------------------------
// プロセスを停止するシグナル一覧
#if defined(__hpux)
int causeStopSignals[]={
	  SIGHUP   ,    SIGINT   ,    SIGQUIT  ,    SIGILL   ,
	  SIGTRAP  ,    SIGABRT  ,    SIGEMT   ,    SIGFPE   ,
	/*SIGKILL  ,*/  SIGBUS   ,    SIGSEGV  ,    SIGSYS   ,
	  SIGPIPE  ,    SIGALRM  ,    SIGTERM  ,    SIGUSR1   ,
	  SIGUSR2  ,  /*SIGCHLD  ,*//*SIGPWR   ,*/  SIGVTALRM ,
	  SIGPROF  ,  /*SIGIO    ,*//*SIGWINCH ,*//*SIGSTOP   ,*/
	/*SIGTSTP  ,*//*SIGCONT  ,*//*SIGTTIN  ,*//*SIGTTOU   ,*/
	/*SIGURG   ,*/  SIGLOST  ,  
	  0};
#elif defined(sparc)
#	if !defined(solaris)
int causeStopSignals[]={
	  SIGHUP   ,    SIGINT   ,    SIGQUIT  ,    SIGILL   ,
	  SIGTRAP  ,    SIGABRT  ,    SIGEMT   ,    SIGFPE   ,
	/*SIGKILL  ,*/  SIGBUS   ,    SIGSEGV  ,    SIGSYS   ,
	  SIGPIPE  ,    SIGALRM  ,    SIGTERM  ,  /*SIGURG   ,*/
	/*SIGSTOP  ,*//*SIGTSTP  ,*//*SIGCONT  ,*//*SIGCHLD  ,*/
	/*SIGTTIN  ,*//*SIGTTOU  ,*//*SIGIO    ,*/  SIGXCPU  ,
	  SIGXFSZ  ,    SIGVTALRM,    SIGPROF  ,  /*SIGWINCH ,*/
	/*SIGLOST  ,*/  SIGUSR1  ,    SIGUSR2  ,
	  0};
#	else
int causeStopSignals[]={
	  SIGHUP   ,    SIGINT   ,    SIGQUIT  ,    SIGILL   ,
	  SIGTRAP  ,    SIGABRT  ,    SIGEMT   ,    SIGFPE   ,
	/*SIGKILL  ,*/  SIGBUS   ,    SIGSEGV  ,    SIGSYS   ,
	  SIGPIPE  ,    SIGALRM  ,    SIGTERM  ,  /*SIGURG   ,*/
	/*SIGSTOP  ,*//*SIGTSTP  ,*//*SIGCONT  ,*//*SIGCHLD  ,*/
	/*SIGTTIN  ,*//*SIGTTOU  ,*//*SIGIO    ,*/  SIGXCPU  ,
	  SIGXFSZ  ,    SIGVTALRM,    SIGPROF  ,  /*SIGWINCH ,*/
	  SIGLOST  ,    SIGUSR1  ,    SIGUSR2  ,
	  0};
#	endif
#elif	defined(__H3050)
int causeStopSignals[]={
	  SIGHUP   ,    SIGINT   ,    SIGQUIT  ,    SIGILL    ,
	  SIGTRAP  ,    SIGABRT  ,    SIGEMT   ,    SIGFPE    ,
	/*SIGKILL  ,*/  SIGBUS   ,    SIGSEGV  ,    SIGSYS    ,
	  SIGPIPE  ,    SIGALRM  ,    SIGTERM  ,    SIGUSR1   ,
	  SIGUSR2  ,  /*SIGCHLD  ,*//*SIGPWR   ,*/  SIGVTALRM ,
	  SIGPROF  ,  /*SIGIO    ,*//*SIGWINDOW,*//*SIGSTOP   ,*/
	/*SIGTSTP  ,*//*SIGCONT  ,*//*SIGTTIN  ,*//*SIGTTOU   ,*/
	/*SIGURG   ,*/  SIGLOST  ,    SIGDIL   ,
	  0};
#else 
int causeStopSignals[]={0};
#endif
CmMain* CmMain::instance_=0;
STR CmMain::applicationName_;
char CmMain::catchStart_[256];
static char copyRight_[]=
"Copyright (C) 1999 Yokogawa Electric Corporation and\n"\
"                   Yokogawa Digital Computer Corporation.\n"\
"All rights reserved.\n";
char dbgFlags[256];
uint32_t logLevel=0;
bool DoHexDump=false;
CSTR __CmMain__funcNeverCalld() {return copyRight_;}

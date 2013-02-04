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
 * $TAHI: v6eval/lib/pkt/RunEnv.cc,v 1.43 2009/08/27 00:10:04 akisada Exp $
 */
#include "RunEnv.h"
#include "CmMain.h"
#include "CmLexer.h"
#include "PvOctets.h"
#include "timeval.h"
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/time.h>
#include <unistd.h>
IfName::IfName(CSTR n,CSTR i,CSTR e):CmCString(n),interface_(i),ether_(e) {}
IfName::~IfName() {}
IfName* IfName::create(CSTR n,CSTR i,CSTR m,CSTR file,uint32_t line) {
	if(n==0||i==0||m==0) {
		CmLexer::eouts(file,line,'E',"format error '%s %s %s'",
			n!=0?n:"",i!=0?i:"",m!=0?m:"");
		return 0;}
	bool ok=false;
	PvEther check(m,ok);
	if(!ok) {
		CmLexer::eouts(file,line,'E',
			"invalid ether address %s for interface %s",m,n);
		return 0;}
	return new IfName(n,i,m);}

CSTR RunEnv::searchPath(CSTR vpath, CSTR file,CmCString& spath) {
	spath=file;
	if(strchr(file,'/')) {return spath.string();}
	if(!vpath||!*vpath) {return spath.string();}
	char work[strlen(vpath)+1];
	strcpy(work,vpath);
	STR p,q;
	for(p=work,q=p;q;p=q+1) {
		static char buf[BUFSIZ];
		if((q=strchr(p,':'))) *q=0;
		sprintf(buf,"%s/%s",p,file);
		if(access(buf,R_OK)==0) {
			spath=buf; break;}}
	return spath.string();}

#define edup(a,b)
bool RunEnv::setString(CmCString*& def,CSTR s,CSTR name) {
	if(s==0&&name!=0) {
		fprintf(stderr,"err: %s is not specified\n",name);
		return false;}
	if(def==0) {def=new CmCString(s);}
	else {edup(def,s); *def=s;}
	return true;}
void RunEnv::socketpath(CSTR s)	{
	if(s==0) {
		fprintf(stderr,"err: socketpath is not specifed\n");
		return;}
	setString(socketpath_,s);}
void RunEnv::filterrule(CSTR s)	{
	if(s==0) {
		fprintf(stderr,"err: filter rule is not specifed\n");
		return;}
	setString(filterrule_,s);}
bool RunEnv::tnFile(CSTR s)	{return setString(tnFile_,s,"TN config");}
bool RunEnv::ifName(CSTR s)	{return setString(ifName_,s,"interface name");}
bool RunEnv::nutFile(CSTR s)	{return setString(nutFile_,s,"NUT config");}
bool RunEnv::pktDef(CSTR s)	{return setString(pktDef_,s,"packet definition file");}
bool RunEnv::seek(CSTR s)	{
	if(s==0) {
		fprintf(stderr,"err: seek not specified\n");
		return false;}
	sscanf(s,"%ld.%06ld",(long int *)&seek_.tv_sec,&seek_.tv_usec);
	return true;}
bool RunEnv::count(CSTR s)	{
	if(s==0) {
		fprintf(stderr,"err: count not specified\n");
		return false;}
	count_=atoi(s);
	return true;}
bool RunEnv::bufsize(CSTR s)	{
	if(s==0) {
		fprintf(stderr,"err: bufsize not specified\n");
		return false;}
	if(atoi(s)<1) {
		fprintf(stderr,"err: Can't set buffer size\n");
		return false;}
	bufsize_=atoi(s);
	return true;}
bool RunEnv::log(CSTR s)	{logLevel=(*s)?atoi(s):1; return true;}
bool RunEnv::dbg(CSTR s)	{CmMain::setDbgFlags(s); return true;}
bool RunEnv::hexdump()	{DoHexDump = true; return true;}
bool RunEnv::timeout(CSTR s) {
	s?sscanf(s,"%ld.%06ld",(long int *)&tvtimeout_.tv_sec,&tvtimeout_.tv_usec):0;
	settimeout_=1; return true;}
void RunEnv::calctvtimeout() {
	if(!settimeout_) return;
	if(seek_==EPOCHTIMEVAL){
		struct timezone tz;
		timeval now;
		gettimeofday(&now,&tz);
		tvtimeout_=tvtimeout_+now;
	} else tvtimeout_=tvtimeout_+seek_;}

const CSTR delm=" \t\n";
bool RunEnv::doNUTline(STR s,IfNameCreator func,CSTR f,uint32_t l) {
	if(*s=='#') return true;
	STR p=strtok(s,delm);
	if(p==0) return true;
	if(strncmp(p,defaultInterfaceName_,interfaceMatchLen)!=0) return true;
	STR ifn=strtok(0,delm);
	STR mac=ifn!=0?strtok(0,delm):0;
	IfName* n=(*func)(p,ifn,mac,f,l);
	if(n!=0) {nutSet_.add(n);}
	return (n!=0);}

bool RunEnv::doTNline(STR s,IfNameCreator func,CSTR f,uint32_t l) {
	if(*s=='#') return true;
	STR p=strtok(s,delm);
	if(p==0) return true;
	if(strcmp(p,"socketpath")==0) {socketpath(strtok(0,delm)); return true;}
	if(strcmp(p,"filter")==0) {filterrule(strtok(0,delm)); return true;}
	if(strncmp(p,defaultInterfaceName_,interfaceMatchLen)!=0) return true;
	STR ifn=strtok(0,delm);
	STR mac=ifn!=0?strtok(0,delm):0;
	IfName* n=(*func)(p,ifn,mac,f,l);
	if(n!=0) {tnSet_.add(n);}
	return (n!=0);}

FILE* RunEnv::configFile(CmString* p,CSTR s,CSTR n,CmCString& spath) {
	char name[MAXPATHLEN];
	CSTR path=0;
	if(p!=0) {path=p->string();}
	else {
		CSTR d=getenv(defaultCongigDirEnv_);
		if(d){
			sprintf(name,"%s/%s",d,s);
			path=name;}
		else
			path=s;}
	if(path==0) {
		fprintf(stderr,"err: not specified %s config\n",n);
		return 0;}
	CSTR str=searchPath(defaultDefSearchPath_,path,spath);
	FILE* iod=fopen(str,"r");
	if(iod==0) {
		fprintf(stderr,"err: cannot open %s config %s\n",n,str);}
	return iod;}

bool RunEnv::doNUTfile(IfNameCreator func) {
	CmCString file;
	FILE* iod=configFile(nutFile_,defaultNUTfileName_,"NUT",file);
	if(iod==0) return false;
	CSTR path=file.string();
	bool rc=true;
	char buf[BUFSIZ];
	uint32_t line=0;
	for(line=1;fgets(buf,sizeof(buf),iod);line++) {
		if(!doNUTline(buf,func,path,line)) rc=false;}
	fclose(iod);
	if(target(0)==0) {
		CmLexer::eouts(path,line,'E',"cannot find default interface %s",
			defaultVirtualIF());
		return false;}
	return rc;}

bool RunEnv::doTNfile(IfNameCreator func) {
	CmCString file;
	FILE* iod=configFile(tnFile_,defaultTNfileName_,"TN",file);
	if(iod==0) return false;
	CSTR path=file.string();
	bool rc=true;
	char buf[BUFSIZ];
	uint32_t line=0;
	for(line=1;fgets(buf,sizeof(buf),iod);line++) {
		if(!doTNline(buf,func,path,line)) rc=false;}
	fclose(iod);
	if(tester(0)==0) {
		CmLexer::eouts(path,line,'E',"cannot find default interface %s",
			defaultVirtualIF());
		return false;}
	return rc;}

static int myEout(const char* fmt,va_list v) {
	fprintf(stderr,"err:");
	return vfprintf(stderr,fmt,v);}

bool RunEnv::doOption(STR* argv,StringList& l) {
	regEoutFunc(myEout);
	bool rc=true;
	STR p;
	for(;*argv&&(p=*++argv);) {
		if(*p!='-') {
			l.add(new CmCString(p));
			continue;}
		bool a=true;
		switch(*++p) {
			case 't': a=tnFile(*++argv);	break;
			case 'i': a=ifName(*++argv);	break;
			case 'n': a=nutFile(*++argv);	break;
			case 'p': a=pktDef(*++argv);	break;
			case 'e': a=timeout(*++argv);	break;
			case 's': a=seek(*++argv);	break;
			case 'c': a=count(*++argv);	break;
			case 'm': a=bufsize(*++argv);	break;
			case 'l': a=log(++p);		break;
			case 'd': a=dbg(++p);		break;
			case 'x': a=hexdump();		break;
			default: 
				fprintf(stderr,"err: unknown option -%s\n",p);
				a=false; break;}
		if(!a) {rc=a;}}
	calctvtimeout();
	return rc;}

CSTR RunEnv::defaultVirtualIF() {
	return ifName_!=0?ifName_->string():defaultInterfaceName_;}

IfName* RunEnv::tester(CSTR s) {
	IfName tmp(s!=0?s:defaultVirtualIF(),0,0);
	return tnSet_.find(&tmp);}

IfName* RunEnv::target(CSTR s) {
	IfName tmp(s!=0?s:defaultVirtualIF(),0,0);
	return nutSet_.find(&tmp);}

CSTR RunEnv::interface() {
	CSTR p=defaultVirtualIF();
	IfName* ifn=tester(p);
	if(ifn==0) {
		fprintf(stderr,"err: cannot find interface %s\n",p);}
	return ifn!=0?ifn->interface():"";}

CSTR RunEnv::interfaceUnixPath() {
	if(interfaceUnixPath_.length()>0) {
		 return interfaceUnixPath_.string();}
	char buf[MAXPATHLEN];
	CSTR name=interface();
	CSTR dir=socketpath_!=0?socketpath_->string():0;
	if(dir==0) {
		CSTR e=getenv(defaultSocketDirEnv_);
		dir=e!=0?e:defaultSocketDir_;}
	sprintf(buf,"%s/%s",dir,name);
	interfaceUnixPath_=buf;
	return interfaceUnixPath_.string();}

CSTR RunEnv::unixPath(CmMain& m,StringList* l) {
	StringList tmp;
	if(!doOption(m.argv(),l!=0?*l:tmp)) {return 0;}
	if(!doTNfile(&IfName::create)) {return 0;}
	if(DBGFLAGS('A')) {print();}
	return interfaceUnixPath();}

CSTR RunEnv::defineFile() {
	return (pktDef_!=0)?pktDef_->string():0;}

#include "PvIfName.h"
static IfName* registerTNIfName(CSTR n,CSTR i,CSTR m,CSTR f,uint32_t l) {
	IfName* in=IfName::create(n,i,m,f,l);
	if(in!=0) {PvIfName::tn(n,m,f,l);}
	return in;}
	
static IfName* registerNUTIfName(CSTR n,CSTR i,CSTR m,CSTR f,uint32_t l) {
	IfName* in=IfName::create(n,i,m,f,l);
	if(in!=0) {PvIfName::nut(n,m,f,l);}
	return in;}

CSTR RunEnv::packetDefinition(CmMain& m,StringList& l,bool& b) {
	b=false;
	if(!doOption(m.argv(),l)) {return 0;}
	bool rc=true;
	if(!doTNfile(&registerTNIfName)) {rc=false;}
	if(!doNUTfile(&registerNUTIfName)) {rc=false;}
	if(!rc) return 0;
	PvIfName::defaultIF(defaultVirtualIF());
	if(DBGFLAGS('A')) {print(); PvIfName::prints();}
	b=rc;
	return defineFile();}

uint32_t RunEnv::bufferSize() {return bufsize_;}
timeval RunEnv::seek() {return seek_;}
uint32_t RunEnv::count() {return count_;}

timeval* RunEnv::tvtimeout() {
	if (!settimeout_)
		return(0);
	else
		return &tvtimeout_;}

CSTR RunEnv::filter() { return filterrule_!=0?filterrule_->string():0;}

void IfName::print(void*,...) const {
	printf("%s:\t%s\t%s\n",string(),interface(),ether());}

void RunEnv::printString(CSTR name,const CmString* def) {
	if(def!=0) {
		printf("%-20s: %s\n",name,def->string());}}
void RunEnv::print() {
	puts("--- COMMAND ARGUMENT ---");
	printString("tester config file",	tnFile_);
	printString("target config file",	nutFile_);
	printString("interface name",		ifName_);
	printString("packet definition",	pktDef_);
	if(settimeout_) {
		time_t sec=tvtimeout_.tv_sec;
		struct tm t=*gmtime(&sec);
		printf("(%02d:%02d:%02d.%06ld GMT)\n",
			t.tm_hour,t.tm_min,t.tm_sec,tvtimeout_.tv_usec);}
	if(seek_!=EPOCHTIMEVAL) {
		time_t sec=seek_.tv_sec;
		struct tm *m=localtime(&sec);
		printf("%-20s: %02d/%02d/%02d %02d:%02d:%02d.%06ld\n",
			"seek",m->tm_year,m->tm_mon+1,m->tm_mday,
			m->tm_hour,m->tm_min,m->tm_sec,seek_.tv_usec);}
	if(count_!=0) {printf("%-20s: %d\n","count",count_);}
	if(logLevel!=0) {printf("%-20s: %d\n","log level",logLevel);}
    	puts("--- TESTING NODE ---");
	printString("socket path",		socketpath_);
	tnSet_.elementsPerform(&IfName::print);
    	puts("--- NODE UNDER TEST ---");
	nutSet_.elementsPerform(&IfName::print);
	puts("--- DEFAULT VALUES ---");
	printf("virtual I/F         : %s\n",defaultVirtualIF());
	printf("interfaceUnixPath() : %s\n",interfaceUnixPath());
	printf("interface()         : %s\n",interface());
	IfName* iftn=tester();
	if(iftn!=0) {
		  printf("tester().ether()    : %s\n",iftn->ether());}
	IfName* ifnut=target();
	if(ifnut!=0) {
		  printf("target().ether()    : %s\n",ifnut->ether());}
}

const uint32_t RunEnv::defaultBufferSize=4096;
const CSTR RunEnv::defaultInterfaceName_="Link0";
const uint32_t RunEnv::interfaceMatchLen=sizeof("Link0")-2;
const CSTR RunEnv::defaultSocketDir_="/tmp";
const CSTR RunEnv::defaultCongigDirEnv_="CONFIGDIR";
const CSTR RunEnv::defaultSocketDirEnv_="SOCKETDIR";
const CSTR RunEnv::defaultTNfileName_="tn.def";
const CSTR RunEnv::defaultNUTfileName_="nut.def";
const CSTR RunEnv::defaultDefSearchPath_=".:/usr/local/v6eval/etc";
CmCString RunEnv::interfaceUnixPath_;
IfNameSet RunEnv::tnSet_(16);
IfNameSet RunEnv::nutSet_(16);
CmCString* RunEnv::socketpath_=0;
CmCString* RunEnv::filterrule_=0;
CmCString* RunEnv::tnFile_=0;
CmCString* RunEnv::ifName_=0;
CmCString* RunEnv::nutFile_=0;
CmCString* RunEnv::pktDef_=0;
uint32_t RunEnv::settimeout_=0;
timeval RunEnv::tvtimeout_;
timeval RunEnv::seek_=EPOCHTIMEVAL;
uint32_t RunEnv::count_=0;
uint32_t RunEnv::bufsize_=defaultBufferSize;
implementCmSet(IfNameSet,IfName);

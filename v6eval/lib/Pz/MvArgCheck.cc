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
 * $TAHI: v6eval/lib/Pz/MvArgCheck.cc,v 1.17 2005/05/09 09:35:24 akisada Exp $
 */
#include "MvFunction.h"
#include "PvObject.h"
#include "PvOctets.h"
#include "PvIfName.h"

#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

bool MvFunction::isV6Addr() const {return false;}
bool MvFunction::isOctets() const {return false;}
bool MvV6::isV6Addr() const {return true;}
bool MvOctets::isOctets() const {return true;}

bool MvFunction::checkArgument(const PFunction&,const PObjectList&) const {
	return false;}

//======================================================================
// within(int,int)
bool MvWithin::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	uint32_t n=a.size();
	CSTR name=o.metaString();
	if(n!=2) {
		o.error("E %s must have 2 arguments, not %d",name,n);
		return false;}
	a[0]->intValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be int",name);
		rc=false;}
	a[1]->intValue(ok);
	if(!ok) {
		o.error("E %s second argument has to be int",name);
		rc=false;}
	return rc;}

//======================================================================
// oneof(var[,var[,...]]) || oneof(int[,int[,...]])
bool MvOneof::checkArgument(const PFunction&,const PObjectList&) const {
	bool rc=true;
	return rc;}

//======================================================================
// comb(var[,var[,...]])
bool MvComb::checkArgument(const PFunction&,const PObjectList&) const {
	bool rc=true;
	return rc;}

//======================================================================
// repeat(int,int)
bool MvRepeat::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=2) {
		o.error("E %s must have 2 arguments, not %d",name,n);
		return false;}
	a[0]->intValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be int",name);
		rc=false;}
	a[1]->intValue(ok);
	if(!ok) {
		o.error("E %s second argument has to be int",name);
		rc=false;}
	return rc;}

//======================================================================
// hexstr(hexstring, int, int)
bool MvHexStr::checkArgument(const PFunction &o, const PObjectList &a) const {
	bool ok = true;
	bool rc = true;
	CSTR name = o.metaString();

	uint32_t n = a.size();
	if((n < 1) || (n > 3)) {
		o.error("E %s must have 1 or 2 or 3 arguments, not %d", name, n);
		return false;
	}

	//
	// 1st argment
	//
	////////////////////////////////////////////////////////////////

	CSTR a0 = a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be string", name);
		rc = false;
	} else {
		uint32_t a0len = a[0]->length();

		if(!isHexStr(a0, a0len)) {
			o.error("E %s argument has to be hex characters", name);
			rc = false;
			ok = false;
		}
	}

	if(n < 2) {
		return(rc);
	}

	//
	// 2nd argment
	//
	////////////////////////////////////////////////////////////////

	a[1]->intValue(ok);
	if(!ok) {
		o.error("E %s second argument has to be int", name);
		rc = false;
	}

	if(n < 3) {
		return(rc);
	}

	//
	// 3rd argment
	//
	////////////////////////////////////////////////////////////////

	a[2]->intValue(ok);
	if(!ok) {
		o.error("E %s third argument has to be int", name);
		rc = false;
	}

	return(rc);
}

//======================================================================
// int2hex(int, int)
bool
MvInt2Hex::checkArgument(const PFunction &o, const PObjectList &a) const
{
	bool ok = true;
	bool rc = true;
	CSTR name = o.metaString();

	uint32_t n = a.size();
	if((n < 1) || (n > 2)) {
		o.error("E %s must have 1 or 2 arguments, not %d", name, n);
		return false;
	}

	//
	// 1st argment
	//
	////////////////////////////////////////////////////////////////

	uint32_t base = a[0]->intValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be int", name);
		rc = false;
	}

	if(n < 2) {
		return(rc);
	}

	//
	// 2nd argment
	//
	////////////////////////////////////////////////////////////////

	uint32_t val = a[1]->intValue(ok);
	if(!ok) {
		o.error("E %s second argument has to be int", name);
		rc = false;
	}

	uint32_t len = 0;
	for( ; base; base /= 0x100, len ++) {}

	if(len > val) {
		o.error("E %s second argument has to be more larger", name);
		rc = false;
	}

	return(rc);
}

//======================================================================
// file(file, offset, length)
bool MvFile::checkArgument(const PFunction &o, const PObjectList &a) const {
	bool ok = true;
	CSTR name = o.metaString();

	uint32_t n = a.size();
	if((n < 1) || (n > 3)) {
		o.error("E %s must have 1 or 2 or 3 arguments, not %d", name, n);
		return(false);
	}

	////////////////////////////////
	// 1st argment                //
	////////////////////////////////
	CSTR a0 = a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be string", name);
		return(false);
	} else {
		int fd = 0;

		if((fd = open(a0, O_RDONLY)) < 0) {
			o.error("E %s: %s", a0, strerror(errno));
			return(false);
		}

		close(fd);
	}

	if(n < 2) {
		return(true);
	}

	struct stat st;
	memset(&st, 0, sizeof(struct stat));

	if(stat(a0, &st) < 0) {
		o.error("E %s: %s", a0, strerror(errno));
		return(false);
	}

	uint32_t size = st.st_size;

	////////////////////////////////
	// 2nd argment                //
	////////////////////////////////
	uint32_t offset = a[1]->intValue(ok);
	if(!ok) {
		o.error("E %s second argument has to be int", name);
		return(false);
	}

	if(offset > size) {
		o.error("E %s offset must be less than file size", name);
		return(false);
	}

	if(n < 3) {
		return(true);
	}

	////////////////////////////////
	// 3rd argment                //
	////////////////////////////////
	uint32_t length = a[2]->intValue(ok);
	if(!ok) {
		o.error("E %s third argument has to be int", name);
		return(false);
	}

	if(offset + length > size) {
		o.error("E %s offset + length must be equal to file size or less than file size", name);
		return(false);
	}

	return(true);
}

//======================================================================
// ascii(ascii, int)
bool MvAscii::checkArgument(const PFunction &o, const PObjectList &a) const {
	bool ok = true;
	bool rc = true;
	CSTR name = o.metaString();

	uint32_t n = a.size();
	if((n < 1) || (n > 2)) {
		o.error("E %s must have 1 or 2 arguments, not %d", name, n);
		return false;
	}

	//
	// 1st argment
	//
	////////////////////////////////////////////////////////////////

	a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be string", name);
		rc = false;
	}

	if(n < 2) {
		return(rc);
	}

	//
	// 2nd argment
	//
	////////////////////////////////////////////////////////////////

	a[1]->intValue(ok);
	if(!ok) {
		o.error("E %s second argument has to be int", name);
		rc = false;
	}

	return(rc);
}

//======================================================================
// dnsstr(ascii)
bool MvDnsStr::checkArgument(const PFunction &o, const PObjectList &a) const {
	bool ok = true;
	bool rc = true;
	CSTR name = o.metaString();

	uint32_t n = a.size();
	if(n != 1) {
		o.error("E %s must have 1 argument, not %d", name, n);
		return(false);
	}

	a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be string", name);
		rc = false;
	}

	return(rc);
}

//======================================================================
// dnsname(ascii)
bool MvDnsName::checkArgument(const PFunction &o, const PObjectList &a) const {
	bool ok = true;
	bool rc = true;
	CSTR name = o.metaString();

	uint32_t n = a.size();
	if((n < 1) || (n > 2)) {
		o.error("E %s must have 1 or 2 arguments, not %d", name, n);
		return(false);
	}

	CSTR a0 = a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be string", name);
		rc = false;
	} else {
		uint32_t a0len = a[0]->length();

		if(!isDnsName(a0, a0len)) {
			o.error("E %s argument - is not a legal name syntax", name);
			rc = false;
			ok = false;
		}
	}

	if(n < 2) {
		return(rc);
	}

	int a1 = a[1]->intValue(ok);
	if(!ok) {
		o.error("E %s second argument has to be int", name);
		rc = false;
	}

	if((a1 < 0) || (a1 > 0x3fff)) {
		o.error("E %s second argument - out of range", name);
		rc = false;
		ok = false;
	}

	return(rc);
}

//======================================================================
// cookie(octet, v6, octet);
// keygentoken(octet, v6, octet, int);

bool MvCookie64::checkArgument(const PFunction &o, const PObjectList &a) const {
	bool rc = true;
	CSTR name = o.metaString();

	uint32_t n = a.size();

	if(n != 4) {
		o.error("E %s must have 4 arguments, not %d", name, n);
		return(false);
	}

	//
	// 1st argment
	//
	////////////////////////////////////////////////////////////////
	if(!a[0]->isOctets()) {
		o.error("E %s first argument has to be octet stream", name);
		rc = false;
	}

	//
	// 2nd argment
	//
	////////////////////////////////////////////////////////////////
	if(!a[1]->isV6Addr()) {
		o.error("E %s second argument has to be V6 addr type", name);
		rc = false;
	}

	//
	// 3rd argment
	//
	////////////////////////////////////////////////////////////////
	if(!a[2]->isOctets()) {
		o.error("E %s third argument has to be octet stream", name);
		rc = false;
	}

	bool ok = true;
	a[3]->intValue(ok);
	if(!ok) {
		o.error("E %s 4th argument has to be int", name);
		rc = false;
	}

	return(rc);
}

//======================================================================
// kbu(octet, octet);

bool MvKbu::checkArgument(const PFunction &o, const PObjectList &a) const {
	bool rc = true;
	CSTR name = o.metaString();

	uint32_t n = a.size();

	if((n < 1) || (n > 2)) {
		o.error("E %s must have 1 or 2 arguments, not %d", name, n);
		return(false);
	}

	//
	// 1st argment
	//
	////////////////////////////////////////////////////////////////
	if(!a[0]->isOctets()) {
		o.error("E %s first argument has to be octet stream", name);
		rc = false;
	}

	if(n < 2) {
		return(rc);
	}

	//
	// 2nd argment
	//
	////////////////////////////////////////////////////////////////
	if(!a[1]->isOctets()) {
		o.error("E %s second argument has to be octet stream", name);
		rc = false;
	}

	return(rc);
}

//======================================================================
// bsa(octet, v6, v6);

bool MfBSA::checkArgument(const PFunction &o, const PObjectList &a) const {
	bool rc = true;
	CSTR name = o.metaString();

	uint32_t n = a.size();
	if(n != 3) {
		o.error("E %s must have 3 arguments, not %d", name, n);
		return false;
	}

	//
	// 1st argment
	//
	////////////////////////////////////////////////////////////////
	if(!a[0]->isOctets()) {
		o.error("E %s first argument has to be octet stream", name);
		rc = false;
	}

	//
	// 2nd argment
	//
	////////////////////////////////////////////////////////////////
	if(!a[1]->isV6Addr()) {
		o.error("E %s second argument has to be V6 addr type", name);
		rc = false;
	}

	//
	// 3rd argment
	//
	////////////////////////////////////////////////////////////////
	if(!a[2]->isV6Addr()) {
		o.error("E %s third argument has to be V6 addr type", name);
		rc = false;
	}

	return(rc);
}

//======================================================================
// dhcpauth(str);

bool MfDHCPAuth::checkArgument(const PFunction &o, const PObjectList &a) const {
	bool ok = true;
	bool rc = true;
	CSTR name = o.metaString();

	uint32_t n = a.size();
	if(n != 1) {
		o.error("E %s must have 1 argument, not %d", name, n);
		return(false);
	}

	a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be string", name);
		rc = false;
	}

	return(rc);
}

//======================================================================
// substr(var,int,int)
bool MvSubstr::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=3) {
		o.error("E %s must have 3 arguments, not %d",name,n);
		return false;}
	if(!a[0]->isName()) {
		o.error("E %s first argument has to be name",name);
		rc=false;}
	a[1]->intValue(ok);
	if(!ok) {
		o.error("E %s second argument has to be int",name);
		rc=false;}
	a[2]->intValue(ok);
	if(!ok) {
		o.error("E %s third argument has to be int",name);
		rc=false;}
	return rc;}

//======================================================================
// patch(var,int,int)
bool MvPatch::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=3) {
		o.error("E %s must have 3 arguments, not %d",name,n);
		return false;}
	if(!a[0]->isName()) {
		o.error("E %s first argument has to be name",name);
		rc=false;}
	a[1]->intValue(ok);
	if(!ok) {
		o.error("E %s second argument has to be int",name);
		rc=false;}
	a[2]->intValue(ok);
	if(!ok) {
		o.error("E %s third argument has to be int",name);
		rc=false;}
	return rc;}

//======================================================================
// left(var,int)
bool MvLeft::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=2) {
		o.error("E %s must have 2 arguments, not %d",name,n);
		return false;}
	if(!a[0]->isName()) {
		o.error("E %s first argument has to be name",name);
		rc=false;}
	a[1]->intValue(ok);
	if(!ok) {
		o.error("E %s second argument has to be int",name);
		rc=false;}
	return rc;}

//======================================================================
// right(var,int)
bool MvRight::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=2) {
		o.error("E %s must have 2 arguments, not %d",name,n);
		return false;}
	if(!a[0]->isName()) {
		o.error("E %s first argument has to be name",name);
		rc=false;}
	a[1]->intValue(ok);
	if(!ok) {
		o.error("E %s second argument has to be int",name);
		rc=false;}
	return rc;}

//======================================================================
// v4(v4Presntation)
bool MvV4::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=1) {
		o.error("E %s must have 1 argument, not %d",name,n);
		return false;}
	CSTR a0=a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be v4 presentation",name);
		rc=false;}
	else {
		PvV4Addr c0(a0,ok);
		if(!ok) {
			o.error("E %s(\"%s\") has to be v4 presentation",
				name,a0);
			rc=false;}}
	return rc;}

//======================================================================
// ether(etherPresntation)
bool MvEther::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=1) {
		o.error("E %s must have 1 argument, not %d",name,n);
		return false;}
	CSTR a0=a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be ether presentation",name);
		rc=false;}
	else {
		PvEther c0(a0,ok);
		if(!ok) {
			o.error("E %s(\"%s\") has to be ether presentation",
				name,a0);
			rc=false;}}
	return rc;}

//======================================================================
// ethersrc([ifname])
bool MvEtherSRC::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=0&&n!=1) {
		o.error("E %s must have 0 or 1 argument, not %d",name,n);
		return false;}
	if(n==0) {return rc;}
	a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be ifname",name);
		rc=false;}
	return rc;}

//======================================================================
// etherdst([ifname])
bool MvEtherDST::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=0&&n!=1) {
		o.error("E %s must have 0 or 1 argument, not %d",name,n);
		return false;}
	if(n==0) {return rc;}
	a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be ifname",name);
		rc=false;}
	return rc;}

//======================================================================
// nutether([ifname])
bool MvEtherNUT::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=0&&n!=1) {
		o.error("E %s must have 0 or 1 arguments, not %d",name,n);
		return false;}
	if(n==0) {return rc;}
	CSTR a0=a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be ifname",name);
		rc=false;}
	else {
		PvIfName* n0=PvIfName::findNut(a0);
		if(n0==0) {
			o.error("W %s(%s) interface not found",name,a0);
			rc=false;}}
	return rc;}

//======================================================================
// tnether([ifname])
bool MvEtherTN::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=0&&n!=1) {
		o.error("E %s must have 0 or 1 arguments, not %d",name,n);
		return false;}
	if(n==0) {return rc;}
	CSTR a0=a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be ifname",name);
		rc=false;}
	else {
		PvIfName* n0=PvIfName::findTn(a0);
		if(n0==0) {
			o.error("W %s(%s) interface not found",name,a0);
			rc=false;}}
	return rc;}

//======================================================================
// v62ethermulti(v6)
bool MvEtherMulti::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=1) {
		o.error("E %s must have 1 argument, not %d",name,n);
		return false;}
	if(!a[0]->isV6Addr()) {
		o.error("E %s first argument has to be V6 addr type",name);
		rc=false;}
	return rc;}

//======================================================================
// v6(v6Presntation)
bool MvV6::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=1) {
		o.error("E %s must have 1 argument, not %d",name,n);
		return false;}
	CSTR a0=a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be v6 presentation",name);
		rc=false;}
	else {
		PvV6Addr c0(a0,ok);
		if(!ok) {
			o.error("E %s(\"%s\") has to be v6 presentation",
				name,a0);
			rc=false;}}
	return rc;}

//======================================================================
// v6src([ifname])
bool MvV6SRC::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=0&&n!=1) {
		o.error("E %s must have 0 or 1 argument, not %d",name,n);
		return false;}
	if(n==0) {return rc;}
	a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be ifname",name);
		rc=false;}
	return rc;}

//======================================================================
// v6dst([ifname])
bool MvV6DST::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=0&&n!=1) {
		o.error("E %s must have 0 or 1 argument, not %d",name,n);
		return false;}
	if(n==0) {return rc;}
	a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be ifname",name);
		rc=false;}
	return rc;}

//======================================================================
// nutv6([ifname])
bool MvV6NUT::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=0&&n!=1) {
		o.error("E %s must have 0 or 1 argument, not %d",name,n);
		return false;}
	if(n==0) {return rc;}
	CSTR a0=a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be ifname",name);
		rc=false;}

	else {
		PvIfName* n0=PvIfName::findNut(a0);
		if(n0==0) {
			o.error("W %s(%s) interface not found",name,a0);
			rc=false;}}
	return rc;}

//======================================================================
// tnv6([ifname])
bool MvV6TN::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=0&&n!=1) {
		o.error("E %s must have 0 or 1 argument, not %d",name,n);
		return false;}
	if(n==0) {return rc;}
	CSTR a0=a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be ifname",name);
		rc=false;}
	else {
		PvIfName* n0=PvIfName::findTn(a0);
		if(n0==0) {
			o.error("W %s(%s) interface not found",name,a0);
			rc=false;}}
	return rc;}

//======================================================================
// v6(v6Presntation,bitwidth[,ifname])
bool MvV6PNUT::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=2&&n!=3) {
		o.error("E %s must have 2 or 3 arguments, not %d",name,n);
		return false;}
	CSTR a0=a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be v6 presentation",name);
		rc=false;}
	else {
		PvV6Addr c0(a0,ok);
		if(!ok) {
			o.error("E %s(\"%s\",,) has to be v6 presentation",
				name,a0);
			rc=false;}}
	int a1=a[1]->intValue(ok);
	if(!ok) {
		o.error("E %s second argument has to be int",name);
		rc=false;}
	else if(a1<0||a1>int(sizeof(v6addr)*8)) {
		o.error("E %s(,%d,) has to be prefix bit width",
			name,a1);
		rc=false;}
	if(n==2) {return rc;}
	CSTR a2=a[2]->strValue(ok);
	if(!ok) {
		o.error("E %s third argument has to be ifname",name);
		rc=false;}
	else {
		PvIfName* n2=PvIfName::findNut(a2);
		if(n2==0) {
			o.error("W %s(,,%s) interface not found",name,a2);
			rc=false;}}
	return rc;}

//======================================================================
// v6(v6Presntation,bitwidth[,ifname])
bool MvV6PTN::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=2&&n!=3) {
		o.error("E %s must have 2 or 3 arguments, not %d",name,n);
		return false;}
	CSTR a0=a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be v6 presentation",name);
		rc=false;}
	else {
		PvV6Addr c0(a0,ok);
		if(!ok) {
			o.error("E %s(\"%s\",,) has to be v6 presentation",
				name,a0);
			rc=false;}}
	int a1=a[1]->intValue(ok);
	if(!ok) {
		o.error("E %s second argument has to be prefix bit width",name);
		rc=false;}
	else if(a1<0||a1>int(sizeof(v6addr)*8)) {
		o.error("E %s(,%d,) has to be prefix bit width",
			name,a1);
		rc=false;}
	if(n==2) {return rc;}
	CSTR a2=a[2]->strValue(ok);
	if(!ok) {
		o.error("E %s third argument has to be ifname",name);
		rc=false;}
	else {
		PvIfName* n2=PvIfName::findTn(a2);
		if(n2==0) {
			o.error("W %s(,,%s) interface not found",name,a2);
			rc=false;}}
	return rc;}

//======================================================================
// v6(v6Presntation,bitwidth,v6)
bool MvV6Merge::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=3) {
		o.error("E %s must have 3 arguments, not %d",name,n);
		return false;}
	CSTR a0=a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be v6 presentation",name);
		rc=false;}
	else {
		PvV6Addr c0(a0,ok);
		if(!ok) {
			o.error("E %s(\"%s\",,) has to be v6 presentation",
				name,a0);
			rc=false;}}
	int a1=a[1]->intValue(ok);
	if(!ok) {
		o.error("E %s second argument has to be int",name);
		rc=false;}
	else if(a1<0||a1>int(sizeof(v6addr)*8)) {
		o.error("E %s(,%d,) has to be prefix bit width",
			name,a1);
		rc=false;}
	if(!a[2]->isV6Addr()) {
		o.error("E %s third argument has to be V6 addr type",name);
		rc=false;}
	return rc;}

//======================================================================
// v6(v6Presntation,bitwidth,v6Presntation)
bool MvV6V6::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=3) {
		o.error("E %s must have 3 arguments, not %d",name,n);
		return false;}
	CSTR a0=a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be v6 presentation",name);
		rc=false;}
	else {
		PvV6Addr c0(a0,ok);
		if(!ok) {
			o.error("E %s(\"%s\",,) has to be v6 presentation",
				name,a0);
			rc=false;}}
	int a1=a[1]->intValue(ok);
	if(!ok) {
		o.error("E %s second argument has to be int",name);
		rc=false;}
	else if(a1<0||a1>int(sizeof(v6addr)*8)) {
		o.error("E %s(,%d,) has to be prefix bit width",
			name,a1);
		rc=false;}
	CSTR a2=a[2]->strValue(ok);
	if(!ok) {
		o.error("E %s third argument has to be v6 presentation",name);
		rc=false;}
	else {
		PvV6Addr c2(a2,ok);
		if(!ok) {
			o.error("E %s(,,\"%s\") has to be v6 presentation",
				name,a2);
			rc=false;}}
	return rc;}

//======================================================================
// ether(etherPresntation)
bool MvV6Ether::checkArgument(const PFunction& o,const PObjectList& a) const {
	bool ok=true;
	bool rc=true;
	CSTR name=o.metaString();
	uint32_t n=a.size();
	if(n!=1) {
		o.error("E %s must have 1 argument, not %d",name,n);
		return false;}
	CSTR a0=a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be ether presentation",name);
		rc=false;}
	else {
		PvEther c0(a0,ok);
		if(!ok) {
			o.error("E %s(\"%s\") has to be ether presentation",
				name,a0);
			rc=false;}}
	return rc;}

//======================================================================
// objectid(oid_string)
bool MvObjectID::checkArgument(const PFunction &o, const PObjectList &a) const {
	bool ok = true;
	bool rc = true;
	CSTR name = o.metaString();

	uint32_t n = a.size();
	if(n!=1) {
		o.error("E %s must have 1 argument, not %d", name, n);
		return false;
	}

	//
	// 1st argment
	//
	////////////////////////////////////////////////////////////////
	
	CSTR a0 = a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be string", name);
		rc = false;
	} else {
		uint32_t a0len = a[0]->length();

		if(!isOidStr(a0, a0len)) {
			o.error("E %s argument has to be hex characters", name);
			rc = false;
			ok = false;
		}
	}	
	
	if(n < 2) {
		return(rc);
	}
	
	return(rc);
}

//======================================================================
// p1_hash(algorithm, SKEYID, g^xi, g^xr, CKY-I, CKY-R, SAi_b, IDii_b)
// p1_hash(algorithm, SKEYID, g^xr, g^xi, CKY-R, CKY-I, SAi_b, IDir_b)
bool
MvP1_HASH::checkArgument(const PFunction &o, const PObjectList &a) const
{
	bool ok = true;
	bool rc = true;
	CSTR name = o.metaString();
	uint32_t d = 0;

	uint32_t n = a.size();
	if(n != 8) {
		o.error("E %s must have 8 arguments, not %d", name, n);
		return false;
	}

	////////////////////////////////
	// 1st argment                //
	////////////////////////////////
	CSTR a0 = a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be string", name);
		rc = false;
	} else {
		for( ; ; ) {
			if(!strcmp(a0, "hmacmd5")) {
				break;
			}

			if(!strcmp(a0, "hmacsha1")) {
				break;
			}

			o.error("E %s unknown algorithm -- %s", name, a0);
			rc = false;
		}
	}

	////////////////////////////////
	// 2nd - 6th argment          //
	////////////////////////////////
	for(d = 1; d < 6; d ++) {
		if(!a[d]->isOctets()) {
			o.error("E %s argument[%d] has to be octet stream",
				name, d);

			rc = false;
		}
	}

	////////////////////////////////
	// 7th - 8th argment          //
	////////////////////////////////
	for(d = 6; d < 8; d ++) {
		if(!a[d]->isName()) {
			o.error("E %s argument[%d] has to be name",
				name, d);

			rc = false;
		}
	}

	return(rc);
}

//--------------------------------------------------------------
// p1_iv("algorithm", "g^xi", "g^xr", length)
bool
MvP1_IV::checkArgument(const PFunction &o, const PObjectList &a) const
{
	bool ok = true;
	bool rc = true;
	CSTR name = o.metaString();
	uint32_t d = 0;

	uint32_t n = a.size();
	if((n != 3) && (n != 4)) {
		o.error("E %s must have 3-4 arguments, not %d", name, n);
		return(false);
	}

	////////////////////////////////
	// 1st argment                //
	////////////////////////////////
	CSTR a0 = a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be string", name);
		rc = false;
	} else {
		for( ; ; ) {
			if(!strcmp(a0, "md5")) {
				break;
			}

			if(!strcmp(a0, "sha1")) {
				break;
			}

			o.error("E %s unknown algorithm -- %s", name, a0);
			rc = false;
		}
	}

	////////////////////////////////
	// 2nd - 3rd argment          //
	////////////////////////////////
	for(d = 1; d < 3; d ++) {
		if(!a[d]->isOctets()) {
			o.error("E %s argument[%d] has to be octet stream",
				name, d);

			rc = false;
		}
	}

	if(n < 4) {
		return(rc);
	}

	////////////////////////////////
	// 4th argment                //
	////////////////////////////////

	a[3]->intValue(ok);
	if(!ok) {
		o.error("E %s 4th argument has to be int", name);
		rc = false;
	}

	return(rc);
}

//--------------------------------------------------------------
// p2_iv("algorithm", "last CBC", length)
bool
MvP2_IV::checkArgument(const PFunction &o, const PObjectList &a) const
{
	bool ok = true;
	bool rc = true;
	CSTR name = o.metaString();

	uint32_t n = a.size();
	if((n != 2) && (n != 3)) {
		o.error("E %s must have 2-3 arguments, not %d", name, n);
		return(false);
	}

	////////////////////////////////
	// 1st argment                //
	////////////////////////////////
	CSTR a0 = a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be string", name);
		rc = false;
	} else {
		for( ; ; ) {
			if(!strcmp(a0, "md5")) {
				break;
			}

			if(!strcmp(a0, "sha1")) {
				break;
			}

			o.error("E %s unknown algorithm -- %s", name, a0);
			rc = false;
		}
	}

	////////////////////////////////
	// 2nd argment                //
	////////////////////////////////
	if(!a[1]->isOctets()) {
		o.error("E %s 2nd argument has to be octet stream", name);
		rc = false;
	}

	if(n < 3) {
		return(rc);
	}

	////////////////////////////////
	// 3rd argment                //
	////////////////////////////////

	a[2]->intValue(ok);
	if(!ok) {
		o.error("E %s 3rd argument has to be int", name);
		rc = false;
	}

	return(rc);
}

//--------------------------------------------------------------
// p2_hash_1("algorithm", "SKEYID_a")
bool
MvP2_HASH_1::checkArgument(const PFunction &o, const PObjectList &a) const
{
	bool ok = true;
	bool rc = true;
	CSTR name = o.metaString();

	uint32_t n = a.size();
	if(n != 2) {
		o.error("E %s must have 2 arguments, not %d", name, n);
		return(false);
	}

	////////////////////////////////
	// 1st argment                //
	////////////////////////////////
	CSTR a0 = a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be string", name);
		rc = false;
	} else {
		for( ; ; ) {
			if(!strcmp(a0, "hmacmd5")) {
				break;
			}

			if(!strcmp(a0, "hmacsha1")) {
				break;
			}

			o.error("E %s unknown algorithm -- %s", name, a0);
			rc = false;
		}
	}

	////////////////////////////////
	// 2nd argment                //
	////////////////////////////////
	if(!a[1]->isOctets()) {
		o.error("E %s 2nd argument has to be octet stream", name);
		rc = false;
	}

	return(rc);
}

//--------------------------------------------------------------
// p2_hash_2("algorithm", "SKEYID_a", "Ni_b")
bool
MvP2_HASH_2::checkArgument(const PFunction &o, const PObjectList &a) const
{
	bool ok = true;
	bool rc = true;
	CSTR name = o.metaString();

	uint32_t n = a.size();
	if(n != 3) {
		o.error("E %s must have 3 arguments, not %d", name, n);
		return(false);
	}

	////////////////////////////////
	// 1st argment                //
	////////////////////////////////
	CSTR a0 = a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be string", name);
		rc = false;
	} else {
		for( ; ; ) {
			if(!strcmp(a0, "hmacmd5")) {
				break;
			}

			if(!strcmp(a0, "hmacsha1")) {
				break;
			}

			o.error("E %s unknown algorithm -- %s", name, a0);
			rc = false;
		}
	}

	////////////////////////////////
	// 2nd argment                //
	////////////////////////////////
	if(!a[1]->isOctets()) {
		o.error("E %s 2nd argument has to be octet stream", name);
		rc = false;
	}

	////////////////////////////////
	// 3rd argment                //
	////////////////////////////////
	if(!a[2]->isOctets()) {
		o.error("E %s 3rd argument has to be octet stream", name);
		rc = false;
	}

	return(rc);
}

//--------------------------------------------------------------
// p2_hash_3("algorithm", "SKEYID_a", "Ni_b", "Nr_b")
bool
MvP2_HASH_3::checkArgument(const PFunction &o, const PObjectList &a) const
{
	bool ok = true;
	bool rc = true;
	CSTR name = o.metaString();

	uint32_t n = a.size();
	if(n != 4) {
		o.error("E %s must have 4 arguments, not %d", name, n);
		return(false);
	}

	////////////////////////////////
	// 1st argment                //
	////////////////////////////////
	CSTR a0 = a[0]->strValue(ok);
	if(!ok) {
		o.error("E %s first argument has to be string", name);
		rc = false;
	} else {
		for( ; ; ) {
			if(!strcmp(a0, "hmacmd5")) {
				break;
			}

			if(!strcmp(a0, "hmacsha1")) {
				break;
			}

			o.error("E %s unknown algorithm -- %s", name, a0);
			rc = false;
		}
	}

	////////////////////////////////
	// 2nd argment                //
	////////////////////////////////
	if(!a[1]->isOctets()) {
		o.error("E %s 2nd argument has to be octet stream", name);
		rc = false;
	}

	////////////////////////////////
	// 3rd argment                //
	////////////////////////////////
	if(!a[2]->isOctets()) {
		o.error("E %s 3rd argument has to be octet stream", name);
		rc = false;
	}

	////////////////////////////////
	// 4th argment                //
	////////////////////////////////
	if(!a[3]->isOctets()) {
		o.error("E %s 4th argument has to be octet stream", name);
		rc = false;
	}

	return(rc);
}

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
 * $TAHI: v6eval/lib/Pz/debug.h,v 1.7 2007/03/07 11:04:57 akisada Exp $
 */

#ifndef __debug_h__
#define __debug_h__

#ifdef RR_DBG
#ifndef XXX_DBG
#define XXX_DBG
#endif	// XXX_DBG
#endif	// RR_DBG

#ifdef DHCP_DBG
#ifndef XXX_DBG
#define XXX_DBG
#endif	// XXX_DBG
#endif	// DHCP_DBG

#ifdef ISAKMP_DBG
#ifndef XXX_DBG
#define XXX_DBG
#endif	// XXX_DBG
#endif	// ISAKMP_DBG

#ifdef VRRP_DBG
#ifndef XXX_DBG
#define XXX_DBG
#endif	// XXX_DBG
#endif	// VRRP_DBG

#ifdef AESXCBC_DBG
#ifndef XXX_DBG
#define XXX_DBG
#endif	// XXX_DBG
#endif	// AESXCBC_DBG

#ifdef AESCTR_DBG
#ifndef XXX_DBG
#define XXX_DBG
#endif	// XXX_DBG
#endif	// AESCTR_DBG

#ifdef ESN_DBG
#ifndef XXX_DBG
#define XXX_DBG
#endif	// XXX_DBG
#endif	// ESN_DBG

#ifdef XXX_DBG
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef xdbg
#define xdbg(file, class, fmt, args...) \
{ \
FILE *stream = fopen(file, "a"); \
if(stream) { \
fprintf(stream, "dbg[%d]: %s: %d: %s::%s(): " fmt, \
getpid(), __FILE__, __LINE__, class, __FUNCTION__, ##args); \
fclose(stream); \
} \
}
#endif  // xdbg

#ifndef xdmp
#define xdmp(file, class, name, buf, buflen) \
{ \
FILE *stream = fopen(file, "a"); \
if(stream) { \
unsigned char *buffer = (unsigned char *)buf; \
unsigned int length = (unsigned int)buflen; \
unsigned int d = 0; \
fprintf(stream, "dmp[%d]: %s: %d: %s::%s(): %s: buffer: ", \
getpid(), __FILE__, __LINE__, class, __FUNCTION__, name); \
for(d = 0; d < length; d ++) { \
fprintf(stream, "%02x", buffer[d]); \
} \
fputc('\n', stream); \
fprintf(stream, "dmp[%d]: %s: %d: %s::%s(): %s: length: %d\n", \
getpid(), __FILE__, __LINE__, class, __FUNCTION__, name, length); \
fclose(stream); \
} \
}
#endif  // xdmp
#endif  // XXX_DBG
#endif	// __debug_h__

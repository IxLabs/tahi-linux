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
 * $TAHI: v6eval/lib/Pz/McRIPng.h,v 1.2 2002/12/02 12:31:30 akisada Exp $
 */

#if !defined(__McRIPng_h__)
#define	__McRIPng_h__	1

#include "McSub.h"
#include "McUDP.h"

////////////////////////////////////////////////////////////////
class McUdp_RIPng: public McHeader {
	McUdp_RIPng(CSTR);

	public:
		virtual ~McUdp_RIPng();

		static McUdp_RIPng *create(CSTR);

		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;

		int32_t upperPort() const {
			return(TP_Udp_RIPng);
		}
};

////////////////////////////////////////////////////////////////
class McRIPng_RTE: public McOption {
	McRIPng_RTE(CSTR);

	public:
		virtual ~McRIPng_RTE();

		static McRIPng_RTE *create(CSTR);

		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class MmRTE_onRIPng: public MmReference_More0 {
	static TypevsMcDict dict_;

	public:
		MmRTE_onRIPng(CSTR);
		virtual ~MmRTE_onRIPng();

		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add_other(McRIPng_RTE *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};
#endif

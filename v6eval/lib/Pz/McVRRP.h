//
// Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011
// Yokogawa Electric Corporation, YDC Corporation,
// IPA (Information-technology Promotion Agency, Japan).
// All rights reserved.
// 
// Redistribution and use of this software in source and binary forms, with 
// or without modification, are permitted provided that the following 
// conditions and disclaimer are agreed and accepted by the user:
// 
// 1. Redistributions of source code must retain the above copyright 
// notice, this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright 
// notice, this list of conditions and the following disclaimer in the 
// documentation and/or other materials provided with the distribution.
// 
// 3. Neither the names of the copyrighters, the name of the project which 
// is related to this software (hereinafter referred to as "project") nor 
// the names of the contributors may be used to endorse or promote products 
// derived from this software without specific prior written permission.
// 
// 4. No merchantable use may be permitted without prior written 
// notification to the copyrighters. However, using this software for the 
// purpose of testing or evaluating any products including merchantable 
// products may be permitted without any notification to the copyrighters.
// 
// 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHTERS, THE PROJECT AND 
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING 
// BUT NOT LIMITED THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.  IN NO EVENT SHALL THE 
// COPYRIGHTERS, THE PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT,STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
// THE POSSIBILITY OF SUCH DAMAGE.
//
// $TAHI: v6eval/lib/Pz/McVRRP.h,v 1.2 2004/09/22 08:54:13 akisada Exp $
//

#ifndef __McVRRP_h__
#define __McVRRP_h__

#include "McSub.h"
#include "MmHeader.h"
#include "MmObject.h"
#include "PControl.h"
#include "ItPosition.h"

class McUpp_VRRP_ONE: public McUpper {
	private:
		static McUpp_VRRP_ONE *instance_;
		McUpp_VRRP_ONE(CSTR);
	public:
		virtual ~McUpp_VRRP_ONE();
		static McUpp_VRRP_ONE *instance();
		int32_t headerType() const { return(TP_Upp_VRRP); }
		bool containsMc(const MObject *mc) const;
		virtual uint32_t length_for_reverse(RControl &,
			ItPosition &, OCTBUF &) const;
		virtual RObject *reverse(RControl &,
			RObject *, ItPosition &, OCTBUF &) const;
};

class McUpp_VRRP: public McHeader {
	protected:
		MmUint *version_;
		void common_member();
		McUpp_VRRP(CSTR);

		void version_member(MmUint *meta) {
			version_ = meta; member(meta); }

	public:
		virtual ~McUpp_VRRP();
		int32_t headerType() const { return(TP_Upp_VRRP); }
		virtual uint32_t length_for_reverse(RControl &,
			ItPosition &, OCTBUF &) const;
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
		virtual int32_t token() const;
		virtual RObject *reverse(RControl &,
			RObject *, ItPosition &, OCTBUF &) const;
		virtual bool generate(WControl &, WObject *, OCTBUF &) const;
};

class McUpp_VRRPvX: public McUpp_VRRP {
	protected:
		McUpp_VRRPvX(CSTR);

	public:
		virtual ~McUpp_VRRPvX();
		static McUpp_VRRPvX *create(CSTR);
};

class McUpp_VRRPv2: public McUpp_VRRP {
	protected:
		McUpp_VRRPv2(CSTR);

	public:
		virtual ~McUpp_VRRPv2();
		static McUpp_VRRPv2 *create(CSTR);
                int32_t vrrpVersion() const { return(2); }
		DEC_HC_MLC(Address);
};

class McUpp_VRRPv3: public McUpp_VRRP {
	protected:
		McUpp_VRRPv3(CSTR);

	public:
		virtual ~McUpp_VRRPv3();
		static McUpp_VRRPv3 *create(CSTR);
                int32_t vrrpVersion() const { return(3); }
		DEC_HC_MLC(Address);
};

class MmHeader_onVRRP: public MmReference_Must1 {
	static TypevsMcDict dict_;

	public:
		MmHeader_onVRRP(CSTR);
		virtual ~MmHeader_onVRRP();

		int32_t token() const { return(metaToken(tkn_header_ref_)); }
		const TypevsMcDict *get_dict() const { return(&dict_); }
		static void add(McUpp_VRRP *mc);
		static void add_other(McUpp_VRRP *mc);
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
};
#endif	// __McVRRP_h__

/*
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011
 * Yokogawa Electric Corporation, 
 * INTAP (Interoperability Technology Association for Information Processing, Japan).
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
 * $TAHI: v6eval/lib/Pz/McDHCPv6.h,v 1.11 2005/05/09 09:35:23 akisada Exp $
 */

#if !defined(__McDHCPv6_h__)
#define	__McDHCPv6_h__	1

#include "PvOctets.h"

#include "McSub.h"
#include "McUDP.h"

const int32_t TP_DHCPv6_Solicit			= 1;
const int32_t TP_DHCPv6_Advertise		= 2;
const int32_t TP_DHCPv6_Request			= 3;
const int32_t TP_DHCPv6_Confirm			= 4;
const int32_t TP_DHCPv6_Renew			= 5;
const int32_t TP_DHCPv6_Rebind			= 6;
const int32_t TP_DHCPv6_Reply			= 7;
const int32_t TP_DHCPv6_Release			= 8;
const int32_t TP_DHCPv6_Decline			= 9;
const int32_t TP_DHCPv6_Reconfigure		= 10;
const int32_t TP_DHCPv6_InformationRequest	= 11;
const int32_t TP_DHCPv6_RelayForward		= 12;
const int32_t TP_DHCPv6_RelayReply		= 13;

const int32_t TP_Opt_DHCPv6_CID			= 1;
const int32_t TP_Opt_DHCPv6_SID			= 2;
const int32_t TP_Opt_DHCPv6_IA_NA		= 3;
const int32_t TP_Opt_DHCPv6_IA_TA		= 4;
const int32_t TP_Opt_DHCPv6_IA_Address		= 5;
const int32_t TP_Opt_DHCPv6_OptionRequest	= 6;
const int32_t TP_Opt_DHCPv6_Preference		= 7;
const int32_t TP_Opt_DHCPv6_ElapsedTime		= 8;
const int32_t TP_Opt_DHCPv6_RelayMessage	= 9;

const int32_t TP_Opt_DHCPv6_Authentication	= 11;
const int32_t TP_Opt_DHCPv6_ServerUnicast	= 12;
const int32_t TP_Opt_DHCPv6_StatusCode		= 13;
const int32_t TP_Opt_DHCPv6_RapidCommit		= 14;
const int32_t TP_Opt_DHCPv6_UserClass		= 15;
const int32_t TP_Opt_DHCPv6_VendorClass		= 16;
const int32_t TP_Opt_DHCPv6_VendorSpecificInfo	= 17;
const int32_t TP_Opt_DHCPv6_IID			= 18;
const int32_t TP_Opt_DHCPv6_ReconfigureMessage	= 19;
const int32_t TP_Opt_DHCPv6_ReconfigureAccept	= 20;

const int32_t TP_Opt_DHCPv6_IA_PD	= 25;
const int32_t TP_Opt_DHCPv6_IA_Prefix	= 26;

const int32_t TP_Opt_DHCPv6_DNS_Servers		= 23;
const int32_t TP_Opt_DHCPv6_DNS_SearchList	= 24;

#ifndef TBD_OPT_NTP_SERVERS
#define TBD_OPT_NTP_SERVERS	27
#endif	// TBD_OPT_NTP_SERVERS
const int32_t TP_Opt_DHCPv6_NTP_Servers		= TBD_OPT_NTP_SERVERS;
#undef TBD_OPT_NTP_SERVERS

#ifndef TBD_OPT_TIME_ZONE
#define TBD_OPT_TIME_ZONE	28
#endif	// TBD_OPT_TIME_ZONE
const int32_t TP_Opt_DHCPv6_NTP_Timezone	= TBD_OPT_TIME_ZONE;
#undef TBD_OPT_TIME_ZONE

const int32_t TP_DHCPv6_DUID_LLT		= 1;
const int32_t TP_DHCPv6_DUID_EN			= 2;
const int32_t TP_DHCPv6_DUID_LL			= 3;

const int32_t TP_DUID_LL_Ether			= 1;

const int32_t TP_DHCPv6_Auth_Delayed		= 2;
const int32_t TP_DHCPv6_Auth_ReconfigureKey	= 3;

////////////////////////////////////////////////////////////////
class PvTimezone: public PvOctets {
	public:
		PvTimezone();
		PvTimezone(uint32_t, OCTSTR = 0, bool = false);
		virtual ~PvTimezone();
		virtual PvObject *shallowCopy() const;
		virtual void print() const;
		virtual void log(uint32_t=0) const;
};

////////////////////////////////////////////////////////////////
class MmTimezone: public MmVarOctets {
	public:
		MmTimezone(CSTR, const PObject *, const PObject *);
		virtual ~MmTimezone();
		virtual PvObject *reversePv(RControl &, const ItPosition &, const ItPosition &, const OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class PaDHCPAuth;
class RmDHCPAuth: public RmObject {
	const PaDHCPAuth *dhcpauth_;
	PvObject *calc_pvalue_;

	public:
		RmDHCPAuth(RObject *, const MObject *, const ItPosition &, const ItPosition &, PvObject *, const PaDHCPAuth *);
		virtual ~RmDHCPAuth();
		void set_calc_pvalue(PvObject *);

		PvObject *calc_pvalue() const{
			return(calc_pvalue_);
		}

		virtual void post_reverse(Con_IPinfo &, RControl &, RObject *);
};

////////////////////////////////////////////////////////////////
class WmDHCPAuth: public WmObject {
	const PaDHCPAuth *dhcpauth_;

	public:
		WmDHCPAuth(WObject *, const MObject *, const PObject *, const PaDHCPAuth *);
		virtual ~WmDHCPAuth();
		virtual void post_generate(Con_IPinfo &, WControl &, OCTBUF &, WObject *);
		virtual bool doEvaluate(WControl &, RObject &);
};

////////////////////////////////////////////////////////////////
class PfDHCPAuth;
class PaDHCPAuth: public PcObject {
	private:
		PfDHCPAuth *dhcpauth_;
	public:
		PaDHCPAuth(const MObject*, CSTR, int);
		virtual ~PaDHCPAuth();

		virtual PObject *dhcpauth_member(PObject *);

		virtual OCTBUF *DHCPAuth_Calculate(const OCTBUF &) const;
		virtual WObject *selfCompose(WControl &, WObject *) const;
};

////////////////////////////////////////////////////////////////
class PfDHCPAuth: public PvFunction {
	private:
		const MfDHCPAuth *meta_;
		OCTSTR context_;
	public:
		PfDHCPAuth(const MfDHCPAuth *, CSTR, int);
		virtual ~PfDHCPAuth();
		const MfDHCPAuth *metaClass() const;
		virtual const MObject *meta() const;
		void init();
		void update(const OCTBUF &);
		PvOctets *result();
};

inline const MfDHCPAuth *PfDHCPAuth::metaClass() const {
	return(meta_);
}

////////////////////////////////////////////////////////////////
class PvDHCPAuth: public PvOctets {
	public:
		PvDHCPAuth(OCTSTR = 0, bool = false);
		virtual ~PvDHCPAuth();
		virtual void log(uint32_t=0) const;
		virtual void print() const;
		static  const PvDHCPAuth *zeroauth();
		virtual PvObject *shallowCopy() const;
};

////////////////////////////////////////////////////////////////
class DHCPAuthList;
class KeepDHCPAuth {
	private:
		DHCPAuthList *dhcpauth_;
		uint32_t current_;
	public:
		KeepDHCPAuth();
		virtual ~KeepDHCPAuth();

		const PaDHCPAuth *GetDHCPAuth(void);
		void SetDHCPAuth(const PaDHCPAuth *);
};

////////////////////////////////////////////////////////////////
class MmDHCPAuth: public MmOctets {
	private:
		KeepDHCPAuth *keepdhcpauth_;
	public:
		MmDHCPAuth(CSTR);
		virtual ~MmDHCPAuth();

		virtual PvObject *reversePv(RControl &, const ItPosition &, const ItPosition &, const OCTBUF &) const;
		virtual RObject *reverseRm(RControl &, RObject *, const ItPosition &, const ItPosition &, PvObject *) const;
		virtual WObject *composeWm(WControl &, WObject *, const PObject *) const;
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
		virtual bool generate(WControl &, WObject *, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class MmDHCPRealm: public MmVarOctets {
	public:
		MmDHCPRealm(CSTR, const PObject *, const PObject *);
		virtual ~MmDHCPRealm();

		virtual ItPosition length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class McUdp_DHCPv6_ONE: public McHeader {
	private:
		static McUdp_DHCPv6_ONE *instance_;
		McUdp_DHCPv6_ONE(CSTR);
	public:
		virtual ~McUdp_DHCPv6_ONE();
		static McUdp_DHCPv6_ONE *instance();

		int32_t upperPort() const {
			return(TP_Udp_DHCPv6_SV);
		}

		bool containsMc(const MObject *mc) const;

		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class McUdp_DHCPv6: public McHeader {
	protected:
		const MmUint *type_;

		void type_member(MmUint *meta) {
			type_ = meta;
			member(meta);
		}

		void common_member();
		McUdp_DHCPv6(CSTR);
	public:
		const MmUint *get_type() const {
			return(type_);
		}

		virtual ~McUdp_DHCPv6();
		virtual int32_t token() const;

		int32_t upperPort() const {
			return(TP_Udp_DHCPv6_SV);
		}

		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;

		DEC_HCGENE(Type);
		DEC_HCEVAL(Type);
		int32_t get_dhcpv6Type(WObject *) const;
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
		virtual bool generate(WControl &, WObject *, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class McUdp_DHCPv6_ANY: public McUdp_DHCPv6 {
	public:
		McUdp_DHCPv6_ANY(CSTR);
		virtual ~McUdp_DHCPv6_ANY();
		static McUdp_DHCPv6_ANY *create(CSTR);
};

////////////////////////////////////////////////////////////////
class McUdp_DHCPv6_Solicit: public McUdp_DHCPv6 {
	public:
		McUdp_DHCPv6_Solicit(CSTR);
		virtual ~McUdp_DHCPv6_Solicit();
		static McUdp_DHCPv6_Solicit *create(CSTR);

		int32_t dhcpv6Type() const {
			return(TP_DHCPv6_Solicit);
		}
};

////////////////////////////////////////////////////////////////
class McUdp_DHCPv6_Advertise: public McUdp_DHCPv6 {
	public:
		McUdp_DHCPv6_Advertise(CSTR);
		virtual ~McUdp_DHCPv6_Advertise();
		static McUdp_DHCPv6_Advertise *create(CSTR);

		int32_t upperPort() const {
			return(TP_Udp_DHCPv6_CL);
		}

		int32_t dhcpv6Type() const {
			return(TP_DHCPv6_Advertise);
		}
};

////////////////////////////////////////////////////////////////
class McUdp_DHCPv6_Request: public McUdp_DHCPv6 {
	public:
		McUdp_DHCPv6_Request(CSTR);
		virtual ~McUdp_DHCPv6_Request();
		static McUdp_DHCPv6_Request *create(CSTR);

		int32_t dhcpv6Type() const {
			return(TP_DHCPv6_Request);
		}
};

////////////////////////////////////////////////////////////////
class McUdp_DHCPv6_Confirm: public McUdp_DHCPv6 {
	public:
		McUdp_DHCPv6_Confirm(CSTR);
		virtual ~McUdp_DHCPv6_Confirm();
		static McUdp_DHCPv6_Confirm *create(CSTR);

		int32_t dhcpv6Type() const {
			return(TP_DHCPv6_Confirm);
		}
};

////////////////////////////////////////////////////////////////
class McUdp_DHCPv6_Renew: public McUdp_DHCPv6 {
	public:
		McUdp_DHCPv6_Renew(CSTR);
		virtual ~McUdp_DHCPv6_Renew();
		static McUdp_DHCPv6_Renew *create(CSTR);

		int32_t dhcpv6Type() const {
			return(TP_DHCPv6_Renew);
		}
};

////////////////////////////////////////////////////////////////
class McUdp_DHCPv6_Rebind: public McUdp_DHCPv6 {
	public:
		McUdp_DHCPv6_Rebind(CSTR);
		virtual ~McUdp_DHCPv6_Rebind();
		static McUdp_DHCPv6_Rebind *create(CSTR);

		int32_t dhcpv6Type() const {
			return(TP_DHCPv6_Rebind);
		}
};

////////////////////////////////////////////////////////////////
class McUdp_DHCPv6_Reply: public McUdp_DHCPv6 {
	public:
		McUdp_DHCPv6_Reply(CSTR);
		virtual ~McUdp_DHCPv6_Reply();
		static McUdp_DHCPv6_Reply *create(CSTR);

		int32_t upperPort() const {
			return(TP_Udp_DHCPv6_CL);
		}

		int32_t dhcpv6Type() const {
			return(TP_DHCPv6_Reply);
		}
};

////////////////////////////////////////////////////////////////
class McUdp_DHCPv6_Release: public McUdp_DHCPv6 {
	public:
		McUdp_DHCPv6_Release(CSTR);
		virtual ~McUdp_DHCPv6_Release();
		static McUdp_DHCPv6_Release *create(CSTR);

		int32_t dhcpv6Type() const {
			return(TP_DHCPv6_Release);
		}
};

////////////////////////////////////////////////////////////////
class McUdp_DHCPv6_Decline: public McUdp_DHCPv6 {
	public:
		McUdp_DHCPv6_Decline(CSTR);
		virtual ~McUdp_DHCPv6_Decline();
		static McUdp_DHCPv6_Decline *create(CSTR);

		int32_t dhcpv6Type() const {
			return(TP_DHCPv6_Decline);
		}
};

////////////////////////////////////////////////////////////////
class McUdp_DHCPv6_Reconfigure: public McUdp_DHCPv6 {
	public:
		McUdp_DHCPv6_Reconfigure(CSTR);
		virtual ~McUdp_DHCPv6_Reconfigure();
		static McUdp_DHCPv6_Reconfigure *create(CSTR);

		int32_t upperPort() const {
			return(TP_Udp_DHCPv6_CL);
		}

		int32_t dhcpv6Type() const {
			return(TP_DHCPv6_Reconfigure);
		}
};

////////////////////////////////////////////////////////////////
class McUdp_DHCPv6_InformationRequest: public McUdp_DHCPv6 {
	public:
		McUdp_DHCPv6_InformationRequest(CSTR);
		virtual ~McUdp_DHCPv6_InformationRequest();
		static McUdp_DHCPv6_InformationRequest *create(CSTR);

		int32_t dhcpv6Type() const {
			return(TP_DHCPv6_InformationRequest);
		}
};

////////////////////////////////////////////////////////////////
class McUdp_DHCPv6_RelayForward: public McUdp_DHCPv6 {
	public:
		McUdp_DHCPv6_RelayForward(CSTR);
		virtual ~McUdp_DHCPv6_RelayForward();
		static McUdp_DHCPv6_RelayForward *create(CSTR);

		int32_t dhcpv6Type() const {
			return(TP_DHCPv6_RelayForward);
		}
};

////////////////////////////////////////////////////////////////
class McUdp_DHCPv6_RelayReply: public McUdp_DHCPv6 {
	public:
		McUdp_DHCPv6_RelayReply(CSTR);
		virtual ~McUdp_DHCPv6_RelayReply();
		static McUdp_DHCPv6_RelayReply *create(CSTR);

		int32_t dhcpv6Type() const {
			return(TP_DHCPv6_RelayReply);
		}
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6: public McOption {
	protected:
		MmUint *code_;

		void code_member(MmUint *meta) {
			code_ = meta;
			member(meta);
		}

		MmUint *length_;

		void length_member(MmUint *meta) {
			length_ = meta;
			member(meta);
		}

		void common_member();
		McOpt_DHCPv6(CSTR);
	public:
		virtual ~McOpt_DHCPv6();

		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
		DEC_HCGENE(Length);

		DEC_HCGENE(Code);
		DEC_HCEVAL(Code);
		int32_t get_optionCode(WObject *) const;
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_ANY: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_ANY(CSTR);
		virtual ~McOpt_DHCPv6_ANY();
		static McOpt_DHCPv6_ANY *create(CSTR);
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_CID: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_CID(CSTR);
		virtual ~McOpt_DHCPv6_CID();
		static McOpt_DHCPv6_CID *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_CID);
		}
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_SID: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_SID(CSTR);
		virtual ~McOpt_DHCPv6_SID();
		static McOpt_DHCPv6_SID *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_SID);
		}
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_IA_NA: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_IA_NA(CSTR);
		virtual ~McOpt_DHCPv6_IA_NA();
		static McOpt_DHCPv6_IA_NA *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_IA_NA);
		}
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_IA_TA: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_IA_TA(CSTR);
		virtual ~McOpt_DHCPv6_IA_TA();
		static McOpt_DHCPv6_IA_TA *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_IA_TA);
		}
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_IA_Address: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_IA_Address(CSTR);
		virtual ~McOpt_DHCPv6_IA_Address();
		static McOpt_DHCPv6_IA_Address *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_IA_Address);
		}
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_OptionRequest: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_OptionRequest(CSTR);
		virtual ~McOpt_DHCPv6_OptionRequest();
		static McOpt_DHCPv6_OptionRequest *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_OptionRequest);
		}

		DEC_HC_MLC(OptionCode);
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_Preference: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_Preference(CSTR);
		virtual ~McOpt_DHCPv6_Preference();
		static McOpt_DHCPv6_Preference *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_Preference);
		}
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_ElapsedTime: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_ElapsedTime(CSTR);
		virtual ~McOpt_DHCPv6_ElapsedTime();
		static McOpt_DHCPv6_ElapsedTime *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_ElapsedTime);
		}
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_RelayMessage: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_RelayMessage(CSTR);
		virtual ~McOpt_DHCPv6_RelayMessage();
		static McOpt_DHCPv6_RelayMessage *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_RelayMessage);
		}
};

////////////////////////////////////////////////////////////////
class AuthInfo {
	uint32_t protocol_;

	public:
		AuthInfo(): protocol_(0) {};
		virtual ~AuthInfo() {};

		void set_protocol(uint32_t);
		uint32_t get_protocol(void) const;
};

inline void AuthInfo::set_protocol(uint32_t d) {protocol_ = d; return;}
inline uint32_t AuthInfo::get_protocol(void) const {return(protocol_);}

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_Authentication: public McOpt_DHCPv6 {
	protected:
		const MmUint *proto_;

		void proto_member(MmUint *meta) {
			proto_ = meta;
			member(meta);
		}

		AuthInfo *AuthInfo_;

	public:
		McOpt_DHCPv6_Authentication(CSTR);
		virtual ~McOpt_DHCPv6_Authentication();
		static McOpt_DHCPv6_Authentication *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_Authentication);
		}

		void nextAuth_set_Control(RControl &, RObject *) const;
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
		AuthInfo *get_auth_info(void) const;

		DEC_HCGENE(Protocol);
		DEC_HCEVAL(Protocol);
		int32_t get_authProto(WObject *) const;
};

inline AuthInfo *McOpt_DHCPv6_Authentication::get_auth_info(void) const {return(AuthInfo_);}

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_ServerUnicast: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_ServerUnicast(CSTR);
		virtual ~McOpt_DHCPv6_ServerUnicast();
		static McOpt_DHCPv6_ServerUnicast *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_ServerUnicast);
		}
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_StatusCode: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_StatusCode(CSTR);
		virtual ~McOpt_DHCPv6_StatusCode();
		static McOpt_DHCPv6_StatusCode *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_StatusCode);
		}
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_RapidCommit: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_RapidCommit(CSTR);
		virtual ~McOpt_DHCPv6_RapidCommit();
		static McOpt_DHCPv6_RapidCommit *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_RapidCommit);
		}
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_UserClass: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_UserClass(CSTR);
		virtual ~McOpt_DHCPv6_UserClass();
		static McOpt_DHCPv6_UserClass *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_UserClass);
		}
};

////////////////////////////////////////////////////////////////
class McDHCPv6_UserClassData: public McOption {
	protected:
		MmUint *length_;

		void length_member(MmUint *meta) {
			length_ = meta;
			member(meta);
		}

		void common_member();
		McDHCPv6_UserClassData(CSTR);
	public:
		virtual ~McDHCPv6_UserClassData();
		static McDHCPv6_UserClassData *create(CSTR);

		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		DEC_HCGENE(Length);
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_VendorClass: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_VendorClass(CSTR);
		virtual ~McOpt_DHCPv6_VendorClass();
		static McOpt_DHCPv6_VendorClass *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_VendorClass);
		}
};

////////////////////////////////////////////////////////////////
class McDHCPv6_VendorClassData: public McOption {
	protected:
		MmUint *length_;

		void length_member(MmUint *meta) {
			length_ = meta;
			member(meta);
		}

		void common_member();
		McDHCPv6_VendorClassData(CSTR);
	public:
		virtual ~McDHCPv6_VendorClassData();
		static McDHCPv6_VendorClassData *create(CSTR);

		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		DEC_HCGENE(Length);
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_VendorSpecificInfo: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_VendorSpecificInfo(CSTR);
		virtual ~McOpt_DHCPv6_VendorSpecificInfo();
		static McOpt_DHCPv6_VendorSpecificInfo *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_VendorSpecificInfo);
		}
};

////////////////////////////////////////////////////////////////
class McDHCPv6_OptionData: public McOption {
	protected:
		MmUint *length_;

		void length_member(MmUint *meta) {
			length_ = meta;
			member(meta);
		}

		void common_member();
		McDHCPv6_OptionData(CSTR);
	public:
		virtual ~McDHCPv6_OptionData();
		static McDHCPv6_OptionData *create(CSTR);

		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		DEC_HCGENE(Length);
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_IID: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_IID(CSTR);
		virtual ~McOpt_DHCPv6_IID();
		static McOpt_DHCPv6_IID *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_IID);
		}
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_ReconfigureMessage: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_ReconfigureMessage(CSTR);
		virtual ~McOpt_DHCPv6_ReconfigureMessage();
		static McOpt_DHCPv6_ReconfigureMessage *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_ReconfigureMessage);
		}
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_ReconfigureAccept: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_ReconfigureAccept(CSTR);
		virtual ~McOpt_DHCPv6_ReconfigureAccept();
		static McOpt_DHCPv6_ReconfigureAccept *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_ReconfigureAccept);
		}
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_IA_PD: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_IA_PD(CSTR);
		virtual ~McOpt_DHCPv6_IA_PD();
		static McOpt_DHCPv6_IA_PD *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_IA_PD);
		}
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_IA_Prefix: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_IA_Prefix(CSTR);
		virtual ~McOpt_DHCPv6_IA_Prefix();
		static McOpt_DHCPv6_IA_Prefix *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_IA_Prefix);
		}
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_DNS_Servers: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_DNS_Servers(CSTR);
		virtual ~McOpt_DHCPv6_DNS_Servers();
		static McOpt_DHCPv6_DNS_Servers *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_DNS_Servers);
		}

		DEC_HC_MLC(Address);
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_DNS_SearchList: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_DNS_SearchList(CSTR);
		virtual ~McOpt_DHCPv6_DNS_SearchList();
		static McOpt_DHCPv6_DNS_SearchList *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_DNS_SearchList);
		}
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_NTP_Servers: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_NTP_Servers(CSTR);
		virtual ~McOpt_DHCPv6_NTP_Servers();
		static McOpt_DHCPv6_NTP_Servers *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_NTP_Servers);
		}

		DEC_HC_MLC(Address);
};

////////////////////////////////////////////////////////////////
class McOpt_DHCPv6_NTP_Timezone: public McOpt_DHCPv6 {
	public:
		McOpt_DHCPv6_NTP_Timezone(CSTR);
		virtual ~McOpt_DHCPv6_NTP_Timezone();
		static McOpt_DHCPv6_NTP_Timezone *create(CSTR);

		int32_t optionCode() const {
			return(TP_Opt_DHCPv6_NTP_Timezone);
		}
};

////////////////////////////////////////////////////////////////
class McDHCPv6_DUID: public McUpper {
	protected:
		MmUint *type_;

		void type_member(MmUint *meta) {
			type_ = meta;
			member(meta);
		}

		MmUint *hwtype_;

		void hwtype_member(MmUint *meta) {
			hwtype_ = meta;
			member(meta);
		}

		void common_member();
		McDHCPv6_DUID(CSTR);
	public:
		virtual ~McDHCPv6_DUID();

		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;

		DEC_HCGENE(Type);
		DEC_HCEVAL(Type);
		int32_t get_duidType(WObject *) const;
};

class McDHCPv6_DUID_ANY: public McDHCPv6_DUID {
	public:
		McDHCPv6_DUID_ANY(CSTR);
		virtual ~McDHCPv6_DUID_ANY();
		static McDHCPv6_DUID_ANY *create(CSTR);
};

////////////////////////////////////////////////////////////////
class McDHCPv6_DUID_LLT_ONE: public McDHCPv6_DUID {
	private:
		static McDHCPv6_DUID_LLT_ONE *instance_;
		McDHCPv6_DUID_LLT_ONE(CSTR);
	public:
		virtual ~McDHCPv6_DUID_LLT_ONE();
		static McDHCPv6_DUID_LLT_ONE *instance();

		int32_t duidType() const {
			return(TP_DHCPv6_DUID_LLT);
		}

		bool containsMc(const MObject *mc) const;

		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class McDHCPv6_DUID_LLT: public McDHCPv6_DUID {
	protected:
		McDHCPv6_DUID_LLT(CSTR);
	public:
		virtual ~McDHCPv6_DUID_LLT();

		int32_t duidType() const {
			return(TP_DHCPv6_DUID_LLT);
		}

		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;

		DEC_HCGENE(HardwareType);
		DEC_HCEVAL(HardwareType);
		int32_t get_hardwareType(WObject *) const;
};

////////////////////////////////////////////////////////////////
class McDHCPv6_DUID_LLT_ANY: public McDHCPv6_DUID_LLT {
	public:
		McDHCPv6_DUID_LLT_ANY(CSTR);
		virtual ~McDHCPv6_DUID_LLT_ANY();
		static McDHCPv6_DUID_LLT_ANY *create(CSTR);
};

////////////////////////////////////////////////////////////////
class McDHCPv6_DUID_LLT_Ether: public McDHCPv6_DUID_LLT {
	public:
		McDHCPv6_DUID_LLT_Ether(CSTR);
		virtual ~McDHCPv6_DUID_LLT_Ether();
		static McDHCPv6_DUID_LLT_Ether *create(CSTR);

		int32_t hardwareType() const {
			return(TP_DUID_LL_Ether);
		}
};

////////////////////////////////////////////////////////////////
class McDHCPv6_DUID_EN: public McDHCPv6_DUID {
	public:
		McDHCPv6_DUID_EN(CSTR);
		virtual ~McDHCPv6_DUID_EN();
		static McDHCPv6_DUID_EN *create(CSTR);

		int32_t duidType() const {
			return(TP_DHCPv6_DUID_EN);
		}
};

////////////////////////////////////////////////////////////////
class McDHCPv6_DUID_LL_ONE: public McDHCPv6_DUID {
	private:
		static McDHCPv6_DUID_LL_ONE *instance_;
		McDHCPv6_DUID_LL_ONE(CSTR);
	public:
		virtual ~McDHCPv6_DUID_LL_ONE();
		static McDHCPv6_DUID_LL_ONE *instance();

		int32_t duidType() const {
			return(TP_DHCPv6_DUID_LL);
		}

		bool containsMc(const MObject *mc) const;

		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class McDHCPv6_DUID_LL: public McDHCPv6_DUID {
	protected:
		McDHCPv6_DUID_LL(CSTR);
	public:
		virtual ~McDHCPv6_DUID_LL();

		int32_t duidType() const {
			return(TP_DHCPv6_DUID_LL);
		}

		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;

		DEC_HCGENE(HardwareType);
		DEC_HCEVAL(HardwareType);
		int32_t get_hardwareType(WObject *) const;
};

////////////////////////////////////////////////////////////////
class McDHCPv6_DUID_LL_ANY: public McDHCPv6_DUID_LL {
	public:
		McDHCPv6_DUID_LL_ANY(CSTR);
		virtual ~McDHCPv6_DUID_LL_ANY();
		static McDHCPv6_DUID_LL_ANY *create(CSTR);
};

////////////////////////////////////////////////////////////////
class McDHCPv6_DUID_LL_Ether: public McDHCPv6_DUID_LL {
	public:
		McDHCPv6_DUID_LL_Ether(CSTR);
		virtual ~McDHCPv6_DUID_LL_Ether();
		static McDHCPv6_DUID_LL_Ether *create(CSTR);

		int32_t hardwareType() const {
			return(TP_DUID_LL_Ether);
		}
};

////////////////////////////////////////////////////////////////
class McDHCPv6_Auth: public McUpper {
	public:
		McDHCPv6_Auth(CSTR);
		virtual ~McDHCPv6_Auth();
		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class McDHCPv6_Auth_ANY: public McDHCPv6_Auth {
	public:
		McDHCPv6_Auth_ANY(CSTR);
		virtual ~McDHCPv6_Auth_ANY();
		static McDHCPv6_Auth_ANY *create(CSTR);
};

////////////////////////////////////////////////////////////////
class McDHCPv6_Auth_Delayed: public McDHCPv6_Auth {
	public:
		McDHCPv6_Auth_Delayed(CSTR);
		virtual ~McDHCPv6_Auth_Delayed();
		static McDHCPv6_Auth_Delayed *create(CSTR);

		virtual PObject *tokenObject(int, CSTR) const;

		int32_t authProto() const {
			return(TP_DHCPv6_Auth_Delayed);
		}
};

////////////////////////////////////////////////////////////////
class McDHCPv6_Auth_ReconfigureKey: public McDHCPv6_Auth {
	public:
		McDHCPv6_Auth_ReconfigureKey(CSTR);
		virtual ~McDHCPv6_Auth_ReconfigureKey();
		static McDHCPv6_Auth_ReconfigureKey *create(CSTR);

		virtual PObject *tokenObject(int, CSTR) const;

		int32_t authProto() const {
			return(TP_DHCPv6_Auth_ReconfigureKey);
		}
};

////////////////////////////////////////////////////////////////
class MmHeader_onDHCPv6: public MmReference_Less1 {
	static TypevsMcDict dict_;

	public:
		MmHeader_onDHCPv6(CSTR);
		virtual ~MmHeader_onDHCPv6();

		int32_t token() const {
			return(metaToken(tkn_payload_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add(McUdp_DHCPv6 *mc);
		static void add_other(McUdp_DHCPv6 *mc);

		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class MmOption_onDHCPv6: public MmReference_More0 {
	static TypevsMcDict dict_;

	public:
		MmOption_onDHCPv6(CSTR);
		virtual ~MmOption_onDHCPv6();

		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add(McOpt_DHCPv6 *mc);
		static void add_other(McOpt_DHCPv6 *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class MmDUID_onOption: public MmReference_Less1 {
	static TypevsMcDict dict_;

	public:
		MmDUID_onOption(CSTR);
		virtual ~MmDUID_onOption();

		int32_t token() const {
			return(metaToken(tkn_payload_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add(McDHCPv6_DUID *mc);
		static void add_other(McDHCPv6_DUID *mc);

		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class MmLLT_onDUID: public MmReference_Must1 {
	static TypevsMcDict dict_;

	public:
		MmLLT_onDUID(CSTR);
		virtual ~MmLLT_onDUID();

		int32_t token() const {
			return(metaToken(tkn_payload_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add(McDHCPv6_DUID_LLT *mc);
		static void add_other(McDHCPv6_DUID_LLT *mc);

		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class MmLL_onDUID: public MmReference_Must1 {
	static TypevsMcDict dict_;

	public:
		MmLL_onDUID(CSTR);
		virtual ~MmLL_onDUID();

		int32_t token() const {
			return(metaToken(tkn_payload_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add(McDHCPv6_DUID_LL *mc);
		static void add_other(McDHCPv6_DUID_LL *mc);

		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class MmUserClass_onOption: public MmReference_More0 {
	static TypevsMcDict dict_;

	public:
		MmUserClass_onOption(CSTR);
		virtual ~MmUserClass_onOption();

		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add_other(McDHCPv6_UserClassData *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class MmVendorClass_onOption: public MmReference_More0 {
	static TypevsMcDict dict_;

	public:
		MmVendorClass_onOption(CSTR);
		virtual ~MmVendorClass_onOption();

		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add_other(McDHCPv6_VendorClassData *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class MmOptionData_onOption: public MmReference_More0 {
	static TypevsMcDict dict_;

	public:
		MmOptionData_onOption(CSTR);
		virtual ~MmOptionData_onOption();

		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add_other(McDHCPv6_OptionData *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class MmProto_onAuth: public MmReference_Must1 {
	static TypevsMcDict dict_;
	McOpt_DHCPv6_Authentication *opt_auth_;

	public:
		MmProto_onAuth(CSTR, McOpt_DHCPv6_Authentication *);
		virtual ~MmProto_onAuth();

		int32_t token() const {
			return(metaToken(tkn_payload_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add_other(McDHCPv6_Auth *mc);
		static void add(McDHCPv6_Auth *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
		McOpt_DHCPv6_Authentication *get_opt_auth(void) const;
};

inline McOpt_DHCPv6_Authentication *MmProto_onAuth::get_opt_auth(void) const {return(opt_auth_);}

interfaceCmList(DHCPAuthList, PaDHCPAuth);
#endif

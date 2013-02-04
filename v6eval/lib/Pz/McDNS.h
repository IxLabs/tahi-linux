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
 * $TAHI: v6eval/lib/Pz/McDNS.h,v 1.5 2005/05/09 09:35:23 akisada Exp $
 */

#if !defined(__McDNS_h__)
#define	__McDNS_h__	1

#include "McSub.h"
#include "McUDP.h"
#include "MmObject.h"
#include "PvOctets.h"

// RFC 1035 - Domain Implementation and Specification
const int32_t TP_DNS_RR_A	= 1;
const int32_t TP_DNS_RR_NS	= 2;
const int32_t TP_DNS_RR_MD	= 3;
const int32_t TP_DNS_RR_MF	= 4;
const int32_t TP_DNS_RR_CNAME	= 5;
const int32_t TP_DNS_RR_SOA	= 6;
const int32_t TP_DNS_RR_MB	= 7;
const int32_t TP_DNS_RR_MG	= 8;
const int32_t TP_DNS_RR_MR	= 9;
const int32_t TP_DNS_RR_NULL	= 10;
const int32_t TP_DNS_RR_WKS	= 11;
const int32_t TP_DNS_RR_PTR	= 12;
const int32_t TP_DNS_RR_HINFO	= 13;
const int32_t TP_DNS_RR_MINFO	= 14;
const int32_t TP_DNS_RR_MX	= 15;
const int32_t TP_DNS_RR_TXT	= 16;

// RFC 1183 - New DNS RR Definitions
const int32_t TP_DNS_RR_RP	= 17;
const int32_t TP_DNS_RR_AFSDB	= 18;
const int32_t TP_DNS_RR_X25	= 19;
const int32_t TP_DNS_RR_ISDN	= 20;
const int32_t TP_DNS_RR_RT	= 21;

// RFC 1664 - Internet DNS for Mail Mapping Tables
const int32_t TP_DNS_RR_PX	= 26;

// RFC 1886 - IPv6 DNS Extensions
const int32_t TP_DNS_RR_AAAA	= 28;

// RFC 2782 - DNS SRV RR
const int32_t TP_DNS_RR_SRV	= 33;

// RFC 2915 - NAPTR DNS RR
const int32_t TP_DNS_RR_NAPTR	= 35;

////////////////////////////////////////////////////////////////
class RmQType: public RmObject {
	public:
		RmQType(RObject *, const MObject *, const ItPosition &, const ItPosition &, PvObject * = 0);
		virtual ~RmQType();
		virtual void printName(uint32_t, CSTR = 0) const;
		virtual void logSelf(uint32_t, CSTR = 0) const;
		void printQType(uint32_t) const;
};

////////////////////////////////////////////////////////////////
class MmQType: public MmUint {
	public:
		MmQType(CSTR, uint16_t, const PObject * = 0, const PObject * = 0, const ICVoverwriter * ow = 0, METH_HC_ForIPinfo meth = 0);
		virtual ~MmQType();

		virtual RObject *reverseRm(RControl &, RObject *, const ItPosition &, const ItPosition &, PvObject *) const;
};

////////////////////////////////////////////////////////////////
class PvDnsName: public PvOctets {
	public:
		PvDnsName();
		PvDnsName(uint32_t, OCTSTR = 0, bool = false);
		virtual ~PvDnsName();
		virtual PvObject *shallowCopy() const;
		virtual void print() const;
		virtual void log(uint32_t=0) const;
};

////////////////////////////////////////////////////////////////
class PvDnsStr: public PvOctets {
	public:
		PvDnsStr();
		PvDnsStr(uint32_t, OCTSTR = 0, bool = false);
		virtual ~PvDnsStr();
		virtual PvObject *shallowCopy() const;
		virtual void print() const;
		virtual void log(uint32_t=0) const;
};

////////////////////////////////////////////////////////////////
class LengthCtl {
	uint32_t length_;
	uint32_t rrlength_;

	public:
		LengthCtl(): length_(0), rrlength_(0) {};
		virtual ~LengthCtl() {};

		void set_length(uint32_t);
		uint32_t get_length(void) const;

		void set_rrlength(uint32_t);
		uint32_t get_rrlength(void) const;
};

inline void LengthCtl::set_length(uint32_t d) {length_ = d; return;}
inline uint32_t LengthCtl::get_length(void) const {return(length_);}

inline void LengthCtl::set_rrlength(uint32_t d) {rrlength_ = d; return;}
inline uint32_t LengthCtl::get_rrlength(void) const {return(rrlength_);}

////////////////////////////////////////////////////////////////
class MmDNSName: public MmVarOctets {
	LengthCtl *LengthCtl_;

	public:
		MmDNSName(CSTR, const PObject *, const PObject *);
		virtual ~MmDNSName();

		virtual ItPosition length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
		virtual PvObject *reversePv(RControl &, const ItPosition &, const ItPosition &, const OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class MmDNSStr: public MmVarOctets {
	LengthCtl *LengthCtl_;

	public:
		MmDNSStr(CSTR, const PObject *, const PObject *);
		virtual ~MmDNSStr();

		virtual ItPosition length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
		virtual PvObject *reversePv(RControl &, const ItPosition &, const ItPosition &, const OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class CountMgr {
	uint32_t generate_QD_;
	uint32_t generate_AN_;
	uint32_t generate_NS_;
	uint32_t generate_AR_;

	uint32_t reverse_QD_;
	uint32_t reverse_AN_;
	uint32_t reverse_NS_;
	uint32_t reverse_AR_;

	uint32_t current_QD_;
	uint32_t current_AN_;
	uint32_t current_NS_;
	uint32_t current_AR_;

	uint32_t length_QD_;
	uint32_t length_AN_;
	uint32_t length_NS_;
	uint32_t length_AR_;

	public:
		CountMgr():
			generate_QD_(0), generate_AN_(0), generate_NS_(0), generate_AR_(0),
			reverse_QD_(0),  reverse_AN_(0),  reverse_NS_(0),  reverse_AR_(0),
			current_QD_(0),  current_AN_(0),  current_NS_(0),  current_AR_(0),
			length_QD_(0),   length_AN_(0),   length_NS_(0),   length_AR_(0) {};

		virtual ~CountMgr() {};

		void increase_generate_QD(void);
		void increase_generate_AN(void);
		void increase_generate_NS(void);
		void increase_generate_AR(void);

		uint32_t get_generate_QD(void) const;
		uint32_t get_generate_AN(void) const;
		uint32_t get_generate_NS(void) const;
		uint32_t get_generate_AR(void) const;

		void set_reverse_QD(uint32_t);
		void set_reverse_AN(uint32_t);
		void set_reverse_NS(uint32_t);
		void set_reverse_AR(uint32_t);

		uint32_t get_reverse_QD(void) const;
		uint32_t get_reverse_AN(void) const;
		uint32_t get_reverse_NS(void) const;
		uint32_t get_reverse_AR(void) const;

		void init_current_QD(void);
		void init_current_AN(void);
		void init_current_NS(void);
		void init_current_AR(void);

		void increase_current_QD(void);
		void increase_current_AN(void);
		void increase_current_NS(void);
		void increase_current_AR(void);

		uint32_t get_current_QD(void) const;
		uint32_t get_current_AN(void) const;
		uint32_t get_current_NS(void) const;
		uint32_t get_current_AR(void) const;

		void set_length_QD(uint32_t);
		void set_length_AN(uint32_t);
		void set_length_NS(uint32_t);
		void set_length_AR(uint32_t);

		uint32_t get_length_QD(void) const;
		uint32_t get_length_AN(void) const;
		uint32_t get_length_NS(void) const;
		uint32_t get_length_AR(void) const;
};

inline void CountMgr::increase_generate_QD(void) {generate_QD_ ++; return;}
inline void CountMgr::increase_generate_AN(void) {generate_AN_ ++; return;}
inline void CountMgr::increase_generate_NS(void) {generate_NS_ ++; return;}
inline void CountMgr::increase_generate_AR(void) {generate_AR_ ++; return;}

inline uint32_t CountMgr::get_generate_QD(void) const {return(generate_QD_);}
inline uint32_t CountMgr::get_generate_AN(void) const {return(generate_AN_);}
inline uint32_t CountMgr::get_generate_NS(void) const {return(generate_NS_);}
inline uint32_t CountMgr::get_generate_AR(void) const {return(generate_AR_);}

inline void CountMgr::set_reverse_QD(uint32_t d) {reverse_QD_ = d; return;}
inline void CountMgr::set_reverse_AN(uint32_t d) {reverse_AN_ = d; return;}
inline void CountMgr::set_reverse_NS(uint32_t d) {reverse_NS_ = d; return;}
inline void CountMgr::set_reverse_AR(uint32_t d) {reverse_AR_ = d; return;}

inline uint32_t CountMgr::get_reverse_QD(void) const {return(reverse_QD_);}
inline uint32_t CountMgr::get_reverse_AN(void) const {return(reverse_AN_);}
inline uint32_t CountMgr::get_reverse_NS(void) const {return(reverse_NS_);}
inline uint32_t CountMgr::get_reverse_AR(void) const {return(reverse_AR_);}

inline void CountMgr::init_current_QD(void) {current_QD_ = 0; return;}
inline void CountMgr::init_current_AN(void) {current_AN_ = 0; return;}
inline void CountMgr::init_current_NS(void) {current_NS_ = 0; return;}
inline void CountMgr::init_current_AR(void) {current_AR_ = 0; return;}

inline void CountMgr::increase_current_QD(void) {current_QD_ ++; return;}
inline void CountMgr::increase_current_AN(void) {current_AN_ ++; return;}
inline void CountMgr::increase_current_NS(void) {current_NS_ ++; return;}
inline void CountMgr::increase_current_AR(void) {current_AR_ ++; return;}

inline uint32_t CountMgr::get_current_QD(void) const {return(current_QD_);}
inline uint32_t CountMgr::get_current_AN(void) const {return(current_AN_);}
inline uint32_t CountMgr::get_current_NS(void) const {return(current_NS_);}
inline uint32_t CountMgr::get_current_AR(void) const {return(current_AR_);}

inline void CountMgr::set_length_QD(uint32_t d) {length_QD_ = d; return;}
inline void CountMgr::set_length_AN(uint32_t d) {length_AN_ = d; return;}
inline void CountMgr::set_length_NS(uint32_t d) {length_NS_ = d; return;}
inline void CountMgr::set_length_AR(uint32_t d) {length_AR_ = d; return;}

inline uint32_t CountMgr::get_length_QD(void) const {return(length_QD_);}
inline uint32_t CountMgr::get_length_AN(void) const {return(length_AN_);}
inline uint32_t CountMgr::get_length_NS(void) const {return(length_NS_);}
inline uint32_t CountMgr::get_length_AR(void) const {return(length_AR_);}

////////////////////////////////////////////////////////////////
class McUdp_DNS: public McHeader {
	static McUdp_DNS *instance_;
	CountMgr *CountMgr_;
	McUdp_DNS(CSTR);

	protected:
		const MmUint *QDCount_;
		const MmUint *ANCount_;
		const MmUint *NSCount_;
		const MmUint *ARCount_;

		void QDCount_member(MmUint *meta) {QDCount_ = meta; member(meta); return;}
		void ANCount_member(MmUint *meta) {ANCount_ = meta; member(meta); return;}
		void NSCount_member(MmUint *meta) {NSCount_ = meta; member(meta); return;}
		void ARCount_member(MmUint *meta) {ARCount_ = meta; member(meta); return;}

	public:
		virtual ~McUdp_DNS();

		static McUdp_DNS *create(CSTR, CSTR, CSTR, CSTR, CSTR);
		static McUdp_DNS *instance() {return instance_;}
		int32_t upperPort() const {return(TP_Udp_DNS);}

		int32_t token() const {
			return(metaToken(tkn_dns_header_));
		}

		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
		void set_CountMgr(RObject *) const;

		CountMgr *get_CountMgr(void) const;

		DEC_HCGENE(QDCount);
		DEC_HCGENE(ANCount);
		DEC_HCGENE(NSCount);
		DEC_HCGENE(ARCount);

		DEC_HCEVAL(QDCount);
		DEC_HCEVAL(ANCount);
		DEC_HCEVAL(NSCount);
		DEC_HCEVAL(ARCount);
};

inline CountMgr *McUdp_DNS::get_CountMgr(void) const {return(CountMgr_);}

////////////////////////////////////////////////////////////////
class McDNS_Question: public McOption {
	McUdp_DNS *McUdp_DNS_;

	McDNS_Question(CSTR);

	public:
		virtual ~McDNS_Question();
		static McDNS_Question *create(CSTR, McUdp_DNS *);

		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class McDNS_Answer: public McOption {
	McUdp_DNS *McUdp_DNS_;

	McDNS_Answer(CSTR);

	public:
		virtual ~McDNS_Answer();
		static McDNS_Answer *create(CSTR, McUdp_DNS *);

		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class McDNS_Authority: public McOption {
	McUdp_DNS *McUdp_DNS_;

	McDNS_Authority(CSTR);

	public:
		virtual ~McDNS_Authority();
		static McDNS_Authority *create(CSTR, McUdp_DNS *);

		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class McDNS_Additional: public McOption {
	McUdp_DNS *McUdp_DNS_;

	McDNS_Additional(CSTR);

	public:
		virtual ~McDNS_Additional();
		static McDNS_Additional *create(CSTR, McUdp_DNS *);

		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class McDNS_QuestionEntry: public McOption {
	LengthCtl *LengthCtl_;

	McDNS_QuestionEntry(CSTR);

	public:
		virtual ~McDNS_QuestionEntry();
		static McDNS_QuestionEntry *create(CSTR);
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class McDNS_RR: public McOption {
	LengthCtl *LengthCtl_;

	protected:
		MmUint *type_;
		MmUint *length_;

		void type_member(MmUint *meta) {
			type_ = meta;
			member(meta);
		}

		void length_member(MmUint *meta) {
			length_ = meta;
			member(meta);
		}

		void common_member();
		McDNS_RR(CSTR);
	public:
		virtual ~McDNS_RR();

		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;

		DEC_HCGENE(Type);
		DEC_HCEVAL(Type);
		int32_t get_rrType(WObject *wmem) const;

		DEC_HCGENE(Length);
};

////////////////////////////////////////////////////////////////
class McDNS_RR_ANY: public McDNS_RR {
	public:
		McDNS_RR_ANY(CSTR);
		virtual ~McDNS_RR_ANY();
		static McDNS_RR_ANY *create(CSTR);
};

////////////////////////////////////////////////////////////////
class McDNS_RR_A: public McDNS_RR {
	public:
		McDNS_RR_A(CSTR);
		virtual ~McDNS_RR_A();
		static McDNS_RR_A *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_A);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_NS: public McDNS_RR {
	public:
		McDNS_RR_NS(CSTR);
		virtual ~McDNS_RR_NS();
		static McDNS_RR_NS *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_NS);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_MD: public McDNS_RR {
	public:
		McDNS_RR_MD(CSTR);
		virtual ~McDNS_RR_MD();
		static McDNS_RR_MD *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_MD);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_MF: public McDNS_RR {
	public:
		McDNS_RR_MF(CSTR);
		virtual ~McDNS_RR_MF();
		static McDNS_RR_MF *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_MF);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_CNAME: public McDNS_RR {
	public:
		McDNS_RR_CNAME(CSTR);
		virtual ~McDNS_RR_CNAME();
		static McDNS_RR_CNAME *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_CNAME);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_SOA: public McDNS_RR {
	public:
		McDNS_RR_SOA(CSTR);
		virtual ~McDNS_RR_SOA();
		static McDNS_RR_SOA *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_SOA);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_MB: public McDNS_RR {
	public:
		McDNS_RR_MB(CSTR);
		virtual ~McDNS_RR_MB();
		static McDNS_RR_MB *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_MB);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_MG: public McDNS_RR {
	public:
		McDNS_RR_MG(CSTR);
		virtual ~McDNS_RR_MG();
		static McDNS_RR_MG *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_MG);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_MR: public McDNS_RR {
	public:
		McDNS_RR_MR(CSTR);
		virtual ~McDNS_RR_MR();
		static McDNS_RR_MR *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_MR);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_NULL: public McDNS_RR {
	public:
		McDNS_RR_NULL(CSTR);
		virtual ~McDNS_RR_NULL();
		static McDNS_RR_NULL *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_NULL);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_WKS: public McDNS_RR {
	public:
		McDNS_RR_WKS(CSTR);
		virtual ~McDNS_RR_WKS();
		static McDNS_RR_WKS *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_WKS);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_PTR: public McDNS_RR {
	public:
		McDNS_RR_PTR(CSTR);
		virtual ~McDNS_RR_PTR();
		static McDNS_RR_PTR *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_PTR);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_HINFO: public McDNS_RR {
	public:
		McDNS_RR_HINFO(CSTR);
		virtual ~McDNS_RR_HINFO();
		static McDNS_RR_HINFO *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_HINFO);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_MINFO: public McDNS_RR {
	public:
		McDNS_RR_MINFO(CSTR);
		virtual ~McDNS_RR_MINFO();
		static McDNS_RR_MINFO *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_MINFO);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_MX: public McDNS_RR {
	public:
		McDNS_RR_MX(CSTR);
		virtual ~McDNS_RR_MX();
		static McDNS_RR_MX *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_MX);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_TXT: public McDNS_RR {
	public:
		McDNS_RR_TXT(CSTR);
		virtual ~McDNS_RR_TXT();
		static McDNS_RR_TXT *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_TXT);
		}

		DEC_HC_MLC(TXT);
};

////////////////////////////////////////////////////////////////
class McDNS_RR_RP: public McDNS_RR {
	public:
		McDNS_RR_RP(CSTR);
		virtual ~McDNS_RR_RP();
		static McDNS_RR_RP *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_RP);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_AFSDB: public McDNS_RR {
	public:
		McDNS_RR_AFSDB(CSTR);
		virtual ~McDNS_RR_AFSDB();
		static McDNS_RR_AFSDB *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_AFSDB);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_X25: public McDNS_RR {
	public:
		McDNS_RR_X25(CSTR);
		virtual ~McDNS_RR_X25();
		static McDNS_RR_X25 *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_X25);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_ISDN: public McDNS_RR {
	public:
		McDNS_RR_ISDN(CSTR);
		virtual ~McDNS_RR_ISDN();
		static McDNS_RR_ISDN *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_ISDN);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_RT: public McDNS_RR {
	public:
		McDNS_RR_RT(CSTR);
		virtual ~McDNS_RR_RT();
		static McDNS_RR_RT *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_RT);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_PX: public McDNS_RR {
	public:
		McDNS_RR_PX(CSTR);
		virtual ~McDNS_RR_PX();
		static McDNS_RR_PX *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_PX);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_AAAA: public McDNS_RR {
	public:
		McDNS_RR_AAAA(CSTR);
		virtual ~McDNS_RR_AAAA();
		static McDNS_RR_AAAA *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_AAAA);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_SRV: public McDNS_RR {
	public:
		McDNS_RR_SRV(CSTR);
		virtual ~McDNS_RR_SRV();
		static McDNS_RR_SRV *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_SRV);
		}
};

////////////////////////////////////////////////////////////////
class McDNS_RR_NAPTR: public McDNS_RR {
	public:
		McDNS_RR_NAPTR(CSTR);
		virtual ~McDNS_RR_NAPTR();
		static McDNS_RR_NAPTR *create(CSTR);

		int32_t rrType() const {
			return(TP_DNS_RR_NAPTR);
		}
};

////////////////////////////////////////////////////////////////
class MmQuestion_onDNS: public MmReference_Less1 {
	static TypevsMcDict dict_;
	McUdp_DNS *McUdp_DNS_;

	public:
		MmQuestion_onDNS(CSTR, McUdp_DNS *);
		virtual ~MmQuestion_onDNS();

		int32_t token() const {
			return(metaToken(tkn_dns_qestion_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add_other(McDNS_Question *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class MmAnswer_onDNS: public MmReference_Less1 {
	static TypevsMcDict dict_;
	McUdp_DNS *McUdp_DNS_;

	public:
		MmAnswer_onDNS(CSTR, McUdp_DNS *);
		virtual ~MmAnswer_onDNS();

		int32_t token() const {
			return(metaToken(tkn_dns_answer_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add_other(McDNS_Answer *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class MmAuthority_onDNS: public MmReference_Less1 {
	static TypevsMcDict dict_;
	McUdp_DNS *McUdp_DNS_;

	public:
		MmAuthority_onDNS(CSTR, McUdp_DNS *);
		virtual ~MmAuthority_onDNS();

		int32_t token() const {
			return(metaToken(tkn_dns_authority_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add_other(McDNS_Authority *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class MmAdditional_onDNS: public MmReference_Less1 {
	static TypevsMcDict dict_;
	McUdp_DNS *McUdp_DNS_;

	public:
		MmAdditional_onDNS(CSTR, McUdp_DNS *);
		virtual ~MmAdditional_onDNS();

		int32_t token() const {
			return(metaToken(tkn_dns_additional_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add_other(McDNS_Additional *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////
class MmEntry_onQuestion: public MmReference_More0 {
	static TypevsMcDict dict_;

	const McUdp_DNS *McUdp_DNS_;

	protected:
		virtual void composeList(WControl &, WObject *, const PObjectList &) const;

	public:
		MmEntry_onQuestion(CSTR, McUdp_DNS *);
		virtual ~MmEntry_onQuestion();

		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add_other(McDNS_QuestionEntry *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;

		virtual WObject *compose(WControl &, WObject *, const PObject *) const;
};

////////////////////////////////////////////////////////////////
class MmRR_onAnswer: public MmReference_More0 {
	static TypevsMcDict dict_;

	const McUdp_DNS *McUdp_DNS_;

	protected:
		virtual void composeList(WControl &, WObject *, const PObjectList &) const;

	public:
		MmRR_onAnswer(CSTR, McUdp_DNS *);
		virtual ~MmRR_onAnswer();

		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add(McDNS_RR *mc);
		static void add_other(McDNS_RR *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;

		virtual WObject *compose(WControl &, WObject *, const PObject *) const;
};

////////////////////////////////////////////////////////////////
class MmRR_onAuthority: public MmReference_More0 {
	static TypevsMcDict dict_;

	const McUdp_DNS *McUdp_DNS_;

	protected:
		virtual void composeList(WControl &, WObject *, const PObjectList &) const;

	public:
		MmRR_onAuthority(CSTR, McUdp_DNS *);
		virtual ~MmRR_onAuthority();

		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add(McDNS_RR *mc);
		static void add_other(McDNS_RR *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;

		virtual WObject *compose(WControl &, WObject *, const PObject *) const;
};

////////////////////////////////////////////////////////////////
class MmRR_onAdditional: public MmReference_More0 {
	static TypevsMcDict dict_;

	const McUdp_DNS *McUdp_DNS_;

	protected:
		virtual void composeList(WControl &, WObject *, const PObjectList &) const;

	public:
		MmRR_onAdditional(CSTR, McUdp_DNS *);
		virtual ~MmRR_onAdditional();

		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add(McDNS_RR *mc);
		static void add_other(McDNS_RR *mc);
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;

		virtual WObject *compose(WControl &, WObject *, const PObject *) const;
};
#endif

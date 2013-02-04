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
// $TAHI: v6eval/lib/Pz/McIKE.h,v 1.3 2008/04/01 23:48:22 akisada Exp $
//

////////////////////////////////////////////////////////////////
// RFC 2407 - IP Security Domain of Interpretation            //
// RFC 2408 - ISAKMP                                          //
////////////////////////////////////////////////////////////////

#ifndef __McIKE_h__
#define __McIKE_h__

#include "PvOctets.h"
#include "McSub.h"
#include "MmObject.h"
#include "McUDP.h"
#include "McESP.h"
#include "PAlgorithm.h"



extern uint32_t ISAKMP_Message_ID;



// RFC 2408 - ISAKMP
// 3.1 ISAKMP Header Format
// Next Payload
const int32_t TP_ISAKMP_NPTYPE_NONE	= 0;
const int32_t TP_ISAKMP_NPTYPE_SA	= 1;
const int32_t TP_ISAKMP_NPTYPE_P	= 2;
const int32_t TP_ISAKMP_NPTYPE_T	= 3;
const int32_t TP_ISAKMP_NPTYPE_KE	= 4;
const int32_t TP_ISAKMP_NPTYPE_ID	= 5;
const int32_t TP_ISAKMP_NPTYPE_CERT	= 6; 
const int32_t TP_ISAKMP_NPTYPE_CR	= 7;
const int32_t TP_ISAKMP_NPTYPE_HASH	= 8;
const int32_t TP_ISAKMP_NPTYPE_SIG	= 9;
const int32_t TP_ISAKMP_NPTYPE_NONCE	= 10;
const int32_t TP_ISAKMP_NPTYPE_N	= 11;
const int32_t TP_ISAKMP_NPTYPE_D	= 12;
const int32_t TP_ISAKMP_NPTYPE_VID	= 13;



// RFC 2408 - ISAKMP
// A.2 Internet IP Security DOI Assigned Value
const int32_t TP_ISAKMP_IPDOI	= 1;



// RFC 2407 - IP Security Domain of Interpretation
// 4.2 IPSEC Situation Definition
// Situation
const int32_t TP_IPSECDOI_SIT_IDENTITY_ONLY	= 0x01;
#if 0
const int32_t TP_IPSECDOI_SIT_SECRECY		= 0x02;
const int32_t TP_IPSECDOI_SIT_INTEGRITY		= 0x04;
#endif



// RFC 2407 - IP Security Domain of Interpretation
// 4.4.1 IPSEC Security Protocol Identifier
const int32_t TP_IPSECDOI_PROTO_ISAKMP		= 1;
const int32_t TP_IPSECDOI_PROTO_AH		= 2;
const int32_t TP_IPSECDOI_PROTO_ESP		= 3;
const int32_t TP_IPSECDOI_PROTO_IPCOMP		= 4;



// RFC 2407 - IP Security Domain of Interpretation
// 4.6.2.1 Identification Type Values
const int32_t TP_IPSECDOI_ID_IPV4_ADDR		= 1;
const int32_t TP_IPSECDOI_ID_FQDN		= 2;
const int32_t TP_IPSECDOI_ID_USER_FQDN		= 3;
const int32_t TP_IPSECDOI_ID_IPV4_ADDR_SUBNET	= 4;
const int32_t TP_IPSECDOI_ID_IPV6_ADDR		= 5;
const int32_t TP_IPSECDOI_ID_IPV6_ADDR_SUBNET	= 6;
const int32_t TP_IPSECDOI_ID_IPV4_ADDR_RANGE	= 7;
const int32_t TP_IPSECDOI_ID_IPV6_ADDR_RANGE	= 8;
const int32_t TP_IPSECDOI_ID_DER_ASN1_DN	= 9;
const int32_t TP_IPSECDOI_ID_DER_ASN1_GN	= 10;
const int32_t TP_IPSECDOI_ID_KEY_ID		= 11;



// RFC 2407 - IP Security Domain of Interpretation
// 4.6.3 IPSEC Notify Message Types
const int32_t TP_ISAKMP_NTYPE_RESPONDER_LIFETIME	= 24576;
const int32_t TP_ISAKMP_NTYPE_REPLAY_STATUS		= 24577;
#if 0
const int32_t TP_ISAKMP_NTYPE_INITIAL_CONTACT		= 24578;
#endif



//
// ISAKMP
//
////////////////////////////////

class McUdp_ISAKMP: public McUpper {
	static McUdp_ISAKMP *instance_;
	static class McHdr_ISAKMP *tophdr_;

	McUdp_ISAKMP(CSTR);

	public:
		virtual ~McUdp_ISAKMP();
		static McUdp_ISAKMP *create(CSTR, CSTR);
		static McUdp_ISAKMP *instance() { return(instance_); }
		int32_t upperPort() const { return(TP_Udp_ISAKMP); }
		virtual uint32_t length_for_reverse(RControl &,
			ItPosition &, OCTBUF &) const;
		virtual int32_t token() const {
			return(metaToken(tkn_isakmp_upper_)); };
};



//
// ISAKMP Header
//
////////////////////////////////

class EFlagValueCtrl {
	uint32_t e;

	public:
		EFlagValueCtrl();
		virtual ~EFlagValueCtrl();

		void set_e(uint32_t E) { e = E; return; }
		uint32_t get_e(void) const { return(e); }
};



class McHdr_ISAKMP: public McHeader {
	McHdr_ISAKMP(CSTR);
	MmUint *Layerlength_;
	MmUint *EFlag_;
	EFlagValueCtrl *ectrl_;
	MmUint *MID_;

	void
	Layerlength_member(MmUint *meta)
	{
		Layerlength_ = meta;
		member(meta);
	}

	void
	EFlag_member(MmUint *meta)
	{
		EFlag_ = meta;
		member(meta);
	}

	void MIDmember(MmUint *);

	DEC_HCGENE(Length);
	DEC_HCGENE(NextPayload);
	DEC_HCEVAL(NextPayload);
	DEC_HCGENE(EFlag);
	DEC_HCEVAL(EFlag);

	int32_t get_next_headerType(WObject *) const;
	bool need_ISAKMP_encryption(WObject *) const;

	public:
		virtual ~McHdr_ISAKMP();
		static McHdr_ISAKMP *create(CSTR);

		virtual uint32_t Layerlength_for_reverse(RControl &,
			ItPosition &, OCTBUF &) const;

		EFlagValueCtrl *get_ectrl(void) const { return(ectrl_); }
};

class MmISAKMP_MessageID: public MmUint {
	public:
		MmISAKMP_MessageID(CSTR, uint16_t, const PObject * = 0,
			const PObject * = 0, const ICVoverwriter * = 0,
			METH_HC_ForIPinfo = 0);
		virtual ~MmISAKMP_MessageID();
		virtual bool generate(WControl &, WObject *, OCTBUF &) const;
};



//
// ISAKMP encryption
//
////////////////////////////////

class McISAKMP_Encryption: public McHeader {
	protected:
		McISAKMP_Encryption(CSTR);
	public:
		virtual ~McISAKMP_Encryption();
		static McISAKMP_Encryption *create(CSTR);
		virtual uint32_t length_for_reverse(RControl &,
			ItPosition &, OCTBUF &) const;
		bool ISAKMP_encryption() const { return(true); }

		virtual RObject *reverse(RControl &,
			RObject *, ItPosition &, OCTBUF &) const;

		virtual RObject *reverseRc(RControl &,
			RObject *, const ItPosition &, PvObject *) const;

		virtual WObject *compose(WControl &,
			WObject *, const PObject *) const;

		virtual WObject *composeWc(WControl &,
			WObject *, const PObject *) const;
};

struct isakmp_gen {
	uint8_t		np;
	uint8_t		reserved;
	uint16_t	len;
} __attribute__((__packed__));

class McISAKMP_Decrypted: public McObject {
	friend class MmISAKMP_Crypted;
#if 0
	MmUint *PadLength_;
#endif
	McISAKMP_Decrypted(CSTR str);
	static  McISAKMP_Decrypted *create(CSTR);
	virtual ~McISAKMP_Decrypted();

	RObject *Self_reverse(RControl &, OCTBUF *) const;
	WObject *Self_composeList(WControl &, const PObjectList &) const;
#if 0
	DEC_HCGENE(PadLength);
#endif
	uint32_t calc_padlen(RControl &, OCTSTR, uint32_t, bool &) const;
};

class MmISAKMP_Crypted: public MmVarOctets {
	class McISAKMP_Decrypted *mDecrypted_;

	public:
		MmISAKMP_Crypted(CSTR str);
		virtual ~MmISAKMP_Crypted();
		virtual MObject *findMember(CSTR str) const;

		virtual ItPosition length_for_reverse(RControl &,
			ItPosition &, OCTBUF &) const;

		virtual RObject *reverseRm(RControl &,
			RObject *, const ItPosition &, const ItPosition &,
			PvObject *) const;

		virtual void composeList(WControl &, WObject *,
			const PObjectList &)const;
};



class MmAlgorithm_onISAKMP: public MmReference_Must1 {
	public:
		MmAlgorithm_onISAKMP(CSTR);
		virtual ~MmAlgorithm_onISAKMP();
		int32_t token() const { return(metaToken(tkn_algorithm_ref_)); }

		virtual RObject *reverse(RControl &,
			RObject *, ItPosition &, OCTBUF &) const;

		virtual bool disused() const;
};

class MmIVEC_onISAKMP: public MmVarOctets {
	public:
		MmIVEC_onISAKMP(CSTR key): MmVarOctets(key) {}
		virtual ~MmIVEC_onISAKMP() {}

		virtual RObject *reverse(RControl &,
			RObject *, ItPosition &, OCTBUF &) const;
		virtual ItPosition length_for_reverse(RControl &,
			ItPosition &, OCTBUF &) const;

		virtual int32_t token() const {
			return(metaToken(tkn_dummy_)); };

		virtual bool disused() const;

		virtual RObject *reverseRm(RControl &, RObject *,
			const ItPosition &, const ItPosition &,
			PvObject *) const;

//		virtual uint32_t objectLength(const PObject * = 0,
//			const WObject * = 0) const;
};



#include "McAlgorithm.h"
class McISAKMP_Algorithm: public McAlgorithm {
	protected:
		McISAKMP_Algorithm(CSTR);

	public:
		virtual ~McISAKMP_Algorithm();
		static  McISAKMP_Algorithm *create(CSTR);
		virtual PObject *tokenObject(int, CSTR) const;
};



class McISAKMP_PlainText: public McObject {
	friend class McISAKMP_Decrypted;

	McISAKMP_PlainText(CSTR str);
	virtual ~McISAKMP_PlainText();

	static McISAKMP_PlainText *create(CSTR);

	virtual uint32_t length_for_reverse(RControl &,
		ItPosition &, OCTBUF &)const;

	virtual RObject *reverse(RControl &,
		RObject *, ItPosition &, OCTBUF &) const;

	virtual bool generate(WControl &, WObject *, OCTBUF &) const;
	virtual void composeList(WControl &,
		WObject *, const PObjectList &) const;
};



class MmISAKMP_Padding: public MmESP_Padding {
	public:
		MmISAKMP_Padding(CSTR key): MmESP_Padding(key) {}
		virtual ~MmISAKMP_Padding() {}

		virtual ItPosition length_for_reverse(RControl &,
			ItPosition &, OCTBUF &) const;
		virtual WObject *composeWm(WControl &,
			WObject *, const PObject *) const;
};

class WmISAKMP_Padding: public WmObject {
	const PaObject* sa_;

	public:
		WmISAKMP_Padding(WObject *, const MObject *, const PaObject *);
		virtual ~WmISAKMP_Padding();
		virtual void internalGeneratePrologue(ItPosition &);
};



class PaISAKMP: public PaESP {
	public:
		PaISAKMP(const MObject *m, CSTR st, int l):
			PaESP(m, st, l) {};
		virtual ~PaISAKMP() {}

		virtual uint32_t Crypted_Length(uint32_t) const;
		virtual OCTBUF *Padding_Octets(uint32_t) const;
};



//
// Generic Payload
//
////////////////////////////////

class McPld_ISAKMP: public McHeader {
	MmUint *Layerlength_;

	void
	Layerlength_member(MmUint *meta)
	{
		Layerlength_ = meta;
		member(meta);
	}

	int32_t get_next_headerType(WObject *) const;

	protected:
		McPld_ISAKMP(CSTR);
		void common_member();
	public:
		virtual ~McPld_ISAKMP();

		virtual uint32_t length_for_reverse(RControl &,
			ItPosition &, OCTBUF &) const;

		DEC_HCGENE(PayloadLength);
		DEC_HCGENE(NextPayload);
		DEC_HCEVAL(NextPayload);
		virtual RObject *reverse(RControl &,
			RObject *, ItPosition &, OCTBUF &) const;
};

class McPld_ISAKMP_ANY: public McPld_ISAKMP {
	protected:
		McPld_ISAKMP_ANY(CSTR);
	public:
		virtual ~McPld_ISAKMP_ANY();
		static McPld_ISAKMP_ANY *create(CSTR);
};



//
// Security Association Payload
//
////////////////////////////////

class McPld_ISAKMP_SA_ONE: public McPld_ISAKMP {
	private:
		static McPld_ISAKMP_SA_ONE *instance_;
		McPld_ISAKMP_SA_ONE(CSTR);
	public:
		static McPld_ISAKMP_SA_ONE *instance();
		virtual ~McPld_ISAKMP_SA_ONE();
		int32_t headerType() const { return(TP_ISAKMP_NPTYPE_SA); }

		bool containsMc(const MObject *) const;
		virtual RObject *reverse(RControl &,
			RObject *, ItPosition &, OCTBUF &) const;
};

class McPld_ISAKMP_SA: public McPld_ISAKMP {
	MmUint *DOI_;

	void
	DOImember(MmUint *meta)
	{
		DOI_ = meta;
		member(meta);
	}

	DEC_HCGENE(DOI);
	DEC_HCEVAL(DOI);
	int32_t get_DOIvalue(WObject *) const;

	protected:
		McPld_ISAKMP_SA(CSTR);
	public:
		virtual ~McPld_ISAKMP_SA();
		void common_member();
		int32_t headerType() const { return(TP_ISAKMP_NPTYPE_SA); }
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
};

class McPld_ISAKMP_SA_ANY: public McPld_ISAKMP_SA {
	protected:
		McPld_ISAKMP_SA_ANY(CSTR);
	public:
		virtual ~McPld_ISAKMP_SA_ANY();
		static McPld_ISAKMP_SA_ANY *create(CSTR);
};

class McPld_ISAKMP_SA_IPsec_ONE: public McPld_ISAKMP {
	private:
		static McPld_ISAKMP_SA_IPsec_ONE *instance_;
		McPld_ISAKMP_SA_IPsec_ONE(CSTR);
	public:
		static McPld_ISAKMP_SA_IPsec_ONE *instance();
		virtual ~McPld_ISAKMP_SA_IPsec_ONE();
		int32_t DOIvalue() const { return(TP_ISAKMP_IPDOI); }

		bool containsMc(const MObject *) const;
		virtual RObject *reverse(RControl &,
			RObject *, ItPosition &, OCTBUF &) const;
};

class McPld_ISAKMP_SA_IPsec: public McPld_ISAKMP_SA {
	MmUint *SIT_;

	void
	SITmember(MmUint *meta)
	{
		SIT_ = meta;
		member(meta);
	}

	DEC_HCGENE(Situation);
	DEC_HCEVAL(Situation);
	int32_t get_SITvalue(WObject *) const;

	protected:
		McPld_ISAKMP_SA_IPsec(CSTR);
	public:
		virtual ~McPld_ISAKMP_SA_IPsec();
		void common_member();
		int32_t DOIvalue() const { return(TP_ISAKMP_IPDOI); }
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
};

class McPld_ISAKMP_SA_IPsec_ANY: public McPld_ISAKMP_SA_IPsec {
	protected:
		McPld_ISAKMP_SA_IPsec_ANY(CSTR);
	public:
		virtual ~McPld_ISAKMP_SA_IPsec_ANY();
		static McPld_ISAKMP_SA_IPsec_ANY *create(CSTR);
};

class McPld_ISAKMP_SA_IPsec_IDonly: public McPld_ISAKMP_SA_IPsec {
	protected:
		McPld_ISAKMP_SA_IPsec_IDonly(CSTR);
	public:
		virtual ~McPld_ISAKMP_SA_IPsec_IDonly();
		static McPld_ISAKMP_SA_IPsec_IDonly *create(CSTR);
		int32_t SITvalue() const {
			return(TP_IPSECDOI_SIT_IDENTITY_ONLY); }
};



//
// Proposal Payload
//
////////////////////////////////

class McPld_ISAKMP_P_ONE: public McPld_ISAKMP {
	private:
		static McPld_ISAKMP_P_ONE *instance_;
		McPld_ISAKMP_P_ONE(CSTR);
	public:
		static McPld_ISAKMP_P_ONE *instance();
		virtual ~McPld_ISAKMP_P_ONE();
		int32_t headerType() const { return(TP_ISAKMP_NPTYPE_P); }

		bool containsMc(const MObject *) const;
		virtual RObject *reverse(RControl &,
			RObject *, ItPosition &, OCTBUF &) const;
};

class Tcounter {
	int32_t transforms_;

	public:
		Tcounter(): transforms_(0) {}
		virtual ~Tcounter() {}
		void inc_trans() { transforms_ ++; return; }
		int32_t get_trans() { return(transforms_); }
		void set_trans(int32_t d) { transforms_ = d; return; }
};

class McPld_ISAKMP_P: public McPld_ISAKMP {
	MmUint *ProtocolID_;
	MmUint *SPIsize_;
	MObject *SPI_;

	class Tcounter *Tcounter_;

	void
	ProtocolIDmember(MmUint *meta)
	{
		ProtocolID_	= meta;
		member(meta);
	}

	DEC_HCGENE(ProtocolID);
	DEC_HCEVAL(ProtocolID);
	DEC_HCGENE(SPIsize);
	DEC_HCGENE(NumOfTransforms);
	DEC_HCEVAL(NumOfTransforms);

	int32_t get_ProtocolID(WObject *) const;

	protected:
		McPld_ISAKMP_P(CSTR);
		void common_member();
		void spisize_member();
		void transnum_member();

		void
		SPIsize_member(MmUint *meta)
		{
			SPIsize_	= meta;
			member(meta);
		}

		void
		SPImember(MObject *meta)
		{
			SPI_	= meta;
			member(meta);
		}
	public:
		virtual ~McPld_ISAKMP_P();
		static McPld_ISAKMP_P *create(CSTR);
		int32_t headerType() const { return(TP_ISAKMP_NPTYPE_P); }
		virtual RObject *reverse(RControl &,
			RObject *, ItPosition &, OCTBUF &) const;
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;

		void increase_transforms() {
			if(Tcounter_) { Tcounter_->inc_trans(); } return; }
		const int32_t get_transforms() const {
			return(Tcounter_? Tcounter_->get_trans(): 0); }
		void set_transforms(int32_t d) const {
			if(Tcounter_) { Tcounter_->set_trans(d); } return; }
#if 0
		virtual bool generate(WControl &, WObject *, OCTBUF &) const;
#else
		virtual WObject *composeWc(WControl &,
			WObject *, const PObject *) const;
#endif
};

class McPld_ISAKMP_P_ISAKMP: public McPld_ISAKMP_P {
	protected:
		McPld_ISAKMP_P_ISAKMP(CSTR);
	public:
		virtual ~McPld_ISAKMP_P_ISAKMP();
		static McPld_ISAKMP_P_ISAKMP *create(CSTR);
		int32_t ProtocolID() const { return(TP_IPSECDOI_PROTO_ISAKMP); }
};

class McPld_ISAKMP_P_IPsec_AH: public McPld_ISAKMP_P {
	protected:
		McPld_ISAKMP_P_IPsec_AH(CSTR);
	public:
		virtual ~McPld_ISAKMP_P_IPsec_AH();
		static McPld_ISAKMP_P_IPsec_AH *create(CSTR);
		int32_t ProtocolID() const { return(TP_IPSECDOI_PROTO_AH); }
};

class McPld_ISAKMP_P_IPsec_ESP: public McPld_ISAKMP_P {
	protected:
		McPld_ISAKMP_P_IPsec_ESP(CSTR);
	public:
		virtual ~McPld_ISAKMP_P_IPsec_ESP();
		static McPld_ISAKMP_P_IPsec_ESP *create(CSTR);
		int32_t ProtocolID() const { return(TP_IPSECDOI_PROTO_ESP); }
};

class McPld_ISAKMP_P_IPCOMP: public McPld_ISAKMP_P {
	protected:
		McPld_ISAKMP_P_IPCOMP(CSTR);
	public:
		virtual ~McPld_ISAKMP_P_IPCOMP();
		static McPld_ISAKMP_P_IPCOMP *create(CSTR);
		int32_t ProtocolID() const { return(TP_IPSECDOI_PROTO_IPCOMP); }
};



//
// Transform Payload
//
////////////////////////////////

class McPld_ISAKMP_T: public McPld_ISAKMP {
	protected:
		McPld_ISAKMP_T(CSTR);
	public:
		virtual ~McPld_ISAKMP_T();
		static McPld_ISAKMP_T *create(CSTR);
		int32_t headerType() const { return(TP_ISAKMP_NPTYPE_T); }
};



//
// Key Exchange Payload
//
////////////////////////////////

class McPld_ISAKMP_KE: public McPld_ISAKMP {
	protected:
		McPld_ISAKMP_KE(CSTR);
	public:
		virtual ~McPld_ISAKMP_KE();
		static McPld_ISAKMP_KE *create(CSTR);
		int32_t headerType() const { return(TP_ISAKMP_NPTYPE_KE); }
};



//
// Identification Payload
//
////////////////////////////////

class McPld_ISAKMP_ID_ONE: public McPld_ISAKMP {
	private:
		static McPld_ISAKMP_ID_ONE *instance_;
		McPld_ISAKMP_ID_ONE(CSTR);
	public:
		static McPld_ISAKMP_ID_ONE *instance();
		virtual ~McPld_ISAKMP_ID_ONE();
		int32_t headerType() const { return(TP_ISAKMP_NPTYPE_ID); }

		bool containsMc(const MObject *) const;
		virtual RObject *reverse(RControl &,
			RObject *, ItPosition &, OCTBUF &) const;
};

class McPld_ISAKMP_ID: public McPld_ISAKMP {
	MmUint *IDtype_;

	void
	IDtype_member(MmUint *meta)
	{
		IDtype_	= meta;
		member(meta);
	}

	DEC_HCGENE(IDtype);
	DEC_HCEVAL(IDtype);
	int32_t get_IDtype(WObject *) const;

	protected:
		McPld_ISAKMP_ID(CSTR);
		void common_member();
	public:
		virtual ~McPld_ISAKMP_ID();
		static McPld_ISAKMP_ID *create(CSTR);
		int32_t headerType() const { return(TP_ISAKMP_NPTYPE_ID); }
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
};

class McPld_ISAKMP_ID_IPV4_ADDR: public McPld_ISAKMP_ID {
	protected:
		McPld_ISAKMP_ID_IPV4_ADDR(CSTR);
	public:
		virtual ~McPld_ISAKMP_ID_IPV4_ADDR();
		static McPld_ISAKMP_ID_IPV4_ADDR *create(CSTR);
		int32_t IDtype() const { return(TP_IPSECDOI_ID_IPV4_ADDR); }
};

class McPld_ISAKMP_ID_FQDN: public McPld_ISAKMP_ID {
	protected:
		McPld_ISAKMP_ID_FQDN(CSTR);
	public:
		virtual ~McPld_ISAKMP_ID_FQDN();
		static McPld_ISAKMP_ID_FQDN *create(CSTR);
		int32_t IDtype() const { return(TP_IPSECDOI_ID_FQDN); }
};

class McPld_ISAKMP_ID_USER_FQDN: public McPld_ISAKMP_ID {
	protected:
		McPld_ISAKMP_ID_USER_FQDN(CSTR);
	public:
		virtual ~McPld_ISAKMP_ID_USER_FQDN();
		static McPld_ISAKMP_ID_USER_FQDN *create(CSTR);
		int32_t IDtype() const { return(TP_IPSECDOI_ID_USER_FQDN); }
};

class McPld_ISAKMP_ID_IPV4_ADDR_SUBNET: public McPld_ISAKMP_ID {
	protected:
		McPld_ISAKMP_ID_IPV4_ADDR_SUBNET(CSTR);
	public:
		virtual ~McPld_ISAKMP_ID_IPV4_ADDR_SUBNET();
		static McPld_ISAKMP_ID_IPV4_ADDR_SUBNET *create(CSTR);

		int32_t
		IDtype() const
		{
			return(TP_IPSECDOI_ID_IPV4_ADDR_SUBNET);
		}
};

class McPld_ISAKMP_ID_IPV6_ADDR: public McPld_ISAKMP_ID {
	protected:
		McPld_ISAKMP_ID_IPV6_ADDR(CSTR);
	public:
		virtual ~McPld_ISAKMP_ID_IPV6_ADDR();
		static McPld_ISAKMP_ID_IPV6_ADDR *create(CSTR);
		int32_t IDtype() const { return(TP_IPSECDOI_ID_IPV6_ADDR); }
};

class McPld_ISAKMP_ID_IPV6_ADDR_SUBNET: public McPld_ISAKMP_ID {
	protected:
		McPld_ISAKMP_ID_IPV6_ADDR_SUBNET(CSTR);
	public:
		virtual ~McPld_ISAKMP_ID_IPV6_ADDR_SUBNET();
		static McPld_ISAKMP_ID_IPV6_ADDR_SUBNET *create(CSTR);

		int32_t
		IDtype() const
		{
			return(TP_IPSECDOI_ID_IPV6_ADDR_SUBNET);
		}
};

class McPld_ISAKMP_ID_IPV4_ADDR_RANGE: public McPld_ISAKMP_ID {
	protected:
		McPld_ISAKMP_ID_IPV4_ADDR_RANGE(CSTR);
	public:
		virtual ~McPld_ISAKMP_ID_IPV4_ADDR_RANGE();
		static McPld_ISAKMP_ID_IPV4_ADDR_RANGE *create(CSTR);

		int32_t
		IDtype() const
		{
			return(TP_IPSECDOI_ID_IPV4_ADDR_RANGE);
		}
};

class McPld_ISAKMP_ID_IPV6_ADDR_RANGE: public McPld_ISAKMP_ID {
	protected:
		McPld_ISAKMP_ID_IPV6_ADDR_RANGE(CSTR);
	public:
		virtual ~McPld_ISAKMP_ID_IPV6_ADDR_RANGE();
		static McPld_ISAKMP_ID_IPV6_ADDR_RANGE *create(CSTR);

		int32_t
		IDtype() const
		{
			return(TP_IPSECDOI_ID_IPV6_ADDR_RANGE);
		}
};

class McPld_ISAKMP_ID_DER_ASN1_DN: public McPld_ISAKMP_ID {
	protected:
		McPld_ISAKMP_ID_DER_ASN1_DN(CSTR);
	public:
		virtual ~McPld_ISAKMP_ID_DER_ASN1_DN();
		static McPld_ISAKMP_ID_DER_ASN1_DN *create(CSTR);
		int32_t IDtype() const { return(TP_IPSECDOI_ID_DER_ASN1_DN); }
};

class McPld_ISAKMP_ID_DER_ASN1_GN: public McPld_ISAKMP_ID {
	protected:
		McPld_ISAKMP_ID_DER_ASN1_GN(CSTR);
	public:
		virtual ~McPld_ISAKMP_ID_DER_ASN1_GN();
		static McPld_ISAKMP_ID_DER_ASN1_GN *create(CSTR);
		int32_t IDtype() const { return(TP_IPSECDOI_ID_DER_ASN1_GN); }
};

class McPld_ISAKMP_ID_KEY_ID: public McPld_ISAKMP_ID {
	protected:
		McPld_ISAKMP_ID_KEY_ID(CSTR);
	public:
		virtual ~McPld_ISAKMP_ID_KEY_ID();
		static McPld_ISAKMP_ID_KEY_ID *create(CSTR);
		int32_t IDtype() const { return(TP_IPSECDOI_ID_KEY_ID); }
};



//
// Certificate Payload
//
////////////////////////////////

class McPld_ISAKMP_CERT: public McPld_ISAKMP {
	protected:
		McPld_ISAKMP_CERT(CSTR);
	public:
		virtual ~McPld_ISAKMP_CERT();
		static McPld_ISAKMP_CERT *create(CSTR);
		int32_t headerType() const { return(TP_ISAKMP_NPTYPE_CERT); }
};



//
// Certificate Request Payload
//
////////////////////////////////

class McPld_ISAKMP_CR: public McPld_ISAKMP {
	protected:
		McPld_ISAKMP_CR(CSTR);
	public:
		virtual ~McPld_ISAKMP_CR();
		static McPld_ISAKMP_CR *create(CSTR);
		int32_t headerType() const { return(TP_ISAKMP_NPTYPE_CR); }
};



//
// Hash Payload
//
////////////////////////////////

class McPld_ISAKMP_HASH: public McPld_ISAKMP {
	protected:
		McPld_ISAKMP_HASH(CSTR);
	public:
		virtual ~McPld_ISAKMP_HASH();
		static McPld_ISAKMP_HASH *create(CSTR);
		int32_t headerType() const { return(TP_ISAKMP_NPTYPE_HASH); }
		virtual PObject *tokenObject(int, CSTR) const;
};

class PfP2_HASH_2: public PvFunction {
	private:
		const MvP2_HASH_2 *meta_;
		OCTSTR context_;
	public:
		PfP2_HASH_2(const MvP2_HASH_2 *, CSTR, int);
		virtual ~PfP2_HASH_2();
		const MvP2_HASH_2 *metaClass() const;
		virtual const MObject *meta() const;
		void init();
		void update(const OCTBUF &);
		PvOctets *result();
};

inline const MvP2_HASH_2 *PfP2_HASH_2::metaClass() const { return(meta_); }

class PaP2_HASH_2: public PcObject {
	private:
		PfP2_HASH_2 *p2_hash_2_;
	public:
		PaP2_HASH_2(const MObject*, CSTR, int);
		virtual ~PaP2_HASH_2();

		virtual PObject *ike_p2_hash_member(PObject *);

		virtual OCTBUF *P2_HASH_2_Calculate(const OCTBUF &) const;
		virtual WObject *selfCompose(WControl &, WObject *) const;
};

interfaceCmList(P2_HASH_2List, PaP2_HASH_2);

class KeepP2_HASH_2 {
	private:
		P2_HASH_2List *p2_hash_2_;
		uint32_t current_;
	public:
		KeepP2_HASH_2();
		virtual ~KeepP2_HASH_2();

		const PaP2_HASH_2 *GetP2_HASH_2(void);
		void SetP2_HASH_2(const PaP2_HASH_2 *);
//		void KeepP2_HASH_2::ResetCounter(void);
};

class MmP2_HASH_2: public MmVarOctets {
	private:
		KeepP2_HASH_2 *keephash_;
	public:
		MmP2_HASH_2(CSTR, const PObject * = 0, const PObject * = 0,
			const ICVoverwriter * = 0, METH_HC_ForIPinfo = 0);
		virtual ~MmP2_HASH_2();

		virtual RObject *reverseRm(RControl &, RObject *,
			const ItPosition &, const ItPosition &,
			PvObject *) const;
		virtual WObject *composeWm(WControl &, WObject *,
			const PObject *) const;
		virtual RObject *reverse(RControl &, RObject *, ItPosition &,
			OCTBUF &) const;
		virtual bool generate(WControl &, WObject *, OCTBUF &) const;
};



class RmP2_HASH_2: public RmObject {
	const PaP2_HASH_2 *hash_;
	PvObject *calc_pvalue_;

	public:
		RmP2_HASH_2(RObject *, const MObject *, const ItPosition &,
			const ItPosition &, PvObject *, const PaP2_HASH_2 *);

		virtual ~RmP2_HASH_2();
		void set_calc_pvalue(PvObject *);
		PvObject *calc_pvalue() const { return(calc_pvalue_); }
		virtual void post_reverse(Con_IPinfo &, RControl &, RObject *);
};



class WmP2_HASH_2: public WmObject {
	const PaP2_HASH_2 *hash_;

	public:
		WmP2_HASH_2(WObject *, const MObject *, const PObject *,
			const PaP2_HASH_2 *);
		virtual ~WmP2_HASH_2();
		virtual void post_generate(Con_IPinfo &, WControl &, OCTBUF &,
			WObject *);
		virtual bool doEvaluate(WControl &, RObject &);
};



//
// Signature Payload
//
////////////////////////////////

class McPld_ISAKMP_SIG: public McPld_ISAKMP {
	protected:
		McPld_ISAKMP_SIG(CSTR);
	public:
		virtual ~McPld_ISAKMP_SIG();
		static McPld_ISAKMP_SIG *create(CSTR);
		int32_t headerType() const { return(TP_ISAKMP_NPTYPE_SIG); }
};



//
// Nonce Payload
//
////////////////////////////////

class McPld_ISAKMP_NONCE: public McPld_ISAKMP {
	protected:
		McPld_ISAKMP_NONCE(CSTR);
	public:
		virtual ~McPld_ISAKMP_NONCE();
		static McPld_ISAKMP_NONCE *create(CSTR);
		int32_t headerType() const { return(TP_ISAKMP_NPTYPE_NONCE); }
};



//
// Notification Payload
//
////////////////////////////////

class McPld_ISAKMP_N_ONE: public McPld_ISAKMP {
	private:
		static McPld_ISAKMP_N_ONE *instance_;
		McPld_ISAKMP_N_ONE(CSTR);
	public:
		static McPld_ISAKMP_N_ONE *instance();
		virtual ~McPld_ISAKMP_N_ONE();
		int32_t headerType() const { return(TP_ISAKMP_NPTYPE_N); }

		bool containsMc(const MObject *) const;
		virtual RObject *reverse(RControl &,
			RObject *, ItPosition &, OCTBUF &) const;
};

class McPld_ISAKMP_N: public McPld_ISAKMP {
	MmUint *DOI_;
	MmUint *SPIsize_;
	MObject *SPI_;

	void
	DOImember(MmUint *meta)
	{
		DOI_	= meta;
		member(meta);
	}

	void
	SPIsize_member(MmUint *meta)
	{
		SPIsize_	= meta;
		member(meta);
	}

	void
	NotifyMessageType_member(MmUint *meta)
	{
		NotifyMessageType_	= meta;
		member(meta);
	}

	DEC_HCGENE(DOI);
	DEC_HCEVAL(DOI);
	int32_t get_DOIvalue(WObject *) const;

	DEC_HCGENE(NotifyMessageType);
	DEC_HCEVAL(NotifyMessageType);
	int32_t get_NotifyMessageType_value(WObject *) const;

	DEC_HCGENE(SPIsize);

	protected:
		MmUint *NotifyMessageType_;
		McPld_ISAKMP_N(CSTR);
		void
		SPImember(MObject *meta)
		{
			SPI_	= meta;
			member(meta);
		}

	public:
		virtual ~McPld_ISAKMP_N();
		void common_member();
		int32_t headerType() const { return(TP_ISAKMP_NPTYPE_N); }
		virtual RObject *reverse(RControl &,
			RObject *, ItPosition &, OCTBUF &) const;
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
};

class McPld_ISAKMP_N_ANY: public McPld_ISAKMP_N {
	protected:
		McPld_ISAKMP_N_ANY(CSTR);
	public:
		virtual ~McPld_ISAKMP_N_ANY();
		static McPld_ISAKMP_N_ANY *create(CSTR);
		int32_t notify_value() const { return(0); }
};

class McPld_ISAKMP_N_ISAKMP: public McPld_ISAKMP_N {
	protected:
		McPld_ISAKMP_N_ISAKMP(CSTR);
	public:
		virtual ~McPld_ISAKMP_N_ISAKMP();
		static McPld_ISAKMP_N_ISAKMP *create(CSTR);
		int32_t DOIvalue() const { return(0); }
		int32_t notify_value() const { return(0); }
};

class McPld_ISAKMP_N_IPsec_ONE: public McPld_ISAKMP {
	private:
		static McPld_ISAKMP_N_IPsec_ONE *instance_;
		McPld_ISAKMP_N_IPsec_ONE(CSTR);
	public:
		static McPld_ISAKMP_N_IPsec_ONE *instance();
		virtual ~McPld_ISAKMP_N_IPsec_ONE();
		int32_t DOIvalue() const { return(TP_ISAKMP_IPDOI); }

		bool containsMc(const MObject *) const;
		virtual RObject *reverse(RControl &,
			RObject *, ItPosition &, OCTBUF &) const;
};

class McPld_ISAKMP_N_IPsec: public McPld_ISAKMP_N {
	protected:
		McPld_ISAKMP_N_IPsec(CSTR);
	public:
		virtual ~McPld_ISAKMP_N_IPsec();
		int32_t DOIvalue() const { return(TP_ISAKMP_IPDOI); }
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
};

class McPld_ISAKMP_N_IPsec_ANY: public McPld_ISAKMP_N_IPsec {
	protected:
		McPld_ISAKMP_N_IPsec_ANY(CSTR);
	public:
		virtual ~McPld_ISAKMP_N_IPsec_ANY();
		static McPld_ISAKMP_N_IPsec_ANY *create(CSTR);
		int32_t DOIvalue() const { return(TP_ISAKMP_IPDOI); }
};

class McPld_ISAKMP_N_IPsec_ResponderLT: public McPld_ISAKMP_N_IPsec {
	protected:
		McPld_ISAKMP_N_IPsec_ResponderLT(CSTR);
	public:
		virtual ~McPld_ISAKMP_N_IPsec_ResponderLT();
		static McPld_ISAKMP_N_IPsec_ResponderLT *create(CSTR);
		int32_t DOIvalue() const { return(TP_ISAKMP_IPDOI); }

		int32_t
		notify_value() const
		{
			return(TP_ISAKMP_NTYPE_RESPONDER_LIFETIME);
		}
};

class McPld_ISAKMP_N_IPsec_ReplayStat: public McPld_ISAKMP_N_IPsec {
	protected:
		McPld_ISAKMP_N_IPsec_ReplayStat(CSTR);
	public:
		virtual ~McPld_ISAKMP_N_IPsec_ReplayStat();
		static McPld_ISAKMP_N_IPsec_ReplayStat *create(CSTR);
		int32_t DOIvalue() const { return(TP_ISAKMP_IPDOI); }

		int32_t
		notify_value() const
		{
			return(TP_ISAKMP_NTYPE_REPLAY_STATUS);
		}
#if 0
		virtual uint32_t length_for_reverse(RControl &,
			ItPosition &, OCTBUF &) const;
#endif
};



//
// Delete Payload
//
////////////////////////////////

class SPIcontrol_onD {
	uint32_t size_;
	uint32_t num_;

	public:
		SPIcontrol_onD();
		virtual ~SPIcontrol_onD();

		void set_size(uint32_t size) { size_ = size; return; }
		uint32_t get_size(void) const { return(size_); }

		void set_num(uint32_t num) { num_ = num; return; }
		uint32_t get_num(void) const { return(num_); }
};

class McPld_ISAKMP_D: public McPld_ISAKMP {
	MmUint *SPIsize_;
	MmUint *NumberOfSPI_;
	SPIcontrol_onD *spi_;

	void
	SPIsize_member(MmUint *meta)
	{
		SPIsize_ = meta;
		member(meta);
	}

	void
	NumberOfSPI_member(MmUint *meta)
	{
		NumberOfSPI_ = meta;
		member(meta);
	}

	protected:
		McPld_ISAKMP_D(CSTR);
	public:
		virtual ~McPld_ISAKMP_D();
		static McPld_ISAKMP_D *create(CSTR);
		int32_t headerType() const { return(TP_ISAKMP_NPTYPE_D); }

		DEC_HCGENE(SPIsize);
		DEC_HCGENE(NumberOfSPI);
		DEC_HC_MLC(SPI);

//		virtual RObject *reverse(RControl &,
//			RObject *, ItPosition &, OCTBUF &) const;

		void set_size(uint32_t size) {
			if(spi_) { spi_->set_size(size); } return; }

		uint32_t get_size(void) const {
			return(spi_? spi_->get_size(): 0); }

		void set_num(uint32_t num) {
			if(spi_) { spi_->set_num(num); } return; }

		uint32_t get_num(void) const {
			return(spi_? spi_->get_num(): 0); }

		virtual WObject *composeWc(WControl &,
			WObject *, const PObject *) const;
};

class MmSPIsize: public MmUint {
	McPld_ISAKMP_D *delete_;

	public:
		MmSPIsize(McPld_ISAKMP_D *, CSTR, uint16_t,
			const PObject * = 0, const PObject * = 0,
			const ICVoverwriter * = 0, METH_HC_ForIPinfo = 0);
		virtual ~MmSPIsize();
		virtual RObject *reverse(RControl &,
			RObject *, ItPosition &, OCTBUF &) const;
};

class MmNumberOfSPI: public MmUint {
	McPld_ISAKMP_D *delete_;

	public:
		MmNumberOfSPI(McPld_ISAKMP_D *, CSTR, uint16_t,
			const PObject * = 0, const PObject * = 0,
			const ICVoverwriter * = 0, METH_HC_ForIPinfo = 0);
		virtual ~MmNumberOfSPI();
		virtual RObject *reverse(RControl &,
			RObject *, ItPosition &, OCTBUF &) const;
};

class MmSPI_onD: public MmVarOctets {
	McPld_ISAKMP_D *delete_;

	public:
		MmSPI_onD(McPld_ISAKMP_D *, CSTR,
			const PObject * = 0, const PObject * = 0,
			const ICVoverwriter * = 0, METH_HC_ForIPinfo = 0);
		virtual ~MmSPI_onD();
		virtual ItPosition length_for_reverse(RControl &,
			ItPosition &, OCTBUF &) const;

		virtual WObject *composeWm(WControl &,
			WObject *, const PObject *) const;
};



//
// Vendor ID Payload
//
////////////////////////////////

class McPld_ISAKMP_VID: public McPld_ISAKMP {
	protected:
		McPld_ISAKMP_VID(CSTR);
	public:
		virtual ~McPld_ISAKMP_VID();
		static McPld_ISAKMP_VID *create(CSTR);
		int32_t headerType() const { return(TP_ISAKMP_NPTYPE_VID); }
};



//
// Data Attributes
//
////////////////////////////////

class McAttr_ISAKMP: public McOption {
	MmUint *Attr_;

	void
	Attr_member(MmUint *meta)
	{
		Attr_	= meta;
		member(meta);
	}

	protected:
		McAttr_ISAKMP(CSTR);
		void common_member();
	public:
		virtual ~McAttr_ISAKMP();
		static McAttr_ISAKMP *create(CSTR);
		DEC_HCGENE(AF);
		DEC_HCEVAL(AF);
		int32_t get_attr_format(WObject *) const;
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
};

class McAttr_ISAKMP_TLV: public McAttr_ISAKMP {
	MmUint *Length_;

	void
	Length_member(MmUint *meta)
	{
		Length_	= meta;
		member(meta);
	}

	protected:
		McAttr_ISAKMP_TLV(CSTR);
	public:
		virtual ~McAttr_ISAKMP_TLV();
		static McAttr_ISAKMP_TLV *create(CSTR);
		int32_t headerType() const { return(0); }
		DEC_HCGENE(Length);
		virtual uint32_t length_for_reverse(RControl &,
			ItPosition &, OCTBUF &) const;
};

class McAttr_ISAKMP_TV: public McAttr_ISAKMP {
	protected:
		McAttr_ISAKMP_TV(CSTR);
	public:
		virtual ~McAttr_ISAKMP_TV();
		static McAttr_ISAKMP_TV *create(CSTR);
		int32_t headerType() const { return(1); }
};



//
// SPI (P, N)
//
////////////////////////////////

class SPIsizeCtrl {
	bool		init_;
	uint32_t	length_;

	public:
		SPIsizeCtrl();
		virtual ~SPIsizeCtrl();

		bool get_init() { return(init_); }
		uint32_t get_length() { return(length_); }
		void set_init(bool d) { init_ = d; return; }
		void set_length(uint32_t d) { length_ = d; return; }
};

class MmISAKMP_SPI: public MmVarOctets {
	protected:
		SPIsizeCtrl *ctrl_;

	public:
		MmISAKMP_SPI(CSTR, const PObject * = 0, const PObject * = 0,
			const ICVoverwriter * = 0, METH_HC_ForIPinfo = 0);

		virtual ~MmISAKMP_SPI();
		virtual ItPosition length_for_reverse(RControl &,
			ItPosition &, OCTBUF &) const;
		void set_ctrl(bool, uint32_t);
};

class MmISAKMP_SPI_ReplayStat: public MmISAKMP_SPI {
	public:
		MmISAKMP_SPI_ReplayStat(CSTR,
			const PObject * = 0, const PObject * = 0,
			const ICVoverwriter * = 0, METH_HC_ForIPinfo = 0);

		virtual ~MmISAKMP_SPI_ReplayStat();
		virtual ItPosition length_for_reverse(RControl &,
			ItPosition &, OCTBUF &) const;
};



//
// FQDN, USER_FQDN (ID)
//
////////////////////////////////

class MmAsciiString: public MmVarOctets {
	public:
		MmAsciiString(CSTR, const PObject * = 0, const PObject * = 0,
			const ICVoverwriter * = 0, METH_HC_ForIPinfo = 0);
		virtual ~MmAsciiString();
		virtual PvObject *reversePv(RControl &, const ItPosition &,
			const ItPosition &, const OCTBUF &) const;
};

class PvAsciiString: public PvOctets {
	public:
		PvAsciiString();
		PvAsciiString(uint32_t, OCTSTR = 0, bool = false);
		virtual ~PvAsciiString();
		virtual PvObject *shallowCopy() const;
		virtual void print() const;
		virtual void log(uint32_t = 0) const;
};



//
// McUdp_ISAKMP <-> McISAKMP_Encryption
//
////////////////////////////////

class MmEncrypt_onISAKMP: public MmReference_More0 {
	static TypevsMcDict dict_;
	McHdr_ISAKMP *tophdr_;

	public:
		MmEncrypt_onISAKMP(CSTR, McHdr_ISAKMP *);
		virtual ~MmEncrypt_onISAKMP();

		int32_t token() const {
			return(metaToken(tkn_isakmp_encryption_ref_)); }

		const TypevsMcDict *get_dict() const { return(&dict_); }
		static void add_other(McISAKMP_Encryption *);
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
		virtual RObject *reverse(RControl &, RObject *,
			ItPosition &, OCTBUF &) const;
};



//
// McUdp_ISAKMP <-> McPld_ISAKMP
//
////////////////////////////////

class MmPayload_onISAKMP: public MmReference_More0 {
	static TypevsMcDict dict_;

	public:
		MmPayload_onISAKMP(CSTR);
		virtual ~MmPayload_onISAKMP();

		int32_t token() const { return(metaToken(tkn_option_ref_)); }
		const TypevsMcDict *get_dict() const { return(&dict_); }
		static void add_other(McPld_ISAKMP *);
		static void add(McPld_ISAKMP *);
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
};



//
// McPld_ISAKMP_SA <-> McPld_ISAKMP_P
//
////////////////////////////////

class MmISAKMP_PonSA: public MmReference_More0 {
	static TypevsMcDict dict_;

	public:
		MmISAKMP_PonSA(CSTR);
		virtual ~MmISAKMP_PonSA();
		int32_t token() const { return(metaToken(tkn_option_ref_)); }
		const TypevsMcDict *get_dict() const { return(&dict_); }
		static void add_other(McPld_ISAKMP *);
		static void add(McPld_ISAKMP *);
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
		virtual RObject *reverse(RControl &,
			RObject *, ItPosition &, OCTBUF &) const;
};



//
// McPld_ISAKMP_P <-> McPld_ISAKMP_T
//
////////////////////////////////

class MmISAKMP_TonP: public MmReference_More0 {
	static TypevsMcDict dict_;
	McPld_ISAKMP_P *p_;

	public:
		MmISAKMP_TonP(McPld_ISAKMP_P *, CSTR);
		virtual ~MmISAKMP_TonP();
		int32_t token() const { return(metaToken(tkn_option_ref_)); }
		const TypevsMcDict *get_dict() const { return(&dict_); }
		static void add_other(McPld_ISAKMP *);
		static void add(McPld_ISAKMP *);
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
		virtual WObject *compose(WControl &,
			WObject *, const PObject *) const;
		virtual RObject *reverse(RControl &,
			RObject *, ItPosition &, OCTBUF &) const;
};



//
// McPld_ISAKMP_SA_ONE <-> McPld_ISAKMP_SA
//
////////////////////////////////

class MmDOIonSA: public MmReference_Must1 {
	static TypevsMcDict dict_;

	public:
		MmDOIonSA(CSTR);
		virtual ~MmDOIonSA();

		int32_t token() const { return(metaToken(tkn_option_ref_)); }
		const TypevsMcDict *get_dict() const { return(&dict_); }
		static void add_other(McPld_ISAKMP *);
		static void add(McPld_ISAKMP *);
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
};



//
// McPld_ISAKMP_N_ONE <-> McPld_ISAKMP_N
//
////////////////////////////////

class MmDOIonN: public MmReference_Must1 {
	static TypevsMcDict dict_;

	public:
		MmDOIonN(CSTR);
		virtual ~MmDOIonN();

		int32_t token() const { return(metaToken(tkn_option_ref_)); }
		const TypevsMcDict *get_dict() const { return(&dict_); }
		static void add_other(McPld_ISAKMP *);
		static void add(McPld_ISAKMP *);
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
};



//
// McPld_ISAKMP_SA_IPsec_ONE <-> McPld_ISAKMP_SA_IPsec
//
////////////////////////////////

class MmSIT_onSA_IPsec: public MmReference_Must1 {
	static TypevsMcDict dict_;

	public:
		MmSIT_onSA_IPsec(CSTR);
		virtual ~MmSIT_onSA_IPsec();

		int32_t token() const { return(metaToken(tkn_option_ref_)); }
		const TypevsMcDict *get_dict() const { return(&dict_); }
		static void add_other(McPld_ISAKMP_SA_IPsec *);
		static void add(McPld_ISAKMP_SA_IPsec *);
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
};



//
// McPld_ISAKMP_N_IPsec_ONE <-> McPld_ISAKMP_N_IPsec
//
////////////////////////////////

class MmType_onN_IPsec: public MmReference_Must1 {
	static TypevsMcDict dict_;

	public:
		MmType_onN_IPsec(CSTR);
		virtual ~MmType_onN_IPsec();

		int32_t token() const { return(metaToken(tkn_option_ref_)); }
		const TypevsMcDict *get_dict() const { return(&dict_); }
		static void add_other(McPld_ISAKMP_N_IPsec *);
		static void add(McPld_ISAKMP_N_IPsec *);
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
};



//
// McPld_ISAKMP_P_ONE <-> McPld_ISAKMP_P
//
////////////////////////////////

class MmProtocolIDonP: public MmReference_Must1 {
	static TypevsMcDict dict_;

	public:
		MmProtocolIDonP(CSTR);
		virtual ~MmProtocolIDonP();

		int32_t token() const { return(metaToken(tkn_option_ref_)); }
		const TypevsMcDict *get_dict() const { return(&dict_); }
		static void add_other(McPld_ISAKMP *);
		static void add(McPld_ISAKMP *);
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
};



//
// McPld_ISAKMP_ID_ONE <-> McPld_ISAKMP_ID
//
////////////////////////////////

class MmIDtype_onID: public MmReference_Must1 {
	static TypevsMcDict dict_;

	public:
		MmIDtype_onID(CSTR);
		virtual ~MmIDtype_onID();

		int32_t token() const { return(metaToken(tkn_option_ref_)); }
		const TypevsMcDict *get_dict() const { return(&dict_); }
		static void add_other(McPld_ISAKMP *);
		static void add(McPld_ISAKMP *);
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
};



//
// McPld_ISAKMP_T <-> McAttr_ISAKMP
//
////////////////////////////////

class MmAttr_onT: public MmReference_More0 {
	static TypevsMcDict dict_;

	public:
		MmAttr_onT(CSTR);
		virtual ~MmAttr_onT();

		int32_t token() const { return(metaToken(tkn_option_ref_)); }
		const TypevsMcDict *get_dict() const { return(&dict_); }
		static void add_other(McAttr_ISAKMP *);
		static void add(McAttr_ISAKMP *);
		bool overwrite_DictType(RControl &,
			ItPosition &, OCTBUF &) const;
};
#endif	// __McIKE_h__

/*
 *
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
 * $TAHI: v6eval/lib/Pz/McMobility.h,v 1.12 2005/05/09 09:35:24 akisada Exp $
 *
 */

#if !defined(__McMobility_h__)
#define __McMobility_h__	1

#include "McSub.h"
#include "MmHeader.h"
#include "McIPv6.h"
#include "WObject.h"
#include "PvObject.h"
#include "MvFunction.h"
#include "PcObject.h"

//MH Type
const int32_t TP_Ext_MH_BRR	= 0;
const int32_t TP_Ext_MH_HoTI	= 1;
const int32_t TP_Ext_MH_CoTI	= 2;
const int32_t TP_Ext_MH_HoT	= 3;
const int32_t TP_Ext_MH_CoT	= 4;
const int32_t TP_Ext_MH_BU	= 5;
const int32_t TP_Ext_MH_BA	= 6;
const int32_t TP_Ext_MH_BE	= 7;

//Option Type (MH)
const int32_t TP_Opt_MH_Pad1			= 0;
const int32_t TP_Opt_MH_PadN			= 1;
const int32_t TP_Opt_MH_BindingRefreshAdvice	= 2;
const int32_t TP_Opt_MH_AlternateCoA		= 3;
const int32_t TP_Opt_MH_NonceIndices		= 4;
const int32_t TP_Opt_MH_BindingAuthData		= 5;

#ifndef TBD_OPT_MH_MOB_NETWORK_PREF
#define TBD_OPT_MH_MOB_NETWORK_PREF	6
#endif	// TBD_OPT_MH_MOB_NETWORK_PREF
const int32_t TP_Opt_MH_MobNetworkPrefix	= TBD_OPT_MH_MOB_NETWORK_PREF;
#undef TBD_OPT_MH_MOB_NETWORK_PREF

class LengthCtrl {
	uint32_t real_length;

	public:
		LengthCtrl(): real_length(0) {};
		virtual ~LengthCtrl() {};

		void set_real_length(uint32_t);
		const uint32_t get_real_length() const;
};

inline void LengthCtrl::set_real_length(uint32_t len) {real_length = len; return;}
inline const uint32_t LengthCtrl::get_real_length() const {return(real_length);}

////////////////////////////////////////////////////////////////
class RmBAstatus: public RmObject {
	public:
		RmBAstatus(RObject *, const MObject *, const ItPosition &, const ItPosition &, PvObject * = 0);
		virtual ~RmBAstatus();
		virtual void printName(uint32_t, CSTR = 0) const;
		virtual void logSelf(uint32_t, CSTR = 0) const;
		void printBAstatus(uint32_t) const;
};

////////////////////////////////////////////////////////////////
class MmBAstatus: public MmUint {
	public:
		MmBAstatus(CSTR, uint16_t, const PObject * = 0, const PObject * = 0, const ICVoverwriter * = 0, METH_HC_ForIPinfo = 0);
		virtual ~MmBAstatus();
		virtual RObject *reverseRm(RControl &, RObject *, const ItPosition &, const ItPosition &, PvObject *) const;
};

////////////////////////////////////////////////////////////////
class RmBEstatus: public RmObject {
	public:
		RmBEstatus(RObject *, const MObject *, const ItPosition &, const ItPosition &, PvObject * = 0);
		virtual ~RmBEstatus();
		virtual void printName(uint32_t, CSTR = 0) const;
		virtual void logSelf(uint32_t, CSTR = 0) const;
		void printBEstatus(uint32_t) const;
};

////////////////////////////////////////////////////////////////
class MmBEstatus: public MmUint {
	public:
		MmBEstatus(CSTR, uint16_t, const PObject * = 0, const PObject * = 0, const ICVoverwriter * = 0, METH_HC_ForIPinfo = 0);
		virtual ~MmBEstatus();
		virtual RObject *reverseRm(RControl &, RObject *, const ItPosition &, const ItPosition &, PvObject *) const;
};

//
//  Mobility Header
//
////////////////////////////////////////////////////////////////////////////////

class McHdr_Ext_MH_ONE: public McHdr_Ext {
	private:
		static McHdr_Ext_MH_ONE *instance_;
		McHdr_Ext_MH_ONE(CSTR);

	public:
		static McHdr_Ext_MH_ONE *instance();

		virtual ~McHdr_Ext_MH_ONE();

		int32_t headerType() const {
			return(TP_Ext_MH);
		}

		bool containsMc(const MObject *) const;
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
};

class McHdr_Ext_MH: public McHdr_Ext {
	protected:
		MmUint *length_;
		MmUint *type_;
		LengthCtrl *LengthCtrl_;

		void length_member(MmUint *meta) {
			length_ = meta;
			member(meta);
		}

		void type_member(MmUint *meta) {
			type_ = meta;
			member(meta);
		}

		void common_member();

		void set_LengthCtrl(uint32_t len) const {
			if(LengthCtrl_) {
				LengthCtrl_->set_real_length(len);
			}
		}

		const uint32_t get_LengthCtrl() const {
			if(LengthCtrl_) {
				return(LengthCtrl_->get_real_length());
			}

			return(0);
		}

		McHdr_Ext_MH(CSTR);
	public:
		virtual ~McHdr_Ext_MH();

		int32_t headerType() const {
			return(TP_Ext_MH);
		}

		// COMPOSE/REVERSE INTERFACE --------------------------------------------------
		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		DEC_HCGENE(HeaderExtLength);
		DEC_HCGENE(Type);
		DEC_HCEVAL(Type);
		int32_t get_mobilityType(WObject *) const;

		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
		virtual bool generate(WControl &, WObject *, OCTBUF &) const;
};

class McHdr_Ext_MH_ANY: public McHdr_Ext_MH {
	public:
		McHdr_Ext_MH_ANY(CSTR);
		virtual ~McHdr_Ext_MH_ANY();
		static McHdr_Ext_MH_ANY *create(CSTR);
};

class McHdr_Ext_MH_BRR: public McHdr_Ext_MH {
	public:
		McHdr_Ext_MH_BRR(CSTR);
		virtual ~McHdr_Ext_MH_BRR();
		static McHdr_Ext_MH_BRR *create(CSTR);

		int32_t mobilityType() const {
			return(TP_Ext_MH_BRR);
		}
};

class McHdr_Ext_MH_HoTI: public McHdr_Ext_MH {
	public:
		McHdr_Ext_MH_HoTI(CSTR);
		virtual ~McHdr_Ext_MH_HoTI();
		static McHdr_Ext_MH_HoTI *create(CSTR);

		int32_t mobilityType() const {
			return(TP_Ext_MH_HoTI);
		}
};

class McHdr_Ext_MH_CoTI: public McHdr_Ext_MH {
	public:
		McHdr_Ext_MH_CoTI(CSTR);
		virtual ~McHdr_Ext_MH_CoTI();
		static McHdr_Ext_MH_CoTI *create(CSTR);

		int32_t mobilityType() const {
			return(TP_Ext_MH_CoTI);
		}
};

class McHdr_Ext_MH_HoT: public McHdr_Ext_MH {
	public:
		McHdr_Ext_MH_HoT(CSTR);
		virtual ~McHdr_Ext_MH_HoT();
		static McHdr_Ext_MH_HoT *create(CSTR);

		int32_t mobilityType() const {
			return(TP_Ext_MH_HoT);
		}
};

class McHdr_Ext_MH_CoT: public McHdr_Ext_MH {
	public:
		McHdr_Ext_MH_CoT(CSTR);
		virtual ~McHdr_Ext_MH_CoT();
		static McHdr_Ext_MH_CoT *create(CSTR);

		int32_t mobilityType() const {
			return(TP_Ext_MH_CoT);
		}
};

class McHdr_Ext_MH_BU: public McHdr_Ext_MH {
	public:
		McHdr_Ext_MH_BU(CSTR);
		virtual ~McHdr_Ext_MH_BU();
		static McHdr_Ext_MH_BU *create(CSTR);

		int32_t mobilityType() const {
			return(TP_Ext_MH_BU);
		}
};

class McHdr_Ext_MH_BA: public McHdr_Ext_MH {
	public:
		McHdr_Ext_MH_BA(CSTR);
		virtual ~McHdr_Ext_MH_BA();
		static McHdr_Ext_MH_BA *create(CSTR);

		int32_t mobilityType() const {
			return(TP_Ext_MH_BA);
		}
};

class McHdr_Ext_MH_BE: public McHdr_Ext_MH {
	public:
		McHdr_Ext_MH_BE(CSTR);
		virtual ~McHdr_Ext_MH_BE();
		static McHdr_Ext_MH_BE *create(CSTR);

		int32_t mobilityType() const {
			return(TP_Ext_MH_BE);
		}
};

//
// header = xx (reference header on the McHdr_Ext_MH_ONE)
//
////////////////////////////////////////////////////////////////////////////////

class MmHeader_onMH: public MmReference_Must1 {
	static TypevsMcDict dict_;	// mibilytyType(MH) vs McHdr_Ext_MH

	public:
		MmHeader_onMH(CSTR);
		virtual ~MmHeader_onMH();

		int32_t token() const {
			return(metaToken(tkn_extent_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add(McHdr_Ext_MH *);
		static void add_other(McHdr_Ext_MH *);

		// COMPOSE/REVERSE INTERFACE --------------------------------------------------
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};

//
//  Option for MH
//
////////////////////////////////////////////////////////////////////////////////

//
//  abstruct class
//
////////////////////////////////////////////////////////////////////////////////

class McOpt_MH: public McOption {
	protected:
		MmUint *type_;

		void type_member(MmUint *meta) {
			type_ = meta;
			member(meta);
		}

		MmUint *length_;

		void length_member(MmUint *meta) {
			length_ = meta;
			member(meta);
		}

		void common_member();
		McOpt_MH(CSTR);

	public:
		virtual ~McOpt_MH();

		// COMPOSE/REVERSE INTERFACE --------------------------------------------------
		virtual uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;

		//HardCording action method
		DEC_HCGENE(Length);
		DEC_HCGENE(Type);
		DEC_HCEVAL(Type);
		int32_t get_mobilityOptionType(WObject *) const;
};

//
//  any optionType Format (for unknown option type)
//
////////////////////////////////////////////////////////////////////////////////

class McOpt_MH_ANY: public McOpt_MH {
	public:
		McOpt_MH_ANY(CSTR);
		virtual ~McOpt_MH_ANY();
		static McOpt_MH_ANY *create(CSTR);
};

class McOpt_MH_Pad1: public McOpt_MH {
	public:
		McOpt_MH_Pad1(CSTR);
		virtual ~McOpt_MH_Pad1();
		static McOpt_MH_Pad1 *create(CSTR);

		int32_t optionType() const {
			return(TP_Opt_MH_Pad1);
		}
};

class McOpt_MH_PadN: public McOpt_MH {
	McOpt_MH_PadN(CSTR);

	public:
		virtual ~McOpt_MH_PadN();
		static McOpt_MH_PadN *create(CSTR);

		int32_t optionType() const {
			return(TP_Opt_MH_PadN);
		}
};

class McOpt_MH_AlternateCoA: public McOpt_MH {
	public:
		McOpt_MH_AlternateCoA(CSTR);
		virtual ~McOpt_MH_AlternateCoA();
		static McOpt_MH_AlternateCoA *create(CSTR);

		int32_t optionType() const {
			return(TP_Opt_MH_AlternateCoA);
		}
};

class McOpt_MH_NonceIndices: public McOpt_MH {
	public:
		McOpt_MH_NonceIndices(CSTR);
		virtual ~McOpt_MH_NonceIndices();
		static McOpt_MH_NonceIndices *create(CSTR);

		int32_t optionType() const {
			return(TP_Opt_MH_NonceIndices);
		}
};

class McOpt_MH_BindingAuthData: public McOpt_MH {
	public:
		McOpt_MH_BindingAuthData(CSTR);
		virtual ~McOpt_MH_BindingAuthData();
		static McOpt_MH_BindingAuthData *create(CSTR);

		virtual PObject *tokenObject(int, CSTR) const;

		int32_t optionType() const {
			return(TP_Opt_MH_BindingAuthData);
		}
};

class McOpt_MH_BindingRefreshAdvice: public McOpt_MH {
	public:
		McOpt_MH_BindingRefreshAdvice(CSTR);
		virtual ~McOpt_MH_BindingRefreshAdvice();
		static McOpt_MH_BindingRefreshAdvice *create(CSTR);

		int32_t optionType() const {
			return(TP_Opt_MH_BindingRefreshAdvice);
		}
};

class McOpt_MH_MobNetworkPrefix: public McOpt_MH {
	public:
		McOpt_MH_MobNetworkPrefix(CSTR);
		virtual ~McOpt_MH_MobNetworkPrefix();
		static McOpt_MH_MobNetworkPrefix *create(CSTR);

		int32_t optionType() const {
			return(TP_Opt_MH_MobNetworkPrefix);
		}
};

//
//  option = xx (reference option(MH) on the McHdr_Ext_MH_XX)
//
////////////////////////////////////////////////////////////////////////////////

class MmOption_onMH: public MmReference_More0 {
	static TypevsMcDict dict_;	// optionType(MH) vs McOpt_MH_XX

	public:
		MmOption_onMH(CSTR);
		virtual ~MmOption_onMH();

		int32_t token() const {
			return(metaToken(tkn_option_ref_));
		}

		const TypevsMcDict *get_dict() const {
			return(&dict_);
		}

		static void add(McOpt_MH *);
		static void add_other(McOpt_MH *);

		// COMPOSE/REVERSE INTERFACE --------------------------------------------------
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////////////////////
class MmMH_Cookie64: public MmOctets {
	public:
		MmMH_Cookie64(CSTR);
		virtual ~MmMH_Cookie64();

		// COMPOSE/REVERSE PROTECTED MmUint -------------------------------------------
		virtual PvObject *reversePv(RControl &, const ItPosition &, const ItPosition &, const OCTBUF &) const;
};

////////////////////////////////////////////////////////////////////////////////
class PaBSA;
class BSAList;
class KeepBSA {
	private:
//		const PaBSA *bsa_;
		BSAList *bsa_;
		uint32_t current_;
	public:
		KeepBSA();
		virtual ~KeepBSA();

		const PaBSA *GetBSA(void);
		void SetBSA(const PaBSA *);
};

////////////////////////////////////////////////////////////////////////////////
class MmMH_Auth: public MmOctets {
	private:
		KeepBSA *keepbsa_;
	public:
		MmMH_Auth(CSTR);
		virtual ~MmMH_Auth();

		// COMPOSE/REVERSE PROTECTED MmUint -------------------------------------------
		virtual PvObject *reversePv(RControl &, const ItPosition &, const ItPosition &, const OCTBUF &) const;
		virtual RObject *reverseRm(RControl &, RObject *, const ItPosition &, const ItPosition &, PvObject *) const;
		virtual WObject *composeWm(WControl &, WObject *, const PObject *) const;
		virtual RObject *reverse(RControl &, RObject *, ItPosition &, OCTBUF &) const;
		virtual bool generate(WControl &, WObject *, OCTBUF &) const;
};

////////////////////////////////////////////////////////////////////////////////
class PfBSA: public PvFunction {
	private:
		const MfBSA *meta_;
		OCTSTR context_;
	public:
		PfBSA(const MfBSA *, CSTR, int);
		virtual ~PfBSA();
		const MfBSA *metaClass() const;
		virtual const MObject *meta() const;
		void init();
		void update(const OCTBUF &);
		PvOctets *result();
};

inline const MfBSA *PfBSA::metaClass() const {
	return(meta_);
}

////////////////////////////////////////////////////////////////////////////////
class PaBSA: public PcObject {
	private:
		PfBSA *bsa_;
	public:
		PaBSA(const MObject*, CSTR, int);
		virtual ~PaBSA();

		//----------------------------------------------------------------------
		// YAC INTERFACE
		virtual PObject *bsa_member(PObject *);

		virtual OCTBUF *BSA_Calculate(const OCTBUF &) const;
		virtual WObject *selfCompose(WControl &, WObject *) const;
};

////////////////////////////////////////////////////////////////////////////////
class RmBSA: public RmObject {
	const PaBSA *bsa_;
	PvObject *calc_pvalue_;

	public:
		RmBSA(RObject *, const MObject *, const ItPosition &, const ItPosition &, PvObject *, const PaBSA *);
		virtual ~RmBSA();
		void set_calc_pvalue(PvObject *);

		PvObject *calc_pvalue() const {
			return(calc_pvalue_);
		}

		virtual void post_reverse(Con_IPinfo &, RControl &, RObject *);
};

////////////////////////////////////////////////////////////////////////////////
class WmBSA: public WmObject {
	const PaBSA *bsa_;

	public:
		WmBSA(WObject *, const MObject *, const PObject *, const PaBSA *);
		virtual ~WmBSA();
		virtual void post_generate(Con_IPinfo &, WControl &, OCTBUF &, WObject *);
		virtual bool doEvaluate(WControl &, RObject &);
};

interfaceCmList(BSAList, PaBSA);
#endif

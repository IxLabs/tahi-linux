/*
 * Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011
 * Yokogawa Electric Corporation, 
 * INTAP (Interoperability Technology Association for Information Processing, Japan),
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
 * $TAHI: v6eval/lib/Pz/PControl.h,v 1.34 2005/05/09 09:35:24 akisada Exp $
 */
#if !defined(__WControl_h__)
#define __WControl_h__

#include "McMobility.h"
#include "McDHCPv6.h"
#include "McIKE.h"
#include "pzCommon.h"

//////////////////////////////////////////////////////////////////////////////

class Con_DictType{
private:
	enum {		uneffect_,finish_,other_,effect_};
	int		state_;
	int32_t		type_;
public:
	Con_DictType():state_(uneffect_),type_(0){}
	~Con_DictType(){}
	bool		uneffect()const{return state_==uneffect_;}
	bool		finish()const{return state_==finish_;}
	bool		other()const{return state_==other_;}
	bool		effect()const{return state_==effect_;}
	int32_t		type()const{return type_;} //only effect
//
	void		uneffetct_Set(){state_=uneffect_; type_=0;}
	void		finish_Set(){state_=finish_; type_=0;}
	void		other_Set(){state_=other_; type_=0;}
	void		type_Set(int32_t type){state_=effect_; type_=type;}
	void 		reset() {state_=uneffect_; type_=0;}
};


//////////////////////////////////////////////////////////////////////////////

class PaObject;
class Con_ESPinfo{
private:
	uint32_t	padlen_;
	uint32_t	payloadlen_;
public:
	Con_ESPinfo():padlen_(0),payloadlen_(0){}
	~Con_ESPinfo(){}
	void		set_PadLength(uint32_t len){padlen_=len;}
	uint32_t	get_PadLength()const{return padlen_;}
	void		set_PayloadLength(uint32_t len){payloadlen_=len;}
	uint32_t	get_PayloadLength()const{return payloadlen_;}
	void 		reset(){padlen_=0; payloadlen_=0;}
};

class Con_ISAKMPinfo: public Con_ESPinfo {
	public:
		Con_ISAKMPinfo(): Con_ESPinfo() {}
		~Con_ISAKMPinfo() {}
};

//////////////////////////////////////////////////////////////////////////////
class PvObject;
class TObject;
class RObject;
class WObject;
class RControl;
class WControl;
class ICVControl;
class ItPosition;

class Con_IPinfo{
//AddrInfo
	const PvObject* SrcAddr_;
	const PvObject* DstAddr_;
	const PvObject* LastDstAddr_;
	bool		Route_isLeft_;
//IPChecksum
	TObject*	postIPChecksum_;
//AHICV
	TObject*	postAHICV_;
//UppChecksum
	TObject*	postUppChecksum_;
//BSA
	TObject*	postBSA_;
	TObject	*postP2_HASH_2_;
	TObject*	postDHCPAuth_;
public:
	Con_IPinfo():
		SrcAddr_(0),DstAddr_(0),LastDstAddr_(0),Route_isLeft_(false),
		postIPChecksum_(0),postAHICV_(0),postUppChecksum_(0),postBSA_(0), postP2_HASH_2_(0), postDHCPAuth_(0) {}
	~Con_IPinfo(){}
public:
	void		SrcAddr(const PvObject* v){SrcAddr_=v;}
	const PvObject* SrcAddr()const{return SrcAddr_;}
	void		DstAddr(const PvObject* v){DstAddr_=v; LastDstAddr(v);}
	const PvObject* DstAddr()const{return DstAddr_;}
	void		LastDstAddr(const PvObject* v){LastDstAddr_=v;}
	const PvObject* LastDstAddr()const{return LastDstAddr_;}
	void		Route_isLeft(bool v){Route_isLeft_=v;}
	bool		Route_isLeft()const{return Route_isLeft_;}
//
	void		postIPChecksum(TObject* t){postIPChecksum_=t;}
	TObject*	postIPChecksum()const{return postIPChecksum_;}
	void		reverse_postIPChecksum(RControl&,RObject* hdr);
	void		generate_postIPChecksum(WControl&,OCTBUF&,WObject* hdr);
//
	void		postAHICV(TObject* t){postAHICV_=t;}
	TObject*	postAHICV()const{return postAHICV_;}
	void		reverse_postAHICV(RControl&,RObject* pkt);
	void		generate_postAHICV(WControl&,OCTBUF& buf,WObject* pkt);
//
	void		postUppChecksum(TObject* t){postUppChecksum_=t;}
	TObject*	postUppChecksum()const{return postUppChecksum_;}
	void		reverse_postUppChecksum(RControl&,RObject* up);
	void		generate_postUppChecksum(WControl&,OCTBUF&,WObject* up);
	void		generate_postUppChecksumWithLength(WControl &, OCTBUF &, WObject *, uint32_t);

	void		postBSA(TObject* t){postBSA_=t;}
	TObject*	postBSA()const{return postBSA_;}

	void		postP2_HASH_2(TObject *t) {
				postP2_HASH_2_ = t; return; }
	TObject*	postP2_HASH_2() const { return(postP2_HASH_2_); }

	void		postDHCPAuth(TObject *t) {postDHCPAuth_ = t; return;}
	TObject*	postDHCPAuth() const {return(postDHCPAuth_);}

	void		reverse_postBSA(RControl&,RObject* up);
	void		reverse_postP2_HASH_2(RControl &, RObject *);
	void		reverse_postDHCPAuth(RControl &, RObject *);
	void		generate_postBSA(WControl&,OCTBUF&,WObject* up);
	void		generate_postP2_HASH_2(WControl &, OCTBUF &, WObject *);
	void		generate_postDHCPAuth(WControl &, OCTBUF &, WObject *);
//
	void		print();
};


//////////////////////////////////////////////////////////////////////////////

class PControl {
private:
	bool	error_;
	int32_t resultcode_;
//
	Con_IPinfo*	IPinfo_;
public:
	PControl();
virtual	~PControl();
//
	operator bool() const;		//true:ok	false:error/warning
	bool error() const;		//true:error	false:warning/ok
	bool warning() const;		//true:warning	false:error/ok
	int32_t resultcode() const;	//error code or warnig code
//
	void set_error(int32_t ecode);
	void set_warning(int32_t wcode);//canot set warning if allready error
	void clear_error();		//clear error and warning state
//OLD INTERFACE
	void result(int32_t ecode);	//forward set_error()
//
virtual	void reset();
	Con_IPinfo*	IPinfo(){return IPinfo_;}
	void set_IPinfo(Con_IPinfo* cn){IPinfo_=cn;}
};

inline PControl::operator bool() const {return !error_;}
inline bool PControl::error() const {return error_;}
inline bool PControl::warning() const {return !error_&&resultcode_!=0;}
inline int32_t PControl::resultcode() const {return resultcode_;}
inline void PControl::set_error(int32_t ecode) {error_=true; resultcode_=ecode;}
inline void PControl::set_warning(int32_t wcode) {if(!error_)resultcode_=wcode;}
inline void PControl::clear_error() {error_=false; resultcode_=0;}
inline void PControl::result(int32_t r) {set_error(r);}

class WControl:public PControl {
private:
//indicate log,debug
	bool	logging_;
	bool	debug_;
//return action(stop to evaluate)
	bool	stop_;
//to compose children(only compose)
	const class TypevsMcDict*	dict_;	//check reference next Mc*
	const PaObject*	pushing_SA_;		//now pushing SA
	const PaBSA *pushing_BSA_;		//now pushing BSA
	const PaP2_HASH_2 *pushing_P2_HASH_2_;
	const PaDHCPAuth *pushing_DHCPAuth_;
public:
	WControl(bool logging=false,bool debug=false);
virtual	~WControl();
	bool logging() const;
	bool debug() const;
	bool msg() const;
	void setStop();
	void clearStop();
	bool stop() const;
virtual	void reset();
//
	const TypevsMcDict* dict() const{return dict_;}
	void dict_set(const TypevsMcDict* d){dict_=d;}
	const PaObject* pushing_SA()const{return pushing_SA_;}
	void pushing_SA(const PaObject* sa){pushing_SA_=sa;}

	const PaBSA *pushing_BSA()const{return pushing_BSA_;}
	const PaP2_HASH_2 *pushing_P2_HASH_2() const {
		return(pushing_P2_HASH_2_); }

	void pushing_BSA(const PaBSA *bsa){pushing_BSA_=bsa;}
	void pushing_P2_HASH_2(const PaP2_HASH_2 *p2_hash_2) {
		pushing_P2_HASH_2_ = p2_hash_2; return; }
	const PaDHCPAuth *pushing_DHCPAuth()const{return pushing_DHCPAuth_;}
	void pushing_DHCPAuth(const PaDHCPAuth *dhcpauth){pushing_DHCPAuth_=dhcpauth;}
// SA compose
	void set_push_SA(const PaObject *);
	void set_push_BSA(const PaBSA *);
	void set_push_P2_HASH_2(const PaP2_HASH_2 *);
	void set_push_DHCPAuth(const PaDHCPAuth *);
protected:
virtual	void	push_SA(const PaObject* sa);
};
inline bool WControl::logging() const {return logging_;}
inline bool WControl::debug() const {return (!logging_&&debug_);}
inline bool WControl::msg() const {return (logging_||debug_);}
inline void WControl::setStop() {stop_=true;}
inline void WControl::clearStop() {stop_=false;}
inline bool WControl::stop() const {return stop_;}

class RControl:public PControl {
private:
	Con_DictType	DictType_;	//decide next Mc*
	uint32_t	SPI_;		//select(pop) SA (with IPinfo)
	const PaObject*	poping_SA_;	//now poping SA
	Con_ESPinfo	ESPinfo_;	//reverse ESP's members(Mm)
	Con_ISAKMPinfo	ISAKMPinfo_;	//reverse ISAKMP's members(Mm)
public:
	RControl();
virtual	~RControl();
virtual	void reset();
//
	Con_DictType&	DictType(){return DictType_;}
	uint32_t&	SPI(){return SPI_;}
	const PaObject* poping_SA()const{return poping_SA_;}
	void poping_SA(const PaObject* sa){poping_SA_=sa;}
	Con_ESPinfo&	ESPinfo(){return ESPinfo_;}
	Con_ISAKMPinfo&	ISAKMPinfo(){return ISAKMPinfo_;}
// SA
	void		set_pop_SA(eAlgorithm); //internal poping_SA_=pop_SA( SPI_ )
//
virtual	void unmatchMessage(CSTR,const PvObject*,const PvObject*);
protected:
virtual	const PaObject*	pop_SA(eAlgorithm,uint32_t =0,const PvObject* =0);
};

class AlgorithmList;
class WSAControl:public WControl {
private:
	AlgorithmList* algorithms_;
public:
	WSAControl(AlgorithmList*);
virtual	~WSAControl();
	AlgorithmList* algorithms() const;
virtual	void push_SA(const PaObject*);
virtual	void reset();
};
inline AlgorithmList* WSAControl::algorithms() const {return algorithms_;}

class AlgorithmList;
class RSAControl:public RControl {
private:
	uint32_t current_;
	AlgorithmList* algorithms_;
public:
	RSAControl(AlgorithmList*);
virtual	~RSAControl();
	AlgorithmList* algorithms() const;
virtual	const PaObject*	pop_SA(eAlgorithm,uint32_t =0,const PvObject* =0);
virtual	void reset();
};
inline AlgorithmList* RSAControl::algorithms() const {return algorithms_;}

///////////////////////////////////////////////////////////////////////////////
// overwrite ICV Controler
class ICVControl{
	const PvObject*	LastDstAddr_;
	const PvObject* CareOfAddr_;	// MIP6AH
	const PvObject* HomeAddr_;	// MIP6AH
	const PvObject*	ShiftRoutAddr_;
	uint32_t	RoutAddrmax_;
	uint32_t	RoutAddrindex_;
	uint32_t	SegmentsLeft_;
	//
	bool		isZero_ExtOptData_;
public:
	ICVControl(const PvObject* lastdst=0):LastDstAddr_(lastdst),
	CareOfAddr_((const PvObject*)0),	// MIP6AH
	HomeAddr_((const PvObject*)0),		// MIP6AH
	ShiftRoutAddr_(0),RoutAddrmax_(0),RoutAddrindex_(0),SegmentsLeft_(0),
	isZero_ExtOptData_(0){}
public:
	void            CareOfAddr(const PvObject* v){CareOfAddr_=v;}	// MIP6AH
	const PvObject* CareOfAddr()const{return CareOfAddr_;}		// MIP6AH
	void            HomeAddr(const PvObject* v){HomeAddr_=v;}	// MIP6AH
	const PvObject* HomeAddr()const{return HomeAddr_;}		// MIP6AH
	void		LastDstAddr(const PvObject* v){LastDstAddr_=v;}
	const PvObject*	LastDstAddr()const{return LastDstAddr_;}
	void		ShiftRoutAddr(const PvObject* v){ShiftRoutAddr_=v;}
	const PvObject*	ShiftRoutAddr()const{return ShiftRoutAddr_;}
	void		RoutAddrmax(uint32_t v){RoutAddrmax_=v;}
	uint32_t	RoutAddrmax()const{return RoutAddrmax_;}
	void		RoutAddrindex(uint32_t v){RoutAddrindex_=v;}//[0,max-1]
	uint32_t	RoutAddrindex()const{return RoutAddrindex_;}
	void		inc_RoutAddrindex(){++RoutAddrindex_;}
	void		SegmentsLeft(uint32_t v){SegmentsLeft_=v;}
	uint32_t	SegmentsLeft()const{return SegmentsLeft_;}
	bool		isShift_RoutAddr()const{
				if(RoutAddrmax_<SegmentsLeft_)return false;
				uint32_t shiftpos = RoutAddrmax_-SegmentsLeft_;
				return RoutAddrindex_>=shiftpos;}
	//
	void		isZero_ExtOptData(bool z){isZero_ExtOptData_=z;}
	bool		isZero_ExtOptData()const{return isZero_ExtOptData_;}
	};


//////////////////////////////////////////////////////////////////////////////
#endif

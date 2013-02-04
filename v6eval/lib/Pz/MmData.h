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
 * $TAHI: v6eval/lib/Pz/MmData.h,v 1.5 2001/10/12 04:56:16 tanaka Exp $
 */
#if !defined(__MmData_h__)
#define	__MmData_h__	1
#include "MmObject.h"

//////////////////////////////////////////////////////////////////////////////
// data = substr(xx) (variable data member)
// data = value
// data = {values}
// data = repeat()
// COMPOSE(more than 0) if notdefine ,must compose a void data
// REVERSE(must 1)
class MmData:public MmObject {
	bool	evalskip_;
	const ICVoverwriter*	ICVoverwriter_;
public:
	MmData(CSTR,bool evalskip=false,const ICVoverwriter* ow=0);
virtual ~MmData();
	int32_t token() const;
virtual bool encodeOctets(WControl&,const ItPosition&,OCTBUF&,const PvOctets&)const;
virtual bool encodeNumber(WControl&,const ItPosition&,OCTBUF&,const PvNumber&)const;
virtual uint32_t objectLength(const PObject*,const WObject* =0) const;
// COMPOSE/REVERSE INTERFACE --------------------------------------------------
virtual WObject* compose(WControl&,WObject* w_parent,const PObject* pl)const;
virtual RObject* reverse(RControl&,RObject* r_parent,ItPosition&,OCTBUF&) const;
// COMPOSE/REVERSE PROTECTED MmData* ------------------------------------------
virtual void composeList(WControl&,
		WObject* w_parent,const PObjectList& pls)const;
virtual uint32_t length_for_reverse(RControl&,ItPosition& at,OCTBUF& buf)const;
virtual WObject* composeWm(WControl&,WObject* w_parent,const PObject* pl)const;
virtual RObject* reverseRm(RControl&,RObject* r_parent,
		const ItPosition& at,const ItPosition& size,PvObject* pv)const;
virtual bool generate(WControl&,WObject*,OCTBUF&) const;
virtual bool disused() const {return evalskip_;}
virtual bool overwrite_ICV(ICVControl&,
		const ItPosition& at,OCTBUF& buf,const TObject* t)const;
};

#include "WObject.h"
class WmData:public WtNode{
private:
	PvObject*	data_;
public:
	WmData(WObject*,const MObject*,const PObject* =0);
virtual ~WmData();
virtual	bool doEvaluate(WControl&,RObject&);
virtual void internalEvaluatePrologue(ItPosition&);
};


#endif

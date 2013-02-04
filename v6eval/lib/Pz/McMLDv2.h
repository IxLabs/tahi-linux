/*
 * Copyright (C) 1999, 2000, and 2001 Yokogawa Electric Corporation,
 * YDC Corporation, IPA (Information-technology Promotion Agency,Japan)
 * Copyright (C) IBM Corporation 2003.
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
 *    Author: Kazuo Hiekata <hiekata@yamato.ibm.com>
 */

#if !defined(__McMLDv2_h__)
#define	__McMLDv2_h__	1

#include "McSub.h"
#include "MmObject.h"
#include "PvOctets.h"
#include "McObject.h"

const int32_t TP_MLDv2_MODE_IS_INCLUDE			= 1;
const int32_t TP_MLDv2_MODE_IS_EXCLUDE			= 2;
const int32_t TP_MLDv2_CHANGE_TO_INCLUDE_MODE	= 3;
const int32_t TP_MLDv2_CHANGE_TO_EXCLUDE_MODE	= 4;
const int32_t TP_MLDv2_ALLOW_NEW_SOURCES		= 5;
const int32_t TP_MLDv2_BLOCK_OLD_SOURCES		= 6;

////////////////////////////////////////////////////////////////
class McMLDv2_AddrRecord: public McUpper {
	protected:
		MmUint *type_;
		void type_member(MmUint *meta) {
			type_ = meta;
			member(meta);
		}

		MmUint *auxdatalen_;
		void auxdatalen_member(MmUint *meta) {
			auxdatalen_ = meta;
			member(meta);
		}

		MmUint *numofsrc_;
		void numofsrc_member(MmUint *meta) {
			numofsrc_ = meta;
			member(meta);
		}
		
		MmV6Addr *mcastaddr_;
		void mcastaddr_member(MmV6Addr *meta) {
			mcastaddr_ = meta;
			member(meta);
		}
		
		void common_member();
	public:
		McMLDv2_AddrRecord(CSTR);
		~McMLDv2_AddrRecord();

		uint32_t length_for_reverse(RControl &, ItPosition &, OCTBUF &) const;
		bool overwrite_DictType(RControl &, ItPosition &, OCTBUF &) const;
		
		//HardCording action method
		DEC_HCGENE(NumOfSources);
		DEC_HC_MLC(SourceAddress);
		
		static McMLDv2_AddrRecord *create(CSTR);
};
#endif

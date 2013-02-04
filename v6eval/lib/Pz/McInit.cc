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
 * $TAHI: v6eval/lib/Pz/McInit.cc,v 1.95 2009/11/25 08:54:41 akisada Exp $
 */

#include "McSub.h"
#include "McNull.h"
#include "McEther.h"
#include "McIPv6.h"
#include "McICMPv6.h"
#include "McMobility.h"
#include "McRR.h"
#include "McIPv4.h"
#include "McICMPv4.h"
#include "McARP.h"
#include "McNoNext.h"
#include "McUDP.h"
#include "McTCP.h"
#include "McAlgorithm.h"
#include "McESP.h"
#include "McAH.h"
#include "McRIPng.h"
#include "McSIP.h"
#include "McDNS.h"
#include "McDHCPv6.h"
#include "McMLDv2.h"
#include "McIGMP.h"
#if !defined(NOT_USE_SNMP)
#include "McSNMP.h"
#endif // NOT_USE_SNMP
#include "McVRRP.h"
#include "McIKE.h"

#include "MmObject.h"
#include "MmChecksum.h"
#include "MmHeader.h"
#include "MmData.h"
#include "PvObject.h"
#include "PvOctets.h"
#include "PvAction.h"
#include "PvAutoItem.h"
#include "PControl.h"
#include "WObject.h"
#include <stdio.h>

#define	UN(n) PvNumber::unique(n)
#define	V6TN()PvV6Addr::TN()
#define	V6NUT()PvV6Addr::NUT()
#define	ETTN()PvEther::TN()
#define	ETNUT()PvEther::NUT()
#define	MUST()PvMUSTDEF::must()
#define EVALANY()	new PvANY()
#define EVALZERO()	new PvOctets(0, 0)
#define GENEHC(mc,cls,mem)	new PvHCgene(mc,(HCgenefunc)&cls::HCGENE(mem))
#define EVALHC(mc,cls,mem)	new PvHCeval(mc,(HCevalfunc)&cls::HCEVAL(mem))

#define ICVCONST()	0
#define ICVZERO()	new ICV_Zero()
#define ICVHC(mc,cls,mem)	new ICV_HC(mc,(METH_HC_OWICV)&cls::HC_OWICV(mem))
#define ICVOPTDATA(mc,cls)	ICVHC(mc,cls,OptExtData)

#define DEF_EVALSKIP	true
#define DEF_EVALGO	false
#define DEF_OPTCHKSUM	true
#define DEF_MUSTCHKSUM	false
#define DEF_NOPSEUDO	false
#define DEF_MUSTPSEUDO	true

McObject *
McObject::addCompound(McObject *mc)
{
	mc->confirmed();
	set_.add(mc);

	return(mc);
}

#define LEXADD(classname,lexname) addCompound(classname::create(lexname))
#define LEXADD2(classname,lexname,tophdrnm)		\
	addCompound(classname::create(lexname,tophdrnm))
#define LEXADD5(classname,lexname,tophdrnm0,tophdrnm1,tophdrnm2,tophdrnm3)	\
	addCompound(classname::create(lexname,tophdrnm0,tophdrnm1,tophdrnm2,tophdrnm3))
#define MCCR(classname,lexname) classname::create(lexname)

McObject *
McObject::initialize()
{
	static bool hasBeenHere = false;

	if(hasBeenHere) {
		return(0);
	}

	hasBeenHere = true;

	/* Payload */
	LEXADD(McPayload,	"Payload");

	/* DataLinkLayer */
	// Ether
	McObject *mc =
	LEXADD2(McFrame_Ether,	"Frame_Ether",	"Hdr_Ether");
	LEXADD2(McFrame_Null,	"Frame_Null",	"Hdr_Null");

/* NetworkLayer */
	// IPv6
	LEXADD2(McPacket_IPv6,			"Packet_IPv6", "Hdr_IPv6" );
	 
	LEXADD(McHdr_Ext_HopByHop,		"Hdr_HopByHop" );
	LEXADD(McHdr_Ext_Destination,		"Hdr_Destination" );
	LEXADD(McHdr_Ext_Routing,		"Hdr_Routing" );
	LEXADD(McHdr_Ext_Fragment,		"Hdr_Fragment" );
	 
	LEXADD(McOpt_Ext_ANY,			"Opt_ANY" );
	LEXADD(McOpt_Ext_Pad1,			"Opt_Pad1" );
	LEXADD(McOpt_Ext_PadN,			"Opt_PadN" );
	LEXADD(McOpt_Ext_JumboPayload,		"Opt_JumboPayload" );
	LEXADD(McOpt_Ext_RouterAlert,		"Opt_RouterAlert" );
	LEXADD(McOpt_Ext_TunnelEncap,		"Opt_TunnelEncapslation" );

	LEXADD(McOpt_Ext_HomeAddress,		"Opt_HomeAddress" );

	// IPsec
	LEXADD(McHdr_Ext_AH,			"Hdr_AH" );
	LEXADD(McAlgorithm_AH,			"AHAlgorithm" );
	LEXADD(McHdr_Ext_ESP,			"Hdr_ESP" );
	LEXADD(McAlgorithm_ESP,			"ESPAlgorithm" );

	// IPv4
	LEXADD2(McPacket_IPv4,			"Packet_IPv4","Hdr_IPv4" );
	McOpt_IPv4::create_options();

	//ARP
 	LEXADD2(McPacket_ARP,			"Packet_ARP","Hdr_ARP" );
	//RARP
	LEXADD2(McPacket_RARP,			"Packet_RARP","Hdr_RARP" );

/* UpperLayer */
	//NoNext
	LEXADD(McUpp_NoNext,			"Upp_NoNext" );

	// VRRP
	LEXADD(McUpp_VRRPvX,	"Upp_VRRPvX");
	LEXADD(McUpp_VRRPv2,	"Upp_VRRPv2");
	LEXADD(McUpp_VRRPv3,	"Upp_VRRPv3");

	//ICMPv6
	LEXADD(McUpp_ICMPv6_ANY,			"ICMPv6_ANY" );
	LEXADD(McUpp_ICMPv6_RS,				"ICMPv6_RS" );
	LEXADD(McUpp_ICMPv6_RA,				"ICMPv6_RA" );
	LEXADD(McUpp_ICMPv6_NS,				"ICMPv6_NS" );
	LEXADD(McUpp_ICMPv6_NA,				"ICMPv6_NA" );
	LEXADD(McUpp_ICMPv6_Redirect,			"ICMPv6_Redirect" );
	LEXADD(McUpp_ICMPv6_MLDQuery,			"ICMPv6_MLDQuery");
#ifndef NOT_USE_MLDV2_QUERY
	LEXADD(McUpp_ICMPv6_MLDv2Query,			"ICMPv6_MLDv2Query");
#endif	// NOT_USE_MLDV2_QUERY
	LEXADD(McUpp_ICMPv6_MLDReport,			"ICMPv6_MLDReport");
	LEXADD(McUpp_ICMPv6_MLDDone,			"ICMPv6_MLDDone");
	LEXADD(McUpp_ICMPv6_MLDv2Report,		"ICMPv6_MLDv2Report");
	LEXADD(McUpp_ICMPv6_EchoRequest,		"ICMPv6_EchoRequest" );
	LEXADD(McUpp_ICMPv6_EchoReply,			"ICMPv6_EchoReply" );
	LEXADD(McUpp_ICMPv6_PacketTooBig,		"ICMPv6_PacketTooBig" );
	LEXADD(McUpp_ICMPv6_DestinationUnreachable,
		"ICMPv6_DestinationUnreachable");

	LEXADD(McUpp_ICMPv6_TimeExceeded,		"ICMPv6_TimeExceeded" );
	LEXADD(McUpp_ICMPv6_ParameterProblem,
		"ICMPv6_ParameterProblem");

	LEXADD(McUpp_ICMPv6_RouterRenumbering,
		"ICMPv6_RouterRenumbering");

//	LEXADD(McUpp_ICMPv6_NameLookups,		"ICMPv6_NameLookups" );

	LEXADD(McUpp_ICMPv6_HAADRequest,		"ICMPv6_HAADRequest");
	LEXADD(McUpp_ICMPv6_HAADReply,			"ICMPv6_HAADReply");
	LEXADD(McUpp_ICMPv6_MobilePrefixSolicit,
		"ICMPv6_MobilePrefixSol");

	LEXADD(McUpp_ICMPv6_MobilePrefixAdvertise,
		"ICMPv6_MobilePrefixAdvertise");
	
	LEXADD(McOpt_ICMPv6_ANY,		"Opt_ICMPv6_ANY" );
	LEXADD(McOpt_ICMPv6_SLL,		"Opt_ICMPv6_SLL" );
	LEXADD(McOpt_ICMPv6_TLL,		"Opt_ICMPv6_TLL" );
	LEXADD(McOpt_ICMPv6_Prefix,		"Opt_ICMPv6_Prefix" );
	LEXADD(McOpt_ICMPv6_Redirected,		"Opt_ICMPv6_Redirected" );
	LEXADD(McOpt_ICMPv6_MTU,		"Opt_ICMPv6_MTU" );
	LEXADD(McOpt_ICMPv6_AdvInterval,	"Opt_ICMPv6_AdvInterval");
	LEXADD(McOpt_ICMPv6_HomeAgentInfo,	"Opt_ICMPv6_HomeAgentInfo");
	LEXADD(McOpt_ICMPv6_RouteInfo,		"Opt_ICMPv6_RouteInfo");

	// Mobility Header
	LEXADD(McHdr_Ext_MH_ANY,	"Hdr_MH_ANY");
	LEXADD(McHdr_Ext_MH_BRR,	"Hdr_MH_BRR");
	LEXADD(McHdr_Ext_MH_HoTI,	"Hdr_MH_HoTI");
	LEXADD(McHdr_Ext_MH_CoTI,	"Hdr_MH_CoTI");
	LEXADD(McHdr_Ext_MH_HoT,	"Hdr_MH_HoT");
	LEXADD(McHdr_Ext_MH_CoT,	"Hdr_MH_CoT");
	LEXADD(McHdr_Ext_MH_BU,		"Hdr_MH_BU");
	LEXADD(McHdr_Ext_MH_BA,		"Hdr_MH_BA");
	LEXADD(McHdr_Ext_MH_BE,		"Hdr_MH_BE");

	// Mobility Options
	LEXADD(McOpt_MH_ANY,			"Opt_MH_ANY");
	LEXADD(McOpt_MH_Pad1,			"Opt_MH_Pad1");
	LEXADD(McOpt_MH_PadN,			"Opt_MH_PadN");
	LEXADD(McOpt_MH_BindingRefreshAdvice,	"Opt_MH_BindingRefreshAdvice");
	LEXADD(McOpt_MH_AlternateCoA,		"Opt_MH_AlternateCoA");
	LEXADD(McOpt_MH_NonceIndices,		"Opt_MH_NonceIndices");
	LEXADD(McOpt_MH_BindingAuthData,	"Opt_MH_BindingAuthData");
	LEXADD(McOpt_MH_MobNetworkPrefix,	"Opt_MH_MobNetworkPrefix");

	//ICMPv4
	LEXADD(McUpp_ICMPv4_ANY,			"ICMPv4_ANY" );
	LEXADD(McUpp_ICMPv4_DestinationUnreachable,
		"ICMPv4_DestinationUnreachable" );

	LEXADD(McUpp_ICMPv4_TimeExceeded,	"ICMPv4_TimeExceeded" );
	LEXADD(McUpp_ICMPv4_ParameterProblem,	"ICMPv4_ParameterProblem" );
	LEXADD(McUpp_ICMPv4_Redirect,	 	"ICMPv4_Redirect" );
	LEXADD(McUpp_ICMPv4_EchoRequest,	"ICMPv4_EchoRequest" );
	LEXADD(McUpp_ICMPv4_EchoReply,		"ICMPv4_EchoReply" );

	//IGMP
	LEXADD(McUpp_IGMP_ANY,			"IGMP_ANY" );
	LEXADD(McUpp_IGMP_IGMPQuery,		"IGMP_IGMPQuery" );
	LEXADD(McUpp_IGMP_IGMPv1Report,		"IGMP_IGMPv1Report" );
	LEXADD(McUpp_IGMP_IGMPv2Report,		"IGMP_IGMPv2Report" );
	LEXADD(McUpp_IGMP_IGMPv2LeaveGroup,	"IGMP_IGMPv2LeaveGroup" );
	LEXADD(McUpp_IGMP_IGMPv3Report,		"IGMP_IGMPv3Report" );

	//UDP
	LEXADD2(McUpp_UDP,			"Upp_UDP",	"Hdr_UDP");

	// RIPng
	LEXADD(McUdp_RIPng,			"Udp_RIPng");
	LEXADD(McRIPng_RTE,			"RIPng_RTE");

	// DNS
	LEXADD5(McUdp_DNS,			"Udp_DNS", "DNS_Question", "DNS_Answer", "DNS_Authority", "DNS_Additional");
	LEXADD(McDNS_QuestionEntry,		"DNS_QuestionEntry");
	LEXADD(McDNS_RR_ANY,			"DNS_RR_ANY");

	// RFC 1035 - Domain Implementation and Specification
	LEXADD(McDNS_RR_A,			"DNS_RR_A");
	LEXADD(McDNS_RR_NS,			"DNS_RR_NS");
	LEXADD(McDNS_RR_MD,			"DNS_RR_MD");
	LEXADD(McDNS_RR_MF,			"DNS_RR_MF");
	LEXADD(McDNS_RR_CNAME,			"DNS_RR_CNAME");
	LEXADD(McDNS_RR_SOA,			"DNS_RR_SOA");
	LEXADD(McDNS_RR_MB,			"DNS_RR_MB");
	LEXADD(McDNS_RR_MG,			"DNS_RR_MG");
	LEXADD(McDNS_RR_MR,			"DNS_RR_MR");
	LEXADD(McDNS_RR_NULL,			"DNS_RR_NULL");
	LEXADD(McDNS_RR_WKS,			"DNS_RR_WKS");
	LEXADD(McDNS_RR_PTR,			"DNS_RR_PTR");
	LEXADD(McDNS_RR_HINFO,			"DNS_RR_HINFO");
	LEXADD(McDNS_RR_MINFO,			"DNS_RR_MINFO");
	LEXADD(McDNS_RR_MX,			"DNS_RR_MX");
	LEXADD(McDNS_RR_TXT,			"DNS_RR_TXT");

	// RFC 1183 - New DNS RR Definitions
	LEXADD(McDNS_RR_RP,			"DNS_RR_RP");
	LEXADD(McDNS_RR_AFSDB,			"DNS_RR_AFSDB");
	LEXADD(McDNS_RR_X25,			"DNS_RR_X25");
	LEXADD(McDNS_RR_ISDN,			"DNS_RR_ISDN");
	LEXADD(McDNS_RR_RT,			"DNS_RR_RT");

	// RFC 1664 - Internet DNS for Mail Mapping Tables
	LEXADD(McDNS_RR_PX,			"DNS_RR_PX");

	// RFC 1886 - IPv6 DNS Extensions
	LEXADD(McDNS_RR_AAAA,			"DNS_RR_AAAA");

	// RFC 2782 - DNS SRV RR
	LEXADD(McDNS_RR_SRV,			"DNS_RR_SRV");

	// RFC 2915 - NAPTR DNS RR
	LEXADD(McDNS_RR_NAPTR,			"DNS_RR_NAPTR");

	// DHCPv6
	LEXADD(McUdp_DHCPv6_ANY,		"Udp_DHCPv6_ANY");
	LEXADD(McUdp_DHCPv6_Solicit,		"Udp_DHCPv6_Solicit");
	LEXADD(McUdp_DHCPv6_Advertise,		"Udp_DHCPv6_Advertise");
	LEXADD(McUdp_DHCPv6_Request,		"Udp_DHCPv6_Request");
	LEXADD(McUdp_DHCPv6_Confirm,		"Udp_DHCPv6_Confirm");
	LEXADD(McUdp_DHCPv6_Renew,		"Udp_DHCPv6_Renew");
	LEXADD(McUdp_DHCPv6_Rebind,		"Udp_DHCPv6_Rebind");
	LEXADD(McUdp_DHCPv6_Reply,		"Udp_DHCPv6_Reply");
	LEXADD(McUdp_DHCPv6_Release,		"Udp_DHCPv6_Release");
	LEXADD(McUdp_DHCPv6_Decline,		"Udp_DHCPv6_Decline");
	LEXADD(McUdp_DHCPv6_Reconfigure,	"Udp_DHCPv6_Reconfigure");
	LEXADD(McUdp_DHCPv6_InformationRequest,	"Udp_DHCPv6_InformationRequest");
	LEXADD(McUdp_DHCPv6_RelayForward,	"Udp_DHCPv6_RelayForward");
	LEXADD(McUdp_DHCPv6_RelayReply,		"Udp_DHCPv6_RelayReply");

	// DHCP options
	LEXADD(McOpt_DHCPv6_ANY,		"Opt_DHCPv6_ANY");
	LEXADD(McOpt_DHCPv6_CID,		"Opt_DHCPv6_CID");
	LEXADD(McOpt_DHCPv6_SID,		"Opt_DHCPv6_SID");
	LEXADD(McOpt_DHCPv6_IA_NA,		"Opt_DHCPv6_IA_NA");
	LEXADD(McOpt_DHCPv6_IA_TA,		"Opt_DHCPv6_IA_TA");
	LEXADD(McOpt_DHCPv6_IA_Address,		"Opt_DHCPv6_IA_Address");
	LEXADD(McOpt_DHCPv6_OptionRequest,	"Opt_DHCPv6_OptionRequest");
	LEXADD(McOpt_DHCPv6_Preference,		"Opt_DHCPv6_Preference");
	LEXADD(McOpt_DHCPv6_ElapsedTime,	"Opt_DHCPv6_ElapsedTime");
	LEXADD(McOpt_DHCPv6_RelayMessage,	"Opt_DHCPv6_RelayMessage");
	LEXADD(McOpt_DHCPv6_Authentication,	"Opt_DHCPv6_Authentication");
	LEXADD(McOpt_DHCPv6_ServerUnicast,	"Opt_DHCPv6_ServerUnicast");
	LEXADD(McOpt_DHCPv6_StatusCode,		"Opt_DHCPv6_StatusCode");
	LEXADD(McOpt_DHCPv6_RapidCommit,	"Opt_DHCPv6_RapidCommit");
	LEXADD(McOpt_DHCPv6_UserClass,		"Opt_DHCPv6_UserClass");
	LEXADD(McOpt_DHCPv6_VendorClass,	"Opt_DHCPv6_VendorClass");
	LEXADD(McOpt_DHCPv6_VendorSpecificInfo,
		"Opt_DHCPv6_VendorSpecificInfo");

	LEXADD(McOpt_DHCPv6_IID,		"Opt_DHCPv6_IID");
	LEXADD(McOpt_DHCPv6_ReconfigureMessage,
		"Opt_DHCPv6_ReconfigureMessage");

	LEXADD(McOpt_DHCPv6_ReconfigureAccept,	"Opt_DHCPv6_ReconfigureAccept");
	LEXADD(McOpt_DHCPv6_IA_PD,		"Opt_DHCPv6_IA_PD");
	LEXADD(McOpt_DHCPv6_IA_Prefix,		"Opt_DHCPv6_IA_Prefix");
	LEXADD(McOpt_DHCPv6_DNS_Servers,	"Opt_DHCPv6_DNS_Servers");
	LEXADD(McOpt_DHCPv6_DNS_SearchList,	"Opt_DHCPv6_DNS_SearchList");
	LEXADD(McOpt_DHCPv6_NTP_Servers,	"Opt_DHCPv6_NTP_Servers");
	LEXADD(McOpt_DHCPv6_NTP_Timezone,	"Opt_DHCPv6_NTP_Timezone");

	// DHCP unique identifier
	LEXADD(McDHCPv6_DUID_ANY,		"DHCPv6_DUID_ANY");
	LEXADD(McDHCPv6_DUID_LLT_ANY,		"DHCPv6_DUID_LLT_ANY");
	LEXADD(McDHCPv6_DUID_LLT_Ether,		"DHCPv6_DUID_LLT_Ether");
	LEXADD(McDHCPv6_DUID_EN,		"DHCPv6_DUID_EN");
	LEXADD(McDHCPv6_DUID_LL_ANY,		"DHCPv6_DUID_LL_ANY");
	LEXADD(McDHCPv6_DUID_LL_Ether,		"DHCPv6_DUID_LL_Ether");

	// user class data
	LEXADD(McDHCPv6_UserClassData,		"DHCPv6_UserClassData");

	// vendor class data
	LEXADD(McDHCPv6_VendorClassData,	"DHCPv6_VendorClassData");

	// option data
	LEXADD(McDHCPv6_OptionData,		"DHCPv6_OptionData");

	// Authentication of DHCP Messages
	LEXADD(McDHCPv6_Auth_ANY,		"DHCPv6_Auth_ANY");

	// Delayed Authentication Protocol
	LEXADD(McDHCPv6_Auth_Delayed,		"DHCPv6_Auth_Delayed");

	// Reconfigure Key Authentication Protocol
	LEXADD(McDHCPv6_Auth_ReconfigureKey,	"DHCPv6_Auth_ReconfigureKey");

	// SIP
	LEXADD(McUdp_SIP,			"Udp_SIP");

	// MLDv2 Address Record
	LEXADD(McMLDv2_AddrRecord,	"MLDv2_AddrRecord");
	
	// IGMPv3 Address Record
	LEXADD(McIGMPv3_AddrRecord,	"IGMPv3_AddrRecord");
	
#if !defined(NOT_USE_SNMP)
	// SNMP and SNMP PDUs
	LEXADD(McUdp_SNMP,		"Udp_SNMP");
	LEXADD(McUdp_SNMPTrap,		"Udp_SNMPTrap");
	LEXADD(McPdu_SNMP_Message,	"Pdu_SNMP_Message");
	LEXADD(McPdu_SNMP_Version,	"Pdu_SNMP_Version");
	LEXADD(McPdu_SNMP_Community,	"Pdu_SNMP_Community");
	LEXADD(McPdu_SNMP_GetRequest,	"Pdu_SNMP_GetRequest");
	LEXADD(McPdu_SNMP_GetNextRequest,	"Pdu_SNMP_GetNextRequest");
	LEXADD(McPdu_SNMP_SetRequest,	"Pdu_SNMP_SetRequest");
	LEXADD(McPdu_SNMP_GetResponse,	"Pdu_SNMP_GetResponse");
	LEXADD(McPdu_SNMP_Trap,	"Pdu_SNMP_Trap");
	LEXADD(McPdu_SNMP_GetBulk,	"Pdu_SNMP_GetBulk");
	LEXADD(McPdu_SNMP_Inform,	"Pdu_SNMP_Inform");
	LEXADD(McPdu_SNMP_Trap2,	"Pdu_SNMP_Trap2");
	LEXADD(McPdu_SNMP_Report,	"Pdu_SNMP_Report");
	LEXADD(McPdu_SNMP_RequestID,	"Pdu_SNMP_RequestID");
	LEXADD(McPdu_SNMP_ErrorStatus,	"Pdu_SNMP_ErrorStatus");
	LEXADD(McPdu_SNMP_ErrorIndex,	"Pdu_SNMP_ErrorIndex");
	LEXADD(McPdu_SNMP_GenericTrap,	"Pdu_SNMP_GenericTrap");
	LEXADD(McPdu_SNMP_SpecificTrap,	"Pdu_SNMP_SpecificTrap");
	LEXADD(McPdu_SNMP_VarBindList,	"Pdu_SNMP_VarBindList");
	LEXADD(McPdu_SNMP_VarBind,		"Pdu_SNMP_VarBind");
	LEXADD(McPdu_SNMP_ANY,	"Pdu_SNMP_ANY");
	LEXADD(McPdu_SNMP_Boolean,	"Pdu_SNMP_Boolean");
	LEXADD(McPdu_SNMP_Integer,	"Pdu_SNMP_Integer");
	LEXADD(McPdu_SNMP_BitString,	"Pdu_SNMP_BitString");
	LEXADD(McPdu_SNMP_OctString,	"Pdu_SNMP_OctString");
	LEXADD(McPdu_SNMP_Null,	"Pdu_SNMP_Null");
	LEXADD(McPdu_SNMP_ObjectID,	"Pdu_SNMP_ObjectID");
	LEXADD(McPdu_SNMP_Sequence,	"Pdu_SNMP_Sequence");
	LEXADD(McPdu_SNMP_Set,	"Pdu_SNMP_Set");
	LEXADD(McPdu_SNMP_Opaque,	"Pdu_SNMP_Opaque");
	LEXADD(McPdu_SNMP_IPAddress,	"Pdu_SNMP_IPAddress");
	LEXADD(McPdu_SNMP_Counter,	"Pdu_SNMPCounter");
	LEXADD(McPdu_SNMP_Gauge,	"Pdu_SNMP_Gauge");
	LEXADD(McPdu_SNMP_TimeTicks,	"Pdu_SNMP_TimeTicks");
#endif //NOT_USE_SNMP

	//TCP
	LEXADD2(McUpp_TCP,			"Upp_TCP","Hdr_TCP" );
	McOpt_TCP::create_options();

	// IKE
	LEXADD2(McUdp_ISAKMP,			"Udp_ISAKMP", "Hdr_ISAKMP");
	LEXADD(McISAKMP_Encryption,		"ISAKMP_Encryption");
	LEXADD(McISAKMP_Algorithm,		"ISAKMP_Algorithm" );
	LEXADD(McPld_ISAKMP_ANY,		"Pld_ISAKMP_ANY");
	LEXADD(McPld_ISAKMP_SA_ANY,		"Pld_ISAKMP_SA_ANY");
	LEXADD(McPld_ISAKMP_SA_IPsec_ANY,	"Pld_ISAKMP_SA_IPsec_ANY");
	LEXADD(McPld_ISAKMP_SA_IPsec_IDonly,	"Pld_ISAKMP_SA_IPsec_IDonly");
	LEXADD(McPld_ISAKMP_P,			"Pld_ISAKMP_P_ANY");
	LEXADD(McPld_ISAKMP_P_ISAKMP,		"Pld_ISAKMP_P_ISAKMP");
	LEXADD(McPld_ISAKMP_P_IPsec_AH,		"Pld_ISAKMP_P_IPsec_AH");
	LEXADD(McPld_ISAKMP_P_IPsec_ESP,	"Pld_ISAKMP_P_IPsec_ESP");
	LEXADD(McPld_ISAKMP_P_IPCOMP,		"Pld_ISAKMP_P_IPCOMP");
	LEXADD(McPld_ISAKMP_T,			"Pld_ISAKMP_T");
	LEXADD(McPld_ISAKMP_KE,			"Pld_ISAKMP_KE");
	LEXADD(McPld_ISAKMP_ID,			"Pld_ISAKMP_ID_ANY");
	LEXADD(McPld_ISAKMP_ID_IPV4_ADDR,	"Pld_ISAKMP_ID_IPV4_ADDR");
	LEXADD(McPld_ISAKMP_ID_FQDN,		"Pld_ISAKMP_ID_FQDN");
	LEXADD(McPld_ISAKMP_ID_USER_FQDN,	"Pld_ISAKMP_ID_USER_FQDN");
	LEXADD(McPld_ISAKMP_ID_IPV4_ADDR_SUBNET,
		"Pld_ISAKMP_ID_IPV4_ADDR_SUBNET");

	LEXADD(McPld_ISAKMP_ID_IPV6_ADDR,	"Pld_ISAKMP_ID_IPV6_ADDR");
	LEXADD(McPld_ISAKMP_ID_IPV6_ADDR_SUBNET,
		"Pld_ISAKMP_ID_IPV6_ADDR_SUBNET");

	LEXADD(McPld_ISAKMP_ID_IPV4_ADDR_RANGE,
		"Pld_ISAKMP_ID_IPV4_ADDR_RANGE");

	LEXADD(McPld_ISAKMP_ID_IPV6_ADDR_RANGE,
		"Pld_ISAKMP_ID_IPV6_ADDR_RANGE");

	LEXADD(McPld_ISAKMP_ID_DER_ASN1_DN,	"Pld_ISAKMP_ID_DER_ASN1_DN");
	LEXADD(McPld_ISAKMP_ID_DER_ASN1_GN,	"Pld_ISAKMP_ID_DER_ASN1_GN");
	LEXADD(McPld_ISAKMP_ID_KEY_ID,		"Pld_ISAKMP_ID_KEY_ID");
	LEXADD(McPld_ISAKMP_CERT,		"Pld_ISAKMP_CERT");
	LEXADD(McPld_ISAKMP_CR,			"Pld_ISAKMP_CR");
	LEXADD(McPld_ISAKMP_HASH,		"Pld_ISAKMP_HASH");
	LEXADD(McPld_ISAKMP_SIG,		"Pld_ISAKMP_SIG");
	LEXADD(McPld_ISAKMP_NONCE,		"Pld_ISAKMP_NONCE");
	LEXADD(McPld_ISAKMP_N_ANY,		"Pld_ISAKMP_N_ANY");
	LEXADD(McPld_ISAKMP_N_ISAKMP,		"Pld_ISAKMP_N_ISAKMP");
	LEXADD(McPld_ISAKMP_N_IPsec_ANY,	"Pld_ISAKMP_N_IPsec_ANY");
	LEXADD(McPld_ISAKMP_N_IPsec_ResponderLT,
		"Pld_ISAKMP_N_IPsec_ResponderLT");
	LEXADD(McPld_ISAKMP_N_IPsec_ReplayStat,
		"Pld_ISAKMP_N_IPsec_ReplayStat");
	LEXADD(McPld_ISAKMP_D,			"Pld_ISAKMP_D");
	LEXADD(McPld_ISAKMP_VID,		"Pld_ISAKMP_VID");

	LEXADD(McAttr_ISAKMP,			"Attr_ISAKMP_ANY");
	LEXADD(McAttr_ISAKMP_TLV,		"Attr_ISAKMP_TLV");
	LEXADD(McAttr_ISAKMP_TV,		"Attr_ISAKMP_TV");

	//
	return(mc);
}



/////// Mc creater implementaion /////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
McPayload* McPayload::create(CSTR key){
	McPayload* mc = new McPayload(key);
	mc->member( new MmData(	"data" ) );
	// dict
	MmPayload::add_other(mc);		//payload=
	MmFrame::add_other(mc);			//frame=
	MmPacket_onEther::add_other(mc);	//Frame_Ether::packet=
	MmPacket_onNull::add_other(mc);		//Frame_Null::packet=
	MmUpper_onIP::add_other(mc);		//Packet_IP::upper=
	MmUpper_onUpper::add_other(mc);		// for forward compatibility
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McFrame_Ether* McFrame_Ether::create(CSTR key,CSTR tophdrkey){
	addCompound(tophdr_ = McTopHdr_Ether::create(tophdrkey));
	McFrame_Ether* mc = new McFrame_Ether(key);
	mc->member( new MmTopHdr(		"header",tophdr_ ) );
	mc->member( new MmPacket_onEther(	"packet" ) );
	// dict
	MmFrame::add(mc);			//frame=
	return mc;}

McTopHdr_Ether* McTopHdr_Ether::create(CSTR key){
	McTopHdr_Ether* mc = new McTopHdr_Ether(key);
	mc->member( new MmEther("DestinationAddress",	ETNUT(),ETTN()));
	mc->member( new MmEther("SourceAddress",	ETTN(),	ETNUT()));
	mc->nextType_member(
		    new MmUint(	"Type", 16, GENEHC(mc,McTopHdr_Ether,Type),EVALHC(mc,McTopHdr_Ether,Type) ) );
	// no dict
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McPacket_IPv6* McPacket_IPv6::create(CSTR key,CSTR tophdrkey){
	addCompound(tophdr_ = McTopHdr_IPv6::create(tophdrkey));
	McPacket_IPv6* mc = new McPacket_IPv6(key);
	mc->member( new MmTopHdr(	"header",tophdr_) );
	mc->member( new MmExtent_onIP(	"exthdr" ) );
	mc->member( new MmUpper_onIP(	"upper" ) );
	// dict
	MmPacket_onNull::add(mc);		//Frame_Null::packet=
	MmPacket_onEther::add(mc);		//Frame_Ether::packet=
	MmUpper_onIP::add(mc);			//Packet_IP::upper=(tunnel)
	return mc;}

McTopHdr_IPv6* McTopHdr_IPv6::create(CSTR key){
	McTopHdr_IPv6* mc = new McTopHdr_IPv6(key);
	mc->member( new MmUint( "Version",	4,
			UN(6),	UN(6),		ICVCONST() ) );
	mc->member( new MmUint( "TrafficClass",	8,
			UN(0),	EVALANY(),	ICVZERO() ) );
	mc->member( new MmUint( "FlowLabel",	20,
			UN(0),	EVALANY(),	ICVZERO() ) );
	mc->Layerlength_member(
		    new MmUint(	"PayloadLength",16,
			GENEHC(mc,McTopHdr_IPv6,PayloadLength),
			EVALANY(),		ICVCONST() ) );
	mc->nextType_member(
		    new MmUint( "NextHeader",	8,
			GENEHC(mc,McTopHdr_IPv6,NextHeader),
			EVALHC(mc,McTopHdr_IPv6,NextHeader),	ICVCONST() ) );
	mc->member( new MmUint( "HopLimit",	8,
			UN(64),	EVALANY(),	ICVZERO() ) );
	mc->member( new MmV6Addr( "SourceAddress",
			V6TN(),V6NUT(),		ICVHC(mc, McTopHdr_IPv6, SourceAddress),	// MIP6AH
			(METH_HC_ForIPinfo)&McTopHdr_IPv6::HC_ForIPinfo(SourceAddress) ) );
	mc->member( new MmV6Addr( "DestinationAddress",
			V6NUT(),V6TN(),		ICVHC(mc,McTopHdr_IPv6,DestinationAddress), 
			(METH_HC_ForIPinfo)&McTopHdr_IPv6::HC_ForIPinfo(DestinationAddress) ) );
	// no dict
	return mc;}


//////////////////////////////////////////////////////////////////////////////
McHdr_Ext_HopByHop* McHdr_Ext_HopByHop::create(CSTR key){
	McHdr_Ext_HopByHop* mc = new McHdr_Ext_HopByHop(key);
	mc->nextType_member(
		    new MmUint( "NextHeader",	8,
			GENEHC(mc,McHdr_Ext_HopByHop,NextHeader),
			EVALHC(mc,McHdr_Ext_HopByHop,NextHeader),	ICVCONST() ) );
	mc->length_member(
		    new MmUint( "HeaderExtLength",8,
			GENEHC(mc,McHdr_Ext_HopByHop,HeaderExtLength),
			EVALANY(),		ICVCONST() ) );
	mc->member( new MmOption_onExt("option") );
	// dict
	MmExtent_onIP::add(mc);		//Packet_IP::exthdr=
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McHdr_Ext_Destination* McHdr_Ext_Destination::create(CSTR key){
	McHdr_Ext_Destination* mc = new McHdr_Ext_Destination(key);
	mc->nextType_member(
		    new MmUint( "NextHeader",	8,
			GENEHC(mc,McHdr_Ext_Destination,NextHeader),
			EVALHC(mc,McHdr_Ext_Destination,NextHeader),	ICVCONST() ) );
	mc->length_member(
		    new MmUint( "HeaderExtLength",8,
			GENEHC(mc,McHdr_Ext_Destination,HeaderExtLength),
			EVALANY(),		ICVCONST() ) );
	mc->member( new MmOption_onExt("option") );
	// dict
	MmExtent_onIP::add(mc);		//Packet_IP::exthdr=
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McHdr_Ext_Routing* McHdr_Ext_Routing::create(CSTR key){
	McHdr_Ext_Routing* mc = new McHdr_Ext_Routing(key);
	mc->nextType_member(
		    new MmUint( "NextHeader",	8,
			GENEHC(mc,McHdr_Ext_Routing,NextHeader),
			EVALHC(mc,McHdr_Ext_Routing,NextHeader),	ICVCONST() ) );
	mc->length_member(
		    new MmUint( "HeaderExtLength",8,
			GENEHC(mc,McHdr_Ext_Routing,HeaderExtLength),
			EVALANY(),		ICVCONST() ) );
	mc->member( new MmUint( "RoutingType",	8,
			UN(0),	UN(0),		ICVCONST() ) );
	mc->member( new MmUint( "SegmentsLeft",	8,
			UN(0),	UN(0),		ICVHC(mc,McHdr_Ext_Routing,SegmentsLeft) ,
			(METH_HC_ForIPinfo)&McHdr_Ext_Routing::HC_ForIPinfo(SegmentsLeft) ) );
	mc->member( new MmUint( "Reserved",	32,
			UN(0),	UN(0),		ICVCONST() ) );
	mc->member( new MmMultiple(
			new MmV6Addr( "Address",
				MUST(),MUST(),	ICVHC(mc,McHdr_Ext_Routing,Address), 
				(METH_HC_ForIPinfo)&McHdr_Ext_Routing::HC_ForIPinfo(Address) ),
			(METH_HC_MLC)&McHdr_Ext_Routing::HC_MLC(Address) ) );
	// dict
	MmExtent_onIP::add(mc);		//Packet_IP::exthdr=
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McHdr_Ext_Fragment* McHdr_Ext_Fragment::create(CSTR key){
	McHdr_Ext_Fragment* mc = new McHdr_Ext_Fragment(key);
	mc->nextType_member(
		    new MmUint( "NextHeader",	8,	MUST(),	MUST() ) );
	mc->member( new MmUint( "Reserved1",	8,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "FragmentOffset",13,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "Reserved2",	2,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "MFlag",	1,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "Identification",32,	UN(0),	UN(0) ) );
	// dict
	MmExtent_onIP::add(mc);		//Packet_IP::exthdr=
	return mc;}


//////////////////////////////////////////////////////////////////////////////
void McOpt_Ext::common_member(){
	int32_t Type = optionType();
	type_member(
		new MmUint( "OptionType",	8,
			UN(Type),UN(Type),	ICVCONST() ) );
	length_member(
		new MmUint( "OptDataLength",	8,
			GENEHC(this,McOpt_Ext,OptDataLength),
			EVALANY(),		ICVCONST() ) );
	}

McOpt_Ext_ANY* McOpt_Ext_ANY::create(CSTR key){
	McOpt_Ext_ANY* mc = new McOpt_Ext_ANY(key);
	mc->common_member();
	mc->member( new MmData( "data" ,DEF_EVALGO,	ICVOPTDATA(mc,McOpt_Ext_ANY) ) );
	// dict
	MmOption_onExt::add_other(mc);	//Hdr_Ext_XX::option=
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McOpt_Ext_Pad1* McOpt_Ext_Pad1::create(CSTR key){
	McOpt_Ext_Pad1* mc = new McOpt_Ext_Pad1(key);
	int32_t Type = mc->optionType();
	mc->type_member(
		new MmUint( "OptionType",	8,
			UN(Type),UN(Type),	ICVCONST() ) );
	// dict
	MmOption_onExt::add(mc);		//Hdr_Ext_XX::option=
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McOpt_Ext_PadN* McOpt_Ext_PadN::create(CSTR key){
	McOpt_Ext_PadN* mc = new McOpt_Ext_PadN(key);
	mc->common_member();
	mc->member( new MmData( "pad",	DEF_EVALSKIP,	ICVOPTDATA(mc,McOpt_Ext_PadN) ) );
	// dict
	MmOption_onExt::add(mc);		//Hdr_Ext_XX::option=
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McOpt_Ext_JumboPayload* McOpt_Ext_JumboPayload::create(CSTR key){
	McOpt_Ext_JumboPayload* mc = new McOpt_Ext_JumboPayload(key);
	mc->common_member();
	mc->member( new MmUint( "JumboPayloadLength",32,
			GENEHC(mc,McOpt_Ext_JumboPayload,JumboPayloadLength),
			EVALANY(),		ICVOPTDATA(mc,McOpt_Ext_JumboPayload) ) );
	// dict
	MmOption_onExt::add(mc);		//Hdr_Ext_XX::option=
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McOpt_Ext_RouterAlert* McOpt_Ext_RouterAlert::create(CSTR key){
	McOpt_Ext_RouterAlert* mc = new McOpt_Ext_RouterAlert(key);
	mc->common_member();
	mc->member( new MmUint( "Value",	16,
			MUST(),	MUST(),		ICVOPTDATA(mc,McOpt_Ext_RouterAlert) ) );
	// dict
	MmOption_onExt::add(mc);		//Hdr_Ext_XX::option=
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McOpt_Ext_TunnelEncap* McOpt_Ext_TunnelEncap::create(CSTR key){
	McOpt_Ext_TunnelEncap* mc = new McOpt_Ext_TunnelEncap(key);
	mc->common_member();
	mc->member( new MmUint( "Limit",	8,
			UN(0),	UN(0),		ICVOPTDATA(mc,McOpt_Ext_TunnelEncap) ) );
	// dict
	MmOption_onExt::add(mc);		//Hdr_Ext_XX::option=
	return mc;}



////////////////////////////////////////
//  Home Address Destination Option   //
////////////////////////////////////////
McOpt_Ext_HomeAddress *
McOpt_Ext_HomeAddress::create(CSTR key)
{
	McOpt_Ext_HomeAddress *mc = new McOpt_Ext_HomeAddress(key);

	mc->common_member();
	mc->member(
		new MmV6Addr(
			"HomeAddress",
			MUST(),
			MUST(),
			ICVHC(mc, McOpt_Ext_HomeAddress, HomeAddress),
			(METH_HC_ForIPinfo)&McOpt_Ext_HomeAddress::HC_ForIPinfo(
				HomeAddress
			)
		)
	);

	MmOption_onExt::add(mc);

	return(mc);
}

//////////////////////////////////////////////////////////////////////////////
McPacket_IPv4* McPacket_IPv4::create(CSTR key,CSTR tophdrkey){
	addCompound(tophdr_ = McTopHdr_IPv4::create(tophdrkey));
	McPacket_IPv4* mc = new McPacket_IPv4(key);
	mc->member( new MmTopHdr(	"header",tophdr_) );
	mc->member( new MmExtent_onIP(	"exthdr" ) );
	mc->member( new MmUpper_onIP(	"upper" ) );
	// dict
	MmPacket_onNull::add(mc);		//Frame_Null::packet=
	MmPacket_onEther::add(mc);		//Frame_Ether::packet=
	MmUpper_onIP::add(mc);			//Packet_IP::upper=(tunnel)
	return mc;}

McTopHdr_IPv4* McTopHdr_IPv4::create(CSTR key){
	McTopHdr_IPv4* mc = new McTopHdr_IPv4(key);
	mc->member( new MmUint( "Version",	4,
			UN(4),	UN(4),		ICVCONST() ) );
	mc->length_member(
		    new MmUint( "IHL",		4,
			GENEHC(mc,McTopHdr_IPv4,IHL),
			EVALANY(),		ICVCONST() ) );
	mc->member( new MmUint( "TypeOfService",8,
			UN(0),	EVALANY(),	ICVZERO() ) );
	mc->Layerlength_member(
		    new MmUint(	"TotalLength",	16,
			GENEHC(mc,McTopHdr_IPv4,TotalLength),
			EVALANY(),		ICVCONST() ) );
	mc->member( new MmUint( "Identifier",	16,
			UN(0),	EVALANY(),	ICVCONST() ) );
	mc->Flags_member(
		    new MmUint( "Reserved",	1,
			UN(0),	UN(0),		ICVZERO() ) );
	mc->Flags_member(
		    new MmUint( "DF",		1,
			UN(0),	EVALANY(),	ICVZERO() ) );
	mc->Flags_member(
		    new MmUint( "MF",		1,
 			UN(0),	UN(0),		ICVZERO() ) );
	mc->fragoffset_member(
		    new MmUint( "FragmentOffset",13,
			UN(0),	UN(0),		ICVZERO() ) );
	mc->member( new MmUint( "TTL",		8,
			UN(255),EVALANY(),	ICVZERO() ) );
	mc->nextType_member(
		    new MmUint( "Protocol",	8,
			GENEHC(mc,McTopHdr_IPv4,Protocol),
			EVALHC(mc,McTopHdr_IPv4,Protocol),	ICVCONST() ) );
	mc->member( new MmIPChecksum( "HeaderChecksum",16,
			DEF_MUSTCHKSUM, 	ICVZERO() ) );
	mc->member( new MmV4Addr( "SourceAddress",
			MUST(),MUST(),		ICVCONST(),
			(METH_HC_ForIPinfo)&McTopHdr_IPv4::HC_ForIPinfo(SourceAddress) ) );
	mc->member( new MmV4Addr( "DestinationAddress",
			MUST(),MUST(),		ICVHC(mc,McTopHdr_IPv4,DestinationAddress),
			(METH_HC_ForIPinfo)&McTopHdr_IPv4::HC_ForIPinfo(DestinationAddress) ) );
	mc->member( new MmOption_onIPv4( "option" ) );
	mc->member( new MmData(	"Padding",DEF_EVALSKIP,	ICVCONST() ) );
	// no dict
	return mc;}


//////////////////////////////////////////////////////////////////////////////
McUpp_NoNext* McUpp_NoNext::create(CSTR key){
	McUpp_NoNext* mc = new McUpp_NoNext(key);
	mc->member( new MmData(	"data" ) );
	// dict
	MmUpper_onIP::add(mc);			//Packet_IP::upper=
	return mc;}



//////////////////////////////////////////////////////////////////////////////
void
McUpp_VRRP::common_member()
{
	int32_t Version = vrrpVersion();

	version_member(new MmUint("Version",	4, UN(Version), UN(Version)));
	member(new MmUint("Type",		4, UN(1), UN(1)));
}

McUpp_VRRPvX *
McUpp_VRRPvX::create(CSTR key)
{
	McUpp_VRRPvX *mc = new McUpp_VRRPvX(key);

	mc->common_member();
	mc->member(new MmData("data"));

	MmHeader_onVRRP::add_other(mc);

	return(mc);
}

McUpp_VRRPv2 *
McUpp_VRRPv2::create(CSTR key)
{
	McUpp_VRRPv2 *mc = new McUpp_VRRPv2(key);

	mc->common_member();

	mc->member(new MmUint("ID",		8, UN(0), UN(0)));
	mc->member(new MmUint("Priority",	8, UN(0), UN(0)));
	mc->member(new MmUint("Count",		8, UN(0), UN(0)));
	mc->member(new MmUint("AuthType",	8, UN(0), UN(0)));
	mc->member(new MmUint("Interval",	8, UN(0), UN(0)));
	mc->member(new MmUppChecksum(
		"Checksum", 16, DEF_MUSTCHKSUM, DEF_NOPSEUDO));

	mc->member(
		new MmMultiple(
			new MmV4Addr(
				"Address",
				MUST(),
				MUST()
			),
			(METH_HC_MLC)&McUpp_VRRPv2::HC_MLC(Address)
		)
	);

	MmHeader_onVRRP::add(mc);

	return(mc);
}

McUpp_VRRPv3 *
McUpp_VRRPv3::create(CSTR key)
{
	McUpp_VRRPv3 *mc = new McUpp_VRRPv3(key);

	mc->common_member();

	mc->member(new MmUint("ID",		8, UN(0), UN(0)));
	mc->member(new MmUint("Priority",	8, UN(0), UN(0)));
	mc->member(new MmUint("Count",		8, UN(0), UN(0)));
	mc->member(new MmUint("Reserved",	4, UN(0), UN(0)));
	mc->member(new MmUint("Interval",	12, UN(0), UN(0)));
	mc->member(new MmUppChecksum(
		"Checksum", 16, DEF_MUSTCHKSUM, DEF_MUSTPSEUDO));

	mc->member(
		new MmMultiple(
			new MmV6Addr(
				"Address",
				MUST(),
				MUST()
			),
			(METH_HC_MLC)&McUpp_VRRPv3::HC_MLC(Address)
		)
	);

	MmHeader_onVRRP::add(mc);

	return(mc);
}



//////////////////////////////////////////////////////////////////////////////
void McUpp_ICMPv6::common_member(){
	int32_t Type = icmpv6Type();
	type_member(
		new MmUint( "Type",		8,	UN(Type),UN(Type) ) );
	Code_member(
		new MmUint( "Code",		8,	UN(0),	UN(0) ) );
	member( new MmUppChecksum( "Checksum",	16 ) );
	//isEqual(DEF_MUSTCHKSUM,DEF_MUSTPSEUDO,ICVCONST)
	}

McUpp_ICMPv6_ANY* McUpp_ICMPv6_ANY::create(CSTR key){
	McUpp_ICMPv6_ANY* mc = new McUpp_ICMPv6_ANY(key);
	mc->common_member();
	mc->member( new MmData( "data" ) );
	// dict
	MmHeader_onICMPv6::add_other(mc);	//Upp_ICMPv6::header= (upper=)
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_RS* McUpp_ICMPv6_RS::create(CSTR key){
	McUpp_ICMPv6_RS* mc = new McUpp_ICMPv6_RS(key);
	mc->common_member();
	mc->member( new MmUint( "Reserved",	32,	UN(0),	UN(0) ) );
	mc->member( new MmOption_onICMPv6( "option" ) );
	// dict
	MmHeader_onICMPv6::add(mc);		//Upp_ICMPv6::header= (upper=)
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_RA *
McUpp_ICMPv6_RA::create(CSTR key)
{
	McUpp_ICMPv6_RA *mc = new McUpp_ICMPv6_RA(key);
	mc->common_member();
	mc->member(new MmUint("CurHopLimit",	8,	UN(0),	UN(0)));
	mc->member(new MmUint("MFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("OFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("HFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("Preference",	2,	UN(0),	UN(0)));
	mc->member(new MmUint("Reserved",	3,	UN(0),	UN(0)));
	mc->member(new MmUint("LifeTime",	16,	UN(0),	UN(0)));
	mc->member(new MmUint("ReachableTime",	32,	UN(0),	UN(0)));
	mc->member(new MmUint("RetransTimer",	32,	UN(0),	UN(0)));

	mc->member(new MmOption_onICMPv6("option"));
	// dict
	MmHeader_onICMPv6::add(mc);		//Upp_ICMPv6::header= (upper=)

	return(mc);
}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_NS* McUpp_ICMPv6_NS::create(CSTR key){
	McUpp_ICMPv6_NS* mc = new McUpp_ICMPv6_NS(key);
	mc->common_member();
	mc->member( new MmUint( "Reserved",	32,	UN(0),	UN(0) ) );
	mc->member( new MmV6Addr( "TargetAddress",	V6NUT(),V6TN()));
	mc->member( new MmOption_onICMPv6( "option" ) );
	// dict
	MmHeader_onICMPv6::add(mc);		//Upp_ICMPv6::header= (upper=)
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_NA* McUpp_ICMPv6_NA::create(CSTR key){
	McUpp_ICMPv6_NA* mc = new McUpp_ICMPv6_NA(key);
	mc->common_member();
	mc->member( new MmUint( "RFlag",	1,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "SFlag",	1,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "OFlag",	1,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "Reserved",	29,	UN(0),	UN(0) ) );
	mc->member( new MmV6Addr( "TargetAddress",	V6TN(),	V6NUT()));
	mc->member( new MmOption_onICMPv6( "option" ) );
	// dict
	MmHeader_onICMPv6::add(mc);		//Upp_ICMPv6::header= (upper=)
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_Redirect* McUpp_ICMPv6_Redirect::create(CSTR key){
	McUpp_ICMPv6_Redirect* mc = new McUpp_ICMPv6_Redirect(key);
	mc->common_member();
	mc->member( new MmUint( "Reserved",	32,	UN(0),	UN(0) ) );
	mc->member( new MmV6Addr( "TargetAddress",	MUST(),	MUST() ) );
	mc->member( new MmV6Addr( "DestinationAddress",	MUST(),	MUST() ) );
	mc->member( new MmOption_onICMPv6( "option" ) );
	// dict
	MmHeader_onICMPv6::add(mc);		//Upp_ICMPv6::header= (upper=)
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_MLDReport *McUpp_ICMPv6_MLDReport::create(CSTR key) {
	McUpp_ICMPv6_MLDReport *mc = new McUpp_ICMPv6_MLDReport(key);
	mc->common_member();
	mc->member( new MmUint( "MaxResponseDelay",16,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "Reserved",	16,	UN(0),	UN(0) ) );
	mc->member( new MmV6Addr( "MulticastAddress",	MUST(),	MUST() ) );
	// dict
	MmHeader_onICMPv6::add(mc);		//Upp_ICMPv6::header= (upper=)
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_MLDDone *McUpp_ICMPv6_MLDDone::create(CSTR key) {
	McUpp_ICMPv6_MLDDone *mc = new McUpp_ICMPv6_MLDDone(key);
	mc->common_member();
	mc->member( new MmUint( "MaxResponseDelay",16,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "Reserved",	16,	UN(0),	UN(0) ) );
	mc->member( new MmV6Addr( "MulticastAddress",	MUST(),	MUST() ) );
	// dict
	MmHeader_onICMPv6::add(mc);		//Upp_ICMPv6::header= (upper=)
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_EchoRequest *McUpp_ICMPv6_EchoRequest::create(CSTR key) {
	McUpp_ICMPv6_EchoRequest *mc = new McUpp_ICMPv6_EchoRequest(key);
	mc->common_member();
	mc->member( new MmUint( "Identifier",	16,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "SequenceNumber",16,	UN(0),	UN(0) ) );
	mc->member( new MmPayload( "payload" ) );
	// dict
	MmHeader_onICMPv6::add(mc);		//Upp_ICMPv6::header= (upper=)
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_EchoReply *McUpp_ICMPv6_EchoReply::create(CSTR key) {
	McUpp_ICMPv6_EchoReply *mc = new McUpp_ICMPv6_EchoReply(key);
	mc->common_member();
	mc->member( new MmUint( "Identifier",	16,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "SequenceNumber",16,	UN(0),	UN(0) ) );
	mc->member( new MmPayload( "payload" ) );
	// dict
	MmHeader_onICMPv6::add(mc);		//Upp_ICMPv6::header= (upper=)
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_PacketTooBig* McUpp_ICMPv6_PacketTooBig::create(CSTR key){
	McUpp_ICMPv6_PacketTooBig* mc = new McUpp_ICMPv6_PacketTooBig(key);
	mc->common_member();
	mc->member( new MmUint( "MTU",		32,	MUST(),	MUST() ) );
	mc->member( new MmPayload( "payload" ) );
	// dict
	MmHeader_onICMPv6::add(mc);		//Upp_ICMPv6::header= (upper=)
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_DestinationUnreachable* McUpp_ICMPv6_DestinationUnreachable::create(CSTR key){
	McUpp_ICMPv6_DestinationUnreachable* mc = new McUpp_ICMPv6_DestinationUnreachable(key);
	mc->common_member();
	mc->member( new MmUint( "Unused",	32,	UN(0),	UN(0) ) );
	mc->member( new MmPayload( "payload" ) );
	// dict
	MmHeader_onICMPv6::add(mc);		//Upp_ICMPv6::header= (upper=)
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_TimeExceeded* McUpp_ICMPv6_TimeExceeded::create(CSTR key){
	McUpp_ICMPv6_TimeExceeded* mc = new McUpp_ICMPv6_TimeExceeded(key);
	mc->common_member();
	mc->member( new MmUint( "Unused",	32,	UN(0),	UN(0) ) );
	mc->member( new MmPayload( "payload" ) );
	// dict
	MmHeader_onICMPv6::add(mc);		//Upp_ICMPv6::header= (upper=)
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_ParameterProblem* McUpp_ICMPv6_ParameterProblem::create(CSTR key){
	McUpp_ICMPv6_ParameterProblem* mc = new McUpp_ICMPv6_ParameterProblem(key);
	mc->common_member();
	mc->member( new MmUint( "Pointer",	32,	MUST(),	MUST() ) );
	mc->member( new MmPayload( "payload" ) );
	// dict
	MmHeader_onICMPv6::add(mc);		//Upp_ICMPv6::header= (upper=)
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv6_RouterRenumbering* McUpp_ICMPv6_RouterRenumbering::create(CSTR key){
	McUpp_ICMPv6_RouterRenumbering* mc = new McUpp_ICMPv6_RouterRenumbering(key);
	mc->common_member();
	mc->member( new MmUint( "SequenceNumber",	32,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "SegmentNumber",	8,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "TFlag",		1,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "RFlag",		1,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "AFlag",		1,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "SFlag",		1,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "PFlag",		1,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "Reserved1",		3,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "MaxDelay",		16,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "Reserved2",		32,	UN(0),	UN(0) ) );
	mc->member( new MmPayload( "payload" ) );
	// dict
	MmHeader_onICMPv6::add(mc);		//Upp_ICMPv6::header= (upper=)
	return mc;}



////////////////////////////////////////////////
//  Home Agent Address Discovery Request      //
////////////////////////////////////////////////
McUpp_ICMPv6_HAADRequest *
McUpp_ICMPv6_HAADRequest::create(CSTR key)
{
	McUpp_ICMPv6_HAADRequest *mc = new McUpp_ICMPv6_HAADRequest(key);

	mc->common_member();
	mc->member(new MmUint("Identifier",	16,	UN(0),	UN(0)));
	mc->member(new MmUint("RFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("Reserved",	15,	UN(0),	UN(0)));

	MmHeader_onICMPv6::add(mc);

	return(mc);
}



////////////////////////////////////////////////
//  Home Agent Address Discovery Reply        //
////////////////////////////////////////////////
McUpp_ICMPv6_HAADReply *
McUpp_ICMPv6_HAADReply::create(CSTR key)
{
	McUpp_ICMPv6_HAADReply *mc = new McUpp_ICMPv6_HAADReply(key);

	mc->common_member();
	mc->member(new MmUint("Identifier",	16,	UN(0),	UN(0)));
	mc->member(new MmUint("RFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("Reserved",	15,	UN(0),	UN(0)));
	mc->member(
		new MmMultiple(
			new MmV6Addr("Address", MUST(), MUST()),
			(METH_HC_MLC)&McUpp_ICMPv6_HAADReply::HC_MLC(Address)
		)
	);

	MmHeader_onICMPv6::add(mc);

	return(mc);
}



////////////////////////////////////////
//  Mobile Prefix Solicitation        //
////////////////////////////////////////
McUpp_ICMPv6_MobilePrefixSolicit *
McUpp_ICMPv6_MobilePrefixSolicit::create(CSTR key)
{
	McUpp_ICMPv6_MobilePrefixSolicit *mc =
		new McUpp_ICMPv6_MobilePrefixSolicit(key);

	mc->common_member();
	mc->member(new MmUint("Identifier",	16,	UN(0),	UN(0)));
	mc->member(new MmUint("Reserved",	16,	UN(0),	UN(0)));

	MmHeader_onICMPv6::add(mc);

	return(mc);
}



////////////////////////////////////////
//  Mobile Prefix Advertisement       //
////////////////////////////////////////
McUpp_ICMPv6_MobilePrefixAdvertise *
McUpp_ICMPv6_MobilePrefixAdvertise::create(CSTR key)
{
	McUpp_ICMPv6_MobilePrefixAdvertise *mc =
		new McUpp_ICMPv6_MobilePrefixAdvertise(key);

	mc->common_member();
	mc->member(new MmUint("Identifier",	16,	UN(0),	UN(0)));
	mc->member(new MmUint("MFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("OFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("Reserved",	14,	UN(0),	UN(0)));
	mc->member(new MmOption_onICMPv6("option"));

	MmHeader_onICMPv6::add(mc);

	return(mc);
}



//////////////////////////////////////////////////////////////////////////////

void McOpt_ICMPv6::common_member(){
	int32_t Type = optionType();
	type_member(
		new MmUint( "Type",	8, UN(Type),UN(Type) ) );
	length_member(
		new MmUint( "Length",	8, GENEHC(this,McOpt_ICMPv6,Length),EVALANY() ) );
	}

McOpt_ICMPv6_ANY* McOpt_ICMPv6_ANY::create(CSTR key){
	McOpt_ICMPv6_ANY* mc = new McOpt_ICMPv6_ANY(key);
	mc->common_member();
	mc->member( new MmData( "data" ) );
	// dict
	MmOption_onICMPv6::add_other(mc);	//Upp_ICMPv6_XX::option=
	return mc;}
//////////////////////////////////////////////////////////////////////////////
McOpt_ICMPv6_SLL* McOpt_ICMPv6_SLL::create(CSTR key){
	McOpt_ICMPv6_SLL* mc = new McOpt_ICMPv6_SLL(key);
	mc->common_member();
	mc->member( new MmEther( "LinkLayerAddress",	ETNUT(),ETTN()));
	// dict
	MmOption_onICMPv6::add(mc);		//Upp_ICMPv6_XX::option=
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McOpt_ICMPv6_TLL* McOpt_ICMPv6_TLL::create(CSTR key){
	McOpt_ICMPv6_TLL* mc = new McOpt_ICMPv6_TLL(key);
	mc->common_member();
	mc->member( new MmEther( "LinkLayerAddress",	ETNUT(),ETTN()));
	// dict
	MmOption_onICMPv6::add(mc);		//Upp_ICMPv6_XX::option=
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McOpt_ICMPv6_Prefix* McOpt_ICMPv6_Prefix::create(CSTR key){
	McOpt_ICMPv6_Prefix* mc = new McOpt_ICMPv6_Prefix(key);
	mc->common_member();
	mc->member( new MmUint( "PrefixLength",	8,	UN(64),	UN(64) ) );
	mc->member( new MmUint( "LFlag",	1,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "AFlag",	1,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "RFlag",	1,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "Reserved1",	5,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "ValidLifetime",32,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "PreferredLifetime",32,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "Reserved2",	32,	UN(0),	UN(0) ) );
	mc->member( new MmV6Addr( "Prefix",		MUST(),	MUST() ) );
	// dict
	MmOption_onICMPv6::add(mc);		//Upp_ICMPv6_XX::option=
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McOpt_ICMPv6_MTU* McOpt_ICMPv6_MTU::create(CSTR key){
	McOpt_ICMPv6_MTU* mc = new McOpt_ICMPv6_MTU(key);
	mc->common_member();
	mc->member( new MmUint( "Reserved",	16,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "MTU",		32,	UN(1500),UN(1500) ) );
	// dict
	MmOption_onICMPv6::add(mc);		//Upp_ICMPv6_XX::option=
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McOpt_ICMPv6_Redirected* McOpt_ICMPv6_Redirected::create(CSTR key){
	McOpt_ICMPv6_Redirected* mc = new McOpt_ICMPv6_Redirected(key);
	mc->common_member();
#ifdef YDC_YET_IMPL
	//request to define MmReserved
	mc->member( new MmReserved( "Reserved", OCTET6,
			new PvZERO(),
			new PvZERO() ) );
#else
	//Sorry devide two MmUint(16bits,32bits)
	mc->member( new MmUint( "Reserved1",	16,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "Reserved2",	32,	UN(0),	UN(0) ) );
#endif
	mc->member( new MmPayload( "payload" ) );
	// dict
	MmOption_onICMPv6::add(mc);		//Upp_ICMPv6_XX::option=
	return mc;}



////////////////////////////////////////
//  Advertisement Interval Option     //
////////////////////////////////////////
McOpt_ICMPv6_AdvInterval *
McOpt_ICMPv6_AdvInterval::create(CSTR key)
{
	McOpt_ICMPv6_AdvInterval *mc = new McOpt_ICMPv6_AdvInterval(key);

	mc->common_member();
	mc->member(new MmUint("Reserved",	16,	UN(0),	UN(0)));
	mc->member(new MmUint("Interval",	32,	UN(0),	UN(0)));

	MmOption_onICMPv6::add(mc);

	return(mc);
}



////////////////////////////////////////
//  Home Agent Information Option     //
////////////////////////////////////////
McOpt_ICMPv6_HomeAgentInfo *
McOpt_ICMPv6_HomeAgentInfo::create(CSTR key)
{
	McOpt_ICMPv6_HomeAgentInfo *mc = new McOpt_ICMPv6_HomeAgentInfo(key);

	mc->common_member();
	mc->member(new MmUint("RFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("Reserved",	15,	UN(0),	UN(0)));
	mc->member(new MmUint("Preference",	16,	UN(0),	UN(0)));
	mc->member(new MmUint("Lifetime",	16,	UN(0),	UN(0)));

	MmOption_onICMPv6::add(mc);

	return(mc);
}



//
//  Route Information Option
//
////////////////////////////////////////

McOpt_ICMPv6_RouteInfo *McOpt_ICMPv6_RouteInfo::create(CSTR key) {
	McOpt_ICMPv6_RouteInfo *mc = new McOpt_ICMPv6_RouteInfo(key);

	mc->common_member();
	mc->member(new MmUint("PrefixLength",	8,	UN(64),	UN(64)));
	mc->member(new MmUint("Reserved1",	3,	UN(0),	UN(0)));
	mc->member(new MmUint("Preference",	2,	UN(0),	UN(0)));
	mc->member(new MmUint("Reserved2",	3,	UN(0),	UN(0)));
	mc->member(new MmUint("RouteLifetime",	32,	UN(0),	UN(0)));
	mc->member(new MmV6Addr("Prefix",	MUST(),	MUST()));

	MmOption_onICMPv6::add(mc);

	return(mc);
}



////////////////////////////////////////////////////////////////
//  Mobility Header                                           //
////////////////////////////////////////////////////////////////
void
McHdr_Ext_MH::common_member()
{
	nextType_member(new MmUint("NextHeader", 8,
			GENEHC(this, McHdr_Ext_MH, NextHeader),
			EVALHC(this, McHdr_Ext_MH, NextHeader),
			ICVCONST()));
	length_member(new MmUint("HeaderExtLength", 8,
			GENEHC(this, McHdr_Ext_MH, HeaderExtLength),
			EVALANY(),
			ICVCONST()));
	type_member(new MmUint("Type", 8,
			GENEHC(this, McHdr_Ext_MH, Type),
			EVALHC(this, McHdr_Ext_MH, Type)));
	member(new MmUint("Reserved1", 8, UN(0), UN(0)));
	member(new MmUppChecksum("Checksum", 16));

	return;
}

McHdr_Ext_MH_ANY *
McHdr_Ext_MH_ANY::create(CSTR key)
{
	McHdr_Ext_MH_ANY *mc = new McHdr_Ext_MH_ANY(key);

	mc->common_member();
	mc->member(new MmData("data"));

	MmHeader_onMH::add_other(mc);

	return(mc);
}



////////////////////////////////////////
//  Binding Refresh Request (BRR)     //
////////////////////////////////////////
McHdr_Ext_MH_BRR *
McHdr_Ext_MH_BRR::create(CSTR key)
{
	McHdr_Ext_MH_BRR *mc = new McHdr_Ext_MH_BRR(key);

	mc->common_member();
	mc->member(new MmUint("Reserved2", 16, UN(0), UN(0)));
	mc->member(new MmOption_onMH("option"));

	MmHeader_onMH::add(mc);

	return(mc);
}



////////////////////////////////
//  Home Test Init (HoTI)     //
////////////////////////////////
McHdr_Ext_MH_HoTI *
McHdr_Ext_MH_HoTI::create(CSTR key)
{
	McHdr_Ext_MH_HoTI *mc = new McHdr_Ext_MH_HoTI(key);

	mc->common_member();
	mc->member(new MmUint("Reserved2", 16, UN(0), UN(0)));
	mc->member(new MmMH_Cookie64("InitCookie"));
	mc->member(new MmOption_onMH("option"));

	MmHeader_onMH::add(mc);

	return(mc);
}



////////////////////////////////
//  Care-of Test Init (CoTI)  //
////////////////////////////////
McHdr_Ext_MH_CoTI *
McHdr_Ext_MH_CoTI::create(CSTR key)
{
	McHdr_Ext_MH_CoTI *mc = new McHdr_Ext_MH_CoTI(key);

	mc->common_member();
	mc->member(new MmUint("Reserved2", 16, UN(0), UN(0)));
	mc->member(new MmMH_Cookie64("InitCookie"));
	mc->member(new MmOption_onMH("option"));

	MmHeader_onMH::add(mc);

	return(mc);
}



////////////////////////////////
//  Home Test (HoT)           //
////////////////////////////////
McHdr_Ext_MH_HoT *
McHdr_Ext_MH_HoT::create(CSTR key)
{
	McHdr_Ext_MH_HoT *mc = new McHdr_Ext_MH_HoT(key);

	mc->common_member();
	mc->member(new MmUint("Index", 16, UN(0), UN(0)));
	mc->member(new MmMH_Cookie64("InitCookie"));
	mc->member(new MmMH_Cookie64("KeygenToken"));
	mc->member(new MmOption_onMH("option"));

	MmHeader_onMH::add(mc);

	return(mc);
}



////////////////////////////////
//  Care-of Test (CoT)        //
////////////////////////////////
McHdr_Ext_MH_CoT *
McHdr_Ext_MH_CoT::create(CSTR key)
{
	McHdr_Ext_MH_CoT *mc = new McHdr_Ext_MH_CoT(key);

	mc->common_member();
	mc->member(new MmUint("Index", 16, UN(0), UN(0)));
	mc->member(new MmMH_Cookie64("InitCookie"));
	mc->member(new MmMH_Cookie64("KeygenToken"));
	mc->member(new MmOption_onMH("option"));

	MmHeader_onMH::add(mc);

	return(mc);
}



////////////////////////////////
//  Binding Update (BU)       //
////////////////////////////////
McHdr_Ext_MH_BU *
McHdr_Ext_MH_BU::create(CSTR key)
{
	McHdr_Ext_MH_BU *mc = new McHdr_Ext_MH_BU(key);

	mc->common_member();
	mc->member(new MmUint("SequenceNumber",	16,	UN(0),	UN(0)));
	mc->member(new MmUint("AFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("HFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("LFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("KFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("MFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("RFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("Reserved2",	10,	UN(0),	UN(0)));
	mc->member(new MmUint("Lifetime",	16,	UN(0),	UN(0)));
	mc->member(new MmOption_onMH("option"));

	MmHeader_onMH::add(mc);

	return(mc);
}



////////////////////////////////////////
//  Binding Acknowledgement (BA)      //
////////////////////////////////////////
McHdr_Ext_MH_BA *
McHdr_Ext_MH_BA::create(CSTR key)
{
	McHdr_Ext_MH_BA *mc = new McHdr_Ext_MH_BA(key);

	mc->common_member();
	mc->member(new MmBAstatus("Status",	8,	UN(0),	UN(0)));
	mc->member(new MmUint("KFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("RFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("Reserved2",	6,	UN(0),	UN(0)));
	mc->member(new MmUint("SequenceNumber",	16,	UN(0),	UN(0)));
	mc->member(new MmUint("Lifetime",	16,	UN(0),	UN(0)));
	mc->member(new MmOption_onMH("option"));

	MmHeader_onMH::add(mc);

	return(mc);
}



////////////////////////////////
//  Binding Error (BE)        //
////////////////////////////////
McHdr_Ext_MH_BE *
McHdr_Ext_MH_BE::create(CSTR key)
{
	McHdr_Ext_MH_BE *mc = new McHdr_Ext_MH_BE(key);

	mc->common_member();
	mc->member(new MmBEstatus("Status",	8,	UN(0),	UN(0)));
	mc->member(new MmUint("Reserved2",	8,	UN(0),	UN(0)));
	mc->member(new MmV6Addr("Address",	MUST(),	MUST()));
	mc->member(new MmOption_onMH("option"));

	MmHeader_onMH::add(mc);

	return(mc);
}



////////////////////////////////////////////////////////////////
//  Mobility Options                                          //
////////////////////////////////////////////////////////////////
void
McOpt_MH::common_member()
{
	type_member(new MmUint("Type",
		8, GENEHC(this, McOpt_MH, Type), EVALHC(this, McOpt_MH, Type)));
	length_member(new MmUint("Length",
		8, GENEHC(this, McOpt_MH, Length), EVALANY()));

	return;
}

McOpt_MH_ANY *
McOpt_MH_ANY::create(CSTR key)
{
	McOpt_MH_ANY *mc = new McOpt_MH_ANY(key);

	mc->common_member();
	mc->member(new MmData("data"));

	MmOption_onMH::add_other(mc);

	return(mc);
}



////////////////////////////////
//  Pad1                      //
////////////////////////////////
McOpt_MH_Pad1 *
McOpt_MH_Pad1::create(CSTR key)
{
	McOpt_MH_Pad1 *mc = new McOpt_MH_Pad1(key);

	int32_t Type = mc->optionType();

	mc->type_member(new MmUint("Type", 8, UN(Type), UN(Type)));

	MmOption_onMH::add(mc);

	return(mc);
}



////////////////////////////////
//  PadN                      //
////////////////////////////////
McOpt_MH_PadN *
McOpt_MH_PadN::create(CSTR key)
{
	McOpt_MH_PadN *mc = new McOpt_MH_PadN(key);

	mc->common_member();
	mc->member(new MmData("pad"));

	MmOption_onMH::add(mc);

	return(mc);
}



////////////////////////////////
//  Binding Refresh Advice    //
////////////////////////////////
McOpt_MH_BindingRefreshAdvice *
McOpt_MH_BindingRefreshAdvice::create(CSTR key)
{
	McOpt_MH_BindingRefreshAdvice *mc =
		new McOpt_MH_BindingRefreshAdvice(key);

	mc->common_member();
	mc->member(new MmUint("Interval", 16, UN(0), UN(0)));

	MmOption_onMH::add(mc);

	return(mc);
}



////////////////////////////////
//  Alternate Care-of Address //
////////////////////////////////
McOpt_MH_AlternateCoA *
McOpt_MH_AlternateCoA::create(CSTR key)
{
	McOpt_MH_AlternateCoA *mc = new McOpt_MH_AlternateCoA(key);

	mc->common_member();
	mc->member(new MmV6Addr("Address", MUST(), MUST()));

	MmOption_onMH::add(mc);

	return(mc);
}



////////////////////////////////
//  Nonce Indices             //
////////////////////////////////
McOpt_MH_NonceIndices *
McOpt_MH_NonceIndices::create(CSTR key)
{
	McOpt_MH_NonceIndices *mc = new McOpt_MH_NonceIndices(key);

	mc->common_member();
	mc->member(new MmUint("HoNonceIndex", 16, UN(0), UN(0)));
	mc->member(new MmUint("CoNonceIndex", 16, UN(0), UN(0)));

	MmOption_onMH::add(mc);

	return(mc);
}



////////////////////////////////////////
//  Binding Authorization Data        //
////////////////////////////////////////
McOpt_MH_BindingAuthData *
McOpt_MH_BindingAuthData::create(CSTR key)
{
	McOpt_MH_BindingAuthData *mc = new McOpt_MH_BindingAuthData(key);

	mc->common_member();
	mc->member(new MmMH_Auth("Authenticator"));

	MmOption_onMH::add(mc);

	return(mc);
}



////////////////////////////////
//  Mobile Network Prefix     //
////////////////////////////////
McOpt_MH_MobNetworkPrefix *
McOpt_MH_MobNetworkPrefix::create(CSTR key) {
	McOpt_MH_MobNetworkPrefix *mc = new McOpt_MH_MobNetworkPrefix(key);

	mc->common_member();
	mc->member(new MmUint("Reserved", 8, UN(0), UN(0)));
	mc->member(new MmUint("PrefixLength", 8, UN(0), UN(0)));
	mc->member(new MmV6Addr("Prefix", MUST(), MUST()));

	MmOption_onMH::add(mc);

	return(mc);
}



//////////////////////////////////////////////////////////////////////////////
McUpp_UDP *McUpp_UDP::create(CSTR key, CSTR tophdrkey) {
	addCompound(tophdr_ = McTopHdr_UDP::create(tophdrkey));
	McUpp_UDP *mc = new McUpp_UDP(key);

	mc->member(new MmTopHdr("header", tophdr_));
	mc->member(new MmUpper_onUpper("payload", tophdr_));

	// dict
	MmUpper_onIP::add(mc);	// Packet_IP::upper=

	return(mc);
}

McTopHdr_UDP *McTopHdr_UDP::create(CSTR key) {
	McTopHdr_UDP *mc = new McTopHdr_UDP(key);

	mc->SrcPort_member(
		new MmUint(
			"SourcePort",
			16,
			GENEHC(mc, McTopHdr_UDP, SourcePort),
			EVALHC(mc, McTopHdr_UDP, SourcePort)
		)
	);
	mc->DstPort_member(
		new MmUint(
			"DestinationPort",
			16,
			GENEHC(mc, McTopHdr_UDP, DestinationPort),
			EVALHC(mc, McTopHdr_UDP, DestinationPort)
		)
	);
	mc->Layerlength_member(new MmUint("Length", 16, GENEHC(mc, McTopHdr_UDP, Length), EVALANY()));
	mc->member(new MmUppChecksum("Checksum", 16, DEF_OPTCHKSUM));

	// no dict
	return(mc);
}

//////////////////////////////////////////////////////////////////////////////
McUpp_TCP* McUpp_TCP::create(CSTR key,CSTR tophdrkey){
	addCompound(tophdr_ = McTopHdr_TCP::create(tophdrkey));
	McUpp_TCP* mc = new McUpp_TCP(key);
	mc->member( new MmTopHdr(	"header",tophdr_ ) );
	mc->member( new MmPayload(	"payload" ) );
	// dict
	MmUpper_onIP::add(mc);			//Packet_IP::upper=
	return mc;}

McTopHdr_TCP* McTopHdr_TCP::create(CSTR key){
	McTopHdr_TCP* mc = new McTopHdr_TCP(key);
	mc->member( new MmUint( "SourcePort",	16,	MUST(),	MUST() ) );
	mc->member( new MmUint( "DestinationPort",16,	MUST(),	MUST() ) );
	mc->member( new MmUint( "SequenceNumber",32,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "AcknowledgmentNumber",32,UN(0),UN(0) ) );
	mc->length_member(
		    new MmUint( "DataOffset",	4,
			GENEHC(mc,McTopHdr_TCP,DataOffset), EVALANY() ) );
	mc->member( new MmUint( "Reserverd",	6,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "URGFlag",	1,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "ACKFlag",	1,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "PSHFlag",	1,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "RSTFlag",	1,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "SYNFlag",	1,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "FINFlag",	1,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "Window",	16,	UN(0),	UN(0) ) );
	mc->member( new MmUppChecksum( "Checksum",16) );
	mc->member( new MmUint( "UrgentPointer",16,	UN(0),	UN(0) ) );
	mc->member( new MmOption_onTCP( "option" ) );
	mc->member( new MmData( "Padding",DEF_EVALSKIP ) );
	// no dict
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McPacket_ARP* McPacket_ARP::create(CSTR key,CSTR tophdrkey){
	addCompound(tophdr_ = McTopHdr_ARP::create(tophdrkey));
	McPacket_ARP* mc = new McPacket_ARP(key);
	mc->member( new MmTopHdr(	"header",tophdr_) );
	// dict
	MmPacket_onEther::add(mc);		//Frame_Ether::packet=
	return mc;}

McTopHdr_ARP* McTopHdr_ARP::create(CSTR key){
	McTopHdr_ARP* mc = new McTopHdr_ARP(key);
	mc->member( new MmUint( "Hardware",	16,	UN(1),	UN(1) ) );
	mc->member( new MmUint( "Protocol",	16,	UN(2048),UN(2048) ) );
	mc->member( new MmUint( "HLEN",		8,	UN(6),	UN(6) ) );
	mc->member( new MmUint( "PLEN",		8,	UN(4),	UN(4) ) );
	mc->member( new MmUint( "Operation",	16,	UN(2),	UN(1) ) );
	mc->member( new MmEther("SenderHAddr",		ETTN(),	ETNUT()));
	mc->member( new MmV4Addr("SenderPAddr",		UN(0),	UN(0) ) );
	mc->member( new MmEther("TargetHAddr",		ETNUT(),ETTN()));
	mc->member( new MmV4Addr("TargetPAddr",		UN(0),	UN(0) ) );
	// no dict
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McPacket_RARP* McPacket_RARP::create(CSTR key,CSTR tophdrkey){
	addCompound(tophdr_ = McTopHdr_RARP::create(tophdrkey));
	McPacket_RARP* mc = new McPacket_RARP(key);
	mc->member( new MmTopHdr(	"header",tophdr_) );
	// dict
	MmPacket_onEther::add(mc);		//Frame_Ether::packet=
	return mc;}

McTopHdr_RARP* McTopHdr_RARP::create(CSTR key){
	McTopHdr_RARP* mc = new McTopHdr_RARP(key);
	mc->member( new MmUint( "Hardware",	16,	UN(1),	UN(1) ) );
	mc->member( new MmUint( "Protocol",	16,	UN(2048),UN(2048) ) );
	mc->member( new MmUint( "HLEN",		8,	UN(6),	UN(6) ) );
	mc->member( new MmUint( "PLEN",		8,	UN(4),	UN(4) ) );
	mc->member( new MmUint( "Operation",	16,	UN(3),	UN(4) ) );
	mc->member( new MmEther("SenderHAddr",		ETTN(),	ETNUT()));
	mc->member( new MmV4Addr("SenderPAddr",		UN(0),	UN(0) ) );
	mc->member( new MmEther("TargetHAddr",		ETNUT(),ETTN()));
	mc->member( new MmV4Addr("TargetPAddr",		UN(0),	UN(0) ) );
	// no dict
	return mc;}

//////////////////////////////////////////////////////////////////////////////
void McUpp_ICMPv4::common_member(){
	int32_t Type = icmpv4Type();
	type_member(
		new MmUint( "Type",		8,	UN(Type),UN(Type) ) );
	member( new MmUint( "Code",		8,	UN(0),	UN(0) ) );
	member( new MmUppChecksum( "Checksum",	16,
			DEF_MUSTCHKSUM, DEF_NOPSEUDO ) );
	}

McUpp_ICMPv4_ANY* McUpp_ICMPv4_ANY::create(CSTR key){
	McUpp_ICMPv4_ANY* mc = new McUpp_ICMPv4_ANY(key);
	mc->common_member();
	mc->member( new MmData( "data" ) );
	//dict
	MmHeader_onICMPv4::add_other(mc);	//Upp_ICMPv4::header= (upper=)
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv4_DestinationUnreachable* McUpp_ICMPv4_DestinationUnreachable::create(CSTR key){
	McUpp_ICMPv4_DestinationUnreachable* mc =
		new McUpp_ICMPv4_DestinationUnreachable(key);
	mc->common_member();
	mc->member( new MmUint( "Unused",	32,	UN(0),	UN(0) ) );
	mc->member( new MmPayload( "payload" ) );
	//dict
	MmHeader_onICMPv4::add(mc);		//Upp_ICMPv4::header= (upper=)
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv4_TimeExceeded* McUpp_ICMPv4_TimeExceeded::create(CSTR key){
	McUpp_ICMPv4_TimeExceeded* mc = new McUpp_ICMPv4_TimeExceeded(key);
	mc->common_member();
	mc->member( new MmUint( "Unused",	32,	UN(0),	UN(0) ) );
	mc->member( new MmPayload( "payload" ) );
	//dict
	MmHeader_onICMPv4::add(mc);		//Upp_ICMPv4::header= (upper=)
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv4_ParameterProblem* McUpp_ICMPv4_ParameterProblem::create(CSTR key){
	McUpp_ICMPv4_ParameterProblem* mc =
		new McUpp_ICMPv4_ParameterProblem(key);
	mc->common_member();
	mc->member( new MmUint( "Pointer",	8,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "Unused",	24,	UN(0),	UN(0) ) );
	mc->member( new MmPayload( "payload" ) );
	//dict
	MmHeader_onICMPv4::add(mc);		//Upp_ICMPv4::header= (upper=)
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv4_Redirect* McUpp_ICMPv4_Redirect::create(CSTR key){
	McUpp_ICMPv4_Redirect* mc = new McUpp_ICMPv4_Redirect(key);
	mc->common_member();
	mc->member( new MmV4Addr("GatewayInternetAddress", MUST(), MUST() ) );
	mc->member( new MmPayload( "payload" ) );
	//dict
	MmHeader_onICMPv4::add(mc);		//Upp_ICMPv4::header= (upper=)
	return mc;}

//////////////////////////////////////////////////////////////////////////////
McUpp_ICMPv4_EchoRequest *McUpp_ICMPv4_EchoRequest::create(CSTR key) {
	McUpp_ICMPv4_EchoRequest *mc = new McUpp_ICMPv4_EchoRequest(key);
	mc->common_member();
	mc->member( new MmUint( "Identifier",	16,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "SequenceNumber",16,	UN(0),	UN(0) ) );
	mc->member( new MmPayload( "payload" ) );
	//dict
	MmHeader_onICMPv4::add(mc);		//Upp_ICMPv4::header= (upper=)
	return mc;}

McUpp_ICMPv4_EchoReply *McUpp_ICMPv4_EchoReply::create(CSTR key) {
	McUpp_ICMPv4_EchoReply *mc = new McUpp_ICMPv4_EchoReply(key);
	mc->common_member();
	mc->member( new MmUint( "Identifier",	16,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "SequenceNumber",16,	UN(0),	UN(0) ) );
	mc->member( new MmPayload( "payload" ) );
	//dict
	MmHeader_onICMPv4::add(mc);		//Upp_ICMPv4::header= (upper=)
	return mc;}

//
// RFC 2080
//
//     RIPng for IPv6
//
////////////////////////////////////////////////////////////////////////////////

McUdp_RIPng *McUdp_RIPng::create(CSTR key) {
	McUdp_RIPng *mc = new McUdp_RIPng(key);

	mc->member(new MmUint("Command", 8, UN(0), UN(0)));
	mc->member(new MmUint("Version", 8, UN(1), UN(1)));
	mc->member(new MmUint("Reserved", 16, UN(0), UN(0)));

	mc->member(new MmRTE_onRIPng("rte"));

	MmUpper_onUpper::add(mc);

	return(mc);
}

McRIPng_RTE *McRIPng_RTE::create(CSTR key) {
	McRIPng_RTE *mc = new McRIPng_RTE(key);

	mc->member(new MmV6Addr("Prefix",		MUST(),	MUST()));
	mc->member(new MmUint("RouteTag",	16,	UN(0),	UN(0)));
	mc->member(new MmUint("PrefixLength",	8,	UN(64),	UN(64)));
	mc->member(new MmUint("Metric",		8,	UN(0),	UN(0)));

	MmRTE_onRIPng::add_other(mc);

	return(mc);
}

//
// RFC 1035
//
//     Domain Implementation and Specification
//
////////////////////////////////////////////////////////////////////////////////

McUdp_DNS *McUdp_DNS::create(CSTR key0, CSTR key1, CSTR key2, CSTR key3, CSTR key4) {
	McUdp_DNS *mc = new McUdp_DNS(key0);

	addCompound(McDNS_Question::create(key1, mc));
	addCompound(McDNS_Answer::create(key2, mc));
	addCompound(McDNS_Authority::create(key3, mc));
	addCompound(McDNS_Additional::create(key4, mc));

	mc->member(new MmUint("Identifier", 16, UN(0), UN(0)));

	mc->member(new MmUint("QR", 1, UN(0), UN(0)));
	mc->member(new MmUint("Opcode", 4, UN(0), UN(0)));
	mc->member(new MmUint("AA", 1, UN(0), UN(0)));
	mc->member(new MmUint("TC", 1, UN(0), UN(0)));
	mc->member(new MmUint("RD", 1, UN(0), UN(0)));
	mc->member(new MmUint("RA", 1, UN(0), UN(0)));
	mc->member(new MmUint("Reserved", 3, UN(0), UN(0)));
	mc->member(new MmUint("RCode", 4, UN(0), UN(0)));
	mc->QDCount_member(new MmUint("QDCount", 16, GENEHC(mc, McUdp_DNS, QDCount), EVALANY()));
	mc->ANCount_member(new MmUint("ANCount", 16, GENEHC(mc, McUdp_DNS, ANCount), EVALANY()));
	mc->NSCount_member(new MmUint("NSCount", 16, GENEHC(mc, McUdp_DNS, NSCount), EVALANY()));
	mc->ARCount_member(new MmUint("ARCount", 16, GENEHC(mc, McUdp_DNS, ARCount), EVALANY()));

	mc->member(new MmQuestion_onDNS("question", mc));
	mc->member(new MmAnswer_onDNS("answer", mc));
	mc->member(new MmAuthority_onDNS("authority", mc));
	mc->member(new MmAdditional_onDNS("additional", mc));

	MmUpper_onUpper::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_Question *McDNS_Question::create(CSTR key, McUdp_DNS *dnshdr) {
	McDNS_Question *mc = new McDNS_Question(key);

	mc->McUdp_DNS_ = dnshdr;

	mc->member(new MmEntry_onQuestion("entry", dnshdr));

	MmQuestion_onDNS::add_other(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_Answer *McDNS_Answer::create(CSTR key, McUdp_DNS *dnshdr) {
	McDNS_Answer *mc = new McDNS_Answer(key);

	mc->McUdp_DNS_ = dnshdr;

	mc->member(new MmRR_onAnswer("rr", dnshdr));

	MmAnswer_onDNS::add_other(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_Authority *McDNS_Authority::create(CSTR key, McUdp_DNS *dnshdr) {
	McDNS_Authority *mc = new McDNS_Authority(key);

	mc->McUdp_DNS_ = dnshdr;

	mc->member(new MmRR_onAuthority("rr", dnshdr));

	MmAuthority_onDNS::add_other(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_Additional *McDNS_Additional::create(CSTR key, McUdp_DNS *dnshdr) {
	McDNS_Additional *mc = new McDNS_Additional(key);

	mc->McUdp_DNS_ = dnshdr;

	mc->member(new MmRR_onAdditional("rr", dnshdr));

	MmAdditional_onDNS::add_other(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_QuestionEntry *McDNS_QuestionEntry::create(CSTR key) {
	McDNS_QuestionEntry *mc = new McDNS_QuestionEntry(key);

	mc->member(new MmDNSName("Name", MUST(), MUST()));
	mc->member(new MmQType("Type", 16, UN(0), UN(0)));
	mc->member(new MmUint("Class", 16, UN(1), UN(1)));

	MmEntry_onQuestion::add_other(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
void McDNS_RR::common_member() {
	member(new MmDNSName("Name", MUST(), MUST()));
	type_member(new MmUint("Type", 16, GENEHC(this, McDNS_RR, Type), EVALHC(this, McDNS_RR, Type)));
	member(new MmUint("Class", 16, UN(1), UN(1)));
	member(new MmUint("TTL", 32, UN(0), UN(0)));
	length_member(new MmUint("Length", 16, GENEHC(this, McDNS_RR, Length), EVALANY()));
}

////////////////////////////////////////////////////////////////
McDNS_RR_ANY *McDNS_RR_ANY::create(CSTR key) {
	McDNS_RR_ANY *mc = new McDNS_RR_ANY(key);

	mc->common_member();
	mc->member(new MmData("data"));

	MmRR_onAnswer::add_other(mc);
	MmRR_onAuthority::add_other(mc);
	MmRR_onAdditional::add_other(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_A *McDNS_RR_A::create(CSTR key) {
	McDNS_RR_A *mc = new McDNS_RR_A(key);

	mc->common_member();
	mc->member(new MmV4Addr("Address", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_NS *McDNS_RR_NS::create(CSTR key) {
	McDNS_RR_NS *mc = new McDNS_RR_NS(key);

	mc->common_member();
	mc->member(new MmDNSName("NSDName", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_MD *McDNS_RR_MD::create(CSTR key) {
	McDNS_RR_MD *mc = new McDNS_RR_MD(key);

	mc->common_member();
	mc->member(new MmDNSName("MADName", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_MF *McDNS_RR_MF::create(CSTR key) {
	McDNS_RR_MF *mc = new McDNS_RR_MF(key);

	mc->common_member();
	mc->member(new MmDNSName("MADName", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_CNAME *McDNS_RR_CNAME::create(CSTR key) {
	McDNS_RR_CNAME *mc = new McDNS_RR_CNAME(key);

	mc->common_member();
	mc->member(new MmDNSName("CName", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_SOA *McDNS_RR_SOA::create(CSTR key) {
	McDNS_RR_SOA *mc = new McDNS_RR_SOA(key);

	mc->common_member();
	mc->member(new MmDNSName("MName", MUST(), MUST()));
	mc->member(new MmDNSName("RName", MUST(), MUST()));
	mc->member(new MmUint("Serial", 32, UN(0), UN(0)));
	mc->member(new MmUint("Refresh", 32, UN(0), UN(0)));
	mc->member(new MmUint("Retry", 32, UN(0), UN(0)));
	mc->member(new MmUint("Expire", 32, UN(0), UN(0)));
	mc->member(new MmUint("Minimum", 32, UN(0), UN(0)));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_MB *McDNS_RR_MB::create(CSTR key) {
	McDNS_RR_MB *mc = new McDNS_RR_MB(key);

	mc->common_member();
	mc->member(new MmDNSName("MADName", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_MG *McDNS_RR_MG::create(CSTR key) {
	McDNS_RR_MG *mc = new McDNS_RR_MG(key);

	mc->common_member();
	mc->member(new MmDNSName("MGMName", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_MR *McDNS_RR_MR::create(CSTR key) {
	McDNS_RR_MR *mc = new McDNS_RR_MR(key);

	mc->common_member();
	mc->member(new MmDNSName("NewName", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_NULL *McDNS_RR_NULL::create(CSTR key) {
	McDNS_RR_NULL *mc = new McDNS_RR_NULL(key);

	mc->common_member();
	mc->member(new MmVarOctets("data"));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_WKS *McDNS_RR_WKS::create(CSTR key) {
	McDNS_RR_WKS *mc = new McDNS_RR_WKS(key);

	mc->common_member();
	mc->member(new MmV4Addr("Address", MUST(), MUST()));
	mc->member(new MmUint("Protocol", 8, UN(0), UN(0)));
	mc->member(new MmVarOctets("BitMap"));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_PTR *McDNS_RR_PTR::create(CSTR key) {
	McDNS_RR_PTR *mc = new McDNS_RR_PTR(key);

	mc->common_member();
	mc->member(new MmDNSName("PTRDName", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_HINFO *McDNS_RR_HINFO::create(CSTR key) {
	McDNS_RR_HINFO *mc = new McDNS_RR_HINFO(key);

	mc->common_member();
	mc->member(new MmDNSStr("CPU", MUST(), MUST()));
	mc->member(new MmDNSStr("OS", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_MINFO *McDNS_RR_MINFO::create(CSTR key) {
	McDNS_RR_MINFO *mc = new McDNS_RR_MINFO(key);

	mc->common_member();
	mc->member(new MmDNSName("RMailbox", MUST(), MUST()));
	mc->member(new MmDNSName("EMailbox", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_MX *McDNS_RR_MX::create(CSTR key) {
	McDNS_RR_MX *mc = new McDNS_RR_MX(key);

	mc->common_member();
	mc->member(new MmUint("Preference", 16, UN(0), UN(0)));
	mc->member(new MmDNSName("Exchange", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_TXT *McDNS_RR_TXT::create(CSTR key) {
	McDNS_RR_TXT *mc = new McDNS_RR_TXT(key);

	mc->common_member();
	mc->member(
		new MmMultiple(
			new MmDNSStr(
				"TXT",
				MUST(),
				MUST()
			),
			(METH_HC_MLC)&McDNS_RR_TXT::HC_MLC(TXT)
		)
	);

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_RP *McDNS_RR_RP::create(CSTR key) {
	McDNS_RR_RP *mc = new McDNS_RR_RP(key);

	mc->common_member();
	mc->member(new MmDNSName("MailBox", MUST(), MUST()));
	mc->member(new MmDNSName("TXTDName", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_AFSDB *McDNS_RR_AFSDB::create(CSTR key) {
	McDNS_RR_AFSDB *mc = new McDNS_RR_AFSDB(key);

	mc->common_member();
	mc->member(new MmUint("Subtype", 16, UN(0), UN(0)));
	mc->member(new MmDNSName("Hostname", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_X25 *McDNS_RR_X25::create(CSTR key) {
	McDNS_RR_X25 *mc = new McDNS_RR_X25(key);

	mc->common_member();
	mc->member(new MmDNSStr("Address", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_ISDN *McDNS_RR_ISDN::create(CSTR key) {
	McDNS_RR_ISDN *mc = new McDNS_RR_ISDN(key);

	mc->common_member();
	mc->member(new MmDNSStr("Address", MUST(), MUST()));
	mc->member(new MmDNSStr("SubAddress", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_RT *McDNS_RR_RT::create(CSTR key) {
	McDNS_RR_RT *mc = new McDNS_RR_RT(key);

	mc->common_member();
	mc->member(new MmUint("Preference", 16, UN(0), UN(0)));
	mc->member(new MmDNSName("Intermediate", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_PX *McDNS_RR_PX::create(CSTR key) {
	McDNS_RR_PX *mc = new McDNS_RR_PX(key);

	mc->common_member();
	mc->member(new MmUint("Preference", 16, UN(0), UN(0)));
	mc->member(new MmDNSName("Map822", MUST(), MUST()));
	mc->member(new MmDNSName("MapX400", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_AAAA *McDNS_RR_AAAA::create(CSTR key) {
	McDNS_RR_AAAA *mc = new McDNS_RR_AAAA(key);

	mc->common_member();
	mc->member(new MmV6Addr("Address", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_SRV *McDNS_RR_SRV::create(CSTR key) {
	McDNS_RR_SRV *mc = new McDNS_RR_SRV(key);

	mc->common_member();
	mc->member(new MmUint("Priority", 16, UN(0), UN(0)));
	mc->member(new MmUint("Weight", 16, UN(0), UN(0)));
	mc->member(new MmUint("Port", 16, UN(0), UN(0)));
	mc->member(new MmDNSName("Target", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

////////////////////////////////////////////////////////////////
McDNS_RR_NAPTR *McDNS_RR_NAPTR::create(CSTR key) {
	McDNS_RR_NAPTR *mc = new McDNS_RR_NAPTR(key);

	mc->common_member();
	mc->member(new MmUint("Order", 16, UN(0), UN(0)));
	mc->member(new MmUint("Preference", 16, UN(0), UN(0)));
	mc->member(new MmDNSStr("Flags", MUST(), MUST()));
	mc->member(new MmDNSStr("Service", MUST(), MUST()));
	mc->member(new MmDNSStr("Regexp", MUST(), MUST()));
	mc->member(new MmDNSName("Replacement", MUST(), MUST()));

	MmRR_onAnswer::add(mc);
	MmRR_onAuthority::add(mc);
	MmRR_onAdditional::add(mc);

	return(mc);
}

//
// draft-ietf-dhc-dhcpv6-28.txt
//
//     DHCP for IPv6 (-28)
//
////////////////////////////////////////////////////////////////////////////////

void McUdp_DHCPv6::common_member() {
        type_member(new MmUint("Type", 8, GENEHC(this, McUdp_DHCPv6, Type), EVALHC(this, McUdp_DHCPv6, Type)));
	return;
}

McUdp_DHCPv6_ANY *McUdp_DHCPv6_ANY::create(CSTR key) {
	McUdp_DHCPv6_ANY *mc = new McUdp_DHCPv6_ANY(key);

	mc->common_member();
	mc->member(new MmData("data"));

	MmHeader_onDHCPv6::add_other(mc);

	return(mc);
}

McUdp_DHCPv6_Solicit *McUdp_DHCPv6_Solicit::create(CSTR key) {
	McUdp_DHCPv6_Solicit *mc = new McUdp_DHCPv6_Solicit(key);

	mc->common_member();
	mc->member(new MmUint("Identifier", 24, UN(0), UN(0)));

	mc->member(new MmOption_onDHCPv6("option"));

	MmHeader_onDHCPv6::add(mc);

	return(mc);
}

McUdp_DHCPv6_Advertise *McUdp_DHCPv6_Advertise::create(CSTR key) {
	McUdp_DHCPv6_Advertise *mc = new McUdp_DHCPv6_Advertise(key);

	mc->common_member();
	mc->member(new MmUint("Identifier", 24, UN(0), UN(0)));

	mc->member(new MmOption_onDHCPv6("option"));

	MmHeader_onDHCPv6::add(mc);

	return(mc);
}

McUdp_DHCPv6_Request *McUdp_DHCPv6_Request::create(CSTR key) {
	McUdp_DHCPv6_Request *mc = new McUdp_DHCPv6_Request(key);

	mc->common_member();
	mc->member(new MmUint("Identifier", 24, UN(0), UN(0)));

	mc->member(new MmOption_onDHCPv6("option"));

	MmHeader_onDHCPv6::add(mc);

	return(mc);
}

McUdp_DHCPv6_Confirm *McUdp_DHCPv6_Confirm::create(CSTR key) {
	McUdp_DHCPv6_Confirm *mc = new McUdp_DHCPv6_Confirm(key);

	mc->common_member();
	mc->member(new MmUint("Identifier", 24, UN(0), UN(0)));

	mc->member(new MmOption_onDHCPv6("option"));

	MmHeader_onDHCPv6::add(mc);

	return(mc);
}

McUdp_DHCPv6_Renew *McUdp_DHCPv6_Renew::create(CSTR key) {
	McUdp_DHCPv6_Renew *mc = new McUdp_DHCPv6_Renew(key);

	mc->common_member();
	mc->member(new MmUint("Identifier", 24, UN(0), UN(0)));

	mc->member(new MmOption_onDHCPv6("option"));

	MmHeader_onDHCPv6::add(mc);

	return(mc);
}

McUdp_DHCPv6_Rebind *McUdp_DHCPv6_Rebind::create(CSTR key) {
	McUdp_DHCPv6_Rebind *mc = new McUdp_DHCPv6_Rebind(key);

	mc->common_member();
	mc->member(new MmUint("Identifier", 24, UN(0), UN(0)));

	mc->member(new MmOption_onDHCPv6("option"));

	MmHeader_onDHCPv6::add(mc);

	return(mc);
}

McUdp_DHCPv6_Reply *McUdp_DHCPv6_Reply::create(CSTR key) {
	McUdp_DHCPv6_Reply *mc = new McUdp_DHCPv6_Reply(key);

	mc->common_member();
	mc->member(new MmUint("Identifier", 24, UN(0), UN(0)));

	mc->member(new MmOption_onDHCPv6("option"));

	MmHeader_onDHCPv6::add(mc);

	return(mc);
}

McUdp_DHCPv6_Release *McUdp_DHCPv6_Release::create(CSTR key) {
	McUdp_DHCPv6_Release *mc = new McUdp_DHCPv6_Release(key);

	mc->common_member();
	mc->member(new MmUint("Identifier", 24, UN(0), UN(0)));

	mc->member(new MmOption_onDHCPv6("option"));

	MmHeader_onDHCPv6::add(mc);

	return(mc);
}

McUdp_DHCPv6_Decline *McUdp_DHCPv6_Decline::create(CSTR key) {
	McUdp_DHCPv6_Decline *mc = new McUdp_DHCPv6_Decline(key);

	mc->common_member();
	mc->member(new MmUint("Identifier", 24, UN(0), UN(0)));

	mc->member(new MmOption_onDHCPv6("option"));

	MmHeader_onDHCPv6::add(mc);

	return(mc);
}

McUdp_DHCPv6_Reconfigure *McUdp_DHCPv6_Reconfigure::create(CSTR key) {
	McUdp_DHCPv6_Reconfigure *mc = new McUdp_DHCPv6_Reconfigure(key);

	mc->common_member();
	mc->member(new MmUint("Identifier", 24, UN(0), UN(0)));

	mc->member(new MmOption_onDHCPv6("option"));

	MmHeader_onDHCPv6::add(mc);

	return(mc);
}

McUdp_DHCPv6_InformationRequest *McUdp_DHCPv6_InformationRequest::create(CSTR key) {
	McUdp_DHCPv6_InformationRequest *mc = new McUdp_DHCPv6_InformationRequest(key);

	mc->common_member();
	mc->member(new MmUint("Identifier", 24, UN(0), UN(0)));

	mc->member(new MmOption_onDHCPv6("option"));

	MmHeader_onDHCPv6::add(mc);

	return(mc);
}

McUdp_DHCPv6_RelayForward *McUdp_DHCPv6_RelayForward::create(CSTR key) {
	McUdp_DHCPv6_RelayForward *mc = new McUdp_DHCPv6_RelayForward(key);

	mc->common_member();
	mc->member(new MmUint("HopCount", 8, UN(0), UN(0)));
	mc->member(new MmV6Addr("LinkAddr", MUST(), MUST()));
	mc->member(new MmV6Addr("PeerAddr", MUST(), MUST()));

	mc->member(new MmOption_onDHCPv6("option"));

	MmHeader_onDHCPv6::add(mc);

	return(mc);
}

McUdp_DHCPv6_RelayReply *McUdp_DHCPv6_RelayReply::create(CSTR key) {
	McUdp_DHCPv6_RelayReply *mc = new McUdp_DHCPv6_RelayReply(key);

	mc->common_member();
	mc->member(new MmUint("HopCount", 8, UN(0), UN(0)));
	mc->member(new MmV6Addr("LinkAddr", MUST(), MUST()));
	mc->member(new MmV6Addr("PeerAddr", MUST(), MUST()));

	mc->member(new MmOption_onDHCPv6("option"));

	MmHeader_onDHCPv6::add(mc);

	return(mc);
}

//
// DHCP options
//
////////////////////////////////////////////////////////////////

void McOpt_DHCPv6::common_member() {
	code_member(new MmUint("Code", 16, GENEHC(this, McOpt_DHCPv6, Code), EVALHC(this, McOpt_DHCPv6, Code)));
	length_member(new MmUint("Length", 16, GENEHC(this, McOpt_DHCPv6, Length), EVALANY()));
}

McOpt_DHCPv6_ANY *McOpt_DHCPv6_ANY::create(CSTR key) {
	McOpt_DHCPv6_ANY *mc = new McOpt_DHCPv6_ANY(key);

	mc->common_member();
	mc->member(new MmData("data"));

	MmOption_onDHCPv6::add_other(mc);

	return(mc);
}

//
// Client Identifier option
//
////////////////////////////////

McOpt_DHCPv6_CID *McOpt_DHCPv6_CID::create(CSTR key) {
	McOpt_DHCPv6_CID *mc = new McOpt_DHCPv6_CID(key);

	mc->common_member();

	mc->member(new MmDUID_onOption("duid"));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// Server Identifier option
//
////////////////////////////////

McOpt_DHCPv6_SID *McOpt_DHCPv6_SID::create(CSTR key) {
	McOpt_DHCPv6_SID *mc = new McOpt_DHCPv6_SID(key);

	mc->common_member();

	mc->member(new MmDUID_onOption("duid"));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// Identity Association for Non-temporary Addresses Option
//
////////////////////////////////

McOpt_DHCPv6_IA_NA *McOpt_DHCPv6_IA_NA::create(CSTR key) {
	McOpt_DHCPv6_IA_NA *mc = new McOpt_DHCPv6_IA_NA(key);

	mc->common_member();

	mc->member(new MmUint("Identifier", 32, UN(0), UN(0)));
	mc->member(new MmUint("Time1", 32, UN(0), UN(0)));
	mc->member(new MmUint("Time2", 32, UN(0), UN(0)));

	mc->member(new MmOption_onDHCPv6("option"));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// Identity Association for Temporary Addresses option
//
////////////////////////////////

McOpt_DHCPv6_IA_TA *McOpt_DHCPv6_IA_TA::create(CSTR key) {
	McOpt_DHCPv6_IA_TA *mc = new McOpt_DHCPv6_IA_TA(key);

	mc->common_member();

	mc->member(new MmUint("Identifier", 32, UN(0), UN(0)));

	mc->member(new MmOption_onDHCPv6("option"));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// IA Address option
//
////////////////////////////////

McOpt_DHCPv6_IA_Address *McOpt_DHCPv6_IA_Address::create(CSTR key) {
	McOpt_DHCPv6_IA_Address *mc = new McOpt_DHCPv6_IA_Address(key);

	mc->common_member();

	mc->member(new MmV6Addr("Address", MUST(), MUST()));
	mc->member(new MmUint("PreferredLifetime", 32, UN(0), UN(0)));
	mc->member(new MmUint("ValidLifetime", 32, UN(0), UN(0)));

	mc->member(new MmOption_onDHCPv6("option"));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// Option Request option
//
////////////////////////////////

McOpt_DHCPv6_OptionRequest *McOpt_DHCPv6_OptionRequest::create(CSTR key) {
	McOpt_DHCPv6_OptionRequest *mc = new McOpt_DHCPv6_OptionRequest(key);

	mc->common_member();
	mc->member(
		new MmMultiple(
			new MmUint("OptionCode", 16, UN(0), UN(0)),
			(METH_HC_MLC)&McOpt_DHCPv6_OptionRequest::HC_MLC(OptionCode)
		)
	);

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// Preference option
//
////////////////////////////////

McOpt_DHCPv6_Preference *McOpt_DHCPv6_Preference::create(CSTR key) {
	McOpt_DHCPv6_Preference *mc = new McOpt_DHCPv6_Preference(key);

	mc->common_member();

	mc->member(new MmUint("Preference", 8, UN(0), UN(0)));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// Elapsed Time option
//
////////////////////////////////

McOpt_DHCPv6_ElapsedTime *McOpt_DHCPv6_ElapsedTime::create(CSTR key) {
	McOpt_DHCPv6_ElapsedTime *mc = new McOpt_DHCPv6_ElapsedTime(key);

	mc->common_member();

	mc->member(new MmUint("Time", 16, UN(0), UN(0)));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// Relay Message option
//
////////////////////////////////

McOpt_DHCPv6_RelayMessage *McOpt_DHCPv6_RelayMessage::create(CSTR key) {
	McOpt_DHCPv6_RelayMessage *mc = new McOpt_DHCPv6_RelayMessage(key);

	mc->common_member();

	mc->member(new MmHeader_onDHCPv6("relay"));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// Authentication option
//
////////////////////////////////

McOpt_DHCPv6_Authentication *McOpt_DHCPv6_Authentication::create(CSTR key) {
	McOpt_DHCPv6_Authentication *mc = new McOpt_DHCPv6_Authentication(key);

	mc->common_member();

	mc->proto_member(new MmUint(
		"Protocol",
		8,
		GENEHC(mc, McOpt_DHCPv6_Authentication, Protocol),
		EVALHC(mc, McOpt_DHCPv6_Authentication, Protocol)
	));

	mc->member(new MmUint("Algorithm", 8, UN(1), UN(1)));
	mc->member(new MmUint("RDM", 8, UN(0), UN(0)));
	mc->member(new MmMH_Cookie64("ReplayDetection"));

	mc->member(new MmProto_onAuth("auth", mc));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// Server unicast option
//
////////////////////////////////

McOpt_DHCPv6_ServerUnicast *McOpt_DHCPv6_ServerUnicast::create(CSTR key) {
	McOpt_DHCPv6_ServerUnicast *mc = new McOpt_DHCPv6_ServerUnicast(key);

	mc->common_member();

	mc->member(new MmV6Addr("Address", MUST(), MUST()));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// Status Code Option
//
////////////////////////////////

McOpt_DHCPv6_StatusCode *McOpt_DHCPv6_StatusCode::create(CSTR key) {
	McOpt_DHCPv6_StatusCode *mc = new McOpt_DHCPv6_StatusCode(key);

	mc->common_member();

	mc->member(new MmUint("StatusCode", 16, UN(0), UN(0)));

	mc->member(new MmData("Message"));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// Rapid Commit option
//
////////////////////////////////

McOpt_DHCPv6_RapidCommit *McOpt_DHCPv6_RapidCommit::create(CSTR key) {
	McOpt_DHCPv6_RapidCommit *mc = new McOpt_DHCPv6_RapidCommit(key);

	mc->common_member();

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// User Class option
//
////////////////////////////////

McOpt_DHCPv6_UserClass *McOpt_DHCPv6_UserClass::create(CSTR key) {
	McOpt_DHCPv6_UserClass *mc = new McOpt_DHCPv6_UserClass(key);

	mc->common_member();

	mc->member(new MmUserClass_onOption("data"));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

McDHCPv6_UserClassData *McDHCPv6_UserClassData::create(CSTR key) {
	McDHCPv6_UserClassData *mc = new McDHCPv6_UserClassData(key);

	mc->length_member(new MmUint("Length", 16, GENEHC(mc, McDHCPv6_UserClassData, Length), EVALANY()));
	mc->member(new MmData("data"));

	MmUserClass_onOption::add_other(mc);

	return(mc);
}

//
// Vendor Class Option
//
////////////////////////////////

McOpt_DHCPv6_VendorClass *McOpt_DHCPv6_VendorClass::create(CSTR key) {
	McOpt_DHCPv6_VendorClass *mc = new McOpt_DHCPv6_VendorClass(key);

	mc->common_member();

	mc->member(new MmUint("EnterpriseNumber", 32, UN(0), UN(0)));

	mc->member(new MmVendorClass_onOption("data"));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

McDHCPv6_VendorClassData *McDHCPv6_VendorClassData::create(CSTR key) {
	McDHCPv6_VendorClassData *mc = new McDHCPv6_VendorClassData(key);

	mc->length_member(new MmUint("Length", 16, GENEHC(mc, McDHCPv6_VendorClassData, Length), EVALANY()));
	mc->member(new MmData("data"));

	MmVendorClass_onOption::add_other(mc);

	return(mc);
}

//
// Vendor-specific Information option
//
////////////////////////////////

McOpt_DHCPv6_VendorSpecificInfo *McOpt_DHCPv6_VendorSpecificInfo::create(CSTR key) {
	McOpt_DHCPv6_VendorSpecificInfo *mc = new McOpt_DHCPv6_VendorSpecificInfo(key);

	mc->common_member();

	mc->member(new MmUint("EnterpriseNumber", 32, UN(0), UN(0)));

	mc->member(new MmOptionData_onOption("data"));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

McDHCPv6_OptionData *McDHCPv6_OptionData::create(CSTR key) {
	McDHCPv6_OptionData *mc = new McDHCPv6_OptionData(key);

	mc->member(new MmUint("Code", 16, UN(0), UN(0)));
	mc->length_member(new MmUint("Length", 16, GENEHC(mc, McDHCPv6_OptionData, Length), EVALANY()));
	mc->member(new MmData("data"));

	MmOptionData_onOption::add_other(mc);

	return(mc);
}

//
// Interface-Id Option
//
////////////////////////////////

McOpt_DHCPv6_IID *McOpt_DHCPv6_IID::create(CSTR key) {
	McOpt_DHCPv6_IID *mc = new McOpt_DHCPv6_IID(key);

	mc->common_member();

	mc->member(new MmData("Identifier"));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// Reconfigure Message option
//
////////////////////////////////

McOpt_DHCPv6_ReconfigureMessage *McOpt_DHCPv6_ReconfigureMessage::create(CSTR key) {
	McOpt_DHCPv6_ReconfigureMessage *mc = new McOpt_DHCPv6_ReconfigureMessage(key);

	mc->common_member();

	mc->member(new MmUint("Type", 8, UN(0), UN(0)));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// Reconfigure Accept option
//
////////////////////////////////

McOpt_DHCPv6_ReconfigureAccept *McOpt_DHCPv6_ReconfigureAccept::create(CSTR key) {
	McOpt_DHCPv6_ReconfigureAccept *mc = new McOpt_DHCPv6_ReconfigureAccept(key);

	mc->common_member();

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// Identity Association for Prefix Delegation Option
//
////////////////////////////////

McOpt_DHCPv6_IA_PD *McOpt_DHCPv6_IA_PD::create(CSTR key) {
	McOpt_DHCPv6_IA_PD *mc = new McOpt_DHCPv6_IA_PD(key);

	mc->common_member();

	mc->member(new MmUint("Identifier", 32, UN(0), UN(0)));
	mc->member(new MmUint("Time1", 32, UN(0), UN(0)));
	mc->member(new MmUint("Time2", 32, UN(0), UN(0)));

	mc->member(new MmOption_onDHCPv6("option"));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// IA_PD Prefix option
//
////////////////////////////////

McOpt_DHCPv6_IA_Prefix *McOpt_DHCPv6_IA_Prefix::create(CSTR key) {
	McOpt_DHCPv6_IA_Prefix *mc = new McOpt_DHCPv6_IA_Prefix(key);

	mc->common_member();

	mc->member(new MmUint("PreferredLifetime", 32, UN(0), UN(0)));
	mc->member(new MmUint("ValidLifetime", 32, UN(0), UN(0)));
	mc->member(new MmUint("PrefixLength", 8, UN(0), UN(0)));
	mc->member(new MmV6Addr("Prefix", MUST(), MUST()));

	mc->member(new MmOption_onDHCPv6("option"));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// Domain Name Server option
//
////////////////////////////////

McOpt_DHCPv6_DNS_Servers *McOpt_DHCPv6_DNS_Servers::create(CSTR key) {
	McOpt_DHCPv6_DNS_Servers *mc = new McOpt_DHCPv6_DNS_Servers(key);

	mc->common_member();
	mc->member(
		new MmMultiple(
			new MmV6Addr("Address", MUST(), MUST()),
			(METH_HC_MLC)&McOpt_DHCPv6_DNS_Servers::HC_MLC(Address)
		)
	);

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// Domain Search List option
//
////////////////////////////////

McOpt_DHCPv6_DNS_SearchList *McOpt_DHCPv6_DNS_SearchList::create(CSTR key) {
	McOpt_DHCPv6_DNS_SearchList *mc = new McOpt_DHCPv6_DNS_SearchList(key);

	mc->common_member();
	mc->member(new MmDNSName("SearchString", MUST(), MUST()));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// Network Time Protocol (NTP) Servers option
//
////////////////////////////////

McOpt_DHCPv6_NTP_Servers *McOpt_DHCPv6_NTP_Servers::create(CSTR key) {
	McOpt_DHCPv6_NTP_Servers *mc = new McOpt_DHCPv6_NTP_Servers(key);

	mc->common_member();
	mc->member(
		new MmMultiple(
			new MmV6Addr("Address", MUST(), MUST()),
			(METH_HC_MLC)&McOpt_DHCPv6_NTP_Servers::HC_MLC(Address)
		)
	);

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// IEEE 1003.1 POSIX Timezone option
//
////////////////////////////////

McOpt_DHCPv6_NTP_Timezone *McOpt_DHCPv6_NTP_Timezone::create(CSTR key) {
	McOpt_DHCPv6_NTP_Timezone *mc = new McOpt_DHCPv6_NTP_Timezone(key);

	mc->common_member();
	mc->member(new MmTimezone("Timezone", MUST(), MUST()));

	MmOption_onDHCPv6::add(mc);

	return(mc);
}

//
// DHCP unique identifier (DUID)
//
////////////////////////////////////////////////////////////////

void McDHCPv6_DUID::common_member() {
	type_member(new MmUint("Type", 16, GENEHC(this, McDHCPv6_DUID, Type), EVALHC(this, McDHCPv6_DUID, Type)));
	return;
}

McDHCPv6_DUID_ANY *McDHCPv6_DUID_ANY::create(CSTR key) {
	McDHCPv6_DUID_ANY *mc = new McDHCPv6_DUID_ANY(key);

	mc->common_member();
	mc->member(new MmData("data"));

	MmDUID_onOption::add_other(mc);

	return(mc);
}

//
// DUID based on link-layer address plus time [DUID-LLT]
//
////////////////////////////////

McDHCPv6_DUID_LLT_ANY *McDHCPv6_DUID_LLT_ANY::create(CSTR key) {
	McDHCPv6_DUID_LLT_ANY *mc = new McDHCPv6_DUID_LLT_ANY(key);

	mc->common_member();

	mc->hwtype_member(
		new MmUint(
			"HardwareType",
			16,
			GENEHC(mc, McDHCPv6_DUID_LLT, HardwareType),
			EVALHC(mc, McDHCPv6_DUID_LLT, HardwareType)
		)
	);
	mc->member(new MmUint("Time", 32, UN(0), UN(0)));
	mc->member(new MmData("LinkLayerAddress"));

	MmLLT_onDUID::add_other(mc);

	return(mc);
}

McDHCPv6_DUID_LLT_Ether *McDHCPv6_DUID_LLT_Ether::create(CSTR key) {
	McDHCPv6_DUID_LLT_Ether *mc = new McDHCPv6_DUID_LLT_Ether(key);

	mc->common_member();

	mc->hwtype_member(
		new MmUint(
			"HardwareType",
			16,
			GENEHC(mc, McDHCPv6_DUID_LLT, HardwareType),
			EVALHC(mc, McDHCPv6_DUID_LLT, HardwareType)
		)
	);
	mc->member(new MmUint("Time", 32, UN(0), UN(0)));
	mc->member(new MmEther("LinkLayerAddress", MUST(), MUST()));

	MmLLT_onDUID::add(mc);

	return(mc);
}

//
// DUID assigned by vendor based on Enterprise number [DUID-EN]
//
////////////////////////////////

McDHCPv6_DUID_EN *McDHCPv6_DUID_EN::create(CSTR key) {
	McDHCPv6_DUID_EN *mc = new McDHCPv6_DUID_EN(key);

	mc->common_member();

	mc->member(new MmUint("EnterpriseNumber", 32, UN(0), UN(0)));
	mc->member(new MmData("Identifier"));

	MmDUID_onOption::add(mc);

	return(mc);
}

//
// DUID based on link-layer address [DUID-LL]
//
////////////////////////////////

McDHCPv6_DUID_LL_ANY *McDHCPv6_DUID_LL_ANY::create(CSTR key) {
	McDHCPv6_DUID_LL_ANY *mc = new McDHCPv6_DUID_LL_ANY(key);

	mc->common_member();

	mc->hwtype_member(
		new MmUint(
			"HardwareType",
			16,
			GENEHC(mc, McDHCPv6_DUID_LL, HardwareType),
			EVALHC(mc, McDHCPv6_DUID_LL, HardwareType)
		)
	);
	mc->member(new MmData("LinkLayerAddress"));

	MmLL_onDUID::add_other(mc);

	return(mc);
}

McDHCPv6_DUID_LL_Ether *McDHCPv6_DUID_LL_Ether::create(CSTR key) {
	McDHCPv6_DUID_LL_Ether *mc = new McDHCPv6_DUID_LL_Ether(key);

	mc->common_member();

	mc->hwtype_member(
		new MmUint(
			"HardwareType",
			16,
			GENEHC(mc, McDHCPv6_DUID_LL, HardwareType),
			EVALHC(mc, McDHCPv6_DUID_LL, HardwareType)
		)
	);
	mc->member(new MmEther("LinkLayerAddress", MUST(), MUST()));

	MmLL_onDUID::add(mc);

	return(mc);
}

//
// Authentication of DHCP Messages
//
////////////////////////////////////////////////////////////////

McDHCPv6_Auth_ANY *McDHCPv6_Auth_ANY::create(CSTR key) {
	McDHCPv6_Auth_ANY *mc = new McDHCPv6_Auth_ANY(key);

	mc->member(new MmData("data"));

	MmProto_onAuth::add_other(mc);

	return(mc);
}

McDHCPv6_Auth_Delayed *McDHCPv6_Auth_Delayed::create(CSTR key) {
	McDHCPv6_Auth_Delayed *mc = new McDHCPv6_Auth_Delayed(key);

	mc->member(new MmDHCPRealm("Realm", MUST(), MUST()));
	mc->member(new MmUint("Identifier", 32, UN(0), UN(0)));
	mc->member(new MmDHCPAuth("Authenticator"));

	MmProto_onAuth::add(mc);

	return(mc);
}

McDHCPv6_Auth_ReconfigureKey *McDHCPv6_Auth_ReconfigureKey::create(CSTR key) {
	McDHCPv6_Auth_ReconfigureKey *mc = new McDHCPv6_Auth_ReconfigureKey(key);

	mc->member(new MmUint("Type", 8, UN(0), UN(0)));
	mc->member(new MmDHCPAuth("data"));

	MmProto_onAuth::add(mc);

	return(mc);
}

//
// RFC 1112/2236/3376
//
//     IGMPv1/IGMPv2/IGMPv3
//
////////////////////////////////////////////////////////////////////////////////

void McUpp_IGMP::common_member(){
	int32_t Type = igmpType();
	type_member(
		new MmUint( "Type",		8,	UN(Type),UN(Type) ) );
	member( new MmUint( "MaxRespTime",		8,	UN(0),	UN(0) ) );
	member( new MmUppChecksum( "Checksum",	16,
			DEF_MUSTCHKSUM, DEF_NOPSEUDO ) );
	}
///////////////////////////////////////////////////////////////////////////////
McUpp_IGMP_ANY* McUpp_IGMP_ANY::create(CSTR key){
	McUpp_IGMP_ANY* mc = new McUpp_IGMP_ANY(key);
	mc->common_member();
	mc->member( new MmData( "data" ) );
	//dict
	MmHeader_onIGMP::add_other(mc);	//Upp_IGMP::header= (upper=)
	return mc;}
////////////////////////////////////////////////////////////////////////////////
McUpp_IGMP_IGMPv3Report* McUpp_IGMP_IGMPv3Report::create(CSTR key){
	McUpp_IGMP_IGMPv3Report* mc = new McUpp_IGMP_IGMPv3Report(key);
	mc->common_member();
	mc->member( new MmUint( "Reserved",	16,	UN(0),	UN(0) ) );
	mc->member(
		new MmUint( "NumOfGroupAddrRecords",	16,
			GENEHC(mc,McUpp_IGMP_IGMPv3Report,NumOfGroupAddrRecords),
			EVALANY(),		ICVCONST() ) );
	mc->member( new MmIGMPv3AddrRecord_onIGMP("IGMPv3_AddrRecord"));
	//dict
	MmHeader_onIGMP::add(mc);	//Upp_IGMP::header= (upper=)
	return mc;}
//////////////////////////////////////////////////////////////////////////////
McUpp_IGMP_IGMPQuery* McUpp_IGMP_IGMPQuery::create(CSTR key){
	McUpp_IGMP_IGMPQuery* mc = new McUpp_IGMP_IGMPQuery(key);
	mc->common_member();
	mc->member( new MmV4Addr( "GroupAddress",	MUST(),	MUST() ) );
	// ICMPv1 and IGMPv2 Query only have GroupAddress
#ifndef NOT_USE_IGMPV3_QUERY
	mc->member(new MmUint("Resv",		4,	UN(0),	UN(0)));
	mc->member(new MmUint("SFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("QRV",		3,	UN(0),	UN(0)));
	mc->member(new MmUint("QQIC",		8,	UN(0),	UN(0)));
	mc->member(
 		new MmUint( "NumOfSources",	16,
			GENEHC(mc,McUpp_IGMP_IGMPQuery,NumOfSources),
 			EVALANY(),		ICVCONST() ) );
	mc->member(
		new MmMultiple(
			new MmV4Addr("SourceAddress", MUST(), MUST()),
			(METH_HC_MLC)&McUpp_IGMP_IGMPQuery::HC_MLC(SourceAddress)
		)
	);
#endif	// NOT_USE_IGMPV3_QUERY
	//dict
	MmHeader_onIGMP::add(mc);		//Upp_IGMP::header= (upper=)
	return mc;}
//////////////////////////////////////////////////////////////////////////////
McUpp_IGMP_IGMPv1Report* McUpp_IGMP_IGMPv1Report::create(CSTR key){
	McUpp_IGMP_IGMPv1Report* mc = new McUpp_IGMP_IGMPv1Report(key);
	mc->common_member();
	mc->member( new MmV4Addr( "GroupAddress",	MUST(),	MUST() ) );
	//dict
	MmHeader_onIGMP::add(mc);	//Upp_IGMP::header= (upper=)
	return mc;}
//////////////////////////////////////////////////////////////////////////////
McUpp_IGMP_IGMPv2Report* McUpp_IGMP_IGMPv2Report::create(CSTR key){
	McUpp_IGMP_IGMPv2Report* mc = new McUpp_IGMP_IGMPv2Report(key);
	mc->common_member();
	mc->member( new MmV4Addr( "GroupAddress",	MUST(),	MUST() ) );
	//dict
	MmHeader_onIGMP::add(mc);	//Upp_IGMP::header= (upper=)
	return mc;}
//////////////////////////////////////////////////////////////////////////////
McUpp_IGMP_IGMPv2LeaveGroup* McUpp_IGMP_IGMPv2LeaveGroup::create(CSTR key){
	McUpp_IGMP_IGMPv2LeaveGroup* mc = new McUpp_IGMP_IGMPv2LeaveGroup(key);
	mc->common_member();
	mc->member( new MmV4Addr( "GroupAddress",	MUST(),	MUST() ) );
	//dict
	MmHeader_onIGMP::add(mc);	//Upp_IGMP::header= (upper=)
	return mc;}
//////////////////////////////////////////////////////////////////////////////
McIGMPv3_AddrRecord *McIGMPv3_AddrRecord::create(CSTR key) {	
	McIGMPv3_AddrRecord *mc = new McIGMPv3_AddrRecord(key);
	mc->type_member(new MmUint("Type", 8, UN(0), UN(0)));
	mc->auxdatalen_member(new MmUint("AuxDataLen", 8, UN(0), UN(0)));
	mc->numofsrc_member(
		new MmUint( "NumOfSources",	16,
			GENEHC(mc,McIGMPv3_AddrRecord,NumOfSources),
			EVALANY(),		ICVCONST() ) );
	mc->mcastaddr_member(new MmV4Addr("GroupAddress", MUST(), MUST()));
	mc->member(
		new MmMultiple(
			new MmV4Addr("SourceAddress", MUST(), MUST()),
			(METH_HC_MLC)&McIGMPv3_AddrRecord::HC_MLC(SourceAddress)
 		)
 	);
	mc->member( new MmData( "AuxData"));
 	// dict
	MmIGMPv3AddrRecord_onIGMP::add(mc);
	return(mc);
}

//
// ICMPv6 MLDv2 Messages
//
////////////////////////////////////////////////////////////////
McUpp_ICMPv6_MLDv2Report *McUpp_ICMPv6_MLDv2Report::create(CSTR key) {
	McUpp_ICMPv6_MLDv2Report *mc = new McUpp_ICMPv6_MLDv2Report(key);
	mc->common_member();
	mc->member( new MmUint( "Reserved",	16,	UN(0),	UN(0) ) );
	mc->member(
		new MmUint( "NumOfMcastAddrRecords",	16,
			GENEHC(mc,McUpp_ICMPv6_MLDv2Report,NumOfMcastAddrRecords),
			EVALANY(),		ICVCONST() ) );
	mc->member( new MmMLDv2AddrRecord_onICMPv6( "MLDv2_AddrRecord"));
	// dict
	MmHeader_onICMPv6::add(mc);
	return(mc);
}

McUpp_ICMPv6_MLDQuery *McUpp_ICMPv6_MLDQuery::create(CSTR key) {
	McUpp_ICMPv6_MLDQuery *mc = new McUpp_ICMPv6_MLDQuery(key);
	mc->common_member();
	mc->member( new MmUint( "MaxResponseDelay",16,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "Reserved",	16,	UN(0),	UN(0) ) );
	mc->member( new MmV6Addr( "MulticastAddress",	MUST(),	MUST() ) );
#ifdef NOT_USE_MLDV2_QUERY
#ifdef USE_MLDV2_QUERY
	mc->member(new MmUint("Resv",		4,	UN(0),	UN(0)));
	mc->member(new MmUint("SFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("QRV",		3,	UN(0),	UN(0)));
	mc->member(new MmUint("QQIC",		8,	UN(0),	UN(0)));
	mc->member(
		new MmUint( "NumOfSources",	16,
			GENEHC(mc,McUpp_ICMPv6_MLDQuery,NumOfSources),
			EVALANY(),		ICVCONST() ) );
	
	mc->member(
		new MmMultiple(
			new MmV6Addr("SourceAddress", MUST(), MUST()),
			(METH_HC_MLC)&McUpp_ICMPv6_MLDQuery::HC_MLC(SourceAddress)
		)
	);
#endif	// USE_MLDV2_QUERY
#endif	// NOT_USE_MLDV2_QUERY
	// dict
	MmHeader_onICMPv6::add(mc);
	return mc;
}

#ifndef NOT_USE_MLDV2_QUERY
McUpp_ICMPv6_MLDv2Query *McUpp_ICMPv6_MLDv2Query::create(CSTR key) {
	McUpp_ICMPv6_MLDv2Query *mc = new McUpp_ICMPv6_MLDv2Query(key);
#if 0
	mc->common_member();
#else	// 0
	int32_t Type = TP_ICMPv6_MLDQuery;

	mc->type_member(
		new MmUint( "Type",		8,	UN(Type),UN(Type) ) );
	mc->Code_member(
		new MmUint( "Code",		8,	UN(0),	UN(0) ) );
	mc->member( new MmUppChecksum( "Checksum",	16 ) );
	//isEqual(DEF_MUSTCHKSUM,DEF_MUSTPSEUDO,ICVCONST)
#endif	// 0
	mc->member( new MmUint( "MaxResponseDelay",16,	UN(0),	UN(0) ) );
	mc->member( new MmUint( "Reserved",	16,	UN(0),	UN(0) ) );
	mc->member( new MmV6Addr( "MulticastAddress",	MUST(),	MUST() ) );
	mc->member(new MmUint("Resv",		4,	UN(0),	UN(0)));
	mc->member(new MmUint("SFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("QRV",		3,	UN(0),	UN(0)));
	mc->member(new MmUint("QQIC",		8,	UN(0),	UN(0)));
	mc->member(
		new MmUint( "NumOfSources",	16,
			GENEHC(mc,McUpp_ICMPv6_MLDv2Query,NumOfSources),
			EVALANY(),		ICVCONST() ) );
	
	mc->member(
		new MmMultiple(
			new MmV6Addr("SourceAddress", MUST(), MUST()),
			(METH_HC_MLC)&McUpp_ICMPv6_MLDv2Query::HC_MLC(SourceAddress)
		)
	);
	// dict
	MmHeader_onICMPv6::add(mc);
	return mc;
}
#endif	// NOT_USE_MLDV2_QUERY

McMLDv2_AddrRecord *McMLDv2_AddrRecord::create(CSTR key) {	
	McMLDv2_AddrRecord *mc = new McMLDv2_AddrRecord(key);
	mc->type_member(new MmUint("Type", 8, UN(0), UN(0)));
	mc->auxdatalen_member(new MmUint("AuxDataLen", 8, UN(0), UN(0)));
	mc->numofsrc_member(
		new MmUint( "NumOfSources",	16,
			GENEHC(mc,McMLDv2_AddrRecord,NumOfSources),
			EVALANY(),		ICVCONST() ) );
	mc->mcastaddr_member(new MmV6Addr("MulticastAddress", MUST(), MUST()));
	mc->member(
		new MmMultiple(
			new MmV6Addr("SourceAddress", MUST(), MUST()),
			(METH_HC_MLC)&McMLDv2_AddrRecord::HC_MLC(SourceAddress)
		)
	);
	mc->member( new MmData( "AuxData"));
	// dict
	MmMLDv2AddrRecord_onICMPv6::add(mc);
	return(mc);
}



////////////////////////////////////////////////////////////////
// RFC 3261 - SIP: Session Initiation Protocol                //
////////////////////////////////////////////////////////////////

McUdp_SIP *McUdp_SIP::create(CSTR key) {
	McUdp_SIP *mc = new McUdp_SIP(key);

//	mc->member(new MmData("data"));
	mc->member(new MmSIPMsg("message", MUST(), MUST()));

	MmUpper_onUpper::add(mc);

	return(mc);
}

#if !defined(NOT_USE_SNMP)
////////////////////////////////////////////////////////////////////////////////
//     SNMP
////////////////////////////////////////////////////////////////////////////////
McUdp_SNMP *McUdp_SNMP::create(CSTR key) {
	McUdp_SNMP *mc = new McUdp_SNMP(key);

	mc->member(new MmPdu_SNMP_onSNMP("snmpmsg"));

	MmUpper_onUpper::add(mc);

	return(mc);
}
McUdp_SNMPTrap *McUdp_SNMPTrap::create(CSTR key) {
	McUdp_SNMPTrap *mc = new McUdp_SNMPTrap(key);

	mc->member(new MmPdu_SNMP_onSNMP("snmpmsg"));

	MmUpper_onUpper::add(mc);

	return(mc);
}
////////////////////////////////////////////////////////////////////////////////
// SNMP PDU
////////////////////////////////////////////////////////////////////////////////
void McPdu_SNMP::common_member() {
	type_member(new MmUint("Type", 8, GENEHC(this, McPdu_SNMP, Type), EVALHC(this, McPdu_SNMP, Type)));
	length_member(new MmUint("Length", 8, GENEHC(this, McPdu_SNMP, Length), EVALANY()));
}
////////////////////////////////
McPdu_SNMP_ANY *McPdu_SNMP_ANY::create(CSTR key) {
	McPdu_SNMP_ANY *mc = new McPdu_SNMP_ANY(key);
	mc->common_member();
	mc->member(new MmData("data"));
	MmPdu_SNMP_onSNMP::add_other(mc);
	MmPdu_SNMP_onPDU::add_other(mc);
	MmSNMP_Version_onSNMP_Message::add_other(mc);
	MmSNMP_Community_onSNMP_Message::add_other(mc);
	MmSNMP_RequestID_onSNMP_Message::add_other(mc);
	MmSNMP_ErrorStatus_onSNMP_Message::add_other(mc);
	MmSNMP_ErrorIndex_onSNMP_Message::add_other(mc);

	MmSNMP_IPAddress_onTrap_Message::add_other(mc);
	MmSNMP_GenericTrap_onTrap_Message::add_other(mc);
	MmSNMP_SpecificTrap_onTrap_Message::add_other(mc);
	MmSNMP_TimeStamp_onTrap_Message::add_other(mc);
	MmSNMP_ObjectID_onTrap_Message::add_other(mc);

	MmSNMP_VarBindList_onPDU::add_other(mc);
	MmSNMP_VarBind_onPDU::add_other(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_Message *McPdu_SNMP_Message::create(CSTR key) {
	McPdu_SNMP_Message *mc = new McPdu_SNMP_Message(key);
	mc->common_member();
	mc->member(new MmSNMP_Version_onSNMP_Message("version"));
	mc->member(new MmSNMP_Community_onSNMP_Message("community"));
	mc->member(new MmPdu_SNMP_onSNMP("pdumsg"));
	MmPdu_SNMP_onSNMP::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_Version *McPdu_SNMP_Version::create(CSTR key) {
	McPdu_SNMP_Version *mc = new McPdu_SNMP_Version(key);
	mc->common_member();
	// mc->member(new MmUint( "Version", 8, UN(0), UN(0)));
	mc->member(new MmSNMPVersion( "Version", 8, UN(0), UN(0)));
	MmSNMP_Version_onSNMP_Message::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_Community *McPdu_SNMP_Community::create(CSTR key) {
	McPdu_SNMP_Community *mc = new McPdu_SNMP_Community(key);
	mc->common_member();
	mc->member(new MmSNMPString("Community", MUST(), MUST()));
	MmSNMP_Community_onSNMP_Message::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_GetRequest *McPdu_SNMP_GetRequest::create(CSTR key) {
	McPdu_SNMP_GetRequest *mc = new McPdu_SNMP_GetRequest(key);
	mc->common_member();
	mc->member(new MmSNMP_RequestID_onSNMP_Message("pdu_requestid"));
	mc->member(new MmSNMP_ErrorStatus_onSNMP_Message("pdu_errorstatus"));
	mc->member(new MmSNMP_ErrorIndex_onSNMP_Message("pdu_errorindex"));
	mc->member(new MmSNMP_VarBindList_onPDU("pdu_varbindlist"));
	MmPdu_SNMP_onSNMP::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_GetNextRequest *McPdu_SNMP_GetNextRequest::create(CSTR key) {
	McPdu_SNMP_GetNextRequest *mc = new McPdu_SNMP_GetNextRequest(key);
	mc->common_member();
	mc->member(new MmSNMP_RequestID_onSNMP_Message("pdu_requestid"));
	mc->member(new MmSNMP_ErrorStatus_onSNMP_Message("pdu_errorstatus"));
	mc->member(new MmSNMP_ErrorIndex_onSNMP_Message("pdu_errorindex"));
	mc->member(new MmSNMP_VarBindList_onPDU("pdu_varbindlist"));
	MmPdu_SNMP_onSNMP::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_SetRequest *McPdu_SNMP_SetRequest::create(CSTR key) {
	McPdu_SNMP_SetRequest *mc = new McPdu_SNMP_SetRequest(key);
	mc->common_member();
	mc->member(new MmSNMP_RequestID_onSNMP_Message("pdu_requestid"));
	mc->member(new MmSNMP_ErrorStatus_onSNMP_Message("pdu_errorstatus"));
	mc->member(new MmSNMP_ErrorIndex_onSNMP_Message("pdu_errorindex"));
	mc->member(new MmSNMP_VarBindList_onPDU("pdu_varbindlist"));
	MmPdu_SNMP_onSNMP::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_GetResponse *McPdu_SNMP_GetResponse::create(CSTR key) {
	McPdu_SNMP_GetResponse *mc = new McPdu_SNMP_GetResponse(key);
	mc->common_member();
	mc->member(new MmSNMP_RequestID_onSNMP_Message("pdu_requestid"));
	mc->member(new MmSNMP_ErrorStatus_onSNMP_Message("pdu_errorstatus"));
	mc->member(new MmSNMP_ErrorIndex_onSNMP_Message("pdu_errorindex"));
	mc->member(new MmSNMP_VarBindList_onPDU("pdu_varbindlist"));
	MmPdu_SNMP_onSNMP::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_Trap *McPdu_SNMP_Trap::create(CSTR key) {
	McPdu_SNMP_Trap *mc = new McPdu_SNMP_Trap(key);
	mc->common_member();
	mc->member(new MmSNMP_ObjectID_onTrap_Message("pdu_enterpriseoid"));
	mc->member(new MmSNMP_IPAddress_onTrap_Message("pdu_ipaddress"));
	mc->member(new MmSNMP_GenericTrap_onTrap_Message("pdu_generictrap"));
	mc->member(new MmSNMP_SpecificTrap_onTrap_Message("pdu_specifictrap"));
	mc->member(new MmSNMP_TimeStamp_onTrap_Message("pdu_timestamp"));
	mc->member(new MmSNMP_VarBindList_onPDU("pdu_varbindlist"));
	MmPdu_SNMP_onSNMP::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_GetBulk *McPdu_SNMP_GetBulk::create(CSTR key) {
	McPdu_SNMP_GetBulk *mc = new McPdu_SNMP_GetBulk(key);
	mc->common_member();
	mc->member(new MmSNMP_RequestID_onSNMP_Message("pdu_requestid"));
	mc->member(new MmSNMP_ErrorStatus_onSNMP_Message("pdu_errorstatus"));
	mc->member(new MmSNMP_ErrorIndex_onSNMP_Message("pdu_errorindex"));
	mc->member(new MmSNMP_VarBindList_onPDU("pdu_varbindlist"));
	MmPdu_SNMP_onSNMP::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_Inform *McPdu_SNMP_Inform::create(CSTR key) {
	McPdu_SNMP_Inform *mc = new McPdu_SNMP_Inform(key);
	mc->common_member();
	mc->member(new MmSNMP_RequestID_onSNMP_Message("pdu_requestid"));
	mc->member(new MmSNMP_ErrorStatus_onSNMP_Message("pdu_errorstatus"));
	mc->member(new MmSNMP_ErrorIndex_onSNMP_Message("pdu_errorindex"));
	mc->member(new MmSNMP_VarBindList_onPDU("pdu_varbindlist"));
	MmPdu_SNMP_onSNMP::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_Trap2 *McPdu_SNMP_Trap2::create(CSTR key) {
	McPdu_SNMP_Trap2 *mc = new McPdu_SNMP_Trap2(key);
	mc->common_member();
	mc->member(new MmSNMP_RequestID_onSNMP_Message("pdu_requestid"));
	mc->member(new MmSNMP_ErrorStatus_onSNMP_Message("pdu_errorstatus"));
	mc->member(new MmSNMP_ErrorIndex_onSNMP_Message("pdu_errorindex"));
	mc->member(new MmSNMP_VarBindList_onPDU("pdu_varbindlist"));
	MmPdu_SNMP_onSNMP::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_Report *McPdu_SNMP_Report::create(CSTR key) {
	McPdu_SNMP_Report *mc = new McPdu_SNMP_Report(key);
	mc->common_member();
	mc->member(new MmSNMP_RequestID_onSNMP_Message("pdu_requestid"));
	mc->member(new MmSNMP_ErrorStatus_onSNMP_Message("pdu_errorstatus"));
	mc->member(new MmSNMP_ErrorIndex_onSNMP_Message("pdu_errorindex"));
	mc->member(new MmSNMP_VarBindList_onPDU("pdu_varbindlist"));
	MmPdu_SNMP_onSNMP::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_RequestID *McPdu_SNMP_RequestID::create(CSTR key) {
	McPdu_SNMP_RequestID *mc = new McPdu_SNMP_RequestID(key);
	mc->common_member();
	mc->member(new MmUint( "RequestID", 32, UN(0), UN(0)));
	MmSNMP_RequestID_onSNMP_Message::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_ErrorStatus *McPdu_SNMP_ErrorStatus::create(CSTR key) {
	McPdu_SNMP_ErrorStatus *mc = new McPdu_SNMP_ErrorStatus(key);
	mc->common_member();
	mc->member(new MmErrorStatus( "ErrorStatus", 8, UN(0), UN(0)));
	MmSNMP_ErrorStatus_onSNMP_Message::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_ErrorIndex *McPdu_SNMP_ErrorIndex::create(CSTR key) {
	McPdu_SNMP_ErrorIndex *mc = new McPdu_SNMP_ErrorIndex(key);
	mc->common_member();
	mc->member(new MmUint( "ErrorIndex", 8, UN(0), UN(0)));
	MmSNMP_ErrorIndex_onSNMP_Message::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_GenericTrap *McPdu_SNMP_GenericTrap::create(CSTR key) {
	McPdu_SNMP_GenericTrap *mc = new McPdu_SNMP_GenericTrap(key);
	mc->common_member();
	mc->member(new MmUint( "GenericTrap", 8, UN(0), UN(0)));
	MmSNMP_GenericTrap_onTrap_Message::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_SpecificTrap *McPdu_SNMP_SpecificTrap::create(CSTR key) {
	McPdu_SNMP_SpecificTrap *mc = new McPdu_SNMP_SpecificTrap(key);
	mc->common_member();
	mc->member(new MmUint( "SpecificTrap", 8, UN(0), UN(0)));
	MmSNMP_SpecificTrap_onTrap_Message::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_VarBindList *McPdu_SNMP_VarBindList::create(CSTR key) {
	McPdu_SNMP_VarBindList *mc = new McPdu_SNMP_VarBindList(key);
	mc->common_member();
	mc->member(new MmSNMP_VarBind_onPDU("pdu"));
	MmSNMP_VarBindList_onPDU::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_VarBind *McPdu_SNMP_VarBind::create(CSTR key) {
	McPdu_SNMP_VarBind *mc = new McPdu_SNMP_VarBind(key);
	mc->common_member();
	mc->member(new MmPdu_SNMP_onPDU("pdu"));
	MmSNMP_VarBind_onPDU::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_Boolean *McPdu_SNMP_Boolean::create(CSTR key) {
	McPdu_SNMP_Boolean *mc = new McPdu_SNMP_Boolean(key);
	mc->common_member();
	mc->member(new MmData("data")); // XXXX
	MmPdu_SNMP_onPDU::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_Integer *McPdu_SNMP_Integer::create(CSTR key) {
	McPdu_SNMP_Integer *mc = new McPdu_SNMP_Integer(key);
	mc->common_member();
	mc->member(new MmData("data")); // XXX
	MmPdu_SNMP_onPDU::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_BitString *McPdu_SNMP_BitString::create(CSTR key) {
	McPdu_SNMP_BitString *mc = new McPdu_SNMP_BitString(key);
	mc->common_member();
	mc->member(new MmData("data")); // XXX
	MmPdu_SNMP_onPDU::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_OctString *McPdu_SNMP_OctString::create(CSTR key) {
	McPdu_SNMP_OctString *mc = new McPdu_SNMP_OctString(key);
	mc->common_member();
	mc->member(new MmSNMPString("OctString", MUST(), MUST()));
	MmPdu_SNMP_onPDU::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_Null *McPdu_SNMP_Null::create(CSTR key) {
	McPdu_SNMP_Null *mc = new McPdu_SNMP_Null(key);
	mc->common_member();
	mc->member(new MmData("data"));
	MmPdu_SNMP_onPDU::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_ObjectID *McPdu_SNMP_ObjectID::create(CSTR key) {
	McPdu_SNMP_ObjectID *mc = new McPdu_SNMP_ObjectID(key);
	mc->common_member();
	mc->member(new MmObjectID("ObjectID", MUST(), MUST())); // XXXX
	MmSNMP_ObjectID_onTrap_Message::add(mc);
	MmPdu_SNMP_onPDU::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_Sequence *McPdu_SNMP_Sequence::create(CSTR key) {
	McPdu_SNMP_Sequence *mc = new McPdu_SNMP_Sequence(key);
	mc->common_member();
	mc->member(new MmData("data")); // XXXX
	MmPdu_SNMP_onPDU::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_Set *McPdu_SNMP_Set::create(CSTR key) {
	McPdu_SNMP_Set *mc = new McPdu_SNMP_Set(key);
	mc->common_member();
	mc->member(new MmData("data")); // XXXX
	MmPdu_SNMP_onPDU::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_Opaque *McPdu_SNMP_Opaque::create(CSTR key) {
	McPdu_SNMP_Opaque *mc = new McPdu_SNMP_Opaque(key);
	mc->common_member();
	mc->member(new MmPdu_SNMP_onPDU("pdu"));
	MmPdu_SNMP_onPDU::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_IPAddress *McPdu_SNMP_IPAddress::create(CSTR key) {
	McPdu_SNMP_IPAddress *mc = new McPdu_SNMP_IPAddress(key);
	mc->common_member();
	mc->member(new MmV4Addr("IPAddress"));
	MmSNMP_IPAddress_onTrap_Message::add(mc);
	MmPdu_SNMP_onPDU::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_Counter *McPdu_SNMP_Counter::create(CSTR key) {
	McPdu_SNMP_Counter *mc = new McPdu_SNMP_Counter(key);
	mc->common_member();
	mc->member(new MmData("Counter")); // XXXX
	MmPdu_SNMP_onPDU::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_Gauge *McPdu_SNMP_Gauge::create(CSTR key) {
	McPdu_SNMP_Gauge *mc = new McPdu_SNMP_Gauge(key);
	mc->common_member();
	mc->member(new MmData("Gauge")); // XXXX
	MmPdu_SNMP_onPDU::add(mc);
	return(mc);
}
////////////////////////////////
McPdu_SNMP_TimeTicks *McPdu_SNMP_TimeTicks::create(CSTR key) {
	McPdu_SNMP_TimeTicks *mc = new McPdu_SNMP_TimeTicks(key);
	mc->common_member();
	mc->member(new MmData("TimeTicks")); // XXXX
	MmSNMP_TimeStamp_onTrap_Message::add(mc);
	MmPdu_SNMP_onPDU::add(mc);
	return(mc);
}
#endif //NOT_USE_SNMP



////////////////////////////////////////////////////////////////
// RFC 2408 - ISAKMP                                          //
////////////////////////////////////////////////////////////////
McUdp_ISAKMP *
McUdp_ISAKMP::create(CSTR key, CSTR tophdrkey)
{
	addCompound(tophdr_ = McHdr_ISAKMP::create(tophdrkey));
	McUdp_ISAKMP *mc = new McUdp_ISAKMP(key);

	mc->member(new MmTopHdr("header", tophdr_));
	mc->member(new MmEncrypt_onISAKMP("encrypt", tophdr_));
	mc->member(new MmPayload_onISAKMP("payload"));

	MmUpper_onUpper::add(mc);

	return(mc);
}



////////////////////////////////
// ISAKMP Header              //
////////////////////////////////
McHdr_ISAKMP *
McHdr_ISAKMP::create(CSTR key)
{
	McHdr_ISAKMP *mc = new McHdr_ISAKMP(key);

	mc->member(new MmMH_Cookie64("InitiatorCookie"));
	mc->member(new MmMH_Cookie64("ResponderCookie"));

	mc->nextType_member(
		new MmUint(
			"NextPayload",
			8,
			GENEHC(mc, McHdr_ISAKMP, NextPayload),
			EVALHC(mc, McHdr_ISAKMP, NextPayload)
		)
	);

	mc->member(new MmUint("MjVer",		4,	UN(1),	UN(1)));
	mc->member(new MmUint("MnVer",		4,	UN(0),	UN(0)));
	mc->member(new MmUint("ExchangeType",	8,	UN(0),	UN(0)));
	mc->member(new MmUint("Reserved",	5,	UN(0),	UN(0)));
	mc->member(new MmUint("AFlag",		1,	UN(0),	UN(0)));
	mc->member(new MmUint("CFlag",		1,	UN(0),	UN(0)));
	mc->EFlag_member(
		new MmUint(
			"EFlag",
			1,
			GENEHC(mc, McHdr_ISAKMP, EFlag),
			EVALHC(mc, McHdr_ISAKMP, EFlag)
		)
	);

	mc->MIDmember(
		new MmISAKMP_MessageID("MessageID",	32,	UN(0),	UN(0)));

	mc->Layerlength_member(
		new MmUint(
			"Length",
			32,
			GENEHC(mc, McHdr_ISAKMP, Length), 
			EVALANY()
		)
	);

	return(mc);
}



////////////////////////////////
// ISAKMP Generic Payload     //
////////////////////////////////
void
McPld_ISAKMP::common_member()
{
	nextType_member(
		new MmUint(
			"NextPayload",
			8,
			GENEHC(this, McPld_ISAKMP, NextPayload),
			EVALHC(this, McPld_ISAKMP, NextPayload)
		)
	);

	member(new MmUint("Reserved1",	8, UN(0), UN(0)));

	Layerlength_member(
		new MmUint(
			"PayloadLength",
			16,
			GENEHC(this, McPld_ISAKMP, PayloadLength),
			EVALANY()
		)
	);

	return;
}



McPld_ISAKMP_ANY *
McPld_ISAKMP_ANY::create(CSTR key)
{
	McPld_ISAKMP_ANY *mc = new McPld_ISAKMP_ANY(key);

	mc->common_member();
	mc->member(new MmVarOctets("data", 0, EVALZERO()));

	MmPayload_onISAKMP::add_other(mc);
	MmISAKMP_PonSA::add_other(mc);
	MmISAKMP_TonP::add_other(mc);

	return(mc);
}



////////////////////////////////////////
// Security Association Payload       //
////////////////////////////////////////
void
McPld_ISAKMP_SA::common_member()
{
	McPld_ISAKMP::common_member();
	DOImember(
		new MmUint(
			"DOI",
			32,
			GENEHC(this, McPld_ISAKMP_SA, DOI),
			EVALHC(this, McPld_ISAKMP_SA, DOI)
		)
	);

	return;
}

McPld_ISAKMP_SA_ANY *
McPld_ISAKMP_SA_ANY::create(CSTR key)
{
	McPld_ISAKMP_SA_ANY *mc = new McPld_ISAKMP_SA_ANY(key);

	mc->common_member();
	mc->member(new MmVarOctets("Situation", 0, EVALZERO()));

	MmDOIonSA::add_other(mc);

	return(mc);
}

void
McPld_ISAKMP_SA_IPsec::common_member()
{
	McPld_ISAKMP_SA::common_member();
	SITmember(
		new MmUint(
			"Situation",
			32,
			GENEHC(this, McPld_ISAKMP_SA_IPsec, Situation),
			EVALHC(this, McPld_ISAKMP_SA_IPsec, Situation)
		)
	);
}

McPld_ISAKMP_SA_IPsec_ANY *
McPld_ISAKMP_SA_IPsec_ANY::create(CSTR key)
{
	McPld_ISAKMP_SA_IPsec_ANY *mc = new McPld_ISAKMP_SA_IPsec_ANY(key);

	mc->common_member();
	mc->member(new MmVarOctets("data", 0, EVALZERO()));

	MmSIT_onSA_IPsec::add_other(mc);

	return(mc);
}

McPld_ISAKMP_SA_IPsec_IDonly *
McPld_ISAKMP_SA_IPsec_IDonly::create(CSTR key)
{
	McPld_ISAKMP_SA_IPsec_IDonly *mc
		= new McPld_ISAKMP_SA_IPsec_IDonly(key);

	mc->common_member();

	mc->member(new MmISAKMP_PonSA("proposal"));

	MmSIT_onSA_IPsec::add(mc);

	return(mc);
}



////////////////////////////////
// Proposal Payload           //
////////////////////////////////
void
McPld_ISAKMP_P::common_member()
{
	McPld_ISAKMP::common_member();

	member(new MmUint("ProposalNumber",	8, UN(0), UN(0)));

	ProtocolIDmember(
		new MmUint(
			"ProtocolID",
			8,
			GENEHC(this, McPld_ISAKMP_P, ProtocolID),
			EVALHC(this, McPld_ISAKMP_P, ProtocolID)
		)
	);

	return;
}

void
McPld_ISAKMP_P::spisize_member()
{
	SPIsize_member(
		new MmUint(
			"SPIsize",
			8,
			GENEHC(this, McPld_ISAKMP_P, SPIsize),
			EVALANY()
		)
	);

	return;
}

void
McPld_ISAKMP_P::transnum_member()
{
	member(
		new MmUint(
			"NumOfTransforms",
			8,
			GENEHC(this, McPld_ISAKMP_P, NumOfTransforms),
			EVALHC(this, McPld_ISAKMP_P, NumOfTransforms)
		)
	);

	return;
}

McPld_ISAKMP_P *
McPld_ISAKMP_P::create(CSTR key)
{
	McPld_ISAKMP_P *mc = new McPld_ISAKMP_P(key);

	mc->common_member();
	mc->spisize_member();
	mc->transnum_member();
	mc->SPImember(new MmISAKMP_SPI("SPI", 0, EVALZERO()));

	mc->member(new MmISAKMP_TonP(mc, "transform"));

	MmProtocolIDonP::add_other(mc);

	return(mc);
}

McPld_ISAKMP_P_ISAKMP *
McPld_ISAKMP_P_ISAKMP::create(CSTR key)
{
	McPld_ISAKMP_P_ISAKMP *mc = new McPld_ISAKMP_P_ISAKMP(key);

	mc->common_member();
	mc->spisize_member();
	mc->transnum_member();
	mc->SPImember(new MmISAKMP_SPI("SPI", 0, EVALZERO()));

	mc->member(new MmISAKMP_TonP(mc, "transform"));

	MmProtocolIDonP::add(mc);

	return(mc);
}

McPld_ISAKMP_P_IPsec_AH *
McPld_ISAKMP_P_IPsec_AH::create(CSTR key)
{
	McPld_ISAKMP_P_IPsec_AH *mc = new McPld_ISAKMP_P_IPsec_AH(key);

	mc->common_member();
	mc->member(new MmUint("SPIsize", 8, UN(4), UN(4)));
	mc->transnum_member();
	mc->member(new MmUint("SPI", 32, UN(0), UN(0)));

	mc->member(new MmISAKMP_TonP(mc, "transform"));

	MmProtocolIDonP::add(mc);

	return(mc);
}

McPld_ISAKMP_P_IPsec_ESP *
McPld_ISAKMP_P_IPsec_ESP::create(CSTR key)
{
	McPld_ISAKMP_P_IPsec_ESP *mc = new McPld_ISAKMP_P_IPsec_ESP(key);

	mc->common_member();
	mc->member(new MmUint("SPIsize", 8, UN(4), UN(4)));
	mc->transnum_member();
	mc->member(new MmUint("SPI", 32, UN(0), UN(0)));

	mc->member(new MmISAKMP_TonP(mc, "transform"));

	MmProtocolIDonP::add(mc);

	return(mc);
}

McPld_ISAKMP_P_IPCOMP *
McPld_ISAKMP_P_IPCOMP::create(CSTR key)
{
	McPld_ISAKMP_P_IPCOMP *mc = new McPld_ISAKMP_P_IPCOMP(key);

	mc->common_member();
	mc->spisize_member();
	mc->transnum_member();
	mc->SPImember(new MmISAKMP_SPI("SPI", 0, EVALZERO()));

	mc->member(new MmISAKMP_TonP(mc, "transform"));

	MmProtocolIDonP::add(mc);

	return(mc);
}



////////////////////////////////
// Transform Payload          //
////////////////////////////////
McPld_ISAKMP_T *
McPld_ISAKMP_T::create(CSTR key)
{
	McPld_ISAKMP_T *mc = new McPld_ISAKMP_T(key);

	mc->common_member();
	mc->member(new MmUint("TransformNumber",  8, UN(0), UN(0)));
	mc->member(new MmUint("TransformID",      8, UN(0), UN(0)));
	mc->member(new MmUint("Reserved2",       16, UN(0), UN(0)));

	mc->member(new MmAttr_onT("attribute"));

	MmPayload_onISAKMP::add(mc);
	MmISAKMP_PonSA::add(mc);
	MmISAKMP_TonP::add(mc);

	return(mc);
}



////////////////////////////////
// Key Exchange Payload       //
////////////////////////////////
McPld_ISAKMP_KE *
McPld_ISAKMP_KE::create(CSTR key)
{
	McPld_ISAKMP_KE *mc = new McPld_ISAKMP_KE(key);

	mc->common_member();
	mc->member(new MmVarOctets("KeyExchangeData", 0, EVALZERO()));

	MmPayload_onISAKMP::add(mc);
	MmISAKMP_PonSA::add(mc);
	MmISAKMP_TonP::add(mc);

	return(mc);
}



////////////////////////////////
// Identification Payload     //
////////////////////////////////
void
McPld_ISAKMP_ID::common_member()
{
	McPld_ISAKMP::common_member();

	IDtype_member(
		new MmUint(
			"IDtype",
			8,
			GENEHC(this, McPld_ISAKMP_ID, IDtype),
			EVALHC(this, McPld_ISAKMP_ID, IDtype)
		)
	);
#if 0
	member(new MmUint("DOIspecificIDdata", 24, UN(0), UN(0)));
#else
	member(new MmUint("ProtocolID",	8, UN(0), UN(0)));
	member(new MmUint("Port",	16, UN(0), UN(0)));
#endif
	return;
}

McPld_ISAKMP_ID *
McPld_ISAKMP_ID::create(CSTR key)
{
	McPld_ISAKMP_ID *mc = new McPld_ISAKMP_ID(key);

	mc->common_member();
	mc->member(new MmVarOctets("ID", 0, EVALZERO()));

	MmIDtype_onID::add_other(mc);

	return(mc);
}

McPld_ISAKMP_ID_IPV4_ADDR *
McPld_ISAKMP_ID_IPV4_ADDR::create(CSTR key)
{
	McPld_ISAKMP_ID_IPV4_ADDR *mc = new McPld_ISAKMP_ID_IPV4_ADDR(key);

	mc->common_member();
	mc->member(new MmV4Addr("ID", MUST(), MUST()));

	MmIDtype_onID::add(mc);

	return(mc);
}

McPld_ISAKMP_ID_FQDN *
McPld_ISAKMP_ID_FQDN::create(CSTR key)
{
	McPld_ISAKMP_ID_FQDN *mc = new McPld_ISAKMP_ID_FQDN(key);

	mc->common_member();
	mc->member(new MmAsciiString("ID", 0, EVALZERO()));

	MmIDtype_onID::add(mc);

	return(mc);
}

McPld_ISAKMP_ID_USER_FQDN *
McPld_ISAKMP_ID_USER_FQDN::create(CSTR key)
{
	McPld_ISAKMP_ID_USER_FQDN *mc = new McPld_ISAKMP_ID_USER_FQDN(key);

	mc->common_member();
	mc->member(new MmAsciiString("ID", 0, EVALZERO()));

	MmIDtype_onID::add(mc);

	return(mc);
}

McPld_ISAKMP_ID_IPV4_ADDR_SUBNET *
McPld_ISAKMP_ID_IPV4_ADDR_SUBNET::create(CSTR key)
{
	McPld_ISAKMP_ID_IPV4_ADDR_SUBNET *mc =
		new McPld_ISAKMP_ID_IPV4_ADDR_SUBNET(key);

	mc->common_member();
	mc->member(new MmV4Addr("ID1", MUST(), MUST()));
	mc->member(new MmV4Addr("ID2", MUST(), MUST()));

	MmIDtype_onID::add(mc);

	return(mc);
}

McPld_ISAKMP_ID_IPV6_ADDR *
McPld_ISAKMP_ID_IPV6_ADDR::create(CSTR key)
{
	McPld_ISAKMP_ID_IPV6_ADDR *mc = new McPld_ISAKMP_ID_IPV6_ADDR(key);

	mc->common_member();
	mc->member(new MmV6Addr("ID", MUST(), MUST()));

	MmIDtype_onID::add(mc);

	return(mc);
}

McPld_ISAKMP_ID_IPV6_ADDR_SUBNET *
McPld_ISAKMP_ID_IPV6_ADDR_SUBNET::create(CSTR key)
{
	McPld_ISAKMP_ID_IPV6_ADDR_SUBNET *mc =
		new McPld_ISAKMP_ID_IPV6_ADDR_SUBNET(key);

	mc->common_member();
	mc->member(new MmV6Addr("ID1", MUST(), MUST()));
	mc->member(new MmV6Addr("ID2", MUST(), MUST()));

	MmIDtype_onID::add(mc);

	return(mc);
}

McPld_ISAKMP_ID_IPV4_ADDR_RANGE *
McPld_ISAKMP_ID_IPV4_ADDR_RANGE::create(CSTR key)
{
	McPld_ISAKMP_ID_IPV4_ADDR_RANGE *mc =
		new McPld_ISAKMP_ID_IPV4_ADDR_RANGE(key);

	mc->common_member();
	mc->member(new MmV4Addr("ID1", MUST(), MUST()));
	mc->member(new MmV4Addr("ID2", MUST(), MUST()));

	MmIDtype_onID::add(mc);

	return(mc);
}

McPld_ISAKMP_ID_IPV6_ADDR_RANGE *
McPld_ISAKMP_ID_IPV6_ADDR_RANGE::create(CSTR key)
{
	McPld_ISAKMP_ID_IPV6_ADDR_RANGE *mc =
		new McPld_ISAKMP_ID_IPV6_ADDR_RANGE(key);

	mc->common_member();
	mc->member(new MmV6Addr("ID1", MUST(), MUST()));
	mc->member(new MmV6Addr("ID2", MUST(), MUST()));

	MmIDtype_onID::add(mc);

	return(mc);
}

McPld_ISAKMP_ID_DER_ASN1_DN *
McPld_ISAKMP_ID_DER_ASN1_DN::create(CSTR key)
{
	McPld_ISAKMP_ID_DER_ASN1_DN *mc =
		new McPld_ISAKMP_ID_DER_ASN1_DN(key);

	mc->common_member();
	mc->member(new MmVarOctets("ID", 0, EVALZERO()));

	MmIDtype_onID::add(mc);

	return(mc);
}

McPld_ISAKMP_ID_DER_ASN1_GN *
McPld_ISAKMP_ID_DER_ASN1_GN::create(CSTR key)
{
	McPld_ISAKMP_ID_DER_ASN1_GN *mc = new McPld_ISAKMP_ID_DER_ASN1_GN(key);

	mc->common_member();
	mc->member(new MmVarOctets("ID", 0, EVALZERO()));

	MmIDtype_onID::add(mc);

	return(mc);
}

McPld_ISAKMP_ID_KEY_ID *
McPld_ISAKMP_ID_KEY_ID::create(CSTR key)
{
	McPld_ISAKMP_ID_KEY_ID *mc = new McPld_ISAKMP_ID_KEY_ID(key);

	mc->common_member();
	mc->member(new MmVarOctets("ID", 0, EVALZERO()));

	MmIDtype_onID::add(mc);

	return(mc);
}



////////////////////////////////
// Certificate Payload        //
////////////////////////////////
McPld_ISAKMP_CERT *
McPld_ISAKMP_CERT::create(CSTR key)
{
	McPld_ISAKMP_CERT *mc = new McPld_ISAKMP_CERT(key);

	mc->common_member();
	mc->member(new MmUint("CertEncoding", 8, UN(0), UN(0)));
	mc->member(new MmVarOctets("CertData", 0, EVALZERO()));

	MmPayload_onISAKMP::add(mc);
	MmISAKMP_PonSA::add(mc);
	MmISAKMP_TonP::add(mc);

	return(mc);
}



////////////////////////////////////////
// Certificate Request Payload        //
////////////////////////////////////////
McPld_ISAKMP_CR *
McPld_ISAKMP_CR::create(CSTR key)
{
	McPld_ISAKMP_CR *mc = new McPld_ISAKMP_CR(key);

	mc->common_member();
	mc->member(new MmUint("CertType", 8, UN(0), UN(0)));
	mc->member(new MmVarOctets("CertAuthority", 0, EVALZERO()));

	MmPayload_onISAKMP::add(mc);
	MmISAKMP_PonSA::add(mc);
	MmISAKMP_TonP::add(mc);

	return(mc);
}



////////////////////////////////
// Hash Payload               //
////////////////////////////////
McPld_ISAKMP_HASH *
McPld_ISAKMP_HASH::create(CSTR key)
{
	McPld_ISAKMP_HASH *mc = new McPld_ISAKMP_HASH(key);

	mc->common_member();
	mc->member(new MmP2_HASH_2("HashData", 0, EVALZERO()));

	MmPayload_onISAKMP::add(mc);
	MmISAKMP_PonSA::add(mc);
	MmISAKMP_TonP::add(mc);

	return(mc);
}



////////////////////////////////
// Signature Payload          //
////////////////////////////////
McPld_ISAKMP_SIG *
McPld_ISAKMP_SIG::create(CSTR key)
{
	McPld_ISAKMP_SIG *mc = new McPld_ISAKMP_SIG(key);

	mc->common_member();
	mc->member(new MmVarOctets("SignatureData", 0, EVALZERO()));

	MmPayload_onISAKMP::add(mc);
	MmISAKMP_PonSA::add(mc);
	MmISAKMP_TonP::add(mc);

	return(mc);
}



////////////////////////////////
// Nonce Payload              //
////////////////////////////////
McPld_ISAKMP_NONCE *
McPld_ISAKMP_NONCE::create(CSTR key)
{
	McPld_ISAKMP_NONCE *mc = new McPld_ISAKMP_NONCE(key);

	mc->common_member();
	mc->member(new MmVarOctets("NonceData", 0, EVALZERO()));

	MmPayload_onISAKMP::add(mc);
	MmISAKMP_PonSA::add(mc);
	MmISAKMP_TonP::add(mc);

	return(mc);
}



////////////////////////////////
// Notification Payload       //
////////////////////////////////
void
McPld_ISAKMP_N::common_member()
{
	McPld_ISAKMP::common_member();
	DOImember(
		new MmUint(
			"DOI",
			32,
			GENEHC(this, McPld_ISAKMP_N, DOI),
			EVALHC(this, McPld_ISAKMP_N, DOI)
		)
	);
	member(new MmUint("ProtocolID", 8, UN(0), UN(0)));

	SPIsize_member(
		new MmUint(
			"SPIsize",
			8,
			GENEHC(this, McPld_ISAKMP_N, SPIsize),
			EVALANY()
		)
	);

	NotifyMessageType_member(
		new MmUint(
			"NotifyMessageType",
			16,
			GENEHC(this, McPld_ISAKMP_N, NotifyMessageType),
			EVALHC(this, McPld_ISAKMP_N, NotifyMessageType)
		)
	);

	return;
}



McPld_ISAKMP_N_ANY *
McPld_ISAKMP_N_ANY::create(CSTR key)
{
	McPld_ISAKMP_N_ANY *mc = new McPld_ISAKMP_N_ANY(key);

	mc->common_member();
	mc->SPImember(new MmISAKMP_SPI("SPI", 0, EVALZERO()));

	mc->member(new MmVarOctets("NotificationData", 0, EVALZERO()));

	MmDOIonN::add_other(mc);

	return(mc);
}



McPld_ISAKMP_N_ISAKMP *
McPld_ISAKMP_N_ISAKMP::create(CSTR key)
{
	McPld_ISAKMP_N_ISAKMP *mc = new McPld_ISAKMP_N_ISAKMP(key);

	mc->common_member();
	mc->SPImember(new MmISAKMP_SPI("SPI", 0, EVALZERO()));

	mc->member(new MmVarOctets("NotificationData", 0, EVALZERO()));

	MmDOIonN::add(mc);

	return(mc);
}



McPld_ISAKMP_N_IPsec_ANY *
McPld_ISAKMP_N_IPsec_ANY::create(CSTR key)
{
	McPld_ISAKMP_N_IPsec_ANY *mc = new McPld_ISAKMP_N_IPsec_ANY(key);

	mc->common_member();
	mc->SPImember(new MmISAKMP_SPI("SPI", 0, EVALZERO()));

	mc->member(new MmVarOctets("NotificationData", 0, EVALZERO()));

	MmType_onN_IPsec::add_other(mc);

	return(mc);
}



McPld_ISAKMP_N_IPsec_ResponderLT *
McPld_ISAKMP_N_IPsec_ResponderLT::create(CSTR key)
{
	McPld_ISAKMP_N_IPsec_ResponderLT *mc =
		new McPld_ISAKMP_N_IPsec_ResponderLT(key);

	mc->common_member();
	mc->SPImember(new MmISAKMP_SPI("SPI", 0, EVALZERO()));

	mc->member(new MmAttr_onT("attribute"));

	MmType_onN_IPsec::add(mc);

	return(mc);
}



McPld_ISAKMP_N_IPsec_ReplayStat *
McPld_ISAKMP_N_IPsec_ReplayStat::create(CSTR key)
{
	McPld_ISAKMP_N_IPsec_ReplayStat *mc =
		new McPld_ISAKMP_N_IPsec_ReplayStat(key);

	mc->common_member();
	mc->SPImember(new MmISAKMP_SPI_ReplayStat("SPI", 0, EVALZERO()));

	mc->member(new MmUint("NotificationData", 32, UN(0), UN(0)));

	MmType_onN_IPsec::add(mc);

	return(mc);
}



////////////////////////////////
// Delete Payload             //
////////////////////////////////
McPld_ISAKMP_D *
McPld_ISAKMP_D::create(CSTR key)
{
	McPld_ISAKMP_D *mc = new McPld_ISAKMP_D(key);

	mc->common_member();

	mc->member(new MmUint("DOI",               32, UN(0), UN(0)));
	mc->member(new MmUint("ProtocolID",         8, UN(0), UN(0)));

	mc->SPIsize_member(
		new MmSPIsize(
			mc,
			"SPIsize",
			8,
			GENEHC(mc, McPld_ISAKMP_D, SPIsize),
			EVALANY()
		)
	);

	mc->NumberOfSPI_member(
		new MmNumberOfSPI(
			mc,
			"NumberOfSPI",
			16,
			GENEHC(mc, McPld_ISAKMP_D, NumberOfSPI),
			EVALANY()
		)
	);

	mc->member(
		new MmMultiple(
			new MmSPI_onD(mc, "SPI", 0, EVALZERO()),
			(METH_HC_MLC)&McPld_ISAKMP_D::HC_MLC(SPI)
		)
	);

	MmPayload_onISAKMP::add(mc);
	MmISAKMP_PonSA::add(mc);
	MmISAKMP_TonP::add(mc);

	return(mc);
}



////////////////////////////////
// Vendor ID Payload          //
////////////////////////////////
McPld_ISAKMP_VID *
McPld_ISAKMP_VID::create(CSTR key)
{
	McPld_ISAKMP_VID *mc = new McPld_ISAKMP_VID(key);

	mc->common_member();
	mc->member(new MmVarOctets("VID", 0, EVALZERO()));

	MmPayload_onISAKMP::add(mc);
	MmISAKMP_PonSA::add(mc);
	MmISAKMP_TonP::add(mc);

	return(mc);
}



////////////////////////////////
// Data Attributes            //
////////////////////////////////
void
McAttr_ISAKMP::common_member()
{
	Attr_member(
		new MmUint(
			"AF",
			1,
			GENEHC(this, McAttr_ISAKMP, AF),
			EVALHC(this, McAttr_ISAKMP, AF)
		)
	);

	member(new MmUint("Type", 15, UN(0), UN(0)));
}



McAttr_ISAKMP *
McAttr_ISAKMP::create(CSTR key)
{
	McAttr_ISAKMP *mc = new McAttr_ISAKMP(key);

	mc->common_member();

	mc->member(new MmVarOctets("data", 0, EVALZERO()));

	MmAttr_onT::add_other(mc);

	return(mc);
}



McAttr_ISAKMP_TLV *
McAttr_ISAKMP_TLV::create(CSTR key)
{
	McAttr_ISAKMP_TLV *mc = new McAttr_ISAKMP_TLV(key);

	mc->common_member();

	mc->Length_member(
		new MmUint(
			"Length",
			16,
			GENEHC(mc, McAttr_ISAKMP_TLV, Length),
			EVALANY()
		)
	);

	mc->member(new MmVarOctets("Value", 0, EVALZERO()));

	MmAttr_onT::add(mc);

	return(mc);
}



McAttr_ISAKMP_TV *
McAttr_ISAKMP_TV::create(CSTR key)
{
	McAttr_ISAKMP_TV *mc = new McAttr_ISAKMP_TV(key);

	mc->common_member();

	mc->member(new MmUint("Value", 16, UN(0), UN(0)));

	MmAttr_onT::add(mc);

	return(mc);
}

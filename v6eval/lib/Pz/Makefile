LIB=	Pz
SRCS=	DmObject.cc ItPosition.cc McAH.cc McARP.cc McAlgorithm.cc	\
	McDHCPv6.cc McDNS.cc McESP.cc McEther.cc McICMPv4.cc		\
	McICMPv6.cc McIGMP.cc McIKE.cc McIPv4.cc McIPv6.cc McInit.cc	\
	McMLDv2.cc McMobility.cc McNoNext.cc McNull.cc McObject.cc	\
	McRIPng.cc McRR.cc McSIP.cc McSNMP.cc McSub.cc McTCP.cc		\
	McUDP.cc McVRRP.cc MfAlgorithm.cc MmChecksum.cc MmData.cc	\
	MmHeader.cc MmObject.cc MvArgCheck.cc MvFunction.cc		\
	PAlgorithm.cc PControl.cc PObject.cc PcObject.cc PlObject.cc	\
	PrItem.cc PvAction.cc PvAutoItem.cc PvIfName.cc PvName.cc	\
	PvObject.cc PvOctets.cc RObject.cc WObject.cc			\
	rijndael-alg-fst.c rijndael-api-fst.c				\
	camellia.c camellia-api.c

CXXFLAGS+=	-O0 -I$(Cm) -I.
CFLAGS+=	-I$(Cm) -I.

#CXXFLAGS=	-pipe -g -Wall -I./ -I$(Cm)
#CFLAGS=	-pipe -g -Wall -I./ -I$(Cm) -I/usr/local/include

.PATH: ${.CURDIR}/crypto/rijndael
.PATH: ${.CURDIR}/crypto/camellia

# <draft-ietf-nemo-basic-support-03.txt>, NEMO Basic Support
# CXXFLAGS+=	-DTBD_OPT_MH_MOB_NETWORK_PREF=6

# <draft-ietf-ipv6-router-selection-05.txt>,
#     Default Router Preferences and More-Specific Routes
# CXXFLAGS+=	-DTBD_OPT_ROUTE_INFO=200

# <draft-ietf-dhc-dhcpv6-opt-timeconfig-03.txt>,
#     Time Configuration Options for DHCPv6
# CXXFLAGS+=	-DTBD_OPT_NTP_SERVERS=27
# CXXFLAGS+=	-DTBD_OPT_TIME_ZONE=28

# RFC 1157,  SNMP
# RFC 1905,  Protocol Operations for SNMPv2
# CXXFLAGS+=   -DNOT_USE_SNMP

# RFC 3376, IGMPv3
# CXXFLAGS+=	-DNOT_USE_IGMPV3_QUERY

# RFC 3775, Mobility Support in IPv6
# CXXFLAGS+=	-DRR_DBG

# RFC 3315, DHCP for IPv6
# CXXFLAGS+=	-DDHCP_DBG

# RFC 3768, VRRP
# draft-ietf-vrrp-ipv6-spec-06.txt, VRRP for IPv6
# CXXFLAGS+=	-DVRRP_DBG

# RFC 2408, ISAKMP
# CXXFLAGS+=	-DISAKMP_DBG

# RFC 3566, AES-XCBC-MAC-96 Algorithm
# CXXFLAGS+=	-DAESXCBC_DBG

# RFC 3686, Using AES Counter Mode With IPsec ESP
# CXXFLAGS+=	-DAESCTR_DBG

# RFC 4303, IP Encapsulating Security Payload (ESP)
# 2.2.1. Extended (64-bit) Sequence Number
# CXXFLAGS+=	-DESN_DBG

.include "bsd.lib.mk"

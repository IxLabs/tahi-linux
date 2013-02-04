		    README of IPv6 Conformance Test Tool

				TAHI Project

			$Date: 2009/11/25 07:22:17 $


Project Overview
================

TAHI Project is the joint effort formed with the objective of
developing and providing the verification technology for IPv6. 

The growth process of IPv4 was the history of encountering various kinds of
obstacles and conquering such obstacles. However, once the position as
infrastructure was established, it is not allowed to repeat the same history.
This is a reason why the verification technology is essential for IPv6
deployment.

	We research and develop conformance tests and interoperability tests
	for IPv6.

	We closely work with the KAME project and USAGI project.
	We help activities of them in the quality side by offering
	the verification technology we develop in the TAHI project and improve
	the development efficiency. 

	We open the results and fruits of the project to the public for FREE.
	Any developer concerned with IPv6 can utilize the results and fruits
	of TAHI project freely. A free software plays an important role in
	progress of the	Internet. We believe that providing the verification
	technology for FREE contributes to advances of IPv6. Besides the
	programs, the specifications and criteria of verification will be
	included in the Package.


Organizations
=============

TAHI Project started at October 1, 1998,
then the project was formed by following three organizations
(in alphabetical order):

	The University of Tokyo 
	YDC Corp.
	Yokogawa Electric Corp. 

Now the project is formed by following two organizations.

	The University of Tokyo 
	Yokogawa Electric Corp. 


Introduction of IPv6 Conformance Test Tool
==========================================
The IPv6 Conformance Test Tool is a tool package to execute conformance
tests for IPv6.

That is, with the Tool, you can
	- define any packet to be sent.
	- define any packet to be captured.
	- program the above actions in any order by perl scripts.

TAHI project also offers you another package for IPv6 conformance test
programs that are developed based on the Tool. For more information,
please refer to 00README.ct.

Both packages "ct" and "v6eval" may be obtained at our site:

	http://www.tahi.org/


Copyright
=========

Please refer to the COPYRIGHT file for copyright notice.


Install and Run 
===============

The Tool is tested on 6.3-RELEASE or higher.
Complete installation instructions can be found in the "INSTALL.v6eval"

Full installation instructions can be found in the "INSTALL.v6eval".


Files
=====

bin/
	internal command sets.

etc/
	sample configuration files.

include/
	macro sets for packet definition.

lib/
	internal libraries.

sample/
	sample test scripts & packet definition files. 

sample_stddef/
	sample test scripts & packet definition files that uses macro set
	contained in "include/" directory.

script/
	Perl modules for test scripts.


Support Header and Option
=========================

The Tool can generate and check the following headers and options:

	IPv6
		IPv6 Header
		Hop-by-Hop Options Header
		Destination Options Header
		Routing Header
		Fragment Header
		Authentication Header
		Encapsulating Security Payload Header

		Hop-by-Hop/Destination Options
			Pad1 Option
			PadN Option
			Tunnel Encapsulation Header Option
			Router Alert Option
			Jumbo Payload Option Header
			Home Address Option

		Mobility Header
			Binding Refresh Request
			Home Test Init
			Care-of Test Init
			Home Test
			Care-of Test
			Binding Update
			Binding Acknowledgement
			Binding Error

		Mobility Options
			Pad1
			PadN
			Binding Refresh Advice
			Alternate Care-of Address
			Nonce Indices
			Binding Authorization Data
			Mobile Network Prefix

	IPv4
		IPv4 Header
		Option
		Authentication Header
		Encapsulating Security Payload Header

	ARP

	RARP

	ICMPv6
		Destination Unreachable
		Packet Too Big
		Time Exceeded Message
		Parameter Problem Message
		Echo Request
		Echo Reply
		Version 1 Multicast Listener Query
		Version 1 Multicast Listener Report
		Version 1 Multicast Listener Done
		Version 2 Multicast Listener Query
		Version 2 Multicast Listener Report
		Router Solicitation Header
		Router Advertisement Header
		Neighbor Solicitation Header
		Neighbor Advertisement Header
		Redirect
		Router Renumbering
		Home Agent Address Discovery Request
		Home Agent Address Discovery Reply
		Mobile Prefix Solicitation
		Mobile Prefix Advertisement

		ICMPv6 Options
			Source Link Layer Address Option
			Target Link Layer Address Option
			Prefix Information Option
			Redirected Header Option
			MTU Option
			Advertisement Interval Option
			Home Agent Information Option
			Route Information Option

	ICMPv4
		Echo Reply
		Destination Unreachable
		Redirect
		Echo Request
		Time Exceeded
		Parameter Problem

	IGMP
		IGMPv1 Query
			- You need to set NOT_USE_IGMPV3_QUERY
			  in lib/Pz/Makefile
			  IGMPv1/v2 Query is incompatible with IGMPv3 Query.
		IGMPv1 Report
		IGMPv2 Query
			- You need to set NOT_USE_IGMPV3_QUERY
			  in lib/Pz/Makefile
			  IGMPv1/v2 Query is incompatible with IGMPv3 Query.
		IGMPv2 Report
		IGMPv2 Leave Group
		IGMPv3 Query
		IGMPv3 Report

	UDP
		UDP Header

		DNS
			RRs
				A
				NS
				MD
				MF
				CNAME
				SOA
				MB
				MG
				MR
				NULL
				WKS
				PTR
				HINFO
				MINFO
				MX
				TXT
				RP
				AFSDB
				X25
				ISDN
				RT
				PX
				AAAA
				SRV
				NAPTR
		SNMP
		ISAKMP
		RIPng

		DHCPv6
			SOLICIT
			ADVERTISE
			REQUEST
			CONFIRM
			RENEW
			REBIND
			REPLY
			RELEASE
			DECLINE
			RECONFIGURE
			INFORMATION-REQUEST
			RELAY-FORW
			RELAY-REPL

			DHCP options
				OPTION_CLIENTID
				OPTION_SERVERID
				OPTION_IA_NA
				OPTION_IA_TA
				OPTION_IAADDR
				OPTION_ORO
				OPTION_PREFERENCE
				OPTION_ELAPSED_TIME
				OPTION_RELAY_MSG
				OPTION_AUTH
				OPTION_UNICAST
				OPTION_STATUS_CODE
				OPTION_RAPID_COMMIT
				OPTION_USER_CLASS
				OPTION_VENDOR_CLASS
				OPTION_VENDOR_OPTS
				OPTION_INTERFACE_ID
				OPTION_RECONF_MSG
				OPTION_RECONF_ACCEPT
				OPTION_DNS_SERVERS
				OPTION_DOMAIN_LIST
				OPTION_NTP_SERVERS
				OPTION_TIME_ZONE
				OPTION_IA_PD
				OPTION_IAPREFIX

		SIP

	TCP
		TCP Header
		Option


Contact Points
==============

For more information about TAHI project, please visit our site:
	http://www.tahi.org/

To contact us, please e-mail to:
	contact@tahi.org


Acknowledgments
===============

We deeply thank the following projects for their great support to us:
	KAME Project	(http://www.kame.net/)
	USAGI Project	(http://www.linux-ipv6.org/)
	WIDE Project	(http://www.wide.ad.jp/)

The tests have ran for the following implementations.
We also thank people who has given us the opportunities.

	all of participants and organizers in

		(in alphabetical order):

		- Connectathon
		- ETSI plugtests
		- TAHI IPv6 Interoperability Test Event

NOTE: If your want to run the test for the above implementations,
      you must configure them to fit testing environment.
							[end of README]

CXXFLAGS+=	-I${Cm} -I${Pz} -I${pkt} -I/usr/local/include
LDFLAGS +=	-L${Cm} -L${Pz} -L${pkt} -L/usr/local/lib
LDADD+=	-lPkt -lPz -lCm -lcrypto
SRCS=	pktbuf.cc 
PROG_CXX=	pktbuf
PROG=	${PROG_CXX}
NOMAN=
NO_MAN=
CLEANFILES+=	.depend

.include "../Makefile.inc"
.include "bsd.prog.mk"

$(PROG): ${Cm}/libCm.a ${Pz}/libPz.a ${pkt}/libPkt.a

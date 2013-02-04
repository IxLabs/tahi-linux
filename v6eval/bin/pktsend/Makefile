CXXFLAGS+=	-I${Cm} -I${Pz} -I${pkt} -I/usr/local/include
LDFLAGS +=	-L${Cm} -L${Pz} -L${pkt} -L/usr/local/lib
LDADD+=	-lPkt -lPz -lCm  -lcrypto
SRCS=	pktsend.cc 
PROG_CXX=	pktsend
PROG=	${PROG_CXX}
CLEANFILES+=	.depend

.include "../Makefile.inc"
.include "bsd.prog.mk"

$(PROG): ${Cm}/libCm.a ${Pz}/libPz.a ${pkt}/libPkt.a

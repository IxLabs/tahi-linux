LIB=	Pkt
YSRCS=	PzParse.y
SRCS=	$(YSRCS:.y=.cc) LxToken.cc LxLexer.cc RAFControl.cc \
	PktAgent.cc PktRecv.cc PktClient.cc PktServer.cc StdAgent.cc \
	BpfAgent.cc Bpfilter.cc Ringbuf.cc RunEnv.cc PktCtlClient.cc \
	PktSndClient.cc PktRcvClient.cc PktBuf.cc PktQueue.cc bufStat.cc

CXXFLAGS+=	-I. -I$(Cm) -I$(Pz) -DHAVE_BPF_HDRCMPLT -I/usr/include
# CXXFLAGS+=	-DVCLEAR_DBG

CLEANFILES+=	PzParse.h
CLEANFILES+=	.depend

.include "bsd.lib.mk"

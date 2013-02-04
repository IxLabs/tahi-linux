Makefile=	Makefile.tmp

all: ${Makefile}
	${MAKE} -f ${Makefile}

${Makefile}: Makefile.PL
	perl Makefile.PL
	perl -pi \
		-e 's/ doc_(perl|site|\$$\(INSTALLDIRS\))_install$$//' \
		${Makefile}

clean: ${Makefile}
	${MAKE} -f ${Makefile} realclean

install: ${Makefile}
	${MAKE} -f ${Makefile} install

depend:

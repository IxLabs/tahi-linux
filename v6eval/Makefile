SUBDIR=lib bin etc script include
PREFIX= /usr/local/v6eval
DOCFILES=00README INSTALL CHANGELOG \
	 00README.v6eval INSTALL.v6eval CHANGELOG.v6eval \
	 COPYRIGHT

DIRS=   $(PREFIX) $(PREFIX)/bin $(PREFIX)/man $(PREFIX)/doc
#SECTIONS=       1 2 3 4 5 6 7 8 9 n
SECTIONS=       1
.MAIN: depend all
beforeinstall:
	for i in $(DIRS); do \
		[ -d $$i ] || mkdir $$i; \
	done
#	for i in $(SECTIONS); do \
#		[ -d $(PREFIX)/man/man$$i ] || mkdir $(PREFIX)/man/man$$i; \
#		[ -d $(PREFIX)/man/cat$$i ] || mkdir $(PREFIX)/man/cat$$i; \
#	done
	for i in $(SECTIONS); do \
		[ -d $(PREFIX)/man/man$$i ] || mkdir $(PREFIX)/man/man$$i; \
	done
	install -c -o bin -g bin -m 444 $(DOCFILES) $(PREFIX)/doc

.include <bsd.subdir.mk>

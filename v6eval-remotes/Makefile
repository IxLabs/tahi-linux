SUBDIR=bin script
PREFIX= /usr/local/v6eval
DOCFILES=00README.remotes COPYRIGHT INSTALL.remotes

DIRS=   $(PREFIX) $(PREFIX)/bin $(PREFIX)/man $(PREFIX)/doc
#SECTIONS=       1 2 3 4 5 6 7 8 9 n
SECTIONS=       1
.MAIN: depend all
beforeinstall:
	for i in $(DIRS); do \
		[ -d $$i ] || mkdir $$i; \
	done
	for i in $(SECTIONS); do \
		[ -d $(PREFIX)/man/man$$i ] || mkdir $(PREFIX)/man/man$$i; \
	done
	install -c -o bin -g bin -m 444 $(DOCFILES) $(PREFIX)/doc

.include <bsd.subdir.mk>

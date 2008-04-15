PREFIX=/usr

BIN=$(PREFIX)/bin
DESTDIR=$(PREFIX)/share/juffed
PIXMAPS=$(PREFIX)/share/pixmaps
APPLICATIONS=$(PREFIX)/share/applications

QMAKE=qmake
MAKE=make
LRELEASE=lrelease

src/juffed:
	cd src && $(QMAKE) && $(MAKE)
	cd src && $(LRELEASE) juffed.pro

distclean:
	rm juffed
	rm l10n/juffed_*.qm
	cd src && make clean
	rm src/Makefile

clean:
	cd src && make clean
	rm src/Makefile

install:
#	Directories
	install -d $(DESTDIR)
	install -d $(DESTDIR)/l10n
	install -d $(DESTDIR)/hlschemes
#	Copy files
	install -m 755 juffed $(DESTDIR)
	install -m 644 COPYING $(DESTDIR)
	install -m 644 l10n/*.qm $(DESTDIR)/l10n/
	install -m 644 hlschemes/* $(DESTDIR)/hlschemes/
#	Binary
	install -d $(BIN)
	cd $(BIN) && ln -sf ../share/juffed/juffed juffed
#	App icon and .desktop file
	install -d $(PIXMAPS)
	install -d $(APPLICATIONS)
	install -m 644 juffed.png $(PIXMAPS)/
	install -m 644 juffed.desktop $(APPLICATIONS)/

uninstall:
	rm -rf $(DESTDIR)
	rm $(BIN)/juffed
	rm $(PIXMAPS)/juffed.png
	rm $(APPLICATIONS)/juffed.desktop


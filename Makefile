linux:
	cd src && make -f Makefile all

win32:
	cd src && make -f Makefile.win32 all

clean:
	cd src && make -f Makefile clean

win32-clean:
	cd src && make -f Makefile.win32 clean

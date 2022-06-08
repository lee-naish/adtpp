# makefile for adtpp tool which supports polymorphic algebraic data types
# for C by preprocessing a foo.adt file to produce a foo.h
# file to be #included in C code

EXECUTABLE = adtpp
ADTPP = ./$(EXECUTABLE)
# Generate VERSION line with: date +'VERSION = 3.%y%m%d'
# or in vim: :/^VERSION /!date +'VERSION = 3.\%y\%m\%d'
VERSION = 3.220608
CC = gcc
LEX = parser.l
YACC = parser.y
YFLAG = -d
OFLAG = -o
# XXX currently some macros result in some spurious warnings so we turn
# them off - could try adding x=x etc in macros and init and see if
# warnings are avoided and -O3 cleans up
CFLAG = -Wall -Wformat -Wno-unused-variable -Wno-maybe-uninitialized
ADT_SUPPORT = adtpp.c polymorph.c poly_fns.c
# To make adtpp we need adt_t.h, which is generated from adt_t.adt
# by adtpp.  To avoid the cyclic dependency and do some basic
# testing we provide a master version of adt_t.h which is copied to
# adt_t.h when we make $(EXECUTABLE); the .adt version is also copied
# from a master version when testing.  If any functionality is changed a
# new (correct!) adt_t.h can be manually copied back to adt_master.h.
ADT_MASTER = adt_master
SOURCE = parser.l parser.y polymorph.c poly_fns.c adtpp.c $(ADT_MASTER).adt $(ADT_MASTER).h makefile

# XXX should go from .o files, not recompile from .c always
# Note: this clobbers adt_t.h then removes it (so make test
# will remake it using adtpp)
$(EXECUTABLE) : lex.yy.c y.tab.c $(ADT_SUPPORT) $(ADT_MASTER).h
	cp $(ADT_MASTER).h adt_t.h
	$(CC) $(CFLAG) -DADTPP_VERSION=\"$(VERSION)\" lex.yy.c y.tab.c $(ADT_SUPPORT) $(OFLAG) $(EXECUTABLE)
	rm adt_t.h

# the adtpp tool takes a foo.adt file and generates a foo.h file
# This generic rule can be copied to user-defined makefiles for convenience
# We comment it out here so adt_master.h is not clobbered automatically
# if adt_master.adt is touched (so we need a rule for adt_t.h)
# %.h : %.adt
# 	$(ADTPP) $<

adt_t.h: adt_t.adt
	$(ADTPP) $<

adt_t.adt : $(ADT_MASTER).adt
	cp $(ADT_MASTER).adt adt_t.adt

lex.yy.c : $(LEX)
	flex $(LEX)

y.tab.c : $(YACC)
	yacc $(YFLAG) $(YACC)

Readme : Readme.src makefile
	sed s/VERSION/$(VERSION)/ < Readme.src > Readme
	echo "You might want to: cp Readme ../README.md"

clean :
	rm -rf lex.yy.c y.tab.c y.output y.tab.h a.out *.o adt_t.h Readme
	-rm -i $(EXECUTABLE)

# test currently just checks that the system bootstraps itself
# consistently.  ie, when it generates adt_t.h from adt_t.adt
# it gets a file identical to $(ADT_MASTER).h
# XXX should do some other testing also
test : $(EXECUTABLE) $(ADT_MASTER).h adt_t.h
	(diff $(ADT_MASTER).h adt_t.h && echo PASSED) || echo FAILED

dist : $(SOURCE)
	tar -cvzf adt_archive_$(VERSION).tar.gz $(SOURCE)

install : $(EXECUTABLE)
	cp $(EXECUTABLE) $(HOME)/bin/


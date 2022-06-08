# adtpp basic documentation for web page

# where to install XXX
WEBDIR = ../../../lee-naish.github.io/papers/adtpp/how_to_use

# adtpp executable for testing
ADTPP = ../../src/adtpp

web : example.adt.txt example.c.txt index.html

webinstall : example.adt.txt example.c.txt index.html
	cp example.adt.txt example.c.txt index.html $(WEBDIR)

example.adt.txt : example.adt
	cp example.adt example.adt.txt

example.c.txt : example.c
	cp example.c example.c.txt

test : example.adt example.c
	$(ADTPP) example.adt
	gcc -o example example.c
	./example

clean :
	-rm -rf example a.out tmp* *.o example.h example.c.txt example.adt.txt

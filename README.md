Adtpp: algebraic data types for C

It is lightweight - simply a preprocessor that converts algebraic type
definitions and declarations into a C header file with macros, typedefs
etc. Files with C code are not processed at all. The type checking of
the C compiler is harnessed as much as possible but error messages are
not very informative.

It is efficient - uses exactly the same representation as C for many
simple data types such as linked lists and binary trees and is more
efficient than typical C code for more complicated types (it produces
low level C code that you really wouldn't want to write yourself, but
you don't need to know the details as it is all wrapped up inside a
nice interface). The implementation uses "tagged pointers", like many
declarative languages.

It is polymorphic - you can define a type such as list of t, where t
is a type variable, then use multiple instances of this type and write
polymorphic functions such as length that work with all instances. Due
to the limitations of C type checking, all instances that are used in
the program must be declared and named.

It is safe(r) - lots of unsafe and error-prone operations that you use in
C can be avoided, such as dereferencing possibly NULL pointers, accessing
possibly inappropriate fields of unions, and casting to/from pointers
to void. It also integrates very easily with a conservative garbage
collector.  Its not entirely safe of course - its still C you are writing.

Initial design, prototype implementation (V0) and more flexible polymorphism  
implementation (V3) by Lee Naish
Initial C implementation (V1) by Yi Lin
Initial polymorphism (V2) implemented by Aleck MacNally
Additional supervision/contributions by Peter Schachte

Copyright (c) 2015 Yi Lin, Aleck MacNally, Lee Naish and Peter Schachte
Originally at https://bitbucket.org/Macnaa/adt4c-with-polymorphism.git
this repository (maintained by Lee Naish) is a copy of Version 3.151006
The intention is to have a cleaned up repository, better documentation
etc. The coding uses adtpp and could be elegant, robust etc but is
actually awful in so many ways. It could do with a major rewrite.
This is Version 3.220608 (the version number after the decimal point
indicates the year/month/day the version was created).

Environment Requirement(Basic):
1. Gcc version 4.0.0
2. Bison version 2.4.2
4. Flex version 2.5.35
5. Make 3.81

directory structure:

src/
C/flex/bison source code etc

doc/
various documentation

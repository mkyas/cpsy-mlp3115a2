CXXFLAGS = -std=c++20 -Wall -O2
LDLIBS = -li2c

testmpl: testmpl.cc libmpl3115a2.a

libmpl3115a2.a: libmpl3115a2.a(MPL3115A2.o)
	ranlib $@

libmpl3115a2.a(MPL3115A2.o): MPL3115A2.o

MPL3115A2.o: MPL3115A2.cpp MPL3115A2.hpp


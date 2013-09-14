all: test test64
asm: test.s test64.s

OBJDUMP = objdump
CXX = g++
CXXFLAGS_BASE = -pipe -Wall -std=c++11 #--coverage
CXXFLAGS_DEBUG = $(CXXFLAGS_BASE) -g -O0 -fsanitize=address
CXXFLAGS_RELEASE = $(CXXFLAGS_BASE) -g -Ofast -march=native -DNDEBUG -funroll-loops #-ftree-vectorizer-verbose=2
#CXXFLAGS = $(CXXFLAGS_DEBUG)
CXXFLAGS = $(CXXFLAGS_RELEASE)

bigint.hpp.gch: bigint.hpp limbs.hpp.gch traits.hpp
	$(CXX) $(CXXFLAGS) -o $@ $< #-include-gch limbs.hpp.gch

limbs.hpp.gch: limbs.hpp traits.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<

test: test.cc bigint.hpp.gch
	$(CXX) $(CXXFLAGS) -o $@ $< #-include-gch bigint.hpp.gch

test64: test.cc bigint.hpp.gch
	$(CXX) $(CXXFLAGS) -DUSE_64BIT_LIMB -o $@ $< #-include-gch bigint.hpp.gch

clean:
	rm -f *.*ch *.s

test.s: test
	$(OBJDUMP) -Sd $< > $@

test64.s: test64
	$(OBJDUMP) -Sd $< > $@

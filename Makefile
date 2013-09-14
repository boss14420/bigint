all: test test64
asm: test.s test64.s

OBJDUMP = objdump
CXX = g++
CXXFLAGS_BASE = -pipe -Wall -std=c++11 #--coverage
CXXFLAGS_DEBUG = $(CXXFLAGS_BASE) -g -O0 
#CXXFLAGS_RELEASE = $(CXXFLAGS_BASE) -g -Ofast -march=native -DNDEBUG -funroll-loops #-ftree-vectorizer-verbose=2
CXXFLAGS_RELEASE = $(CXXFLAGS_BASE) -march=armv6zk -mcpu=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -Ofast -DNDEBUG -funroll-loops
#CXXFLAGS = $(CXXFLAGS_DEBUG)
CXXFLAGS = $(CXXFLAGS_RELEASE)

bigint.hpp.gch: bigint.hpp limbs.hpp.gch traits.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< #-include-gch limbs.hpp.gch

limbs.hpp.gch: limbs.hpp traits.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test.o: test.cc bigint.hpp.gch
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test: test.o
	$(CXX) $(CXXFLAGS) -o $@ $< #-include-gch bigint.hpp.gch

test64.o: test.cc bigint.hpp.gch
	$(CXX) $(CXXFLAGS) -DUSE_64BIT_LIMB -c -o $@ $< #-include-gch bigint.hpp.gch

test64: test64.o
	$(CXX) $(CXXFLAGS) -o $@ %<

clean:
	rm -f *.*ch *.s *.o

test.s: test
	$(OBJDUMP) -Sd $< > $@

test64.s: test64
	$(OBJDUMP) -Sd $< > $@

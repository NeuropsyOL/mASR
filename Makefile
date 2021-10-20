# This file is part of the asr project

include /usr/share/openmha/config.mk

# Include openmha headers
INCLUDES=-I/usr/include/openmha

# Link against libopenmha
LIBS=-lopenmha

# Sources
SOURCES=$(wildcard codegen/*.cpp)

# Use mha version number the plugin is linked against
MHAVERSION=$(shell mha --help | grep version | sed 's/.* version \([0-9]*\.[0-9]*\.[0-9]*\).*/\1/g')

# Version for packaging
REVISION=$(shell git rev-list --count HEAD)

# Arch for packaging
ARCH=$(shell uname -m)

# If the git repository contains any non-committed modifications, then
# GITMODIFIED is set to "-modified", else it is empty.  GITMODIFIED is used
# as part of the debian package version to easily spot non-reproducible debs
GITMODIFIED=$(shell test -z $(git status --porcelain -uno) || echo "-modified")

# Part of the git commit SHA1 is stored in COMMITHASH and becomes part of the
# debian package version
COMMITHASH=$(shell git log -1 --abbrev=7 --pretty='format:%h')

# FULLVERSIONALL is the package version plus git age plus git hash plus any
# modification indicator but without the compiler version.
FULLVERSIONALL=$(MHAVERSION)-$(REVISION)-$(COMMITHASH)$(GITMODIFIED)
export FULLVERSIONALL

# FULLVERSIONGCC is FULLVERSIONALL plus the compiler version
FULLVERSIONGCC=$(FULLVERSIONALL)-gcc$(GCC_VER)

CXXFLAGS+=-Wno-error=unused-function

# Default target
all: asr$(DYNAMIC_LIB_EXT)

# Convenience target: Remove compiled products
clean:
	rm -rf codegen asr$(DYNAMIC_LIB_EXT)

asr.zip: asr_calibrate_simple.m asr_process_simple.m make.m
	matlab -batch 'make'

codegen/.directory: asr.zip
	unzip -d codegen asr.zip
	touch codegen/.directory

asr$(DYNAMIC_LIB_EXT): asr.cpp codegen/.directory
	$(CXX) -shared -o asr$(DYNAMIC_LIB_EXT) $(CXXFLAGS) -Wno-deprecated-copy -Icodegen $(INCLUDES) $(LIBS) $< $(SOURCES)

benchmark: benchmark.cpp codegen/.directory
	$(CXX) $< $(SOURCES) $(CXXFLAGS) -Wno-deprecated-copy -Icodegen -L/opt/homebrew/lib -lbenchmark -lpthread -o benchmark

test: test.cpp codegen/.directory
	$(CXX) $< $(SOURCES) $(CXXFLAGS) -Wno-deprecated-copy -Icodegen -L/opt/homebrew/lib -lgtest -lgtest_main -lpthread -o test

deb:
	test -e asr$(DYNAMIC_LIB_EXT) || $(MAKE) asr$(DYNAMIC_LIB_EXT)
	mhamakedeb asr.csv $(FULLVERSIONGCC)
# This file is part of the asr project

include /usr/share/openmha/config.mk

# Include openmha headers
INCLUDES=-I/usr/include/openmha

# Link against libopenmha
LIBS=-lopenmha

# Sources
SOURCES=$(wildcard codegen/*.cpp)

# Version for packaging
VERSION=$(shell git rev-list --count HEAD)

# Arch for packaging
ARCH=$(shell uname -m)

# Default target
all: asr$(DYNAMIC_LIB_EXT) asr.zip

# Redirect asr to proper target
asr: asr$(DYNAMIC_LIB_EXT)

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

	$(CXX) $< $(SOURCES) $(CXXFLAGS) -Wno-deprecated-copy -Icodegen -lbenchmark -lpthread -o benchmark
benchmark: benchmark.cpp codegen/.directory

deb: asr$(DYNAMIC_LIB_EXT)
	touch asr_$(VERSION)-1_$(ARCH).deb

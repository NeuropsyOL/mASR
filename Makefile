# This file is part of the asr project

include /usr/share/openmha/config.mk

# Include openmha headers
INCLUDES=-I/usr/include/openmha

# Link against libopenmha
LIBS=-lopenmha

# Sources
SOURCES=$(wildcard codegen/*.cpp)

# Default target is example21
all: asr codegen

# Convenience target: Remove compiled products
clean:
	rm -rf codegen asr$(DYNAMIC_LIB_EXT)

asr.zip: asr_calibrate_simple.m asr_process_simple.m make.m
	matlab -batch 'make'
	unzip -d codegen asr.zip

asr: asr.cpp asr.zip
	$(CXX) -shared -o asr$(DYNAMIC_LIB_EXT) $(CXXFLAGS) -Wno-deprecated-copy -Icodegen $(INCLUDES) $(LIBS) $< $(SOURCES)

benchmark: benchmark.cpp asr.zip
	$(CXX) $< $(SOURCES) $(CXXFLAGS) -Wno-deprecated-copy -Icodegen -lbenchmark -lpthread -o benchmark

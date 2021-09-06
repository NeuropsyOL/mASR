# This file is part of the asr project

include /usr/local/share/openmha/config.mk

# Include openmha headers
INCLUDES=-I/usr/local/include/openmha

# Link against libopenmha
LIBS=-lopenmha

# Sources
SOURCES=$(wildcard *.cpp)

# Default target is example21
all: asr

# Convenience target: Remove compiled products
clean:
	rm -rf codegen asr$(DYNAMIC_LIB_EXT)

asr: $(patsubst benchmark.cpp,,$(SOURCES))
	$(CXX) -shared -o asr$(DYNAMIC_LIB_EXT) $(CXXFLAGS) -Wno-deprecated-copy $(INCLUDES) $(LIBS) $^

benchmark: $(patsubst asr.cpp,,$(SOURCES))
	$(CXX) -o benchmark $(CXXFLAGS) -Wno-error -Wno-deprecated-copy $(INCLUDES) $(LDLIBS) -lbenchmark $^

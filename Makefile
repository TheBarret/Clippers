# Clippers - Simple C++ URL extraction & grouping toolset
CXX      := g++
CXXFLAGS := -Wall -O2 -std=c++17
LDFLAGS1 := -lgumbo
LDFLAGS2 := -lcurl

TARGETS  := collector group harvest

all: $(TARGETS)

collector: collector.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS1)

group: group.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

harvest: harvest.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS2)

clean:
	rm -f $(TARGETS)

.PHONY: all clean

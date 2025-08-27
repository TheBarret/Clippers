# Clippers - Simple C++ URL extraction & grouping toolset
CXX      := g++
CXXFLAGS := -Wall -O2 -std=c++17
LDFLAGS  := -lgumbo

TARGETS  := collector group

all: $(TARGETS)

collector: collector.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

group: group.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)

.PHONY: all clean

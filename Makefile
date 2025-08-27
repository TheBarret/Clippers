CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread
LIBS = -lcurl

TARGET = harvest
SOURCE = harvest.cpp

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE) $(LIBS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/

# Development build with debug info
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Release build with optimizations
release: CXXFLAGS += -DNDEBUG -O3
release: $(TARGET)

# Check if libcurl is available
check-deps:
	@echo "Checking dependencies..."
	@pkg-config --exists libcurl && echo "✓ libcurl found" || echo "✗ libcurl not found"
	@echo "C++17 support: $(shell $(CXX) -std=c++17 -E - < /dev/null >/dev/null 2>&1 && echo "✓" || echo "✗")"

# Install dependencies (Ubuntu/Debian)
install-deps-ubuntu:
	sudo apt-get update
	sudo apt-get install -y libcurl4-openssl-dev build-essential

# Install dependencies (CentOS/RHEL/Fedora)
install-deps-centos:
	sudo yum install -y libcurl-devel gcc-c++ make
	# For newer versions:
	# sudo dnf install -y libcurl-devel gcc-c++ make

# Install dependencies (Arch)
install-deps-arch:
	sudo pacman -S --needed curl base-devel

help:
	@echo "Available targets:"
	@echo "  all          - Build the harvest application"
	@echo "  clean        - Remove built files"
	@echo "  install      - Install to /usr/local/bin/"
	@echo "  debug        - Build with debug information"
	@echo "  release      - Build with optimizations"
	@echo "  check-deps   - Check if dependencies are available"
	@echo "  install-deps-ubuntu  - Install dependencies on Ubuntu/Debian"
	@echo "  install-deps-centos  - Install dependencies on CentOS/RHEL/Fedora"
	@echo "  install-deps-arch    - Install dependencies on Arch Linux"
	@echo "  help         - Show this help message"

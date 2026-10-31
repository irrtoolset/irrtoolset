# Thin wrapper around CMake.
BUILD_DIR ?= build
PREFIX    ?= /usr/local
CMAKE     ?= cmake
CMAKE_ARGS ?=

.PHONY: all bootstrap configure build install examples clean distclean

all: build

# Install build dependencies for this platform.
bootstrap:
	./misc/bootstrap.sh

# Cheap and idempotent, so always run it: a failed first configure leaves a
# CMakeCache.txt behind but no build files.
configure:
	$(CMAKE) -S . -B $(BUILD_DIR) -DCMAKE_INSTALL_PREFIX=$(PREFIX) $(CMAKE_ARGS)

build: configure
	$(CMAKE) --build $(BUILD_DIR)

install: build
	$(CMAKE) --install $(BUILD_DIR)

# Generate the example router configs (queries whois.radb.net).
examples: build
	RTCONFIG=$(abspath $(BUILD_DIR))/bin/rtconfig OUTDIR=$(abspath $(BUILD_DIR))/examples ./examples/build_examples.sh

clean:
	-$(CMAKE) --build $(BUILD_DIR) --target clean

distclean:
	rm -rf $(BUILD_DIR)

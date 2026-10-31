BUILD_DIR ?= build
PREFIX    ?= /usr/local
CMAKE     ?= cmake
CMAKE_ARGS ?=

.DEFAULT_GOAL := help
.PHONY: help bootstrap configure build install examples clean distclean

help: ## Show this message
	@echo "Hello! This is the IRRToolSet build wrapper around CMake."
	@echo
	@echo "Targets:"
	@awk -F ':.*## ' '/^[a-z-]+:.*## /{printf "  make %-10s %s\n", $$1, $$2}' $(MAKEFILE_LIST)
	@echo
	@echo "Variables (current): BUILD_DIR=$(BUILD_DIR) PREFIX=$(PREFIX) CMAKE_ARGS=$(CMAKE_ARGS)"

bootstrap: ## Install build dependencies for this platform
	./misc/bootstrap.sh

# Always run: a failed configure leaves CMakeCache.txt but no build files.
configure: ## Configure the build in BUILD_DIR
	$(CMAKE) -S . -B $(BUILD_DIR) -DCMAKE_INSTALL_PREFIX=$(PREFIX) $(CMAKE_ARGS)

build: configure ## Build the binaries into BUILD_DIR/bin
	$(CMAKE) --build $(BUILD_DIR)

install: build ## Install into PREFIX (honours DESTDIR)
	$(CMAKE) --install $(BUILD_DIR)

examples: build ## Generate example router configs into BUILD_DIR/examples (needs network)
	RTCONFIG=$(abspath $(BUILD_DIR))/bin/rtconfig OUTDIR=$(abspath $(BUILD_DIR))/examples ./examples/build_examples.sh

clean: ## Remove build outputs, keep the configuration
	-$(CMAKE) --build $(BUILD_DIR) --target clean

distclean: ## Delete BUILD_DIR entirely
	rm -rf $(BUILD_DIR)

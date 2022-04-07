SOURCES := $(shell find src/ -type f | grep -E '\.(h|c)(pp)?$$')

.PHONY: help
help:
	@echo -e "Try one of these make targets:\n"
	@grep "^\.PHONY: " Makefile | cut -d" " -f2- | sed -e "s/ /\n/g" | grep -v "^_"



.PHONY: build
build: build/TDT4230_Project
build/TDT4230_Project: ${SOURCES} build/Makefile | has-make
	make -C build



.PHONY: run
run: 
	cd build && ./TDT4230_Project



.PHONY: build-and-run
build-and-run: build
	clear && cd build && ./TDT4230_Project



.PHONY: clean
clean:
	@rm -rfv build/*


# === helpers: ===

build/Makefile: | build/ has-cmake
	cd build && cmake ..


# make folders, use as order-only prerequisite
%/:
	mkdir -p $*

# check if program is available in PATH, use as order-only prerequisite
has-%:
	@command -v $* >/dev/null || ( \
		echo "ERROR: Command '$*' not found! Make sure it is installed and available in PATH"; \
		false; \
	) >&2
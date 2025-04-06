all: cmake
.PHONY : all

compile-commands:
	./scripts/compile-commands.sh

.PHONY : compile-commands

# NOTE: -D flag(s) may not be needed on linux
cmake:
	# cmake -S . -B build -DSDLTTF_VENDORED=TRUE -DSDLIMAGE_VENDORED=TRUE
	cmake -S . -B build -DSDLTTF_VENDORED=TRUE -DCMAKE_POLICY_VERSION_MINIMUM=3.5

.PHONY : cmake

vs-cmake:
	cmake -G "Visual Studio 17 2022" -S . -B build

.PHONY : vs-cmake

build:
	cmake --build build --parallel

build-no-parallel:
	cmake --build build

.PHONY : build

full-clean:
	rm -rf ./build

.PHONY : full-clean

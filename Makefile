all: build/Makefile
	make -C build

build/Makefile:
	mkdir -p build
	cmake -H. -Bbuild


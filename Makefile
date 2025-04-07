.PHONY: all test clean

SOURCES := $(wildcard src/*.c)

all:
	echo "Build run"
	mkdir -p build
	gcc -o build/main $(SOURCES)

test: 
	echo "Tests run"
	build/main

clean:
	rm -rf build/*

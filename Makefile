.PHONY: all test clean

all:
	echo "Build run"
	mkdir -p build
	gcc -o build/dummy src/dummy.c

test: 
	echo "Tests run"
	build/dummy

clean:
	rm -rf build/*

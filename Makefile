.PHONY: all test clean

all:
	echo "Build run"
	gcc -o build/dummy src/dummy.c

test: 
	echo "Tests run"
	build/dummy

clean:
	rm -rf build/*

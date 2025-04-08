.PHONY: all test clean

ASN_FILES := $(wildcard dataview/*.asn)
ACN_FILES := $(wildcard dataview/*.acn)
ASN1_POLICY_FLAGS ?= -typePrefix cfdp -renamePolicy 3 -equal -fp AUTO -c -ig -uPER -ACN

SOURCES := $(wildcard src/*.c)
DATAVIEW_SOURCES := $(wildcard dataview/*.c)

all:
	echo "Build run"
	mkdir -p build
	gcc -Isrc -Idataview -o build/main $(SOURCES) $(DATAVIEW_SOURCES)

build-asn:
	rm -rf dataview/*.h
	rm -rf dataview/*.c
	asn1scc -o dataview ${ASN1_POLICY_FLAGS} ${ASN_FILES} ${ACN_FILES}

test: 
	echo "Tests run"
	build/main

clean:
	rm -rf build/*

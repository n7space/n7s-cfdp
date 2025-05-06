.PHONY: all test clean

ASN_FILES := $(wildcard dataview/*.asn)
ACN_FILES := $(wildcard dataview/*.acn)
ASN1_POLICY_FLAGS ?= -typePrefix cfdp -renamePolicy 3 -equal -fp AUTO -c -ig -uPER -ACN

SOURCES := $(wildcard src/*.c)
DATAVIEW_SOURCES := $(wildcard dataview/*.c)
TEST_SOURCES := $(wildcard test/test_send_of_small_file/*.c)

all:
	echo "Build run"
	mkdir -p build
	gcc -Isrc -Idataview -o build/main $(SOURCES) $(DATAVIEW_SOURCES)

build-asn:
	rm -rf dataview/*.h
	rm -rf dataview/*.c
	asn1scc -mfm mapping_functions -o dataview ${ASN1_POLICY_FLAGS} ${ASN_FILES} ${ACN_FILES}

test-send-small-file:
	mkdir -p build
	gcc -g -pthread -Isrc -Idataview -Itest -o build/cfdp_test $(filter-out src/main.c, $(SOURCES)) $(TEST_SOURCES) $(DATAVIEW_SOURCES)
	chmod +x test/test_send_of_small_file/python_cfdp_receiver.py
	python3 test/test_send_of_small_file/python_cfdp_receiver.py &
	pushd build; ./cfdp_test; popd
	sleep 1
	pkill python3

test: test-send-small-file

clean:
	rm -rf build/*

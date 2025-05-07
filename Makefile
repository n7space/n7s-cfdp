.PHONY: all test clean

ASN_FILES := $(wildcard dataview/*.asn)
ACN_FILES := $(wildcard dataview/*.acn)
ASN1_POLICY_FLAGS ?= -typePrefix cfdp -renamePolicy 3 -equal -fp AUTO -c -ig -uPER -ACN

SOURCES := $(wildcard src/*.c)
DATAVIEW_SOURCES := $(wildcard dataview/*.c)
TEST_SOURCES := $(wildcard test/test_send_of_small_file/*.c)

CFDP_PYTHON := test/test_send_of_small_file/python_cfdp_receiver.py
CFDP_PYTHON_PID := script.pid
RECEIVER_FILE := test/test_send_of_small_file/target/received_small.txt

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
	mkdir -p test/test_send_of_small_file/target
	-pkill python3
	gcc -g -pthread -Isrc -Idataview -Itest -o build/cfdp_test $(filter-out src/main.c, $(SOURCES)) $(TEST_SOURCES) $(DATAVIEW_SOURCES)
	chmod +x $(CFDP_PYTHON)
	python3 $(CFDP_PYTHON) & echo $$! > $(CFDP_PYTHON_PID)
	sleep 1
	./build/cfdp_test
	sleep 1
	kill `cat $(CFDP_PYTHON_PID)` && rm -f $(CFDP_PYTHON_PID)
	rm -f $(RECEIVER_FILE)

test: test-send-small-file

clean:
	rm -rf build/*

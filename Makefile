.PHONY: all test clean

ASN_FILES := $(wildcard dataview/*.asn)
ACN_FILES := $(wildcard dataview/*.acn)
ASN1_POLICY_FLAGS ?= -typePrefix cfdp -renamePolicy 3 -equal -fp AUTO -c -ig -uPER -ACN

SOURCES := $(wildcard src/*.c)
DATAVIEW_SOURCES := $(wildcard dataview/*.c)
SEND_SMALL_FILE_TEST_SOURCES := $(wildcard test/test_send_small_file/*.c)
RECEIVE_SMALL_FILE_TEST_SOURCES := $(wildcard test/test_receive_small_file/*.c)
SEND_MANY_SMALL_FILES_TEST_SOURCES := $(wildcard test/test_send_many_small_files/*.c)
RECEIVE_MANY_SMALL_FILES_TEST_SOURCES := $(wildcard test/test_receive_many_small_files/*.c)
SEND_MEDIUM_FILE_TEST_SOURCES := $(wildcard test/test_send_medium_file/*.c)

CFDP_PYTHON_RECEIVER := test/test_send_small_file/python_cfdp_receiver.py
CFDP_PYTHON_RECEIVER_PID := script.pid
CFDP_PYTHON_MANY_FILES_RECEIVER := test/test_send_many_small_files/python_cfdp_receiver.py
CFDP_PYTHON_SENDER := test/test_receive_small_file/python_cfdp_sender.py
CFDP_PYTHON_MANY_FILES_SENDER := test/test_receive_many_small_files/python_cfdp_sender.py
CFDP_PYTHON_MEDIUM_RECEIVER := test/test_send_medium_file/python_cfdp_receiver.py

SENT_FILE1 := test/test_send_small_file/target/received_small1.txt
RECEIVED_FILE1 := test/test_receive_small_file/target/received_small1.txt

SENT_MANY_FILE1 := test/test_send_many_small_files/target/received_small1.txt
SENT_MANY_FILE2 := test/test_send_many_small_files/target/received_small2.txt
SENT_MANY_FILE3 := test/test_send_many_small_files/target/received_small3.txt

RECEIVED_MANY_FILE1 := test/test_receive_many_small_files/target/received_small1.txt
RECEIVED_MANY_FILE2 := test/test_receive_many_small_files/target/received_small2.txt
RECEIVED_MANY_FILE3 := test/test_receive_many_small_files/target/received_small3.txt

CFDP_PID := script.pid

all:
	echo "Build run"
	mkdir -p build
	gcc -Isrc -Idataview -o build/main $(SOURCES) $(DATAVIEW_SOURCES)

build-asn:
	rm -rf dataview/*.h
	rm -rf dataview/*.c
	asn1scc -mfm mapping_functions -o dataview ${ASN1_POLICY_FLAGS} ${ASN_FILES} ${ACN_FILES}

clean:
	rm -rf build/*
	rm -rf test/test_send_small_file/target/*
	rm -rf test/test_receive_small_file/target/*
	rm -rf test/test_send_many_small_files/target/*
	rm -rf test/test_receive_many_small_files/target/*
	rm -rf test/test_send_medium_file/target/*

test-send-small-file:
	mkdir -p build
	mkdir -p test/test_send_small_file/target
	-pkill python3
	gcc -g -pthread -Isrc -Idataview -Itest -o build/send_small_file_cfdp_test $(filter-out src/main.c, $(SOURCES)) $(SEND_SMALL_FILE_TEST_SOURCES) $(DATAVIEW_SOURCES)
	chmod +x $(CFDP_PYTHON_RECEIVER)
	python3 $(CFDP_PYTHON_RECEIVER) & echo $$! > $(CFDP_PYTHON_RECEIVER_PID)
	sleep 1
	./build/send_small_file_cfdp_test
	sleep 1
	kill `cat $(CFDP_PYTHON_RECEIVER_PID)` && rm -f $(CFDP_PYTHON_RECEIVER_PID)

test-receive-small-file:
	mkdir -p build
	mkdir -p test/test_receive_small_file/target
	-pkill python3
	gcc -g -pthread -Isrc -Idataview -Itest -o build/receive_small_file_cfdp_test $(filter-out src/main.c, $(SOURCES)) $(RECEIVE_SMALL_FILE_TEST_SOURCES) $(DATAVIEW_SOURCES)
	./build/receive_small_file_cfdp_test & echo $$! > $(CFDP_PID)
	sleep 1
	chmod +x $(CFDP_PYTHON_SENDER)
	python3 $(CFDP_PYTHON_SENDER)
	sleep 1
	kill `cat $(CFDP_PID)` && rm -f $(CFDP_PID)

test-send-many-small-files:
	mkdir -p build
	mkdir -p test/test_send_many_small_files/target
	-pkill python3
	gcc -g -pthread -Isrc -Idataview -Itest -o build/send_many_small_files_cfdp_test $(filter-out src/main.c, $(SOURCES)) $(SEND_MANY_SMALL_FILES_TEST_SOURCES) $(DATAVIEW_SOURCES)
	chmod +x $(CFDP_PYTHON_MANY_FILES_RECEIVER)
	python3 $(CFDP_PYTHON_MANY_FILES_RECEIVER) & echo $$! > $(CFDP_PYTHON_RECEIVER_PID)
	sleep 1
	./build/send_many_small_files_cfdp_test
	sleep 1
	kill `cat $(CFDP_PYTHON_RECEIVER_PID)` && rm -f $(CFDP_PYTHON_RECEIVER_PID)

test-receive-many-small-files:
	mkdir -p build
	mkdir -p test/test_receive_many_small_files/target
	-pkill python3
	gcc -g -pthread -Isrc -Idataview -Itest -o build/receive_many_small_files_cfdp_test $(filter-out src/main.c, $(SOURCES)) $(RECEIVE_MANY_SMALL_FILES_TEST_SOURCES) $(DATAVIEW_SOURCES)
	./build/receive_many_small_files_cfdp_test & echo $$! > $(CFDP_PID)
	sleep 1
	chmod +x $(CFDP_PYTHON_MANY_FILES_SENDER)
	python3 $(CFDP_PYTHON_MANY_FILES_SENDER)
	sleep 1
	kill `cat $(CFDP_PID)` && rm -f $(CFDP_PID)

test-send-medium-file:
	mkdir -p build
	mkdir -p test/test_send_medium_file/target
	-pkill python3
	gcc -g -pthread -Isrc -Idataview -Itest -o build/send_medium_file_cfdp_test $(filter-out src/main.c, $(SOURCES)) $(SEND_MEDIUM_FILE_TEST_SOURCES) $(DATAVIEW_SOURCES)
	chmod +x $(CFDP_PYTHON_MEDIUM_RECEIVER)
	python3 $(CFDP_PYTHON_MEDIUM_RECEIVER) & echo $$! > $(CFDP_PYTHON_RECEIVER_PID)
	sleep 1
	./build/send_medium_file_cfdp_test
	sleep 1
	kill `cat $(CFDP_PYTHON_RECEIVER_PID)` && rm -f $(CFDP_PYTHON_RECEIVER_PID)

test: clean test-send-small-file test-receive-small-file test-send-many-small-files test-receive-many-small-files test-send-medium-file

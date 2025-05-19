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
RECEIVE_MEDIUM_FILE_TEST_SOURCES := $(wildcard test/test_receive_medium_file/*.c)
SEND_BIG_FILE_TEST_SOURCES := $(wildcard test/test_send_big_file/*.c)
RECEIVE_BIG_FILE_TEST_SOURCES := $(wildcard test/test_receive_big_file/*.c)
SEND_BIG_FILE_TEST_WITH_TRANSPORT_NOT_READY_SOURCES := $(wildcard test/test_send_file_with_transport_not_ready/*.c)
TEST_SOURCE := test/test_filestore.c test/test_transport.c

CFDP_PYTHON_RECEIVER := test/test_send_small_file/python_cfdp_receiver.py
CFDP_PYTHON_RECEIVER_PID := script.pid
CFDP_PYTHON_MANY_FILES_RECEIVER := test/test_send_many_small_files/python_cfdp_receiver.py
CFDP_PYTHON_SENDER := test/test_receive_small_file/python_cfdp_sender.py
CFDP_PYTHON_MANY_FILES_SENDER := test/test_receive_many_small_files/python_cfdp_sender.py
CFDP_PYTHON_MEDIUM_SENDER := test/test_receive_medium_file/python_cfdp_sender.py
CFDP_PYTHON_BIG_SENDER := test/test_receive_big_file/python_cfdp_sender.py
CFDP_PYTHON_MEDIUM_RECEIVER := test/test_send_medium_file/python_cfdp_receiver.py
CFDP_PYTHON_BIG_RECEIVER := test/test_send_big_file/python_cfdp_receiver.py
CFDP_PYTHON_BIG_TRANSPORT_IS_READY_RECEIVER := test/test_send_file_with_transport_not_ready/python_cfdp_receiver.py

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
	rm -rf test/test_receive_medium_file/target/*
	rm -rf test/test_receive_big_file/target/*
	rm -rf test/test_send_file_with_transport_not_ready/target/*

test-send-small-file:
	mkdir -p build
	mkdir -p test/test_send_small_file/target
	-pkill python3
	gcc -g -pthread -Isrc -Idataview -Itest -o build/send_small_file_cfdp_test $(filter-out src/main.c, $(SOURCES)) $(SEND_SMALL_FILE_TEST_SOURCES) $(DATAVIEW_SOURCES) $(TEST_SOURCE)
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
	gcc -g -pthread -Isrc -Idataview -Itest -o build/receive_small_file_cfdp_test $(filter-out src/main.c, $(SOURCES)) $(RECEIVE_SMALL_FILE_TEST_SOURCES) $(DATAVIEW_SOURCES) $(TEST_SOURCE)
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
	gcc -g -pthread -Isrc -Idataview -Itest -o build/send_many_small_files_cfdp_test $(filter-out src/main.c, $(SOURCES)) $(SEND_MANY_SMALL_FILES_TEST_SOURCES) $(DATAVIEW_SOURCES) $(TEST_SOURCE)
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
	gcc -g -pthread -Isrc -Idataview -Itest -o build/receive_many_small_files_cfdp_test $(filter-out src/main.c, $(SOURCES)) $(RECEIVE_MANY_SMALL_FILES_TEST_SOURCES) $(DATAVIEW_SOURCES) $(TEST_SOURCE)
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
	gcc -g -pthread -Isrc -Idataview -Itest -o build/send_medium_file_cfdp_test $(filter-out src/main.c, $(SOURCES)) $(SEND_MEDIUM_FILE_TEST_SOURCES) $(DATAVIEW_SOURCES) $(TEST_SOURCE)
	chmod +x $(CFDP_PYTHON_MEDIUM_RECEIVER)
	python3 $(CFDP_PYTHON_MEDIUM_RECEIVER) & echo $$! > $(CFDP_PYTHON_RECEIVER_PID)
	sleep 1
	./build/send_medium_file_cfdp_test
	sleep 1
	kill `cat $(CFDP_PYTHON_RECEIVER_PID)` && rm -f $(CFDP_PYTHON_RECEIVER_PID)

test-receive-medium-file:
	mkdir -p build
	mkdir -p test/test_receive_medium_file/target
	-pkill python3
	gcc -g -pthread -Isrc -Idataview -Itest -o build/receive_medium_file_cfdp_test $(filter-out src/main.c, $(SOURCES)) $(RECEIVE_MEDIUM_FILE_TEST_SOURCES) $(DATAVIEW_SOURCES) $(TEST_SOURCE)
	./build/receive_medium_file_cfdp_test & echo $$! > $(CFDP_PID)
	sleep 1
	chmod +x $(CFDP_PYTHON_MEDIUM_SENDER)
	python3 $(CFDP_PYTHON_MEDIUM_SENDER)
	sleep 1
	kill `cat $(CFDP_PID)` && rm -f $(CFDP_PID)

test-send-big-file:
	mkdir -p build
	mkdir -p test/test_send_big_file/target
	-pkill python3
	gcc -g -pthread -Isrc -Idataview -Itest -o build/send_big_file_cfdp_test $(filter-out src/main.c, $(SOURCES)) $(SEND_BIG_FILE_TEST_SOURCES) $(DATAVIEW_SOURCES) $(TEST_SOURCE)
	chmod +x $(CFDP_PYTHON_BIG_RECEIVER)
	python3 $(CFDP_PYTHON_BIG_RECEIVER) & echo $$! > $(CFDP_PYTHON_RECEIVER_PID)
	sleep 1
	./build/send_big_file_cfdp_test
	sleep 1
	kill `cat $(CFDP_PYTHON_RECEIVER_PID)` && rm -f $(CFDP_PYTHON_RECEIVER_PID)

test-receive-big-file:
	mkdir -p build
	mkdir -p test/test_receive_big_file/target
	-pkill python3
	gcc -g -pthread -Isrc -Idataview -Itest -o build/receive_big_file_cfdp_test $(filter-out src/main.c, $(SOURCES)) $(RECEIVE_BIG_FILE_TEST_SOURCES) $(DATAVIEW_SOURCES) $(TEST_SOURCE)
	./build/receive_big_file_cfdp_test & echo $$! > $(CFDP_PID)
	sleep 1
	chmod +x $(CFDP_PYTHON_BIG_SENDER)
	python3 $(CFDP_PYTHON_BIG_SENDER)
	sleep 1
	kill `cat $(CFDP_PID)` && rm -f $(CFDP_PID)

test-send-file-with-transport-not-ready:
	mkdir -p build
	mkdir -p test/test_send_file_with_transport_not_ready/target
	-pkill python3
	gcc -g -pthread -Isrc -Idataview -Itest -o build/send_big_file_cfdp_test $(filter-out src/main.c, $(SOURCES)) $(SEND_BIG_FILE_TEST_WITH_TRANSPORT_NOT_READY_SOURCES) $(DATAVIEW_SOURCES) $(TEST_SOURCE)
	chmod +x $(CFDP_PYTHON_BIG_TRANSPORT_IS_READY_RECEIVER)
	python3 $(CFDP_PYTHON_BIG_TRANSPORT_IS_READY_RECEIVER) & echo $$! > $(CFDP_PYTHON_RECEIVER_PID)
	sleep 1
	./build/send_big_file_cfdp_test
	sleep 1
	kill `cat $(CFDP_PYTHON_RECEIVER_PID)` && rm -f $(CFDP_PYTHON_RECEIVER_PID)

test: clean test-send-small-file test-receive-small-file test-send-many-small-files test-receive-many-small-files test-send-medium-file test-receive-medium-file test-send-big-file test-receive-big-file test-send-file-with-transport-not-ready

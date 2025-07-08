#ifndef TEST_FILESTORE_H
#define TEST_FILESTORE_H

#include "constants.h"
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t test_filestore_get_file_size(const char *filepath);

void test_filestore_read_file(const char *filepath, uint32_t offset, char *data,
			      const uint32_t length);

void test_filestore_write_to_file(const char *filepath, uint32_t offset,
				  const char *data, const uint32_t length);

uint32_t
test_filestore_calculate_checksum(const char *filepath,
				  const enum ChecksumType checksum_type);

void test_filestore_copy_file(const char *src_path, const char *dest_path);

void test_delete_file(const char *filepath);

bool test_filestore_dump_directory_listing_to_file(const char *dirpath,
						   const char *dump_filepath);

#endif
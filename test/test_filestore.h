#ifndef TEST_FILESTORE_H
#define TEST_FILESTORE_H

#include "constants.h"
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t test_filestore_get_file_size(void *user_data, const char *filepath);

bool test_filestore_read_file(void *user_data, const char *filepath, uint32_t offset, char *data,
			      const uint32_t length);

bool test_filestore_write_to_file(void *user_data, const char *filepath, uint32_t offset,
				  const uint8_t *data, const uint32_t length);

bool test_delete_file(void *user_data, const char *filepath);

bool test_filestore_dump_directory_listing(
	    void *user_data, const char *dirpath, uint8_t *listing_data, uint32_t length);

#endif
#ifndef FILESTORE_H
#define FILESTORE_H

#include "stdint.h"

struct filestore_cfg {

	void (*filestore_delete_file)(const char *filepath);

	uint64_t (*filestore_get_file_size)(const char *filepath);
	void (*filestore_read)(const char *filepath, uint32_t offset,
			       char *data, const uint32_t length);
	void (*filestore_write)(const char *filepath, uint32_t offset,
				const char *data, const uint32_t length);
	bool (*filestore_dump_directory_listing)(const char *dirpath,
						 char *listing_data,
						 uint32_t length);

	void *file_ptr;
};

#endif
#include "test_filestore.h"

uint64_t test_filestore_get_file_size(void *user_data, const char *filepath)
{
	FILE *file = fopen(filepath, "rb");
	if (file == NULL) {
		printf("Error: Could not open file %s\n", filepath);
		return 0;
	}

	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fclose(file);
	return size;
}

void test_filestore_read_file(void *user_data, const char *filepath, uint32_t offset, char *data,
			      const uint32_t length)
{
	FILE *file = fopen(filepath, "rb");
	if (file == NULL) {
		printf("Error: Could not open file %s\n", filepath);
		return;
	}

	fseek(file, offset, SEEK_SET);
	if (fread(data, sizeof(byte), length, file) != length) {
		printf("Error: Bad read\n");
		fclose(file);
		return;
	}

	fclose(file);
}

void test_filestore_write_to_file(void *user_data, const char *filepath, uint32_t offset,
				  const uint8_t *data, const uint32_t length)
{
	FILE *file = fopen(filepath, "a");
	if (file == NULL) {
		printf("Error: Could not open file %s\n", filepath);
		return;
	}

	fseek(file, offset, SEEK_SET);
	if (fwrite(data, sizeof(byte), length, file) != length) {
		printf("Error: Bad write\n");
		fclose(file);
		return;
	}

	fclose(file);
}

void test_delete_file(void *user_data, const char *filepath)
{
	if (remove(filepath) != 0) {
		printf("Error: Delete file error\n");
	}
}

bool test_filestore_dump_directory_listing(
	    void *user_data, const char *dirpath, uint8_t *listing_data, uint32_t length)
{
	DIR *dir = opendir(dirpath);
    if (!dir) {
        snprintf((char *)listing_data, length, "Error opening directory: %s\n", strerror(errno));
        return false;
    }

    struct dirent *entry;
    size_t offset = 0;

    while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 ||
		    strcmp(entry->d_name, "..") == 0)
			continue;

        int written = snprintf((char *)(listing_data + offset), length - offset, "%s\n", entry->d_name);
        if (written < 0 || (size_t)written >= length - offset) {
            // Buffer full or error
            closedir(dir);
            return false;
        }
        offset += written;
    }
	listing_data[offset] = '\0';
    closedir(dir);
    return true;
}
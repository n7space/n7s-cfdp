#include "test_filestore.h"

uint64_t test_filestore_get_file_size(const char *filepath)
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

void test_filestore_read_file(const char *filepath, uint32_t offset, char *data,
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

void test_filestore_write_to_file(const char *filepath, uint32_t offset,
				  const char *data, const uint32_t length)
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

uint32_t
test_filestore_calculate_checksum(const char *filepath,
				  const enum ChecksumType checksum_type)
{
	if (checksum_type != CHECKSUM_TYPE_MODULAR) {
		return 0;
	}

	FILE *file = fopen(filepath, "rb");
	if (file == NULL) {
		printf("Error: Could not open file %s\n", filepath);
		return 0;
	}

	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	uint32_t checksum = 0;
	uint8_t buffer[4];
	long x = 0;

	while (x < file_size) {
		size_t bytes_to_read = 4;
		if (x > file_size - 4) {
			bytes_to_read = file_size % 4;
		}

		fseek(file, x, SEEK_SET);
		size_t read_bytes = fread(buffer, 1, bytes_to_read, file);
		uint32_t value = 0;

		for (size_t i = 0; i < read_bytes; ++i) {
			value |= (uint32_t)buffer[i] << ((3 - i) * 8);
		}

		checksum += value;
		x += 4;
	}

	return checksum;
}

void test_filestore_copy_file(const char *src_path, const char *dest_path)
{
	FILE *src = fopen(src_path, "rb");
	if (!src) {
		printf("Error: Failed to open source file %s\n", src_path);
		return;
	}

	FILE *dest = fopen(dest_path, "wb");
	if (!dest) {
		printf("Error: Failed to open destination file%s\n", dest_path);
		fclose(src);
		return;
	}

	char buffer[4096];
	size_t bytes;
	while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
		if (fwrite(buffer, 1, bytes, dest) != bytes) {
			printf("Error: Write error");
			fclose(src);
			fclose(dest);
			return;
		}
	}

	fclose(src);
	fclose(dest);
}

void test_delete_file(const char *filepath)
{
	if (remove(filepath) != 0) {
		printf("Error: Delete file error\n");
	}
}

bool test_filestore_dump_directory_listing_to_file(const char *dirpath, 
		const char *dump_filepath)
{
	DIR  *dir  = NULL;
    FILE *outf = NULL;
    struct dirent *entry = NULL;
    bool status = false;

    dir = opendir(dirpath);
    if (!dir) {
		printf("Error: Failed to open a dirpath %s\n", dirpath);
    	return false;
	}

    outf = fopen(dump_filepath, "w");
    if (!outf){
		printf("Error: Failed to open a dump_filepath %s\n", dump_filepath);
    	closedir(dir);
    	return false;
	}

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".")  == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        if (fprintf(outf, "%s\n", entry->d_name) < 0) {
			printf("Error: Failed to dump files listing\n");
			fclose(outf);
            closedir(dir);
    		return false;
        }
    }


    fclose(outf);
    closedir(dir);
	return true;

}
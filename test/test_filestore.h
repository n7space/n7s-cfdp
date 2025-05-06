#ifndef TEST_FILESTORE_H
#define TEST_FILESTORE_H

uint64_t test_filestore_get_file_size(const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        printf("Error: Could not open file\n");
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    return size;
}

void test_filestore_read_file(const char *filepath, uint32_t offset, char *data, const uint32_t length) {
    FILE *file = fopen(filepath, "rb");
    if (file == NULL)
    {
        printf("Error: Could not open file\n");
        return;
    }

    fseek(file, offset, SEEK_SET);
    if (fread(data, sizeof(byte), length, file) != length)
    {
        printf("Error: Bad read\n");
        fclose(file);
        return;
    }

    fclose(file);
}

void test_filestore_write_to_file(const char *filepath, uint32_t offset, const char *data, const uint32_t length) {
    FILE *file = fopen(filepath, "wb");
    if (file == NULL)
    {
        printf("Error: Could not open file\n");
        return;
    }

    fseek(file, offset, SEEK_SET);
    if (fwrite(data, sizeof(byte), length, file) != length)
    {
        printf("Error: Bad write\n");
        fclose(file);
        return;
    }

    fclose(file);
}

uint32_t test_filestore_calculate_checksum(const char *filepath){
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        printf("Error: Could not open file\n");
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

void test_filestore_copy_file(const char *src_path, const char *dest_path) {
    FILE *src = fopen(src_path, "rb");
    if (!src) {
        printf("Error: Failed to open source file");
        return;
    }

    FILE *dest = fopen(dest_path, "wb");
    if (!dest) {
        printf("Error: Failed to open destination file");
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

#endif